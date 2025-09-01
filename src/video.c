#include "video.h"
#include "memory.h"
#include "PRinternal/viint.h"
#include "main.h"
#include "rcp_dkr.h"
#include "camera.h"
#include "types.h"
#include "audiomgr.h"

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
s32 sBlackScreenTimer;
u16 *gVideoCurrFramebuffer; // Official Name: currentScreen
u16 *gVideoLastFramebuffer; // Official Name: otherScreen
u16 *gVideoCurrDepthBuffer;
u16 *gVideoLastDepthBuffer; // Official Name: otherZbuf
u8 gVideoDeltaCounter;
u8 gVideoDeltaTime;
OSScClient gVideoSched;
s32 gVideoSkipNextRate = FALSE;
u8 gBitDepth = G_IM_SIZ_16b;
OSViMode gGlobalVI;

/******************************/

/**
 * Set up the framebuffers and the VI.
 * Framebuffers are allocated at runtime.
 * Official Name: viInit
 */
void video_init(void) {
    video_delta_reset();
    gVideoCurrFbIndex = 1;
    fb_swap();
}

void video_alloc(void) {
    s32 i;
    //void (*func)(s32 index);
    wcopy(&osViModeNtscLan1, &gGlobalVI, sizeof(OSViMode));

    //overlay_load(OVERLAY_RESCHANGE);

    //func = overlay_symbol(OVERLAY_RESCHANGE, "reschange_alloc");

    for (i = 0; i < 3; i++) {
        gVideoFramebuffers[i] = NULL;
        //(*func)(i);
        fb_alloc(i);
    }

    //overlay_free(OVERLAY_RESCHANGE);
}

/**
 * Return the current framebuffer dimensions as a single s32 value.
 * The high 16 bits are the height of the frame, and the low 16 bits are the width.
 * Official Name: viGetCurrentSize
 */
s32 fb_size(void) {
    return (SCREEN_HEIGHT << 16) | SCREEN_WIDTH;
}

void vi_change(int width, int height) {
    void (*func)(s32 width, s32 height);

    overlay_load(OVERLAY_RESCHANGE);
    func = overlay_symbol(OVERLAY_RESCHANGE, "vi_reschange");
    (*func)(width, height);
    overlay_free(OVERLAY_RESCHANGE);
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
    s32 bitSize;
    u16 *fbAddr;
    s32 fbSize;
    u8 *addr;
#if EXPANSION_PAK_SUPPORT
    if (gExpansionPak) {
        width = 480;
        height = 360;
        bitSize = 4;
    } else {
        if (width > 320) {
            width = 320;
        }
        if (height > 240) {
            height = 240;
        }
        bitSize = 2;
    }
#else
    if (width > 320) {
        width = 320;
    }
    if (height > 240) {
        height = 240;
    }
    bitSize = 2;
#endif

    fbSize = (width * height) * bitSize;
    if (fbSize & 0x3F) {
        fbSize = (s32) FBALIGN(fbSize);
    }
    switch (index) {
        default:
            addr = (u8 *) (0x80100000 - (fbSize));
        break;
        case 1:
            addr = (u8 *) 0x80100000;
        break;
        case 2:
            addr = (u8 *) (0x80300000 - (fbSize));
        break;
    }
  
#if EXPANSION_PAK_SUPPORT || defined(FIFO_4MB)
    if (gGfxSPTaskOutputBuffer == NULL) {
        gGfxSPTaskOutputBuffer = mempool_alloc_fixed(POOL_MAIN, OUTPUT_BUFFER_SIZE, (u8 *) (0x80100000 + fbSize), PP_RAM_TASKBUFFER, TRUE);
        gGfxSPTaskOutputBuffer = (u64 *) (((s32) gGfxSPTaskOutputBuffer + 0xF) & ~0xF);
    }
#endif

    gVideoFramebuffers[index] = mempool_alloc_fixed(POOL_MAIN, fbSize, addr, PP_RAM_FRAMEBUFFERS, TRUE);
    bzero(gVideoFramebuffers[index], fbSize);
    fbAddr = gVideoFramebuffers[index];
    fbAddr[100] = 0xBEEF;
    if (gVideoDepthBuffer == NULL) {
        gVideoDepthBuffer = mempool_alloc_fixed(POOL_MAIN, fbSize, (u8 *) 0x80300000, PP_RAM_FRAMEBUFFERS, TRUE);
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
