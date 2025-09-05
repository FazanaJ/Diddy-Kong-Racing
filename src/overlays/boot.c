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
#include "save_layout.h"
#include "src/thread30_bgload.h"

#define debug_print(x) ((void)(x))

// GLOBAL_ASM

extern s32 gGameCurrentEntrance;
extern s32 gSPTaskNum;
extern OSScClient *gNMISched[3];
extern OSMesg gGameMesgBuf[3];
extern OSMesgQueue gGameMesgQueue;
extern s32 gNMIMesgBuf;
extern s32 gNumGfxTasksAtScheduler;
extern s32 gGameCurrentCutscene;
extern s8 gIsLoading;
extern s32 sControllerStatus;
extern u64 *gSchedStack;
extern s8 gSetupVideo;
extern Vehicle gLevelDefaultVehicleID;
extern s32 gGameMode;
extern Gfx *gDisplayLists[2];
extern Mtx *gMatrixHeap[2];
extern Vertex *gVertexHeap[2];
extern Triangle *gTriangleHeap[2];
extern Settings *gSettingsPtr;
extern s8 gLevelSettings[16];
extern s16 *gArcTanTable;
extern u8 gSortMats;
extern s16 gSortBufCount[SORT_ENTRIES];
extern SortBuffer *gSortBuffer[SORT_ENTRIES];
extern char *gTempLevelNames;
extern char **gLevelNames;
extern u8 gTwoPlayerAdvRace;
extern s8 gCurrentDefaultVehicle;
extern s32 gNumberOfLevelHeaders;
extern s32 gNumberOfWorlds;
extern LevelGlobalData *gGlobalLevelTable;
extern s32 *gTempAssetTable;
extern LevelHeader *gCurrentLevelHeader;

extern u8 __osContPifRam[];
extern u8 __osContLastCmd;
void __osSiGetAccess(void);
void __osSiRelAccess(void);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
u32 get_clockspeed(void);
void skGetId(u32 *arg);
u32 emux_detect(void); // defined in asm/emux.s

void get_pj64_version() {
    u32 *pifRam32;
    s32 i;
    u8 pifCheck;
    const u8 cicTest[] = {
        0x0F, 0x0F,
        0xEC, 0x3C, 0xB6, 0x76, 0xB8, 0x1D, 0xBB, 0x8F,
        0x6B, 0x3A, 0x80, 0xEC, 0xED, 0xEA, 0x5B
    };
    // When calling this function, we know that the emulator is some version of Project 64,
    // and it isn't using the PJ64 4.0 interpreter core. Figure out which version it is.
    
    // PJ64 4.0 dynarec core doesn't update the COUNT register correctly within recompiled functions
    if (get_clockspeed() == 0) {
        gPlatform |= PJ64_4;
        return;
    }

    // Instead of implementing this PIF command correctly, PJ64 versions prior to 3.0 just have
    // a set of hardcoded values for some requests. At least one of these hardcoded values has a
    // typo in it, making it give an incorrect result.
    __osSiGetAccess();
    pifRam32 = (u32*)__osContPifRam;
    for (i = 0; i < 15; i++) pifRam32[i] = 0;
    pifRam32[15] = 2;    

    bcopy(cicTest, &__osContPifRam[46], 17);

    __osSiRawStartDma(OS_WRITE, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    __osContLastCmd = 254;

    __osSiRawStartDma(OS_READ, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    pifCheck = __osContPifRam[54];
    __osSiRelAccess();

    if (pifCheck == 0xB0) {
        gPlatform |= PJ64_1;
    } else {
        gPlatform |= PJ64_3;
    }
}

u8 check_cache_emulation() {
    u8 cacheEmulated;
    // Disable interrupts to ensure that nothing evicts the variable from cache while we're using it.
    u32 saved = __osDisableInt();
    // Create a variable with an initial value of 1. This value will remain cached.
    volatile u8 sCachedValue = 1;
    // Overwrite the variable directly in RDRAM without going through cache.
    // This should preserve its value of 1 in dcache if dcache is emulated correctly.
    *(u8*)(K0_TO_K1(&sCachedValue)) = 0;
    // Read the variable back from dcache, if it's still 1 then cache is emulated correctly.
    // If it's zero, then dcache is not emulated correctly.
    cacheEmulated = sCachedValue;
    // Restore interrupts
    __osRestoreInt(saved);
    return cacheEmulated;
}

void get_platform(void) {
    u32 cf;
    u32 magic;
    u16 halfMagic;
    if ((u32)IO_READ(DPC_PIPEBUSY_REG) | (u32)IO_READ(DPC_TMEM_REG) | (u32)IO_READ(DPC_BUFBUSY_REG)) {
        gPlatform |= emux_detect() ? ARES : CONSOLE;

        if (gPlatform & CONSOLE) {
            if (__osBbIsBb) {
                gPlatform = IQUE | CONSOLE;
                debug_printf("iQue Player detected.\n");
                return;
            } else {
                char region[5];
                if (osTvType == OS_TV_PAL) {
                    sprintf(region, "PAL");
                } else if (osTvType == OS_TV_NTSC) {
                    sprintf(region, "NTSC");
                } else {
                    sprintf(region, "MPAL");
                }
                debug_printf("%s N64 Console detected.\n", region);
            }
        } else {
            debug_printf("Ares N64 Emulator Detected.\n");
        }
        return;
    }

    gPlatform |= EMULATOR;
    cf = get_clockspeed();
    if (cf > 1) {
        gPlatform |= CF_2;
    }

    // Perform a read from unmapped PIF ram.
    // On console and well behaved emulators, this echos back the lower half of
    // the requested memory address, repeating it if a whole word is requested.
    // So in this case, it should result in 0x01040104
    osPiRawReadIo(0x1fd00104u, &magic);
    if (magic == 0u) {
        // Older versions of mupen (and pre-2.12 ParallelN64) just always read 0
        gPlatform |= MUPEN_OLD;
        debug_printf("Mupen64 Emulator Detected.\n");
    } else {
        __osPiGetAccess();
        while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
        halfMagic = *((volatile u16*)0xbfd00106u);
        __osPiRelAccess();

        // Now do a halfword read instead.
        switch (halfMagic) {
            // This is the correct result (echo back the lower half of the requested address)
            case 0x0106: {
                // Test to see if the libpl emulator extension is present.
                osPiRawWriteIo(0x1ffb0000u, 0u);
                osPiRawReadIo(0x1ffb0000u, &magic);
                if (magic == 0x00500000u) {
                    // libpl is supported. Must be ParallelN64
                    gPlatform |= PARALLEL_LAUNCHER;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }
                
                // If the cache is emulated, it's Ares
                if (check_cache_emulation()) {
                    gPlatform |= ARES;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }

                // its the Project64 4.0 interpreter core
                gPlatform |= PJ64_4;
                debug_printf("Project64 Emulator Detected.\n");
                break;
            }
            // This looks like it should be the expected result considering what we got when we
            // requested the whole word, but that's actually wrong. Later versions of mupen
            // (and the Simple64 fork of it) get this wrong.
            case 0x0104:
                if (check_cache_emulation()) {
                    gPlatform |= SIMPLE64;
                    debug_printf("Simple64 Emulator Detected.\n");
                } else {
                    gPlatform |= MUPEN_NEXT;
                    debug_printf("Mupen64 Emulator Detected.\n");
                }
                break;
            // If reading a word gives the correct response, but reading a halfword always gives 0,
            // then we are dealing with some version of Project 64. Call into this helper function
            // to find out which version we're dealing with.
            case 0x0000:
                get_pj64_version();
                debug_printf("Project64 Emulator Detected.\n");
                break;
            // No known emulator gives any other value. If we somehow manage to get here, just return 0
            default:
                debug_printf("Cannot determine run environment.\n");
                gPlatform = 0;
                break;
        }
    }
    debug_printf("Counter Factor Setting: %d.\n", cf);
}

/**
 * Defaults allocations for 4 players
 */
void default_alloc_displaylist_heap(void) {
    s32 numberOfPlayers;
    s32 totalSize;
    s32 gfxAdd;

    if (gDebug) {
        gfxAdd = NUM_DEBUG_GFX;
    } else {
        gfxAdd = 0;
    }

    numberOfPlayers = FOUR_PLAYERS;
    totalSize = ((NUM_GFX_COMMANDS + gfxAdd) * sizeof(Gwords)) +
                (NUM_MTX_COMMANDS * sizeof(Mtx)) +
                (NUM_VTX_COMMANDS * sizeof(Vertex)) +
                (NUM_TRI_COMMANDS * sizeof(Triangle));

    gDisplayLists[0] = (Gfx *) mempool_alloc_safe(totalSize, PP_RAM_CMDBUF);
    gMatrixHeap[0] = (Mtx *) ((u8 *) gDisplayLists[0] + ((NUM_GFX_COMMANDS + gfxAdd) * sizeof(Gwords)));
    gMatrixHeap[0] = (Mtx *) align16((u8 *) gMatrixHeap[0]);
    gVertexHeap[0] = (Vertex *) ((u8 *) gMatrixHeap[0] + ((NUM_MTX_COMMANDS - 1) * sizeof(Mtx)));
    gTriangleHeap[0] = (Triangle *) ((u8 *) gVertexHeap[0] + (NUM_VTX_COMMANDS * sizeof(Vertex)));

    gDisplayLists[1] = (Gfx *) mempool_alloc_safe(totalSize, PP_RAM_CMDBUF);
    gMatrixHeap[1] = (Mtx *) ((u8 *) gDisplayLists[1] + ((NUM_GFX_COMMANDS + gfxAdd) * sizeof(Gwords)));
    gMatrixHeap[1] = (Mtx *) align16((u8 *) gMatrixHeap[1]);
    gVertexHeap[1] = (Vertex *) ((u8 *) gMatrixHeap[1] + ((NUM_MTX_COMMANDS - 1) * sizeof(Mtx)));
    gTriangleHeap[1] = (Triangle *) ((u8 *) gVertexHeap[1] + (NUM_VTX_COMMANDS * sizeof(Vertex)));
}

/**
 * Initialise global game settings data.
 * Allocate space to accomodate it then set the start points for each data point.
 */
void calc_and_alloc_heap_for_settings(void) {
    s32 dataSize;
    u32 sizes[15];
    s32 numWorlds, numLevels;

    level_global_init();
    reset_character_id_slots();
    level_count(&numLevels, &numWorlds);
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

    gSettingsPtr = mempool_alloc_safe(sizes[14], PP_RAM_SAVES);
    gSettingsPtr->courseFlagsPtr = (s32 *) ((u8 *) gSettingsPtr + sizes[0]);
    gSettingsPtr->balloonsPtr = (s16 *) ((u8 *) gSettingsPtr + sizes[1]);
    gSettingsPtr->tajFlags = 0;
    gSettingsPtr->flapInitialsPtr[0] = (u16 *) ((u8 *) gSettingsPtr + sizes[2]);
    gSettingsPtr->flapInitialsPtr[1] = (u16 *) ((u8 *) gSettingsPtr + sizes[3]);
    gSettingsPtr->flapInitialsPtr[2] = (u16 *) ((u8 *) gSettingsPtr + sizes[4]);
    gSettingsPtr->flapTimesPtr[0] = (u16 *) ((u8 *) gSettingsPtr + sizes[5]);
    gSettingsPtr->flapTimesPtr[1] = (u16 *) ((u8 *) gSettingsPtr + sizes[6]);
    gSettingsPtr->flapTimesPtr[2] = (u16 *) ((u8 *) gSettingsPtr + sizes[7]);
    gSettingsPtr->courseInitialsPtr[0] = (u16 *) ((u8 *) gSettingsPtr + sizes[8]);
    gSettingsPtr->courseInitialsPtr[1] = (u16 *) ((u8 *) gSettingsPtr + sizes[9]);
    gSettingsPtr->courseInitialsPtr[2] = (u16 *) ((u8 *) gSettingsPtr + sizes[10]);
    gSettingsPtr->courseTimesPtr[0] = (u16 *) ((u8 *) gSettingsPtr + sizes[11]);
    gSettingsPtr->courseTimesPtr[1] = (u16 *) ((u8 *) gSettingsPtr + sizes[12]);
    gSettingsPtr->courseTimesPtr[2] = (u16 *) ((u8 *) gSettingsPtr + sizes[13]);
    gSettingsPtr->unk4C = (Settings4C *) &gLevelSettings;
    gSaveDataFlags = // Set bits 0/1/2/8 and wipe out all others
        SAVE_DATA_FLAG_READ_FLAP_TIMES | SAVE_DATA_FLAG_READ_COURSE_TIMES | SAVE_DATA_FLAG_READ_SAVE_DATA |
        SAVE_DATA_FLAG_READ_EEPROM_SETTINGS;
}

void sortbuffer_init(void) {
    s32 i;
    if (gSortBuffer[SORT_OPA]) {
        mempool_free(gSortBuffer[SORT_OPA]);
    }

    if (gSortMats == FALSE) {
        return;
    }

    gSortBuffer[SORT_OPA] = (SortBuffer *) mempool_alloc(sizeof(SortBuffer) * 500, PP_RAM_STACK);
    gSortBuffer[SORT_DECAL] = (SortBuffer *) (((u8 *) gSortBuffer[SORT_OPA]) + (sizeof(SortBuffer) * 300));
    gSortBuffer[SORT_XLU] = (SortBuffer *) (((u8 *) gSortBuffer[SORT_DECAL]) + (sizeof(SortBuffer) * 100));
    for (i = 0; i < SORT_ENTRIES; i++) {
        gSortBuffer[i][0].nextIndex = -1;
        gSortBuffer[i][0].index = 0;
        gSortBufCount[i] = 0;
    }
}

s32 userconfig_read(void) {
    ConfigBits b;
    UserConfig *c;

    if (save_detect() == 0) {
        return -1;
    }

    c = &gConfig;

    save_readwrite((void *) &b, VIDEOCONFIG_START, sizeof(ConfigBits), OS_READ);

    if (b.magic != 0x14) {
        //debug_printf("Bad magic! %X\n", b.magic);
        bzero(&b, sizeof(ConfigBits));
        b.magic = 0x14;
        save_readwrite((void *) &b, VIDEOCONFIG_START, sizeof(ConfigBits), OS_WRITE);

        // To return 1 signals the game to show the video mode screen. NTSC users don't need this, so just return 0 as normal.
        switch (osTvType) {
            case OS_TV_TYPE_PAL:
                gConfig.screenRegion = REGIONMODE_PAL50;
                return 1;
            case OS_TV_TYPE_NTSC:
                gConfig.screenRegion = REGIONMODE_NTSC;
                return 0;
            case OS_TV_TYPE_MPAL:
                gConfig.screenRegion = REGIONMODE_MPAL;
                return 0;
        }
    } else {
        //debug_printf("Good magic! %X\n", b.magic);
        c->antiAliasing = b.antiAliasing;
        c->dedither = b.dedither;
        c->screenBits = b.screenBits;
        c->screenWidth = b.screenWidth;
        c->screenRegion = b.screenRegion;
        //c->terrainQuality = b.terrainQuality;
        return 0;
    }
    return 0;
}

/**
 * Allocates memory for gGlobalLevelTable, then populates it with relevant data from every level header.
 * The level headers are streamed from ROM.
 * Additionally loads other globally accessed information, like level names, then runs a checksum compare, for good
 * measure.
 */
void level_global_init(void) {
    s32 i;
    s32 size;
    UNUSED s32 checksumCount;
    u8 *header;
    UNUSED s32 j;

    header = mempool_alloc_safe(sizeof(LevelHeader), PP_RAM_ASSETTABLE);
    gTempAssetTable = (s32 *) asset_table_load(ASSET_LEVEL_HEADERS_TABLE);
    i = 0;
    gNumberOfLevelHeaders = 0;
    while (gTempAssetTable[gNumberOfLevelHeaders] != -1) {
        gNumberOfLevelHeaders++;
    }
    gNumberOfLevelHeaders--;
    gGlobalLevelTable = mempool_alloc_safe(gNumberOfLevelHeaders * sizeof(LevelGlobalData), PP_RAM_ASSETTABLE);
    gCurrentLevelHeader = (LevelHeader *) header;
    gNumberOfWorlds = -1;
    for (i = 0; i < gNumberOfLevelHeaders; i++) {
        asset_load(ASSET_LEVEL_HEADERS, (u32) gCurrentLevelHeader, gTempAssetTable[i], sizeof(LevelHeader));
        if (gNumberOfWorlds < gCurrentLevelHeader->world) {
            gNumberOfWorlds = gCurrentLevelHeader->world;
        }
        gGlobalLevelTable[i].world = gCurrentLevelHeader->world;
        gGlobalLevelTable[i].raceType = gCurrentLevelHeader->race_type;
        gGlobalLevelTable[i].vehicles = ((u16) gCurrentLevelHeader->available_vehicles) << 4;
        gGlobalLevelTable[i].vehicles |= gCurrentLevelHeader->vehicle & 0xF;
    }
    gNumberOfWorlds++;
    mempool_free(gTempAssetTable);
    mempool_free(header);
    gTempAssetTable = (s32 *) asset_table_load(ASSET_LEVEL_NAMES_TABLE);
    for (i = 0; gTempAssetTable[i] != (-1); i++) {}
    i--;
    size = gTempAssetTable[i] - gTempAssetTable[0];
    gLevelNames = mempool_alloc_safe(i * sizeof(s32), PP_RAM_ASSETTABLE);
    gTempLevelNames = mempool_alloc_safe(size, PP_RAM_ASSETTABLE);
    asset_load(ASSET_LEVEL_NAMES, (u32) gTempLevelNames, 0, size);
    for (size = 0; size < i; size++) {
        gLevelNames[size] = (char *) &gTempLevelNames[gTempAssetTable[size]];
    }
    mempool_free(gTempAssetTable);
    // Antipiracy measure
#ifdef ANTI_TAMPER
    checksumCount = 0;
    for (j = 0; j < gViewportFuncLength; j++) {
        checksumCount += ((u8 *) (&viewport_rsp_set))[j];
    }
    if (checksumCount != gViewportFuncChecksum) {
        drm_disable_input();
    }
#endif
}

void thread3_boot(void) {
    s32 videoRet;

    gIsLoading = FALSE;
    gLevelDefaultVehicleID = VEHICLE_CAR;

    if (gDebug) {
        init_usb_thread();
    }
    gSchedStack = mempool_alloc_safe(STACK_SCHED, PP_RAM_STACK);
    osCreateScheduler(&gMainSched, gSchedStack + STACKSIZE(STACK_SCHED), /*priority*/ 13, 1);
    gSchedStack[0] = 0;
    gSchedStack[STACKSIZE(STACK_SCHED) - 1] = 0;
    video_init();
    gzip_init();
    gfxtask_init(&gMainSched);
    sControllerStatus = input_init();
    videoRet = userconfig_read();
    if (videoRet == 1) {
        gSetupVideo = TRUE;
    } else {
        gSetupVideo = FALSE;
    }
    audio_init(&gMainSched);
    audspat_init();
    tex_init_textures();
    allocate_object_model_pools();
    allocate_object_pools();
    debug_text_init();
    allocate_ghost_data();
    init_particle_assets();
    weather_init();
    trigtable_generate();
    calc_and_alloc_heap_for_settings();
    default_alloc_displaylist_heap();
    load_fonts();
    init_controller_paks();
    init_save_data();
    vi_change(SCREEN_WIDTH, SCREEN_HEIGHT);
    sBlackScreenTimer = 1;
    init_particle_buffers(4, 4, 110, 48, 32, 0);
    osCreateMesgQueue(&gGameMesgQueue, gGameMesgBuf, 3);
    osScAddClient(&gMainSched, (OSScClient*) gNMISched, &gGameMesgQueue, OS_SC_ID_VIDEO);
    sortbuffer_init();
    mempool_free_timer(2);
    gNMIMesgBuf = 0;
    gGameCurrentEntrance = 0;
    gGameCurrentCutscene = 0;
    gSPTaskNum = 0;
    gSaveDataFlags = input_update(gSaveDataFlags, 0);
    gGameMode = GAMEMODE_INTRO;
}
