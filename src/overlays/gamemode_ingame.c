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
#include "src/borders.h"
#include "src/game_text.h"

extern s8 gGameNumPlayers;
extern Vehicle gLevelDefaultVehicleID;
extern s8 gLevelSettings[16];
extern s8 gIsLoading;
extern s32 gGameCurrentCutscene;
extern s8 gNextMap;
extern s32 D_801234FC;
extern s8 gLevelLoadType;
extern s32 gGameCurrentEntrance;
extern s32 D_801234F8;
extern s32 gPlayableMapId;
extern Gfx *gCurrDisplayList;
extern Mtx *gGameCurrMatrix;
extern Vertex *gGameCurrVertexList;
extern Triangle *gGameCurrTriList;
extern s8 gFutureFunLandLevelTarget;
extern s8 gPostRaceViewPort;
extern s8 gPauseLockTimer;
extern s8 gIsPaused;
extern s32 gGameMode;
extern s8 gDrumstickSceneLoadTimer;
extern s16 gLevelLoadTimer;

/**
 * The main behaviour function involving all of the ingame stuff.
 * Involves the updating of all objects and setting up the render scene.
 */
void mode_game_temp(s32 updateRate) {
    s32 buttonPressedInputs, buttonHeldInputs, i, loadContext, sp3C;

    loadContext = LEVEL_CONTEXT_NONE;
    buttonHeldInputs = 0;
    buttonPressedInputs = 0;

    // Get input data for all 4 players.
    for (i = 0; i < get_active_player_count(); i++) {
        buttonHeldInputs |= input_held(i);
        buttonPressedInputs |= input_pressed(i);
    }
    
    // Update all objects
    if (!gIsPaused) {
        obj_update(updateRate);
        if (check_if_showing_cutscene_camera() == 0 || get_race_countdown()) {
            if (buttonPressedInputs & START_BUTTON && level_properties_get() == 0 && gDrumstickSceneLoadTimer == 0 &&
                gGameMode == GAMEMODE_INGAME && gPostRaceViewPort == FALSE && gLevelLoadTimer == 0 &&
                gPauseLockTimer == 0) {
                buttonPressedInputs = 0;
                gIsPaused = TRUE;
                menu_pause_init();
            }
        }
    } else {
        set_anti_aliasing(TRUE);
    }
    gPauseLockTimer -= updateRate;
    if (gPauseLockTimer < 0) {
        gPauseLockTimer = 0;
    }
    if (gPostRaceViewPort) {
        gIsPaused = FALSE;
    }
    gParticlePtrList_flush();
    ainode_update();
    render_scene(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
    if (gGameMode == GAMEMODE_INGAME) {
        // Ignore the user's L/R/Z buttons.
        buttonHeldInputs &= ~(L_TRIG | R_TRIG | Z_TRIG);
    }
    if (gPostRaceViewPort) {
        i = menu_postrace(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, updateRate);
        switch (i) {
            case POSTRACE_OPT_2:
                buttonHeldInputs |= (L_TRIG | Z_TRIG);
                break;
            case POSTRACE_OPT_1:
                gPostRaceViewPort = FALSE;
                func_8006D8F0(-1);
                break;
            case POSTRACE_OPT_4:
                level_properties_reset();
                gDrumstickSceneLoadTimer = 0;
                buttonHeldInputs |= (L_TRIG | R_TRIG);
                break;
            case POSTRACE_OPT_5:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_TRACK_SELECT;
                break;
            case POSTRACE_OPT_8:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_RESULTS;
                break;
            case POSTRACE_OPT_9:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_TROPHY_ROUND;
                break;
            case POSTRACE_OPT_10:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_TROPHY_RESULTS;
                break;
            case POSTRACE_OPT_11:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_UNUSED;
                break;
            case POSTRACE_OPT_12:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_CHARACTER_SELECT;
                break;
            case POSTRACE_OPT_13:
                buttonHeldInputs |= L_TRIG, loadContext = LEVEL_CONTEXT_UNK7;
                break;
        }
    }
    process_onscreen_textbox(updateRate);
    i = textbox_visible();
    if (i != 0) {
        if (i == 2) {
            gIsPaused = TRUE;
        }
        if (textbox_visible() != 2) {
            gIsPaused = FALSE;
            menu_close_dialogue();
        }
    }
    
    if (gIsPaused) {
        i = menu_pause_loop(&gCurrDisplayList, updateRate);
        switch (i) {
            case PAUSE_CONTINUE:
                gIsPaused = FALSE;
                break;
            case PAUSE_RESET:
                sound_clear_delayed();
                reset_delayed_text();
                if (func_80023568() != 0 && is_in_two_player_adventure()) {
                    swap_lead_player();
                }
                buttonHeldInputs |= (L_TRIG | Z_TRIG);
                break;
            case PAUSE_QUIT_LOBBY:
                sound_clear_delayed();
                reset_delayed_text();
                if (func_80023568() != 0 && is_in_two_player_adventure()) {
                    swap_lead_player();
                }
                buttonHeldInputs |= L_TRIG;
                break;
            case PAUSE_QUIT_TRACKS:
                loadContext = LEVEL_CONTEXT_TRACK_SELECT;
                reset_delayed_text();
                buttonHeldInputs |= L_TRIG;
                break;
            case PAUSE_QUIT_CHARSELECT:
                loadContext = LEVEL_CONTEXT_CHARACTER_SELECT;
                reset_delayed_text();
                buttonHeldInputs |= L_TRIG;
                break;
            case PAUSE_OPT_6:
                gIsPaused = FALSE;
                break;
            case PAUSE_QUIT_CHALLENGE:
                mode_end_taj_race(CHALLENGE_END_QUIT);
                gIsPaused = FALSE;
                break;
            case PAUSE_OPT_4:
                gDrumstickSceneLoadTimer = 0;
                sound_clear_delayed();
                reset_delayed_text();
                level_properties_reset();
                buttonHeldInputs |= (L_TRIG | R_TRIG);
                break;
        }
    }
    divider_draw(&gCurrDisplayList);
    hud_render_general(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, updateRate);
    divider_clear_coverage(&gCurrDisplayList);
    if (gFutureFunLandLevelTarget) {
        if (func_800214C4() != 0) {
            gPlayableMapId = ASSET_LEVEL_FUTUREFUNLANDHUB;
            D_801234F8 = TRUE;
            gGameCurrentEntrance = 0;
            gFutureFunLandLevelTarget = FALSE;
        }
    }
    sp3C = FALSE;
    if (gDrumstickSceneLoadTimer) {
        gDrumstickSceneLoadTimer -= updateRate;
        if (gDrumstickSceneLoadTimer <= 0) {
            gDrumstickSceneLoadTimer = 0;
            level_properties_push(ASSET_LEVEL_CENTRALAREAHUB, 0, VEHICLE_CAR, CUTSCENE_ID_NONE);
            level_properties_push(ASSET_LEVEL_WIZPIGAMULETSEQUENCE, 0, -1, CUTSCENE_ID_UNK_A);
            sp3C = TRUE;
        }
    }
    if (gLevelLoadTimer > 0) {
        gLevelLoadTimer -= updateRate;
        if (gLevelLoadTimer <= 0) {
            buttonHeldInputs = L_TRIG;
            sp3C = TRUE;
            switch (gLevelLoadType) {
                case LEVEL_LOAD_UNK1:
                    buttonHeldInputs = (L_TRIG | Z_TRIG);
                    break;
                case LEVEL_LOAD_TROPHY_RACE:
                    loadContext = LEVEL_CONTEXT_TROPHY_ROUND;
                    trophyround_adventure();
                    D_801234FC = 2;
                    break;
                case LEVEL_LOAD_LIGHTHOUSE_CUTSCENE:
                    gFutureFunLandLevelTarget = TRUE;
                    // fall-through
                case LEVEL_LOAD_FUTURE_FUN_LAND:
                    D_801234F8 = TRUE;
                    gPlayableMapId = gNextMap;
                    gGameCurrentEntrance = 0;
                    gGameCurrentCutscene = 0;
                    buttonHeldInputs = 0;
                    break;
            }
            gLevelLoadType = LEVEL_LOAD_NORMAL;
            gLevelLoadTimer = 0;
        }
    }
    if (sp3C) {
        if (level_properties_get() != 0) {
            level_properties_pop(&gPlayableMapId, &gGameCurrentEntrance, &i, &gGameCurrentCutscene);
            set_frame_blackout_timer();
            if (gPlayableMapId < 0) {
                if (gPlayableMapId == SPECIAL_MAP_ID_NO_LEVEL || gPlayableMapId == SPECIAL_MAP_ID_UNK_NEG10) {
                    if (gPlayableMapId == SPECIAL_MAP_ID_UNK_NEG10 && is_in_two_player_adventure()) {
                        swap_lead_player();
                    }
                    buttonHeldInputs |= L_TRIG;
                    D_801234FC = 2;
                } else {
                    buttonHeldInputs = 0;
                    D_801234FC = 1;
                    loadContext = LEVEL_CONTEXT_CREDITS;
                }
            } else {
                D_801234FC = 0;
                D_801234F8 = TRUE;
                buttonHeldInputs = 0;
            }
        }
    } else {
        sp3C = func_8006C300();
        if (level_properties_get()) {
            if (gLevelLoadTimer == 0) {
                i = func_800214C4();
                if ((i != 0) || ((buttonPressedInputs & A_BUTTON) && (sp3C != 0))) {
                    if (sp3C != 0) {
                        music_change_on();
                    }
                    set_frame_blackout_timer();
                    level_properties_pop(&gPlayableMapId, &gGameCurrentEntrance, &i, &gGameCurrentCutscene);
                    if (gPlayableMapId < 0) {
                        if (gPlayableMapId == -1 || gPlayableMapId == -10) {
                            if (gPlayableMapId == -10 && is_in_two_player_adventure()) {
                                swap_lead_player();
                            }
                            buttonHeldInputs |= L_TRIG;
                            D_801234FC = 2;
                        } else {
                            buttonHeldInputs = 0;
                            D_801234FC = 1;
                            loadContext = LEVEL_CONTEXT_CREDITS;
                        }
                    } else {
                        D_801234F8 = TRUE;
                    }
                }
            }
        }
    }
    if ((buttonHeldInputs & L_TRIG && gGameMode == GAMEMODE_INGAME) || D_801234FC != 0) {
        gIsPaused = FALSE;
        gLevelLoadTimer = 0;
        gPostRaceViewPort = FALSE;
        unload_level_game();
        safe_mark_write_save_file(get_save_file_index());
        if (loadContext) {
            gIsLoading = FALSE;
            switch (loadContext) {
                case LEVEL_CONTEXT_TRACK_SELECT:
                    // Go to track select menu from "Select Track" option in tracks menu.
                    load_menu_with_level_background(MENU_TRACK_SELECT, SPECIAL_MAP_ID_NO_LEVEL, 1);
                    break;
                case LEVEL_CONTEXT_RESULTS:
                    load_menu_with_level_background(MENU_RESULTS, ASSET_LEVEL_TROPHYRACE, 0);
                    break;
                case LEVEL_CONTEXT_TROPHY_ROUND:
                    load_menu_with_level_background(MENU_TROPHY_RACE_ROUND, ASSET_LEVEL_TROPHYRACE, 0);
                    break;
                case LEVEL_CONTEXT_TROPHY_RESULTS:
                    load_menu_with_level_background(MENU_TROPHY_RACE_RANKINGS, ASSET_LEVEL_TROPHYRACE, 0);
                    break;
                case LEVEL_CONTEXT_UNUSED:
                    // Trophy race related?
                    load_menu_with_level_background(MENU_UNUSED_22, ASSET_LEVEL_TROPHYRACE, 0);
                    break;
                case LEVEL_CONTEXT_CHARACTER_SELECT:
                    // Go to character select menu from "Select Character" option in tracks menu.
                    i = 0;
                    if (is_drumstick_unlocked()) {
                        i ^= 1;
                    }
                    if (is_tt_unlocked()) {
                        i ^= 3;
                    }
                    charselect_prev(1, 0);
                    load_menu_with_level_background(MENU_CHARACTER_SELECT, ASSET_LEVEL_CHARACTERSELECT, i);
                    break;
                case LEVEL_CONTEXT_UNK7:
                    gIsLoading = TRUE;
                    load_menu_with_level_background(MENU_NEWGAME_CINEMATIC, SPECIAL_MAP_ID_NO_LEVEL, 0);
                    gIsLoading = FALSE;
                    break;
                case LEVEL_CONTEXT_CREDITS:
                    load_menu_with_level_background(MENU_CREDITS, SPECIAL_MAP_ID_NO_LEVEL, 0);
                    break;
            }
        } else if (D_801234FC == 1) {
            if (gLevelSettings[2] == -1) {
                load_menu_with_level_background(MENU_UNUSED_8, SPECIAL_MAP_ID_NO_LEVEL, 0);
            } else {
                gIsLoading = TRUE;
                load_menu_with_level_background(MENU_TRACK_SELECT_ADVENTURE, SPECIAL_MAP_ID_NO_LEVEL, -1);
            }
        } else if (!(buttonHeldInputs & R_TRIG)) {
            if (!(buttonHeldInputs & Z_TRIG)) {
                gPlayableMapId = gLevelSettings[0];
                gGameCurrentEntrance = gLevelSettings[15];
                gGameCurrentCutscene = gLevelSettings[gLevelSettings[1] + 8];
                gLevelDefaultVehicleID = leveltable_vehicle_default(gPlayableMapId);
                if (gGameCurrentCutscene < 0) {
                    gGameCurrentCutscene = CUTSCENE_ID_UNK_64;
                }
            }
            load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
        } else {
            safe_mark_write_save_file(get_save_file_index());
            load_menu_with_level_background(MENU_TITLE, SPECIAL_MAP_ID_NO_LEVEL, 0);
        }
        D_801234FC = 0;
    }
    if (D_801234F8) {
        gPostRaceViewPort = FALSE;
        unload_level_game();
        load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
        safe_mark_write_save_file(get_save_file_index());
        D_801234F8 = FALSE;
    }
}