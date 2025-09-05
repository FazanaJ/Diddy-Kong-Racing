#include "game.h"

#include "asset_enums.h"
#include "asset_loading.h"
#include "audio.h"
#include "audio_spatial.h"
#include "audiosfx.h"
#include "camera.h"
#include "common.h"
#include "joypad.h"
#include "lights.h"
#include "macros.h"
#include "memory.h"
#include "menu.h"
#include "objects.h"
#include "racer.h"
#include "rcp_dkr.h"
#include "save_data.h"
#include "set_rsp_segment.h"
#include "structs.h"
#include "textures_sprites.h"
#include "thread3_main.h"
#include "tracks.h"
#include "types.h"
#include "video.h"
#include "weather.h"
#include "main.h"
#include "usb/usb.h"

/************ .data ************/

char *gTempLevelNames = NULL;
s8 gCurrentDefaultVehicle = -1;
u8 gTwoPlayerAdvRace = FALSE;
s32 gIsInRace = 0;

// Updated automatically from calc_func_checksums.py
s32 gViewportFuncChecksum = ViewportFuncChecksum;
s32 gViewportFuncLength = 0x154;
s16 gLevelPropertyStackPos = 0;
s16 D_800DD32C = 0;
s8 D_800DD330 = 0;

/*******************************/

/************ .bss ************/

s32 *gTempAssetTable;
s32 gMapId;
LevelHeader *gCurrentLevelHeader;
char **gLevelNames;
s32 gNumberOfLevelHeaders;
s32 gNumberOfWorlds;
LevelGlobalData *gGlobalLevelTable;
AIBehaviourTable *gAIBehaviourTable;
s16 gLevelPropertyStack[5 * 4]; // Stores level info for cutscenes. 5 sets of four properties.

/******************************/

/**
 * Returns the default vehicle from the set map ID.
 */
Vehicle leveltable_vehicle_default(s32 mapId) {
    if (mapId > 0 && mapId < gNumberOfLevelHeaders) {
        return gGlobalLevelTable[mapId].vehicles & 0xF;
    }
    return VEHICLE_CAR;
}

/**
 * Returns the available vehicles from the set map ID.
 */
s32 leveltable_vehicle_usable(s32 mapId) {
    if (mapId > 0 && mapId < gNumberOfLevelHeaders) {
        s32 temp = gGlobalLevelTable[mapId].vehicles;
        if (temp != 0) {
            return (temp >> 4) & 0xF;
        }
    }
    return (1 << VEHICLE_CAR);
}

/**
 * Returns the race type from the set map ID.
 */
s8 leveltable_type(s32 mapId) {
    if (mapId >= 0 && mapId < gNumberOfLevelHeaders) {
        return gGlobalLevelTable[mapId].raceType;
    }
    return -1;
}

/**
 * Returns the world ID from the set map ID.
 */
s8 leveltable_world(s32 mapId) {
    if (mapId >= 0 && mapId < gNumberOfLevelHeaders) {
        return gGlobalLevelTable[mapId].world;
    }
    return 0;
}

/**
 * Returns the ID of the current hub world. Example: Dino Domain.
 */
s32 level_world_id(s32 worldId) {
    s8 *hubAreaIds;

    if (worldId < 0 || worldId >= gNumberOfWorlds) {
        worldId = 0;
    }
    hubAreaIds = (s8 *) get_misc_asset(ASSET_MISC_HUB_AREA_IDS);

    return hubAreaIds[worldId];
}

/**
 * Writes the level and hub count to the two arguments passed through.
 */
void level_count(s32 *outLevelCount, s32 *outWorldCount) {
    *outLevelCount = gNumberOfLevelHeaders;
    *outWorldCount = gNumberOfWorlds;
}

/**
 * Returns true if the current event is a regular race or a boss race.
 * Returns false if it's a menu, challenge or hubworld.
 */
s32 level_is_race(void) {
    return gIsInRace;
}

/**
 * If the level's music ID is nonzero, set the current background music.
 */
void level_music_start(f32 tempo) {
    if (gCurrentLevelHeader->music != SEQUENCE_NONE) {
        music_channel_reset_all();
        music_play(gCurrentLevelHeader->music);
        music_tempo_set_relative(tempo);
        music_dynamic_set(gCurrentLevelHeader->instruments);
    }
}

/**
 * Return the current map ID.
 */
s32 level_id(void) {
    return gMapId;
}

/**
 * Return the race type ID of the current level.
 * Official name: levelGetType
 */
u8 level_type(void) {
    return gCurrentLevelHeader->race_type;
}

/**
 * Return the header data of the current level.
 * Official Name: levelGetLevel
 */
LevelHeader *level_header(void) {
    return gCurrentLevelHeader;
}

/**
 * Returns the name of the level from the passed ID
 */
char *level_name(s32 levelId) {
    char *levelName;
    u8 numberOfNullPointers = 0;

    if (levelId < 0 || levelId >= gNumberOfLevelHeaders) {
        return NULL;
    }

    levelName = gLevelNames[levelId];
    switch (get_language()) {
        case LANGUAGE_GERMAN:
            while (numberOfNullPointers < 1) {
                if (*(levelName++) == 0) {
                    numberOfNullPointers++;
                }
            }
            break;
        case LANGUAGE_FRENCH:
            while (numberOfNullPointers < 2) {
                if (*(levelName++) == 0) {
                    numberOfNullPointers++;
                }
            }
            break;
        case LANGUAGE_JAPANESE:
            while (numberOfNullPointers < 3) {
                if (*(levelName++) == 0) {
                    numberOfNullPointers++;
                }
            }
            break;
    }
    return levelName;
}

/**
 * Call multiple functions to stop and free audio, then free track, weather and wave data.
 */
void level_free(void) {
    aitable_free();
    bgdraw_primcolour(0, 0, 0);
    mempool_free(gCurrentLevelHeader);
    sndp_stop_all_looped();
    music_stop();
    music_jingle_stop();
    music_channel_reset_all();
#ifdef USE_DYNLIGHTS
    lights_free();
#endif
    free_track();
    audspat_reset();
    sound_volume_change(VOLUME_NORMAL);
    if (gCurrentLevelHeader->weatherEnable > 0) {
        weather_free();
    }
    //! @bug this will never be true because skyDome is signed.
    if (gCurrentLevelHeader->skyDome == -1) {
        tex_free(gCurrentLevelHeader->unkA4);
    }
}

/**
 * Frees the AI behaviour table from memory.
 */
void aitable_free(void) {
    if (gAIBehaviourTable) {
        mempool_free(gAIBehaviourTable);
    }
}

/**
 * Return the behaviour value table for AI racers.
 */
AIBehaviourTable *aitable_get(void) {
    return gAIBehaviourTable;
}

/**
 * Return whether it is a standard race with two players in adventure mode.
 */
s8 race_is_adventure_2P(void) {
    return gTwoPlayerAdvRace;
}

/**
 * Pushes the current level data onto a stack.
 * Used for preserving certain properties when viewing cutscenes, where this information would otherwise be lost.
 */
void level_properties_push(s32 levelId, s32 entranceId, Vehicle vehicleId, s32 cutsceneId) {
    gLevelPropertyStack[gLevelPropertyStackPos++] = levelId;
    gLevelPropertyStack[gLevelPropertyStackPos++] = entranceId;
    gLevelPropertyStack[gLevelPropertyStackPos++] = vehicleId;
    gLevelPropertyStack[gLevelPropertyStackPos++] = cutsceneId;
}

/**
 * Reads the level data from the stack, then pops it.
 * Used after cutscenes to properly restore the previous level status.
 */
void level_properties_pop(s32 *levelId, s32 *entranceId, s32 *vehicleId, s32 *cutsceneId) {
    s32 tempVehicleID;

    gLevelPropertyStackPos--;
    *cutsceneId = gLevelPropertyStack[gLevelPropertyStackPos--];
    tempVehicleID = gLevelPropertyStack[gLevelPropertyStackPos--];
    *entranceId = gLevelPropertyStack[gLevelPropertyStackPos--];
    *levelId = gLevelPropertyStack[gLevelPropertyStackPos];

    if (tempVehicleID != -1) {
        *vehicleId = tempVehicleID;
    }

    D_800DD32C = 1;
}

/**
 * Resets the position in the level propert stack, effectively clearing it.
 */
void level_properties_reset(void) {
    gLevelPropertyStackPos = 0;
}

/**
 * Returns the position of the level property stack.
 * Should always return a multiple of 4.
 */
s16 level_properties_get(void) {
    return gLevelPropertyStackPos;
}

s32 func_8006C300(void) {
    if (D_800DD330 >= 2) {
        D_800DD330 = 1;
        return 0;
    } else {
        return D_800DD330;
    }
}
