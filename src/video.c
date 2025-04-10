#include "video.h"
#include "PRinternal/viint.h"

/************ .data ************/

u16 *gVideoDepthBuffer = NULL;
UNUSED s32 D_800DE774 = 0;
UNUSED s8 D_800DE778 = 2;

VideoModeResolution gVideoModeResolutions[] = {
    { SCREEN_WIDTH, SCREEN_HEIGHT },                   // 320x240
    { SCREEN_WIDTH, SCREEN_HEIGHT },                   // 320x240
    { HIGH_RES_SCREEN_WIDTH, SCREEN_HEIGHT },          // 640x240
    { HIGH_RES_SCREEN_WIDTH, SCREEN_HEIGHT },          // 640x240
    { HIGH_RES_SCREEN_WIDTH, HIGH_RES_SCREEN_HEIGHT }, // 640x480
    { HIGH_RES_SCREEN_WIDTH, HIGH_RES_SCREEN_HEIGHT }, // 640x480
    { HIGH_RES_SCREEN_WIDTH, HIGH_RES_SCREEN_HEIGHT }, // 640x480
    { HIGH_RES_SCREEN_WIDTH, HIGH_RES_SCREEN_HEIGHT }, // 640x480
};

// This value exists in order to make sure there are no out of bounds accesses of gVideoModeResolutions
#define NUM_RESOLUTION_MODES ((s32) (sizeof(gVideoModeResolutions) / sizeof(VideoModeResolution)) - 1)

/*******************************/

/************ .bss ************/

s32 gVideoRefreshRate; // Official Name: viFramesPerSecond
f32 gVideoAspectRatio;
f32 gVideoHeightRatio;
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
UNUSED OSMesg D_801262E8[8];
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
    if (osTvType == OS_TV_TYPE_PAL) {
        gVideoRefreshRate = REFRESH_50HZ;
        gVideoAspectRatio = ASPECT_RATIO_PAL;
        gVideoHeightRatio = HEIGHT_RATIO_PAL;
    } else if (osTvType == OS_TV_TYPE_MPAL) {
        gVideoRefreshRate = REFRESH_60HZ;
        gVideoAspectRatio = ASPECT_RATIO_MPAL;
        gVideoHeightRatio = HEIGHT_RATIO_MPAL;
    } else {
        gVideoRefreshRate = REFRESH_60HZ;
        gVideoAspectRatio = ASPECT_RATIO_NTSC;
        gVideoHeightRatio = HEIGHT_RATIO_NTSC;
    }

    if (osTvType == OS_TV_TYPE_PAL) {
        for (i = 0; i <= NUM_RESOLUTION_MODES; i++) {
            gVideoModeResolutions[i].height += PAL_HEIGHT_DIFFERENCE;
        }
    }

    video_delta_reset();
    fb_mode_set(videoModeIndex);
    for (i = 0; i < 3; i++) {
        gVideoFramebuffers[0];
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
 * Unused function that would return the current video mode index.
 */
UNUSED s32 fb_mode(void) {
    return gVideoModeIndex;
}

/**
 * Unused function that would change the framebuffer dimensions.
 * Since only one kind of video mode is ever used, this function is never called.
 */
UNUSED void fb_mode_size(s32 fbIndex) {
    gVideoFbWidths[fbIndex] = gVideoModeResolutions[gVideoModeIndex & NUM_RESOLUTION_MODES].width;
    gVideoFbHeights[fbIndex] = gVideoModeResolutions[gVideoModeIndex & NUM_RESOLUTION_MODES].height;
}

/**
 * Return the current framebuffer dimensions as a single s32 value.
 * The high 16 bits are the height of the frame, and the low 16 bits are the width.
 * Official Name: viGetCurrentSize
 */
s32 fb_size(void) {
    return (gVideoFbHeights[gVideoCurrFbIndex] << 16) | gVideoFbWidths[gVideoCurrFbIndex];
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

    /*if (gConfig.screenQuality) {
        gBitDepth = G_IM_SIZ_32b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_32 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_DIVOT_ON | VI_CTRL_ANTIALIAS_MODE_3 | 0x3000;
        mul = 4;
    } else {*/
        //gBitDepth = G_IM_SIZ_16b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_DIVOT_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000;
        mul = 2;
    //}

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
    if (width <= 320 && 1) {
        mode->comRegs.xScale = 0x201;
        mode->comRegs.ctrl &= ~VI_CTRL_ANTIALIAS_MODE_1;
        mode->comRegs.ctrl |= VI_CTRL_ANTIALIAS_MODE_3;
    }
    mode->fldRegs[0].origin = width * mul;
    mode->fldRegs[1].origin = width * 4;
    gVideoAspectRatio = ((f32) width / (f32) height);
    osViSetMode(mode);
    osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
}

/**
 * Initialise the VI settings.
 * It first checks the TV type ad then will set the properties of the VI
 * depending on the gVideoModeIndex value.
 * Most of these go unused, as the value is always 1.
 */
void fb_init_vi(void) {
    s32 viModeTableIndex;
    OSViMode *tvViMode;

    viModeTableIndex = OS_VI_NTSC_LPN1;
    if (osTvType == OS_TV_TYPE_PAL) {
        viModeTableIndex = OS_VI_PAL_LPN1;
    } else if (osTvType == OS_TV_TYPE_MPAL) {
        viModeTableIndex = OS_VI_MPAL_LPN1;
    }

    switch (gVideoModeIndex & NUM_RESOLUTION_MODES) {
        case VIDEO_MODE_LOWRES_LAN:
            stubbed_printf("320 by 240 Point sampled, Non interlaced.\n");
            osViSetMode(&osViModeTable[viModeTableIndex]);
            break;
        case VIDEO_MODE_LOWRES_LPN:
            //!@bug: The video mode being set here is Point sampled
            // but the printf implies it was intended to be Anti-aliased.
            // By my understanding, this is the case we will always hit in code,
            // So maybe it was swapped out late in development?
            stubbed_printf("320 by 240 Anti-aliased, Non interlaced.\n");
            tvViMode = &osViModeNtscLpn1;
            if (osTvType == OS_TV_TYPE_PAL) {
                tvViMode = &osViModePalLpn1;
            } else if (osTvType == OS_TV_TYPE_MPAL) {
                tvViMode = &osViModeMpalLpn1;
            }
            fb_memcpy((u8 *) tvViMode, (u8 *) &gTvViMode, sizeof(OSViMode));
            if (osTvType == OS_TV_TYPE_PAL) {
                // A simple osViExtendVStart to add an additional 24 scanlines?
                gTvViMode.fldRegs[0].vStart -= (PAL_HEIGHT_DIFFERENCE << 16);
                gTvViMode.fldRegs[1].vStart -= (PAL_HEIGHT_DIFFERENCE << 16);
                gTvViMode.fldRegs[0].vStart += PAL_HEIGHT_DIFFERENCE;
                gTvViMode.fldRegs[1].vStart += PAL_HEIGHT_DIFFERENCE;
            }
            osViSetMode(&gTvViMode);
            break;
        case VIDEO_MODE_MEDRES_LPN:
            stubbed_printf("640 by 240 Point sampled, Non interlaced.\n");
            tvViMode = &osViModeNtscLpn1;
            if (osTvType == OS_TV_TYPE_PAL) {
                tvViMode = &osViModePalLpn1;
            } else if (osTvType == OS_TV_TYPE_MPAL) {
                tvViMode = &osViModeMpalLpn1;
            }

            fb_memcpy((u8 *) tvViMode, (u8 *) &gTvViMode, sizeof(OSViMode));
            gTvViMode.comRegs.width = WIDTH(HIGH_RES_SCREEN_WIDTH);
            gTvViMode.comRegs.xScale = SCALE(1, 0);
            gTvViMode.fldRegs[0].origin = ORIGIN(HIGH_RES_SCREEN_WIDTH * 2);
            gTvViMode.fldRegs[1].origin = ORIGIN(HIGH_RES_SCREEN_WIDTH * 2);
            osViSetMode(&gTvViMode);
            break;
        case VIDEO_MODE_MEDRES_LAN:
            stubbed_printf("640 by 240 Anti-aliased, Non interlaced.\n");
            tvViMode = &osViModeNtscLan1;
            if (osTvType == OS_TV_TYPE_PAL) {
                tvViMode = &osViModePalLan1;
            } else if (osTvType == OS_TV_TYPE_MPAL) {
                tvViMode = &osViModeMpalLan1;
            }
            fb_memcpy((u8 *) tvViMode, (u8 *) &gTvViMode, sizeof(OSViMode));
            gTvViMode.comRegs.width = WIDTH(HIGH_RES_SCREEN_WIDTH);
            gTvViMode.comRegs.xScale = SCALE(1, 0);
            gTvViMode.fldRegs[0].origin = ORIGIN(HIGH_RES_SCREEN_WIDTH * 2);
            gTvViMode.fldRegs[1].origin = ORIGIN(HIGH_RES_SCREEN_WIDTH * 2);
            osViSetMode(&gTvViMode);
            break;
        case VIDEO_MODE_HIGHRES_HPN:
            stubbed_printf("640 by 480 Point sampled, Interlaced.\n");
            osViSetMode(&osViModeTable[viModeTableIndex + OS_VI_NTSC_HPN1]);
            break;
        case VIDEO_MODE_HIGHRES_HAN:
            stubbed_printf("640 by 480 Anti-aliased, Interlaced.\n");
            osViSetMode(&osViModeTable[viModeTableIndex + OS_VI_NTSC_HAN1]);
            break;
        case VIDEO_MODE_HIGHRES_HPF:
            stubbed_printf("640 by 480 Point sampled, Interlaced, De-flickered.\n");
            osViSetMode(&osViModeTable[viModeTableIndex + OS_VI_NTSC_HPF1]);
            break;
        case VIDEO_MODE_HIGHRES_HAF:
            stubbed_printf("640 by 480 Anti-aliased, Interlaced, De-flickered.\n");
            osViSetMode(&osViModeTable[viModeTableIndex + OS_VI_NTSC_HAF1]);
            break;
    }
    osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
}

/**
 * Allocate the selected framebuffer index from the main pool.
 * Will also allocate the depthbuffer if it does not already exist.
 * Framebuffers should be 64 bit aligned, but since the memory allocator
 * already aligns by 16, it only needs 48 bits of alignment in addition.
 */
void fb_alloc(s32 index) {
    if (gVideoFramebuffers[index] != 0) {
        mempool_locked_unset((u8 *) gVideoFramebuffers[index]); // Effectively unused.
        mempool_free(gVideoFramebuffers[index]);
    }
    gVideoFbWidths[index] = gVideoModeResolutions[gVideoModeIndex & NUM_RESOLUTION_MODES].width;
    gVideoFbHeights[index] = gVideoModeResolutions[gVideoModeIndex & NUM_RESOLUTION_MODES].height;
    if (gVideoModeIndex >= VIDEO_MODE_MIDRES_MASK) {
        gVideoFramebuffers[index] =
            mempool_alloc_safe((HIGH_RES_SCREEN_WIDTH * HIGH_RES_SCREEN_HEIGHT * 2) + 0x30, COLOUR_TAG_WHITE);
        gVideoFramebuffers[index] = FBALIGN(gVideoFramebuffers[index]);
        if (gVideoDepthBuffer == NULL) {
            gVideoDepthBuffer =
                mempool_alloc_safe((HIGH_RES_SCREEN_WIDTH * HIGH_RES_SCREEN_HEIGHT * 2) + 0x30, COLOUR_TAG_WHITE);
            gVideoDepthBuffer = FBALIGN(gVideoDepthBuffer);
        }
    } else {
        gVideoFramebuffers[index] =
            mempool_alloc_safe((gVideoFbWidths[index] * gVideoFbHeights[index] * 2) + 0x30, COLOUR_TAG_WHITE);
        gVideoFramebuffers[index] = FBALIGN(gVideoFramebuffers[index]);
        if (gVideoDepthBuffer == NULL) {
            gVideoDepthBuffer =
                mempool_alloc_safe((gVideoFbWidths[index] * gVideoFbHeights[index] * 2) + 0x30, COLOUR_TAG_WHITE);
            gVideoDepthBuffer = FBALIGN(gVideoDepthBuffer);
        }
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

void func_8007AB24(u8 arg0) {
    D_801262E4 = arg0;
}

/**
 * Unused function that returns the refresh rate, after performance.
 * A fully performant game would return 60.
 * Perhaps may have been used originally to calculate the factor in which to handle frameskipping with.
 */
UNUSED s32 vi_refresh_rate(void) {
    return (s32) ((f32) gVideoRefreshRate / (f32) gVideoDeltaTime);
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

/**
 * Copy byte-by-byte a region from one address to another.
 */
void fb_memcpy(u8 *src, u8 *dest, s32 len) {
    s32 i;

    for (i = 0; i < len; i++) {
        *dest++ = *src++;
    }
}
