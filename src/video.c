/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x8007A310 */

#include "video.h"
#include "viint.h"
#include "camera.h"
#include "game.h"
#include "main.h"
#include "printf.h"
#include "rcp.h"

/************ .data ************/

u16 *gVideoDepthBuffer = NULL;

/*******************************/

/************ .bss ************/

s32 gVideoRefreshRate; //Official Name: viFramesPerSecond
f32 gVideoAspectRatio;
f32 gVideoHeightRatio;
OSViMode gTvViMode;
s32 gVideoFbWidths[NUM_FRAMEBUFFERS];
s32 gVideoFbHeights[NUM_FRAMEBUFFERS];
u16 *gVideoFramebuffers[NUM_FRAMEBUFFERS];
s32 gVideoWriteFbIndex;
s32 gVideoReadyFbIndex;
s32 gVideoFrontFbIndex;
s32 gVideoCurrFbIndex;
s32 gVideoModeIndex;
s32 sBlackScreenTimer;
u16 *gVideoCurrFramebuffer; //Official Name: currentScreen
u16 *gVideoLastFramebuffer; //Official Name: otherScreen
u16 *gVideoCurrDepthBuffer;
u16 *gVideoLastDepthBuffer; //Official Name: otherZbuf
u8 gFrameBufferIndex = 0;
s32 gVideoHasReadyFrame;
OSScClient gVideoSched;
u8 gExpansionPak = FALSE;
u8 gUseExpansionMemory = FALSE;
u16 gScreenWidth = SCREEN_WIDTH;
u16 gScreenHeight;
s32 gVideoSkipNextRate = FALSE;
s32 gBootTimer = 8;

/******************************/

/**
 * Set up the framebuffers and the VI.
 * Framebuffers are allocated at runtime.
 * Official Name: viInit
 */
void init_video(s32 videoModeIndex) {
    s32 i;
    gVideoRefreshRate = REFRESH_60HZ;
    gVideoAspectRatio = ASPECT_RATIO_NTSC;
    gVideoHeightRatio = HEIGHT_RATIO_NTSC;

    

    gVideoSkipNextRate = TRUE;
    set_video_mode_index(videoModeIndex);
    // I run this even with an expansion pak just to use up the memory.
    // Means I don't run into any issues if I test without a pak that just happened to work with.
    if (SCREEN_WIDTH * SCREEN_HEIGHT <= 320 * 240) {
        for (i = 0; i < NUM_FRAMEBUFFERS; i++) {
            gVideoFramebuffers[i] = 0;
            init_framebuffer(i);
        }
    }
// If Enforced 4MB is on, move framebuffers to expansion memory
#if EXPANSION_PAK_SUPPORT == 0 && !defined(FORCE_4MB_MEMORY)
    if (gExpansionPak) {
        u16 *fbAddr;
        for (i = 0; i < NUM_FRAMEBUFFERS; i++) {
            gVideoFramebuffers[i] = (u16 *) (0x80500000 + (i * 0x100000));
            fbAddr = gVideoFramebuffers[i];
            // Write this as part of framebuffer emulation detection.
            fbAddr[100] = 0xBEEF;
        }
        gVideoDepthBuffer = (u16 *) 0x80400000;
        fbAddr = gVideoDepthBuffer;
        fbAddr[100] = 0xBEEF;
    }
#endif
    gVideoWriteFbIndex = 0;
    swap_framebuffers();
    init_vi_settings();
    sBlackScreenTimer = 12;
    osViBlack(TRUE);
}

/**
 * Set the current video mode to the id specified.
 */
void set_video_mode_index(s32 videoModeIndex) {
    gVideoModeIndex = videoModeIndex;
}

/**
 * Return the current framebuffer dimensions as a single s32 value.
 * The high 16 bits are the height of the frame, and the low 16 bits are the width.
 * Official Name: viGetCurrentSize
 */
s32 get_video_width_and_height_as_s32(void) {
    return (gScreenHeight << 16) | gScreenWidth;
}

OSViMode gGlobalVI;

void change_vi(OSViMode *mode, int width, int height) {
    s32 addPAL = 0;
    s32 addX = 16;
    gGlobalVI = osViModeNtscLan1;

    if (height < 240) {
        if (width == SCREEN_WIDTH_16_10) {
            addX = 20;
        } else if (width == SCREEN_WIDTH_WIDE) {
            addX = 24;
        }
        mode->comRegs.width = width;
        mode->comRegs.xScale = ((width + addX)*512)/320;
        // Y Scale
        mode->fldRegs[0].yScale = (((height + 16 - (addPAL * 2))*1024)/240);
        mode->fldRegs[1].yScale = (((height + 16 - (addPAL * 2))*1024)/240);
        // X Centre
        mode->fldRegs[0].origin = width*2;
        mode->fldRegs[1].origin = width*4;

        mode->comRegs.hStart = (428-304 + (gConfig.screenPosX * 2)) << 16 | (428+304 + (gConfig.screenPosX * 2));
        mode->fldRegs[0].vStart = (277-height + (gConfig.screenPosY * 2)) << 16 | (271+height + (gConfig.screenPosY * 2));
        mode->fldRegs[1].vStart = (277-height + (gConfig.screenPosY * 2)) << 16 | (271+height + (gConfig.screenPosY * 2));
    } else if (height == 240) {
        mode->comRegs.width = width;
        mode->comRegs.xScale = (width*512)/320;
        mode->fldRegs[0].origin = width*2;
        mode->fldRegs[1].origin = width*4;
        mode->fldRegs[0].yScale = ((height*1024)/240);
        mode->fldRegs[1].yScale = ((height*1024)/240);
    } else {
        mode->comRegs.width = width;
        mode->comRegs.xScale = (width*512)/320;
        mode->comRegs.ctrl |= 0x40;
        mode->fldRegs[0].origin = width*2;
        mode->fldRegs[1].origin = width*4;
        mode->fldRegs[0].yScale = 0x2000000|((height*1024)/240);
        mode->fldRegs[1].yScale = 0x2000000|((height*1024)/240);
        mode->fldRegs[0].vStart = mode->fldRegs[1].vStart-0x20002;
    }
    gVideoAspectRatio = ((f32) width / (f32) height);
    reset_perspective_matrix();
}

void set_dither_filter(void) {
    if (gConfig.dedither) {
        osViSetSpecialFeatures(VI_CTRL_DITHER_FILTER_ON | VI_CTRL_DIVOT_ON, VI_CTRL_GAMMA_ON);
    } else {
        osViSetSpecialFeatures(0, VI_CTRL_GAMMA_ON | VI_CTRL_DITHER_FILTER_ON | VI_CTRL_DIVOT_ON);
    }
}

/**
 * Initialise the VI settings.
 * It first checks the TV type ad then will set the properties of the VI
 * depending on the gVideoModeIndex value.
 * Most of these go unused, as the value is always 1.
 */
void init_vi_settings(void) {
	change_vi(&gGlobalVI, SCREEN_WIDTH, SCREEN_HEIGHT);
	osViSetMode(&gGlobalVI);
    gScreenWidth = SCREEN_WIDTH;
    gScreenHeight = SCREEN_HEIGHT;
    set_dither_filter();
}

/**
 * Allocate the selected framebuffer index from the main pool.
 * Will also allocate the depthbuffer if it does not already exist.
 */
void init_framebuffer(s32 index) {
    s32 width = SCREEN_WIDTH;
    u16 *fbAddr;
    if (gVideoFramebuffers[index] != 0) {
        func_80071538((u8 *) gVideoFramebuffers[index]);
        free_from_memory_pool(gVideoFramebuffers[index]);
    }
#if EXPANSION_PAK_SUPPORT
    if (gExpansionPak) {
        width = SCREEN_WIDTH_WIDE;
    }
#endif
#if EXPANSION_PAK_SUPPORT || defined(FIFO_4MB)
    if (gGfxSPTaskOutputBuffer == NULL) {
        gGfxSPTaskOutputBuffer = allocate_from_main_pool_safe(FIFO_BUFFER_SIZE, MEMP_TASKBUFFER);
    }
#endif
    gVideoFramebuffers[index] = allocate_from_main_pool_safe((width * SCREEN_HEIGHT * 2) + 0x30, MEMP_FRAMEBUFFERS);
    gVideoFramebuffers[index] = (u16 *)(((s32)gVideoFramebuffers[index] + 0x3F) & ~0x3F);
    fbAddr = gVideoFramebuffers[index];
    fbAddr[100] = 0xBEEF;
    if (gVideoDepthBuffer == NULL) {
        gVideoDepthBuffer = allocate_from_main_pool_safe((width * SCREEN_HEIGHT * 2) + 0x30, MEMP_FRAMEBUFFERS);
        gVideoDepthBuffer = (u16 *)(((s32)gVideoDepthBuffer + 0x3F) & ~0x3F);
        fbAddr = gVideoDepthBuffer;
        fbAddr[100] = 0xBEEF;
    }
}

void swap_framebuffers(void);

/**
 * Read the framebuffer and check to see if the magic number written is still there.
 * Ideally, it should've been written over, so it being there still means framebuffer emulation is off.
*/
void detect_framebuffer(void) {
    u16 *fbAddr;
    fbAddr = gVideoCurrFramebuffer;
    if (fbAddr[100] != 0xBEEF) {
        gPlatform |= FBE;
    }
    fbAddr = gVideoCurrFramebuffer;
    if (fbAddr[100] != 0xBEEF) {
        gPlatform |= DBE;
    }
    if (gPlatform & FBE) {
        puppyprint_log("Framebuffer Emulation detected.");
    } else {
        puppyprint_log("Framebuffer Emulation missing.");
    }
}

/**
 * Wait for the finished message from the scheduler while counting up a timer,
 * then update the current framebuffer index.
 * This function also has a section where it counts a timer that goes no higher
 * than an update magnitude of 2. It's only purpose is to be used as a divisor
 * in the unused function, get_video_refresh_speed.
 */
void swap_framebuffer_when_ready(void) {
    if (sBlackScreenTimer) {
        sBlackScreenTimer--;
        if (sBlackScreenTimer == 0) {
            osViBlack(FALSE);
        }
    }
    swap_framebuffers();
    if (gBootTimer) {
        gBootTimer--;
        if (gBootTimer == 0) {
            detect_framebuffer();
        }
    }
}

void swap_framebuffers(void) {        
    gVideoLastFramebuffer = gVideoFramebuffers[(gVideoCurrFbIndex + (NUM_FRAMEBUFFERS - 1)) % NUM_FRAMEBUFFERS];
    gVideoCurrFbIndex++;
    if (gVideoCurrFbIndex >= NUM_FRAMEBUFFERS) {
        gVideoCurrFbIndex = 0;
    }
    gVideoCurrFramebuffer = gVideoFramebuffers[gVideoCurrFbIndex];

    gVideoLastDepthBuffer = gVideoDepthBuffer;
    gVideoCurrDepthBuffer = gVideoDepthBuffer;

}
