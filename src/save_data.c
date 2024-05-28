/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80072250 */

#include "save_data.h"
#include "memory.h"
#include "PR/pfs.h"
#include "PR/os_cont.h"
#include "macros.h"
#include "structs.h"
#include "camera.h"
#include "objects.h"
#include "game.h"
#include "thread3_main.h"
#include "controller.h"

/************ .data ************/

s8 *D_800DE440 = 0;

u8 gN64FontCodes[] = "\0               0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#'*+,-./:=?@";

u8 sControllerPaksPresent = 0; // Bits 0, 1, 2, and 3 of the bit pattern correspond to Controllers 1, 2, 3, and 4.
                               // 1 if a controller pak is present
s32 gRumbleKillTimer = 0;

/*******************************/

/************ .rodata ************/

// File Names
const char sDKRacingAdv1[] = "DKRACING-ADV";
const char sDKRacingAdv2[] = "DKRACING-ADV";
const char sDKRacingTimes1[] = "DKRACING-TIMES";
const char sDKRacingTimes2[] = "DKRACING-TIMES";
const char sDKRacingAdv3[] = "DKRACING-ADV";
const char sDKRacingAdv4[] = "DKRACING-ADV";
const char sDKRacingTimes3[] = "DKRACING-TIMES";
const char sDKRacingTimes4[] = "DKRACING-TIMES";

const char sDKRacingGhosts[] = "DKRACING-GHOSTS";
const char sDKRacingGhostFileExt[] = "";

/*********************************/

/************ .bss ************/

OSMesgQueue *sControllerMesgQueue;
UNUSED s32 D_80124014;

OSPfs pfs[MAXCONTROLLERS];

/* Size: 0xA bytes */
typedef struct RumbleData {
    /* 0x00 */ s16 rumbleType;        // Which rumble type was called
    /* 0x02 */ s16 initialStrength;   // What lingeringStrength gets set to when it hits zero.
    /* 0x04 */ s16 timer;             // Time left to spin the motor
    /* 0x06 */ s16 lingeringStrength; // Timer that decides to continue spinning the motor
    /* 0x08 */ s16 spinTime;          // Time the motor has been spinning
} RumbleData;
RumbleData gRumble[MAXCONTROLLERS];

#define RUMBLE_DURATION (type * 2) + 1 // Accesses the second byte of the rumble table.
#define RUMBLE_STRENGTH (type * 2)     // Accesses the first byte of the rumble table.

s16 *sRumbleTable; // Misc Asset 19, first byte is strength, second byte is duration.
/**
 * Values for Misc Asset 19
 * 00 2D 00 12
 * 00 46 00 0A
 * 00 3C 00 3C
 * 00 64 00 0F
 * 00 3C 00 36
 * 00 32 00 0F
 * 00 32 00 34
 * 00 46 00 3C
 * 00 55 00 3C
 * 00 46 00 16
 * 00 50 00 4B
 * 00 64 00 0C
 * 00 3C 00 18
 * 00 50 00 1E
 * 00 64 00 2A
 * 00 32 00 3C
 * 00 28 00 0A
 * 00 28 00 0A
 * 00 64 00 0F
 **/
u8 gRumbleOn;
u8 gRumblePresent; // Bits 0, 1, 2, and 3 of the bit pattern correspond to Controllers 1, 2, 3, and 4.
                   // 1 if a rumble pak is present
u8 gRumbleIdle;    // Bitfield showing which rumble paks are not spinning.
u8 gRumbleActive;  // Bitfield showing which rumble paks are currently spinning.
s32 gRumbleDetectionTimer;
u8 *D_801241EC;
u32 D_801241F0;
u32 D_801241F4;

/*******************************/

/**
 * Returns the player ID of the controller. Flips them around in 2 player adventure if the players have been swapped.
 */
u8 input_get_id(s32 controllerIndex) {
    if ((controllerIndex == 0 || controllerIndex == 1) && func_8000E158()) {
        controllerIndex = 1 - controllerIndex;
    }
    return get_player_id(controllerIndex);
}

/**
 * Reset the rumble state for all controllers and set whether or not to allow rumble.
 */
void rumble_init(u8 canRumble) {
    gRumbleOn = canRumble;
    if (canRumble) {
        gRumbleDetectionTimer = 121;
        gRumbleIdle = 0xF;
        return;
    }
    rumble_kill();
}

/**
 * Enable rumble for the controller using a behaviour table set by type.
 */
void rumble_set(s16 controllerIndex, u8 type) {
    s32 index;

    if (type < 19 && controllerIndex >= 0 && controllerIndex < 4) {
        index = (input_get_id(controllerIndex)) & 0xFFFF;
        if (gRumble[index].rumbleType == type) {
            if (gRumble[index].spinTime < 0) {
                gRumble[index].spinTime = -300;
            }
            gRumble[index].timer = sRumbleTable[RUMBLE_DURATION];
        } else {
            gRumble[index].rumbleType = type;
            if (sRumbleTable[RUMBLE_STRENGTH] != 0) {
                rumble_start(controllerIndex, sRumbleTable[RUMBLE_STRENGTH], sRumbleTable[RUMBLE_DURATION]);
            }
        }
    }
}

/**
 * Enable rumble for the controller using a behaviour table set by type.
 * Also control the strength arbitrarily.
 */
void rumble_set_fade(s16 controllerIndex, u8 type, f32 strength) {
    s32 index;

    if (type < 19 && controllerIndex >= 0 && controllerIndex < 4) {
        index = input_get_id(controllerIndex) & 0xFFFF;
        if (type == gRumble[index].rumbleType) {
            if (gRumble[index].spinTime < 0) {
                gRumble[index].spinTime = -300;
            }
            gRumble[index].timer = sRumbleTable[RUMBLE_DURATION];
        } else {
            if (strength < 0.0f) {
                strength = 0.0f;
            }
            if (strength > 1.0f) {
                strength = 1.0f;
            }
            gRumble[index].rumbleType = type;
            if (sRumbleTable[RUMBLE_STRENGTH] != 0) {
                rumble_start(controllerIndex, (sRumbleTable[RUMBLE_STRENGTH] * strength),
                             sRumbleTable[RUMBLE_DURATION]);
            }
        }
    }
}

/**
 * Check the controller should be able to rumble, then enable rumble for it.
 */
void rumble_start(s16 controllerIndex, s16 strength, s16 timer) {
    s16 index;
    u8 contBit;

    if (gRumbleOn && controllerIndex >= 0 && controllerIndex < 4) {
        contBit = input_get_id(controllerIndex);
        index = 0xFFFF;
        index &= contBit;
        contBit = 1 << index;
        gRumbleIdle |= contBit;
        gRumbleActive &= ~contBit;
        gRumble[index].lingeringStrength = ((strength * strength) * 0.1);
        gRumble[index].initialStrength = ((strength * strength) * 0.1);
        gRumble[index].timer = timer;
    }
}

/**
 * Stop all rumble pak activity.
 */
void rumble_kill(void) {
    gRumbleKillTimer = 3;
}

/**
 * Loop through and detect any newly connected or disconnected rumble paks.
 * Start and stop any motors set by gRumble, or stop activity altogether if requested.
 */
void rumble_update(s32 updateRate) {
    RumbleData *pak;
    s32 pfsStatus;
    u8 i;
    u8 controllerToCheck;
    u8 pfsBitPattern;

    if (__osBbIsBb) {
        return;
    }

    if (gRumbleIdle != 0 || gRumbleKillTimer != 0) {
        gRumbleDetectionTimer += updateRate;
        if (gRumbleDetectionTimer > 120) {
            gRumbleDetectionTimer = 0;
            osPfsIsPlug(sControllerMesgQueue, &pfsBitPattern);
            for (i = 0, controllerToCheck = 1; i < MAXCONTROLLERS; i++, controllerToCheck <<= 1) {
                if ((pfsBitPattern & controllerToCheck) && !(~gRumbleIdle & gRumblePresent & controllerToCheck)) {
                    if (osMotorInit(sControllerMesgQueue, &pfs[i], i) != 0) {
                        gRumbleIdle &= ~controllerToCheck;
                        gRumbleActive &= ~controllerToCheck;
                        gRumblePresent &= ~controllerToCheck;
                    } else {
                        gRumblePresent |= controllerToCheck;
                    }
                }
            }
        }
        if (gRumblePresent != 0 || gRumbleKillTimer != 0) {
            pfsStatus = 0;
            if (gRumbleKillTimer != 0) {
                osPfsIsPlug(sControllerMesgQueue, &pfsBitPattern);
            }
            for (i = 0, controllerToCheck = 1, pak = gRumble; i < MAXCONTROLLERS; i++, controllerToCheck <<= 1, pak++) {
                if (gRumbleKillTimer != 0) {
                    pak->rumbleType = pak->lingeringStrength = pak->timer = -1;
                    if (!(pfsBitPattern & controllerToCheck)) {
                        continue;
                    } else if (osMotorInit(sControllerMesgQueue, &pfs[i], i) == 0) {
                        osMotorStop(&pfs[i]);
                    }
                } else if (gRumbleIdle & gRumblePresent & controllerToCheck) {
                    if (pak->timer <= 0) {
                        gRumbleIdle &= ~controllerToCheck;
                        pak->rumbleType = -1;
                        pak->spinTime = 0;
                        osMotorStop(&pfs[i]);
                    } else {
                        pak->timer -= updateRate;
                        pak->spinTime += updateRate;
                        if (pak->spinTime < 0) {
                            continue;
                        } else if (pak->spinTime > 600) {
                            gRumbleIdle &= ~controllerToCheck;
                            pak->rumbleType = -1;
                            pak->spinTime = -300;
                            osMotorStop(&pfs[i]);
                        } else {
                            if (pak->initialStrength > 490.0f) {
                                if (!(gRumbleActive & controllerToCheck)) {
                                    pfsStatus |= osMotorStart(&pfs[i]);
                                    gRumbleActive |= controllerToCheck;
                                }
                            } else if (pak->initialStrength < 3.6f) {
                                if (gRumbleActive & controllerToCheck) {
                                    pfsStatus |= osMotorStop(&pfs[i]);
                                    gRumbleActive &= ~controllerToCheck;
                                }
                            } else if (pak->lingeringStrength >= 256) {
                                if (!(gRumbleActive & controllerToCheck)) {
                                    pfsStatus |= osMotorStart(&pfs[i]);
                                    gRumbleActive |= controllerToCheck;
                                }
                                pak->lingeringStrength -= 256;
                            } else {
                                if (gRumbleActive & controllerToCheck) {
                                    pfsStatus |= osMotorStop(&pfs[i]);
                                    gRumbleActive &= ~controllerToCheck;
                                }
                                pak->lingeringStrength += pak->initialStrength + 4;
                            }
                        }
                    }
                }
            }
            if (pfsStatus != 0) {
                gRumblePresent = 0;
            }
            if (gRumbleKillTimer != 0) {
                gRumbleKillTimer--;
            }
        }
    }
}

// arg0 is the number of bits we care about.
s32 func_80072C54(s32 arg0) {
    s32 ret;
    u32 var_v0;

    if (arg0 <= 0) {
        return 0;
    }

    ret = 0;
    var_v0 = 1 << (arg0 - 1);

    // Loop backwards through arg0
    while (arg0 != 0) {
        // After shifting D_801241F4 right 8 times
        if (D_801241F4 == 0) {
            D_801241F0 = *D_801241EC++;
            D_801241F4 = 128; // 1000 0000 in binary.
        }
        if (D_801241F0 & D_801241F4) {
            ret |= var_v0;
        }
        var_v0 >>= 1;
        D_801241F4 >>= 1;
        arg0--;
    }

    return ret;
}

// arg0 is the number of bits we care about
// arg1 is the bit being looked for.
void func_80072E28(s32 arg0, u32 arg1) {
    u32 var_v0;

    if (arg0 > 0) {
        var_v0 = 1 << (arg0 - 1);
        while (arg0 != 0) {
            if (D_801241F4 == 0) {
                *D_801241EC++ = D_801241F0;
                D_801241F0 = 0;
                D_801241F4 = 128; // Reset the byte counter, so we can shift 8 times before coming back here.
            }
            if (arg1 & var_v0) {
                D_801241F0 |= D_801241F4;
            }
            var_v0 >>= 1;
            D_801241F4 >>= 1;
            arg0--;
        }
        *D_801241EC = D_801241F0;
    }
}

void populate_settings_from_save_data(Settings *settings, u8 *saveData) {
    s32 i;
    s32 levelCount;
    s32 worldCount;
    s32 temp_v0;
    s32 var_s1;
    s16 var_a0;
    u8 temp_v1;

    clear_game_progress(settings);
    get_number_of_levels_and_worlds(&levelCount, &worldCount);
    D_801241EC = saveData;
    D_801241F0 = D_801241F4 = 0;
    var_a0 = func_80072C54(0x10) - 5;
    for (i = 2; i < 40; i++) {
        var_a0 -= saveData[i];
    } // Must be one line
    if (var_a0 == 0) {
        for (i = 0, var_s1 = 0; i < levelCount; i++) {
            temp_v0 = get_map_race_type(i);
            if ((temp_v0 == 0) || (temp_v0 & 0x40) || (temp_v0 == 8)) {
                temp_v1 = func_80072C54(2);
                if (temp_v1 > 0) {
                    // Set Map Visited
                    settings->courseFlagsPtr[i] |= RACE_VISITED;
                }
                if (temp_v1 >= 2) {
                    // Set Map Completed
                    settings->courseFlagsPtr[i] |= RACE_CLEARED;
                }
                if (temp_v1 >= 3) {
                    // Set Map Silver Coin Challenge Completed
                    settings->courseFlagsPtr[i] |= RACE_CLEARED_SILVER_COINS;
                }
                var_s1 += 2;
            }
        }
        func_80072C54(0x44 - var_s1);
        settings->tajFlags = func_80072C54(6);
        settings->trophies = func_80072C54(0xA);
        settings->bosses = func_80072C54(0xC);
        for (i = 0; i < worldCount; i++) {
            settings->balloonsPtr[i] = func_80072C54(7);
        }
        settings->ttAmulet = func_80072C54(3);
        settings->wizpigAmulet = func_80072C54(3);
        for (i = 0; i < worldCount; i++) {
            settings->courseFlagsPtr[get_hub_area_id(i)] |= func_80072C54(16) << 16;
        }
        settings->keys = func_80072C54(8);
        settings->cutsceneFlags = func_80072C54(0x20);
        settings->filename = func_80072C54(0x10);
        func_80072C54(8);
        settings->newGame = FALSE;
    }
}

void func_800732E8(Settings *settings, u8 *saveData) {
    s16 var_v0;
    s8 temp_v0;
    u8 courseStatus;
    s32 levelCount;
    s32 worldCount;
    s32 i;
    s32 var_s0;

    get_number_of_levels_and_worlds(&levelCount, &worldCount);
    D_801241EC = saveData;
    D_801241F0 = 0;
    D_801241F4 = 128;
    func_80072E28(16, 0);
    for (i = 0, var_s0 = 0; i < levelCount; i++) {
        temp_v0 = get_map_race_type(i);
        if ((temp_v0 == 0) || (temp_v0 & 0x40) || (temp_v0 == 8)) {
            courseStatus = 0;
            // Map visited
            if (settings->courseFlagsPtr[i] & RACE_VISITED) {
                courseStatus = 1;
            }
            // Map completed
            if (settings->courseFlagsPtr[i] & RACE_CLEARED) {
                courseStatus++;
            }
            // Map Silver coin challenge completed
            if (settings->courseFlagsPtr[i] & RACE_CLEARED_SILVER_COINS) {
                courseStatus++;
            }
            func_80072E28(2, courseStatus);
            var_s0 += 2;
        }
    }
    func_80072E28(68 - var_s0, 0);
    func_80072E28(6, settings->tajFlags);
    func_80072E28(10, settings->trophies);
    func_80072E28(12, settings->bosses);
    for (i = 0; i < worldCount; i++) {
        func_80072E28(7, settings->balloonsPtr[i]);
    }
    func_80072E28(3, settings->ttAmulet);
    func_80072E28(3, settings->wizpigAmulet);
    for (i = 0; i < worldCount; i++) {
        func_80072E28(16, (settings->courseFlagsPtr[get_hub_area_id(i)] >> 16) & 0xFFFF);
    }
    func_80072E28(8, settings->keys);
    func_80072E28(32, settings->cutsceneFlags);
    func_80072E28(16, settings->filename);
    func_80072E28(8, 0);
    for (i = 2, var_v0 = 5; i < 40; i++) {
        var_v0 += saveData[i];
    }
    D_801241EC = saveData;
    D_801241F0 = 0;
    D_801241F4 = 128;
    func_80072E28(16, var_v0);
}

// arg1 is eepromData, from read_eeprom_data
// arg2 seems to be a flag for either lap times or course initials?
void func_80073588(Settings *settings, u8 *saveData, u8 arg2) {
    s16 availableVehicles;
    s32 levelCount;
    s32 worldCount;
    s32 i;
    s16 sum;

    clear_lap_records(settings, arg2);
    get_number_of_levels_and_worlds(&levelCount, &worldCount);

    if (arg2 & 1) {
        D_801241EC = saveData;
        D_801241F0 = 0;
        D_801241F4 = 0;
        for (i = 2, sum = 5; i < 192; i++) {
            sum += D_801241EC[i];
        }
        sum -= func_80072C54(16);
        if (sum == 0) {
            for (i = 0; i < levelCount; i++) {
                if (get_map_race_type(i) == 0) {
                    availableVehicles = get_map_available_vehicles(i);
                    // Car Available
                    if (availableVehicles & 1) {
                        settings->flapTimesPtr[0][i] = func_80072C54(16);
                        settings->flapInitialsPtr[0][i] = func_80072C54(16);
                    }
                    // Hovercraft Available
                    if (availableVehicles & 2) {
                        settings->flapTimesPtr[1][i] = func_80072C54(16);
                        settings->flapInitialsPtr[1][i] = func_80072C54(16);
                    }
                    // Plane Available
                    if (availableVehicles & 4) {
                        settings->flapTimesPtr[2][i] = func_80072C54(16);
                        settings->flapInitialsPtr[2][i] = func_80072C54(16);
                    }
                }
            }
        }
    }
    if (arg2 & 2) {
        D_801241EC = saveData + 192;
        D_801241F0 = 0;
        D_801241F4 = 0;
        for (i = 2, sum = 5; i < 192; i++) {
            sum += D_801241EC[i];
        }
        sum -= func_80072C54(16);
        if (sum == 0) {
            for (i = 0; i < levelCount; i++) {
                if (get_map_race_type(i) == 0) {
                    availableVehicles = get_map_available_vehicles(i);
                    // Car Available
                    if (availableVehicles & 1) {
                        settings->courseTimesPtr[0][i] = func_80072C54(16);
                        settings->courseInitialsPtr[0][i] = func_80072C54(16);
                    }
                    // Hovercraft Available
                    if (availableVehicles & 2) {
                        settings->courseTimesPtr[1][i] = func_80072C54(16);
                        settings->courseInitialsPtr[1][i] = func_80072C54(16);
                    }
                    // Plane Available
                    if (availableVehicles & 4) {
                        settings->courseTimesPtr[2][i] = func_80072C54(16);
                        settings->courseInitialsPtr[2][i] = func_80072C54(16);
                    }
                }
            }
        }
    }
}

void func_800738A4(Settings *settings, u8 *saveData) {
    s16 availableVehicles;
    s32 levelCount;
    s32 worldCount;
    s32 i;
    s32 vehicleCount;
    s16 sum;

    get_number_of_levels_and_worlds(&levelCount, &worldCount);
    D_801241EC = saveData;
    D_801241F0 = 0;
    D_801241F4 = 128;
    func_80072E28(16, 0);
    for (vehicleCount = 0, i = 0; i < levelCount; i++) {
        if (get_map_race_type(i) == 0) {
            availableVehicles = get_map_available_vehicles(i);
            // Car Available
            if (availableVehicles & 1) {
                func_80072E28(16, settings->flapTimesPtr[0][i]);
                func_80072E28(16, settings->flapInitialsPtr[0][i]);
                vehicleCount++;
            }
            // Hovercraft Available
            if (availableVehicles & 2) {
                func_80072E28(16, settings->flapTimesPtr[1][i]);
                func_80072E28(16, settings->flapInitialsPtr[1][i]);
                vehicleCount++;
            }
            // Plane Available
            if (availableVehicles & 4) {
                func_80072E28(16, settings->flapTimesPtr[2][i]);
                func_80072E28(16, settings->flapInitialsPtr[2][i]);
                vehicleCount++;
            }
            if (vehicleCount >= 48) {
                break;
            }
        }
    }

    D_801241EC = saveData;
    D_801241F0 = 0;
    D_801241F4 = 128;
    for (i = 2, sum = 5; i < 192; i++) {
        sum += D_801241EC[i];
    }
    func_80072E28(16, sum);
    D_801241EC = saveData + 192;
    D_801241F0 = 0;
    D_801241F4 = 128;
    func_80072E28(16, 0);
    for (i = 0; i < levelCount; i++) {
        if (get_map_race_type(i) == 0) {
            availableVehicles = get_map_available_vehicles(i);
            // Car Available
            if (availableVehicles & 1) {
                func_80072E28(16, settings->courseTimesPtr[0][i]);
                func_80072E28(16, settings->courseInitialsPtr[0][i]);
            }
            // Hovercraft Available
            if (availableVehicles & 2) {
                func_80072E28(16, settings->courseTimesPtr[1][i]);
                func_80072E28(16, settings->courseInitialsPtr[1][i]);
            }
            // Plane Available
            if (availableVehicles & 4) {
                func_80072E28(16, settings->courseTimesPtr[2][i]);
                func_80072E28(16, settings->courseInitialsPtr[2][i]);
            }
        }
    }
    D_801241EC = saveData + 192;
    D_801241F0 = 0;
    D_801241F4 = 128;
    for (i = 2, sum = 5; i < 192; i++) {
        sum += D_801241EC[i];
    }
    func_80072E28(16, sum);
}

s32 get_game_data_file_size(void) {
    return 256;
}

s32 get_time_data_file_size(void) {
    return 512;
}

SIDeviceStatus get_file_extension(s32 controllerIndex, s32 fileType, char *fileExt) {
#define BLANK_EXT_CHAR ' '
    char *fileNames[16];
    char *fileExtensions[16];
    u8 fileTypes[16];
    u32 fileSizes[16];
    s32 var_s1;
    s32 fileNum;
    char fileExtChar;
    SIDeviceStatus ret;

    fileExtChar = BLANK_EXT_CHAR;
    var_s1 = 0;
    ret = get_controller_pak_file_list(controllerIndex, 16, fileNames, fileExtensions, fileSizes, fileTypes);
    if (ret == CONTROLLER_PAK_GOOD) {
        for (fileNum = 0; fileNum < 16; fileNum++) {
            if (fileNames[fileNum] == NULL) {
                continue;
            }
            if (fileType == SAVE_FILE_TYPE_CPAK_SAVE) {
                if ((bcmp((u8 *) fileNames[fileNum], (char *) sDKRacingAdv1, strlen((char *) sDKRacingAdv2)) != 0)) {
                    continue;
                }
            } else if ((bcmp((u8 *) fileNames[fileNum], (char *) sDKRacingTimes1, strlen((char *) sDKRacingTimes2)) !=
                        0)) {
                continue;
            }

            if (fileExtChar < fileExtensions[fileNum][0]) {
                fileExtChar = fileExtensions[fileNum][0];
            }
            var_s1 |= (1 << (fileExtensions[fileNum][0] + (BLANK_EXT_CHAR - 1)));
        }
        if (fileExtChar == BLANK_EXT_CHAR) {
            fileExtChar = 'A';
        } else if (fileExtChar == 'Z') {
            for (fileExtChar = 'A'; fileExtChar <= 'Z'; var_s1 >>= 1, fileExtChar++) {
                if (!(var_s1 & 1)) {
                    break;
                }
            }
        } else {
            fileExtChar++;
        }
        // Even though file extensions are technically 4 characters, only the first is used.
        // So this will set the extension to that character and then a null terminator.
        fileExt[0] = fileExtChar;
        fileExt[1] = '\0';
    }
    return ret;
}

// Read DKRACING-ADV data into settings?
s32 read_game_data_from_controller_pak(s32 controllerIndex, char *fileExt, Settings *settings) {
    s32 *alloc;
    s32 ret;
    s32 fileNumber;
    s32 fileSize;

    // Initialize controller pak
    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | ret;
    }
    ret = get_file_number(controllerIndex, (char *) sDKRacingAdv3, fileExt, &fileNumber);
    if (ret == CONTROLLER_PAK_GOOD) {
        ret = get_file_size(controllerIndex, fileNumber, &fileSize);
        if (fileSize == 0) {
            ret = CONTROLLER_PAK_BAD_DATA;
        }
        if (ret == CONTROLLER_PAK_GOOD) {
            alloc = allocate_from_main_pool_safe(fileSize, MEMP_MISC);
            ret = read_data_from_controller_pak(controllerIndex, fileNumber, (u8 *) alloc, fileSize);

            if (ret == CONTROLLER_PAK_GOOD) {
                if (*alloc == GAMD) {
                    populate_settings_from_save_data(settings, (u8 *) (alloc + 1));
                    if (settings->newGame) {
                        ret = CONTROLLER_PAK_CHANGED;
                    }
                } else {
                    ret = CONTROLLER_PAK_BAD_DATA;
                }
            }

            free_from_memory_pool(alloc);
        }
    }

    start_reading_controller_data(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        ret |= (controllerIndex << 30);
    }
    return ret;
}

s32 write_game_data_to_controller_pak(s32 controllerIndex, Settings *arg1) {
    char *fileExt;
    u8 *gameData; // Probably a struct where the first s32 is GAMD
    s32 ret;
    s32 fileSize;

    fileSize = 256; // 256 bytes
    gameData = allocate_from_main_pool_safe(fileSize, MEMP_MISC);
    *((s32 *) gameData) = GAMD;
    func_800732E8(arg1, gameData + 4);
    ret = get_file_extension(controllerIndex, 3, (char *) &fileExt);
    if (ret == CONTROLLER_PAK_GOOD) {
        ret = write_controller_pak_file(controllerIndex, -1, (char *) sDKRacingAdv4, (char *) &fileExt, gameData,
                                        fileSize);
    }
    free_from_memory_pool(gameData);
    if (ret != CONTROLLER_PAK_GOOD) {
        ret |= (controllerIndex << 30);
    }
    return ret;
}

// Read time data from controller pak into settings
s32 read_time_data_from_controller_pak(s32 controllerIndex, char *fileExt, Settings *settings) {
    s32 *cpakData;
    s32 status;
    s32 fileNumber;
    s32 fileSize;

    status = get_si_device_status(controllerIndex);
    if (status != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | status;
    }

    status = get_file_number(controllerIndex, (char *) sDKRacingTimes3, fileExt, &fileNumber);
    if (status == CONTROLLER_PAK_GOOD) {
        status = get_file_size(controllerIndex, fileNumber, &fileSize);
        if (fileSize == 0) {
            status = CONTROLLER_PAK_BAD_DATA;
        }

        if (status == CONTROLLER_PAK_GOOD) {
            cpakData = allocate_from_main_pool_safe(fileSize, MEMP_MISC);

            status = read_data_from_controller_pak(controllerIndex, fileNumber, (u8 *) cpakData, fileSize);
            if (status == CONTROLLER_PAK_GOOD) {
                if (*cpakData == TIMD) {
                    func_80073588(settings, (u8 *) (cpakData + 1),
                                  SAVE_DATA_FLAG_READ_FLAP_TIMES | SAVE_DATA_FLAG_READ_COURSE_TIMES);
                } else {
                    status = CONTROLLER_PAK_BAD_DATA;
                }
            }

            free_from_memory_pool(cpakData);
        }
    }

    start_reading_controller_data(controllerIndex);
    if (status != CONTROLLER_PAK_GOOD) {
        status |= (controllerIndex << 30);
    }

    return status;
}

s32 write_time_data_to_controller_pak(s32 controllerIndex, Settings *arg1) {
    u8 *timeData; // Should probably be a struct or maybe an array?
    s32 ret;
    s32 fileSize;
    char *fileExt;

    fileSize = 512; // 512 bytes
    timeData = allocate_from_main_pool_safe(fileSize, MEMP_MISC);
    *((s32 *) timeData) = TIMD;
    func_800738A4(arg1, timeData + 4);
    ret = get_file_extension(controllerIndex, 4, (char *) &fileExt);
    if (ret == CONTROLLER_PAK_GOOD) {
        ret = write_controller_pak_file(controllerIndex, -1, (char *) sDKRacingTimes4, (char *) &fileExt, timeData,
                                        fileSize);
    }
    free_from_memory_pool(timeData);
    if (ret != CONTROLLER_PAK_GOOD) {
        ret |= (controllerIndex << 30);
    }
    return ret;
}

s32 save_detect(void) {
#if EEP4K || EEP16K
    return osEepromProbe(&sSIMesgQueue);
#elif SRAM
    return nuPiInitSram();
#endif
}

void save_readwrite(u64 *data, u32 offset, u32 size, s32 type) {
    u32 i;
    u32 addr;
    s32 (*func)(OSMesgQueue *, u8 address, u8 *buffer);
#if EEP4K || EEP16K
    if (type == OS_READ) {
        func = osEepromRead;
    } else {
        func = osEepromWrite;
    }
    for (i = 0, addr = offset / sizeof(u64); i < size / sizeof(u64); i++, addr++) {
        (*func)(&sSIMesgQueue, addr, (u8 *) &data[i]);
    }
#elif SRAM
    nuPiReadWriteSram(offset, (u8 *) &data[0], size, type);
#endif
}

// Returns TRUE / FALSE for whether a given save file is a new game. Also populates the settings object.
s32 read_save_file(s32 saveFileNum, Settings *settings) {
    s32 startingAddress;
    u64 *saveData;
    s32 blocks;
    s32 ret;

    if (save_detect() == 0) {
        return -1;
    }
    switch (saveFileNum) {
        case 0:
            startingAddress = 0;
            break;
        case 1:
            startingAddress = 5;
            break;
        case 2:
            startingAddress = 10;
            break;
        default:
            startingAddress = 10;
            break;
    }
    blocks = 5;
    saveData = allocate_from_main_pool_safe(blocks * sizeof(u64), MEMP_MISC);
    save_readwrite(saveData, startingAddress * sizeof(u64), blocks * sizeof(u64), OS_READ);
    populate_settings_from_save_data(settings, (u8 *) saveData);
    free_from_memory_pool(saveData);
    ret = settings->newGame;
    if (settings->newGame) {
        erase_save_file(saveFileNum, settings);
        ret = settings->newGame;
    }
    return ret;
}

void erase_save_file(s32 saveFileNum, Settings *settings) {
    s32 startingAddress;
    u8 *saveData;
    u64 *alloc;
    s32 blockSize;
    s32 levelCount;
    s32 worldCount;
    s32 i;

    if (save_detect() == 0) {
        return;
    }

    get_number_of_levels_and_worlds(&levelCount, &worldCount);
    for (i = 0; i < levelCount; i++) {
        settings->courseFlagsPtr[i] = 0;
    }
    for (i = 0; i < worldCount; i++) {
        settings->balloonsPtr[i] = 0;
    }
    settings->trophies = 0;
    settings->bosses = 0;
    settings->tajFlags = 0;
    settings->cutsceneFlags = 0;
    settings->newGame = TRUE;
    switch (saveFileNum) {
        case 0:
            startingAddress = 0;
            break;
        case 1:
            startingAddress = 5;
            break;
        case 2:
            startingAddress = 10;
            break;
        default:
            startingAddress = 10;
            break;
    }
    blockSize = 5;
    alloc = allocate_from_main_pool_safe(blockSize * sizeof(u64), MEMP_MISC);
    saveData = (u8 *) alloc;
    // Blank out the data before writing it.
    for (i = 0; i < blockSize * (s32) sizeof(u64); i++) {
        saveData[i] = 0xFF;
    } // Must be one line
    if (!is_reset_pressed()) {
        save_readwrite(alloc, startingAddress * sizeof(u64), blockSize * sizeof(u64), OS_WRITE);
    }
    free_from_memory_pool(alloc);
}

/**
 * Writes Eeprom in 5 block chunks of data starting at either 0x0, 0x5, or 0xA
 * In Save File:
 * 0x00 - 0x27 Save 1
 * 0x28 - 0x4F Save 2
 * 0x50 - 0x77 Save 3
 */
s32 write_save_data(s32 saveFileNum, Settings *settings) {
    s32 startingAddress;
    u64 *alloc;
    s32 blocks;

    if (save_detect() == 0) {
        return -1;
    }

    switch (saveFileNum) {
        case 0:
            startingAddress = 0x00 / sizeof(u64);
            break;
        case 1:
            startingAddress = 0x28 / sizeof(u64);
            break;
        case 2:
            startingAddress = 0x50 / sizeof(u64);
            break;
        default:
            startingAddress = 0x50 / sizeof(u64);
            break;
    }

    blocks = 5;
    alloc = allocate_from_main_pool_safe(blocks * sizeof(u64), MEMP_MISC);
    func_800732E8(settings, (u8 *) alloc);

    if (!is_reset_pressed()) {
        save_readwrite(alloc, startingAddress * sizeof(u64), blocks * sizeof(u64), OS_WRITE);
    }

    free_from_memory_pool(alloc);

    return 0;
}

/**
 * Read Eeprom Data for addresses 0x10 - 0x39
 * arg1 is a flag
 * arg1 is descended from (gSaveDataFlags & 3) so the first 2 bits of that value.
 * bit 1 is for 0x10 - 0x27 - SAVE_DATA_FLAG_READ_FLAP_TIMES
 * bit 2 is for 0x28 - 0x39 - SAVE_DATA_FLAG_READ_COURSE_TIMES
 */
s32 read_eeprom_data(Settings *settings, u8 flags) {
    u64 *alloc;

    if (save_detect() == 0) {
        return -1;
    }

    alloc = allocate_from_main_pool_safe(SAVE_SIZE, MEMP_MISC);

    if (flags & SAVE_DATA_FLAG_READ_FLAP_TIMES) {
        save_readwrite(alloc, EEP_FLAP_OFFSET * sizeof(u64), EEP_FLAP_SIZE * sizeof(u64), OS_READ);
        func_80073588(settings, (u8 *) alloc, SAVE_DATA_FLAG_READ_FLAP_TIMES);
    }

    if (flags & SAVE_DATA_FLAG_READ_COURSE_TIMES) {
        save_readwrite(&alloc[EEP_FLAP_SIZE], EEP_COURSE_TIME_OFFSET * sizeof(u64), EEP_COURSE_RECORD_SIZE * sizeof(u64), OS_READ);
        func_80073588(settings, (u8 *) alloc, SAVE_DATA_FLAG_READ_COURSE_TIMES);
    }

    free_from_memory_pool(alloc);
    return 0;
}

/**
 * Write Eeprom Data for addresses 0x10 - 0x39
 * 0x80 - 0x1C8 in file
 * arg1 is a flag
 * arg1 is descended from gSaveDataFlags bits 4 and 5.
 * bit 1 is for 0x10 - 0x27 - SAVE_DATA_FLAG_READ_FLAP_TIMES
 * bit 2 is for 0x28 - 0x39 - SAVE_DATA_FLAG_READ_COURSE_TIMES
 */
s32 write_eeprom_data(Settings *settings, u8 flags) {
    u64 *alloc;

    if (save_detect() == 0) {
        return -1;
    }

    alloc = allocate_from_main_pool_safe(SAVE_SIZE, MEMP_MISC);

    func_800738A4(settings, (u8 *) alloc);

    if (flags & SAVE_DATA_FLAG_READ_FLAP_TIMES) {
        if (!is_reset_pressed()) {
            save_readwrite(alloc, EEP_FLAP_OFFSET * sizeof(u64), EEP_FLAP_SIZE * sizeof(u64), OS_WRITE);
        }
    }

    if (flags & SAVE_DATA_FLAG_READ_COURSE_TIMES) {
        if (!is_reset_pressed()) {
            save_readwrite(&alloc[EEP_FLAP_SIZE], EEP_COURSE_TIME_OFFSET * sizeof(u64), EEP_COURSE_RECORD_SIZE * sizeof(u64), OS_WRITE);
        }
    }

    free_from_memory_pool(alloc);
    return 0;
}

/**
 * Calculates a checksum for the game settings, and compares
 * against this to validate the data on load.
 */
s32 calculate_eeprom_settings_checksum(u64 eepromSettings) {
    s32 ret;
    s32 i;

    ret = 5;
    for (i = 0; i <= 13; i++) {
        ret += (u16) (eepromSettings >> (i << 2)) & 0xF;
    }
    return ret;
}

/**
 * Reads eeprom address 0xF (15)
 * This contains the settings data, and global unlocks like characters
 * Address (0xF * sizeof(u64)) = 0x78 - 0x80 of the actual save data file
 */
s32 read_eeprom_settings(u64 *eepromSettings) {
    s32 temp;
    s32 sp20;

    if (save_detect() == 0) {
        return -1;
    }
    
    save_readwrite(eepromSettings, 0xF * sizeof(u64), 1 * sizeof(u64), OS_READ);
    sp20 = calculate_eeprom_settings_checksum(*eepromSettings);
    temp = *eepromSettings >> 56;
    if (sp20 != temp) {
        // bit 24 = Unknown
        // bit 25 = Seems to be a flag for whether subtitles are enabled or not.
        *eepromSettings = 0x3000000; // Sets bits 24 and 25 high
        *eepromSettings <<= 8;
        *eepromSettings >>= 8;
        *eepromSettings |= (u64) calculate_eeprom_settings_checksum(*eepromSettings) << 56;
    }

    return 1;
}

/**
 * Writes eeprom address 0xF (15)
 * This contains the settings data, and global unlocks like characters
 * Address (0xF * sizeof(u64)) = 0x78 - 0x80 of the actual save data file
 */
s32 write_eeprom_settings(u64 *eepromSettings) {
    if (save_detect() == 0) {
        return -1;
    }
    *eepromSettings <<= 8;
    *eepromSettings >>= 8;
    *eepromSettings |= (s64) (calculate_eeprom_settings_checksum(*eepromSettings)) << 56;
    if (is_reset_pressed() == 0) {
        save_readwrite(eepromSettings, 0xF * sizeof(u64), 1 * sizeof(u64), OS_WRITE);
    }
    return 1;
}

s16 calculate_ghost_header_checksum(GhostHeader *ghostHeader) {
    s16 i;
    s16 len;
    s16 sum;

    sum = 0;
    len = (s16) (ghostHeader->nodeCount * sizeof(GhostNode)) + sizeof(GhostHeader);
    for (i = 2; i < len; i++) {
        sum += ((u8 *) ghostHeader)[i];
    }
    return sum;
}

void func_80074AA8(GhostHeader *ghostHeader, s16 characterID, s16 time, s16 nodeCount, u8 *dest) {
    ghostHeader->checksum = 0;
    ghostHeader->characterID = characterID;
    ghostHeader->unk3 = 0;
    ghostHeader->time = time;
    ghostHeader->nodeCount = nodeCount;
    wcopy(dest, (u8 *) ghostHeader + 8, nodeCount * sizeof(GhostNode));
    ghostHeader->checksum = calculate_ghost_header_checksum(ghostHeader);
}

// Seems to only be called when used as an argument for savemenu_check_space. Effectively just returns 0x6700
s32 get_ghost_data_file_size(void) {
    int x = 0x1100;
    return (&x)[0] * 6 + 0x100;
}

#ifdef NON_EQUIVALENT
typedef struct unkGhostData {
    u8 unk0;
    u8 unk1;
    s16 unk2;
    u8 unk4;
    u8 unk5;
    s16 unk6;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
} unkGhostData;
// cpak_read_ghost
s32 func_80074B34(s32 controllerIndex, s16 levelId, s16 vehicleId, u16 *ghostCharacterId, s16 *ghostTime,
                  s16 *ghostNodeCount, GhostHeader *ghostData) {
#define GHSS_FILE_SIZE 0x100
    static OSPfs *pfs; // Maybe static?
    s32 i;
    u8 *cPakFile;
    s32 bytesFree;
    s32 fileNumber;
    s32 notesFree;
    s32 fileOffset;
    s32 fileSize;
    SIDeviceStatus ret;
    SIDeviceStatus retTemp;
    GhostHeader *fileData;
    unkGhostData *temp_v0_4;

    fileOffset = 0;
    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    }
    if (ghostCharacterId != NULL) {
        *ghostTime = -1;
        *ghostCharacterId = -1;
    }
    ret = get_file_number(controllerIndex, (char *) sDKRacingGhosts, (char *) sDKRacingGhostFileExt, &fileNumber);
    if (ret == CONTROLLER_PAK_GOOD) {
        cPakFile = allocate_from_main_pool_safe(GHSS_FILE_SIZE, COLOUR_TAG_BLACK);
        pfs = &pfs[controllerIndex]; // This should be uninintialized?
        if (!(pfs->status & PFS_INITIALIZED)) {
            osPfsInit(sControllerMesgQueue, &pfs, controllerIndex);
        }
        ret = read_data_from_controller_pak(controllerIndex, fileNumber, cPakFile, GHSS_FILE_SIZE);
        if (ret == CONTROLLER_PAK_GOOD) {
            // The first 4 bytes of any data from the controller pak will have initials declaring it's type.
            if (*((s32 *) cPakFile) == GHSS) {
                fileData = (GhostHeader *) (cPakFile + 4);
                for (i = 0; i < 5; i++) {
                    if (levelId == fileData[i].unk0.levelID && vehicleId == fileData[i].unk0.vehicleID) {
                        fileOffset = fileData[i].unk2;
                        fileSize = fileData[i].nodeCount - fileData[i].unk2;
                        break;
                    }
                }
                if (fileOffset == 0) {
                    ret = CONTROLLER_PAK_NO_ROOM_FOR_GHOSTS;
                    if (fileData->unk0.levelID == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                    if (fileData->unk4 == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                    temp_v0_4 = (unkGhostData *) (fileData + 1);
                    if (temp_v0_4->unk0 == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                    if (temp_v0_4->unk4 == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                    if (temp_v0_4->unk8 == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                    if (temp_v0_4->unkC == 0xFF) {
                        ret = CONTROLLER_PAK_SWITCH_TO_RUMBLE;
                    }
                }
            } else {
                ret = CONTROLLER_PAK_BAD_DATA;
            }
        }
        free_from_memory_pool(cPakFile);
        if (fileOffset != 0) {
            if (ghostCharacterId != NULL) {
                cPakFile = allocate_from_main_pool_safe(fileSize + GHSS_FILE_SIZE, COLOUR_TAG_BLACK);
                retTemp = CONTROLLER_PAK_BAD_DATA;
                if (osPfsReadWriteFile(pfs, fileNumber, PFS_READ, fileOffset, fileSize, cPakFile) == 0) {
                    fileData = (GhostHeader *) cPakFile;
                    if (calculate_ghost_header_checksum(fileData) == fileData->checksum) {
                        *ghostCharacterId = fileData->characterID;
                        *ghostTime = fileData->time;
                        *ghostNodeCount = fileData->nodeCount;
                        bcopy(fileData, ghostData, *ghostNodeCount * sizeof(GhostNode));
                        retTemp = CONTROLLER_PAK_GOOD;
                    } else {
                        retTemp = CONTROLLER_PAK_BAD_DATA;
                    }
                }
                free_from_memory_pool(cPakFile);
                ret = retTemp;
            }
        }
        if ((fileOffset != 0) && (ghostCharacterId == NULL)) {
            ret = CONTROLLER_PAK_GOOD;
        }
    }
    start_reading_controller_data(controllerIndex);
    if (ret == CONTROLLER_PAK_CHANGED) {
        if (get_free_space(controllerIndex, &bytesFree, &notesFree) == CONTROLLER_PAK_GOOD) {
            if (bytesFree < get_ghost_data_file_size() || notesFree == 0) {
                return CONTROLLER_PAK_FULL;
            }
        } else {
            return CONTROLLER_PAK_BAD_DATA;
        }
    }
    return ret;
}
#else
GLOBAL_ASM("asm/non_matchings/save_data/func_80074B34.s")
#endif

typedef struct GhostHeaderAltUnk0 {
    u8 levelID;
    u8 vehicleID; // 0 = Car, 1 = Hovercraft, 2 = Plane
} GhostHeaderAltUnk0;

/* Size: 4 bytes */
typedef struct GhostHeaderAlt {
    union {
        GhostHeaderAltUnk0 unk0;
        s16 checksum;
    };
    union {
        struct {
            u8 characterID; // 9 = T.T.
            u8 unk3;
        };
        s16 unk2;
    };
} GhostHeaderAlt;

typedef struct GhostDataData {
    s16 unk0;
    s16 unk2;
    u8 unk4;
    u8 unk5;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    u8 padB[10];
    s16 unk16;
    s16 unk18;
    s16 unk1A;
} GhostDataData;

typedef struct GhostData {
    /* 0x00 */ s32 headerId;
    union {
        /* 0x04 */ GhostHeader *ghostHeader;
        /* 0x04 */ GhostDataData *ghostData;
    };
    /* 0x08 */ u8 unk8_pad[0x14];
    /* 0x1C */ u8 unk1C;
    /* 0x1E */ s16 unk1E;
} GhostData;

#ifdef NON_EQUIVALENT
// This is just a mess until we can figure out the ghost data structs properly.
SIDeviceStatus func_80074EB8(s32 controllerIndex, s16 arg1, s16 arg2, s16 ghostCharacterId, s16 ghostTime,
                             s16 ghostNodeCount, u8 *dest) {
#define GHSS_FILE_SIZE 0x100
    SIDeviceStatus ret;
    s32 fileSize;
    u8 *temp_v0_3;
    GhostDataData *temp_v0_4;
    void *temp_v0_5;
    GhostHeader *temp_v1;
    GhostHeader *ghost;

    fileSize = 0x1100 * 6;
    temp_v0_3 = allocate_from_main_pool_safe(fileSize + SAVE_SIZE, COLOUR_TAG_BLACK);
    *((s32 *) temp_v0_3) = GHSS;
    ghost = (GhostHeader *) (temp_v0_3 + 4);
    // temp_v0_3->unk0 = GHSS;
    ghost->unk0.levelID = arg1;
    ghost->nodeCount = 0x100;
    temp_v1 = ghost + 4;
    // ghost->unkA = (s16) (ghost->nodeCount + 0x1100);
    ghost->unk5 = arg2;
    temp_v0_4 = temp_v1 + 4;
    temp_v0_4->unk0 = 0xFF;
    temp_v0_4->unk4 = 0xFF;
    temp_v0_4->unk2 = (s16) temp_v1->time;
    temp_v0_4->unk6 = (s16) temp_v1->time;
    // temp_v0_5 = temp_v1 + (3 * 4);
    // temp_v0_5->unk0 = 0xFF;
    // temp_v0_5->unk4 = 0xFF;
    // temp_v0_5->unk2 = (s16) temp_v1->time;
    // temp_v0_5->unk8 = 0xFF;
    // temp_v0_5->unk6 = (s16) temp_v1->time;
    // temp_v0_5->unkC = 0xFF;
    // temp_v0_5->unkA = (s16) temp_v1->time;
    // temp_v0_5->unkE = (s16) temp_v1->time;
    func_80074AA8((GhostHeader *) &ghost[temp_v1->unk2], ghostCharacterId, ghostTime, ghostNodeCount, dest);
    ret =
        write_controller_pak_file(controllerIndex, -1, "DKRACING-GHOSTS", "", (u8 *) ghost, fileSize + GHSS_FILE_SIZE);
    free_from_memory_pool(ghost);
    return ret;
}
#else
GLOBAL_ASM("asm/non_matchings/save_data/func_80074EB8.s")
#endif

#ifdef NON_EQUIVALENT
SIDeviceStatus func_80075000(s32 controllerIndex, s16 levelId, s16 vehicleId, s16 ghostCharacterId, s16 ghostTime,
                             s16 ghostNodeCount, GhostHeader *ghostData) {
    GhostHeaderAlt *sp70;
    s32 sp58;
    s32 fileSize;
    s32 fileNumber;
    SIDeviceStatus ret;
    s32 ghostIndex;
    s32 i;
    GhostHeader *ghostFileData;
    GhostHeaderAlt *ghostFileDataAlt;
    GhostData *fileData;
    GhostData *fileDataToWrite;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    }
    ret = get_file_number(controllerIndex, "DKRACING-GHOSTS", "", &fileNumber);
    if (ret == CONTROLLER_PAK_CHANGED) {
        start_reading_controller_data(controllerIndex);
        return func_80074EB8(controllerIndex, levelId, vehicleId, ghostCharacterId, ghostTime, ghostNodeCount,
                             &ghostData->unk0.levelID);
    }
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    } else {
        ret = get_file_size(controllerIndex, fileNumber, &fileSize);
        if (ret != CONTROLLER_PAK_GOOD) {
            start_reading_controller_data(controllerIndex);
            return ret;
        } else {
            fileData = allocate_from_main_pool_safe(fileSize + 0x100, COLOUR_TAG_BLACK);
            ret = read_data_from_controller_pak(controllerIndex, fileNumber, fileData, fileSize);
            start_reading_controller_data(controllerIndex);
            if (ret != CONTROLLER_PAK_GOOD) {
                free_from_memory_pool(fileData);
            } else {
                ret = CONTROLLER_PAK_BAD_DATA;
                if (fileData->headerId == GHSS) {
                    ghostFileData = &fileData->ghostHeader;
                    ghostFileDataAlt = (GhostHeaderAlt *) &fileData->ghostHeader;

                    for (i = 0, ghostIndex = -1; i < 6; i++) {
                        if (levelId == ghostFileDataAlt[i].unk0.levelID &&
                            vehicleId == ghostFileDataAlt[i].unk0.vehicleID) {
                            ghostIndex = i;
                            break;
                        }
                    }

                    if ((ghostIndex != -1) &&
                        (fileData[ghostFileDataAlt[ghostIndex].unk2].ghostHeader->time < ghostTime)) {
                        ghostIndex = -2;
                    }

                    if (ghostIndex == -1) {
                        ghostFileDataAlt = (GhostHeaderAlt *) &fileData->ghostHeader;
                        for (i = 0; i < 6; i++) {
                            if (ghostFileDataAlt[i].unk0.levelID == 0xFF) {
                                ghostIndex = i;
                                break;
                            }
                        }
                    }
                    if (ghostIndex == -2) {
                        ret = CONTROLLER_PAK_GOOD;
                    } else if (ghostIndex == -1) {
                        ret = CONTROLLER_PAK_NO_ROOM_FOR_GHOSTS;
                    } else {
                        sp58 = (0x1100 - ghostFileData[ghostIndex].nodeCount) + ghostFileData[ghostIndex].unk2;
                        fileDataToWrite = allocate_from_main_pool_safe(fileSize + 0x100, COLOUR_TAG_BLACK);
                        fileDataToWrite->headerId = GHSS;
                        sp70 = &fileDataToWrite->ghostHeader;
                        fileDataToWrite->unk1C = 0xFF;
                        fileDataToWrite->unk1E = (s16) (ghostFileData[3].unk2 + sp58);
                        for (i = 0; i < 6; i++) {
                            sp70[i].unk0.levelID = ghostFileData[i].unk0.levelID;
                            sp70[i].unk0.vehicleID = ghostFileData[i].unk0.vehicleID;
                            if (ghostIndex >= i) {
                                sp70[i].unk2 = ghostFileData[i].unk2;
                            } else {
                                sp70[i].unk2 = (ghostFileData[i].unk2 + sp58);
                            }
                            bcopy(&fileData[ghostFileData[i].unk2], &fileDataToWrite[sp70[i].unk2],
                                  ghostFileData[i].nodeCount - ghostFileData[i].unk2);
                        }
                        func_80074AA8((GhostHeader *) &fileDataToWrite[sp70[ghostIndex].unk2], ghostCharacterId,
                                      ghostTime, ghostNodeCount, &ghostData->unk0.levelID);
                        sp70[ghostIndex].unk0.levelID = levelId;
                        sp70[ghostIndex].unk0.vehicleID = vehicleId;
                        ret = write_controller_pak_file(controllerIndex, fileNumber, "DKRACING-GHOSTS", "",
                                                        fileDataToWrite, fileSize);
                        free_from_memory_pool(fileDataToWrite);
                    }
                }
                free_from_memory_pool(fileData);
            }
        }
    }
    return ret;
}
#else
GLOBAL_ASM("asm/non_matchings/save_data/func_80075000.s")
#endif

#ifdef NON_EQUIVALENT
// I give up. Ghost data structs have be beat for now.

s32 func_800753D8(s32 controllerIndex, s32 arg1) {
    GhostData *dataToWrite;
    s32 fileNumber;
    s32 fileLength;
    u8 *sp28;
    u8 *sp20;
    s16 temp_t9_2;
    s16 temp_t9_3;
    s32 temp_a3;
    s32 temp_t8;
    s32 temp_t8_2;
    s32 var_at;
    s32 ret;
    s32 var_v1_4;
    GhostDataData *temp_t1;
    GhostData *fileData;
    GhostData *temp_v0_6;
    u8 *temp_v1;
    u8 temp_t2_2;
    u8 temp_t3;
    u8 temp_t7;
    u8 temp_t7_2;
    u8 temp_t8_3;
    u8 temp_t8_4;
    GhostDataData *temp_v0_5;
    u8 *temp_v0_8;
    GhostDataData *var_v0;
    u8 *var_v0_2;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    }
    ret = get_file_number(controllerIndex, "DKRACING-GHOSTS", "", &fileNumber);
    if (ret == CONTROLLER_PAK_GOOD) {
        ret = get_file_size(controllerIndex, fileNumber, &fileLength);
        if (ret != CONTROLLER_PAK_GOOD) {
            start_reading_controller_data(controllerIndex);
            return ret;
        }
        fileData = (GhostData *) allocate_from_main_pool_safe(fileLength + 0x100, COLOUR_TAG_BLACK);
        ret = read_data_from_controller_pak(controllerIndex, fileNumber, (u8 *) fileData, fileLength);
        start_reading_controller_data(controllerIndex);
        if (ret == CONTROLLER_PAK_GOOD) {
            ret = CONTROLLER_PAK_BAD_DATA;
            if (fileData->headerId == GHSS) {
                temp_t8 = arg1 * 4;
                temp_v0_5 = (GhostDataData *) &fileData[temp_t8];
                temp_a3 = temp_v0_5->unk6 - temp_v0_5->unkA;
                temp_v0_6 = (GhostData *) allocate_from_main_pool_safe(fileLength + 0x100, COLOUR_TAG_BLACK);
                temp_v1 = fileData->ghostHeader;
                dataToWrite = temp_v0_6;
                temp_t1 = &temp_v1[temp_t8];
                sp20 = temp_t1;
                sp28 = temp_v1;
                bcopy(fileData, temp_v0_6, temp_t1->unk2);
                if (arg1 != 5) {
                    bcopy(&fileData[temp_t1->unk6], &(&dataToWrite[temp_t1->unk6])[temp_a3],
                          fileData->ghostData->unk1A - temp_t1->unk6);
                }
                var_v0 = &dataToWrite[arg1].ghostData;
                for (var_v1_4 = arg1; var_v1_4 < 6; var_v1_4++) {
                    var_v0[var_v1_4].unk2 = (var_v0[var_v1_4].unk6 + temp_a3);
                    var_v0[var_v1_4].unk18 = var_v0[var_v1_4].unk4;
                    var_v0[var_v1_4].unk1A = var_v0[var_v1_4].unk5;
                }
                dataToWrite->ghostData->unk1A = dataToWrite->ghostData->unk16;
                ret = write_controller_pak_file(controllerIndex, fileNumber, "DKRACING-GHOSTS", "", (u8 *) dataToWrite,
                                                fileLength);
            }
            free_from_memory_pool(dataToWrite);
        }
        free_from_memory_pool(fileData);
    } else {
        start_reading_controller_data(controllerIndex);
    }

    return ret;
}
#else
GLOBAL_ASM("asm/non_matchings/save_data/func_800753D8.s")
#endif

SIDeviceStatus func_800756D4(s32 controllerIndex, u8 *levelIDs, u8 *vehicleIDs, u8 *characterIDs, s16 *checksumIDs) {
    s32 i;
    u8 *fileData;
    s32 ret; // sp64
    GhostHeaderAlt *var_s1;
    s32 fileNumber;
    s32 fileSize;
    u8 temp_v0_2;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    }
    for (i = 0; i < 6; i++) {
        levelIDs[i] = 0xFF;
        checksumIDs[i] = 0;
        temp_v0_2 = checksumIDs[i];
        characterIDs[i] = temp_v0_2;
        vehicleIDs[i] = temp_v0_2;
    }
    ret = get_file_number(controllerIndex, "DKRACING-GHOSTS", "", &fileNumber);
    if (ret == CONTROLLER_PAK_GOOD) {
        ret = get_file_size(controllerIndex, fileNumber, &fileSize);
        if (ret == CONTROLLER_PAK_GOOD) {
            fileData = allocate_from_main_pool_safe(fileSize + 0x100, MEMP_MISC);
            ret = read_data_from_controller_pak(controllerIndex, fileNumber, (u8 *) fileData, fileSize);
            if (ret == CONTROLLER_PAK_GOOD) {
                for (i = 0, var_s1 = (GhostHeaderAlt *) (&fileData[4]); i < 6; i++) {
                    if (var_s1[i].unk0.levelID != 0xFF) {
                        if (calculate_ghost_header_checksum((GhostHeader *) &fileData[var_s1[i].unk2]) !=
                            ((GhostHeaderAlt *) &fileData[var_s1[i].unk2])->checksum) {
                            ret = CONTROLLER_PAK_BAD_DATA;
                            break;
                        } else {
                            levelIDs[i] = var_s1[i].unk0.levelID;
                            vehicleIDs[i] = var_s1[i].unk0.vehicleID;
                            characterIDs[i] = fileData[var_s1[i].unk2 + 2];
                            checksumIDs[i] = ((GhostHeaderAlt *) &fileData[var_s1[i].unk2] + 1)->checksum;
                        }
                    }
                }
            }
            free_from_memory_pool(fileData);
        }
    }
    start_reading_controller_data(controllerIndex);
    return ret;
}

/* Official name: packOpen */
SIDeviceStatus get_si_device_status(s32 controllerIndex) {
    OSMesg unusedMsg;
    s32 ret;
    s32 bytes_not_used;
    s32 i;

    if (sControllerMesgQueue->validCount == 0 && __osBbIsBb == FALSE) {
        if (osMotorInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex) == 0) {
            return CONTROLLER_PAK_RUMBLE_PAK_FOUND;
        }
    }

    // Couldn't get a for loop to match this
    i = 0;
    while (sControllerMesgQueue->validCount != 0 && i < 10) {
        osRecvMesg(sControllerMesgQueue, &unusedMsg, OS_MESG_NOBLOCK);
        i++;
    }

    for (i = 0; i <= 4; i++) {
        ret = osPfsFreeBlocks(&pfs[controllerIndex], &bytes_not_used);
        if (ret == PFS_ERR_INVALID) {
            ret = osPfsInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex);
        }
        if (ret == PFS_ERR_ID_FATAL && __osBbIsBb == FALSE) {
            if (osMotorInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex) == 0) {
                return CONTROLLER_PAK_RUMBLE_PAK_FOUND;
            }
        }
        if (ret == PFS_ERR_NEW_PACK && __osBbIsBb == FALSE) {
            if ((osPfsInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex) == PFS_ERR_ID_FATAL) &&
                (osMotorInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex) == 0)) {
                return CONTROLLER_PAK_RUMBLE_PAK_FOUND;
            }
            return CONTROLLER_PAK_CHANGED;
        }
        if (ret == PFS_ERR_NOPACK || ret == PFS_ERR_DEVICE) {
            return CONTROLLER_PAK_NOT_FOUND;
        }
        if (ret == PFS_ERR_BAD_DATA) {
            return CONTROLLER_PAK_BAD_DATA;
        }
        if (ret == PFS_ERR_ID_FATAL) {
            return CONTROLLER_PAK_WITH_BAD_ID;
        }
        if (ret == PFS_ERR_INCONSISTENT) {
            return CONTROLLER_PAK_INCONSISTENT;
        }
        if (ret == 0) {
            return CONTROLLER_PAK_GOOD;
        }
    }

    return CONTROLLER_PAK_NOT_FOUND;
}

/* Official name: packClose */
s32 start_reading_controller_data(UNUSED s32 controllerIndex) {
    osContStartReadData(sControllerMesgQueue);
    return 0;
}

void init_controller_paks(void) {
    s32 controllerIndex;
    s32 ret;
    u8 controllerBit;
    u8 pakPattern;
    s8 maxControllers;

    sControllerMesgQueue = &sSIMesgQueue;
    sRumbleTable = (s16 *) get_misc_asset(ASSET_MISC_RUMBLE_DATA);
    gRumbleIdle = gRumbleActive = 0xF;
    gRumbleOn = TRUE;
    gRumbleDetectionTimer = 0;
    gRumbleKillTimer = 1;
    sControllerPaksPresent = gRumblePresent = 0;

    // pakPattern will set the first 4 bits representing each controller
    // and it will be 1 if there's something attached.
    osPfsIsPlug(sControllerMesgQueue, &pakPattern);

    for (controllerIndex = 0, controllerBit = 1, maxControllers = MAXCONTROLLERS; controllerIndex != maxControllers;
         controllerIndex++, controllerBit <<= 1) {
        gRumble[controllerIndex].initialStrength = 0;
        gRumble[controllerIndex].timer = -1;
        gRumble[controllerIndex].rumbleType = -1;
        gRumble[controllerIndex].lingeringStrength = gRumble[controllerIndex].initialStrength;

        // If something is plugged into the controller
        if (pakPattern & controllerBit) {
            ret = osPfsInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex);
            if (ret == PFS_ERR_NEW_PACK) {
                ret = osPfsInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex);
            }
            if (ret == 0) {
                // If we found a controller pak, set the bit that has one
                sControllerPaksPresent |= controllerBit;
            } else if (ret == PFS_ERR_ID_FATAL) {
                ret = osMotorInit(sControllerMesgQueue, &pfs[controllerIndex], controllerIndex);
                if (ret == 0) {
                    // If we found a rumble pak, set the bit that has one
                    gRumblePresent |= controllerBit;
                }
            }
        }
    }

    osContStartReadData(sControllerMesgQueue);
}

// Inspects and repairs the Controller Pak's file system
/* Official name: packRepair */
SIDeviceStatus repair_controller_pak(s32 controllerIndex) {
    s32 ret;
    s32 status = get_si_device_status(controllerIndex);
    if (status == CONTROLLER_PAK_GOOD || status == CONTROLLER_PAK_INCONSISTENT) {
        status = osPfsChecker(&pfs[controllerIndex]);
        if (status == 0) {
            ret = CONTROLLER_PAK_GOOD;
        } else if (status == PFS_ERR_NEW_PACK) {
            ret = CONTROLLER_PAK_CHANGED;
        } else {
            ret = CONTROLLER_PAK_INCONSISTENT;
        }
    } else {
        ret = CONTROLLER_PAK_GOOD;
    }
    start_reading_controller_data(controllerIndex);
    return ret;
}

// Reformat Controller Pak
/* Official name: packFormat */
SIDeviceStatus reformat_controller_pak(s32 controllerIndex) {
    s32 ret;
    s32 status = get_si_device_status(controllerIndex);
    if (status == CONTROLLER_PAK_GOOD || status == CONTROLLER_PAK_INCONSISTENT ||
        status == CONTROLLER_PAK_WITH_BAD_ID) {
        status = osPfsReFormat(&pfs[controllerIndex], sControllerMesgQueue, controllerIndex);
        if (status == 0) {
            ret = CONTROLLER_PAK_GOOD;
        } else if (status == PFS_ERR_NEW_PACK) {
            ret = CONTROLLER_PAK_CHANGED;
        } else {
            ret = CONTROLLER_PAK_INCONSISTENT;
        }
    } else {
        ret = CONTROLLER_PAK_GOOD;
    }
    start_reading_controller_data(controllerIndex);
    return ret;
}

/* Official Name: packDirectory */
s32 get_controller_pak_file_list(s32 controllerIndex, s32 maxNumOfFilesToGet, char **fileNames, char **fileExtensions,
                                 u32 *fileSizes, u8 *fileTypes) {
    OSPfsState state;
    s32 ret;
    s32 maxNumOfFilesOnCpak;
    s32 files_used;
    s8 *temp_D_800DE440;
    s32 i;
    u32 gameCode;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | ret;
    }

    if (osPfsNumFiles(&pfs[controllerIndex], &maxNumOfFilesOnCpak, &files_used) != 0) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | CONTROLLER_PAK_BAD_DATA;
    }

    if (get_language() == LANGUAGE_JAPANESE) {
        gameCode = JPN_GAME_CODE;
    } else if (osTvType == TV_TYPE_PAL) {
        gameCode = PAL_GAME_CODE;
    } else {
        gameCode = NTSC_GAME_CODE;
    }

    if (maxNumOfFilesToGet < maxNumOfFilesOnCpak) {
        maxNumOfFilesOnCpak = maxNumOfFilesToGet;
    }

    if (D_800DE440 != NULL) {
        free_from_memory_pool(D_800DE440);
    }

    files_used = maxNumOfFilesOnCpak * 24;
    D_800DE440 = allocate_from_main_pool_safe(files_used, MEMP_MISC);
    bzero(D_800DE440, files_used);
    temp_D_800DE440 = D_800DE440;

    // TODO: There's probably an unidentified struct here
    for (i = 0; i < maxNumOfFilesOnCpak; i++) {
        fileNames[i] = (char *) temp_D_800DE440;
        temp_D_800DE440 += 0x12;
        fileExtensions[i] = (char *) temp_D_800DE440;
        fileSizes[i] = 0;
        fileTypes[i] = SAVE_FILE_TYPE_UNSET;
        temp_D_800DE440 += 6;
    }

    while (i < maxNumOfFilesToGet) {
        fileExtensions[i] = 0;
        fileNames[i] = 0;
        fileSizes[i] = 0;
        fileTypes[i] = SAVE_FILE_TYPE_UNSET;
        i++;
    }

    for (i = 0; i < maxNumOfFilesOnCpak; i++) {
        ret = osPfsFileState(&pfs[controllerIndex], i, &state);
        if (ret == PFS_ERR_INVALID) {
            fileNames[i] = 0;
            continue;
        }

        if (ret != 0) {
            start_reading_controller_data(controllerIndex);
            return CONTROLLER_PAK_BAD_DATA;
        }

        font_codes_to_string((char *) &state.game_name, (char *) fileNames[i], PFS_FILE_NAME_LEN);
        font_codes_to_string((char *) &state.ext_name, (char *) fileExtensions[i], PFS_FILE_EXT_LEN);
        fileSizes[i] = state.file_size;
        fileTypes[i] = SAVE_FILE_TYPE_CPAK_OTHER;

        if ((state.game_code == gameCode) && (state.company_code == COMPANY_CODE)) {
            fileTypes[i] = get_file_type(controllerIndex, i);
        }
    }

    start_reading_controller_data(controllerIndex);
    return CONTROLLER_PAK_GOOD;
}

// Free D_800DE440
void packDirectoryFree(void) {
    if (D_800DE440 != 0) {
        free_from_memory_pool(D_800DE440);
    }
    D_800DE440 = 0;
}

// Get Available Space in Controller Pak
// Upper bytes of return value could be controllerIndex
/* Official Name: packFreeSpace */
s32 get_free_space(s32 controllerIndex, u32 *bytesFree, s32 *notesFree) {
    s32 ret;
    s32 bytesNotUsed;
    s32 maxNotes;
    s32 notesUsed;

    ret = get_si_device_status(controllerIndex);
    if (ret == CONTROLLER_PAK_GOOD) {
        if (bytesFree != 0) {
            ret = osPfsFreeBlocks(&pfs[controllerIndex], &bytesNotUsed);
            if (ret != 0) {
                start_reading_controller_data(controllerIndex);
                return (controllerIndex << 30) | CONTROLLER_PAK_BAD_DATA;
            }
            *bytesFree = bytesNotUsed;
        }
        if (notesFree != 0) {
            ret = osPfsNumFiles(&pfs[controllerIndex], &maxNotes, &notesUsed);
            if (ret != 0) {
                start_reading_controller_data(controllerIndex);
                return (controllerIndex << 30) | CONTROLLER_PAK_BAD_DATA;
            }
            if (notesUsed >= 16) {
                *notesFree = 0;
            } else {
                *notesFree = 16 - notesUsed;
            }
        }
    } else {
        ret |= controllerIndex << 30;
    }
    start_reading_controller_data(controllerIndex);
    return ret;
}

/* Official Name: packDeleteFile */
s32 delete_file(s32 controllerIndex, s32 fileNum) {
    OSPfsState state;
    s32 ret;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | ret;
    }

    ret = (controllerIndex << 30) | CONTROLLER_PAK_BAD_DATA;

    if (osPfsFileState(&pfs[controllerIndex], fileNum, &state) == 0) {
        if (osPfsDeleteFile(&pfs[controllerIndex], state.company_code, state.game_code, (u8 *) &state.game_name,
                            (u8 *) &state.ext_name) == 0) {
            ret = CONTROLLER_PAK_GOOD;
        }
    }

    start_reading_controller_data(controllerIndex);

    return ret;
}

// Copies a file from one controller pak to the other
/* Official Name: packCopyFile */
s32 copy_controller_pak_data(s32 controllerIndex, s32 fileNumber, s32 secondControllerIndex) {
    char fileName[PFS_FILE_NAME_LEN];
    char fileExt[PFS_FILE_EXT_LEN];
    OSPfsState state;
    s32 status;
    u8 *alloc;

    status = get_si_device_status(controllerIndex);
    if (status != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | status;
    }

    if (osPfsFileState(&pfs[controllerIndex], fileNumber, &state) != 0) {
        start_reading_controller_data(controllerIndex);
        return (controllerIndex << 30) | CONTROLLER_PAK_BAD_DATA;
    }

    alloc = allocate_from_main_pool_safe(state.file_size, MEMP_MISC);

    status = read_data_from_controller_pak(controllerIndex, fileNumber, alloc, state.file_size);
    start_reading_controller_data(controllerIndex);
    if (status != CONTROLLER_PAK_GOOD) {
        free_from_memory_pool(alloc);
        return (controllerIndex << 30) | status;
    }

    font_codes_to_string(state.game_name, fileName, PFS_FILE_NAME_LEN);
    font_codes_to_string(state.ext_name, fileExt, PFS_FILE_EXT_LEN);

    status = write_controller_pak_file(secondControllerIndex, -1, fileName, fileExt, alloc, state.file_size);
    if (status != CONTROLLER_PAK_GOOD) {
        status |= (secondControllerIndex << 30);
    }

    free_from_memory_pool(alloc);
    return status;
}

// There is likely a struct that looks like this in here.
// typedef struct fileStruct {
//     s32 fileType;
//     u8 game_name[PFS_FILE_NAME_LEN];
// } fileStruct;
/* Official name: packOpenFile */
SIDeviceStatus get_file_number(s32 controllerIndex, char *fileName, char *fileExt, s32 *fileNumber) {
    u32 gameCode;
    char fileNameAsFontCodes[PFS_FILE_NAME_LEN];
    char fileExtAsFontCodes[PFS_FILE_EXT_LEN];
    s32 ret;

    string_to_font_codes(fileName, fileNameAsFontCodes, PFS_FILE_NAME_LEN);
    string_to_font_codes(fileExt, fileExtAsFontCodes, PFS_FILE_EXT_LEN);

    if (get_language() == LANGUAGE_JAPANESE) {
        gameCode = JPN_GAME_CODE;
    } else if (osTvType == TV_TYPE_PAL) {
        gameCode = PAL_GAME_CODE;
    } else {
        gameCode = NTSC_GAME_CODE;
    }

    ret = osPfsFindFile(&pfs[controllerIndex], COMPANY_CODE, gameCode, (u8 *) fileNameAsFontCodes,
                        (u8 *) fileExtAsFontCodes, fileNumber);
    if (ret == 0) {
        return CONTROLLER_PAK_GOOD;
    }
    if ((ret == PFS_ERR_NOPACK) || (ret == PFS_ERR_DEVICE)) {
        return CONTROLLER_PAK_NOT_FOUND;
    }
    if (ret == PFS_ERR_INCONSISTENT) {
        return CONTROLLER_PAK_INCONSISTENT;
    }
    if (ret == PFS_ERR_ID_FATAL) {
        return CONTROLLER_PAK_WITH_BAD_ID;
    }
    if (ret == PFS_ERR_INVALID) {
        return CONTROLLER_PAK_CHANGED;
    }

    return CONTROLLER_PAK_BAD_DATA;
}

/* Official name: packReadFile */
SIDeviceStatus read_data_from_controller_pak(s32 controllerIndex, s32 fileNum, u8 *data, s32 dataLength) {
    s32 readResult = osPfsReadWriteFile(&pfs[controllerIndex], fileNum, PFS_READ, 0, dataLength, data);

    if (readResult == 0) {
        return CONTROLLER_PAK_GOOD;
    }
    if ((readResult == PFS_ERR_NOPACK) || (readResult == PFS_ERR_DEVICE)) {
        return CONTROLLER_PAK_NOT_FOUND;
    }
    if (readResult == PFS_ERR_INCONSISTENT) {
        return CONTROLLER_PAK_INCONSISTENT;
    }
    if (readResult == PFS_ERR_ID_FATAL) {
        return CONTROLLER_PAK_WITH_BAD_ID;
    }
    if (readResult == PFS_ERR_INVALID) {
        return CONTROLLER_PAK_CHANGED;
    }

    return CONTROLLER_PAK_BAD_DATA;
}

// If fileNumber -1, it creates a new file?
/* Official name: packWriteFile */
SIDeviceStatus write_controller_pak_file(s32 controllerIndex, s32 fileNumber, char *fileName, char *fileExt,
                                         u8 *dataToWrite, s32 fileSize) {
    s32 temp;
    u8 fileNameAsFontCodes[PFS_FILE_NAME_LEN];
    u8 fileExtAsFontCodes[PFS_FILE_EXT_LEN];
    SIDeviceStatus ret;
    s32 file_number;
    s32 bytesToSave;
    u32 game_code;

    ret = get_si_device_status(controllerIndex);
    if (ret != CONTROLLER_PAK_GOOD) {
        start_reading_controller_data(controllerIndex);
        return ret;
    }

    bytesToSave = fileSize;
    temp = fileSize & 0xFF;
    if (temp != 0) {
        bytesToSave = (fileSize - temp) + 0x100;
    }

    string_to_font_codes(fileName, (char *) fileNameAsFontCodes, PFS_FILE_NAME_LEN);
    string_to_font_codes(fileExt, (char *) fileExtAsFontCodes, PFS_FILE_EXT_LEN);

    if (get_language() == LANGUAGE_JAPANESE) {
        game_code = JPN_GAME_CODE;
    } else if (osTvType == TV_TYPE_PAL) {
        game_code = PAL_GAME_CODE;
    } else {
        game_code = NTSC_GAME_CODE;
    }

    ret = get_file_number(controllerIndex, fileName, fileExt, &file_number);
    if (ret == CONTROLLER_PAK_GOOD) {
        if (fileNumber != -1 && fileNumber != file_number) {
            ret = CONTROLLER_PAK_BAD_DATA;
        }
    } else if (ret == CONTROLLER_PAK_CHANGED) {
        if (fileNumber != -1) {
            ret = CONTROLLER_PAK_BAD_DATA;
        } else {
            temp = osPfsAllocateFile(&pfs[controllerIndex], COMPANY_CODE, game_code, fileNameAsFontCodes,
                                     fileExtAsFontCodes, bytesToSave, &file_number);
            if (temp == 0) {
                ret = CONTROLLER_PAK_GOOD;
            } else if (temp == PFS_DATA_FULL || temp == PFS_DIR_FULL) {
                ret = CONTROLLER_PAK_FULL;
            } else {
                ret = CONTROLLER_PAK_BAD_DATA;
            }
        }
    }

    if (ret == CONTROLLER_PAK_GOOD) {
        temp = osPfsReadWriteFile(&pfs[controllerIndex], file_number, PFS_WRITE, 0, bytesToSave, dataToWrite);
        if (temp == 0) {
            ret = CONTROLLER_PAK_GOOD;
        } else if ((temp == PFS_ERR_NOPACK) || (temp == PFS_ERR_DEVICE)) {
            ret = CONTROLLER_PAK_NOT_FOUND;
        } else if (temp == PFS_ERR_INCONSISTENT) {
            ret = CONTROLLER_PAK_INCONSISTENT;
        } else if (temp == PFS_ERR_ID_FATAL) {
            ret = CONTROLLER_PAK_WITH_BAD_ID;
        } else {
            ret = CONTROLLER_PAK_BAD_DATA;
        }
    }

    start_reading_controller_data(controllerIndex);
    return ret;
}

/**
 * Get File size for given controller's Control Pak
 * Official name: packFileSize
 */
SIDeviceStatus get_file_size(s32 controllerIndex, s32 fileNum, s32 *fileSize) {
    OSPfsState state;

    *fileSize = 0;
    if (osPfsFileState(&pfs[controllerIndex], fileNum, &state) == 0) {
        *fileSize = state.file_size;
        return CONTROLLER_PAK_GOOD;
    }
    return CONTROLLER_PAK_BAD_DATA;
}

/**
 * Converts N64 Font codes used in controller pak file names, into C ASCII a coded string
 */
char *font_codes_to_string(char *inString, char *outString, s32 stringLength) {
    s32 index = *inString;
    char *ret = outString;

    while (index != 0 && stringLength != 0) {
        // Less than sizeof(gN64FontCodes) - 3.
        // So basically make sure it's a valid font code value
        if (index <= 65) {
            *outString = gN64FontCodes[index];
            outString++;
        } else {
            // Replace invalid characters with a hyphen
            *outString = '-';
            outString++;
        }

        inString++;
        stringLength--;
        index = *inString;
    }

    // Pad any extra characters with NULL font code
    while (stringLength != 0) {
        *outString = 0;
        stringLength--;
        outString++;
    }

    *outString = 0;
    return ret;
}

/**
 * Converts a C ASCII string into N64 Font codes for controller pak file names
 */
char *string_to_font_codes(char *inString, char *outString, s32 stringLength) {
    s32 i;
    char currentChar;
    char *ret = outString;

    while (*inString != 0 && stringLength != 0) {
        *outString = 0;
        for (i = 0; i < 65; i++) {
            currentChar = *inString;
            if (currentChar == gN64FontCodes[i]) {
                *outString = i;
                outString++;
                break;
            }
        }

        inString++;
        stringLength--;
    }

    while (stringLength != 0) {
        *outString = 0;
        stringLength--;
        outString++;
    }

    *outString = 0;
    return ret;
}

/**
 * For the given controller, and file, this will return a value for which type of file it is.
 */
SaveFileType get_file_type(s32 controllerIndex, s32 fileNum) {
    s32 *data;
    s32 ret;

    ret = SAVE_FILE_TYPE_CPAK_OTHER;
    data = allocate_from_main_pool_safe(0x100, MEMP_MISC);
    if (read_data_from_controller_pak(controllerIndex, fileNum, (u8 *) data, 0x100) == CONTROLLER_PAK_GOOD) {
        switch (*data) {
            case GAMD:
                ret = SAVE_FILE_TYPE_CPAK_SAVE;
                break;
            case TIMD:
                ret = SAVE_FILE_TYPE_CPAK_TIMES;
                break;
            case GHSS:
                ret = SAVE_FILE_TYPE_CPAK_GHOST;
                break;
            default:
                ret = SAVE_FILE_TYPE_CPAK_OTHER;
                break;
        }
    }
    free_from_memory_pool(data);
    return ret;
}
