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
#include "src/thread30_bgload.h"
#include "src/game_text.h"
#include "src/borders.h"

// GLOBAL_ASM

extern s8 gGameNumPlayers;
extern Vehicle gLevelDefaultVehicleID;
extern s8 gLevelSettings[16];
extern s8 gIsLoading;
extern s32 gGameCurrentCutscene;
extern s32 D_801234FC;
extern s32 gGameCurrentEntrance;
extern s32 gPlayableMapId;
extern Gfx *gCurrDisplayList;
extern Mtx *gGameCurrMatrix;
extern Vertex *gGameCurrVertexList;
extern Triangle *gGameCurrTriList;
extern s8 gPostRaceViewPort;
extern s8 gIsPaused;
extern s32 gGameMode;
extern s32 gRenderMenu;
extern Gfx *gDisplayLists[2];
extern s32 gSPTaskNum;
extern Settings *gSettingsPtr;
extern Vehicle gMenuVehicleID;

/**
 * Used in menus, update objects and draw the game.
 * In the tracks menu, this only runs if there's a track actively loaded.
 */
void update_menu_scene(s32 updateRate) {
    if (bgload_active() == FALSE) {
        if (gMenuStopUpdating == FALSE) {
            obj_update(updateRate);
            gParticlePtrList_flush();
            ainode_update();
        }
        render_scene(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
        process_onscreen_textbox(updateRate);
        rdp_init(&gCurrDisplayList);
        divider_draw(&gCurrDisplayList);
        divider_clear_coverage(&gCurrDisplayList);
    }
}

/**
 * Main function for handling behaviour in menus.
 * Runs the menu code, with a simplified object update and scene rendering system.
 */
void mode_menu(s32 updateRate) {
    s32 menuLoopResult;
    s32 temp;
    s32 playerVehicle;
    s32 temp5;

    gIsPaused = FALSE;
    gPostRaceViewPort = NULL;
    if (!gIsLoading && gRenderMenu) {
        update_menu_scene(updateRate);
    }
    menuLoopResult =
        menu_loop(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
    gRenderMenu = TRUE;
    if (menuLoopResult == -2) {
        gRenderMenu = FALSE;
        return;
    }
    if (menuLoopResult != -1 && menuLoopResult & MENU_RESULT_FLAGS_200) {
        unload_level_menu();
        gCurrDisplayList = gDisplayLists[gSPTaskNum];
        gDPFullSync(gCurrDisplayList++);
        gSPEndDisplayList(gCurrDisplayList++);
        gPlayableMapId = menuLoopResult & 0x7F;
        gLevelDefaultVehicleID = leveltable_vehicle_default(gPlayableMapId);
        gGameCurrentEntrance = 0;
        gGameCurrentCutscene = CUTSCENE_ID_UNK_64;
        gGameMode = GAMEMODE_INGAME;
        gIsPaused = FALSE;
        gPostRaceViewPort = NULL;
        load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
        safe_mark_write_save_file(get_save_file_index());
        return;
    }
    if (menuLoopResult != -1 && menuLoopResult & MENU_RESULT_FLAGS_100) {
        unload_level_game();
        gIsPaused = FALSE;
        gPostRaceViewPort = NULL;
        switch (menuLoopResult & 0x7F) {
            case MENU_RESULT_TRACKS_MODE:
                load_menu_with_level_background(MENU_TRACK_SELECT, SPECIAL_MAP_ID_NO_LEVEL, 1);
                break;
            case MENU_RESULT_UNK14:
                gPlayableMapId = ASSET_LEVEL_CENTRALAREAHUB;
                gGameCurrentEntrance = 0;
                gGameCurrentCutscene = CUTSCENE_ID_UNK_64;
                gGameMode = GAMEMODE_INGAME;
                load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
                safe_mark_write_save_file(get_save_file_index());
                break;
            case MENU_RESULT_RETURN_TO_GAME:
                gGameCurrentEntrance = 0;
                gPlayableMapId = gLevelSettings[0];
                gGameCurrentCutscene = CUTSCENE_ID_UNK_64;
                gGameMode = GAMEMODE_INGAME;
                temp5 = gLevelSettings[1];
                if (gLevelSettings[15] >= 0) {
                    gGameCurrentEntrance = gLevelSettings[15];
                }
                temp = gLevelSettings[temp5 + 8];
                if (temp >= 0) {
                    gGameCurrentCutscene = temp;
                }
                load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
                safe_mark_write_save_file(get_save_file_index());
                break;
            case MENU_RESULT_UNK2:
                gGameMode = GAMEMODE_INGAME;
                load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
                break;
            case MENU_RESULT_UNK3:
                gGameMode = GAMEMODE_INGAME;
                gPlayableMapId = gLevelSettings[0];
                gGameCurrentEntrance = gLevelSettings[15];
                gGameCurrentCutscene = gLevelSettings[gLevelSettings[1] + 8];
                gLevelDefaultVehicleID = leveltable_vehicle_default(gPlayableMapId);
                load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
                break;
            default:
                load_menu_with_level_background(MENU_TITLE, SPECIAL_MAP_ID_NO_LEVEL, 0);
                break;
        }
        return;
    }
    if (menuLoopResult & MENU_RESULT_FLAGS_80 && menuLoopResult != -1) {
        unload_level_menu();
        gCurrDisplayList = gDisplayLists[gSPTaskNum];
        gDPFullSync(gCurrDisplayList++);
        gSPEndDisplayList(gCurrDisplayList++);

        menuLoopResult &= 0x7f;
        gLevelSettings[1] = menuLoopResult;
        gLevelSettings[0] = gPlayableMapId;

        gPlayableMapId = gLevelSettings[menuLoopResult + 2];
        gGameCurrentEntrance = gLevelSettings[menuLoopResult + 4];
        gGameMode = GAMEMODE_INGAME;
        gGameCurrentCutscene = gLevelSettings[menuLoopResult + 12];
        playerVehicle = get_player_selected_vehicle(PLAYER_ONE);
        gGameNumPlayers = gSettingsPtr->gNumRacers - 1;
        load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, playerVehicle);
        D_801234FC = 0;
        gLevelDefaultVehicleID = gMenuVehicleID;
        return;
    }
    if (menuLoopResult > 0) {
        unload_level_menu();
        gCurrDisplayList = gDisplayLists[gSPTaskNum];
        gDPFullSync(gCurrDisplayList++);
        gSPEndDisplayList(gCurrDisplayList++);
        gGameMode = GAMEMODE_INGAME;
        load_next_ingame_level(menuLoopResult, -1, gLevelDefaultVehicleID);
        if (gSettingsPtr->newGame && !is_in_tracks_mode()) {
            music_change_on();
            gSettingsPtr->newGame = FALSE;
        }
    }
}