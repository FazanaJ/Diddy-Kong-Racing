/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800249E0 */

#include "tracks.h"

#include "types.h"
#include "macros.h"
#include "structs.h"
#include "f3ddkr.h"
#include "PR/gu.h"
#include "textures_sprites.h"
#include "objects.h"
#include "game.h"
#include "memory.h"
#include "racer.h"
#include "camera.h"
#include "waves.h"
#include "game_ui.h"
#include "weather.h"
#include "particles.h"
#include "lib/src/libc/rmonPrintf.h"
#include "objects.h"
#include "math_util.h"
#include "main.h"
#include "printf.h"
#include "collision.h"
#include "controller.h"

// Maximum size for a level model is 522.5 KiB
#define LEVEL_MODEL_MAX_SIZE 0x80000
#define LEVEL_SEGMENT_MAX 128

/************ .data ************/

s32 D_800DC870 = 0; // Currently unknown, might be a different type.
//!@bug These two transition effects are marked to not clear when done, meaning they stay active the whole time.
FadeTransition gFullFadeToBlack = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_OUT, FADE_COLOR_BLACK, 40, 0);
FadeTransition gCircleFadeToBlack = FADE_TRANSITION(FADE_CIRCLE, FADE_FLAG_OUT, FADE_COLOR_BLACK, 70, 0);

f32 D_800DC884[10] = {
    0.0f, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f
};

Vec3f D_800DC8AC[3][3] = {
    { {{{ 50.0f, 0.0f, 32.0f }}}, {{{  -50.0f,   0.0f,  32.0f }}}, {{{  -50.0f, 100.0f,  32.0f }}} },
    { {{{  0.0f, 0.0f, 32.0f }}}, {{{  130.0f,  60.0f, -68.0f }}}, {{{  130.0f, -60.0f, -68.0f }}} },
    { {{{  0.0f, 0.0f, 32.0f }}}, {{{ -130.0f, -60.0f, -68.0f }}}, {{{ -130.0f,  60.0f, -68.0f }}} },
};

LevelModel *gCurrentLevelModel = NULL; //Official Name: track
LevelHeader *gCurrentLevelHeader2 = NULL;

s32 D_800DC920 = -1;
s32 *D_800DC924 = NULL;
s32 D_800DC928 = 0; // Currently unknown, might be a different type.

s8 D_800DC92C[24] = {
    0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4, 5, 6, 1, 1, 0, 5, 3, 2, 7, 7, 8, 3
    // There may or may not be extra zeroes here.
};

/*******************************/

/************ .bss ************/

Gfx *gSceneCurrDisplayList;
MatrixS *gSceneCurrMatrix;
Vertex *gSceneCurrVertexList;
TriangleList *gSceneCurrTriList;

ObjectSegment *gSceneActiveCamera;

s32 gSceneCurrentPlayerID;
Object *gSkydomeSegment;
s32 gShadowHeapFlip; // Flips between 0 and 1 to prevent incorrect access between frames.
s32 D_8011B0CC;
s32 gShadowIndex;
s32 gSceneStartSegment;
s32 D_8011B0D8;
s32 gSceneRenderSkyDome;
s8 gDrawLevelSegments;
u8 D_8011B0E1;
u8 D_8011B0E2;
u8 D_8011B0E3;
f32 D_8011B0E4;
f32 D_8011B0E8;
f32 D_8011B0EC;
s32 D_8011B0F0;
s32 D_8011B0F4;
s32 D_8011B0F8; //gIsInCutscene?
s32 D_8011B100;
s32 D_8011B104;
s32 D_8011B108;
s32 D_8011B10C;
s32 D_8011B110;
u32 D_8011B114;
s32 D_8011B118;
s32 D_8011B11C;
unk8011B120 D_8011B120[32]; //Struct sizeof(0x10) / sizeof(16)
s32 D_8011B320[4];
unk8011B330 D_8011B330[120]; //Struct sizeof(0x20) / sizeof(32)
s32 D_8011C230;
s32 D_8011C234;
unk8011C238 D_8011C238[32]; //Struct sizeof(0xC) / sizeof(12)
s32 D_8011C3B8[320];
s32 D_8011C8B8[512];
s32 D_8011D0B8;
Vec4f *D_8011D0BC;
TextureHeader *D_8011D0C0;
Object *D_8011D0C4;
f32 D_8011D0C8;
s16 D_8011D0CC;
s16 D_8011D0CE;
s16 D_8011D0D0;
f32 gShadowOpacity;
f32 D_8011D0D8;
f32 D_8011D0DC;
f32 D_8011D0E0;
f32 D_8011D0E4;
s32 D_8011D0E8;
s32 D_8011D0EC;
f32 D_8011D0F0;
f32 D_8011D0F4;
Vec4f D_8011D0F8[3];
Vec4f D_8011D128[3];
s32 D_8011D158[3]; // Unused? Or part of something bigger above?
s32 D_8011D164;
s32 D_8011D168[84];
WaterProperties *gTrackWaves[20];
s8 D_8011D308;
LevelModel *gTrackModelHeap;
s32 *gLevelModelTable;
Triangle *gShadowHeapTris[4];
Triangle *gCurrentShadowTris;
Vertex *gShadowHeapVerts[4];
Vertex *gCurrentShadowVerts;
DrawTexture *gShadowHeapTextures[4];
DrawTexture *gCurrentShadowTexture;
s32 D_8011D364;
s32 D_8011D368; //xOffset?
s32 D_8011D36C; //yOffset?
s32 *D_8011D370;
s32 *D_8011D374;
s32 D_8011D378;
s32 gScenePlayerViewports;
u32 gWaveBlockCount;
FogData gFogData[4];
Vec3i gScenePerspectivePos;
unk8011D474 *D_8011D474; // 0x10 bytes struct?
unk8011D478 *D_8011D478; // 0xC bytes struct?
s8 *D_8011D47C;
Vertex *D_8011D480[2];
Vertex *D_8011D488;
s32 D_8011D48C;
TriangleList *D_8011D490[2];
Vertex *D_8011D498;
s16 D_8011D49C;
s16 D_8011D49E;
f32 D_8011D4A0;
f32 D_8011D4A4;
f32 D_8011D4A8;
f32 D_8011D4AC;
f32 D_8011D4B0;
s8 D_8011D4B4;
s16 D_8011D4B6;
s16 D_8011D4B8;
s16 D_8011D4BA;
s16 D_8011D4BC;
RenderNodeTrack *gRenderNodeHead;
RenderNodeTrack *gRenderNodeTail;
RenderListTrack *gMateriallistHead;
RenderListTrack *gMateriallistTail;
u8 sShowAll = FALSE;
void *gSorterHeap;
u32 gSorterPos;

/******************************/

/**
 * Sets the number of expected viewports in the scene.
 * Like most other viewport vars, it's 0-3 rather than 1-4.
 * Set as an s32 for some reason.
*/
s32 set_scene_viewport_num(s32 numPorts) {
    gScenePlayerViewports = numPorts;
    return 0;
}

/**
 * Initialises the level.
 * Allocates RAM to load generate the level geometry, spawn objects and generate shadows.
*/
void init_track(u32 geometry, u32 skybox, s32 numberOfPlayers, Vehicle vehicle, u32 entranceId, u32 collectables, u32 arg6) {
    s32 i;

    gCurrentLevelHeader2 = gCurrentLevelHeader;
    D_8011B0F8 = FALSE;
    D_8011B100 = 0;
    D_8011B104 = 0;
    D_8011B108 = 0;
    D_8011B10C = 0;
    
    if (gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_1 || gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_2) {
        D_8011B0F8 = TRUE;
    }

    func_8002C0C4(geometry);

    gWaveBlockCount = 0;
    
    if (numberOfPlayers < 2 && !gConfig.perfMode) {
        for (i = 0; i < gCurrentLevelModel->numberOfSegments; i++) {
            if (gCurrentLevelModel->segments[i].hasWaves != 0) {
                gWaveBlockCount++;
                gCurrentLevelModel->segments[i].hasWaves = 1;
            }
        }
    }

    if (is_in_two_player_adventure() && (gCurrentLevelHeader2->race_type == RACETYPE_DEFAULT || gCurrentLevelHeader2->race_type & RACETYPE_CHALLENGE)) {
        i = 2;
    } else {
        i = numberOfPlayers + 1;
    }
    
    if (gWaveBlockCount) {
        func_800B82B4(gCurrentLevelModel, gCurrentLevelHeader2, i);
    }
    
    set_active_viewports_and_max(numberOfPlayers);
    spawn_skydome(skybox);
    D_8011B110 = 0;
    D_8011B114 = 0x10000;
    func_80011390();
    func_8000C8F8(arg6, 0);
    func_8000C8F8(collectables, 1);
    gScenePlayerViewports = numberOfPlayers;
    func_8000CC7C(vehicle, entranceId, numberOfPlayers);
    func_8000B020(72, 64);
    
    if (geometry == 0 && entranceId == 0) {
        transition_begin(&gCircleFadeToBlack);
    } else {
        transition_begin(&gFullFadeToBlack);
    }
    set_active_viewports_and_max(gScenePlayerViewports);

    numberOfPlayers = gScenePlayerViewports;
    for (i = 0; i < ARRAY_COUNT(gShadowHeapTextures); i++) {
        gShadowHeapTextures[i] = (DrawTexture *) allocate_from_main_pool_safe(sizeof(DrawTexture) * 200, MEMP_SHADOWS);
        gShadowHeapTris[i] = (Triangle *) allocate_from_main_pool_safe(sizeof(Triangle) * 400, MEMP_SHADOWS);
        gShadowHeapVerts[i] = (Vertex *) allocate_from_main_pool_safe(sizeof(Vertex) * 1000, MEMP_SHADOWS);
    }

    gShadowHeapFlip = 0;
    update_shadows(SHADOW_SCENERY, SHADOW_SCENERY, LOGIC_NULL);
    update_shadows(SHADOW_ACTORS, SHADOW_ACTORS, LOGIC_NULL);
    gShadowHeapFlip = 1;
    update_shadows(SHADOW_SCENERY, SHADOW_SCENERY, LOGIC_NULL);
    update_shadows(SHADOW_ACTORS, SHADOW_ACTORS, LOGIC_NULL);
    gShadowHeapFlip = 0;
    if (gCurrentLevelHeader2->unkB7) {
        D_8011B0E1 = gCurrentLevelHeader2->unkB4;
        D_8011B0E2 = gCurrentLevelHeader2->unkB5;
        D_8011B0E3 = gCurrentLevelHeader2->unkB6;
        func_80025510(numberOfPlayers + 1);
    }
}

u16 gFastBG[240];
u8 gMakeBG = FALSE;

f32 approach_f32_asymptotic(f32 current, f32 target, f32 multiplier) {
    current = current + (target - current) * multiplier;
    return current;
}



void render_fast_bg(Gfx **dList) {
    if (gMakeBG) {
        LevelHeader *h = gCurrentLevelHeader2;
        u16 colour;
        s32 i;
        
        for (i = 0; i < 240; i++) {
            f32 index = (f32) ((f32) i / 240.0f);
            
            colour =  GPACK_RGBA5551(((s32) approach_f32_asymptotic(h->unkBE, h->unkC1, index)), 
                                     ((s32) approach_f32_asymptotic(h->unkBF, h->unkC2, index)), 
                                     ((s32) approach_f32_asymptotic(h->unkC0, h->unkC3, index)), 1);
            gFastBG[i] = colour;
        }
        gMakeBG = FALSE;
    }

    gDPPipeSync((*dList)++);
    gDPSetRenderMode((*dList)++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType((*dList)++, G_CYC_COPY);
	gDPSetTextureImage((*dList)++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 240, gFastBG);
	gDPSetTile((*dList)++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 60, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0);
	gDPLoadTile((*dList)++, 7, 0, 0, 956, 0);
	gDPSetTile((*dList)++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 60, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 8, 0);
	gDPSetTileSize((*dList)++, 0, 0, 0, 956, 0);
    gDPPipeSync((*dList)++);
    gSPTextureRectangleFlip((*dList)++, 0, 0, (gScreenWidth - 1) << 2, (gScreenHeight - 1) << 2, G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
    gDPPipeSync((*dList)++);
}

/**
 * The root function for rendering the entire scene.
 * Handles drawing the track, objects and the majority of the HUD in single player.
*/
void render_scene(Gfx **dList, MatrixS **mtx, Vertex **vtx, TriangleList **tris, s32 updateRate) {
    s32 i;
    s32 numViewports;
    s32 tempUpdateRate;
    s8 flip;
    s32 posX;
    s32 posY;
    s32 j;
    profiler_begin_timer();

    gSceneCurrDisplayList = *dList;
    gSceneCurrMatrix = *mtx;
    gSceneCurrVertexList = *vtx;
    gSceneCurrTriList = *tris;
    gSceneRenderSkyDome = TRUE;
    numViewports = set_active_viewports_and_max(gScenePlayerViewports);
    if (is_game_paused()) {
        tempUpdateRate = 0;
    } else {
        tempUpdateRate = updateRate;
    }
    if (gWaveBlockCount) {
        profiler_begin_timer2();
        func_800B9C18(tempUpdateRate);
        profiler_add(PP_WAVES, first2);
    }
    if (gMapId != ASSET_LEVEL_CHARACTERSELECT) {
        update_shadows(SHADOW_ACTORS, SHADOW_ACTORS, updateRate);
    }
    for (i = 0; i < 7; i++) {
        if ((s32) gCurrentLevelHeader2->unk74[i] != -1) {
            update_colour_cycle(gCurrentLevelHeader2->unk74[i], tempUpdateRate);
        }
    }
    if (gCurrentLevelHeader2->pulseLightData != (PulsatingLightData *) -1) {
        update_pulsating_light_data(gCurrentLevelHeader2->pulseLightData, tempUpdateRate);
    }
    gDrawLevelSegments = TRUE;
    if (gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_2) {
        gDrawLevelSegments = FALSE;
    }
    if (gCurrentLevelHeader2->skyDome == -1) {
        i = (gCurrentLevelHeader2->unkA4->width << 9) - 1;
        gCurrentLevelHeader2->unkA8 = (gCurrentLevelHeader2->unkA8 + (gCurrentLevelHeader2->unkA2 * tempUpdateRate)) & i;
        i = (gCurrentLevelHeader2->unkA4->height << 9) - 1;
        gCurrentLevelHeader2->unkAA = (gCurrentLevelHeader2->unkAA + (gCurrentLevelHeader2->unkA3 * tempUpdateRate)) & i;
        tex_animate_texture(gCurrentLevelHeader2->unkA4, &D_8011B114, &D_8011B110, tempUpdateRate);
    }
    flip = FALSE;
    if (get_filtered_cheats() & CHEAT_MIRRORED_TRACKS) {
        flip = TRUE;
    }
#ifndef NO_ANTIPIRACY
    // Antipiracy measure
    if (IO_READ(0x200) != 0xAC290000) {
        flip = TRUE;
    }
#endif
    reset_render_settings(dList);
    gDkrDisableBillboard((*dList)++);
    gSPClearGeometryMode((*dList)++, G_CULL_FRONT);
    gDPSetBlendColor((*dList)++, 0, 0, 0, 0x64);
    gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor((*dList)++, 255, 255, 255, 0);
    func_800AD40C();
    update_fog(numViewports, tempUpdateRate);
    func_800AF404(tempUpdateRate);
    if (gCurrentLevelModel->numberOfAnimatedTextures > 0) {
        animate_level_textures(tempUpdateRate);
    }
    for (j = gSceneCurrentPlayerID = 0; j < numViewports; gSceneCurrentPlayerID++, j = gSceneCurrentPlayerID) {
        if (j == 0) {
            if (func_8000E184() && numViewports == 1) {
                gSceneCurrentPlayerID = PLAYER_TWO;
            }
        }
        if (flip) {
            gSPSetGeometryMode((*dList)++, G_CULL_FRONT);
        }
        apply_fog(dList, gSceneCurrentPlayerID);
        gDPPipeSync((*dList)++);
        set_active_camera(gSceneCurrentPlayerID);
        func_80066CDC(dList, &gSceneCurrMatrix);
        func_8002A31C();
        // Show detailed skydome in single player.
        if (!gConfig.perfMode) {
            if (numViewports < 2 || gSkipCutbacks) {
                func_80068408(dList, &gSceneCurrMatrix);
                if (gCurrentLevelHeader2->skyDome == -1) {
                    gSceneCurrDisplayList = *dList;
                    func_80028050();
                    *dList = gSceneCurrDisplayList;
                } else {
                    render_skydome(dList);
                }
            } else {
                func_8006807C(dList, &gSceneCurrMatrix);
                draw_gradient_background(dList);
                func_80067D3C(dList, &gSceneCurrMatrix);
                //render_fast_bg(dList);
                func_80068408(dList, &gSceneCurrMatrix);
            }
        } else {
            func_80068408(dList, &gSceneCurrMatrix);
        }
        gDPPipeSync((*dList)++);
        initialise_player_viewport_vars(dList, updateRate);
        profiler_reset_timer();
        set_weather_limits(-1, -512);
        // Show weather effects in single player.
        if (gCurrentLevelHeader2->weatherEnable > 0 && numViewports < 2 && !gConfig.perfMode) {
            process_weather(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, &gSceneCurrTriList, tempUpdateRate);
        }
        profiler_add(PP_WEATHER, first);
        func_800AD030(get_active_camera_segment());
        func_800ACA20(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_active_camera_segment());
#ifdef PUPPYPRINT_DEBUG
        if (gPuppyPrint.showCvg) {
            puppyprint_render_coverage(dList);
        }
#endif
        profiler_reset_timer();
        render_hud(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_racer_object_by_port(gSceneCurrentPlayerID), updateRate);
        profiler_add(PP_HUD, first);
    }
    // Show TT Cam toggle for the fourth viewport when playing 3 player.
    if (numViewports == 3 && gCurrentLevelHeader->race_type != RACETYPE_CHALLENGE_EGGS && 
        gCurrentLevelHeader->race_type != RACETYPE_CHALLENGE_BATTLE && gCurrentLevelHeader->race_type != RACETYPE_CHALLENGE_BANANAS) {
        if (gHudToggleSettings[gHUDNumPlayers] == 0) {
            if (flip) {
                gSPSetGeometryMode((*dList)++, G_CULL_FRONT);
            }
            apply_fog(dList, PLAYER_FOUR);
            gDPPipeSync((*dList)++);
            set_active_camera(PLAYER_FOUR);
            gCutsceneCameraActive = FALSE;
            func_800278E8(updateRate);
            func_80066CDC(dList, &gSceneCurrMatrix);
            func_8002A31C();
            func_8006807C(dList, &gSceneCurrMatrix);
            //draw_gradient_background(dList);
            func_80067D3C(dList, &gSceneCurrMatrix);
            func_80068408(dList, &gSceneCurrMatrix);
            gDPPipeSync((*dList)++);
            initialise_player_viewport_vars(dList, updateRate);
            set_weather_limits(-1, -512);
            func_800AD030(get_active_camera_segment());
            func_800ACA20(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_active_camera_segment());
            set_text_font(FONT_COLOURFUL);
            posX = (gScreenWidth / 2) + 10;
            posY = (gScreenHeight / 2) + 5;
            draw_text(dList, posX, posY, "TT CAM", ALIGN_TOP_LEFT);
        } else {
            set_active_camera(PLAYER_FOUR);
            func_800278E8(updateRate);
        }
    }
    func_800682AC(dList);
    gDPPipeSync((*dList)++);
    gDkrDisableBillboard((*dList)++);
    gShadowHeapFlip = 1 - gShadowHeapFlip;
    //*dList = gSceneCurrDisplayList;
    *mtx = gSceneCurrMatrix;
    *vtx = gSceneCurrVertexList;
    *tris = gSceneCurrTriList;
}

GLOBAL_ASM("asm/non_matchings/tracks/func_80025510.s")

void func_800257D0(void) {
    if (D_800DC924 != 0) {
        free_from_memory_pool(D_8011D474);
        free_from_memory_pool(D_800DC924);
        D_800DC924 = 0;
    }
}

#ifdef NON_EQUIVALENT
void func_80026070(LevelModelSegmentBoundingBox *, f32, f32, f32);
void func_80026430(LevelModelSegment *, f32, f32, f32);
void func_80026E54(s16 arg0, s8 *arg1, f32 arg2, f32 arg3);
//Alternative Attempt: https://decomp.me/scratch/2C6dJ
void func_8002581C(u8 *segmentIds, s32 numberOfSegments, s32 viewportIndex) {
    Vertex *spAC;
    s8 *spA8;
    s8 sp7C;
    LevelModelSegmentBoundingBox *bbox;
    Vertex *temp_t6;
    f32 yCameraCoss;
    f32 x1Sins;
    f32 x2Sins;
    f32 z2Coss;
    f32 temp_f22;
    f32 yCameraSins;
    f32 z1Coss;
    s16 *var_a0;
    s16 temp_s3;
    s32 temp_t3_2;
    s16 var_s0;
    s16 i;
    s16 var_s4;
    s16 var_v1;
    s32 temp_t3;
    s32 temp_t4;
    s32 continueLoop;
    s16 check1;
    s16 check2;
    s16 check3;
    s16 check4;
    s8 *temp_t3_3;
    s8 *temp_t7;
    s8 *temp_v0_4;
    s8 *var_t6;
    s8 temp_t3_4;
    u8 segmentIndex;

    D_8011D490[0] = D_8011D474[viewportIndex].unk0;
    D_8011D490[1] = D_8011D474[viewportIndex].unk4;
    D_8011D480[0] = D_8011D474[viewportIndex].unk8;
    D_8011D480[1] = D_8011D474[viewportIndex].unkC;
    load_and_set_texture_no_offset(&gSceneCurrDisplayList, NULL, RENDER_ANTI_ALIASING | RENDER_Z_COMPARE);
    D_8011D49C = 0;
    D_8011D49E = 0;
    yCameraSins = sins_f(gSceneActiveCamera->trans.y_rotation * -1);
    yCameraCoss = coss_f(gSceneActiveCamera->trans.y_rotation * -1);
    D_8011D4AC = (gSceneActiveCamera->trans.x_position + (yCameraSins * 250.0f));
    D_8011D4B0 = (gSceneActiveCamera->trans.z_position + (yCameraCoss * 250.0f));
    D_8011D4A0 = -yCameraCoss;
    D_8011D4A4 = yCameraSins;
    temp_f22 = -((yCameraSins * D_8011D4AC) + (yCameraCoss * D_8011D4B0));
    D_8011D4A8 = -((D_8011D4A0 * D_8011D4AC) + (D_8011D4A4 * D_8011D4B0));
    for (i = 0; i < numberOfSegments; i++) {
        segmentIndex = segmentIds[i];
        bbox = &gCurrentLevelModel->segmentsBoundingBoxes[segmentIndex];
        x1Sins = bbox->x1 * yCameraSins;
        z1Coss = bbox->z1 * yCameraCoss;
        x2Sins = bbox->x2 * yCameraSins;
        z2Coss = bbox->z2 * yCameraCoss;
        check1 = FALSE;
        check2 = FALSE;
        check3 = FALSE;
        check4 = FALSE;
        if ((x1Sins + z1Coss + temp_f22) <= 0.0f) {
            check1 = TRUE;
        }
        if ((x2Sins + z1Coss + temp_f22) <= 0.0f) {
            check2 = TRUE;
        }
        if ((x1Sins + z2Coss + temp_f22) <= 0.0f) {
            check3 = TRUE;
        }
        if ((x2Sins + z2Coss + temp_f22) <= 0.0f) {
            check4 = TRUE;
        }
        if (((s16) ((s16) (check1 + check2) + check3) + check4) & 3) {
            func_80026430(bbox, yCameraSins, yCameraCoss, temp_f22);
            if (gCurrentLevelModel->segments[segmentIndex].unk3C & 2) {
                func_80026070(bbox, yCameraSins, yCameraCoss, temp_f22);
            }
        }
    }
    func_80026C14( 300, (gCurrentLevelModel->lowerYBounds - 195), 1);
    func_80026C14(-300, (gCurrentLevelModel->lowerYBounds - 195), 1);
    func_80026C14( 300, (gCurrentLevelModel->upperYBounds + 195), 0);
    func_80026C14(-300, (gCurrentLevelModel->upperYBounds + 195), 0);
    if (D_8011D49E < D_8011D4BA && D_8011D49E != 0) {
        continueLoop = TRUE;
        do {
            for (i = 0; i < D_8011D49E - 1; i++) {
                if (D_8011D478[i].unk7 < D_8011D478[i].unk0) {
                    temp_t3 = D_8011D478[i].unk7;
                    D_8011D478[i].unk7 = D_8011D478[i].unk0;
                    temp_t4 = D_8011D478[i+1].unk0;
                    D_8011D478[i].unk0 = temp_t3;
                    D_8011D478[i+1].unk0 = D_8011D478[i].unk4;
                    D_8011D478[i].unk4 = temp_t4;
                    continueLoop = FALSE;
                }
            }
        } while (!continueLoop);
        var_s0 = 0;
        for (i = 0; i < D_8011D49E; i++) {
            temp_t3_2 = D_8011D478[var_s0].unk7 * 2;
            if (D_8011D47C[temp_t3_2] == -1) {
                D_8011D478[i].unk4 = (D_8011D478[i].unk7 | 2);
                D_8011D47C[temp_t3_2] = i;
            } else {
                D_8011D47C[temp_t3_2] = i;
            }
        }
        temp_t6 = gSceneCurrVertexList;
        gSceneCurrVertexList = D_8011D480[D_8011D4B4];
        temp_t7 = gSceneCurrTriList;
        gSceneCurrTriList = D_8011D490[D_8011D4B4];
        D_8011D4B4 = 1 - D_8011D4B4;
        var_s4 = D_8011D478->unk0;
        D_8011D488 = gSceneCurrVertexList;
        D_8011D498 = gSceneCurrTriList;
        D_8011D4B6 = 0;
        D_8011D4B8 = 0;
        spAC = temp_t6;
        spA8 = temp_t7;
        for (i = 0; i < D_8011D49E; i++) {
            if ((i < D_8011D49E) != 0) {
                var_a0 = &D_8011D478[i].unk0;
                if (var_s4 == *var_a0) {
                    temp_t3_3 = &(&sp7C)[var_s0];
                    if (var_a0[3] & 2) {
                        var_s0++;
                        *temp_t3_3 = var_a0[7];
                    } else {
                        for (var_v1 = 0; var_v1 < var_s0; var_v1++) {
                            var_t6 = &(&sp7C)[var_v1];
                            if (*var_t6 == var_a0[7]) {
                                var_s0 -= 1;
                                while (var_v1 < var_s0) {
                                    temp_v0_4 = &(&sp7C)[var_v1];
                                    temp_t3_4 = temp_v0_4[1];
                                    temp_v0_4[0] = temp_t3_4;
                                    var_v1++;
                                }
                            }
                        }
                    }
                }
            }
            if (i < D_8011D49E) {
                temp_s3 = D_8011D478[i].unk0;
                if (var_s4 != temp_s3) {
                    func_80026E54(var_s0, &sp7C, (f32) temp_s3, (f32) var_s4);
                    var_s4 = temp_s3;
                }
            }
        }
        if (D_8011D4B6 != 0) {
            gSPVertexDKR(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(D_8011D488), D_8011D4B6, 0);
            gSPPolygon(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(D_8011D498), (D_8011D4B6  >> 1), TRIN_DISABLE_TEXTURE);
        }
        gSceneCurrVertexList = spAC;
        gSceneCurrTriList = spA8;
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_8002581C.s")
#endif

GLOBAL_ASM("asm/non_matchings/tracks/func_80026070.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80026430.s")

void func_80026C14(s16 arg0, s16 arg1, s32 arg2) {
    s16 i;
    s16 j;

    if (D_8011D49E < D_8011D4BA) {
        i = 0;
        while (i < D_8011D49E && D_8011D478[i].unk0 < arg0) {
            i++;
        }
        while(i < D_8011D49E && arg0 == D_8011D478[i].unk0 && D_8011D478[i].unk2 < arg1) {
            i++;
        }
        j = D_8011D49E;
        while (i < j) {
            D_8011D478[j].unk0 = D_8011D478[j-1].unk0;
            D_8011D478[j].unk2 = D_8011D478[j-1].unk2;
            D_8011D478[j].unk7 = D_8011D478[j-1].unk7;
            D_8011D478[j].unk6 = D_8011D478[j-1].unk6;
            j--;
        }
        D_8011D478[i].unk0 = arg0;
        D_8011D478[i].unk2 = arg1;
        D_8011D478[i].unk4 = 0;
        D_8011D478[i].unk7 = D_8011D49C;
        D_8011D478[i].unk6 = arg2;
        D_8011D47C[D_8011D49E] = -1;
        if (D_8011D49E & 1) {
            D_8011D49C++;
        }
        D_8011D49E++;
    }
}

GLOBAL_ASM("asm/non_matchings/tracks/func_80026E54.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80027184.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80027568.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_800278E8.s")

/**
 * Handle the flipbook effect for level geometry textures.
*/
void animate_level_textures(s32 updateRate) {
    s32 segmentNumber, batchNumber;
    LevelModelSegment *segment;
    TextureHeader *texture;
    TriangleBatchInfo *batch;
    s32 temp;

    segment = gCurrentLevelModel->segments;
    for (segmentNumber = 0; segmentNumber < gCurrentLevelModel->numberOfSegments; segmentNumber++) {
        batch = segment[segmentNumber].batches;
        for (batchNumber = 0; batchNumber < segment[segmentNumber].numberOfBatches; batchNumber++) {
            if (batch[batchNumber].flags & BATCH_FLAGS_TEXTURE_ANIM) {
                if (batch[batchNumber].textureIndex != 0xFF) {
                    texture = gCurrentLevelModel->textures[batch[batchNumber].textureIndex].texture;
                    if (texture->numOfTextures != 0x100 && texture->frameAdvanceDelay) {
                        temp = batch[batchNumber].unk7 << 6;
                        if (batch[batchNumber].flags & BATCH_FLAGS_UNK80000000) {
                            temp |= batch[batchNumber].unk6;
                            tex_animate_texture(texture, &batch[batchNumber].flags, &temp, updateRate);
                            batch[batchNumber].unk6 = temp & 0x3F;
                        } else {
                            tex_animate_texture(texture, &batch[batchNumber].flags, &temp, updateRate);
                        }
                        batch[batchNumber].unk7 = (temp >> 6) & 0xFF;
                    }
                }
            }
        }
    }
}

/**
 * Spawns and initialises the skydome object seen ingame.
 * Skipped if the object ID doesn't exist.
 * Also compares a checksum which can potentially trigger anti-tamper measures.
*/
void spawn_skydome(s32 objectID) {
    LevelObjectEntryCommon spawnObject;

    // Antipiracy measure
    //compare_balloon_checksums();
    if (objectID == -1) {
        gSkydomeSegment = NULL;
        return;
    }
    spawnObject.x = 0;
    spawnObject.y = 0;
    spawnObject.z = 0;
    spawnObject.size = 8;
    spawnObject.objectID = objectID;
    gSkydomeSegment = spawn_object(&spawnObject, 2);
    if (gSkydomeSegment != NULL) {
        gSkydomeSegment->segment.level_entry = NULL;
        gSkydomeSegment->unk4A = -1;
    }
}

/**
 * Sets the status to render the sky.
 * If set to false, will skip the background and skydome rendering.
*/
void set_skydome_visbility(s32 renderSky) {
    gSceneRenderSkyDome = renderSky;
}

// init_skydome
//https://decomp.me/scratch/jmbc1
GLOBAL_ASM("asm/non_matchings/tracks/func_80028050.s")

/**
 * Instead of drawing the skydome with textures, draw a solid coloured background.
 * Using different colours set in the level header, the vertices are coloured and
 * it gives the background a gradient effect.
*/
void draw_gradient_background(Gfx **dList) {
    s16 y0;
    s16 y1;
    u8 headerRed0;
    u8 headerGreen0;
    u8 headerBlue0;
    u8 headerRed1;
    u8 headerGreen1;
    u8 headerBlue1;
    Vertex *verts;
    Triangle *tris;
    f32 widescreen = 1.0f;
    profiler_begin_timer();

#ifndef NATIVE_RES_WIDESCREEN
    switch (gConfig.screenMode) {
    case 1:
        widescreen = 1.2f;
        break;
    case 2:
        widescreen = 1.33f;
        break;
    } 
#endif

    verts = (Vertex *) gSceneCurrVertexList;
    tris = (Triangle *) gSceneCurrTriList;
    headerRed0 = gCurrentLevelHeader2->unkC1;
    headerGreen0 = gCurrentLevelHeader2->unkC2;
    headerBlue0 = gCurrentLevelHeader2->unkC3;
    headerRed1 = gCurrentLevelHeader2->unkBE;
    headerGreen1 = gCurrentLevelHeader2->unkBF;
    headerBlue1 = gCurrentLevelHeader2->unkC0;
    reset_render_settings(dList);
    load_and_set_texture_no_offset(dList, 0, RENDER_FOG_ACTIVE);
    gSPVertexDKR((*dList)++, OS_PHYSICAL_TO_K0(verts), 4, 0);
    gSPPolygon((*dList)++, OS_PHYSICAL_TO_K0(tris), 2, 0);
    y0 = -150;
    y1 = 150;
    if (gNumberOfViewports == TWO_PLAYERS) {
        y0 >>= 1;
        y1 >>= 1;
    }
    verts[0].x = -200 * widescreen;
    verts[0].y = y0;
    verts[0].z = 20;
    verts[0].r = headerRed0;
    verts[0].g = headerGreen0;
    verts[0].b = headerBlue0;
    verts[0].a = 255;
    verts[1].x = 200 * widescreen;
    verts[1].y = y0;
    verts[1].z = 20;
    verts[1].r = headerRed0;
    verts[1].g = headerGreen0;
    verts[1].b = headerBlue0;
    verts[1].a = 255;
    verts[2].x = -200 * widescreen;
    verts[2].y = y1;
    verts[2].z = 20;
    verts[2].r = headerRed1;
    verts[2].g = headerGreen1;
    verts[2].b = headerBlue1;
    verts[2].a = 255;
    verts[3].x = 200 * widescreen;
    verts[3].y = y1;
    verts[3].z = 20;
    verts[3].r = headerRed1;
    verts[3].g = headerGreen1;
    verts[3].b = headerBlue1;
    verts[3].a = 255;
    tris[0].flags = 0x40;
    tris[0].vi0 = 2;
    tris[0].vi1 = 1;
    tris[0].vi2 = 0;
    tris[0].uv0.u = 0;
    tris[0].uv0.v = 0;
    tris[0].uv1.u = 0;
    tris[0].uv1.v = 0;
    tris[0].uv2.u = 0;
    tris[0].uv2.v = 0;
    tris[1].flags = 0x40;
    tris[1].vi0 = 3;
    tris[1].vi1 = 2;
    tris[1].vi2 = 1;
    tris[1].uv0.u = 0;
    tris[1].uv0.v = 0;
    tris[1].uv1.u = 0;
    tris[1].uv1.v = 0;
    tris[1].uv2.u = 0;
    tris[1].uv2.v = 0;
    verts += 4;
    gSceneCurrVertexList = verts;
    tris += 2;
    gSceneCurrTriList = (TriangleList *) tris;
    profiler_add(PP_BACKGROUND, first);
}

/**
 * Sets the position to the current camera's position then renders the skydome if set to be visible.
*/
void render_skydome(Gfx **dList) {
    ObjectSegment *cam;
    s32 prevRender = gIsObjectRender;
    profiler_begin_timer();
    if (gSkydomeSegment == NULL)
        return;

    cam = get_active_camera_segment();
    if (gCurrentLevelHeader2->skyDome == 0) {
        gSkydomeSegment->segment.trans.x_position = cam->trans.x_position;
        gSkydomeSegment->segment.trans.y_position = cam->trans.y_position;
        gSkydomeSegment->segment.trans.z_position = cam->trans.z_position;
    }

    func_80068408(dList, &gSceneCurrMatrix);
    if (gSceneRenderSkyDome) {
        gIsObjectRender = 2;
        render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, gSkydomeSegment);
        gIsObjectRender = prevRender;
    }
    profiler_add(PP_BACKGROUND, first);
}

/**
 * Sets up all of the required variables for the player's view perspective.
 * This includes setting up the camera index, viewport and 
*/
void initialise_player_viewport_vars(Gfx **dList, s32 updateRate) {
    s32 i;
    s32 numRacers;
    s32 viewportID;
    Object **racers;
    s32 segmentIndex;
    Object_Racer *racer;

    gSceneActiveCamera = get_active_camera_segment();
    viewportID = gActiveCameraID;
    compute_scene_camera_transform_matrix();
    update_envmap_position(gScenePerspectivePos.x / 65536.0f, gScenePerspectivePos.y / 65536.0f, gScenePerspectivePos.z / 65536.0f);
    segmentIndex = gSceneActiveCamera->object.cameraSegmentID;
    if (segmentIndex > -1 && (segmentIndex < gCurrentLevelModel->numberOfSegments)) {
        gSceneStartSegment = gCurrentLevelModel->segments[segmentIndex].unk28;
    } else {
        gSceneStartSegment = -1;
    }
    if (gWaveBlockCount != 0) {
        profiler_begin_timer();
        func_800B8B8C();
        racers = get_racer_objects(&numRacers);
        if (gSceneActiveCamera->object.unk36 != 7 && numRacers > 0 && !gCutsceneCameraActive) {
            i = -1; 
            do {
                i++;
                racer = &racers[i]->unk64->racer;
            } while(i < numRacers - 1 && viewportID != racer->playerIndex);
            func_800B8C04(racers[i]->segment.trans.x_position, racers[i]->segment.trans.y_position, racers[i]->segment.trans.z_position, gActiveCameraID, updateRate);
        } else {
            func_800B8C04(gSceneActiveCamera->trans.x_position, gSceneActiveCamera->trans.y_position, gSceneActiveCamera->trans.z_position, gActiveCameraID, updateRate);
        }
        profiler_add(PP_WAVES, first);
    }
    gCurrentLevelHeader->unk3 = 1;
    render_level_geometry_and_objects(dList);
#ifdef PUPPYPRINT_DEBUG
    if (get_buttons_pressed_from_player(0) & L_JPAD && viewportID == 0) {
        sShowAll ^= 1;
    }
#endif
}

/**
 * Enable or disable anti aliasing.
 * Improves visual quality at the cost of performance.
*/
void set_anti_aliasing(UNUSED s32 setting) {
}

void pop_render_list_track(Gfx **dList) {
    RenderNodeTrack *renderList = gRenderNodeHead;
    s32 prevAlpha = 256;

    while (renderList) {
        if (renderList->primAlpha != prevAlpha) {
            gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, renderList->primAlpha);
            prevAlpha = renderList->primAlpha;
        }
        if (renderList->material) {
            load_and_set_texture(dList, renderList->material, renderList->flags, renderList->texOffset);
        }
        gSPVertexDKR((*dList)++, OS_PHYSICAL_TO_K0(renderList->vtx), renderList->vtxCount, 0);
        gSPPolygon((*dList)++, OS_PHYSICAL_TO_K0(renderList->tri), renderList->triCount, renderList->material != NULL);
        renderList = renderList->next;
    }
    gRenderNodeHead = NULL;
    gRenderNodeTail = NULL;
    gMateriallistHead = NULL;
    gMateriallistTail = NULL;
    
    gSorterPos = (u32) gSorterHeap + MATERIAL_SORT_BUFFER - sizeof(RenderNodeTrack);
}

/**
 * Find which segments can and should be rendered, then render their opaque geometry.
 * Render all objects inside visible segments then render the level's semitransparent geometry.
 * Afterwards, render particles.
*/
void render_level_geometry_and_objects(Gfx **dList) {
    s32 objCount;
    s32 numberOfSegments;
    s32 objFlags;
    s32 sp160;
    s32 i;
    s32 visibleFlags;
    u8 segmentIds[LEVEL_SEGMENT_MAX];
    u8 objectsVisible[LEVEL_SEGMENT_MAX];
    s32 visible;
    Object *obj;

    func_80012C30();

    sp160 = func_80014814(&objCount);

    if (gCurrentLevelModel->numberOfSegments > 1) {
        profiler_begin_timer2();
        numberOfSegments = 0;
        traverse_segments_bsp_tree(0, 0, gCurrentLevelModel->numberOfSegments - 1, segmentIds, &numberOfSegments);
        profiler_add(PP_BLOCKS, first2);
    } else {
        numberOfSegments = 1;
        segmentIds[0] = 0;
    }

    if (numberOfSegments > LEVEL_SEGMENT_MAX) {
        puppyprint_assert("numberOfSegments exceeds LEVEL_SEGMENT_MAX. (%d > %d)", numberOfSegments, LEVEL_SEGMENT_MAX);
    }

    bzero(&objectsVisible[1], numberOfSegments - 1);

    objectsVisible[0] = TRUE;

    if (gDrawLevelSegments) {
        for (i = 0; i < numberOfSegments; i++) {
            render_level_segment(dList, segmentIds[i], FALSE); // Render opaque segments
            objectsVisible[segmentIds[i] + 1] = TRUE;
        }
        pop_render_list_track(dList);
    }

    if (gCurrentLevelModel->numberOfSegments < 2) {
        objectsVisible[1] = TRUE;
    }

    reset_render_settings(dList);
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_OBJGFX] = osGetCount();
#endif
    gIsObjectRender = TRUE;
    func_80015348(sp160, objCount - 1);
    visibleFlags = OBJ_FLAGS_INVIS_PLAYER1 << (gActiveCameraID & 1);

    for (i = sp160; i < objCount; i++) {
        obj = get_object(i);
        visible = 255;
        objFlags = obj->segment.trans.flags;
        if (objFlags & OBJ_FLAGS_UNK_0080) {
            visible = 0;
        } else if (!(objFlags & OBJ_FLAGS_DEACTIVATED)) {
            visible = obj->segment.object.opacity;
        }
        if (objFlags & visibleFlags) {
            visible = 0;
        }
        if (obj != NULL && visible && (objectsVisible[obj->segment.object.segmentID + 1] || obj->segment.camera.unk34 > 1000.0f) && check_if_in_draw_range(obj)) {
            if (obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) {
                render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                continue;
            } else if (obj->shadow != NULL) {
                render_object_shadow(dList, obj, obj->shadow);
            }
            render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            if (obj->waterEffect != NULL && obj->segment.header->flags & 0x10) {
                render_object_water_effects(dList, obj, obj->waterEffect);
            }
        }
    }

    for (i = objCount - 1; i >= sp160; i--) {
        obj = get_object(i);
        objFlags = obj->segment.trans.flags;
        if (objFlags & visibleFlags) {
            visible = FALSE;
        } else {
            visible = TRUE;
        }
        if (obj != NULL && visible && objFlags & OBJ_FLAGS_UNK_0100 && objectsVisible[obj->segment.object.segmentID + 1] && check_if_in_draw_range(obj)) {
            if (obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) {
                render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                continue;
            } else if (obj->shadow != NULL) {
                render_object_shadow(dList, obj, obj->shadow);
            }
            render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            if ((obj->waterEffect != NULL) && (obj->segment.header->flags & 0x10)) {
                render_object_water_effects(dList, obj, obj->waterEffect);
            }
        }
    }
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[1][PP_OBJGFX] = osGetCount();
#endif
    gIsObjectRender = FALSE;
    if (gDrawLevelSegments) {
        for (i = numberOfSegments - 1; i >= 0; i--) {
            render_level_segment(dList, segmentIds[i], TRUE); // Render transparent segments
        }
        pop_render_list_track(dList);
    }
    if (gWaveBlockCount != 0) {
        profiler_begin_timer();
        func_800BA8E4(dList, &gSceneCurrMatrix, gActiveCameraID);
        profiler_add(PP_WAVES, first);
    }

    reset_render_settings(dList);
    load_and_set_texture_no_offset(dList, 0, RENDER_FOG_ACTIVE | RENDER_Z_COMPARE);
    func_80012C3C(dList);
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_PARTICLEGFX] = osGetCount();
#endif
    // Particles and FX
    for (i = objCount - 1; i >= sp160; i--) {
        obj = get_object(i);
        visible = 255;
        objFlags = obj->segment.trans.flags;
        if (objFlags & OBJ_FLAGS_UNK_0080) {
            visible = 1;
        } else if (!(objFlags & OBJ_FLAGS_DEACTIVATED)) {
            visible = obj->segment.object.opacity;
        }
        if (objFlags & visibleFlags) {
            visible = 0;
        }
        if (obj->behaviorId == BHV_RACER && visible >= 255) {
            visible = 0;
        }
        if (obj != NULL && visible < 255 && objectsVisible[obj->segment.object.segmentID + 1] && check_if_in_draw_range(obj)) {
            if (visible > 0) {
                if (obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) {
                    render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                    continue;
                } else if (obj->shadow != NULL) {
                    render_object_shadow(dList, obj, obj->shadow);
                }
                render_object(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                if ((obj->waterEffect != 0) && (obj->segment.header->flags & 0x10)) {
                    render_object_water_effects(dList, obj, obj->waterEffect);
                }
            }
            if (obj->behaviorId == BHV_RACER) {
                render_racer_shield(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                render_racer_magnet(dList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            }
        }
    }

    pop_render_list_track(dList);
    if (D_800DC924 && func_80027568()) {
        profiler_begin_timer3();
        gSceneCurrDisplayList = *dList;
        func_8002581C(segmentIds, numberOfSegments, gActiveCameraID);
        *dList = gSceneCurrDisplayList;
        profiler_add(PP_VOID, first3);
    }
    
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[1][PP_PARTICLEGFX] = osGetCount();
#endif
}

void find_material_list_track(RenderNodeTrack *node) {
    RenderListTrack *matList;
    profiler_begin_timer();

    if (gRenderNodeHead == NULL) {
        gRenderNodeHead = node;
        gSorterPos -= sizeof(RenderListTrack);
        matList = (RenderListTrack *) gSorterPos;
        gMateriallistHead = matList;
    } else {
        if (node->material) {
            RenderListTrack *list = gMateriallistHead;
            while (list) {
                if (list->entryHead->material == node->material) {
                    if (list->entryHead == gRenderNodeTail) {
                        gRenderNodeTail = node;
                    }
                    if (list->entryHead->next) {
                        list->entryHead->next->prev = node;
                    }
                    node->next = list->entryHead->next;
                    node->prev = list->entryHead;
                    list->entryHead->next = node;
                    list->entryHead = node;
                    profiler_add(PP_SORTING, first);
                    return;
                }
                list = list->next;
            }
        }
        gSorterPos -= sizeof(RenderListTrack);
        matList = (RenderListTrack *) gSorterPos;
        gMateriallistTail->next = matList;
        gRenderNodeTail->next = node;
    }
    node->next = NULL;
    matList->entryHead = node;
    matList->next = NULL;
    gMateriallistTail = matList;
    node->prev = gRenderNodeTail;
    gRenderNodeTail = node;
    profiler_add(PP_SORTING, first);
}

/**
 * Render a batch of level geometry.
 * Since opaque and transparent are done in two separate runs, it will skip over the other.
 * Has a special case for the flashing lights in Spaceport Alpha, too.
*/
void render_level_segment(Gfx **dList, s32 segmentId, s32 nonOpaque) {
    LevelModelSegment *segment;
    s32 i;
    TriangleBatchInfo *batchInfo;
    TextureHeader *texture;
    s32 renderBatch;
    s32 numberVertices;
    s32 numberTriangles;
    s32 vertices;
    s32 triangles;
    s32 color;
    s32 levelHeaderIndex;
    s32 texOffset;
    s32 sp78;
    s32 startPos;
    s32 endPos;
    s32 batchFlags;
    s32 textureFlags;
    numberVertices = (batchInfo + 1)->verticesOffset - batchInfo->verticesOffset;
    segment = &gCurrentLevelModel->segments[segmentId];
    if (gScenePlayerViewports < VIEWPORTS_COUNT_2_PLAYERS && !gConfig.perfMode) {
        sp78 = (nonOpaque && gWaveBlockCount) ? (func_800B9228(segment)) : (0);
    } else {
        sp78 = 0;
    }
    if (nonOpaque) {
        startPos = segment->numberofOpaqueBatches;
        endPos = segment->numberOfBatches;
    } else {
        startPos = 0;
        endPos = segment->numberofOpaqueBatches;
    }
    for (i = startPos; i < endPos; i++) {
        batchInfo = &segment->batches[i];
        textureFlags = RENDER_NONE;
        if (batchInfo->flags & BATCH_FLAGS_HIDDEN) {
            continue;
        }
        batchFlags = batchInfo->flags;
        renderBatch = 0;
        if (batchInfo->textureIndex == 0xFF) {
            texture = FALSE;
        } else {
            texture = gCurrentLevelModel->textures[batchInfo->textureIndex].texture;
            textureFlags = texture->flags;
        }
        batchFlags |= BATCH_FLAGS_UNK00000008 | BATCH_FLAGS_UNK00000002;
        
        if (!(batchFlags & BATCH_FLAGS_DEPTH_WRITE) && !(batchFlags & BATCH_FLAGS_RECEIVE_SHADOWS)) {
            if (gConfig.antiAliasing) {
                batchFlags &= ~RENDER_ANTI_ALIASING;
            } else {
                batchFlags |= RENDER_ANTI_ALIASING;
            }
        }
        if ((!(textureFlags & RENDER_SEMI_TRANSPARENT) && !(batchFlags & BATCH_FLAGS_WATER)) || batchFlags & BATCH_FLAGS_RECEIVE_SHADOWS) {
            renderBatch = TRUE;
        }
        if (nonOpaque) {
            renderBatch = (renderBatch + 1) & 1;
        }
        if (sp78 && batchFlags & BATCH_FLAGS_WATER) {
            renderBatch = FALSE;
        }
        if (!renderBatch) {
            continue;
        }
        numberTriangles = batchInfo->facesOffset;
        numberVertices = (batchInfo + 1)->verticesOffset - batchInfo->verticesOffset;
        numberTriangles = (batchInfo + 1)->facesOffset - numberTriangles;
        vertices = (s32) &segment->vertices[batchInfo->verticesOffset];
        triangles = (s32) &segment->triangles[batchInfo->facesOffset];
        texOffset = batchInfo->unk7 << 14;
        levelHeaderIndex = (batchFlags >> 28) & 7;
        if (levelHeaderIndex != (batchInfo->verticesOffset * 0)) {
            gDPSetEnvColor((*dList)++, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->red, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->green, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->blue, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->alpha);
        } else {
            gDPSetEnvColor((*dList)++, 255, 255, 255, 0);
        }
        if (batchFlags & BATCH_FLAGS_PULSATING_LIGHTS) {
            color = gCurrentLevelHeader2->pulseLightData->outColorValue;
            gDPSetPrimColor((*dList)++, 0, 0, color, color, color, color);
            load_blinking_lights_texture(dList, texture, batchFlags, texOffset);
            gSPVertexDKR((*dList)++, OS_PHYSICAL_TO_K0(vertices), numberVertices, 0);
            gSPPolygon((*dList)++, OS_PHYSICAL_TO_K0(triangles), numberTriangles, TRIN_ENABLE_TEXTURE);
            gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
        } else {
#ifdef PUPPYPRINT_DEBUG
            if (sShowAll) {
                load_and_set_texture(dList, texture, batchFlags, texOffset);
                batchFlags = TRUE;
                if (texture == NULL) {
                    batchFlags = FALSE;
                }
                gSPVertexDKR((*dList)++, OS_PHYSICAL_TO_K0(vertices), numberVertices, 0);
                gSPPolygon((*dList)++, OS_PHYSICAL_TO_K0(triangles), numberTriangles, batchFlags);
            } else {
#endif
                RenderNodeTrack *entry;
                gSorterPos -= sizeof(RenderNodeTrack);
                entry = (RenderNodeTrack *) gSorterPos;
                entry->material = texture;
                entry->flags = batchFlags;
                entry->texOffset = texOffset;
                entry->primAlpha = 255;
                entry->tri = (Triangle *) triangles;
                entry->vtx = (Vertex *) vertices;
                entry->triCount = numberTriangles;
                entry->vtxCount = numberVertices;
#ifdef PUPPYPRINT_DEBUG
                find_material_list_track(entry);
            }
#endif
        }
    }
}

/**
 * Parse through applicable segments in the level.
 * Uses function recursion to ensure adjacent segments remain next to each other in the list.
*/
void traverse_segments_bsp_tree(s32 nodeIndex, s32 segmentIndex, s32 segmentIndex2, u8 *segmentsOrder, s32 *segmentsOrderIndex) {
    BspTreeNode *curNode;
    s32 camValue;

    curNode = &gCurrentLevelModel->segmentsBspTree[nodeIndex];
    if (curNode->splitType == 0) {
        camValue = gSceneActiveCamera->trans.x_position; // Camera X
    } else if (curNode->splitType == 1) {
        camValue = gSceneActiveCamera->trans.y_position; // Camera Y
    } else {
        camValue = gSceneActiveCamera->trans.z_position; // Camera Z
    }

    if (camValue < curNode->splitValue) {
        if (curNode->leftNode != -1) {
            traverse_segments_bsp_tree(curNode->leftNode, segmentIndex, curNode->segmentIndex - 1, segmentsOrder, segmentsOrderIndex);
        } else {
            add_segment_to_order(segmentIndex, segmentsOrderIndex, segmentsOrder);
        }

        if (curNode->rightNode != -1) {
            traverse_segments_bsp_tree(curNode->rightNode, curNode->segmentIndex, segmentIndex2, segmentsOrder, segmentsOrderIndex);
        } else {
            add_segment_to_order(segmentIndex2, segmentsOrderIndex, segmentsOrder);
        }
    } else {
        if (curNode->rightNode != -1) {
            traverse_segments_bsp_tree(curNode->rightNode, curNode->segmentIndex, segmentIndex2, segmentsOrder, segmentsOrderIndex);
        } else {
            add_segment_to_order(segmentIndex2, segmentsOrderIndex, segmentsOrder);
        }

        if (curNode->leftNode != -1) {
            traverse_segments_bsp_tree(curNode->leftNode, segmentIndex, curNode->segmentIndex - 1, segmentsOrder, segmentsOrderIndex);
        } else {
            add_segment_to_order(segmentIndex, segmentsOrderIndex, segmentsOrder);
        }
    }
}

/**
 * Add this segment index to the specified segment ordering table if the segment in question is in view of the camera.
*/
void add_segment_to_order(s32 segmentIndex, s32 *segmentsOrderIndex, u8 *segmentsOrder) {
    u32 temp;
    if (segmentIndex < gCurrentLevelModel->numberOfSegments) {
        if (gSceneStartSegment != -1) {
            temp = gCurrentLevelModel->segmentsBitfields[gSceneStartSegment + (segmentIndex >> 3)];
            temp >>= segmentIndex & 7;
            temp &= 0xFF;
        } else {
            temp = 1;
        }
        if (temp & 1 && should_segment_be_visible(&gCurrentLevelModel->segmentsBoundingBoxes[segmentIndex])) {
            segmentsOrder[(*segmentsOrderIndex)++] = segmentIndex;
        }
    }
}

/**
 * Iterates through every existing segment to see which one the active camera is inside.
 * Uses mainly a two dimensional axis check here, instead of the function above.
 * Returns the segment currently inside.
 * Official Name: trackGetBlock
*/
s32 get_level_segment_index_from_position(f32 xPos, f32 yPos, f32 zPos) {
    LevelModelSegmentBoundingBox *bb;
    s32 i;
    s32 z = zPos;
    s32 x = xPos;
    s32 y = yPos;
    s32 minVal;
    s32 result;
    s32 heightDiff;

    if (gCurrentLevelModel == NULL) {
        return -1;
    }

    minVal = 1000000;
    result = -1;

    for (i = 0; i < gCurrentLevelModel->numberOfSegments; i++) {
        bb = &gCurrentLevelModel->segmentsBoundingBoxes[i];
        if (x < bb->x2 && bb->x1 < x && z < bb->z2 && bb->z1 < z) {
            heightDiff = (bb->y2 + bb->y1) >> 1; // Couldn't get / 2 to match, but >> 1 does.
            heightDiff = y - heightDiff;
            if (heightDiff < 0) {
                heightDiff = -heightDiff;
            }
            if (heightDiff < minVal) {
                minVal = heightDiff;
                result = i;
            }
        }
    }

    return result;
}

/**
 * Iterates through every existing segment to see which one the active camera is inside.
 * Uses mainly a two dimensional axis check here, instead of the function above.
 * Increments a counter based on if it's got a camera inside.
 * Because there's a tiny margin, multiple segments can be considered populated,
 * meaning that sometimes it will 2 instead of 1.
*/
s32 get_inside_segment_count_xz(s32 x, s32 z, s32 *arg2) {
    s32 i;
    s32 cnt = 0;
    LevelModelSegmentBoundingBox *bb;
    for (i = 0; i < gCurrentLevelModel->numberOfSegments; i++) {
        bb = gCurrentLevelModel->segmentsBoundingBoxes + i;
        if (x < bb->x2 + 4 && bb->x1 - 4 < x
         && z < bb->z2 + 4 && bb->z1 - 4 < z) {
            *arg2 = i;
            cnt++;
            arg2++;
        }
    }
    return cnt;
}

/**
 * Carbon copy of the above function, but takes into account the Y axis, too.
 * Official name: trackGetCubeBlockList
*/
s32 get_inside_segment_count_xyz(s32 *arg0, s16 xPos1, s16 yPos1, s16 zPos1, s16 xPos2, s16 yPos2, s16 zPos2) {
    s32 cnt;
    s32 i;
    LevelModelSegmentBoundingBox *bb;

    xPos1 -= 4;
    yPos1 -= 4;
    zPos1 -= 4;
    xPos2 += 4;
    yPos2 += 4;
    zPos2 += 4;

    i = 0;
    cnt = 0;

    while (i < gCurrentLevelModel->numberOfSegments) {
        bb = &gCurrentLevelModel->segmentsBoundingBoxes[i];
        if ((bb->x2 >= xPos1) && (xPos2 >= bb->x1) &&
            (bb->z2 >= zPos1) && (zPos2 >= bb->z1) &&
            (bb->y2 >= yPos1) && (yPos2 >= bb->y1)) {
            cnt++;
            *arg0++ = i;
        }
        i++;
    }
    return cnt;
}

/**
 * Returns this segment data.
*/
LevelModelSegment *get_segment(s32 segmentID) {
    if (segmentID < 0 || gCurrentLevelModel->numberOfSegments < segmentID)
        return NULL;

    return &gCurrentLevelModel->segments[segmentID];
}

/**
 * Returns the bounding box data of this segment.
 * Official name: trackBlockDim
*/
LevelModelSegmentBoundingBox *get_segment_bounding_box(s32 segmentID) {
    if (segmentID < 0 || gCurrentLevelModel->numberOfSegments < segmentID)
        return NULL;

    return &gCurrentLevelModel->segmentsBoundingBoxes[segmentID];
}

void func_8002A31C(void) {
    f32 ox1;
    f32 oy1;
    f32 oz1;
    f32 ox2;
    f32 oy2;
    f32 oz2;
    f32 ox3;
    f32 oy3;
    f32 oz3;
    Matrix *cameraMatrix;
    f32 inverseMagnitude;
    f32 x;
    f32 y;
    f32 z;
    s32 i;
    f32 w;

    cameraMatrix = get_projection_matrix_f32();
    for (i = 0; i < ARRAY_COUNT(D_8011D0F8);) {
        x = D_800DC8AC[i][0].x;
        y = D_800DC8AC[i][0].y;
        z = D_800DC8AC[i][0].z;
        ox1 = x;
        oy1 = y;
        oz1 = z;
        guMtxXFMF(*cameraMatrix, x, y, z, &ox1, &oy1, &oz1);
        x = D_800DC8AC[i][1].x;
        y = D_800DC8AC[i][1].y;
        z = D_800DC8AC[i][1].z;
        ox2 = x;
        oy2 = y;
        oz2 = z;
        guMtxXFMF(*cameraMatrix, x, y, z, &ox2, &oy2, &oz2);
        x = D_800DC8AC[i][2].x;
        y = D_800DC8AC[i][2].y;
        z = D_800DC8AC[i][2].z;
        ox3 = x;
        oy3 = y;
        oz3 = z;
        guMtxXFMF(*cameraMatrix, x, y, z, &ox3, &oy3, &oz3);
        x = ((oz2 - oz3) * oy1) + (oy2 * (oz3 - oz1)) + (oy3 * (oz1 - oz2));
        y = ((ox2 - ox3) * oz1) + (oz2 * (ox3 - ox1)) + (oz3 * (ox1 - ox2));
        z = ((oy2 - oy3) * ox1) + (ox2 * (oy3 - oy1)) + (ox3 * (oy1 - oy2));
        inverseMagnitude = (1.0f / sqrtf((x * x) + (y * y) + (z * z)));
        if (inverseMagnitude > 0.0f) {
            x *= inverseMagnitude;
            y *= inverseMagnitude;
            z *= inverseMagnitude;
        }
        w = -((ox1 * x) + (oy1 * y) + (oz1 * z));
        D_8011D0F8[i].x = x;
        D_8011D0F8[i].y = y;
        D_8011D0F8[i].z = z;
        D_8011D0F8[i].w = w;
        i++;
    }
}

/**
 * Takes a normalised (0-1) face direction of the active camera, then adds together a magnitude
 * to a total figure to determine whether or not a segment should be visible.
 * There's a large unused portion at the bottom writing to two vars, that are never later read.
*/
s32 should_segment_be_visible(LevelModelSegmentBoundingBox *bb) {
    s32 sp48;
    s32 i, j;
    f32 dirX, dirY, dirZ, dirW;
    
    for (j = 0; j < 3; j++) {
        dirX = D_8011D0F8[j].x;
        dirY = D_8011D0F8[j].y;
        dirZ = D_8011D0F8[j].z;
        dirW = D_8011D0F8[j].w;
        
        for (i = 0; i < 8; i++) {
            if (i & 1) {
                sp48 = bb->x1 * dirX;
            } else {
                sp48 = bb->x2 * dirX;
            }
            if (i & 2) {
                sp48 += bb->y1 * dirY;
            } else {
                sp48 += bb->y2 * dirY;
            }
            if (i & 4) {
                sp48 += bb->z1 * dirZ;
            } else {
                sp48 += bb->z2 * dirZ;
            }
            sp48 += dirW;
            if (sp48 > 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Get the draw distance of the object, then compare it to the active camera position.
 * At the edge of its view distance, it will set its alpha based on distance, giving it a fade in or out effect.
 * Objects in range return true, objects out of range return false.
*/
s32 check_if_in_draw_range(Object *obj) {
    f32 w;
    f32 y;
    f32 fadeDist;
    f32 z;
    f32 x;
    f32 viewDistance;
    s32 alpha;
    s32 i;
    Object_64 *obj64;
    f32 accum;
    s32 temp2;
    f32 dist;

    if (!(obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED)) {
        alpha = 255;
        viewDistance = obj->segment.header->drawDistance * obj->segment.header->drawDistance;
        if (obj->segment.header->drawDistance) {
            if (gScenePlayerViewports == 3) {
                viewDistance *= 0.5f;
            }

            dist = get_distance_to_active_camera(obj->segment.trans.x_position, obj->segment.trans.y_position, obj->segment.trans.z_position);
            
            if (viewDistance < dist) {
                return FALSE;
            }
            
            fadeDist = viewDistance * 0.8f;
            fadeDist *= fadeDist;
            if (fadeDist < dist) {
                temp2 = viewDistance - fadeDist;
                temp2 *= 2;
                if (temp2 > 0) {
                    fadeDist = dist - fadeDist;
                    alpha = ((f32) (1.0f - (fadeDist / temp2)) * 255.0f);
                }
                if (alpha == 0) {
                    alpha = 1;
                }
            }
        }
        switch (obj->behaviorId) {
            case BHV_RACER:
                obj64 = obj->unk64;
                obj->segment.object.opacity = ((obj64->racer.transparency + 1) * alpha) >> 8;
                break;
            case BHV_UNK_3A: //Ghost Object?
                obj64 = obj->unk64;
                obj->segment.object.opacity = obj64->racer.transparency;
                break;
            case BHV_ANIMATED_OBJECT: // Cutscene object?
            case BHV_CAMERA_ANIMATION:
            case BHV_CAR_ANIMATION:
            case BHV_CHARACTER_SELECT:
            case BHV_VEHICLE_ANIMATION: // Title screen actor
            case BHV_HIT_TESTER: // hittester
            case BHV_HIT_TESTER_2: // animated objects?
            case BHV_ANIMATED_OBJECT_2: // space ships
                obj64 = obj->unk64;
                obj->segment.object.opacity = obj64->effect_box.pad0[0x42];
                break;
            case BHV_PARK_WARDEN:
            case BHV_GOLDEN_BALLOON:
            case BHV_PARK_WARDEN_2: // GBParkwarden
                break;
            default:
                obj->segment.object.opacity = alpha;
                break;
        }
        for (i = 0; i < 3; i++) {
            x = D_8011D0F8[i].x;
            z = D_8011D0F8[i].z;
            w = D_8011D0F8[i].w;
            y = D_8011D0F8[i].y;
            accum = (x * obj->segment.trans.x_position) + (y * obj->segment.trans.y_position) + (z * obj->segment.trans.z_position) + w + obj->segment.camera.unk34;
            if (accum < 0.0f) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void func_8002ACC8(s32 arg0) {
    D_8011B0F4 = arg0;
}

s32 func_8002ACD4(f32 *arg0, f32 *arg1, f32 *arg2) {
    *arg0 = D_8011B0E4;
    *arg1 = D_8011B0E8;
    *arg2 = D_8011B0EC;
    return D_8011B0F0;
}

/**
 * Iterates through active waves on the track, then saves its rotation vector and height if found.
 * Returns zero if no waves are found, or if too high up.
 * Two types exist: calm, which have no means of displacement, and wavy, which do.
*/
s32 get_wave_properties(f32 yPos, f32 *waterHeight, Vec3f *rotation) {
    s32 var_a0;
    WaterProperties **wave;
    s32 i;
    s32 index;
    s32 len;
    f32 height;
    
    len = D_8011D308;
    if (rotation != NULL) {
        rotation->f[0] = 0.0f;
        rotation->f[2] = 0.0f;
        rotation->f[1] = 1.0f;
    }
    wave = gTrackWaves;
    for (var_a0 = i = 0; i < len; i++) {
        if (wave[i]->type == WATER_CALM || wave[i]->type == WATER_WAVY) {
            var_a0++;
        }
    }
    if (var_a0 == 0) {
        return 0;
    }
    wave = gTrackWaves;
    index = -1;
    for (i = 0; i < len; i++) {
        height = wave[i]->waveHeight;
        if (wave[i]->type == WATER_CALM || wave[i]->type == WATER_WAVY) {
            if (yPos < height + 25.0f && (wave[i]->rotY > 0.5f || var_a0 == 1)) {
                index = i;
            }
        } else
        if (index >= 0 && var_a0 >= 2 && yPos < height - 20.0f) {
            index = -1;
        }
    }
    if (index < 0) {
        return 0;
    }
    *waterHeight = gTrackWaves[index]->waveHeight;
    if (rotation != NULL) {
        rotation->f[0] = gTrackWaves[index]->rotX;
        rotation->f[1] = gTrackWaves[index]->rotY;
        rotation->f[2] = gTrackWaves[index]->rotZ;
    }
    return gTrackWaves[index]->type;
}

GLOBAL_ASM("asm/non_matchings/tracks/func_8002B0F4.s")

s32 func_8002B9BC(Object *obj, f32 *arg1, f32 *arg2, s32 arg3) {
    LevelModelSegment *seg;

    if (arg2 != NULL) {
        arg2[0] = 0.0f;
        arg2[2] = 0.0f;
        arg2[1] = 1.0f;
    }
    if ((obj->segment.object.segmentID < 0) || (obj->segment.object.segmentID >= gCurrentLevelModel->numberOfSegments)) {
        return FALSE;
    }
    seg = &gCurrentLevelModel->segments[obj->segment.object.segmentID];
    if ((seg->hasWaves != 0) && (gWaveBlockCount != 0) && (arg3 == 1)) {
        profiler_begin_timer();
        *arg1 = func_800BB2F4(obj->segment.object.segmentID, obj->segment.trans.x_position, obj->segment.trans.z_position, arg2);
        profiler_add(PP_WAVES, first);
        return TRUE;
    } else {
        *arg1 = seg->unk38;
        return TRUE;
    }
}

GLOBAL_ASM("asm/non_matchings/tracks/func_8002BAB0.s")

#ifdef NON_MATCHING
// generate_track
// Loads a level track from the index in the models table.
// Has regalloc issues.
void func_8002C0C4(s32 modelId) {
    s32 i, j, k;
    s32 temp_s4;
    s32 temp;
    LevelModel *mdl;
    s32 levelSize = 0;
    
    set_texture_colour_tag(MEMP_LEVEL_TEXTURES);
    D_8011D370 = allocate_from_main_pool_safe(0x7D0, MEMP_LEVEL_MODELS);
    D_8011D374 = allocate_from_main_pool_safe(0x1F4, MEMP_LEVEL_MODELS);
    D_8011D378 = 0;
    gAssetColourTag = MEMP_HEADERS;
    gLevelModelTable = (s32*) load_asset_section_from_rom(ASSET_LEVEL_MODELS_TABLE);
    gAssetColourTag = COLOUR_TAG_GREY;
    alloc_ghost_pool();
    
    for(i = 0; gLevelModelTable[i] != -1; i++);
    i--;
    if (modelId >= i) {
        modelId = 0;
    }
    
    //offset = gLevelModelTable[modelId];
    temp_s4 = gLevelModelTable[modelId + 1] - gLevelModelTable[modelId];

    levelSize = get_asset_uncompressed_size(ASSET_LEVEL_MODELS, gLevelModelTable[modelId]);
    levelSize = MIN(levelSize * 3, LEVEL_MODEL_MAX_SIZE);
    gTrackModelHeap = allocate_from_main_pool_safe(LEVEL_MODEL_MAX_SIZE, MEMP_LEVEL_MODELS);
    gCurrentLevelModel = gTrackModelHeap;

    // temp = compressedRamAddr
    temp = (s32) gCurrentLevelModel;
    temp +=  ((levelSize) - temp_s4);
    temp -= ((s32)temp % 16); // Align to 16-byte boundary.
    
    load_asset_to_address(ASSET_LEVEL_MODELS, temp, gLevelModelTable[modelId], temp_s4);
    gzip_inflate((u8*) temp, (u8*) gCurrentLevelModel);
    free_from_memory_pool(gLevelModelTable); // Done with the level models table, so free it.
    
    mdl = gCurrentLevelModel;
    
    LOCAL_OFFSET_TO_RAM_ADDRESS(TextureInfo *, gCurrentLevelModel->textures);
    LOCAL_OFFSET_TO_RAM_ADDRESS(LevelModelSegment *, gCurrentLevelModel->segments);
    LOCAL_OFFSET_TO_RAM_ADDRESS(LevelModelSegmentBoundingBox *, gCurrentLevelModel->segmentsBoundingBoxes);
    LOCAL_OFFSET_TO_RAM_ADDRESS(u8 *, gCurrentLevelModel->unkC);
    LOCAL_OFFSET_TO_RAM_ADDRESS(u8 *, gCurrentLevelModel->segmentsBitfields);
    LOCAL_OFFSET_TO_RAM_ADDRESS(BspTreeNode *, gCurrentLevelModel->segmentsBspTree);
    
    for(k = 0; k < gCurrentLevelModel->numberOfSegments; k++) {
        LOCAL_OFFSET_TO_RAM_ADDRESS(Vertex *, gCurrentLevelModel->segments[k].vertices);
        LOCAL_OFFSET_TO_RAM_ADDRESS(Triangle *, gCurrentLevelModel->segments[k].triangles);
        LOCAL_OFFSET_TO_RAM_ADDRESS(TriangleBatchInfo *, gCurrentLevelModel->segments[k].batches);
        LOCAL_OFFSET_TO_RAM_ADDRESS(u8 *, gCurrentLevelModel->segments[k].unk14);
    }
    for(k = 0; k < gCurrentLevelModel->numberOfTextures; k++) {
        gCurrentLevelModel->textures[k].texture = load_texture(((s32)gCurrentLevelModel->textures[k].texture) | 0x8000);
    }
    j = (s32)gCurrentLevelModel + gCurrentLevelModel->modelSize;
    for(k = 0; k < gCurrentLevelModel->numberOfSegments; k++) {
        gCurrentLevelModel->segments[k].unk10 = (s16 *) j;
        j = (s32) align16(((u8 *) (gCurrentLevelModel->segments[k].numberOfTriangles * 2)) + j);
        gCurrentLevelModel->segments[k].unk18 = (s16 *) j;
        j = (s32) &((u8*)j)[func_8002CC30(&gCurrentLevelModel->segments[k])];
        func_8002C954(&gCurrentLevelModel->segments[k], &gCurrentLevelModel->segmentsBoundingBoxes[k], k);
        gCurrentLevelModel->segments[k].unk30 = 0;
        gCurrentLevelModel->segments[k].unk34 = (s16 *) j;
        func_8002C71C(&gCurrentLevelModel->segments[k]);
        j = (s32) align16(((u8 *) (gCurrentLevelModel->segments[k].unk32 * 2)) + j);
    }
    temp_s4 = j - (s32)gCurrentLevelModel;
    set_free_queue_state(0);
    free_from_memory_pool(gTrackModelHeap);
    allocate_at_address_in_main_pool(temp_s4, (u8* ) gTrackModelHeap, MEMP_LEVEL_MODELS);
    set_free_queue_state(2);
    minimap_init(gCurrentLevelModel);

    for(i = 0; i < gCurrentLevelModel->numberOfSegments; i++) {
        for(temp_s4 = 0; temp_s4 < gCurrentLevelModel->segments[i].numberOfBatches; temp_s4++) {
            for(k = gCurrentLevelModel->segments[i].batches[temp_s4].verticesOffset; 
                k < gCurrentLevelModel->segments[i].batches[temp_s4+1].verticesOffset; 
                k++) {
                // Why do this? Why not just set the vertex colors in the model itself?
                if(gCurrentLevelModel->segments[i].vertices[k].r == 1 && gCurrentLevelModel->segments[i].vertices[k].g == 1) { 
                    gCurrentLevelModel->segments[i].vertices[k].a = gCurrentLevelModel->segments[i].vertices[k].b;
                    gCurrentLevelModel->segments[i].vertices[k].r = 0x80;
                    gCurrentLevelModel->segments[i].vertices[k].g = 0x80;
                    gCurrentLevelModel->segments[i].vertices[k].b = 0x80;
                    gCurrentLevelModel->segments[i].batches[temp_s4].flags |= 0x08000000;
                }
            }
        }
    }
    set_texture_colour_tag(MEMP_MISC_TEXTURES);
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_8002C0C4.s")
#endif

void func_8002C71C(LevelModelSegment *segment) {
    s32 curVertY;
    s32 numVerts;
    s32 i;

    segment->unk38 = -10000;
    numVerts = 0;
    for (i = 0; i < segment->numberOfBatches; i++) {
        if (segment->batches[i].flags & 0x2000) {
            segment->unk34[numVerts++] = i;
            curVertY = segment->vertices[segment->batches[i].verticesOffset].y;
            if (segment->unk38 == -10000 || segment->unk38 < curVertY) {
                segment->unk38 = curVertY;
            }
        }
    }
    segment->unk32 = numVerts;
}

/**
 * Returns the current loaded level geometry
 * Official Name: trackGetTrack
*/
LevelModel *get_current_level_model(void) {
    return gCurrentLevelModel;
}

/**
 * Frees and unloads level data from RAM.
*/
void free_track(void) {
    s32 i;

    func_8000B290();
    if (gWaveBlockCount != 0) {
        free_waves();
    }
    for (i = 0; i < gCurrentLevelModel->numberOfTextures; i++) {
        free_texture(gCurrentLevelModel->textures[i].texture);
    }
    free_from_memory_pool(gTrackModelHeap);
    free_from_memory_pool(D_8011D370);
    free_from_memory_pool(D_8011D374);
    free_sprite((Sprite *) gCurrentLevelModel->minimapSpriteIndex);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        free_from_memory_pool(gShadowHeapTextures[i]);
        free_from_memory_pool(gShadowHeapTris[i]);
        free_from_memory_pool(gShadowHeapVerts[i]);
    }
    func_800257D0();
    if (gSkydomeSegment != NULL) {
        free_object(gSkydomeSegment);
        gParticlePtrList_flush();
    }
    free_ghost_pool();
    free_all_objects();
    gMakeBG = TRUE;
    gCurrentLevelModel = NULL;
}

void func_8002C954(LevelModelSegment *segment, LevelModelSegmentBoundingBox *bbox, UNUSED s32 arg2) {
    Vertex *vert;
    s16 boxDelta;
    s32 vertZ;
    s32 vertX;
    s16 boxMin;
    s16 bit;
    s16 maxX;
    s32 j;
    s16 minX;
    s16 val;
    s16 maxZ;
    s16 minZ;
    s16 k;
    s16 boxMax;
    s32 i;
    s32 l;
    s32 startTri;
    s32 endTri;
    s32 vertsOffset;

    for (i = 0; i < segment->numberOfBatches; i++) {
        startTri = segment->batches[i].facesOffset;
        endTri = segment->batches[i+1].facesOffset;
        vertsOffset = segment->batches[i].verticesOffset;
        for (j = startTri; j < endTri; j++) {
            if (segment->triangles[j].flags & 0x80) {
                segment->unk10[j] = 0;
            } else {
                maxX = -32000;
                maxZ = -32000;
                minZ = 32000;
                minX = 32000;
                
                for (l = 0; l < 3; l++) {
                    vert = &segment->vertices[segment->triangles[j].verticesArray[l + 1] + vertsOffset];
                    k = vert->x; vertX = k; // This is probably fake, but it matches.
                    vertZ = vert->z;
                    if (maxX < vertX) {
                        maxX = vertX;
                    }
                    if (vertX < minX) {
                        minX = vertX;
                    }
                    if (maxZ < vertZ) {
                        maxZ = vertZ;
                    }
                    if (vertZ < minZ) {
                        minZ = vertZ;
                    }
                }
                boxDelta = ((bbox->x2 - bbox->x1) >> 3) + 1;
                bit = 1;
                boxMax = boxDelta + bbox->x1;
                boxMin = bbox->x1;
                val = 0;
                for (k = 0; k < 8; k++) {
                    if (boxMax >= minX && maxX >= boxMin) {
                        val |= bit;
                    }
                    boxMax += boxDelta;
                    boxMin += boxDelta;
                    bit <<= 1;
                }
                boxDelta = ((bbox->z2 - bbox->z1) >> 3) + 1;
                boxMax = boxDelta + bbox->z1;
                boxMin = bbox->z1;
                for (k = 0; k < 8; k++) {
                    if (boxMax >= minZ && maxZ >= boxMin) {
                        val |= bit;
                    }
                    boxMax += boxDelta;
                    boxMin += boxDelta;
                    bit <<= 1;
                }
                segment->unk10[j] = val;
            }
        }
    } 
}

GLOBAL_ASM("asm/non_matchings/tracks/func_8002CC30.s")

typedef struct unk8002D30C_a0 {
    u8 pad00[0x04];
    struct unk8002D30C_a0 *unk04;
    struct unk8002D30C_a0 *unk08;
} unk8002D30C_a0;

void trackMakeAbsolute(unk8002D30C_a0 *arg0, s32 arg1) {
    while(1) {
        if(!arg0) {
            return;
        }
        if (arg0->unk04) {
            arg0->unk04 = (unk8002D30C_a0 *)((s32)arg0->unk04 + arg1);
        }
        if (arg0->unk08) {
            arg0->unk08 = (unk8002D30C_a0 *)((s32)arg0->unk08 + arg1);
        }

        trackMakeAbsolute(arg0->unk04, arg1);
        arg0 = arg0->unk08;
    }
}

/**
 * Render the shadow of an object on the ground as a decal.
 * Can subdivide itself to wrap around the terrain properly, as the N64 lacks stencil buffering.
 */
void render_object_shadow(Gfx **dList, Object *obj, ShadowData *shadow) {
    s32 i;
    s32 numVerts;
    s32 numTris;
    Vertex *vtx;
    Triangle *tri;
    s32 flags;
    s32 offsetY;
    s32 offsetX;
    s32 alpha;
    profiler_begin_timer();
    
    if (obj->segment.header->shadowGroup) {
        if (shadow->meshStart != -1) {
            D_8011B0CC = gShadowHeapFlip;
            if (obj->segment.header->shadowGroup == SHADOW_SCENERY) {
                D_8011B0CC += 2;
            }
            i = shadow->meshStart;
            gCurrentShadowTexture = gShadowHeapTextures[D_8011B0CC];
            gCurrentShadowTris = gShadowHeapTris[D_8011B0CC];
            gCurrentShadowVerts = gShadowHeapVerts[D_8011B0CC];
            alpha = gCurrentShadowVerts[gCurrentShadowTexture[i].yOffset].a;
            flags = RENDER_FOG_ACTIVE | RENDER_Z_COMPARE;
            if (alpha == 0 || obj->segment.object.opacity == 0) {
                profiler_add(PP_SHADOW, first);
                return;
            } else if (alpha != 255 || obj->segment.object.opacity != 255) {
                flags = RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE;
                alpha = (obj->segment.object.opacity * alpha) >> 8;
                gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, alpha);
            }
            while (i < shadow->meshEnd) {
                // I hope we can clean this part up.
                offsetX = gCurrentShadowTexture[i].xOffset;
                offsetY = gCurrentShadowTexture[i].yOffset;
                numTris = gCurrentShadowTexture[i+1].xOffset - offsetX;
                numVerts = gCurrentShadowTexture[i+1].yOffset - offsetY;
                tri = (Triangle *) &gCurrentShadowTris[offsetX];
                vtx = (Vertex *) &gCurrentShadowVerts[offsetY];
#ifdef PUPPYPRINT_DEBUG
                if (sShowAll) {
                    load_and_set_texture_no_offset(dList, gCurrentShadowTexture[i].texture, flags);
                    gSPVertexDKR((*dList)++, OS_K0_TO_PHYSICAL(vtx), numVerts, 0);
                    gSPPolygon((*dList)++, OS_K0_TO_PHYSICAL(tri), numTris, 1);
                } else {
#endif
                    RenderNodeTrack *entry;
                    gSorterPos -= sizeof(RenderNodeTrack);
                    entry = (RenderNodeTrack *) gSorterPos;
                    entry->material = gCurrentShadowTexture[i].texture;
                    entry->flags = flags;
                    entry->primAlpha = alpha;
                    entry->texOffset = 0;
                    entry->tri = (Triangle *) tri;
                    entry->vtx = (Vertex *) vtx;
                    entry->triCount = numTris;
                    entry->vtxCount = numVerts;
                    find_material_list_track(entry);
#ifdef PUPPYPRINT_DEBUG
                }
#endif
                i++;
            }
            
            if (flags != RENDER_Z_COMPARE) {
                gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
            }
        }
    }
    profiler_add(PP_SHADOW, first);
}

/**
 * Used only by cars, render a texture on the surface of the water where the car is
 * to give the wave effect. Works almost identically to shadows, since water can be wavy.
 */
void render_object_water_effects(Gfx **dList, Object *obj, WaterEffect *effect) {
    s32 i;
    s32 numVerts;
    s32 numTris;
    Vertex *vtx;
    Triangle *tri;
    s32 flags;
    profiler_begin_timer();

    if (obj->segment.header->waterEffectGroup) {
        if (effect->meshStart != -1) {
            gShadowIndex = gShadowHeapFlip;
            i = effect->meshStart;
            if (obj->segment.header->waterEffectGroup == SHADOW_SCENERY) {
                gShadowIndex = gShadowHeapFlip;
                gShadowIndex += 2;
                if (get_distance_to_active_camera(obj->segment.trans.x_position, obj->segment.trans.y_position, obj->segment.trans.z_position) > 768.0f * 768.0f) {
                    profiler_add(PP_SHADOW, first);
                    return;
                }
            }
            flags = RENDER_FOG_ACTIVE | RENDER_Z_COMPARE;
            gCurrentShadowTexture = gShadowHeapTextures[gShadowIndex];
            gCurrentShadowTris = gShadowHeapTris[gShadowIndex];
            gCurrentShadowVerts = gShadowHeapVerts[gShadowIndex];
            while (i < effect->meshEnd) {
                numTris = gCurrentShadowTexture[i + 1].xOffset - gCurrentShadowTexture[i].xOffset;
                numVerts = gCurrentShadowTexture[i + 1].yOffset - gCurrentShadowTexture[i].yOffset;
                tri = &gCurrentShadowTris[gCurrentShadowTexture[i].xOffset];
                vtx = &gCurrentShadowVerts[gCurrentShadowTexture[i].yOffset];
                load_and_set_texture_no_offset(dList, gCurrentShadowTexture[i].texture, flags);
                gSPVertexDKR((*dList)++, OS_K0_TO_PHYSICAL(vtx), numVerts, 0);
                gSPPolygon((*dList)++, OS_K0_TO_PHYSICAL(tri), numTris, 1);
                i++;
            }
        }
    }
    profiler_add(PP_SHADOW, first);
}

/**
 * Updates shadow and water effect properties for each relevant object in the scene.
 * The first argument decides whether to update shadows for static objects or moving objects.
 * Checks how many players there are before deciding whether to cast a shadow for that object.
*/
void update_shadows(s32 group, s32 waterGroup, s32 updateRate) {
    s32 objIndex;
    s32 objectCount;
    Object *obj;
    ObjectHeader *objHeader;
    f32 dist;
    s32 radius;
    s32 radius2;
    s32 numViewports;
    Object **objects;
    s32 skipShading;
    TextureHeader* waterTex;
    ShadowData *shadow;
    WaterEffect *waterEffect;
    s32 playerIndex;
    profiler_begin_timer();

    D_8011B0CC = gShadowHeapFlip;
    if (group == SHADOW_SCENERY) {
        D_8011B0CC += 2;
    }
    gCurrentShadowTris = (Triangle *) gShadowHeapTris[D_8011B0CC];
    gCurrentShadowVerts = (Vertex *) gShadowHeapVerts[D_8011B0CC];
    gCurrentShadowTexture = (DrawTexture *) gShadowHeapTextures[D_8011B0CC];
    D_8011D364 = 0;
    D_8011D368 = 0;
    D_8011D36C = 0;
    numViewports = gNumberOfViewports;
    objects = objGetObjList(&objIndex, &objectCount);
    while (objIndex < objectCount) {
        obj = objects[objIndex];
        objHeader = obj->segment.header;
        waterEffect = obj->waterEffect;
        shadow = obj->shadow;
        objIndex += 1;
        if ((obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED)) {
            continue;
        }
        if (shadow != NULL && shadow->scale > 0.0f && group == objHeader->shadowGroup) {
            shadow->meshStart = -1;
        } 
        if (obj->segment.trans.flags & OBJ_FLAGS_INVISIBLE) {
            shadow = NULL;
        }
        if ((shadow != NULL && objHeader->shadowGroup == SHADOW_ACTORS) || (waterEffect != NULL && objHeader->waterEffectGroup == SHADOW_ACTORS)) {
            dist = get_distance_to_active_camera(obj->segment.trans.x_position, obj->segment.trans.y_position, obj->segment.trans.z_position);
        } else {
            dist = 0;
        }
        if (shadow != NULL && shadow->scale > 0.0f && group == objHeader->shadowGroup) {
            gShadowOpacity = 1.0f;
            shadow->meshStart = -1;
            skipShading = FALSE;
            if (objHeader->shadowGroup == SHADOW_ACTORS && numViewports > ONE_PLAYER && numViewports <= FOUR_PLAYERS) {
                if (obj->behaviorId == BHV_RACER) {
                    playerIndex = obj->unk64->racer.playerIndex;
                    if (playerIndex != PLAYER_COMPUTER) {
                        func_8002E234(obj, FALSE);
                        skipShading = TRUE;
                    }
                } else if (obj->behaviorId == BHV_WEAPON) {
                    func_8002E234(obj, FALSE);
                    skipShading = TRUE;
                }
            } else {
                radius = objHeader->unk4A * objHeader->unk4A;
                radius2 = objHeader->unk4C * objHeader->unk4C;
                if (dist < radius) {
                    if (radius2 < dist) {
                        gShadowOpacity = (radius - dist) / ( radius - radius2);
                    }
                    func_8002E234(obj, FALSE);
                    skipShading = TRUE;
                }
            }
            if (skipShading == FALSE && obj->shading != NULL) {
                func_8002DE30(obj);
            }
        }
        if (waterEffect != NULL && waterEffect->scale > 0.0f && waterGroup == objHeader->waterEffectGroup) {
            waterEffect->meshStart = -1;
            gShadowOpacity = 1.0f;
            waterTex = waterEffect->texture;
            if (waterTex != NULL && updateRate && waterTex->numOfTextures != 0x100) {
                waterEffect->textureFrame += waterEffect->animationSpeed;
                while (waterTex->numOfTextures < waterEffect->textureFrame) {
                    waterEffect->textureFrame -= waterTex->numOfTextures;
                } 
            }
                
            if (objHeader->shadowGroup == SHADOW_ACTORS && numViewports > ONE_PLAYER) {
                if (obj->behaviorId == BHV_RACER) {
                    playerIndex = obj->unk64->racer.playerIndex;
                    if (playerIndex != PLAYER_COMPUTER) {
                        func_8002E234(obj, TRUE);
                    }
                } else {
                    radius = objHeader->unk4A * objHeader->unk4A;
                    radius2 = objHeader->unk4C * objHeader->unk4C;
                    if (dist < radius) {
                        if (radius2 < dist) {
                            gShadowOpacity = (radius - dist) / (radius - radius2);
                        }
                        func_8002E234(obj, TRUE);
                    }
                    func_8002E234(obj, TRUE);
                }
            }
        }
    }
    gCurrentShadowTexture[D_8011D364].xOffset = D_8011D368;
    gCurrentShadowTexture[D_8011D364].yOffset = D_8011D36C;
    profiler_add(PP_SHADOW, first);
}

void func_8002DE30(Object *obj) {
    s32 sp94;
    s32 sp90;
    s32 blockId;
    s32 var_t3;
    s32 k;
    u32 batchFlags;
    s32 foundResult;
    s32 i;
    LevelModelSegment *block;
    Triangle *triangle;
    Vertex *vertices;
    s32 minYPos;
    s32 maxYPos;
    s32 j;

    sp94 = (s32) obj->segment.trans.y_position + obj->segment.header->unk44;
    sp90 = (s32) obj->segment.trans.y_position + obj->segment.header->unk42;
    blockId = obj->segment.object.segmentID;
    foundResult = FALSE;
    if (blockId != -1) {
        var_t3 = func_800314DC(
            &gCurrentLevelModel->segmentsBoundingBoxes[blockId], 
            obj->segment.trans.x_position - 16.0f, obj->segment.trans.z_position - 16.0f, 
            obj->segment.trans.x_position + 16.0f, obj->segment.trans.z_position + 16.0f
        );
        block = &gCurrentLevelModel->segments[blockId]; 
        for(i = 0; i < block->numberOfBatches && !foundResult; i++) {
            if (!(block->batches[i].flags & (BATCH_FLAGS_HIDDEN | BATCH_FLAGS_RECEIVE_SHADOWS | BATCH_FLAGS_WATER | BATCH_FLAGS_FORCE_NO_SHADOWS))) {
                batchFlags = (block->batches[i].flags >> 19) & (BATCH_FLAGS_UNK00000001 | BATCH_FLAGS_UNK00000002 | BATCH_FLAGS_UNK00000004);
                vertices = &block->vertices[block->batches[i].verticesOffset];
                for(j = block->batches[i].facesOffset; j < block->batches[i+1].facesOffset && !foundResult; j++) {
                    blockId = block->unk10[j] & var_t3;
                    if(blockId){}
                    if (((block->unk10[j] & var_t3) & 0xFF) && ((block->unk10[j] & var_t3) & 0xFF00)) {
                        triangle = &block->triangles[j];
                        minYPos = vertices[triangle->verticesArray[1]].y;
                        maxYPos = minYPos;
                        for(k = 1; k < 3; k++) {
                            if (vertices[triangle->verticesArray[k + 1]].y < minYPos) {
                                minYPos = vertices[triangle->verticesArray[k + 1]].y;
                            } else if (maxYPos < vertices[triangle->verticesArray[k + 1]].y) {
                                maxYPos = vertices[triangle->verticesArray[k + 1]].y;
                            }
                        }
                        if (maxYPos >= sp90 && sp94 >= minYPos) {
                            if (point_triangle_2d_xz_intersection(
                                    obj->segment.trans.x_position,
                                    obj->segment.trans.z_position,
                                    &vertices[triangle->verticesArray[1]].x, 
                                    &vertices[triangle->verticesArray[2]].x,
                                    &vertices[triangle->verticesArray[3]].x)
                            ) {
                                foundResult = TRUE;
                                obj->shading->unk0 += (((1.0f - D_800DC884[batchFlags]) - obj->shading->unk0) * 0.2f);
                            }
                        }
                    }
                }
            }
            
        }
    }
}

// Generate shadow
#ifdef NON_EQUIVALENT
void func_8002E234(Object *obj, s32 bool) {
    s32 *inSegs;
    f32 xPos;
    f32 zPos;
    s32 *new_var;
    f32 character_scale;
    f32 var_f2;
    s32 yPos;
    s32 cheats;
    s32 i;
    s32 test;
    f32 temp;
    s32 segs;

    yPos = obj->segment.trans.y_position;
    character_scale = 1.0f;
    if (obj->behaviorId == BHV_RACER) {
        cheats = get_filtered_cheats();
        if (cheats & CHEAT_BIG_CHARACTERS) {
            character_scale = 1.4f;
        } else if (cheats & CHEAT_SMALL_CHARACTERS) {
            character_scale = 0.714f;
        }
    }
    
    D_8011D0C4 = obj;
    D_8011D0C8 = 2.0f;
    if (D_8011D0C4) { } //fakematch

    if (bool) {
        D_8011D0B8 = 0;
        obj->waterEffect->meshStart = D_8011D364;
        D_8011D0C0 = set_animated_texture_header(obj->waterEffect->texture, obj->waterEffect->textureFrame << 8);
        D_8011D0CE = obj->segment.header->unk48 + yPos;
        D_8011D0CC = obj->segment.header->unk46 + yPos;
        if ((gWaveBlockCount == 0) || ((get_viewport_count() <= 0))) {
            D_8011D0C8 = 0;
        }
        D_8011D0D8 = (obj->waterEffect->scale * character_scale);
        D_8011D0DC = D_8011D0D8 * 10.0f;
        D_8011D0E0 = D_8011D0D8 * 10.0f;
        D_8011D0F0 = -1.0f;
    } else {
        obj->shadow->meshStart = D_8011D364;
        D_8011D0C0 = obj->shadow->texture;
        D_8011D0CE = obj->segment.header->unk44 + yPos;
        D_8011D0CC = obj->segment.header->unk42 + yPos;
        if (obj->behaviorId != BHV_RACER) {
            var_f2 = obj->segment.object.distanceToCamera;
            if (var_f2 < 0.0) {
                var_f2 = -var_f2;
            }
            var_f2 -= 512.0;
            if (var_f2 < 0.0) {
                var_f2 = 0.0;
            }
            if (var_f2 > 1024.0) {
                var_f2 = 1024.0;
            }
            D_8011D0C8 += (var_f2 * 0.005f);
        }
        D_8011D0D8 = (obj->shadow->scale * character_scale);
        D_8011D0DC = D_8011D0D8 * 10.0f;
        D_8011D0E0 = D_8011D0D8 * 10.0f;
        D_8011D0E4 = 4.0f * D_8011D0DC * D_8011D0E0;
        D_8011D0F0 = (obj->segment.header->unk42 * 0.125f);
        if (D_8011D0F0 < 0.0f) {
            D_8011D0F0 = -D_8011D0F0;
        }
        D_8011D0F4 = (7.0f * D_8011D0F0);
        if (1) { } if (1) { } if (1) { } if (1) { } if (1) { } if (1) { }
        D_8011D0D0 = -0x8000;
    }
    D_8011D0D8 = 144.0f / D_8011D0D8;
    xPos = obj->segment.trans.x_position;
    zPos = obj->segment.trans.z_position;
    segs = get_inside_segment_count_xyz(inSegs, (xPos - D_8011D0DC), D_8011D0CC, (zPos - D_8011D0E0), (xPos + D_8011D0DC), D_8011D0CE, (zPos + D_8011D0E0));
    D_8011C230 = 0;
    D_8011B118 = 0;
    for (i = 0; i < ARRAY_COUNT(D_8011B320); i++) {
        D_8011B320[i] = 0;
    }
    new_var = inSegs; //fake?
    D_8011D0E8 = -1;
    D_8011D0EC = -1;
    for (i = 0; i < segs; i++) {
        if (new_var[i] >= 0) {
            if (bool && (gCurrentLevelModel->segments[inSegs[i]].hasWaves != 0) && (gWaveBlockCount != 0)) {
                func_8002EEEC();
            } else {
                test = func_800314DC(
                    &gCurrentLevelModel->segmentsBoundingBoxes[new_var[i]], 
                    (obj->segment.trans.x_position - D_8011D0DC), //x1
                    (obj->segment.trans.z_position - D_8011D0E0), //z1
                    (obj->segment.trans.x_position + D_8011D0DC), //x2
                    (obj->segment.trans.z_position + D_8011D0E0)  //z2
                );
                func_8002E904(&gCurrentLevelModel->segments[new_var[i]], test, bool);
            }
        }
    }
    if (D_8011C230 > 0) {
        if ((obj->shading != NULL) && !bool) {
            obj->shading->unk0 = func_8002FA64();
        }
        func_8002F2AC();
        func_8002F440();
    }
    if (!bool) {
        obj->shadow->meshEnd = D_8011D364;
    }
    else {
        obj->waterEffect->meshEnd = D_8011D364;
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_8002E234.s")
#endif

GLOBAL_ASM("asm/non_matchings/tracks/func_8002E904.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002EEEC.s")

#ifdef NON_EQUIVALENT
void func_8002F2AC(void) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f16;
    f32 temp_f8;
    unk8011B120_unkC *var_v0;
    s32 i, j;

    for (i = 0; i < D_8011B118; i++) {
        var_v0 = D_8011B120[i].unkC;
        temp_f16 = D_8011B120[i].x * var_v0->unk0;
        temp_f10 = var_v0->unkC;
        temp_f8 = var_v0->unk4;
        temp_f12 = D_8011B120[i].z * var_v0->unk8;
        D_8011B120[i].y = (f32) (-(temp_f16 + temp_f12 + temp_f10) / temp_f8);
    }

    for (i = 0; D_8011B320[i] > 0 ; i++) {
        for (j = 0; j < D_8011B320[i]; j++) {
            var_v0 = D_8011B330[i].unkC;
            temp_f16 = D_8011B330[i].x * var_v0->unk0;
            temp_f10 = var_v0->unkC;
            temp_f8 = var_v0->unk4;
            temp_f12 = D_8011B330[i].z * var_v0->unk8;
            D_8011B330[i].z = (f32) (-(temp_f16 + temp_f12 + temp_f10) / temp_f8);            
        }
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_8002F2AC.s")
#endif

GLOBAL_ASM("asm/non_matchings/tracks/func_8002F440.s")

//Transition points between different lighting levels, used by certain objects
f32 func_8002FA64(void) {
    f32 var_f2;
    f32 x0, z0, x1, z1, x2, z2;
    s32 temp_t5;
    s32 var_s2;
    s32 i;

    var_f2 = 0.0f;
    if (D_8011C230 > 0) {
        if (D_8011D0EC != 0) {
            if (D_8011D0E8 > 0) {
                var_f2 = D_800DC884[D_8011D0E8] * D_8011D0E4;
            }
        } else {
            for (i = 0; i < D_8011C230; i++) {
                if (D_8011C238[i].unkA > 0) {
                    temp_t5 = D_8011C238[i].unk1;
                    if (temp_t5 & 1) {
                        x0 = D_8011B330[D_8011C238[i].unk2[0]].x;
                        z0 = D_8011B330[D_8011C238[i].unk2[0]].z;
                    } else {
                        x0 = D_8011B120[D_8011C238[i].unk2[0]].x;
                        z0 = D_8011B120[D_8011C238[i].unk2[0]].z;
                    }
                    temp_t5 >>= 1;
                    if (temp_t5 & 1) {
                        x1 = D_8011B330[D_8011C238[i].unk2[1]].x;
                        z1 = D_8011B330[D_8011C238[i].unk2[1]].z;
                    } else {
                        x1 = D_8011B120[D_8011C238[i].unk2[1]].x;
                        z1 = D_8011B120[D_8011C238[i].unk2[1]].z;
                    }
                    temp_t5 >>= 1;
                    for (var_s2 = 2; var_s2 < D_8011C238[i].unk0; var_s2++) {
                        if (temp_t5 & 1) {
                            x2 = D_8011B330[D_8011C238[i].unk2[var_s2]].x;
                            z2 = D_8011B330[D_8011C238[i].unk2[var_s2]].z;
                        } else {
                            x2 = D_8011B120[D_8011C238[i].unk2[var_s2]].x;
                            z2 = D_8011B120[D_8011C238[i].unk2[var_s2]].z;
                        }
                        temp_t5 >>= 1;
                        var_f2 += area_triangle_2d(x0, z0, x1, z1, x2, z2) * D_800DC884[D_8011C238[i].unkA];
                        x1 = x2;
                        z1 = z2;
                    }
                }
            }
        }
        
        if (D_8011D0E4 < var_f2) {
            var_f2 = D_8011D0E4 * 0.99f;
        }
    }
    return (D_8011D0E4 - var_f2) / D_8011D0E4;
}

s32 func_8002FD74(f32 x0, f32 z0, f32 x1, f32 x2, s32 count, Vec4f *arg5) {
    if(count > 0) {
        f32 minX = arg5[0].x;
        f32 maxX = arg5[0].x;
        f32 minZ = arg5[0].z;
        f32 maxZ = arg5[0].z;
        s32 i;

        for(i = 1; i < count; i++) {
            if (arg5[i].x < minX) {
                minX = arg5[i].x;
            } else if (maxX < arg5[i].x) {
                maxX = arg5[i].x;
            }
            if (arg5[i].z < minZ) {
                minZ = arg5[i].z;
            } else if (maxZ < arg5[i].z) {
                maxZ = arg5[i].z;
            }
        }
    
        if ((x0 <= maxX) && (z0 <= maxZ) && (minX <= x1) && (minZ <= x2)) {
            return -1;
        }
    }

    return 0;   
}

GLOBAL_ASM("asm/non_matchings/tracks/func_8002FF6C.s")

#ifdef NON_EQUIUVALENT
void func_800304C8(Vec4f *arg0) {
    s16 found1;
    s16 found2;
    s16 found3;
    f32 temp;
    f32 arg02x;
    f32 arg00z;
    f32 compare;
    
    found1 = FALSE;
    found2 = FALSE;
    found3 = FALSE;
    temp = arg0[0].z;
    arg00z = temp;
    compare = 0.0f;
    temp = (D_8011D0C4->segment.trans.z_position - arg0[1].z);
    
    if ((((D_8011D0C4->segment.trans.x_position - arg0[0].x) * (arg0[1].z - arg00z)) - ((arg0[1].x - arg0[0].x) * (((0, D_8011D0C4->segment.trans.z_position)) - arg00z))) >= compare) {
        found1 = TRUE;
    }
    if ((((D_8011D0C4->segment.trans.x_position - arg0[1].x) * (arg0[2].z - arg0[1].z)) - (temp * (arg0[2].x - arg0[1].x))) >= compare) {
        found2 = TRUE;
    }
    arg02x = arg0[2].x;
    if (found1 == found2) {
        f32 zPosDiff = (arg00z - arg0[2].z);
        if ((((D_8011D0C4->segment.trans.x_position - arg02x) * zPosDiff) - ((arg0[0].x - arg02x) * (arg02x - arg0[2].z))) >= compare) {
            found3 = TRUE;
        }
        if (found2 == found3) {
            f32 test = (-(((D_8011D0BC->x * D_8011D0C4->segment.trans.x_position) + (D_8011D0BC->z * D_8011D0C4->segment.trans.z_position)) + D_8011D0BC->w)) / D_8011D0BC->y;
            if (D_8011D0D0 < test) {
                D_8011D0D0 = test;
            }
        }
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_800304C8.s")
#endif

/**
 * Instantly update current fog properties.
*/
void set_fog(s32 fogIdx, s16 near, s16 far, u8 red, u8 green, u8 blue) {
    s32 tempNear;
    FogData *fogData;
    
    fogData = &gFogData[fogIdx];
    
    if (far < near) {
        tempNear = near;
        near = far;
        far = tempNear;
    }
    
    if (far > 1023) {
        far = 1023;
    }
    if (near >= far - 5) {
        near = far - 5;
    }

    fogData->addFog.near = 0;
    fogData->addFog.far = 0;
    fogData->addFog.r = 0;
    fogData->addFog.g = 0;
    fogData->addFog.b = 0;
    fogData->fog.r = red << 16;
    fogData->fog.g = green << 16;
    fogData->fog.b = blue << 16;
    fogData->fog.near = near << 16;
    fogData->fog.far = far << 16;
    fogData->intendedFog.r = red;
    fogData->intendedFog.g = green;
    fogData->intendedFog.b = blue;
    fogData->intendedFog.near = near;
    fogData->intendedFog.far = far;
    fogData->switchTimer = 0;
    fogData->fogChanger = NULL;
}

/**
 * Writes the current fog settings to the arguments.
 * Pre-shifts the data, so the raw values are correct.
*/
void get_fog_settings(s32 playerID, s16 *near, s16 *far, u8 *r, u8 *g, u8 *b) {
    *near = gFogData[playerID].fog.near >> 16;
    *far = gFogData[playerID].fog.far >> 16;
    *r = gFogData[playerID].fog.r >> 16;
    *g = gFogData[playerID].fog.g >> 16;
    *b = gFogData[playerID].fog.b >> 16;
}

/**
 * Sets the fog of the player ID to the default values.
 * Current fog attributes are rightshifted 16 bytes.
*/
void reset_fog(s32 playerID) {
    gFogData[playerID].addFog.near = 0;
    gFogData[playerID].addFog.far = 0;
    gFogData[playerID].addFog.r = 0;
    gFogData[playerID].addFog.g = 0;
    gFogData[playerID].addFog.b = 0;
    gFogData[playerID].fog.near = 1018 << 16;
    gFogData[playerID].fog.far = 1023 << 16;
    gFogData[playerID].intendedFog.r = gFogData[playerID].fog.r >> 16;
    gFogData[playerID].intendedFog.g = gFogData[playerID].fog.g >> 16;
    gFogData[playerID].intendedFog.b = gFogData[playerID].fog.b >> 16;
    gFogData[playerID].intendedFog.near = 1018;
    gFogData[playerID].intendedFog.far = 1023;
    gFogData[playerID].switchTimer = 0;
    gFogData[playerID].fogChanger = NULL;
}

/**
 * If the fog override timer is active, apply that and slowly degrade.
 * Otherwise, set the current fog to the intended fog settings.
*/
void update_fog(s32 viewportCount, s32 updateRate) {
    s32 i;
    for (i = 0; i < viewportCount; i++) {
        if (gFogData[i].switchTimer > 0) {
            if (updateRate < gFogData[i].switchTimer) {
                gFogData[i].fog.r += gFogData[i].addFog.r * updateRate;
                gFogData[i].fog.g += gFogData[i].addFog.g * updateRate;
                gFogData[i].fog.b += gFogData[i].addFog.b * updateRate;
                gFogData[i].fog.near += gFogData[i].addFog.near * updateRate;
                gFogData[i].fog.far += gFogData[i].addFog.far * updateRate;
                gFogData[i].switchTimer -= updateRate;
            } else {
                gFogData[i].fog.r = gFogData[i].intendedFog.r << 16;
                gFogData[i].fog.g = gFogData[i].intendedFog.g << 16;
                gFogData[i].fog.b = gFogData[i].intendedFog.b << 16;
                gFogData[i].fog.near = gFogData[i].intendedFog.near << 16;
                gFogData[i].fog.far = gFogData[i].intendedFog.far << 16;
                gFogData[i].switchTimer = 0;
            }
        }
    }
}

/**
 * Sets the fog settings for the active viewport based on the parameters of the environment data.
*/
void apply_fog(Gfx **dList, s32 playerID) {
    gDPSetFogColor((*dList)++, gFogData[playerID].fog.r >> 0x10, gFogData[playerID].fog.g >> 0x10, gFogData[playerID].fog.b >> 0x10, 0xFF);
    gSPFogPosition((*dList)++, gFogData[playerID].fog.near >> 0x10, gFogData[playerID].fog.far >> 0x10);
}

/**
 * Sets the active viewport's fog target when passed through.
 * Used in courses to make less, or more dense.
 * @bug: Timer doesn't account for PAL, meaning fog will scroll 20% slower on PAL systems.
*/
void obj_loop_fogchanger(Object *obj) {
    s32 nearTemp;
    s32 fogNear;
    s32 views;
    s32 playerIndex;
    s32 index;
    s32 fogFar;
    s32 i;
    s32 fogR;
    s32 fogG;
    s32 fogB;
    f32 x;
    f32 z;
    s32 switchTimer;
    LevelObjectEntry_FogChanger *fogChanger;
    Object **racers;
    Object_Racer *racer;
    FogData *fog;
    ObjectSegment *camera;
    
    racers = NULL;
    fogChanger = (LevelObjectEntry_FogChanger *) obj->segment.level_entry;
    camera = NULL;
    
    if (gCutsceneCameraActive) {
        camera = get_cutscene_camera_segment();
        views = gNumberOfViewports + 1;
    } else {
        racers = get_racer_objects(&views);
    }
    
    for(i = 0; i < views; i++) {
        index = PLAYER_COMPUTER;
        if (racers != NULL) {
            racer = &racers[i]->unk64->racer;
            playerIndex = racer->playerIndex;
            if (playerIndex >= PLAYER_ONE && playerIndex <= PLAYER_FOUR && obj != gFogData[playerIndex].fogChanger) {
                index = playerIndex;
                x = racers[i]->segment.trans.x_position;
                z = racers[i]->segment.trans.z_position;
            }
        } else if (i <= PLAYER_FOUR && obj != gFogData[i].fogChanger) {
            index = i;
            x = camera[i].trans.x_position;
            z = camera[i].trans.z_position;
        }
        if (index != PLAYER_COMPUTER) {
            x -= obj->segment.trans.x_position;
            z -= obj->segment.trans.z_position;
            if ((x * x) + (z * z) < obj->properties.distance.radius) {
                fogNear = fogChanger->near;
                fogFar = fogChanger->far;
                fogR = fogChanger->r;
                fogG = fogChanger->g;
                fogB = fogChanger->b;
                switchTimer = fogChanger->switchTimer;
                // Swap near and far if they're the wrong way around.
                if (fogFar < fogNear) {
                    nearTemp = fogNear;
                    fogNear = fogFar;
                    fogFar = nearTemp;
                }
                if (fogFar > 1023) {
                    fogFar = 1023;
                }
                if (fogNear >= fogFar - 5) {
                    fogNear = fogFar - 5;
                }
                fog = &gFogData[index];
                fog->intendedFog.r = fogR;
                fog->intendedFog.g = fogG;
                fog->intendedFog.b = fogB;
                fog->intendedFog.near = fogNear;
                fog->intendedFog.far = fogFar;
                fog->addFog.r = ((fogR << 16) - fog->fog.r) / switchTimer;
                fog->addFog.g = ((fogG << 16) - fog->fog.g) / switchTimer;
                fog->addFog.b = ((fogB << 16) - fog->fog.b) / switchTimer;
                fog->addFog.near = ((fogNear << 16) - fog->fog.near) / switchTimer;
                fog->addFog.far = ((fogFar << 16) - fog->fog.far) / switchTimer;
                fog->switchTimer = switchTimer;
                fog->fogChanger = obj;
            }
        }
    }
}

/**
 * Set the fog properties from the current values to the target, over a time specified by switchTimer.
 * @bug: Timer doesn't account for PAL, meaning fog will scroll 20% slower on PAL systems.
*/
void slowly_change_fog(s32 fogIdx, s32 red, s32 green, s32 blue, s32 near, s32 far, s32 switchTimer) {
    s32 temp;
    FogData *fogData;

    fogData = &gFogData[fogIdx];
    
    if (far < near) {
        temp = near;
        near = far;
        far = temp;
    }
    
    if (far > 1023) {
        far = 1023;
    }
    if (near >= far - 5) {
        near = far - 5;
    }
    
    fogData->intendedFog.r = red;
    fogData->intendedFog.g = green;
    fogData->intendedFog.b = blue;
    fogData->intendedFog.near = near;
    fogData->intendedFog.far = far;
    fogData->addFog.r = ((red << 16) - fogData->fog.r) / switchTimer;
    fogData->addFog.g = ((green << 16) - fogData->fog.g) / switchTimer;
    fogData->addFog.b = ((blue << 16) - fogData->fog.b) / switchTimer;
    fogData->addFog.near = ((near << 16) - fogData->fog.near) / switchTimer;
    fogData->addFog.far = ((far << 16) - fogData->fog.far) / switchTimer;
    fogData->switchTimer = switchTimer;
    fogData->fogChanger = NULL;
}

void compute_scene_camera_transform_matrix(void) {
    Matrix mtx;
    ObjectTransform trans;

    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = -65536.0f;

    trans.z_rotation = gSceneActiveCamera->trans.z_rotation;
    trans.x_rotation = gSceneActiveCamera->trans.x_rotation;
    trans.y_rotation = gSceneActiveCamera->trans.y_rotation;
    trans.x_position = 0.0f;
    trans.y_position = 0.0f;
    trans.z_position = 0.0f;
    trans.scale = 1.0f;

    object_transform_to_matrix(mtx, &trans);
    guMtxXFMF(mtx, x, y, z, &x, &y, &z);

    //Store x/y/z as integers
    gScenePerspectivePos.x = x;
    gScenePerspectivePos.y = y;
    gScenePerspectivePos.z = z;
}
