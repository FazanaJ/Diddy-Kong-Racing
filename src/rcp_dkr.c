#include "rcp_dkr.h"
#include "types.h"
#include "structs.h"
#include "macros.h"
#include "video.h"
#include "camera.h"
#include "set_rsp_segment.h"
#include "racer.h"
#include "thread3_main.h"
#include "main.h"
#include "textures_sprites.h"
#include "memory.h"
#include "menu.h"
#include "game.h"

/************ .data ************/

s16 gGfxTaskMesgNums[16] = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

u8 sBGPrimColourR = 0;
u8 sBGPrimColourG = 0;
u8 sBGPrimColourB = 0;
s32 sBackgroundFillColour = GPACK_RGBA5551(0, 0, 0, 1) | (GPACK_RGBA5551(0, 0, 0, 1) << 16);

u32 gTexBGShiftX = 64;
TextureHeader *gTexBGTex1 = NULL;
TextureHeader *gTexBGTex2 = NULL;

BackgroundFunction gBGDrawFunc = NULL;
s32 gGfxBufCounter = 0;
s32 gGfxTaskIsRunning = FALSE;


Gfx dScaledRectangleBaseModes[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_FOG),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

Gfx dRaceFinishBackgroundSettings[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_FOG),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsDPSetOtherMode(DKR_OMH_1CYC_POINT_NOPERSP, DKR_OML_COMMON | G_RM_OPA_SURF | G_RM_OPA_SURF2),
    gsSPEndDisplayList(),
};

Gfx dTextureRectangleModes[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_FOG),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
    gsDPSetOtherMode(DKR_OMH_1CYC_POINT_NOPERSP, DKR_OML_COMMON | G_RM_XLU_SURF | G_RM_XLU_SURF2),
    gsSPEndDisplayList(),
};


Gfx dTextureRectangleScaledOpa[][2] = {
    // Bilinear Filtered texture
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_AA_OPA_SURF,
                     G_RM_AA_OPA_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_OPA_SURF,
                     G_RM_OPA_SURF2),
    // Point Sampled texture
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT_NOPERSP, G_RM_AA_OPA_SURF,
                     G_RM_AA_OPA_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT_NOPERSP, G_RM_OPA_SURF,
                     G_RM_OPA_SURF2)
};

Gfx dTextureRectangleScaledXlu[][2] = {
    // Bilinear Filtered texture
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_AA_XLU_SURF,
                     G_RM_AA_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_XLU_SURF,
                     G_RM_XLU_SURF2),
    // Point Sampled texture
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT_NOPERSP, G_RM_AA_XLU_SURF,
                     G_RM_AA_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT_NOPERSP, G_RM_XLU_SURF,
                     G_RM_XLU_SURF2)
};

/*******************************/

/************ .bss ************/

u8 gDramStack[SP_DRAM_STACK_SIZE8];
u64 *gGfxSPTaskOutputBuffer;
OSMesgQueue gRCPMesgQueue;
OSMesg gRCPMesgBuf;
OSMesgQueue gGfxTaskMesgQueue;
OSMesg gGfxTaskMesgBuf[8];
u8 gInvertBG;
DKR_OSTask gGfxTaskBuf[2];
OSMesgQueue *osScInterruptQ;

/*******************************/

extern OSMesgQueue gGameMesgQueue;
extern OSSched gMainSched;
extern s32 gNumGfxTasksAtScheduler;

DKR_OSTask gRdpTaskA = {
    NULL,                                                // next
    0,                                                   // state
    OS_SC_LAST_TASK | OS_SC_NEEDS_RDP | OS_SC_NEEDS_RSP, // flags
    NULL,                                                // frameBuffer
    {
        // task
        M_GFXTASK,                      // type
        OS_TASK_DP_WAIT,                // flags
        (u64 *) rspF3DDKRBootStart,     // ucode_boot
        0,                              // ucode_boot_size
        (u64 *) rspF3DDKRXbusStart,     // ucode
        0,                              // ucode_size
        (u64 *) rspF3DDKRDataXbusStart, // ucode_data
        SP_UCODE_DATA_SIZE,             // ucode_data_size
        (u64 *) gDramStack,             // dram_stack
        SP_DRAM_STACK_SIZE8,            // dram_stack_size
        NULL,                           // output_buff
        0,                              // output_buff_size
        0,                              // data_ptr
        0,                              // data_size
        (u64 *) gGfxTaskYieldData,      // yield_data_ptr
        sizeof(gGfxTaskYieldData),      // yield_data_size
    },
    &gGameMesgQueue, // mesgQueue
    NULL,            // mesg
};

DKR_OSTask gRdpTaskB = {
    NULL,                                                // next
    0,                                                   // state
    OS_SC_LAST_TASK | OS_SC_NEEDS_RDP | OS_SC_NEEDS_RSP, // flags
    NULL,                                                // frameBuffer
    {
        // task
        M_GFXTASK,                      // type
        OS_TASK_DP_WAIT,                // flags
        (u64 *) rspF3DDKRBootStart,     // ucode_boot
        0,                              // ucode_boot_size
        (u64 *) rspF3DDKRXbusStart,     // ucode
        0,                              // ucode_size
        (u64 *) rspF3DDKRDataXbusStart, // ucode_data
        SP_UCODE_DATA_SIZE,             // ucode_data_size
        (u64 *) gDramStack,             // dram_stack
        SP_DRAM_STACK_SIZE8,            // dram_stack_size
        NULL,                           // output_buff
        0,                              // output_buff_size
        0,                              // data_ptr
        0,                              // data_size
        (u64 *) gGfxTaskYieldData,      // yield_data_ptr
        sizeof(gGfxTaskYieldData),      // yield_data_size
    },
    &gGameMesgQueue, // mesgQueue
    NULL,            // mesg
};

DKR_OSTask *gRdpCurTask = &gRdpTaskA;

/*******************************/

/**
 * Prepare the gfx task for the F3DDKR XBus microcode.
 * Sends a message to the scheduler to start processing an RSP task once set up.
 * Official Name: rcpFast3d
 */
void gfxtask_run_xbus(Gfx *dlBegin, Gfx *dlEnd) {
    DKR_OSTask *dkrtask;

    dkrtask = gRdpCurTask;
    dkrtask->task.data_ptr = (u64 *) dlBegin;
    dkrtask->task.data_size = ((s32) dlEnd - (s32) dlBegin) >> 3; // Shifted by 3, repsenting the size of the Gfx type.
    dkrtask->task.ucode_boot_size = (s32) (rspF3DDKRDramStart - rspF3DDKRBootStart);
    dkrtask->frameBuffer = gVideoCurrFramebuffer;

    osScSubmitTask(&gMainSched, (void *) dkrtask);
    gRdpCurTask = (DKR_OSTask *) ((u32) gRdpCurTask ^ (u32) &gRdpTaskA ^ (u32) &gRdpTaskB);
}

/**
 *
 * Prepare the gfx task for the F3DDKR FIFO microcode.
 * Sends a message to the scheduler to start processing an RSP task once set up.
 * Goes unused, and is broken.
 */
void gfxtask_run_fifo(Gfx *dlBegin, Gfx *dlEnd) {
    DKR_OSTask *dkrtask;
    u64 *taskStart;
    u64 *taskEnd;

#if EXPANSION_PAK_SUPPORT || defined(FIFO_4MB)
    taskStart = (u64 *) gGfxSPTaskOutputBuffer;
    taskEnd = (u64 *) ((u8 *) gGfxSPTaskOutputBuffer + OUTPUT_BUFFER_SIZE);
#else
    taskStart = (u64 *) 0x80680000;
    taskEnd = (u64 *) 0x806E0000;
#endif

    dkrtask = gRdpCurTask;
    dkrtask->task.ucode = (u64 *) rspF3DDKRFifoStart;
    dkrtask->task.ucode_data = (u64 *) rspF3DDKRDataFifoStart;
    dkrtask->task.data_ptr = (u64 *) dlBegin;
    dkrtask->task.data_size = ((s32) dlEnd - (s32) dlBegin) >> 3; // Shifted by 3, repsenting the size of the Gfx type.
    dkrtask->task.ucode_boot_size = (s32) (rspF3DDKRDramStart - rspF3DDKRBootStart);
    dkrtask->task.output_buff = (u64 *) taskStart;
    dkrtask->task.output_buff_size = (u64 *) taskEnd;
    dkrtask->frameBuffer = gVideoCurrFramebuffer;

    osScSubmitTask(&gMainSched, (void *) dkrtask);
    gRdpCurTask = (DKR_OSTask *) ((u32) gRdpCurTask ^ (u32) &gRdpTaskA ^ (u32) &gRdpTaskB);
}

/**
 * Called from the main game loop, will halt until a message comes through saying the graphics task
 * has finished.
 * Alternatively, if no task is active, then it will just skip.
 * Official Name: rcpWaitDP
 */
s32 gfxtask_wait(void) {
    OSMesg mesg;

    while (gNumGfxTasksAtScheduler > 0) {
        osRecvMesg(&gGameMesgQueue, &mesg, OS_MESG_BLOCK);

        if ((s32) mesg == OS_SC_DONE_MSG) {
            gNumGfxTasksAtScheduler--;
        }
    }
    return 0;
}

/**
 * Checks the current map ID and returns true if the fill background needs to be rendered.
 * Ideally, you want the level to not.
*/
s32 bgdraw_init(void) {
    s32 map = get_current_map_id();

    if (gCurrentMenuId == MENU_CREDITS || gCurrentMenuId == MENU_LOGOS) {
        gInvertBG = TRUE;
    } else {
        gInvertBG = FALSE;
    }

    switch (map) {
        case ASSET_LEVEL_CENTRALAREAHUB:
        case ASSET_LEVEL_WHALEBAY:
        case ASSET_LEVEL_PIRATELAGOON:
        case ASSET_LEVEL_DINODOMAINHUB:
        case ASSET_LEVEL_DINODOMAINTROPHYANIM:
        //case ASSET_LEVEL_FRONTEND:
        case ASSET_LEVEL_WIZPIG2:
        case ASSET_LEVEL_SNOWBALLVALLEY:
        case ASSET_LEVEL_DARKWATERBEACH:
        case ASSET_LEVEL_TITLESCREENSEQUENCE:
        case ASSET_LEVEL_BLUEY1:
        case ASSET_LEVEL_BLUEY2:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
    }
}

/**
 * Sets the primitive colour for the cyclemode fillrect background.
 * Official name: rcpSetScreenColour
 */
void bgdraw_primcolour(u8 red, u8 green, u8 blue) {
    sBGPrimColourR = red;
    sBGPrimColourG = green;
    sBGPrimColourB = blue;
}

/**
 * Sets the fill colour for the fillmode fillrect background.
 * Uses RGBA5551
 * Official name: rcpSetBorderColour
 */
void bgdraw_fillcolour(s32 red, s32 green, s32 blue) {
    sBackgroundFillColour = GPACK_RGBA5551(red, green, blue, 1);
    sBackgroundFillColour |= (sBackgroundFillColour << 16);
}

/**
 * Clears the ZBuffer first, then decides how to draw the background which goes directly
 * over the colour buffer. DrawBG if set to 0 (which never happens) will completely skip
 * over clearing the colour buffer.
 * Official Name: rcpClearScreen
 */
void bgdraw_render(Gfx **dList, Mtx **mtx, s32 drawBG) {
    s32 widthAndHeight;
    s32 w;
    s32 h;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 wP;

    widthAndHeight = fb_size();
    w = GET_VIDEO_WIDTH(widthAndHeight) - 1;
    h = GET_VIDEO_HEIGHT(widthAndHeight) - 1;
    wP = w + 1;

    DEBUG_VAR(gDebug->misc.drawBG, drawBG);
    DEBUG_VAR(gDebug->misc.invertBG, gInvertBG);

    gDPPipeSync((*dList)++);
    //!@bug: the scissor does not need the off by one here, despite being intended for fill mode.
    gDPSetScissor((*dList)++, 0, 0, 0, wP, h + 1);
    gDPSetCycleType((*dList)++, G_CYC_FILL);
    gDPSetColorImage((*dList)++, G_IM_FMT_RGBA, gBitDepth, wP, SEGMENT_ZBUFFER << 24);
    gDPSetFillColor((*dList)++, GPACK_RGBA5551(255, 255, 240, 0) << 16 | GPACK_RGBA5551(255, 255, 240, 0));
    gDPFillRectangle((*dList)++, 0, 0, w, h);
    gDPPipeSync((*dList)++);
    gDPSetColorImage((*dList)++, G_IM_FMT_RGBA, gBitDepth, wP, SEGMENT_FRAMEBUFFER << 24);
    if (check_viewport_background_flag(PLAYER_ONE)) {
        if (gTexBGTex1) {
            bgdraw_texture(dList);
        } else if (gBGDrawFunc != NULL) {
            (gBGDrawFunc)(dList, mtx);
        } else {
            if (drawBG) {
                gDPSetFillColor((*dList)++, sBackgroundFillColour);
                gDPFillRectangle((*dList)++, 0, 0, w, h);
            }
        }
        // Used for secondary viewport backgrounds. This does not need to be 1 cycle, this could easily work with
        // fillmode.
        if ((drawBG || gInvertBG) && copy_viewport_background_size_to_coords(0, &x1, &y1, &x2, &y2)) {
            gDPSetCycleType((*dList)++, G_CYC_FILL);
            gDPSetRenderMode((*dList)++, G_RM_NOOP, G_RM_NOOP2);
            x2--;
            if (drawBG) {
                y2--;
                goto otherBG;
            } else {
                gDPSetFillColor((*dList)++,
                (GPACK_RGBA5551(0, 0, 0, 1) << 16) |
                    GPACK_RGBA5551(0, 0, 0, 1));

                gDPFillRectangle((*dList)++, 0, 0, w, y1 - 1);
                gDPFillRectangle((*dList)++, 0, y2, w, h);
            }
        }
    } else {
        if (gTexBGTex1) {
            bgdraw_texture(dList);
        } else if (gBGDrawFunc != NULL) {
            (gBGDrawFunc)(dList, mtx);
        } else if (drawBG) {
            x1 = 0;
            y1 = 0;
            x2 = w;
            y2 = h;
            otherBG:
            gDPSetFillColor((*dList)++,
                            (GPACK_RGBA5551(sBGPrimColourR, sBGPrimColourG, sBGPrimColourB, 1) << 16) |
                                GPACK_RGBA5551(sBGPrimColourR, sBGPrimColourG, sBGPrimColourB, 1));
            gDPFillRectangle((*dList)++, x1, y1, x2, y2);
        }
    }
    gDPPipeSync((*dList)++);
    viewport_scissor(dList);
}

/**
 * Gets the framebuffer width, then points to the start of segment 0x01 in memory.
 * afterwards, calls the draw command that initialises all the rendermodes, ready for use.
 */
void rdp_init(Gfx **dList) {
    s32 width = GET_VIDEO_WIDTH(fb_size());
    gDPPipeSync((*dList)++);
    gDPSetColorImage((*dList)++, G_IM_FMT_RGBA, gBitDepth, width, SEGMENT_FRAMEBUFFER << 24);
    gDPSetDepthImage((*dList)++, SEGMENT_ZBUFFER << 24);
    //gSPDisplayList((*dList)++, dRdpInit);
}

/**
 * Calls the draw command that sets all the OtherModes, ready for use.
 * Official name: rcpInitSp or rcpInitDpNoSize
 */
void rsp_init(Gfx **dList) {
    //gSPDisplayList((*dList)++, dRspInit);
    if (gConfig.terrainQuality) {
        gSPClipRatio((*dList)++, FRUSTRATIO_1);
    } else {
        gSPClipRatio((*dList)++, FRUSTRATIO_2);
    }
}

/**
 * Initialise the mesg queues for the gfx tasks, that interface with the scheduler.
 */
void gfxtask_init(OSSched *sc) {
    osCreateMesgQueue(&gRCPMesgQueue, &gRCPMesgBuf, 1);
    //osCreateMesgQueue(&gUnusedMesgQueue, gUnusedMesgBuf, ARRAY_COUNT(gUnusedMesgBuf));
}

/**
 * Set one or two textures for the patterned world themed background.
 * Can also apply a shift to make tiling less obvious.
 */
void bgdraw_texture_init(TextureHeader *tex1, TextureHeader *tex2, u32 shiftX) {
    gTexBGTex1 = tex1;
    gTexBGTex2 = tex2;
    gTexBGShiftX = shiftX * 4;
}

/**
 * Seems to render the background screen after a race finishes while you're at the menu deciding what to do next.
 * Official Name: rcpMosaicClear
 */
void bgdraw_texture(Gfx **dList) {
    s32 widthAndHeight;
    // The following variables use the (30.2) fixed-point format
    s32 sumTextureHeights;
    s32 videoHeight;
    s32 videoWidth;
    s32 texWidth;
    s32 texHeight;
    s32 xOffset;
    s32 uly;
    s32 ulx;

    widthAndHeight = fb_size();
    videoWidth = GET_VIDEO_WIDTH(widthAndHeight);
    videoHeight = GET_VIDEO_HEIGHT(widthAndHeight) & 0xFFFF;
    gSPDisplayList((*dList)++, dRaceFinishBackgroundSettings);

    if (gTexBGTex2 == NULL) {
        // Fill the background with a single texture
        gDkrDmaDisplayList((*dList)++, OS_K0_TO_PHYSICAL(gTexBGTex1->cmd), gTexBGTex1->numberOfCommands);
        videoWidth <<= 2;
        videoHeight <<= 2;
        texWidth = gTexBGTex1->width << 2;
        texHeight = gTexBGTex1->height << 2;
        for (xOffset = 0, uly = 0; uly < videoHeight; uly += texHeight) {
            for (ulx = -xOffset; ulx < videoWidth; ulx += texWidth) {
                if (ulx < 0) {
                    gSPTextureRectangle((*dList)++, 0, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE, -(ulx << 3), 0, 1024, 1024);
                } else {
                    gSPTextureRectangle((*dList)++, ulx, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE, 0, 0, 1024, 1024);
                }
            }
            xOffset = (xOffset + gTexBGShiftX) & (texWidth - 1);
        }
    } else {
        // The screen is filled with alternating horizontal stripes of two textures
        gDkrDmaDisplayList((*dList)++, OS_K0_TO_PHYSICAL(gTexBGTex1->cmd), gTexBGTex1->numberOfCommands);
        videoWidth <<= 2;
        videoHeight <<= 2;
        uly = 0;
        texWidth = gTexBGTex1->width << 2;
        texHeight = gTexBGTex1->height << 2;
        sumTextureHeights = (gTexBGTex2->height << 2) + texHeight;
        for (xOffset = 0; uly < videoHeight; uly += sumTextureHeights) {
            for (ulx = -xOffset; ulx < videoWidth; ulx += texWidth) {
                if (ulx < 0) {
                    gSPTextureRectangle((*dList)++, 0, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE,
                                        -(ulx << 3), 0, 1024, 1024);
                } else {
                    gSPTextureRectangle((*dList)++, ulx, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE, 0, 0,
                                        1024, 1024);
                }
            }
            xOffset = (xOffset + gTexBGShiftX) & (texWidth - 1);
        }

        gDkrDmaDisplayList((*dList)++, OS_K0_TO_PHYSICAL(gTexBGTex2->cmd), gTexBGTex2->numberOfCommands);
        // Width and height are mistakenly multiplied by 4 again, but it doesn't cause any issues
        videoWidth <<= 2;
        videoHeight <<= 2;

        uly = texHeight;
        texHeight = gTexBGTex2->height << 2;
        for (xOffset = 0; uly < videoHeight; uly += sumTextureHeights) {
            for (ulx = -xOffset; ulx < videoWidth; ulx += texWidth) {
                if (ulx < 0) {
                    gSPTextureRectangle((*dList)++, 0, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE,
                                        -(ulx << 3), 0, 1024, 1024);
                } else {
                    gSPTextureRectangle((*dList)++, ulx, uly, ulx + texWidth, uly + texHeight, G_TX_RENDERTILE, 0, 0,
                                        1024, 1024);
                }
            }
            xOffset = (xOffset + gTexBGShiftX) & (texWidth - 1);
        }
    }
    gDPPipeSync((*dList)++);
}

/**
 * Sets the function pointer to whatever's passed through.
 * If nonzero, will override the background drawing section.
 */
void bgdraw_set_func(BackgroundFunction func) {
    gBGDrawFunc = func;
}

/**
 * Renders one or more textures directly on screen resulting from the passed image properties.
 * Texture rectangle coordinates use 10.2 precision and texture coords use 10.5 precision.
 * Typically, you do these shifts in the draw call itself, but Rare decided to do it beforehand.
 */
void texrect_draw(Gfx **dList, DrawTexture *element, s32 xPos, s32 yPos, u8 red, u8 green, u8 blue, u8 alpha) {
    TextureHeader *tex;
    s32 i;
    s32 uly;
    s32 ulx;
    s32 lry;
    s32 lrx;
    s32 t;
    s32 s;

    gSPDisplayList((*dList)++, dTextureRectangleModes);
    gDPSetPrimColor((*dList)++, 0, 0, red, green, blue, alpha);
    xPos <<= 2;
    yPos <<= 2;
    for (i = 0; (tex = element[i].texture); i++) {
        ulx = (element[i].xOffset << 2) + xPos;
        uly = (element[i].yOffset << 2) + yPos;
        lrx = (tex->width << 2) + ulx;
        lry = (tex->height << 2) + uly;
        if (lrx > 0 && lry > 0) {
            s = 0;
            t = 0;
            if (ulx < 0) {
                s = -(ulx << 3);
                ulx = 0;
            }
            if (uly < 0) {
                t = -(uly << 3);
                uly = 0;
            }
            gDkrDmaDisplayList((*dList)++, OS_PHYSICAL_TO_K0(tex->cmd), tex->numberOfCommands);
            DEBUG_VAR(gDebug->misc.texLoads, gDebug->misc.texLoads + 1);
            gSPTextureRectangle((*dList)++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, 1024, 1024);
        }
    }
    gDPPipeSync((*dList)++);
    gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
}

/**
 * Renders one or more textures directly on screen resulting from the passed image properties.
 * Texture rectangle coordinates use 10.2 precision and texture coords use 10.5 precision.
 * Typically, you do these shifts in the draw call itself, but Rare decided to do it beforehand.
 * Also applies texel shifting in order to apply scaling.
 */
void texrect_draw_scaled(Gfx **dList, DrawTexture *element, f32 xPos, f32 yPos, f32 xScale, f32 yScale, u32 colour,
                         s32 flags) {
    TextureHeader *tex;
    Gfx *dmaDlist;
    s32 i;
    s32 bFlipX;
    s32 bFlipY;
    s32 s;    // the texture coordinate s of upper-left corner of rectangle (s10.5)
    s32 t;    // the texture coordinate t of upper-left corner of rectangle (s10.5)
    s32 dsdx; // the change in s for each change in x (s5.10)
    s32 dtdy; // the change in t for each change in y (s5.10)
    s32 ulx;  // the y-coordinate of upper-left corner of rectangle (10.2, 0.0~1023.75)
    s32 uly;  // the y-coordinate of upper-left corner of rectangle (10.2, 0.0~1023.75)
    s32 lrx;  // the x-coordinate of lower-right corner of rectangle (10.2, 0.0~1023.75)
    s32 lry;  // the y-coordinate of lower-right corner of rectangle (10.2, 0.0~1023.75)
    s32 xPos4x;
    s32 yPos4x;
    s32 width;
    s32 height;

    height = fb_size();
    width = GET_VIDEO_WIDTH(height) * 4;
    height = (GET_VIDEO_HEIGHT(height) & 0xFFFF) * 4;

    // If the colour is fully opaque
    if ((colour & 0xFF) == 0xFF) {
        dmaDlist = dTextureRectangleScaledOpa[(u8) flags & 0xFF];
    } else {
        dmaDlist = dTextureRectangleScaledXlu[(u8) flags & 0xFF];
    }

    gSPDisplayList((*dList)++, dScaledRectangleBaseModes);
    gDkrDmaDisplayList((*dList)++, OS_K0_TO_PHYSICAL(dmaDlist), numberOfGfxCommands(dTextureRectangleScaledOpa[0]));
    gDPSetPrimColorRGBA((*dList)++, colour);

    bFlipX = flags & TEXRECT_FLIP_X;
    bFlipY = flags & TEXRECT_FLIP_Y;
    xScale *= 4;
    yScale *= 4;
    xPos4x = xPos * 4;
    yPos4x = yPos * 4;

    for (i = 0; (tex = element[i].texture); i++) {
        if (!bFlipX) {
            ulx = (s32) (element[i].xOffset * xScale) + xPos4x;
        } else {
            lrx = xPos4x - (s32) (element[i].xOffset * xScale);
            ulx = lrx - (s32) (tex->width * xScale);
        }
        if (!bFlipY) {
            uly = (s32) (element[i].yOffset * yScale) + yPos4x;
        } else {
            lry = yPos4x - (s32) (element[i].yOffset * yScale);
            uly = lry - (s32) (tex->height * yScale);
        }
        if (ulx < width && uly < height) {
            if (!bFlipX) {
                lrx = (s32) (tex->width * xScale) + ulx;
            }
            if (!bFlipY) {
                lry = (s32) (tex->height * yScale) + uly;
            }
            if (lrx > 0 && lry > 0 && ulx < lrx && uly < lry) {
                dsdx = ((tex->width - 1) << 12) / (lrx - ulx);
                if (bFlipX) {
                    s = (tex->width - 1) << 5;
                    dsdx = -dsdx;
                } else {
                    s = 0;
                }
                dtdy = ((tex->height - 1) << 12) / (lry - uly);
                if (bFlipY) {
                    t = (tex->height - 1) << 5;
                    dtdy = -dtdy;
                } else {
                    t = 0;
                }
                if (ulx < 0) {
                    s += (-ulx * dsdx) >> 7;
                    ulx = 0;
                }
                if (uly < 0) {
                    t += (-uly * dtdy) >> 7;
                    uly = 0;
                }

                gDkrDmaDisplayList((*dList)++, OS_PHYSICAL_TO_K0(tex->cmd), tex->numberOfCommands);
                DEBUG_VAR(gDebug->misc.texLoads, gDebug->misc.texLoads + 1);
                gSPTextureRectangle((*dList)++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);
            }
        }
    }

    gDPPipeSync((*dList)++);
    gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
}
