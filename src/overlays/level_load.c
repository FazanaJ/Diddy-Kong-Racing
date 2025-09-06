#include "PRinternal/viint.h"
#include "src/main.h"
#include "src/game.h"
#include "src/memory.h"
#include "src/stacks.h"
#include "src/thread0_epc.h"
#include "src/thread3_main.h"
#include "PR/os_internal.h"
#include "PRinternal/piint.h"
#include "src/joypad.h"
#include "src/video.h"
#include "string.h"
#include "src/asset_loading.h"
#include "src/overlay.h"
#include "src/tracks.h"
#include "src/particles.h"
#include "src/math_util.h"
#include "src/audio.h"
#include "src/save_data.h"
#include "src/printf.h"
#include "src/font.h"
#include "src/textures_sprites.h"
#include "src/object_models.h"
#include "src/objects.h"
#include "src/gzip.h"
#include "src/audio_spatial.h"
#include "src/usb/usb.h"
#include "src/weather.h"
#include "src/menu.h"
#include "src/fade_transition.h"
#include "src/thread30_bgload.h"
#include "save_layout.h"

extern s16 D_800DD32C;
extern s8 D_800DD330;
extern s32 gSPTaskNum;
extern Gfx *gDisplayLists[2];
extern Gfx *gCurrDisplayList;
extern s8 gDrawFrameTimer;
extern s8 gSkipGfxTask;
extern s8 gIsLoading;
extern u8 gTwoPlayerAdvRace;
extern s8 gCurrentDefaultVehicle;
extern s32 *gTempAssetTable;
extern AIBehaviourTable *gAIBehaviourTable;
extern s32 gMapId;
extern s32 gIsInRace;
extern LevelHeader *gCurrentLevelHeader;

FadeTransition D_800DD3F4 = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_OUT, FADE_COLOR_BLACK, 20, 0);

/**
 * Minor func that overrides the user fps cap in certain scenarios because it's not important.
 * FPS is uncapped in menus, and conditionally ingame.
*/
void levelinit_framecap(s32 levelID) {
    s32 cap;
    if (get_game_mode() == GAMEMODE_INGAME) {
        switch (levelID) {
            case ASSET_LEVEL_OPTIONSBACKGROUND:
            case ASSET_LEVEL_WIZPIGAMULETSEQUENCE:
            case ASSET_LEVEL_TTAMULETSEQUENCE:
            case ASSET_LEVEL_FRONTEND:
                cap = 0;
                break;
            default:
                cap = gConfig.frameCap;
                break;
        }
    } else {
        switch (levelID) {
            case ASSET_LEVEL_PARTYSEQUENCE:
            case ASSET_LEVEL_LASTBIT:
            case ASSET_LEVEL_LASTBITB:
                cap = 0;
                break;
            default:
                cap = 0;
                break;
        }
    }
    sched_framecap(cap);
}

/**
 * Set the skill level of the AI.
 * Apply offsets based on game mode.
 */
void aitable_init(s8 *aiLevelTable) {
    s32 temp;
    UNUSED s32 temp2;
    s16 tableIndexCount;
    s8 aiLevel;
    Settings *settings;

    aiLevel = 0;
    if (is_in_tracks_mode() == FALSE) {
        settings = get_settings();
        temp = settings->courseFlagsPtr[settings->courseId];
        if (temp & 2) {
            aiLevel = 1;
        }
        if (temp & 4) {
            aiLevel = 2;
        }
    } else {
        aiLevel = 3;
    }
    if (get_trophy_race_world_id()) {
        aiLevel = 4;
    }
    if (is_in_adventure_two()) {
        aiLevel += 5;
    }
    aiLevel = aiLevelTable[aiLevel];
    if (get_filtered_cheats() & CHEAT_ULTIMATE_AI) {
        aiLevel = 9;
    }
    if (get_game_mode() == GAMEMODE_MENU) {
        aiLevel = 5;
    }
    gTempAssetTable = (s32 *) asset_table_load(ASSET_AI_BEHAVIOUR_TABLE);
    tableIndexCount = 0;
    while (-1 != (s32) gTempAssetTable[tableIndexCount]) {
        tableIndexCount++;
    }
    tableIndexCount--;
    if (aiLevel >= tableIndexCount) {
        aiLevel = 0;
    }
    temp2 = gTempAssetTable[aiLevel];
    temp = gTempAssetTable[aiLevel + 1] - temp2;
    gAIBehaviourTable = mempool_alloc_safe(temp, PP_RAM_ASSETTABLE);
    asset_load(ASSET_AI_BEHAVIOUR, (u32) gAIBehaviourTable, temp2, temp);
    mempool_free(gTempAssetTable);
}

/**
 * Loads and sets up the level header, then loads and sets of the level geometry.
 * Sets weather, fog and active cutscenes where applicable.
 * Official Name: levelInit
 */
void level_load(s32 levelId, s32 numberOfPlayers, s32 entranceId, Vehicle vehicleId, s32 cutsceneId) {
    s8 *someAsset;
    s32 i;
    s32 size;
    s32 var_s0;
    s32 wizpig;
    s32 numPlayers;
    s32 prevLevelID;
    Settings *settings;
    s32 offset;

    rumble_kill();
    if (cutsceneId == -1) {
        cutsceneId = CUTSCENE_NONE;
    }
    if (numberOfPlayers == ZERO_PLAYERS) {
        numPlayers = 1;
        numberOfPlayers = ONE_PLAYER;
    } else {
        numPlayers = 0;
    }

    if (numberOfPlayers == ONE_PLAYER) {
        sndp_set_active_sound_limit(8);
    } else if (numberOfPlayers == TWO_PLAYERS) {
        sndp_set_active_sound_limit(12);
    } else {
        sndp_set_active_sound_limit(16);
    }
    settings = get_settings();
    gTempAssetTable = (s32 *) asset_table_load(ASSET_LEVEL_HEADERS_TABLE);

    for (i = 0; gTempAssetTable[i] != -1; i++) {}
    i--;
    if (levelId >= i) {
        levelId = ASSET_LEVEL_CENTRALAREAHUB;
    }

    offset = gTempAssetTable[levelId];
    size = gTempAssetTable[levelId + 1] - offset;
    gCurrentLevelHeader = (LevelHeader *) mempool_alloc_safe(size, PP_RAM_ASSETTABLE);
    asset_load(ASSET_LEVEL_HEADERS, (u32) gCurrentLevelHeader, offset, size);
    D_800DD330 = 0;
    prevLevelID = levelId;
    if (gCurrentLevelHeader->race_type == RACETYPE_DEFAULT) {
        level_properties_reset();
    }
    if (level_properties_get() == 0 && D_800DD32C == 0) {
        if (gCurrentLevelHeader->race_type == RACETYPE_BOSS) {
            var_s0 = settings->courseFlagsPtr[levelId];
            wizpig = FALSE;
            if (gCurrentLevelHeader->world == WORLD_CENTRAL_AREA ||
                gCurrentLevelHeader->world == WORLD_FUTURE_FUN_LAND) {
                wizpig = TRUE;
            }
            if (!(var_s0 & 1) || wizpig) {
                level_properties_push(levelId, entranceId, vehicleId, cutsceneId);
                if (settings->bosses & (1 << settings->worldId)) {
                    cutsceneId = CUTSCENE_ID_UNK_7;
                } else {
                    cutsceneId = CUTSCENE_ID_UNK_3;
                }
                if (wizpig) {
                    cutsceneId = 0;
                    if (var_s0 & 1) {
                        D_800DD330 = 2;
                    }
                }
                someAsset = (s8 *) get_misc_asset(ASSET_MISC_67);
                for (var_s0 = 0; levelId != someAsset[var_s0]; var_s0 += 2) {}
                levelId = someAsset[var_s0 + 1];
                entranceId = cutsceneId;
            }
        }
        if (gCurrentLevelHeader->race_type == RACETYPE_HUBWORLD) {
            if (gCurrentLevelHeader->world > WORLD_CENTRAL_AREA && gCurrentLevelHeader->world < WORLD_FUTURE_FUN_LAND) {
                var_s0 = gCurrentLevelHeader->world;
                if (settings->keys & (1 << var_s0) &&
                    !(settings->cutsceneFlags & (CUTSCENE_DINO_DOMAIN_KEY << (var_s0 + 31)))) {
                    // Trigger World Key unlocking Challenge Door cutscene.
                    level_properties_push(levelId, entranceId, vehicleId, cutsceneId);
                    settings->cutsceneFlags |= CUTSCENE_DINO_DOMAIN_KEY << (var_s0 + 31);
                    someAsset = (s8 *) get_misc_asset(ASSET_MISC_68);
                    levelId = someAsset[var_s0 - 1];
                    entranceId = 0;
                    cutsceneId = CUTSCENE_ID_UNK_5;
                }
            }
        }
        if (gCurrentLevelHeader->race_type == RACETYPE_HUBWORLD && gCurrentLevelHeader->world == WORLD_CENTRAL_AREA &&
            !(settings->cutsceneFlags & CUTSCENE_WIZPIG_FACE) && settings->wizpigAmulet >= 4) {
            // Trigger wizpig face cutscene
            level_properties_push(levelId, entranceId, vehicleId, cutsceneId);
            entranceId = 0;
            cutsceneId = CUTSCENE_NONE;
            settings->cutsceneFlags |= CUTSCENE_WIZPIG_FACE;
            levelId = ((s8 *) get_misc_asset(ASSET_MISC_68))[4];
        }
    }
    D_800DD32C = 0;
    if (prevLevelID != levelId) {
        mempool_free(gCurrentLevelHeader);
        offset = gTempAssetTable[levelId];
        size = gTempAssetTable[levelId + 1] - offset;
        gCurrentLevelHeader = mempool_alloc_safe(size, PP_RAM_ASSETTABLE);
        asset_load(ASSET_LEVEL_HEADERS, (u32) gCurrentLevelHeader, offset, size);
    }
    mempool_free(gTempAssetTable);
    aitable_init((s8 *) &gCurrentLevelHeader->AILevelTable);
    func_8000CBC0();
    gMapId = levelId;
    for (var_s0 = 0; var_s0 < 7; var_s0++) {
        if ((s32) gCurrentLevelHeader->unk74[var_s0] != -1) {
            gCurrentLevelHeader->unk74[var_s0] =
                (LevelHeader_70 *) get_misc_asset((s32) gCurrentLevelHeader->unk74[var_s0]);
            func_8007F1E8((LevelHeader_70 *) gCurrentLevelHeader->unk74[var_s0]);
        }
    }

    if (cutsceneId == CUTSCENE_ID_UNK_64) {
        if (get_trophy_race_world_id() != 0) {
            if (gCurrentLevelHeader->race_type == RACETYPE_DEFAULT) {
                cutsceneId = CUTSCENE_NONE;
            }
        } else if (is_in_tracks_mode() == 1) {
            if (gCurrentLevelHeader->race_type == RACETYPE_DEFAULT) {
                cutsceneId = CUTSCENE_NONE;
            }
        }
    }
    if (gCurrentLevelHeader->race_type == RACETYPE_DEFAULT || gCurrentLevelHeader->race_type == RACETYPE_BOSS) {
        gIsInRace = TRUE;
    } else {
        gIsInRace = FALSE;
    }
    if (numPlayers && gCurrentLevelHeader->race_type != RACETYPE_CUTSCENE_2) {
        gCurrentLevelHeader->race_type = RACETYPE_CUTSCENE_1;
    }
    music_voicelimit_set(gCurrentLevelHeader->voiceLimit);
    music_volume_reset();
#ifdef USE_DYNLIGHTS
    lights_init(32);
#endif
    var_s0 = VEHICLE_CAR;
    if (vehicleId >= VEHICLE_CAR && vehicleId < NUMBER_OF_PLAYER_VEHICLES) {
        var_s0 = gCurrentLevelHeader->unk4F[vehicleId];
    }
    set_taj_challenge_type(var_s0);
    var_s0 = settings->worldId;
    if (gCurrentLevelHeader->world != -1) {
        settings->worldId = gCurrentLevelHeader->world;
    }
    settings->courseId = levelId;
    if (var_s0 == WORLD_CENTRAL_AREA && settings->worldId > 0) {
        gCurrentDefaultVehicle = get_level_default_vehicle();
    }
    if (settings->worldId == WORLD_CENTRAL_AREA && var_s0 > 0 && gCurrentDefaultVehicle != -1) {
        vehicleId = gCurrentDefaultVehicle;
    }
    set_vehicle_id_for_menu(vehicleId);
    if (gCurrentLevelHeader->race_type == RACETYPE_HUBWORLD) {
        if (settings->worldId - 1 >= 0) {
            var_s0 = 8 << (settings->worldId + 31);
            if (settings->worldId == 5) {
                if (settings->balloonsPtr[0] >= 47) {
                    if (settings->ttAmulet >= 4) {
                        if ((settings->cutsceneFlags & var_s0) == 0) {
                            settings->cutsceneFlags |= var_s0;
                            cutsceneId = CUTSCENE_ID_UNK_5;
                        }
                    }
                }
            } else {
                if (settings->balloonsPtr[settings->worldId] >= 4) {
                    if (!(settings->cutsceneFlags & var_s0)) {
                        settings->cutsceneFlags |= var_s0;
                        cutsceneId = CUTSCENE_ID_UNK_5;
                    }
                }
                var_s0 <<= 5;
                if (settings->balloonsPtr[settings->worldId] >= 8) {
                    if (!(settings->cutsceneFlags & var_s0)) {
                        settings->cutsceneFlags |= var_s0;
                        cutsceneId = CUTSCENE_ID_UNK_5;
                    }
                }
            }
        }
    }

    var_s0 = settings->courseFlagsPtr[levelId]; // Redundant
    if (numberOfPlayers != ONE_PLAYER && gCurrentLevelHeader->race_type == RACETYPE_DEFAULT) {
        cutsceneId = CUTSCENE_ID_UNK_64;
    }
    if ((gCurrentLevelHeader->race_type == RACETYPE_DEFAULT || gCurrentLevelHeader->race_type & RACETYPE_CHALLENGE) &&
        is_in_two_player_adventure()) {
        gTwoPlayerAdvRace = TRUE;
        cutsceneId = CUTSCENE_ID_UNK_64;
    } else {
        gTwoPlayerAdvRace = FALSE;
    }
    if (gCurrentLevelHeader->race_type == RACETYPE_DEFAULT && numPlayers == 0 && is_time_trial_enabled()) {
        cutsceneId = CUTSCENE_ID_UNK_64;
    }
    cutscene_id_set(cutsceneId);
    init_track(gCurrentLevelHeader->geometry, gCurrentLevelHeader->skybox, numberOfPlayers, vehicleId, entranceId,
               gCurrentLevelHeader->collectables, gCurrentLevelHeader->unkBA);
    if (gCurrentLevelHeader->fogNear == 0 && gCurrentLevelHeader->fogFar == 0 && gCurrentLevelHeader->fogR == 0 &&
        gCurrentLevelHeader->fogG == 0 && gCurrentLevelHeader->fogB == 0) {
        for (var_s0 = 0; var_s0 < 4; var_s0++) {
            reset_fog(var_s0);
        }
    } else {
        for (var_s0 = 0; var_s0 < 4; var_s0++) {
            set_fog(var_s0, gCurrentLevelHeader->fogNear, gCurrentLevelHeader->fogFar, (u8) gCurrentLevelHeader->fogR,
                    gCurrentLevelHeader->fogG, gCurrentLevelHeader->fogB);
        }
    }
    settings = get_settings();
    if (gCurrentLevelHeader->world != -1) {
        settings->worldId = gCurrentLevelHeader->world;
    }
    settings->courseId = levelId;
    if (gCurrentLevelHeader->weatherEnable > 0) {
        weather_reset(gCurrentLevelHeader->weatherType, gCurrentLevelHeader->weatherEnable,
                      gCurrentLevelHeader->weatherVelX << 8, gCurrentLevelHeader->weatherVelY << 8,
                      gCurrentLevelHeader->weatherVelZ << 8, gCurrentLevelHeader->weatherIntensity * 257,
                      gCurrentLevelHeader->weatherOpacity * 257);
        weather_clip_planes(-1, -512);
    }
    if (gCurrentLevelHeader->skyDome == -1) {
        gCurrentLevelHeader->unkA4 = load_texture((s32) gCurrentLevelHeader->unkA4);
        gCurrentLevelHeader->unkA8 = 0;
        gCurrentLevelHeader->unkAA = 0;
    }
    if ((s32) gCurrentLevelHeader->pulseLightData != -1) {
        gCurrentLevelHeader->pulseLightData =
            (PulsatingLightData *) get_misc_asset((s32) gCurrentLevelHeader->pulseLightData);
        init_pulsating_light_data(gCurrentLevelHeader->pulseLightData);
    }
    cam_set_fov(gCurrentLevelHeader->cameraFOV);
    bgdraw_primcolour(gCurrentLevelHeader->bgColorRed, gCurrentLevelHeader->bgColorGreen,
                      gCurrentLevelHeader->bgColorBlue);
    video_delta_reset();
    levelinit_framecap(levelId);
}

void levelload_free_menu(void) {
    gIsLoading = TRUE;
    mempool_free_timer(0);
    // @recomp Wait for the current graphics task.
    if (gSkipGfxTask == FALSE && bgload_active() == FALSE) {
        if (gDrawFrameTimer != 1) {
            gfxtask_wait();
        }
        gSkipGfxTask = TRUE;
    }
    level_free();
    if (gCurrentMenuId != MENU_VIDEO_OPTIONS) {
        transition_begin(&D_800DD3F4);
    }
    //reset_particles();
    hud_free();
    mempool_free_timer(2);
}

void levelload_free_game(void) {
    mempool_free_timer(0);
    if (gSkipGfxTask == FALSE) {
        if (gDrawFrameTimer != 1) {
            gfxtask_wait();
        }
        gSkipGfxTask = TRUE;
    }
    level_free();
    transition_begin(&D_800DD3F4);
    //reset_particles();
    hud_free();
    gCurrDisplayList = gDisplayLists[gSPTaskNum];
    gDPFullSync(gCurrDisplayList++);
    gSPEndDisplayList(gCurrDisplayList++);
    mempool_free_timer(2);
}