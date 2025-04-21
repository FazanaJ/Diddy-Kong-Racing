#include "video.h"
#include "PRinternal/viint.h"
#include "main.h"
#include "rcp_dkr.h"

/************ .data ************/

u16 *gVideoDepthBuffer = NULL;

/*******************************/

/************ .bss ************/

s32 gVideoRefreshRate = 60;
f32 gVideoAspectRatio;
OSViMode gTvViMode;
s32 gVideoFbWidths[3];
s32 gVideoFbHeights[3];
u16 *gVideoFramebuffers[3];
s32 gVideoCurrFbIndex;
s32 gVideoModeIndex;
s32 sBlackScreenTimer;
u16 *gVideoCurrFramebuffer; // Official Name: currentScreen
u16 *gVideoLastFramebuffer; // Official Name: otherScreen
u16 *gVideoCurrDepthBuffer;
u16 *gVideoLastDepthBuffer; // Official Name: otherZbuf
u8 D_801262E4;
u8 gVideoDeltaCounter;
u8 gVideoDeltaTime;
OSScClient gVideoSched;
s32 gVideoSkipNextRate = FALSE;

/******************************/

/**
 * Set up the framebuffers and the VI.
 * Framebuffers are allocated at runtime.
 * Official Name: viInit
 */
void video_init(s32 videoModeIndex, OSSched *sc) {
    s32 i;

    video_delta_reset();
    fb_mode_set(videoModeIndex);
    for (i = 0; i < 3; i++) {
        gVideoFramebuffers[i] = NULL;
        fb_alloc(i);
    }
    gVideoCurrFbIndex = 1;
    fb_swap();
    //fb_init_vi();
    vi_change(SCREEN_WIDTH, SCREEN_HEIGHT);
    sBlackScreenTimer = 12;
    osViBlack(TRUE);
    gVideoDeltaCounter = 0;
    D_801262E4 = 3;
}

/**
 * Set the current video mode to the id specified.
 */
void fb_mode_set(s32 videoModeIndex) {
    gVideoModeIndex = videoModeIndex;
}

/**
 * Return the current framebuffer dimensions as a single s32 value.
 * The high 16 bits are the height of the frame, and the low 16 bits are the width.
 * Official Name: viGetCurrentSize
 */
s32 fb_size(void) {
    return (SCREEN_HEIGHT << 16) | SCREEN_WIDTH;
}

OSViMode gGlobalVI;

void vi_change(int width, int height) {
    s32 addPAL = 0;
    s32 addX = 0;
    s32 mul;
    OSViMode *mode = &gGlobalVI;
    if (osTvType == OS_TV_TYPE_PAL) {
        gGlobalVI = osViModePalLan1;
    } else {
        gGlobalVI = osViModeNtscLan1;
    }

    if (gConfig.screenBits == SCREENBITS_16b) {
        //gBitDepth = G_IM_SIZ_16b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000;
        mul = 2;
    } else {
        //gBitDepth = G_IM_SIZ_32b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_32 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_ANTIALIAS_MODE_3 | 0x3000;
        mul = 4;
    }

    if (height < 240) {
        /*if (width == SCREEN_WIDTH_16_10) {
            addX = 20;
        } else if (width == SCREEN_WIDTH_WIDE) {
            addX = 24;
        } else {*/
            addX = 16;
        //}
        // Y Scale
        mode->fldRegs[0].yScale = (((height + 16 - (addPAL * 2)) * 1024) / 240);
        mode->fldRegs[1].yScale = (((height + 16 - (addPAL * 2)) * 1024) / 240);

        mode->comRegs.hStart = (428 - 304 + 0) << 16 | (428 + 304 + 0);
        mode->fldRegs[0].vStart =
            (277 - height + 0) << 16 | (271 + height + 0);
        mode->fldRegs[1].vStart =
            (277 - height + 0) << 16 | (271 + height + 0);
    } else if (height == 240) {
        mode->fldRegs[0].yScale = ((height * 1024) / 240);
        mode->fldRegs[1].yScale = ((height * 1024) / 240);
    } else {
        mode->comRegs.ctrl |= 0x40;
        mode->fldRegs[0].yScale = 0x2000000 | ((height * 1024) / 240);
        mode->fldRegs[1].yScale = 0x2000000 | ((height * 1024) / 240);
        mode->fldRegs[0].vStart = mode->fldRegs[1].vStart - 0x20002;
    }
    mode->comRegs.width = width;
    mode->comRegs.xScale = ((width + addX) * 512) / 320;
    // Disable VI resampling if frame size is 320.
    if (gConfig.antiAliasing == AA_OFF) {
        if (width <= 320) {
            mode->comRegs.xScale = 0x201;
            mode->comRegs.ctrl &= ~VI_CTRL_ANTIALIAS_MODE_1;
            mode->comRegs.ctrl |= VI_CTRL_ANTIALIAS_MODE_3;
        }
    } else {
        mode->comRegs.ctrl |= VI_CTRL_DIVOT_ON;
    }
    mode->fldRegs[0].origin = width * mul;
    mode->fldRegs[1].origin = width * 4;
    gVideoAspectRatio = ((f32) width / (f32) height);
    osViSetMode(mode);
    vi_dither();
}

void vi_dither(void) {
    if (gConfig.dedither) {
        osViSetSpecialFeatures(OS_VI_DIVOT_ON);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    } else {
        osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    }
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
}

/**
 * Allocate the selected framebuffer index from the main pool.
 * Will also allocate the depthbuffer if it does not already exist.
 * Framebuffers should be 64 bit aligned, but since the memory allocator
 * already aligns by 16, it only needs 48 bits of alignment in addition.
 */
void fb_alloc(s32 index) {
    s32 width = SCREEN_WIDTH;
    s32 height = SCREEN_HEIGHT;
    s32 bitSize = 2;
    u16 *fbAddr;
    s32 fbSize;
    u8 *addr;
#if EXPANSION_PAK_SUPPORT
    if (gExpansionPak) {
        //width = SCREEN_WIDTH_WIDE;
        //height = SCREEN_HEIGHT_HIGH;
        //bitSize = 4;
    }
#endif

    fbSize = (width * height) * bitSize;
    if (fbSize & 0x3F) {
        fbSize = (s32) FBALIGN(fbSize);
    }
    switch (index) {
        case 0:
            addr = (u8 *) 0x80200000;
        break;
        case 1:
            addr = (u8 *) (0x80400000 - (fbSize));
        break;
        case 2:
            if (gUseExpansionMemory) {
                addr = (u8 *) 0x80400000;
            } else {
                addr = (u8 *) (0x80300000 - (fbSize));
            }
        break;
    }
  
#if EXPANSION_PAK_SUPPORT || defined(FIFO_4MB)
    if (gGfxSPTaskOutputBuffer == NULL) {
        gGfxSPTaskOutputBuffer = mempool_alloc_fixed(OUTPUT_BUFFER_SIZE, (u8 *) (0x80200000 + fbSize), PP_RAM_TASKBUFFER);
        gGfxSPTaskOutputBuffer = (u64 *) (((s32) gGfxSPTaskOutputBuffer + 0xF) & ~0xF);
    }
#endif

    gVideoFramebuffers[index] = mempool_alloc_fixed(fbSize, addr, PP_RAM_FRAMEBUFFERS);
    bzero(gVideoFramebuffers[index], fbSize);
    fbAddr = gVideoFramebuffers[index];
    fbAddr[100] = 0xBEEF;
    if (gVideoDepthBuffer == NULL) {
        gVideoDepthBuffer = mempool_alloc_fixed(fbSize, (u8 *) (0x80200000 - (fbSize)), PP_RAM_FRAMEBUFFERS);
        fbAddr = gVideoDepthBuffer;
        fbAddr[100] = 0xBEEF;
    }
}

/**
 * Sets the video counters to their default values.
 * Another renmant from an unused system.
 */
void video_delta_reset(void) {
    gVideoDeltaCounter = 0;
    gVideoDeltaTime = 2;
    gVideoSkipNextRate = TRUE;
}

/**
 * Wait for the finished message from the scheduler while counting up a timer,
 * then update the current framebuffer index.
 * This function also has a section where it counts a timer that goes no higher
 * than an update magnitude of 2. It's only purpose is to be used as a divisor
 * in the unused function, vi_refresh_rate.
 */
void fb_update(s32 updateRate) {
    if (sBlackScreenTimer) {
        sBlackScreenTimer -= updateRate;
        if (sBlackScreenTimer <= 0) {
            osViBlack(FALSE);
            sBlackScreenTimer = 0;
        }
    }
    osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
    fb_swap();
    /*if (gBootTimer) {
        gBootTimer--;
        if (gBootTimer == 0) {
            detect_framebuffer();
        }
    }*/
}

#define NUM_FRAMEBUFFERS 3

/**
 * Flips the current framebuffer index, swapping to the other framebuffer
 * for the next frame, then update the current and previous framebuffer pointers.
 */
void fb_swap(void) {
    gVideoLastFramebuffer = gVideoFramebuffers[(gVideoCurrFbIndex + (NUM_FRAMEBUFFERS - 1)) % NUM_FRAMEBUFFERS];
    gVideoCurrFbIndex++;
    if (gVideoCurrFbIndex >= NUM_FRAMEBUFFERS) {
        gVideoCurrFbIndex = 0;
    }
    gVideoCurrFramebuffer = gVideoFramebuffers[gVideoCurrFbIndex];

    gVideoLastDepthBuffer = gVideoDepthBuffer;
    gVideoCurrDepthBuffer = gVideoDepthBuffer;
}
