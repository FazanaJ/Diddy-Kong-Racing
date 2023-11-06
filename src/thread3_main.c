/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x8006C330 */

#include "thread3_main.h"

#include <PR/os_cont.h>
#include <PR/gu.h>
#include <PR/os_time.h>
#include "memory.h"
#include "types.h"
#include "macros.h"
#include "structs.h"
#include "f3ddkr.h"
#include "asset_enums.h"
#include "asset_loading.h"
#include "menu.h"
#include "video.h"
#include "gzip.h"
#include "printf.h"
#include "thread0_epc.h"
#include "thread30_track_loading.h"
#include "weather.h"
#include "audio.h"
#include "objects.h"
#include "camera.h"
#include "save_data.h"
#include "rcp.h"
#include "audiosfx.h"
#include "audiomgr.h"
#include "lights.h"
#include "textures_sprites.h"
#include "PR/os_internal.h"
#include "printf.h"
#include "fade_transition.h"
#include "borders.h"
#include "audio_spatial.h"
#include "tracks.h"
#include "game_text.h"
#include "game_ui.h"
#include "main.h"
#include "object_models.h"
#include "racer.h"
#include "particles.h"
#include "math_util.h"
#include "controller.h"
#include "game.h"
#include "string.h"

#include "config.h"
#ifdef ENABLE_USB
#include "usb/dkr_usb.h"
#endif

/************ .data ************/

s8 sAntiPiracyTriggered = 0;
s32 gSaveDataFlags = 0; //Official Name: load_save_flags
s32 gScreenStatus = OSMESG_SWAP_BUFFER;
s32 sControllerStatus = 0;
s8 gSkipGfxTask = FALSE;
s8 gDrumstickSceneLoadTimer = 0;
s16 gLevelLoadTimer = 0;
s8 gPauseLockTimer = 0; // If this is above zero, the player cannot pause the game.
s8 gFutureFunLandLevelTarget = FALSE;
s8 gDmemInvalid = FALSE;
s8 gDrawFrameTimer = 0;
s32 gNumF3dCmdsPerPlayer[4] = NUM_GFX_COMMANDS;
s32 gNumHudVertsPerPlayer[4] = NUM_VERTICES;
s32 gNumHudMatPerPlayer[4] = NUM_MATRICES;
s32 gNumHudTrisPerPlayer[4] = NUM_HUD_VERTS;
FadeTransition D_800DD3F4 = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_OUT, FADE_COLOR_BLACK, 20, 0);
s32 sLogicUpdateRate = LOGIC_5FPS;
f32 sLogicUpdateRateF = 12.0f;
FadeTransition gDrumstickSceneTransition = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_NONE, FADE_COLOR_WHITE, 30, -1);
FadeTransition gLevelFadeOutTransition = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_NONE, FADE_COLOR_BLACK, 30, -1);
FadeTransition D_800DD424 = FADE_TRANSITION(FADE_FULLSCREEN, FADE_FLAG_NONE, FADE_COLOR_BLACK, 260, -1);
s32 gNumGfxTasksAtScheduler = 0;

/*******************************/

/************ .bss ************/

Gfx *gDisplayLists[2];
Gfx *gCurrDisplayList;
MatrixS *gMatrixHeap[2];
MatrixS *gGameCurrMatrix;
Vertex *gVertexHeap[2];
Vertex *gGameCurrVertexList;
TriangleList *gTriangleHeap[2];
TriangleList *gGameCurrTriList;
s8 gLevelSettings[16];
OSSched gMainSched; // 0x288 / 648 bytes
u64 gSchedStack[THREAD5_STACK / sizeof(u64)];
s32 gSPTaskNum;
s32 gGameMode;
s32 gRenderMenu; // I don't think this is ever not 1
// Similar to gMapId, but is 0 if not currently playing a level (e.g. start menu).
s32 gPlayableMapId;
s32 D_801234F8;
s32 D_801234FC;
s32 gGameNumPlayers;
s32 gGameCurrentEntrance;
s32 gGameCurrentCutscene;
s32 gPrevPlayerCount;
Settings *gSettingsPtr;
s8 gIsLoading;
s8 gIsPaused;
s8 gPostRaceViewPort;
Vehicle gLevelDefaultVehicleID;
Vehicle gMenuVehicleID; // Looks to be the current level's vehicle ID.
s32 sBootDelayTimer;
s8 gLevelLoadType;
s8 gNextMap;
s32 gCurrNumF3dCmdsPerPlayer;
s32 gCurrNumHudMatPerPlayer;
s32 gCurrNumHudTrisPerPlayer;
s32 gCurrNumHudVertsPerPlayer;
OSScClient *gNMISched[3];
OSMesg gGameMesgBuf[3];
OSMesgQueue gGameMesgQueue;
s32 gNMIMesgBuf; //Official Name: resetPressed

/******************************/


/**
 * Main looping function for the main thread.
 * Official Name: mainThread
 */
void thread3_main(UNUSED void *unused) {
    OSMesg mesg;

    init_game();
    gSaveDataFlags = handle_save_data_and_read_controller(gSaveDataFlags, 0);
    sBootDelayTimer = 0;
    gGameMode = GAMEMODE_INTRO;

    while (1) {
        while (gNumGfxTasksAtScheduler < 2) {
            main_game_loop();
        }

        osRecvMesg(&gGameMesgQueue, &mesg, OS_MESG_BLOCK);

        switch ((s32) mesg) {
        case OS_SC_DONE_MSG:
            gNumGfxTasksAtScheduler--;
            break;

        case OS_SC_PRE_NMI_MSG:
            gNMIMesgBuf = TRUE;
            func_80072708();
            audioStopThread();
            stop_thread30();
            IO_WRITE(SP_STATUS_REG, SP_SET_HALT | SP_CLR_INTR_BREAK | SP_CLR_YIELD | SP_CLR_YIELDED | SP_CLR_TASKDONE | SP_CLR_RSPSIGNAL
                    | SP_CLR_CPUSIGNAL | SP_CLR_SIG5 | SP_CLR_SIG6 | SP_CLR_SIG7);
            osDpSetStatus(DPC_SET_XBUS_DMEM_DMA | DPC_CLR_FREEZE | DPC_CLR_FLUSH | DPC_CLR_TMEM_CTR | DPC_CLR_PIPE_CTR
                    | DPC_CLR_CMD_CTR | DPC_CLR_CMD_CTR);
            while (1);
            break;
        }
#ifdef PUPPYPRINT_DEBUG
    gPokeThread[0] = 0;
    gThread3Stack[0]++;
    gThread3Stack[THREAD3_STACK / sizeof(u64) - 1]++;
    if (gThread3Stack[THREAD3_STACK / sizeof(u64) - 1] != gThread3Stack[0]) {
        puppyprint_assert("Thread 3 Stack overflow");
    }
#endif
    }
}


struct ConfigOptions gConfig;
u8 gHideHUD = FALSE;

void init_config(void) {
    bzero(&gConfig, sizeof(gConfig));
    if (gPlatform & EMULATOR) {
        gConfig.noCutbacks = TRUE;
        gConfig.antiAliasing = 1;
        gConfig.dedither = TRUE;
        gConfig.perfMode = FALSE;
    }
    gConfig.perfMode = TRUE;
}

/**
 * Setup all of the necessary pieces required for the game to function.
 * This includes the memory pool. controllers, video, audio, core assets and more.
 * Official Name: mainInitGame
 */
void init_game(void) {

    init_main_memory_pool();
    init_rzip(); // Initialise gzip decompression related things
#ifndef NO_ANTIPIRACY
    sAntiPiracyTriggered = TRUE;
    if (check_imem_validity()) {
        sAntiPiracyTriggered = FALSE;
    }
#endif
    gIsLoading = FALSE;
    gLevelDefaultVehicleID = VEHICLE_CAR;

    osCreateScheduler(&gMainSched, &gSchedStack[THREAD5_STACK / sizeof(u64)], OS_SC_PRIORITY, (u8) 0, 1);
    init_video(VIDEO_MODE_LOWRES_LPN);
    init_PI_mesg_queue();
    setup_gfx_mesg_queues();
    audio_init(&gMainSched);
    func_80008040(); // Should be very similar to allocate_object_model_pools
    sControllerStatus = init_controllers();
    tex_init_textures();
    allocate_object_model_pools();
    allocate_object_pools();
    diPrintfInit();
    allocate_ghost_data();
    init_particle_assets();
    init_weather();
    calc_and_alloc_heap_for_settings();
    default_alloc_displaylist_heap();
    load_fonts();
    init_controller_paks();
    func_80081218(); // init_save_data
    create_and_start_thread30();
#ifdef ENABLE_USB
    init_usb_thread();
#endif
    osCreateMesgQueue(&gGameMesgQueue, gGameMesgBuf, 3);
    osScAddClient(&gMainSched, (OSScClient*) gNMISched, &gGameMesgQueue, OS_SC_ID_VIDEO);
    gNMIMesgBuf = 0;
    gGameCurrentEntrance = 0;
    gGameCurrentCutscene = 0;
    gSPTaskNum = 0;

    gSorterHeap = allocate_from_main_pool(0x4000, MEMP_MISC);
    gSorterPos = (u32) gSorterHeap + MATERIAL_SORT_BUFFER - sizeof(RenderNodeTrack);

    gCurrDisplayList = gDisplayLists[gSPTaskNum];
    gDPFullSync(gCurrDisplayList++);
    gSPEndDisplayList(gCurrDisplayList++);
    get_platform();
    init_config();
    load_game_text_table();

    osSetTime(0);
}

#ifdef FIFO_UCODE
s8 suCodeSwitch = 0;
#endif

u32 sPrevTime = 0;
u32 sDeltaTime = 0;
s32 sTotalTime = 0;

/**
 * The main gameplay loop.
 * Contains all game logic, audio and graphics processing.
 */
void main_game_loop(void) {
    s32 framebufferSize;
#ifdef PUPPYPRINT_DEBUG
    profiler_reset_values();
    profiler_snapshot(THREAD4_START);
#endif

#ifdef ENABLE_USB
    tick_usb_thread();
#endif

    if (gVideoSkipNextRate) {
        sLogicUpdateRate = LOGIC_60FPS;
        sLogicUpdateRateF = 1.0f;
        sTotalTime = 0;
        sPrevTime = 0;
        gVideoSkipNextRate = FALSE;
    } else {
        sDeltaTime = osGetCount() - sPrevTime;
        sPrevTime = osGetCount();
        sLogicUpdateRateF = (f32) sDeltaTime / (f32) OS_USEC_TO_CYCLES(16666);
        if (sLogicUpdateRateF <= 0.0001f) {
            sLogicUpdateRateF = 0.0001f;
        }
        if (osTvType == TV_TYPE_PAL) {
            sLogicUpdateRateF *= 1.2f;
        }
        sTotalTime += OS_CYCLES_TO_USEC(sDeltaTime);
        sTotalTime -= 16666;
        sLogicUpdateRate = LOGIC_60FPS;
        while (sTotalTime > 16666) {
            sTotalTime -= 16666;
            sLogicUpdateRate++;
            if (sLogicUpdateRate == 4) {
                sTotalTime = 0;
            }
            if (sLogicUpdateRate > LOGIC_12FPS) {
                sLogicUpdateRate = LOGIC_12FPS;
            }
        }
    }

    gCurrDisplayList = gDisplayLists[gSPTaskNum];
    gGameCurrMatrix = gMatrixHeap[gSPTaskNum];
    gGameCurrVertexList = gVertexHeap[gSPTaskNum];
    gGameCurrTriList = gTriangleHeap[gSPTaskNum];

    gSPSegment(gCurrDisplayList++, 0, 0 + K0BASE);
    gSPSegment(gCurrDisplayList++, 1, (s32) gVideoLastFramebuffer + K0BASE);
    gSPSegment(gCurrDisplayList++, 2, (s32) gVideoLastDepthBuffer + K0BASE);
    gSPSegment(gCurrDisplayList++, 4, (s32) gVideoLastFramebuffer - 0x500 + K0BASE);
    init_rsp(&gCurrDisplayList);
    init_rdp_and_framebuffer(&gCurrDisplayList);
    render_background(&gCurrDisplayList, (Matrix *) &gGameCurrMatrix, TRUE); 
    gSaveDataFlags = handle_save_data_and_read_controller(gSaveDataFlags, sLogicUpdateRate);
    switch (gGameMode) {
        case GAMEMODE_INTRO: // Pre-boot screen
            pre_intro_loop();
            break;
        case GAMEMODE_MENU: // In a menu
            menu_logic_loop(sLogicUpdateRate);
            break;
        case GAMEMODE_INGAME: // In game (Controlling a character)
            ingame_logic_loop(sLogicUpdateRate);
            break;
    }

    // This is a good spot to place custom text if you want it to overlay it over ALL the
    // menus & gameplay.
    
#ifdef ENABLE_USB
#ifdef SHOW_USB_INFO
    render_usb_info();
#endif
#endif

    sound_update_queue(sLogicUpdateRate);
    print_debug_strings(&gCurrDisplayList);
    render_dialogue_boxes(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList);
    close_dialogue_box(4);
    assign_dialogue_box_id(4);
    // handle_transitions will perform the logic of transitions and return the transition ID.
    if (handle_transitions(sLogicUpdateRate)) {
        render_fade_transition(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList);
    }
    if ((sBootDelayTimer >= 8) && (is_controller_missing())) {
        print_missing_controller_text(&gCurrDisplayList, sLogicUpdateRate);
    }

#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_PROFILER_DRAW] = osGetCount();
    if (gPuppyPrint.enabled) {
        render_profiler();
        count_triangles((u8*) gDisplayLists[gSPTaskNum], (u8*) gCurrDisplayList);
    }
    gPuppyPrint.mainTimerPoints[1][PP_PROFILER_DRAW] = osGetCount();
#endif
    profiler_snapshot(THREAD4_END);

    gDPFullSync(gCurrDisplayList++);
    gSPEndDisplayList(gCurrDisplayList++);

    copy_viewports_to_stack();
    if (gDrawFrameTimer != 1) {
        if (gSkipGfxTask == FALSE) {
            wait_for_gfx_task();
        }
    } else {
        gDrawFrameTimer = 0;
    }
    gSkipGfxTask = FALSE;
    clear_free_queue();
    if (!gIsPaused) {
        gCutsceneCameraActive = FALSE;
    }
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_PROFILER_CALC] = osGetCount();
    calculate_and_update_fps();
    puppyprint_calculate_average_times();
    perfIteration++;
    if (perfIteration == NUM_PERF_ITERATIONS - 1) {
        perfIteration = 0;
    }
    gPuppyPrint.mainTimerPoints[1][PP_PROFILER_CALC] = osGetCount();
#endif
    if (gDrawFrameTimer == 2) {
        framebufferSize = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
        memcpy(gVideoLastFramebuffer, gVideoCurrFramebuffer, (s32) gVideoCurrFramebuffer + framebufferSize);
    }

    swap_framebuffer_when_ready();

    if (gDrawFrameTimer == 0) {
#ifndef FIFO_UCODE
        setup_ostask_xbus(gDisplayLists[gSPTaskNum], gCurrDisplayList);
#else
    #ifdef FIFO_4MB
        if (suCodeSwitch == FALSE && IO_READ(DPC_BUFBUSY_REG) + IO_READ(DPC_CLOCK_REG) + IO_READ(DPC_TMEM_REG)) {
    #else
        if (suCodeSwitch == FALSE && IO_READ(DPC_BUFBUSY_REG) + IO_READ(DPC_CLOCK_REG) + IO_READ(DPC_TMEM_REG) && gExpansionPak) {
    #endif
            setup_ostask_fifo(gDisplayLists[gSPTaskNum], gCurrDisplayList);
        } else {
            setup_ostask_xbus(gDisplayLists[gSPTaskNum], gCurrDisplayList);
        }
#endif
        gNumGfxTasksAtScheduler++;
        gSPTaskNum ^= 1;
    }
    if (gDrawFrameTimer) {
        gDrawFrameTimer--;
    }
}

/**
 * Loads a level for gameplay based on what the next track ID is, with the option to override.
*/
void load_next_ingame_level(s32 numPlayers, s32 trackID, Vehicle vehicle) {
    gGameNumPlayers = numPlayers - 1;
    if (trackID == -1) {
        gPlayableMapId = get_track_id_to_load();
    } else {
        gPlayableMapId = trackID; // Unused, because arg1 is always -1.
    }
    load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, vehicle);
}

/**
 * Calls load_level() with the same arguments except for the cutsceneId,
 * which is the value at gGameCurrentCutscene. Also does some other stuff.
 * Used when ingame.
 */
void load_level_game(s32 levelId, s32 numberOfPlayers, s32 entranceId, Vehicle vehicleId) {
    profiler_begin_timer();
    alloc_displaylist_heap(numberOfPlayers);
    set_free_queue_state(0);
    camera_init();
    load_level(levelId, numberOfPlayers, entranceId, vehicleId, gGameCurrentCutscene);
    init_hud(gNumberOfViewports);
    func_800AE728(8, 0x10, 0x96, 0x64, 0x32, 0);
    func_8001BF20();
    osSetTime(0);
    sPrevTime = 0;
    set_free_queue_state(2);
    func_80072298(1);
    puppyprint_log("Level [%s] loaded in %2.3fs.", get_level_name(levelId), (f64) (OS_CYCLES_TO_USEC(osGetCount() - profiler_get_timer()) / 1000000.0f));
}

/**
 * Call numerous functions to clear data in RAM.
 * Then call to free particles, HUD and text.
 * Waits for a GFX task before unloading.
*/
void unload_level_game(void) {
    set_free_queue_state(0);
    if (gSkipGfxTask == FALSE) {
        if (gDrawFrameTimer != 1) {
            wait_for_gfx_task();
        }
        gSkipGfxTask = TRUE;
    }
    clear_audio_and_track();
    transition_begin(&D_800DD3F4);
    func_800AE270();
    free_hud();
    gCurrDisplayList = gDisplayLists[gSPTaskNum];
    gDPFullSync(gCurrDisplayList++);
    gSPEndDisplayList(gCurrDisplayList++);
    set_free_queue_state(2);
}

/**
 * The main behaviour function involving all of the ingame stuff.
 * Involves the updating of all objects and setting up the render scene.
*/
void ingame_logic_loop(s32 updateRate) {
    s32 buttonPressedInputs, buttonHeldInputs, i, loadContext, sp3C;

    loadContext = LEVEL_CONTEXT_NONE;
    buttonHeldInputs = 0;
    buttonPressedInputs = 0;

    // Get input data for all 4 players.
    for (i = 0; i < get_active_player_count(); i++) {
        buttonHeldInputs |= get_buttons_held_from_player(i);
        buttonPressedInputs |= get_buttons_pressed_from_player(i);
    }
    
    /*if (get_buttons_pressed_from_player(0) & L_TRIG && !(get_buttons_held_from_player(0) & U_JPAD)) {
        s32 soundID;
        gHideHUD ^= 1;
        if (gHideHUD == 0) {
            soundID = SOUND_TING_HIGH;
        } else {
            soundID = SOUND_TING_LOW;
        }
        sound_play(soundID, NULL);
    }*/
#ifndef NO_ANTIPIRACY
    // Spam the start button, making the game unplayable because it's constantly paused.
    if (sAntiPiracyTriggered) {
        buttonPressedInputs |= START_BUTTON;
    }
#endif
    // Update all objects
    if (!gIsPaused) {
        update_time_dialation(updateRate);
        func_80010994(updateRate);
        if (gCutsceneCameraActive == 0 || get_race_countdown()) {
            if (buttonPressedInputs & START_BUTTON && gLevelPropertyStackPos == 0 && gDrumstickSceneLoadTimer == 0
                && gGameMode == GAMEMODE_INGAME && gPostRaceViewPort == NULL && gLevelLoadTimer == 0 && gPauseLockTimer == 0) {
                buttonPressedInputs = 0;
                gIsPaused = TRUE;
                func_80093A40();
            }
        }
    }
    gPauseLockTimer -= updateRate;
    if (gPauseLockTimer < 0) {
        gPauseLockTimer = 0;
    }
    if (gPostRaceViewPort) {
        gIsPaused = FALSE;
    }
    gParticlePtrList_flush();
    func_8001BF20();
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[0][PP_LEVELGFX] = osGetCount();
#endif
    render_scene(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
#ifdef PUPPYPRINT_DEBUG
    gPuppyPrint.mainTimerPoints[1][PP_LEVELGFX] = osGetCount();
#endif
    if (gGameMode == GAMEMODE_INGAME) {
        // Ignore the user's L/R/Z buttons.
        buttonHeldInputs &= ~(L_TRIG | R_TRIG | Z_TRIG);
    }
    if (gPostRaceViewPort) {
        i = func_80095728(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, updateRate); 
        switch (i) {
            case 2:
                buttonHeldInputs |= (L_TRIG | Z_TRIG);
                break;
            case 1:
                gPostRaceViewPort = NULL;
                func_8006D8F0(-1);
                break;
            case 4:
                gLevelPropertyStackPos = 0; 
                gDrumstickSceneLoadTimer = 0;
                buttonHeldInputs |= (L_TRIG | R_TRIG);
                break;
            case 5:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_TRACK_SELECT;
                break;
            case 8:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_RESULTS;
                break;
            case 9:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_TROPHY_ROUND;
                break;
            case 10:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_TROPHY_RESULTS;
                break;
            case 11:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_UNUSED;
                break;
            case 12:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_CHARACTER_SELECT;
                break;
            case 13:
                buttonHeldInputs |= L_TRIG,
                loadContext = LEVEL_CONTEXT_UNK7;
                break;
        }
    }
    process_onscreen_textbox(updateRate);
    i = func_800C3400();
    if (i != 0) {
        if (i == 2) {
            gIsPaused = TRUE;
        }
        if (func_800C3400() != 2) {
            gIsPaused = FALSE;
            n_alSeqpDelete();
        }
    }
    if (gIsPaused) {
        i = render_pause_menu(&gCurrDisplayList, updateRate);
        switch (i - 1) {
            case 0:
                gIsPaused = FALSE;
                break;
            case 1:
                sound_clear_delayed();
                reset_delayed_text();
                if (func_80023568() != 0 && is_in_two_player_adventure()) {
                    swap_lead_player();
                }
                buttonHeldInputs |= (L_TRIG | Z_TRIG);
                break;
            case 2:
                sound_clear_delayed();
                reset_delayed_text();
                if (func_80023568() != 0 && is_in_two_player_adventure()) {
                    swap_lead_player();
                }
                buttonHeldInputs |= L_TRIG;
                break;
            case 4:
                loadContext = LEVEL_CONTEXT_TRACK_SELECT;
                reset_delayed_text();
                buttonHeldInputs |= L_TRIG;
                break;
            case 11:
                loadContext = LEVEL_CONTEXT_CHARACTER_SELECT;
                reset_delayed_text();
                buttonHeldInputs |= L_TRIG;
                break;
            case 5:
                gIsPaused = FALSE;
                break;
            case 6:
                func_80022E18(1);
                gIsPaused = FALSE;
                break;
            case 3:
                gDrumstickSceneLoadTimer = 0;
                sound_clear_delayed();
                reset_delayed_text();
                gLevelPropertyStackPos = 0;
                buttonHeldInputs |= (L_TRIG | R_TRIG);
                break;
        }
    }
    init_rdp_and_framebuffer(&gCurrDisplayList);
    render_borders_for_multiplayer(&gCurrDisplayList);
    render_minimap_and_misc_hud(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, updateRate);
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
            push_level_property_stack(ASSET_LEVEL_CENTRALAREAHUB, 0, VEHICLE_CAR, CUTSCENE_ID_NONE);
            push_level_property_stack(ASSET_LEVEL_WIZPIGAMULETSEQUENCE, 0, -1, CUTSCENE_ID_UNK_A);
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
                    func_80098208();
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
        if (gLevelPropertyStackPos != 0) {
            pop_level_property_stack(&gPlayableMapId, &gGameCurrentEntrance, &i, &gGameCurrentCutscene);
            gDrawFrameTimer = 2;
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
        if (gLevelPropertyStackPos) {
            if (gLevelLoadTimer == 0) {
                i = func_800214C4();
                if ((i != 0) || ((buttonPressedInputs & A_BUTTON) && (sp3C != 0))) {
                    if (sp3C != 0) {
                        music_change_on();
                    }
                    gDrawFrameTimer = 2;
                    pop_level_property_stack(&gPlayableMapId, &gGameCurrentEntrance, &i, &gGameCurrentCutscene);
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
        gPostRaceViewPort = NULL;
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
                    func_8008AEB4(1, 0);
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
                gLevelDefaultVehicleID = get_map_default_vehicle(gPlayableMapId);
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
        gPostRaceViewPort = NULL;
        unload_level_game();
        load_level_game(gPlayableMapId, gGameNumPlayers, gGameCurrentEntrance, gLevelDefaultVehicleID);
        safe_mark_write_save_file(get_save_file_index());
        D_801234F8 = FALSE;
    }
}

/**
 * Reset dialogue and set the transition effect for the cutscene showing an unlocked Drumstick.
*/
void set_drumstick_unlock_transition(void) {
    gDrumstickSceneLoadTimer = 44;
    gIsPaused = 0;
    n_alSeqpDelete();
    transition_begin(&gDrumstickSceneTransition);
}

void func_8006D8E0(s32 arg0) {
    gPostRaceViewPort = arg0 + 1;
}

void func_8006D8F0(UNUSED s32 arg0) {
    s32 temp;
    if (gGameMode != GAMEMODE_UNUSED_4) {
        gPlayableMapId = gLevelSettings[0];
        gGameCurrentEntrance = 0;
        gGameCurrentCutscene = CUTSCENE_ID_UNK_64;
        temp = gLevelSettings[1];
        if (gLevelSettings[15] >= 0) {
            gGameCurrentEntrance = gLevelSettings[15];
        }
        if (gLevelSettings[temp + 8] >= 0) {
            gGameCurrentCutscene = gLevelSettings[temp + 8];
        }
        D_801234F8 = TRUE;
    }
}

void func_8006D968(s8 *arg0) {
    // Is arg0 LevelObjectEntry_Exit?
    s32 i;
    if (gGameMode != GAMEMODE_UNUSED_4) {
        gLevelSettings[0] = gPlayableMapId;
        for (i = 0; i < 2; i++) {
            gLevelSettings[i + 2] = arg0[i + 8];   //0x8-0x9 - destinationMapId
            gLevelSettings[i + 4] = arg0[i + 10];  //0xA-0xB - overworldSpawnIndex
            gLevelSettings[i + 6] = arg0[i + 12];  //0xC-0xD - ?
            gLevelSettings[i + 8] = arg0[i + 14];  //0xE-0xF - ?
            gLevelSettings[i + 10] = arg0[i + 18]; //0x12-0x13 - ?
            gLevelSettings[i + 12] = arg0[i + 20]; //0x14-0x15 - ?
        }
        gLevelSettings[14] = arg0[22]; //0x16 - ?
        gLevelSettings[15] = arg0[23]; //0x17 returnSpawnIndex
        D_801234FC = 1;
    }
}

/**
 * Returns the current game mode.
 */
GameMode get_game_mode(void) {
    return gGameMode;
}

/**
 * Sets up and loads a level to be used in the background of the menu that's about to be set up.
 * Used for every kind of menu that's not ingame.
*/
void load_menu_with_level_background(s32 menuId, s32 levelId, s32 cutsceneId) {
    alloc_displaylist_heap(PLAYER_ONE);
    gGameMode = GAMEMODE_MENU;
    gRenderMenu = TRUE;
    set_sound_channel_volume(0, 32767);
    set_sound_channel_volume(1, 32767);
    set_sound_channel_volume(2, 32767);
    camera_init();

    if (!gIsLoading) {
        gIsLoading = FALSE;
        if (levelId < 0) {
            gIsLoading = TRUE;
        } else {
            load_level_menu(levelId, -1, 0, VEHICLE_PLANE, cutsceneId);
        }
    }
    if (menuId == MENU_UNUSED_2 || menuId == MENU_LOGOS || menuId == MENU_TITLE) {
        reset_title_logo_scale();
    }
    menu_init(menuId);
    gGameCurrentEntrance = 0;
}

/**
 * Set the default vehicle option from the current loaded level.
 */
void set_level_default_vehicle(Vehicle vehicleID) {
    gLevelDefaultVehicleID = vehicleID;
}

/**
 * Sets the vehicle option that the next level loaded for a menu may use.
*/
void set_vehicle_id_for_menu(Vehicle vehicleId) {
    gMenuVehicleID = vehicleId;
}

/**
 * Get the default vehicle option, set by a loaded level.
 */
Vehicle get_level_default_vehicle(void) {
    return gLevelDefaultVehicleID;
}

/**
 * Calls load_level() with the same arguments, but also does some other stuff.
 * Used for menus.
 */
void load_level_menu(s32 levelId, s32 numberOfPlayers, s32 entranceId, Vehicle vehicleId, s32 cutsceneId) {
    profiler_begin_timer();
    set_free_queue_state(0);
    camera_init();
    load_level(levelId, numberOfPlayers, entranceId, vehicleId, cutsceneId);
    init_hud(gNumberOfViewports);
    func_800AE728(4, 4, 0x6E, 0x30, 0x20, 0);
    func_8001BF20();
    osSetTime(0);
    sPrevTime = 0;
    set_free_queue_state(2);
    if (gBootTimer == 0) {
        puppyprint_log("Level (%s) (Menu) loaded in %2.3fs.", get_level_name(levelId), (f64) (OS_CYCLES_TO_USEC(osGetCount() - profiler_get_timer()) / 1000000.0f));
    }
}

/**
 * Call numerous functions to clear data in RAM.
 * Then call to free particles, HUD and text.
*/
void unload_level_menu(void) {
    if (!gIsLoading) {
        gIsLoading = TRUE;
        set_free_queue_state(0);
        clear_audio_and_track();
        transition_begin(&D_800DD3F4);
        func_800AE270();
        free_hud();
        set_free_queue_state(2);
    }
    gIsLoading = FALSE;
}

/**
 * Used in menus, update objects and draw the game.
 * In the tracks menu, this only runs if there's a track actively loaded.
*/
void update_menu_scene(s32 updateRate) {
    if (gThread30NeedToLoadLevel == NULL) {
        update_time_dialation(updateRate);
        func_80010994(updateRate);
        gParticlePtrList_flush();
        func_8001BF20();
#ifdef PUPPYPRINT_DEBUG
        gPuppyPrint.mainTimerPoints[0][PP_LEVELGFX] = osGetCount();
#endif
        render_scene(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
#ifdef PUPPYPRINT_DEBUG
        gPuppyPrint.mainTimerPoints[1][PP_LEVELGFX] = osGetCount();
#endif
        process_onscreen_textbox(updateRate);
        init_rdp_and_framebuffer(&gCurrDisplayList);
        render_borders_for_multiplayer(&gCurrDisplayList);
    }
}

/**
 * Main function for handling behaviour in menus.
 * Runs the menu code, with a simplified object update and scene rendering system.
*/
void menu_logic_loop(s32 updateRate) {
    s32 menuLoopResult;
    s32 temp;
    s32 playerVehicle;
    s32 temp5;
    
    gIsPaused = FALSE;
    gPostRaceViewPort = NULL;
    if (!gIsLoading && gRenderMenu) {
        update_menu_scene(updateRate);
    }
    menuLoopResult = menu_loop(&gCurrDisplayList, &gGameCurrMatrix, &gGameCurrVertexList, &gGameCurrTriList, updateRate);
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
        gLevelDefaultVehicleID = get_map_default_vehicle(gPlayableMapId);
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
                gLevelDefaultVehicleID = get_map_default_vehicle(gPlayableMapId);
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

        gPlayableMapId = gLevelSettings[menuLoopResult+2];
        gGameCurrentEntrance = gLevelSettings[menuLoopResult+4];
        gGameMode = GAMEMODE_INGAME;
        gGameCurrentCutscene = gLevelSettings[menuLoopResult+12];
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

/**
 * Loads a level, intended to be used in a menu.
 * Skips loading many things, otherwise used in gameplay.
*/
void load_level_for_menu(s32 levelId, s32 numberOfPlayers, s32 cutsceneId) {
    Vehicle vehicleID = VEHICLE_PLANE;
    if (!gIsLoading) {
        unload_level_menu();
        if (gThread30NeedToLoadLevel == 0) {
            gCurrDisplayList = gDisplayLists[gSPTaskNum];
            gDPFullSync(gCurrDisplayList++);
            gSPEndDisplayList(gCurrDisplayList++);
        }
    }
    if (levelId != (s32) SPECIAL_MAP_ID_NO_LEVEL) {
        switch (levelId) {
        case ASSET_LEVEL_TRICKYTOPS1:
            vehicleID = VEHICLE_TRICKY;
            break;
        case ASSET_LEVEL_BLUEY1:
            vehicleID = VEHICLE_BLUEY;
            break;
        case ASSET_LEVEL_BUBBLER1:
            vehicleID = VEHICLE_BUBBLER;
            break;
        case ASSET_LEVEL_SMOKEY1:
            vehicleID = VEHICLE_SMOKEY;
            break;
        case ASSET_LEVEL_WIZPIG1:
            vehicleID = VEHICLE_WIZPIG;
            break;
        case ASSET_LEVEL_CENTRALAREAHUB:
            vehicleID = VEHICLE_PLANE;
            break;
        default: 
            vehicleID = get_map_default_vehicle(levelId);
            if (vehicleID > VEHICLE_PLANE) {
                vehicleID = VEHICLE_PLANE;
            }
            break;
        }
        alloc_displaylist_heap(TWO_PLAYERS);
        load_level_menu(levelId, numberOfPlayers, 0, vehicleID, cutsceneId);
        gIsLoading = FALSE;
        return;
    }
    gIsLoading = TRUE;
}

/**
 * Initialise global game settings data.
 * Allocate space to accomodate it then set the start points for each data point.
*/
void calc_and_alloc_heap_for_settings(void) {
    s32 dataSize;
    u32 sizes[15];
    s32 numWorlds, numLevels;

    init_level_globals();
    reset_character_id_slots();
    get_number_of_levels_and_worlds(&numLevels, &numWorlds);
    sizes[0] = sizeof(Settings);
    sizes[1] = sizes[0] + (numLevels * 4); // balloonsPtr
    sizes[2] = sizes[1] + (numWorlds * 2); // flapInitialsPtr[0]
    dataSize = (numLevels * 2);
    sizes[3] = sizes[2] + dataSize;   // flapInitialsPtr[1]
    sizes[4] = sizes[3] + dataSize;   // flapInitialsPtr[2]
    sizes[5] = sizes[4] + dataSize;   // flapTimesPtr[0]
    sizes[6] = sizes[5] + dataSize;   // flapTimesPtr[1]
    sizes[7] = sizes[6] + dataSize;   // flapTimesPtr[2]
    sizes[8] = sizes[7] + dataSize;   // courseInitialsPtr[0]
    sizes[9] = sizes[8] + dataSize;   // courseInitialsPtr[1]
    sizes[10] = sizes[9] + dataSize;  // courseInitialsPtr[2]
    sizes[11] = sizes[10] + dataSize; // courseTimesPtr[0]
    sizes[12] = sizes[11] + dataSize; // courseTimesPtr[1]
    sizes[13] = sizes[12] + dataSize; // courseTimesPtr[2]
    sizes[14] = sizes[13] + dataSize; // total size

    gSettingsPtr = allocate_from_main_pool_safe(sizes[14], MEMP_MISC);
    gSettingsPtr->courseFlagsPtr = (s32 *)((u8 *)gSettingsPtr + sizes[0]);
    gSettingsPtr->balloonsPtr = (s16 *)((u8 *)gSettingsPtr + sizes[1]);
    gSettingsPtr->tajFlags = 0;
    gSettingsPtr->flapInitialsPtr[0] = (u16 *)((u8 *)gSettingsPtr + sizes[2]);
    gSettingsPtr->flapInitialsPtr[1] = (u16 *)((u8 *)gSettingsPtr + sizes[3]);
    gSettingsPtr->flapInitialsPtr[2] = (u16 *)((u8 *)gSettingsPtr + sizes[4]);
    gSettingsPtr->flapTimesPtr[0] = (u16 *)((u8 *)gSettingsPtr + sizes[5]);
    gSettingsPtr->flapTimesPtr[1] = (u16 *)((u8 *)gSettingsPtr + sizes[6]);
    gSettingsPtr->flapTimesPtr[2] = (u16 *)((u8 *)gSettingsPtr + sizes[7]);
    gSettingsPtr->courseInitialsPtr[0] = (u16 *)((u8 *)gSettingsPtr + sizes[8]);
    gSettingsPtr->courseInitialsPtr[1] = (u16 *)((u8 *)gSettingsPtr + sizes[9]);
    gSettingsPtr->courseInitialsPtr[2] = (u16 *)((u8 *)gSettingsPtr + sizes[10]);
    gSettingsPtr->courseTimesPtr[0] = (u16 *)((u8 *)gSettingsPtr + sizes[11]);
    gSettingsPtr->courseTimesPtr[1] = (u16 *)((u8 *)gSettingsPtr + sizes[12]);
    gSettingsPtr->courseTimesPtr[2] = (u16 *)((u8 *)gSettingsPtr + sizes[13]);
    gSettingsPtr->unk4C = (Settings4C *) &gLevelSettings;
    gSaveDataFlags = // Set bits 0/1/2/8 and wipe out all others
        SAVE_DATA_FLAG_READ_FLAP_TIMES |
        SAVE_DATA_FLAG_READ_COURSE_TIMES |
        SAVE_DATA_FLAG_READ_SAVE_DATA |
        SAVE_DATA_FLAG_READ_EEPROM_SETTINGS;
}

/**
 * Set the init values for each racer based on which character they are and which player they are.
 * Then reset race status.
*/
void init_racer_headers(void) {
    s32 i, j;
    gSettingsPtr->gNumRacers = get_number_of_active_players();
    for (i = 0; i < NUM_RACERS_1P; i++) {
        gSettingsPtr->racers[i].best_times = 0;
        gSettingsPtr->racers[i].character = get_character_id_from_slot(i);
        if (gSettingsPtr->gNumRacers >= 2) {
            gSettingsPtr->racers[i].starting_position = i;
        } else if (is_in_two_player_adventure()) {
            gSettingsPtr->racers[i].starting_position = 5 - i;
        } else {
            gSettingsPtr->racers[i].starting_position = 7 - i;
        }
        gSettingsPtr->racers[i].unk7 = 0;
        for (j = 0; j < 4; j++) {
            gSettingsPtr->racers[i].placements[j] = 0;
        }
        gSettingsPtr->racers[i].course_time = 0;
        for (j = 0; j < 3; j++) {
            gSettingsPtr->racers[i].lap_times[j] = 0;
        }
    }
    gSettingsPtr->timeTrialRacer = 0;
    gSettingsPtr->unk115[0] = 0;
    gSettingsPtr->unk115[1] = 0;
    gSettingsPtr->display_times = 0;
    gSettingsPtr->worldId = 0;
    gSettingsPtr->courseId = 0;
}

/**
 * Depending on flags, clear fastest lap times and/or overall course times.
*/
void clear_lap_records(Settings *settings, s32 flags) {
    s32 i, j;
    s32 numWorlds, numLevels;
    s32 index;
    u16 *temp_v0;

    get_number_of_levels_and_worlds(&numLevels, &numWorlds);
    temp_v0 = (u16 *) get_misc_asset(ASSET_MISC_23);
    for (i = 0; i < NUMBER_OF_SAVE_FILES; i++) {
        for (j = 0; j < numLevels; j++) {
            index = (j * 12) + (i * 4);
            if (flags & 1) {
                settings->flapInitialsPtr[i][j] = temp_v0[index + 3];
                settings->flapTimesPtr[i][j] = temp_v0[index + 2];
            }
            if (flags & 2) {
                settings->courseInitialsPtr[i][j] = temp_v0[index + 1];
                settings->courseTimesPtr[i][j] = temp_v0[index];
            }
        }
    }
}

/**
 * Set all game progression values to their default, as if it were a new game.
*/
void clear_game_progress(Settings *settings) {
    s32 i;
    s32 worldCount;
    s32 levelCount;

    get_number_of_levels_and_worlds(&levelCount, &worldCount);
    settings->newGame = TRUE;

    for (i = 0; i < worldCount; i++) {
        settings->balloonsPtr[i] = 0;
    }
    for (i = 0; i < levelCount; i++) {
        settings->courseFlagsPtr[i] = RACE_UNATTEMPTED;
    }

    settings->keys = 0;
    settings->unkA = 0;
    settings->bosses = 0;
    settings->trophies = 0;
    settings->cutsceneFlags = CUTSCENE_NONE;
    settings->tajFlags = 0;
    settings->ttAmulet = 0;
    settings->wizpigAmulet = 0;
}

/**
 * Return the global game settings.
 * This is where global game records and perferences are stored.
*/
Settings *get_settings(void) {
    return gSettingsPtr;
}

/**
 * Returns the value in gIsPaused.
 */
s8 is_game_paused(void) {
    return gIsPaused;
}

/**
 * Returns the status of the post-race shrunken viewport.
*/
s8 is_postrace_viewport_active(void) {
    return gPostRaceViewPort;
}

/**
 * Sets and returns (nonzero) the message set when pressing the reset button.
 * Official name: mainResetPressed
 */
s32 is_reset_pressed(void) {
    return gNMIMesgBuf;
}

/**
 * Returns the current map ID if ingame, since this var is only set ingame.
*/
s32 get_ingame_map_id(void) {
    return gPlayableMapId;
}

void mark_to_read_flap_and_course_times(void) {
    gSaveDataFlags |= (SAVE_DATA_FLAG_READ_FLAP_TIMES | SAVE_DATA_FLAG_READ_COURSE_TIMES);
}

/**
 * Marks a flag to read the save file from the passed index from flash.
 */
void mark_read_save_file(s32 saveFileIndex) {
    //Wipe out bits 8 and 9
    gSaveDataFlags &= ~(SAVE_DATA_FLAG_READ_EEPROM_SETTINGS | SAVE_DATA_FLAG_WRITE_EEPROM_SETTINGS);
    //Place saveFileIndex at bits 8 and 9 and set bit 2
    gSaveDataFlags |= (SAVE_DATA_FLAG_READ_SAVE_DATA | ((saveFileIndex & SAVE_DATA_FLAG_INDEX_VALUE) << 8));
}

/**
 * Marks a flag to read all save file data from flash.
 */
void mark_read_all_save_files(void) {
    gSaveDataFlags |= SAVE_DATA_FLAG_READ_ALL_SAVE_DATA; //Set bit 3
}

/**
 * Marks a flag to write flap times to the eeprom
 */
void mark_to_write_flap_times(void) {
    gSaveDataFlags |= SAVE_DATA_FLAG_WRITE_FLAP_TIMES;
}

/**
 * Marks a flag to write course times to the eeprom
 */
void mark_to_write_course_times(void) {
    gSaveDataFlags |= SAVE_DATA_FLAG_WRITE_COURSE_TIMES;
}

/**
 * Marks a flag to write both flap times and course times to the eeprom
 */
void mark_to_write_flap_and_course_times(void) {
    gSaveDataFlags |= (SAVE_DATA_FLAG_WRITE_FLAP_TIMES | SAVE_DATA_FLAG_WRITE_COURSE_TIMES);
}

/**
 * Forcefully marks a flag to write a save file to flash.
 * Official Name: mainSaveGame
 */
void force_mark_write_save_file(s32 saveFileIndex) {
    gSaveDataFlags &= ~SAVE_DATA_FLAG_WRITE_SAVE_FILE_NUMBER_BITS; //Wipe out bits 10 and 11
    gSaveDataFlags |= (SAVE_DATA_FLAG_WRITE_SAVE_DATA | ((saveFileIndex & 3) << 10)); //Set bit 6 and place saveFileIndex into bits 10 and 11
}

/**
 * Marks a flag to write a save file to flash as long as we're not in tracks mode, and we're in the draw game render context.
 * This should prevent save data from being overwritten outside of Adventure Mode.
 * Official Name: mainSaveGame2
 */
void safe_mark_write_save_file(s32 saveFileIndex) {
    if (gGameMode == GAMEMODE_INGAME && !is_in_tracks_mode()) {
        gSaveDataFlags &= ~SAVE_DATA_FLAG_WRITE_SAVE_FILE_NUMBER_BITS; //Wipe out bits 10 and 11
        gSaveDataFlags |= (SAVE_DATA_FLAG_WRITE_SAVE_DATA | ((saveFileIndex & 3) << 10));; //Set bit 6 and place saveFileIndex into bits 10 and 11
    }
}

/**
 * Marks a flag to erase a save file from flash later
 */
void mark_save_file_to_erase(s32 saveFileIndex) {
    //Set bit 7 and and place saveFileIndex into bits 10 and 11 while wiping everything else
    gSaveDataFlags = SAVE_DATA_FLAG_ERASE_SAVE_DATA | ((saveFileIndex & 3) << 10);
}

//Always called after updating a value in sEepromSettings
void mark_write_eeprom_settings(void) {
    gSaveDataFlags |= SAVE_DATA_FLAG_WRITE_EEPROM_SETTINGS; // Set bit 9
}

/**
 * Allocates an amount of memory for the number of players passed in.
 */
void alloc_displaylist_heap(s32 numberOfPlayers) {
    s32 num;
    s32 totalSize;

    if (numberOfPlayers != gPrevPlayerCount) {
        gPrevPlayerCount = numberOfPlayers;
        num = numberOfPlayers;
        set_free_queue_state(0);
        free_from_memory_pool(gDisplayLists[0]);
        free_from_memory_pool(gDisplayLists[1]);
        totalSize =
            ((gNumF3dCmdsPerPlayer[num] * sizeof(Gwords)))
            + ((gNumHudMatPerPlayer[num] * sizeof(Matrix)))
            + ((gNumHudVertsPerPlayer[num] * sizeof(Vertex)))
            + ((gNumHudTrisPerPlayer[num] * sizeof(Triangle)));
        gDisplayLists[0] = (Gfx *) allocate_at_address_in_main_pool(totalSize, (u8 *) gDisplayLists[0], MEMP_GFXBUFFERS);
        gDisplayLists[1] = (Gfx *) allocate_at_address_in_main_pool(totalSize, (u8 *) gDisplayLists[1], MEMP_GFXBUFFERS);
        if ((gDisplayLists[0] == NULL) || gDisplayLists[1] == NULL) {
            if (gDisplayLists[0] != NULL) {
                free_from_memory_pool(gDisplayLists[0]);
                gDisplayLists[0] = NULL;
            }
            if (gDisplayLists[1] != NULL) {
                free_from_memory_pool(gDisplayLists[1]);
                gDisplayLists[1] = NULL;
            }
            default_alloc_displaylist_heap();
        }
        gMatrixHeap[0] = (MatrixS *)((u8 *) gDisplayLists[0] + ((gNumF3dCmdsPerPlayer[num] * sizeof(Gwords))));
        gTriangleHeap[0] = (TriangleList *)((u8 *) gMatrixHeap[0] + ((gNumHudMatPerPlayer[num] * sizeof(Matrix))));
        gVertexHeap[0] = (Vertex *)((u8 *) gTriangleHeap[0] + ((gNumHudTrisPerPlayer[num] * sizeof(Triangle))));
        gMatrixHeap[1] = (MatrixS *)((u8 *) gDisplayLists[1] + ((gNumF3dCmdsPerPlayer[num] * sizeof(Gwords))));
        gTriangleHeap[1] = (TriangleList *)((u8 *) gMatrixHeap[1] + ((gNumHudMatPerPlayer[num] * sizeof(Matrix))));
        gVertexHeap[1] = (Vertex *)((u8 *) gTriangleHeap[1] + ((gNumHudTrisPerPlayer[num] * sizeof(Triangle))));
        gCurrNumF3dCmdsPerPlayer = gNumF3dCmdsPerPlayer[num];
        gCurrNumHudMatPerPlayer = gNumHudMatPerPlayer[num];
        gCurrNumHudTrisPerPlayer = gNumHudTrisPerPlayer[num];
        gCurrNumHudVertsPerPlayer = gNumHudVertsPerPlayer[num];
        set_free_queue_state(2);
    }
    gCurrDisplayList = gDisplayLists[gSPTaskNum];
    gGameCurrMatrix = gMatrixHeap[gSPTaskNum];
    gGameCurrTriList = gTriangleHeap[gSPTaskNum];
    gGameCurrVertexList = gVertexHeap[gSPTaskNum];

    gDPFullSync(gCurrDisplayList++);
    gSPEndDisplayList(gCurrDisplayList++);
}

/**
 * Returns FALSE if dmem doesn't begin with a -1. This is checked on every main game loop iteration.
 */
s32 check_dmem_validity(void) {
    if (IO_READ(SP_DMEM_START) != -1U) {
        return FALSE;
    }
    return TRUE;
}

/**
 * Defaults allocations for 4 players
 */
void default_alloc_displaylist_heap(void) {
    s32 numberOfPlayers;
    s32 totalSize;

    numberOfPlayers = FOUR_PLAYERS;
    gPrevPlayerCount = numberOfPlayers;
    totalSize = (gNumF3dCmdsPerPlayer[numberOfPlayers] * sizeof(Gwords))
        + (gNumHudMatPerPlayer[numberOfPlayers] * sizeof(Matrix))
        + (gNumHudVertsPerPlayer[numberOfPlayers] * sizeof(Vertex))
        + (gNumHudTrisPerPlayer[numberOfPlayers] * sizeof(Triangle));

    gDisplayLists[0] = (Gfx *) allocate_from_main_pool_safe(totalSize, MEMP_GFXBUFFERS);
    gMatrixHeap[0] = (MatrixS *) ((u8 *) gDisplayLists[0] + (gNumF3dCmdsPerPlayer[numberOfPlayers] * sizeof(Gwords)));
    gVertexHeap[0] = (Vertex *) ((u8 *) gMatrixHeap[0] + (gNumHudMatPerPlayer[numberOfPlayers] * sizeof(Matrix)));
    gTriangleHeap[0] = (TriangleList *) ((u8 *) gVertexHeap[0] + (gNumHudVertsPerPlayer[numberOfPlayers] * sizeof(Vertex)));

    gDisplayLists[1] = (Gfx *) allocate_from_main_pool_safe(totalSize, MEMP_GFXBUFFERS);
    gMatrixHeap[1] = (MatrixS *) ((u8 *) gDisplayLists[1] + (gNumF3dCmdsPerPlayer[numberOfPlayers] * sizeof(Gwords)));
    gVertexHeap[1] = (Vertex *) ((u8 *) gMatrixHeap[1] + (gNumHudMatPerPlayer[numberOfPlayers] * sizeof(Matrix)));
    gTriangleHeap[1] = (TriangleList *) ((u8 *) gVertexHeap[1] + (gNumHudVertsPerPlayer[numberOfPlayers] * sizeof(Vertex)));

    gCurrNumF3dCmdsPerPlayer = gNumF3dCmdsPerPlayer[numberOfPlayers];
    gCurrNumHudMatPerPlayer = gNumHudMatPerPlayer[numberOfPlayers];
    gCurrNumHudTrisPerPlayer = gNumHudTrisPerPlayer[numberOfPlayers];
    gCurrNumHudVertsPerPlayer = gNumHudVertsPerPlayer[numberOfPlayers];
}

void func_8006F140(s32 arg0) {
    if (gLevelLoadTimer == 0) {
        gLevelLoadTimer = 40;
        gLevelLoadType = 0;
        if (arg0 == 1) { //FADE_BARNDOOR_HORIZONTAL?
            transition_begin(&gLevelFadeOutTransition);
        }
        if (arg0 == 3) { //FADE_CIRCLE?
            gLevelLoadTimer = 282;
            transition_begin(&D_800DD424);
        }
        if (arg0 == 4) { //FADE_WAVES?
            gLevelLoadTimer = 360;
            transition_begin(&D_800DD424);
        }
        if (arg0 == 0) { //FADE_FULLSCREEN?
            gLevelLoadTimer = 2;
        }
    }
}

/**
 * Begins a fade transition, then signals to the level loading that it wants to be a trophy race.
*/
void begin_trophy_race_teleport(void) {
    if (gLevelLoadTimer == 0) {
        transition_begin(&gLevelFadeOutTransition);
        gLevelLoadTimer = 40;
        gLevelLoadType = LEVEL_LOAD_TROPHY_RACE;
    }
}

/**
 * Check if all available trophy races and Wizpig 1 has been beaten, and if the cutscene has not yet played.
*/
void begin_lighthouse_rocket_cutscene(void) {
    if (gLevelLoadTimer == 0) {
        if ((gSettingsPtr->trophies & 0xFF) == 0xFF && !(gSettingsPtr->cutsceneFlags & CUTSCENE_LIGHTHOUSE_ROCKET) && gSettingsPtr->bosses & 1) {
            gSettingsPtr->cutsceneFlags |= CUTSCENE_LIGHTHOUSE_ROCKET;
            transition_begin(&gLevelFadeOutTransition);
            gLevelLoadTimer = 40;
            gNextMap = ASSET_LEVEL_ROCKETSEQUENCE;
            gLevelLoadType = LEVEL_LOAD_LIGHTHOUSE_CUTSCENE;
        }
    }
}

/**
 * Begin a transition, then set the next level to the passed argument.
 * This is used only to warp to Future Fun Land from the hub area.
*/
void begin_level_teleport(s32 levelID) {
    if (gLevelLoadTimer == 0) {
        gNextMap = levelID;
        transition_begin(&gLevelFadeOutTransition);
        gLevelLoadTimer = 40;
        gLevelLoadType = LEVEL_LOAD_FUTURE_FUN_LAND;
    }
}

/**
 * Set the nunber of frames to disallow pausing for.
*/
void set_pause_lockout_timer(u8 time) {
    gPauseLockTimer = time;
}

/**
 * Switch the data around for player 1 and 2 for two player adventure,
 * effectively passing the lead over to the other player.
*/
void swap_lead_player(void) {
    s32 i;
    u8 temp;
    u8 *first_racer_data;
    u8 *second_racer_data;

    swap_player_1_and_2_ids();
    func_8000E194();

    first_racer_data = (u8 *)(gSettingsPtr->racers);
    second_racer_data = (u8 *)(gSettingsPtr->racers + 1);

    for (i = 0; i < (s32) sizeof(Racer); i++) {
        temp = first_racer_data[i];
        first_racer_data[i] = second_racer_data[i];
        second_racer_data[i] = temp;
    }
}

#if SKIP_INTRO == SKIP_TITLE
 #define BOOT_LVL MENU_TITLE
#elif SKIP_INTRO == SKIP_CHARACTER
 #define BOOT_LVL MENU_CHARACTER_SELECT
#elif SKIP_INTRO == SKIP_MENU
 #define BOOT_LVL MENU_GAME_SELECT
#else
 #define BOOT_LVL MENU_BOOT
#endif // SKIP_INTRO

/**
 * Give the player 8 frames to enter the CPak menu with start, then load the intro sequence.
 */
void pre_intro_loop(void) {
    s32 i;
    s32 buttonInputs = 0;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        buttonInputs |= get_buttons_held_from_player(i);
    }
    if (buttonInputs & START_BUTTON) {
        gShowControllerPakMenu = TRUE;
    }
#ifndef SKIP_INTRO
    sBootDelayTimer++;
#else
    sBootDelayTimer = 8;
#endif
    if (sBootDelayTimer >= 8) {
        load_menu_with_level_background(BOOT_LVL, ASSET_LEVEL_OPTIONSBACKGROUND, 2);
    }
}

/**
 * Returns TRUE if the game doesn't detect any controllers.
 * Official name: mainDemoOnly
 */
s32 is_controller_missing(void) {
    if (sControllerStatus == CONTROLLER_MISSING) {
        return TRUE;
    } else {
        return FALSE;
    }
}

#ifndef NO_ANTIPIRACY
/**
 * Ran on boot, will make sure the CIC chip (CIC6103) is to spec. Will return true if it's all good, otherwise it returns false.
 * The intention of this function, is an attempt to check that the cartridge is a legitimate copy.
 * A false read, meaning you're caught running an illegitimate copy, will force the game to pause when you enter the world.
 */
s32 check_imem_validity(void) {
    if (IO_READ(SP_IMEM_START) != CIC_ID) {
        return FALSE;
    }
    return TRUE;
}
#endif