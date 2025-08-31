#include "asset_enums.h"
#include "src/printf.h"
#include "src/main.h"
#include "src/memory.h"
#include "src/rcp_dkr.h"
#include "src/video.h"
#include "src/audiomgr.h"
#include "src/camera.h"
#include "PR/os.h"
#include "PR/rcp.h"
#include "PRinternal/viint.h"

/*
 * Video Interface (VI) mode type
 */
#define OS_VI_NTSC_LPN1		0	/* NTSC */
#define OS_VI_NTSC_LPF1		1
#define OS_VI_NTSC_LAN1		2
#define OS_VI_NTSC_LAF1		3
#define OS_VI_NTSC_LPN2		4
#define OS_VI_NTSC_LPF2		5
#define OS_VI_NTSC_LAN2		6
#define OS_VI_NTSC_LAF2		7
#define OS_VI_NTSC_HPN1		8
#define OS_VI_NTSC_HPF1		9
#define OS_VI_NTSC_HAN1		10
#define OS_VI_NTSC_HAF1		11
#define OS_VI_NTSC_HPN2		12
#define OS_VI_NTSC_HPF2		13

#define OS_VI_PAL_LPN1		14	/* PAL */
#define OS_VI_PAL_LPF1		15
#define OS_VI_PAL_LAN1		16
#define OS_VI_PAL_LAF1		17
#define OS_VI_PAL_LPN2		18
#define OS_VI_PAL_LPF2		19
#define OS_VI_PAL_LAN2		20
#define OS_VI_PAL_LAF2		21
#define OS_VI_PAL_HPN1		22
#define OS_VI_PAL_HPF1		23
#define OS_VI_PAL_HAN1		24
#define OS_VI_PAL_HAF1		25
#define OS_VI_PAL_HPN2		26
#define OS_VI_PAL_HPF2		27

#define OS_VI_MPAL_LPN1		28	/* MPAL - mainly Brazil */
#define OS_VI_MPAL_LPF1		29
#define OS_VI_MPAL_LAN1		30
#define OS_VI_MPAL_LAF1		31
#define OS_VI_MPAL_LPN2		32
#define OS_VI_MPAL_LPF2		33
#define OS_VI_MPAL_LAN2		34
#define OS_VI_MPAL_LAF2		35
#define OS_VI_MPAL_HPN1		36
#define OS_VI_MPAL_HPF1		37
#define OS_VI_MPAL_HAN1		38
#define OS_VI_MPAL_HAF1		39
#define OS_VI_MPAL_HPN2		40
#define OS_VI_MPAL_HPF2		41

#define OS_VI_PAL60_LAN1    56

/*
 * Video Interface (VI) special features
 */
#define	OS_VI_GAMMA_ON			    0x0001
#define	OS_VI_GAMMA_OFF			    0x0002
#define	OS_VI_GAMMA_DITHER_ON		0x0004
#define	OS_VI_GAMMA_DITHER_OFF		0x0008
#define	OS_VI_DIVOT_ON			    0x0010
#define	OS_VI_DIVOT_OFF			    0x0020
#define	OS_VI_DITHER_FILTER_ON		0x0040
#define	OS_VI_DITHER_FILTER_OFF		0x0080

/*
 * Video Interface (VI) mode attribute bit
 */
#define OS_VI_BIT_NONINTERLACE	    0x0001          /* lo-res */
#define OS_VI_BIT_INTERLACE		    0x0002          /* lo-res */
#define OS_VI_BIT_NORMALINTERLACE	0x0004          /* hi-res */
#define OS_VI_BIT_DEFLICKINTERLACE	0x0008          /* hi-res */
#define OS_VI_BIT_ANTIALIAS		    0x0010
#define OS_VI_BIT_POINTSAMPLE	    0x0020
#define OS_VI_BIT_16PIXEL		    0x0040
#define OS_VI_BIT_32PIXEL		    0x0080
#define OS_VI_BIT_LORES			    0x0100
#define OS_VI_BIT_HIRES			    0x0200
#define OS_VI_BIT_NTSC			    0x0400
#define OS_VI_BIT_PAL			    0x0800

OSViMode osViModePalLan1 = {
    OS_VI_PAL_LAN1,  // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON |
            VI_CTRL_DIVOT_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000,  // ctrl
        WIDTH(320),                                                // width
        BURST(58, 35, 4, 64),                                      // burst
        VSYNC(625),                                                // vSync
        HSYNC(3177, 21),                                           // hSync
        LEAP(3183, 3182),                                          // leap
        HSTART(128, 768),                                          // hStart
        SCALE(2, 0),                                               // xScale
        VCURRENT(0),                                               // vCurrent
    },
    {// fldRegs
     {
         //[0]
         ORIGIN(640),          // origin
         SCALE(1, 0),          // yScale
         HSTART(95, 569),      // vStart
         BURST(107, 2, 9, 0),  // vBurst
         VINTR(2),             // vIntr
     },
     {
         //[1]
         ORIGIN(640),          // origin
         SCALE(1, 0),          // yScale
         HSTART(95, 569),      // vStart
         BURST(107, 2, 9, 0),  // vBurst
         VINTR(2),             // vIntr
     }}};

OSViMode osViModeMpalLan1 = {
    OS_VI_MPAL_LAN1,  // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON |
            VI_CTRL_DIVOT_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000,  // ctrl
        WIDTH(320),                                                // width
        BURST(57, 30, 5, 70),                                      // burst
        VSYNC(525),                                                // vSync
        HSYNC(3089, 4),                                            // hSync
        LEAP(3097, 3098),                                          // leap
        HSTART(108, 748),                                          // hStart
        SCALE(2, 0),                                               // xScale
        VCURRENT(0),                                               // vCurrent
    },
    {// fldRegs
     {
         //[0]
         ORIGIN(640),         // origin
         SCALE(1, 0),         // yScale
         HSTART(37, 511),     // vStart
         BURST(4, 2, 14, 0),  // vBurst
         VINTR(2),            // vIntr
     },
     {
         //[1]
         ORIGIN(640),         // origin
         SCALE(1, 0),         // yScale
         HSTART(37, 511),     // vStart
         BURST(4, 2, 14, 0),  // vBurst
         VINTR(2),            // vIntr
     }}};

OSViMode osViModePal60Lan1 = {
    OS_VI_PAL60_LAN1, // type
    {
        // comRegs
        VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_DIVOT_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000, // ctrl
        WIDTH(320),               // width
        BURST(58, 30, 4, 69),     // burst
        // Ideally VSYNC would be 525 but this produces marginally-too-slow retraces. 519 is the closest value that
        // produces an ~16.6ms retrace (specifically about 16.70ms). We expect 519 to be OK compatibility-wise, it's
        // within 1.5% of the nominal value.
        VSYNC(519),               // vSync
        // We could also have modified HSYNC to correct the retrace timings, however in general HSYNC is more sensitive
        // than VSYNC so we choose to leave HSYNC at the nominal value.
        HSYNC(3177, 23),          // hSync
        LEAP(3183, 3181),         // leap
        HSTART(128, 768),         // hStart
        SCALE(2, 0),              // xScale
        VCURRENT(0),              // vCurrent
    },
    { // fldRegs
      {
          // [0]
          ORIGIN(640),         // origin
          SCALE(1, 0),         // yScale
          START(37, 511),      // vStart
          BURST(107, 2, 9, 0), // vBurst
          VINTR(2),            // vIntr
      },
      {
          // [1]
          ORIGIN(640),         // origin
          SCALE(1, 0),         // yScale
          START(37, 511),      // vStart
          BURST(107, 2, 9, 0), // vBurst
          VINTR(2),            // vIntr
      } },
};

extern OSViMode gGlobalVI;
extern s32 osViClock;

/*void vi_dither(void) {
    if (gConfig.dedither) {
        osViSetSpecialFeatures(OS_VI_DIVOT_ON);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    } else {
        osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    }
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
}*/

void vi_reschange(int width, int height) {
    s32 addPAL = 0;
    s32 addX = 0;
    s32 mul;
    f32 tempWidth;
    s32 posX;
    s32 posY;
    static u16 prevWidth = 0;
    static u16 prevHeight = 0;
    static u8 prevBits = 0;
    OSViMode *mode = &gGlobalVI;
    OSViMode *base;

    if (gConfig.screenRegion == REGIONMODE_PAL50) {
        base = &osViModePalLan1;
        osViClock = VI_PAL_CLOCK;
    } else if (gConfig.screenRegion == REGIONMODE_MPAL) {
        base = &osViModeMpalLan1;
        osViClock = VI_MPAL_CLOCK;
    } else if (gConfig.screenRegion == REGIONMODE_PAL60) {
        base = &osViModePal60Lan1;
        osViClock = VI_PAL_CLOCK;
    } else {
        base = &osViModeNtscLan1;
        osViClock = VI_NTSC_CLOCK;
    }

    wcopy(base, &gGlobalVI, sizeof(OSViMode));
    osAiSetFrequency(OUTPUT_RATE);

    if (gConfig.screenBits == SCREENBITS_16b) {
        gBitDepth = G_IM_SIZ_16b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_16 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_ANTIALIAS_MODE_1 | 0x3000;
        mul = 2;
    } else {
        gBitDepth = G_IM_SIZ_32b;
        mode->comRegs.ctrl = VI_CTRL_TYPE_32 | VI_CTRL_GAMMA_DITHER_ON | VI_CTRL_GAMMA_ON | VI_CTRL_ANTIALIAS_MODE_3 | 0x3000;
        mul = 4;
    }

    if (height < 240) {
        if (width == SCREEN_WIDTH_16_10) {
            addX = 20;
        } else if (width == SCREEN_WIDTH_WIDE) {
            addX = 24;
        } else {
            addX = 16;
        }
        // Y Scale
        posX = gConfig.screenPosX * 2;
        posY = gConfig.screenPosY * 2;
        mode->fldRegs[0].yScale = (((height + 16 - (addPAL * 2)) * 1024) / 240);
        mode->fldRegs[1].yScale = (((height + 16 - (addPAL * 2)) * 1024) / 240);

        mode->comRegs.hStart = (428 - 304 + posX) << 16 | (428 + 304 + posX);
        mode->fldRegs[0].vStart =
            (277 - height + posY) << 16 | (271 + height + posY);
        mode->fldRegs[1].vStart =
            (277 - height + posY) << 16 | (271 + height + posY);
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
    if (gConfig.screenWidth == RESOLUTION_384x240) {
        tempWidth = 384;
    } else if (gConfig.screenWidth == RESOLUTION_424x240) {
        tempWidth = 424;
    } else {
        tempWidth = 320;
    }
    gVideoAspectRatio = ((f32) tempWidth / (f32) height);
    cam_persp_init();
    osViSetMode(mode);
    if (gConfig.dedither) {
        osViSetSpecialFeatures(OS_VI_DIVOT_ON);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    } else {
        osViSetSpecialFeatures(OS_VI_DIVOT_OFF);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_OFF);
    }
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);

    if (width != prevWidth || height != prevHeight || prevBits != gBitDepth) {
        prevWidth = width;
        prevHeight = height;
        prevBits = gBitDepth;
        osViBlack(TRUE);
        sBlackScreenTimer = 10;
    }
}

/**
 * Allocate the selected framebuffer index from the main pool.
 * Will also allocate the depthbuffer if it does not already exist.
 * Framebuffers should be 64 bit aligned, but since the memory allocator
 * already aligns by 16, it only needs 48 bits of alignment in addition.
 */
void reschange_alloc(s32 index) {
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