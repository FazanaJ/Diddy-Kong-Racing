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

// Maximum size for a level model is 522.5 KiB
#define LEVEL_MODEL_MAX_SIZE 0x80000
#define LEVEL_SEGMENT_MAX 128

/************ .data ************/

s32 D_800DC870 = 0; // Currently unknown, might be a different type.
//!@bug These two transition effects are marked to not clear when done, meaning they stay active the whole time.
unknown800DC874 D_800DC874 = { { -128 }, 40, 0 };
unknown800DC874 D_800DC87C = { { -125 }, 70, 0 };

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

/************ .rodata ************/

const char gViewport4Text[] = "TT CAM";

/*********************************/

/************ .bss ************/

Gfx *gSceneCurrDisplayList;
MatrixS *gSceneCurrMatrix;
Vertex *gSceneCurrVertexList;
TriangleList *gSceneCurrTriList;

ObjectSegment *gSceneActiveCamera;

s32 gSceneCurrentPlayerID;
Object *gSkydomeSegment;
s32 D_8011B0C0;
s32 D_8011B0C4;
s32 D_8011B0C8;
s32 D_8011B0CC;
s32 D_8011B0D0;
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
s32 D_8011B0F8;
s32 gAntiAliasing;
s32 D_8011B100;
s32 D_8011B104;
s32 D_8011B108;
s32 D_8011B10C;
s32 D_8011B110;
u32 D_8011B114;
s32 D_8011B118;
s32 D_8011B11C;
s32 D_8011B120[128];
s32 D_8011B320[4];
s32 D_8011B330[960];
s32 D_8011C230[2];
s32 D_8011C238[96];
s32 D_8011C3B8[320];
s32 D_8011C8B8[512];
s32 D_8011D0B8;
s32 D_8011D0BC;
s32 D_8011D0C0;
s32 D_8011D0C4;
s32 D_8011D0C8;
s16 D_8011D0CC;
s16 D_8011D0CE;
s32 D_8011D0D0;
f32 D_8011D0D4;
s32 D_8011D0D8;
s32 D_8011D0DC;
s32 D_8011D0E0;
f32 D_8011D0E4;
s32 D_8011D0E8;
s32 D_8011D0EC;
s32 D_8011D0F0;
s32 D_8011D0F4;
Vec4f D_8011D0F8[3];
Vec4f D_8011D128[3];
s32 D_8011D158[3]; // Unused? Or part of something bigger above?
s32 D_8011D164;
s32 D_8011D168[84];
WaterProperties *gTrackWaves[20];
s8 D_8011D308;
LevelModel *gTrackModelHeap;
s32 *gLevelModelTable;
s32 *D_8011D320[4];
unk8011D330 *D_8011D330;
s32 D_8011D334;
s32 *D_8011D338[4];
unk8011D348 *D_8011D348;
s32 D_8011D34C;
s32 *D_8011D350[4];
unk8011D360 *D_8011D360;
s32 D_8011D364;
s32 D_8011D368;
s32 D_8011D36C;
s32 *D_8011D370;
s32 *D_8011D374;
s32 D_8011D378;
s32 gScenePlayerViewports;
u32 D_8011D384;
FogData gFogData[4];
Vec3i gScenePerspectivePos;
unk8011D474 *D_8011D474; // 0x10 bytes struct?
unk8011D478 *D_8011D478; // 0x8 bytes struct?
s16 **D_8011D47C;
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

// track_init
#ifdef NON_EQUIVALENT
void func_8002C0C4(s32 modelId);
void func_800249F0(u32 arg0, u32 arg1, s32 arg2, Vehicle vehicle, u32 arg4, u32 arg5, u32 arg6) {
    s32 i;
    s32 tmp_a2;

    gCurrentLevelHeader2 = get_current_level_header();
    D_8011B0F8 = 0;
    D_8011B100 = 0;
    D_8011B104 = 0;
    D_8011B108 = 0;
    D_8011B10C = 0;
    if (gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_1 || gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_2) {
        D_8011B0F8 = 1;
    }
    func_8002C0C4(arg0);

    if (arg2 < 2) {
        D_8011D384 = 0;
        for (i = 0; i < gCurrentLevelModel->numberOfSegments; i++) {
            if (gCurrentLevelModel->segments[i].unk2B != 0) {
                D_8011D384++;
                gCurrentLevelModel->segments[i].unk2B = 1;
            }
        }
    }
    if (is_in_two_player_adventure() && (gCurrentLevelHeader2->race_type == RACETYPE_DEFAULT || gCurrentLevelHeader2->race_type & RACETYPE_CHALLENGE)) {
        tmp_a2 = 2;
    } else {
        tmp_a2 = arg2;
        tmp_a2++;
    }
    if (D_8011D384) {
        func_800B82B4(gCurrentLevelModel, gCurrentLevelHeader2, tmp_a2);
    }
    set_active_viewports_and_max(arg2);
    spawn_skydome(arg1);
    D_8011B110 = 0;
    D_8011B114 = 0x10000;
    func_80011390();
    func_8000C8F8(arg6, 0);
    func_8000C8F8(arg5, 1);
    gScenePlayerViewports = arg2;
    func_8000CC7C(vehicle, arg4, arg2);
    func_8000B020(72, 64);
    if (arg0 == 0 && arg4 == 0) {
        transition_begin(&D_800DC87C);
    } else {
        transition_begin(&D_800DC874);
    }
    set_active_viewports_and_max(gScenePlayerViewports);
    gAntiAliasing = 0;
    i = 0;
    do {
        D_8011D350[i] = allocate_from_main_pool_safe(3200, COLOUR_TAG_YELLOW);
        D_8011D320[i] = allocate_from_main_pool_safe(12800, COLOUR_TAG_YELLOW);
        D_8011D338[i] = allocate_from_main_pool_safe(20000, COLOUR_TAG_YELLOW);
    } while ((s32)&D_8011D338[++i] != (s32)&D_8011D348);

    D_8011B0C8 = 0;
    func_8002D8DC(1, 1, 0);
    func_8002D8DC(2, 2, 0);
    D_8011B0C8 = 1;
    func_8002D8DC(1, 1, 0);
    func_8002D8DC(2, 2, 0);
    
    D_8011B0C8 = 0;
    if (gCurrentLevelHeader2->unkB7) {
        D_8011B0E1 = gCurrentLevelHeader2->unkB4;
        D_8011B0E2 = gCurrentLevelHeader2->unkB5;
        D_8011B0E3 = gCurrentLevelHeader2->unkB6;
        func_80025510(arg2 + 1);
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_800249F0.s")
#endif

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

    gSceneCurrDisplayList = *dList;
    gSceneCurrMatrix = *mtx;
    gSceneCurrVertexList = *vtx;
    gSceneCurrTriList = *tris;
    gSceneRenderSkyDome = TRUE;
    D_8011B0C4 = 0;
    D_8011B0C0 = 0;
    numViewports = set_active_viewports_and_max(gScenePlayerViewports);
    if (is_game_paused()) {
        tempUpdateRate = 0;
    } else {
        tempUpdateRate = updateRate;
    }
    if (D_8011D384) {
        func_800B9C18(tempUpdateRate);
    }
    func_8002D8DC(2, 2, updateRate);
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
        gAntiAliasing = TRUE;
    }
    if (gCurrentLevelHeader2->race_type == RACETYPE_CUTSCENE_1 || gCurrentLevelHeader2->unkBD) {
        gAntiAliasing = TRUE;
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
    reset_render_settings(&gSceneCurrDisplayList);
    gDkrDisableBillboard(gSceneCurrDisplayList++);
    gSPClearGeometryMode(gSceneCurrDisplayList++, G_CULL_FRONT);
    gDPSetBlendColor(gSceneCurrDisplayList++, 0, 0, 0, 0x64);
    gDPSetPrimColor(gSceneCurrDisplayList++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(gSceneCurrDisplayList++, 255, 255, 255, 0);
    func_800AD40C();
    update_fog(numViewports, tempUpdateRate);
    func_800AF404(tempUpdateRate);
    if (gCurrentLevelModel->numberOfAnimatedTextures > 0) {
        func_80027E24(tempUpdateRate);
    }
    for (j = gSceneCurrentPlayerID = 0; j < numViewports; gSceneCurrentPlayerID++, j = gSceneCurrentPlayerID) {
        if (j == 0) {
            if (func_8000E184() && numViewports == 1) {
                gSceneCurrentPlayerID = PLAYER_TWO;
            }
        }
        if (flip) {
            gSPSetGeometryMode(gSceneCurrDisplayList++, G_CULL_FRONT);
        }
        apply_fog(gSceneCurrentPlayerID);
        gDPPipeSync(gSceneCurrDisplayList++);
        set_active_camera(gSceneCurrentPlayerID);
        func_80066CDC(&gSceneCurrDisplayList, &gSceneCurrMatrix);
        func_8002A31C();
        // Show detailed skydome in single player.
        if (numViewports < 2 || gSkipCutbacks) {
            func_80068408(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            if (gCurrentLevelHeader2->skyDome == -1) {
                func_80028050();
            } else {
                render_skydome();
            }
        } else {
            func_8006807C(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            draw_gradient_background();
            func_80067D3C(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            func_80068408(&gSceneCurrDisplayList, &gSceneCurrMatrix);
        }
        gDPPipeSync(gSceneCurrDisplayList++);
        initialise_player_viewport_vars(updateRate);
        set_weather_limits(-1, -512);
        // Show weather effects in single player.
        if (gCurrentLevelHeader2->weatherEnable > 0 && numViewports < 2) {
            process_weather(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, &gSceneCurrTriList, tempUpdateRate);
        }
        func_800AD030(get_active_camera_segment());
        func_800ACA20(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_active_camera_segment());
        render_hud(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_racer_object_by_port(gSceneCurrentPlayerID), updateRate);
    }
    // Show TT Cam toggle for the fourth viewport when playing 3 player.
    if (numViewports == 3 && get_current_level_race_type() != RACETYPE_CHALLENGE_EGGS && 
        get_current_level_race_type() != RACETYPE_CHALLENGE_BATTLE && get_current_level_race_type() != RACETYPE_CHALLENGE_BANANAS) {
        if (get_hud_setting() == 0) {
            if (flip) {
                gSPSetGeometryMode(gSceneCurrDisplayList++, G_CULL_FRONT);
            }
            apply_fog(PLAYER_FOUR);
            gDPPipeSync(gSceneCurrDisplayList++);
            set_active_camera(PLAYER_FOUR);
            disable_cutscene_camera();
            func_800278E8(updateRate);
            func_80066CDC(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            func_8002A31C();
            func_8006807C(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            //draw_gradient_background();
            func_80067D3C(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            func_80068408(&gSceneCurrDisplayList, &gSceneCurrMatrix);
            gDPPipeSync(gSceneCurrDisplayList++);
            initialise_player_viewport_vars(updateRate);
            set_weather_limits(-1, -512);
            func_800AD030(get_active_camera_segment());
            func_800ACA20(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, get_active_camera_segment());
            set_text_font(FONT_COLOURFUL);
            posX = (gScreenWidth / 2) + 10;
            posY = (gScreenHeight / 2) + 5;
            draw_text(&gSceneCurrDisplayList, posX, posY, (char *)(&gViewport4Text), ALIGN_TOP_LEFT); // TT CAM
        } else {
            set_active_camera(PLAYER_FOUR);
            func_800278E8(updateRate);
        }
    }
    func_800682AC(&gSceneCurrDisplayList);
    gDPPipeSync(gSceneCurrDisplayList++);
    gDkrDisableBillboard(gSceneCurrDisplayList++);
    D_8011B0C8 = 1 - D_8011B0C8;
    *dList = gSceneCurrDisplayList;
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
void func_80026C14(s16 arg0, s16 arg1, s32 arg2);
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
                if (D_8011D478[i].unk8 < D_8011D478[i].unk0) {
                    temp_t3 = D_8011D478[i].unk8;
                    D_8011D478[i].unk8 = D_8011D478[i].unk0;
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
            temp_t3_2 = D_8011D478[var_s0].unk8 * 2;
            if (D_8011D47C[temp_t3_2][0] == -1) {
                D_8011D478[i].unk4 = (s8) (D_8011D478[i].unk8 | 2);
                D_8011D47C[temp_t3_2][0] = (s8) i;
            } else {
                D_8011D47C[temp_t3_2][0] = (s8) i;
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
GLOBAL_ASM("asm/non_matchings/tracks/func_80026C14.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80026E54.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80027184.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_80027568.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_800278E8.s")

void func_80027E24(s32 updateRate) {
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
                    if ((texture->numOfTextures != 0x100) && (texture->frameAdvanceDelay != 0)) {
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

//https://decomp.me/scratch/jmbc1
GLOBAL_ASM("asm/non_matchings/tracks/func_80028050.s")

/**
 * Instead of drawing the skydome with textures, draw a solid coloured background.
 * Using different colours set in the level header, the vertices are coloured and
 * it gives the background a gradient effect.
*/
void draw_gradient_background(void) {
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
    reset_render_settings(&gSceneCurrDisplayList);
    load_and_set_texture_no_offset(&gSceneCurrDisplayList, 0, RENDER_FOG_ACTIVE);
    gSPVertexDKR(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(verts), 4, 0);
    gSPPolygon(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(tris), 2, 0);
    y0 = -150;
    y1 = 150;
    if (get_viewport_count() == TWO_PLAYERS) {
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
}

/**
 * Sets the position to the current camera's position then renders the skydome if set to be visible.
*/
void render_skydome(void) {
    ObjectSegment *cam;
    if (gSkydomeSegment == NULL)
        return;

    cam = get_active_camera_segment();
    if (gCurrentLevelHeader2->skyDome == 0) {
        gSkydomeSegment->segment.trans.x_position = cam->trans.x_position;
        gSkydomeSegment->segment.trans.y_position = cam->trans.y_position;
        gSkydomeSegment->segment.trans.z_position = cam->trans.z_position;
    }

    func_80068408(&gSceneCurrDisplayList, &gSceneCurrMatrix);
    if (gSceneRenderSkyDome) {
        render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, gSkydomeSegment);
    }
}

/**
 * Sets up all of the required variables for the player's view perspective.
 * This includes setting up the camera index, viewport and 
*/
void initialise_player_viewport_vars(s32 updateRate) {
    s32 i;
    s32 numRacers;
    s32 viewportID;
    Object **racers;
    s32 segmentIndex;
    Object_Racer *racer;

    gSceneActiveCamera = get_active_camera_segment();
    viewportID = get_current_viewport();
    compute_scene_camera_transform_matrix();
    update_envmap_position((f32) gScenePerspectivePos.x / 65536.0f, (f32) gScenePerspectivePos.y / 65536.0f, (f32) gScenePerspectivePos.z / 65536.0f);
    segmentIndex = gSceneActiveCamera->object.cameraSegmentID;
    if (segmentIndex > -1 && (segmentIndex < gCurrentLevelModel->numberOfSegments)) {
        gSceneStartSegment = gCurrentLevelModel->segments[segmentIndex].unk28;
    } else {
        gSceneStartSegment = -1;
    }
    if (D_8011D384 != 0) {
        func_800B8B8C();
        racers = get_racer_objects(&numRacers);
        if (gSceneActiveCamera->object.unk36 != 7 && numRacers > 0 && !check_if_showing_cutscene_camera()) {
            i = -1; 
            do {
                i++;
                racer = &racers[i]->unk64->racer;
            } while(i < numRacers - 1 && viewportID != racer->playerIndex);
            func_800B8C04(racers[i]->segment.trans.x_position, racers[i]->segment.trans.y_position, racers[i]->segment.trans.z_position, get_current_viewport(), updateRate);
        } else {
            func_800B8C04((s32) gSceneActiveCamera->trans.x_position, (s32) gSceneActiveCamera->trans.y_position, (s32) gSceneActiveCamera->trans.z_position, get_current_viewport(), updateRate);
        }
    }
    get_current_level_header()->unk3 = 1;
    render_level_geometry_and_objects();
}

/**
 * Enable or disable anti aliasing.
 * Improves visual quality at the cost of performance.
*/
void set_anti_aliasing(s32 setting) {
    gAntiAliasing = setting;
}

/**
 * Find which segments can and should be rendered, then render their opaque geometry.
 * Render all objects inside visible segments then render the level's semitransparent geometry.
 * Afterwards, render particles.
*/
void render_level_geometry_and_objects(void) {
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

    if (get_settings()->courseId == ASSET_LEVEL_OPENINGSEQUENCE) {
        gAntiAliasing = TRUE;
    }

    sp160 = func_80014814(&objCount);

    if (gCurrentLevelModel->numberOfSegments > 1) {
        numberOfSegments = 0;
        traverse_segments_bsp_tree(0, 0, gCurrentLevelModel->numberOfSegments - 1, segmentIds, &numberOfSegments);
    } else {
        numberOfSegments = 1;
        segmentIds[0] = 0;
    }

    for (i = 1; i <= gCurrentLevelModel->numberOfSegments; i++) {
        objectsVisible[i] = FALSE; // why not a bzero?
    }

    objectsVisible[0] = TRUE;

    if (gDrawLevelSegments) {
        for (i = 0; i < numberOfSegments; i++) {
            render_level_segment(segmentIds[i], FALSE); // Render opaque segments
            objectsVisible[segmentIds[i] + 1] = TRUE;
        }
    }

    if (gCurrentLevelModel->numberOfSegments < 2) {
        objectsVisible[1] = TRUE;
    }

    reset_render_settings(&gSceneCurrDisplayList);
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_OBJGFX] = osGetCount();
#endif
    gIsObjectRender = TRUE;
    func_80015348(sp160, objCount - 1);
    visibleFlags = OBJ_FLAGS_INVIS_PLAYER1 << (get_current_viewport() & 1);

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
        if (obj != NULL && visible == 255 && (objectsVisible[obj->segment.object.segmentID + 1] || obj->segment.camera.unk34 > 1000.0f) && check_if_in_draw_range(obj)) {
            if (obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) {
                render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                continue;
            } else if (obj->shadow != NULL) {
                render_object_shadow(obj, obj->shadow);
            }
            render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            if (obj->unk58 != NULL && obj->segment.header->unk30 & 0x10) {
                func_8002D670(obj, obj->unk58);
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
                render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                continue;
            } else if (obj->shadow != NULL) {
                render_object_shadow(obj, obj->shadow);
            }
            render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            if ((obj->unk58 != NULL) && (obj->segment.header->unk30 & 0x10)) {
                func_8002D670(obj, obj->unk58);
            }
        }
    }
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[1][PP_OBJGFX] = osGetCount();
#endif
    gIsObjectRender = FALSE;
    if (gDrawLevelSegments) {
        for (i = numberOfSegments - 1; i >= 0; i--) {
            render_level_segment(segmentIds[i], TRUE); // Render transparent segments
        }
    }
    if (D_8011D384 != 0) {
        func_800BA8E4(&gSceneCurrDisplayList, &gSceneCurrMatrix, get_current_viewport());
    }

    reset_render_settings(&gSceneCurrDisplayList);
    load_and_set_texture_no_offset(&gSceneCurrDisplayList, 0, RENDER_FOG_ACTIVE | RENDER_Z_COMPARE);
    func_80012C3C(&gSceneCurrDisplayList);
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
                    render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                    goto skip;
                } else if (obj->shadow != NULL) {
                    render_object_shadow(obj, obj->shadow);
                }
                render_object(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                if ((obj->unk58 != 0) && (obj->segment.header->unk30 & 0x10)) {
                    func_8002D670(obj, obj->unk58);
                }
            }
skip:
            if (obj->behaviorId == BHV_RACER) {
                render_racer_shield(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
                render_racer_magnet(&gSceneCurrDisplayList, &gSceneCurrMatrix, &gSceneCurrVertexList, obj);
            }
        }
    }

    if (D_800DC924 && func_80027568()) {
        func_8002581C(segmentIds, numberOfSegments, get_current_viewport());
    }
    gAntiAliasing = FALSE;
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[1][PP_PARTICLEGFX] = osGetCount();
#endif
}

/**
 * Render a batch of level geometry.
 * Since opaque and transparent are done in two separate runs, it will skip over the other.
 * Has a special case for the flashing lights in Spaceport Alpha, too.
*/
void render_level_segment(s32 segmentId, s32 nonOpaque) {
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
    s32 isInvisible;
    s32 levelHeaderIndex;
    s32 texOffset;
    s32 sp78;
    s32 startPos;
    s32 endPos;
    s32 batchFlags;
    s32 textureFlags;
    numberVertices = (batchInfo + 1)->verticesOffset - batchInfo->verticesOffset;
    segment = &gCurrentLevelModel->segments[segmentId];
    if (gScenePlayerViewports < VIEWPORTS_COUNT_2_PLAYERS) {
        sp78 = (nonOpaque && D_8011D384) ? (func_800B9228(segment)) : (0);
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
        isInvisible = batchInfo->flags & BATCH_FLAGS_HIDDEN;
        if (isInvisible) {
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
        
        if (!(batchFlags & BATCH_FLAGS_DEPTH_WRITE) && !(batchFlags & BATCH_FLAGS_UNK00000800)) {
            if (gConfig.antiAliasing || gOverrideAA) {
                batchFlags &= ~RENDER_ANTI_ALIASING;
            } else {
                batchFlags |= RENDER_ANTI_ALIASING;
            }
        }
        if ((!(textureFlags & RENDER_SEMI_TRANSPARENT) && !(batchFlags & BATCH_FLAGS_WATER)) || batchFlags & BATCH_FLAGS_UNK00000800) {
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
            gDPSetEnvColor(gSceneCurrDisplayList++, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->red, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->green, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->blue, 
            ((LevelHeader_70 *)((u8 **)(&((LevelHeader **) gCurrentLevelHeader2)[levelHeaderIndex]))[28])->alpha);
        } else {
            gDPSetEnvColor(gSceneCurrDisplayList++, 255, 255, 255, 0);
        }
        if (batchFlags & BATCH_FLAGS_PULSATING_LIGHTS) {
            color = gCurrentLevelHeader2->pulseLightData->outColorValue;
            gDPSetPrimColor(gSceneCurrDisplayList++, 0, 0, color, color, color, color);
            load_blinking_lights_texture(&gSceneCurrDisplayList, texture, batchFlags, texOffset);
            gSPVertexDKR(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(vertices), numberVertices, 0);
            gSPPolygon(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(triangles), numberTriangles, TRIN_ENABLE_TEXTURE);
            gDPSetPrimColor(gSceneCurrDisplayList++, 0, 0, 255, 255, 255, 255);
        } else {
            load_and_set_texture(&gSceneCurrDisplayList, texture, batchFlags, texOffset);
            batchFlags = TRUE;
            if (texture == NULL) {
                batchFlags = FALSE;
            }
            gSPVertexDKR(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(vertices), numberVertices, 0);
            gSPPolygon(gSceneCurrDisplayList++, OS_PHYSICAL_TO_K0(triangles), numberTriangles, batchFlags);
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

    cameraMatrix = func_80069DA4();
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
        inverseMagnitude = (1.0 / sqrtf((x * x) + (y * y) + (z * z)));
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
    s32 isVisible;
    f32 dirX, dirY, dirZ, dirW;
    
    for (j = 0; j < 3; j++) {
        dirX = D_8011D0F8[j].x;
        dirY = D_8011D0F8[j].y;
        dirZ = D_8011D0F8[j].z;
        dirW = D_8011D0F8[j].w;
        
        for (i = 0, isVisible = FALSE; i < 8 && !isVisible; i++) {
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
                isVisible = TRUE;
            }
        }
        if (i == 8 && !isVisible) {
            return FALSE;
        }
    }
    return TRUE;
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
                if (temp2 > 0) {
                    fadeDist = dist - fadeDist;
                    alpha = ((f32) (1.0f - ((fadeDist) / temp2)) * 255.0f);
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
    if ((seg->unk2B != 0) && (D_8011D384 != 0) && (arg3 == 1)) {
        *arg1 = func_800BB2F4(obj->segment.object.segmentID, obj->segment.trans.x_position, obj->segment.trans.z_position, arg2);
        return TRUE;
    } else {
        *arg1 = seg->unk38;
        return TRUE;
    }
}

GLOBAL_ASM("asm/non_matchings/tracks/func_8002BAB0.s")

#ifdef NON_MATCHING
// Loads a level track from the index in the models table.
// Has regalloc issues.
void func_8002C0C4(s32 modelId) {
    s32 i, j, k;
    s32 temp_s4;
    s32 temp;
    LevelModel *mdl;
    s32 levelSize = 0;
    
    set_texture_colour_tag(COLOUR_TAG_GREEN);
    D_8011D370 = allocate_from_main_pool_safe(0x7D0, COLOUR_TAG_YELLOW);
    D_8011D374 = allocate_from_main_pool_safe(0x1F4, COLOUR_TAG_YELLOW);
    D_8011D378 = 0;
    gLevelModelTable = (s32*) load_asset_section_from_rom(ASSET_LEVEL_MODELS_TABLE);
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
    gTrackModelHeap = allocate_from_main_pool_safe(LEVEL_MODEL_MAX_SIZE, COLOUR_TAG_YELLOW);
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
    allocate_at_address_in_main_pool(temp_s4, (u8* ) gTrackModelHeap, COLOUR_TAG_YELLOW);
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
    set_texture_colour_tag(COLOUR_TAG_MAGENTA);
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
    if (D_8011D384 != 0) {
        func_800B7D20();
    }
    for (i = 0; i < gCurrentLevelModel->numberOfTextures; i++) {
        free_texture(gCurrentLevelModel->textures[i].texture);
    }
    free_from_memory_pool(gTrackModelHeap);
    free_from_memory_pool(D_8011D370);
    free_from_memory_pool(D_8011D374);
    free_sprite((Sprite *) gCurrentLevelModel->minimapSpriteIndex);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        free_from_memory_pool(D_8011D350[i]);
        free_from_memory_pool(D_8011D320[i]);
        free_from_memory_pool(D_8011D338[i]);
    }
    func_800257D0();
    if (gSkydomeSegment != NULL) {
        gParticlePtrList_addObject(gSkydomeSegment);
        gParticlePtrList_flush();
    }
    free_ghost_pool();
    func_8000C604();
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
                
                for (l = 0; l != 3; l++) {
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
                boxDelta = ( (bbox->x2 - bbox->x1) >> 3) + 1;
                bit = 1;
                boxMax = boxDelta + bbox->x1;
                boxMin = bbox->x1;
                val = 0;
                for (k = 0; k < 8; k++) {
                    if ((boxMax >= minX) && (maxX >= boxMin)) {
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
                    if ((boxMax >= minZ) && (maxZ >= boxMin)) {
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
void render_object_shadow(Object *obj, ShadowData *shadow) {
    s32 i;
    s32 temp_a0;
    s32 temp_a3;
    Vertex *vtx;
    Triangle *tri;
    s32 flags;
    s32 new_var;
    s32 new_var2;
    s32 someAlpha;
    
    if (obj->segment.header->unk32 != 0) {
        if (shadow->unk8 != -1 && D_8011B0C4 == 0) {
            D_8011B0CC = D_8011B0C8;
            if (obj->segment.header->unk32 == 1) {
                D_8011B0CC += 2;
            }
            i = shadow->unk8;
            D_8011D360 = (unk8011D360 *) D_8011D350[D_8011B0CC];
            D_8011D330 = (unk8011D330 *) D_8011D320[D_8011B0CC];
            D_8011D348 = (unk8011D348 *) D_8011D338[D_8011B0CC];
            someAlpha = D_8011D348[D_8011D360[i].unk6].unk9;
            flags = RENDER_FOG_ACTIVE | RENDER_Z_COMPARE;
            if (someAlpha == 0 || obj->segment.object.opacity == 0) {
                i = shadow->unkA;
            } else if (someAlpha != 255 || obj->segment.object.opacity != 255) {
                flags = RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE;
                someAlpha = (obj->segment.object.opacity * someAlpha) >> 8;
                gDPSetPrimColor(gSceneCurrDisplayList++, 0, 0, 255, 255, 255, someAlpha);
            }
            while (i < shadow->unkA) {
                load_and_set_texture_no_offset(&gSceneCurrDisplayList, (TextureHeader *) D_8011D360[i].unk0, flags);
                // I hope we can clean this part up.
                new_var2 = D_8011D360[i].unk4;
                new_var = D_8011D360[i].unk6;
                temp_a3 = D_8011D360[i+1].unk4 - new_var2;
                temp_a0 = D_8011D360[i+1].unk6 - new_var;
                tri = (Triangle *) &D_8011D330[new_var2];
                vtx = (Vertex *) &D_8011D348[new_var];
                gSPVertexDKR(gSceneCurrDisplayList++, OS_K0_TO_PHYSICAL(vtx), temp_a0, 0);
                gSPPolygon(gSceneCurrDisplayList++, OS_K0_TO_PHYSICAL(tri), temp_a3, 1);
                i++;
            }
            
            if (flags != RENDER_Z_COMPARE) {
                gDPSetPrimColor(gSceneCurrDisplayList++, 0, 0, 255, 255, 255, 255);
            }
        }
    }
}

void func_8002D670(Object *obj, ShadowData *shadow) {
    s32 i;
    s32 temp_a0;
    s32 temp_a3;
    Vertex *vtx;
    Triangle *tri;
    s32 flags;

    if (obj->segment.header->unk36 != 0) {
        if ((shadow->unk8 != -1) && (D_8011B0C4 == 0)) {
            D_8011B0D0 = D_8011B0C8;
            i = shadow->unk8;
            if (obj->segment.header->unk36 == 1) {
                D_8011B0D0 = D_8011B0C8;
                D_8011B0D0 += 2;
                if (get_distance_to_active_camera(obj->segment.trans.x_position, obj->segment.trans.y_position, obj->segment.trans.z_position) > 768.0f * 768.0f) {
                    i = shadow->unkA;
                }
            }
            flags = RENDER_FOG_ACTIVE | RENDER_Z_COMPARE;
            D_8011D360 = (unk8011D360* ) D_8011D350[D_8011B0D0];
            D_8011D330 = (unk8011D330* ) D_8011D320[D_8011B0D0];
            D_8011D348 = (unk8011D348* ) D_8011D338[D_8011B0D0];
            while (i < shadow->unkA) {
                load_and_set_texture_no_offset(&gSceneCurrDisplayList, (TextureHeader *) D_8011D360[i].unk0, flags);
                temp_a3 = D_8011D360[i+1].unk4 - D_8011D360[i].unk4;
                temp_a0 = D_8011D360[i+1].unk6 - D_8011D360[i].unk6;
                tri = &((Triangle *) D_8011D330)[D_8011D360[i].unk4];
                vtx = &((Vertex *) D_8011D348)[D_8011D360[i].unk6];
                gSPVertexDKR(gSceneCurrDisplayList++, OS_K0_TO_PHYSICAL(vtx), temp_a0, 0);
                gSPPolygon(gSceneCurrDisplayList++, OS_K0_TO_PHYSICAL(tri), temp_a3, 1);
                i++;
            }
        }
    }
}

void func_8002D8DC(s32 arg0, s32 arg1, s32 updateRate) {
    s32 sp94;
    s32 sp90;
    Object *obj;
    ObjectHeader *objHeader;
    f32 dist;
    s32 radius;
    s32 radius2;
    s32 numViewports;
    Object **objects;
    s32 var_a0;
    Object_58_4* obj58_4;
    ShadowData *shadow;
    Object_58* obj58;
    s32 playerIndex;

    D_8011B0CC = D_8011B0C8;
    if (arg0 == 1) {
        D_8011B0CC += 2;
    }
    D_8011D330 = (unk8011D330* ) D_8011D320[D_8011B0CC];
    D_8011D348 = (unk8011D348* ) D_8011D338[D_8011B0CC];
    D_8011D360 = (unk8011D360* ) D_8011D350[D_8011B0CC];
    D_8011D364 = 0;
    D_8011D368 = 0;
    D_8011D36C = 0;
    numViewports = get_viewport_count();
    objects = objGetObjList(&sp94, &sp90);
    while (sp94 < sp90) {
        obj = objects[sp94];
        objHeader = obj->segment.header;
        obj58 = obj->unk58;
        shadow = obj->shadow;
        sp94 += 1;
        if (!(obj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED)) {
            if (shadow != NULL && shadow->scale > 0.0f && arg0 == objHeader->unk32) {
                shadow->unk8 = -1;
            } 
            if (obj->segment.trans.flags & OBJ_FLAGS_INVISIBLE) {
                shadow = NULL;
            }
            if ((shadow != NULL && objHeader->unk32 == 2) || (obj58 != NULL && objHeader->unk36 == 2)) {
                dist = get_distance_to_active_camera(obj->segment.trans.x_position, obj->segment.trans.y_position, obj->segment.trans.z_position);
            } else {
                dist = 0;
            }
            if (shadow != NULL && shadow->scale > 0.0f && arg0 == objHeader->unk32) {
                D_8011D0D4 = 1.0f;
                shadow->unk8 = -1;
                var_a0 = FALSE;
                if (objHeader->unk32 == 2 && numViewports > ONE_PLAYER && numViewports <= FOUR_PLAYERS) {
                    if (obj->behaviorId == BHV_RACER) {
                        playerIndex = obj->unk64->racer.playerIndex;
                        if (playerIndex != PLAYER_COMPUTER) {
                            func_8002E234(obj, FALSE);
                            var_a0 = TRUE;
                        }
                    } else if (obj->behaviorId == BHV_WEAPON) {
                        func_8002E234(obj, FALSE);
                        var_a0 = TRUE;
                    }
                } else {
                    radius = objHeader->unk4A * objHeader->unk4A;
                    radius2 = objHeader->unk4C * objHeader->unk4C;
                    if (dist < radius) {
                        if (radius2 < dist) {
                            D_8011D0D4 = (radius - dist) / ( radius - radius2);
                        }
                        func_8002E234(obj, FALSE);
                        var_a0 = TRUE;
                    }
                }
                if ((!var_a0) && (obj->unk54 != NULL)) {
                    func_8002DE30(obj);
                }
            }
            if (obj58 != NULL && obj58->unk0 > 0.0f && arg1 == objHeader->unk36) {
                obj58->unk8 = -1;
                D_8011D0D4 = 1.0f;
                obj58_4 = obj58->unk4;
                if (obj58_4 != NULL && updateRate != 0 && obj58_4->unk12 != 0x100) {
                    obj58->unkC += obj58->unkE;
                    while (obj58_4->unk12 < obj58->unkC) {
                        obj58->unkC -= obj58_4->unk12;
                    } 
                }
                
                if (objHeader->unk32 == 2 && numViewports > ONE_PLAYER && numViewports <= FOUR_PLAYERS) {
                    if (obj->behaviorId == BHV_RACER) {
                        playerIndex = obj->unk64->racer.playerIndex;
                        if (playerIndex != PLAYER_COMPUTER) {
                            func_8002E234(obj, TRUE);
                        }
                    } else if (obj->behaviorId == BHV_WEAPON) {
                        func_8002E234(obj, TRUE);
                    }
                } else {
                    radius = objHeader->unk4A * objHeader->unk4A;
                    radius2 = objHeader->unk4C * objHeader->unk4C;
                    if (dist < radius) {
                        if (radius2 < dist) {
                            D_8011D0D4 = (radius - dist) / (radius - radius2);
                        }
                        func_8002E234(obj, TRUE);
                    }
                }
            }
        }
    }
    D_8011D360[D_8011D364].unk4 = D_8011D368;
    D_8011D360[D_8011D364].unk6 = D_8011D36C;
}

#ifdef NON_EQUIVALENT
void func_8002DE30(Object *obj) {
    s32 sp94;
    s32 sp90;
    s32 sp8C;
    s32 sp88;
    s32 i;
    s32 sp5C;
    LevelModelSegment *segment;
    ObjectHeader *objHeader;
    Triangle *temp_t0;
    TriangleBatchInfo *batches;
    Vertex *vert;
    f32 temp_f0_2;
    s16 temp_v0_3;
    s16 segmentIndex;
    s16 var_a1;
    s16 var_a2;
    s16 facesOffset;
    s32 obj_yPos;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s4;
    s32 var_s7;
    s32 var_t2;
    s32 var_v1;
    u32 temp_v1_2;
    u8 *var_a0;
    u8 temp_t7;
    s32 vertsOffset;

    objHeader = obj->segment.header;
    obj_yPos = obj->segment.trans.y_position;
    sp94 = obj_yPos + objHeader->unk44;
    sp90 = obj_yPos + objHeader->unk42;
    segmentIndex = obj->segment.object.segmentID;
    var_s7 = 0;
    if (segmentIndex != -1) {
        //func_800314DC = collision
        temp_v0 = func_800314DC(&gCurrentLevelModel->segmentsBoundingBoxes[segmentIndex], 
            (obj->segment.trans.x_position - 16.0f),
            (obj->segment.trans.z_position - 16.0f),
            (obj->segment.trans.x_position + 16.0f),
            (obj->segment.trans.z_position + 16.0f));

        segment = &gCurrentLevelModel->segments[segmentIndex];
        for (i = 0; i < segment->numberOfBatches; i++) {
            vertsOffset = segment->batches[i].verticesOffset;
            
        }

        i = 0;
        segment = &gCurrentLevelModel->segments[segmentIndex];
        var_t2 = 0;
        if  (segment->numberOfBatches > 0) {
            batches = segment->batches;
            vertsOffset = batches->verticesOffset;
loop_3:
            temp_v1_2 = batches->flags;
            if (!(temp_v1_2 & 0x6900)) {
                facesOffset = batches->facesOffset;
                vert = &segment->vertices[vertsOffset];
                if ((facesOffset < batches[1].textureIndex) != 0) {
                    var_s4 = facesOffset * 2;
                    if (var_s7 == 0) {
loop_6:
                        temp_v0_2 = *(segment->unk10 + var_s4) & temp_v0;
                        if ((temp_v0_2 & 0xFF) && (temp_v0_2 & 0xFF00)) {
                            temp_t0 = &segment->triangles[facesOffset];
                            temp_t7 = temp_t0->verticesArray[1];
                            var_a0 = &temp_t0->verticesArray[1];
                            var_v1 = 1;
                            var_a1 = vert[temp_t7].y;
                            var_a2 = var_a1;
                            do {
                                var_v1 += 1;
                                temp_v0_3 = vert[var_a0[1]].y;
                                if (temp_v0_3 < var_a1) {
                                    var_a1 = temp_v0_3;
                                } else if (var_a2 < temp_v0_3) {
                                    var_a2 = temp_v0_3;
                                }
                                var_a0 += 1;
                            } while (var_v1 != 3);
                            if ((var_a2 >= sp90) && (sp94 >= var_a1)) {
                                sp88 = temp_v0;
                                sp5C = var_t2;
                                if (point_triangle_2d_xz_intersection(
                                        obj->segment.trans.x_position, 
                                        obj->segment.trans.z_position, 
                                        &vert[temp_t7].x,
                                        &vert[temp_t0->verticesArray[2]].x,
                                        &vert[temp_t0->verticesArray[3]].x
                                ) != 0) {
                                    var_s7 = 1;
                                    obj->unk54->unk0 += (((1.0f - D_800DC884[temp_v1_2]) - obj->unk54->unk0) * 0.2f);
                                }
                            }
                        }
                        facesOffset += 1;
                        batches = &segment->batches[var_t2];
                        var_s4 += 2;
                        if (facesOffset < batches[1].textureIndex) {
                            if (var_s7 != 0) {

                            } else {
                                goto loop_6;
                            }
                        }
                    }
                }
            }
            var_t2 += 0xC;
            i++;
            batches += 0xC;
            if (i < segment->numberOfBatches) {
                if (var_s7 != 0) {

                } else {
                    goto loop_3;
                }
            }
        }
    }
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_8002DE30.s")
#endif
GLOBAL_ASM("asm/non_matchings/tracks/func_8002E234.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002E904.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002EEEC.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002F2AC.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002F440.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002FA64.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002FD74.s")
GLOBAL_ASM("asm/non_matchings/tracks/func_8002FF6C.s")

GLOBAL_ASM("asm/non_matchings/tracks/func_800304C8.s")

#ifdef NON_EQUIVALENT

// Should be functionally equivalent.
void func_80030664(s32 arg0, s16 arg1, s16 arg2, u8 arg3, u8 arg4, u8 arg5) {
    s32 max, min;

    max = arg2;
    min = arg1;
    if (arg2 < arg1) {
        max = arg1;
        min = arg2;
    }
    if (max >= 0x400) {
        max = 0x3FF;
    }
    if (min >= max - 5) {
        min = max - 5;
    }
    gFogData[arg0].addFog.near = 0;
    gFogData[arg0].addFog.far = 0;
    gFogData[arg0].addFog.r = 0;
    gFogData[arg0].addFog.g = 0;
    gFogData[arg0].addFog.b = 0;
    gFogData[arg0].fog.r = arg3 << 0x10;
    gFogData[arg0].fog.g = arg4 << 0x10;
    gFogData[arg0].fog.b = arg5 << 0x10;
    gFogData[arg0].fog.near = min << 0x10;
    gFogData[arg0].fog.far = max << 0x10;
    gFogData[arg0].intendedFog.r = arg3;
    gFogData[arg0].intendedFog.near = min;
    gFogData[arg0].intendedFog.far = max;
    gFogData[arg0].switchTimer = 0;
    gFogData[arg0].fogChanger = 0;
    gFogData[arg0].intendedFog.g = arg4;
    gFogData[arg0].intendedFog.b = arg5;
}

#else
GLOBAL_ASM("asm/non_matchings/tracks/func_80030664.s")
#endif

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
void apply_fog(s32 playerID) {
    gDPSetFogColor(gSceneCurrDisplayList++, gFogData[playerID].fog.r >> 0x10, gFogData[playerID].fog.g >> 0x10, gFogData[playerID].fog.b >> 0x10, 0xFF);
    gSPFogPosition(gSceneCurrDisplayList++, gFogData[playerID].fog.near >> 0x10, gFogData[playerID].fog.far >> 0x10);
}

/**
 * Sets the active viewport's fog target when passed through.
 * Used in courses to make less, or more dense.
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
    
    if (check_if_showing_cutscene_camera()) {
        camera = get_cutscene_camera_segment();
        views = get_viewport_count() + 1;
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

#ifdef NON_EQUIVALENT
void func_80030DE0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    s32 max, min;
    FogData *entry;

    entry = &gFogData[arg0];
    max = arg5;
    min = arg4;
    if (arg5 < arg4) {
        max = arg4;
        min = arg5;
    }
    if (max > 0x3FF) {
        max = 0x3FF;
    }
    if (min >= max - 5) {
        min = max - 5;
    }
    entry->intendedFog.r = arg1;
    entry->intendedFog.g = arg2;
    entry->intendedFog.b = arg3;
    entry->intendedFog.near = min;
    entry->intendedFog.far = max;
    entry->addFog.r = ((arg1 << 16) - entry->fog.r) / arg6;
    entry->addFog.g = ((arg2 << 16) - entry->fog.g) / arg6;
    entry->addFog.b = ((arg3 << 16) - entry->fog.b) / arg6;
    entry->addFog.near = ((min << 16) - entry->fog.near) / arg6;
    entry->addFog.far = ((max << 16) - entry->fog.far) / arg6;
    entry->switchTimer = arg6;
    entry->fogChanger = 0;
}
#else
GLOBAL_ASM("asm/non_matchings/tracks/func_80030DE0.s")
#endif

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