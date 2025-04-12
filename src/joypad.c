#include "joypad.h"
#include "game.h"
#include "printf.h"
#include "objects.h"
#include "math_util.h"
#include "macros.h"

s32 sNoControllerPluggedIn =
    FALSE; // Looks to be a boolean for whether a controller is plugged in. FALSE if plugged in, and TRUE if not.
u16 gButtonMask = 0xFFFF; // Used when anti-cheat/anti-tamper has failed in init_level_globals()

OSMesgQueue sSIMesgQueue;
OSMesg sSIMesgBuf;
OSMesg gSIMesg;
OSContStatus gControllerStatus[MAXCONTROLLERS];
OSContPad gControllerCurrData[MAXCONTROLLERS];
OSContPad gControllerPrevData[MAXCONTROLLERS];
u16 gControllerButtonsPressed[MAXCONTROLLERS];
u16 gControllerButtonsReleased[MAXCONTROLLERS];
u8 sPlayerID[16];

/**
 * Return the serial interface message queue.
 * Official name: joyMessageQ
 */
OSMesgQueue *si_mesg(void) {
    return &sSIMesgQueue;
}

/**
 * Initialise the player controllers, and return the status when finished.
 * Official name: joyInit
 */
s32 input_init(void) {
    UNUSED s32 *temp1;
    u8 bitpattern;
    UNUSED s32 *temp2;

    osCreateMesgQueue(&sSIMesgQueue, &sSIMesgBuf, 1);
    osSetEventMesg(OS_EVENT_SI, &sSIMesgQueue, gSIMesg);
    osContInit(&sSIMesgQueue, &bitpattern, gControllerStatus);
    osContStartReadData(&sSIMesgQueue);
    input_assign_players();

    sNoControllerPluggedIn = FALSE;

    if ((bitpattern & CONT_ABSOLUTE) && (!(gControllerStatus[0].errno & CONT_NO_RESPONSE_ERROR))) {
        return CONTROLLER_EXISTS;
    }

    if (!bitpattern) {} // Fakematch

    sNoControllerPluggedIn = TRUE;

    return CONTROLLER_MISSING;
}

extern s32 gCurrentMenuId;
extern s8 gDoneTalkingToNPC[];
extern s32 sLogicUpdateRate;

s32 autoplay_drive(f32 x, f32 y, f32 z, f32 distCheck) {
    Object *obj = get_racer_object(0);
    Object_Racer *racer = (Object_Racer *) obj->unk64;
    f32 dist;
    s16 angleDiff;
    s16 base = obj->segment.trans.rotation.y_rotation - 0x4000;
    static u8 sATap = 0;
    s16 absDiff;
    static u8 sDrift = FALSE;

    angleDiff = base + atan2s(obj->segment.trans.z_position - z, obj->segment.trans.x_position - x);

    absDiff = ABS(angleDiff);

    if (absDiff > 0x200) {
        if (sDrift || (absDiff > 0x2000 && racer->vehicleID != VEHICLE_HOVERCRAFT)) {
            sDrift = TRUE;
            if (racer->vehicleID == VEHICLE_CAR) {
                gControllerCurrData[sPlayerID[0]].button |= B_BUTTON;
            }
            gControllerCurrData[sPlayerID[0]].button |= R_TRIG;
            gControllerCurrData[sPlayerID[0]].stick_x = 70;
        } else {
            gControllerCurrData[sPlayerID[0]].stick_x = (MIN(absDiff, 0x400) / 16);
        }
        if (angleDiff < 0) {
            gControllerCurrData[sPlayerID[0]].stick_x *= -1;
        }
    } else {
        sDrift = FALSE;
    }



    sATap += sLogicUpdateRate;
    if (sATap >= 15 && racer->vehicleID == VEHICLE_CAR) {
        sATap = 0;
    } else {
        gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
    }

    if (racer->vehicleID == VEHICLE_PLANE) {
        f32 mag = MIN(ABSF((y - obj->segment.trans.y_position) / 4.0f), 70);
        //render_printf("B: %2.2f\n", mag);
        if (y > obj->segment.trans.y_position) {
            gControllerCurrData[sPlayerID[0]].stick_y = -mag;
        } else {
            gControllerCurrData[sPlayerID[0]].stick_y = mag;
        }
    }


    dist = (((obj->segment.trans.x_position - x) * (obj->segment.trans.x_position - x)) + 
            ((obj->segment.trans.y_position - y) * (obj->segment.trans.y_position - y)) + 
            ((obj->segment.trans.z_position - z) * (obj->segment.trans.z_position - z)));

    if (dist < distCheck * distCheck) {
        return 1;
    }

    return 0;
}

extern s32 gObjectCount;
extern Object **gObjPtrList;

Object *autoplay_find_balloon(s32 balloonID) {
    Object *tempObj;
    s32 i;

    i = 0;
    if (gObjectCount > 0) {
        do {
            tempObj = gObjPtrList[i];
            if (!(tempObj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) && tempObj->behaviorId == BHV_GOLDEN_BALLOON) {
                Object_NPC *balloon = (Object_NPC *) tempObj->unk64;
                if (tempObj->segment.level_entry->goldenBalloon.balloonID == balloonID) {
                    return tempObj;
                }
            }
            i += 1;
        } while (i < gObjectCount);
    }
    return NULL;
}
Object *autoplay_find_balloon2(f32 x, f32 z) {
    Object *tempObj;
    Object *bestObj;
    f32 diffX;
    f32 diffZ;
    f32 distance;
    f32 bestDist;
    s32 i;

    bestDist = 0xFFFFFFFF;
    i = 0;
    bestObj = NULL;
    if (gObjectCount > 0) {
        do {
            tempObj = gObjPtrList[i];
            if (!(tempObj->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) && tempObj->behaviorId == BHV_GOLDEN_BALLOON) {
                diffX = tempObj->segment.trans.x_position - x;
                diffZ = tempObj->segment.trans.z_position - z;
                distance = sqrtf((diffX * diffX) + (diffZ * diffZ));
                if (bestDist > distance) {
                    bestDist = distance;
                    bestObj = tempObj;
                }
            }
            i += 1;
        } while (i < gObjectCount);
    }
    return bestObj;
}

extern u8 gAutoDrive;
extern Settings *gSavefileData[4];
extern s8 gDialogueSubmenu;
extern s8 sCurrentMenuID;
extern s8 gPostraceFinishState;
extern s32 gMenuOption;

void autoplay_inputs(void) {
    static u8 sControllerFlip = 0;
    static u8 sCharSelectInputs = 0;
    static u8 sTransform = 0;
    static u8 sCheckpoint = 0;
    static u8 sCheckpointID = 0;
    static u8 sBootTime = 0;
    static u8 sFileSelectInputs = 0;
    static u8 sPrevMapID;
    Object *obj;
    Object_Racer *racer;
    Settings *settings;
    Object *balloon;

    if (sBootTime < 60) {
        sBootTime++;
        return;
    }
    
    sControllerFlip ^= 1;
    if (get_game_mode() == GAMEMODE_MENU) {
        switch (gCurrentMenuId) {
            case MENU_TITLE:
            case MENU_GAME_SELECT:
                sCharSelectInputs = 0;
                sFileSelectInputs = 0;
                if (sControllerFlip) {
                    gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                    gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                } else {
                    gControllerButtonsReleased[sPlayerID[0]] |= A_BUTTON;
                }
                break;
            case MENU_FILE_SELECT:
                sCharSelectInputs = 0;
                if (sControllerFlip) {
                    if (sFileSelectInputs < 3) {
                        gControllerCurrData[sPlayerID[0]].stick_x = 70;
                        sFileSelectInputs++;
                    } else if (sFileSelectInputs >= 10) {
                        if (sFileSelectInputs == 10) {
                            gControllerCurrData[sPlayerID[0]].stick_y = -70;
                            sFileSelectInputs++;
                        } else if (gSavefileData[2]->newGame == FALSE) {
                            gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                            gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                        } else {
                            sFileSelectInputs = 3;
                        }
                    } else {
                        if (gSavefileData[2]->newGame == FALSE) {
                            sFileSelectInputs = 10;
                        } else {
                            gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                            gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                        }
                    }
                }
                break;
            case MENU_CHARACTER_SELECT:
                if (sControllerFlip) {
                    if (sCharSelectInputs < 2) {
                        sCharSelectInputs++;
                        gControllerCurrData[sPlayerID[0]].stick_y = -70;
                    } else {
                        s32 max;
                        if (is_tt_unlocked()) {
                            max = 4;
                        } else {
                            max = 3;
                        }
                        if (sCharSelectInputs >= max) {
                            gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                            gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                        } else {
                            gControllerCurrData[sPlayerID[0]].stick_x = 70;
                        }
                        sCharSelectInputs++;
                    }
                } else {
                    gControllerButtonsReleased[sPlayerID[0]] |= A_BUTTON;
                }
                break;
        }
    } else if (get_game_mode() == GAMEMODE_INGAME) {
        s32 map = get_current_map_id();
        s32 b;
        if (map != sPrevMapID) {
            sCheckpointID = 0;
            sPrevMapID = map;
        }
        settings = get_settings();
        if (settings) {
            b = *settings->balloonsPtr;
        } else {
            b = 0;
        }
        
        obj = get_racer_object(0);
        if (obj) {
            racer = (Object_Racer *) obj->unk64;
            render_printf("X: %2.2f\n", obj->segment.trans.x_position);
            render_printf("Y: %2.2f\n", obj->segment.trans.y_position);
            render_printf("Z: %2.2f\n", obj->segment.trans.z_position);
        }
        gAutoDrive = FALSE;
        switch (map) {
            case ASSET_LEVEL_CENTRALAREAHUB:
                switch (b) {
                    case 0:
                        obj = get_racer_object(0);

                        if (obj == NULL) {
                            break;
                        }
                        racer = (Object_Racer *) obj->unk64;

                        //balloon = autoplay_find_balloon2(obj->segment.trans.x_position, obj->segment.trans.z_position);

                        //if (balloon) {
                            //render_printf("%d\n", balloon->segment.level_entry->goldenBalloon.balloonID);
                        //}
                        // dino domain: 10
                        // cliff: 14
                        // ocean: 2
                        // trees: 6

                        if (sTransform != 10) {
                            if (sControllerFlip && sTransform <= 20) {
                                if (obj && racer->vehicleID == VEHICLE_PLANE) {
                                    gControllerCurrData[sPlayerID[0]].button |= B_BUTTON;
                                    gControllerButtonsPressed[sPlayerID[0]] |= B_BUTTON;
                                    if (sCurrentMenuID == 0) {
                                        sTransform = 10;
                                        sCheckpointID = 0;
                                    }
                                } else if (sCurrentMenuID == 0) {
                                    gControllerCurrData[sPlayerID[0]].button |= Z_TRIG;
                                    gControllerButtonsPressed[sPlayerID[0]] |= Z_TRIG;
                                } else if (sCurrentMenuID == DIALOGUEPAGE_TAJ_ROOT) {
                                    gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                                    gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                                } else if (sCurrentMenuID == DIALOGUEPAGE_TAJ_VEHICLE_SELECT) {
                                    if (gDialogueSubmenu != 2) {
                                        gControllerCurrData[sPlayerID[0]].stick_y = -70;
                                    } else {
                                        if (sTransform < 2) {
                                            sTransform++;
                                            if (sTransform >= 2) {
                                                gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                                                gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                                                sTransform = 50;
                                            }
                                        }
                                    }
                                }
                            }
                            if (sTransform > 20) {
                                sTransform--;
                            }
                        } else {
                            switch (sCheckpointID) {
                                case 0:
                                    if (autoplay_drive(-135.0f, 400.0f, 700.0f, 400.0f)) {
                                        sCheckpointID++;
                                    }
                                    break;
                                case 1:
                                    balloon = autoplay_find_balloon(14);
                                    if (balloon) {
                                        //render_printf("Balloon X: %2.2f\n", balloon->segment.trans.x_position);
                                        //render_printf("Balloon Y: %2.2f\n", balloon->segment.trans.y_position);
                                        //render_printf("Balloon Z: %2.2f\n", balloon->segment.trans.z_position);
                                        autoplay_drive(balloon->segment.trans.x_position, balloon->segment.trans.y_position + 50.0f, balloon->segment.trans.z_position, 1.0f);
                                    }
                                    break;
                            }
                        }
                        break;
                    case 1:
                        switch (sCheckpointID) {
                            case 1:
                                if (autoplay_drive(2032.0f, 350.0f, 2040.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 2:
                                if (autoplay_drive(3650.0f, 350.0f, 2109.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 3:
                                if (autoplay_drive(4073.0f, 0.0f, -2103.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 4:
                                balloon = autoplay_find_balloon(2);
                                if (balloon) {
                                    autoplay_drive(balloon->segment.trans.x_position, balloon->segment.trans.y_position + 50.0f, balloon->segment.trans.z_position, 1.0f);
                                }
                                break;
                        }
                        break;
                    case 2:
                        switch (sCheckpointID) {
                            case 4:
                                if (autoplay_drive(3750.0f, -100.0f, -5866.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 5:
                                if (autoplay_drive(3776.0f, 200.0f, -2641.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 6:
                                if (autoplay_drive(3537.0f, 300.0f, -2457.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 7:
                                balloon = autoplay_find_balloon(6);
                                if (balloon) {
                                    autoplay_drive(balloon->segment.trans.x_position, balloon->segment.trans.y_position + 50.0f, balloon->segment.trans.z_position, 1.0f);
                                }
                                break;
                        }
                        break;
                    case 3:
                        switch (sCheckpointID) {
                            case 7:
                                if (autoplay_drive(3537.0f, 300.0f, -2457.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 8:
                                if (autoplay_drive(3776.0f, 200.0f, -2641.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 9:
                                if (autoplay_drive(3650.0f, 200.0f, 2109.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 10:
                                if (autoplay_drive(2032.0f, 350.0f, 2040.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 11:
                                if (autoplay_drive(400.0f, 500.0f, 1257.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 12:
                                if (autoplay_drive(-2000.0f, 600.0f, 1321.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 13:
                                balloon = autoplay_find_balloon(10);
                                if (balloon) {
                                    autoplay_drive(balloon->segment.trans.x_position, balloon->segment.trans.y_position + 50.0f, balloon->segment.trans.z_position, 1.0f);
                                }
                                break;
                        }
                        break;
                    case 4:
                        if (autoplay_drive(-4000.0f, 270.0f, 2800.0f, 300.0f)) {
                            sCheckpointID++;
                        }
                    break;
                }
                break;
            case ASSET_LEVEL_DINODOMAINHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(-42, -40, 0, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    switch(b) {
                        case 4:
                            autoplay_drive(850, 0, 400, 1.0f);
                            break;
                        case 5:
                        case 9:
                            autoplay_drive(950, 0, -365, 1.0f);
                            break;
                        case 6:
                        case 10:
                            autoplay_drive(375, 0, 930, 1.0f);
                            break;
                        case 7:
                        case 11:
                            autoplay_drive(-360, 0, -910, 1.0f);
                            break;
                        case 12:
                            autoplay_drive(-782, 0, 1620, 1.0f);
                            break;
                        case 8:
                            if (settings->bosses & 2) {
                                autoplay_drive(850, 0, 400, 1.0f);
                            } else {
                                autoplay_drive(-782, 0, 1620, 1.0f);
                            }
                            break;
                    }
                }
                break;
            default:
                if (get_race_start_timer() == 0 && get_current_level_race_type() == RACETYPE_DEFAULT) {
                    obj = get_racer_object(0);
                    racer = (Object_Racer *) obj->unk64;
                    if (racer->racePosition > 1) {
                        racer->boostTimer = 1;
                        racer->boostType = BOOST_SMALL;
                    }
                    gAutoDrive = TRUE;
                    if (racer->raceFinished) {
                        if (sControllerFlip) {
                            if (gMenuOption < 1 /*&& gPostraceFinishState == POSTRACE_STAGE_OPTIONS*/) {
                                gControllerCurrData[sPlayerID[0]].stick_y = -70;
                            } else {
                                gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                                gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                            }
                        }
                    }
                }

                break;
        }
    }
}

/**
 * Reads arg0 for a set of flags on whether to read, write, or erase any save data.
 * Also reads the latest inputs from the controllers, and sets their values.
 * Official name: joyRead
 */
s32 input_update(s32 saveDataFlags, s32 updateRate) {
    Settings **allSaves;
    OSMesg unusedMsg;
    Settings *settings;
    s32 i;

    if (osRecvMesg(&sSIMesgQueue, &unusedMsg, OS_MESG_NOBLOCK) == 0) {
        // Back up old controller data
        for (i = 0; i < MAXCONTROLLERS; i++) {
            gControllerPrevData[i] = gControllerCurrData[i];
        }
        osContGetReadData(gControllerCurrData);
        if (saveDataFlags != 0) {
            settings = get_settings();
            if (SAVE_DATA_FLAG_READ_EEPROM_INDEX(saveDataFlags)) {
                read_eeprom_data(settings, SAVE_DATA_FLAG_READ_EEPROM_INDEX(saveDataFlags));
            }
            if (saveDataFlags & SAVE_DATA_FLAG_READ_ALL_SAVE_DATA) {
                allSaves = get_all_save_files_ptr();
                for (i = 0; i < NUMBER_OF_SAVE_FILES; i++) {
                    read_save_file(i, allSaves[i]);
                }
            }
            if (saveDataFlags & SAVE_DATA_FLAG_READ_SAVE_DATA) {
                read_save_file(SAVE_DATA_FLAG_READ_SAVE_FILE_NUMBER(saveDataFlags), settings);
            }
            if (SAVE_DATA_FLAG_WRITE_EEPROM_INDEX(saveDataFlags)) {
                write_eeprom_data(settings, SAVE_DATA_FLAG_WRITE_EEPROM_INDEX(saveDataFlags));
            }
            if (saveDataFlags & SAVE_DATA_FLAG_WRITE_SAVE_DATA) {
                write_save_data(SAVE_DATA_FLAG_WRITE_SAVE_FILE_NUMBER(saveDataFlags), settings);
            }
            if (saveDataFlags & SAVE_DATA_FLAG_ERASE_SAVE_DATA) {
                erase_save_file(SAVE_DATA_FLAG_WRITE_SAVE_FILE_NUMBER(saveDataFlags), settings);
            }
            // @bug These next two if statements check the same bits
            // as the ones used to set the save file number to read from.
            if (saveDataFlags & SAVE_DATA_FLAG_READ_EEPROM_SETTINGS) {
                read_eeprom_settings(get_eeprom_settings_pointer());
            }
            if (saveDataFlags & SAVE_DATA_FLAG_WRITE_EEPROM_SETTINGS) {
                write_eeprom_settings(get_eeprom_settings_pointer());
            }
            // Reset all flags
            saveDataFlags = 0;
        }
        rumble_update(updateRate);
        osContStartReadData(&sSIMesgQueue);
    }
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (sNoControllerPluggedIn) {
            gControllerCurrData[i].button = 0;
        }
        // XOR the diff between the last read of the controller data with the current read to see what buttons have been
        // pushed and released.
        gControllerButtonsPressed[i] =
            ((gControllerCurrData[i].button ^ gControllerPrevData[i].button) & gControllerCurrData[i].button) &
            gButtonMask;
        gControllerButtonsReleased[i] =
            ((gControllerCurrData[i].button ^ gControllerPrevData[i].button) & gControllerPrevData[i].button) &
            gButtonMask;
    }
#ifdef AUTOPLAY
    autoplay_inputs();
#endif
    return saveDataFlags;
}

/**
 * Set the first 4 player ID's to the controller numbers, so players can input in the menus after boot.
 * Official name: joyResetMap
 */
void input_assign_players(void) {
    s32 i;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        sPlayerID[i] = i;
    }
}

/**
 * Assign the first four player ID's to the index of the connected players.
 * Assign the next four player ID's to the index of the players who are not connected.
 * Official name: joyCreateMap
 */
void charselect_assign_players(s8 *activePlayers) {
    s32 i;
    s32 temp = 0;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (activePlayers[i]) {
            sPlayerID[temp++] = i;
        }
    }
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (!activePlayers[i]) {
            sPlayerID[temp++] = i;
        }
    }
}

/**
 * Returns the id of the selected index.
 * Official name: joyGetController
 */
u8 input_player_id(s32 player) {
    return sPlayerID[player];
}

/**
 * Swaps the ID's of the first two indexes.
 * This applies in 2 player adventure, so that player 2 can control the car in the overworld.
 */
void input_swap_id(void) {
    u8 tempID = sPlayerID[0];
    sPlayerID[0] = sPlayerID[1];
    sPlayerID[1] = tempID;
}

/**
 * Returns the buttons that are currently pressed down on the controller.
 * Official name: joyGetButtons
 */
u16 input_held(s32 player) {
    return gControllerCurrData[sPlayerID[player]].button;
}

/**
 * Returns the buttons that are newly pressed during that frame.
 * NOTE: This was a u16, but we only got a match in menu_ghost_data_loop when it was a u32 for some reason
 * Official name: joyGetPressed
 */
u32 input_pressed(s32 player) {
    return gControllerButtonsPressed[sPlayerID[player]];
}

/**
 * Returns the buttons that are no longer pressed in that frame.
 * Official name: joyGetReleased
 */
u16 input_released(s32 player) {
    return gControllerButtonsReleased[sPlayerID[player]];
}

/**
 * Clamps the X joystick axis of the selected player to 70 and returns it.
 * Official name: joyGetStickX
 */
s32 input_clamp_stick_x(s32 player) {
    return input_clamp_stick_mag(gControllerCurrData[sPlayerID[player]].stick_x);
}

/**
 * Clamps the Y joystick axis of the selected player to 70 and returns it.
 * Official name: joyGetStickY
 */
s32 input_clamp_stick_y(s32 player) {
    return input_clamp_stick_mag(gControllerCurrData[sPlayerID[player]].stick_y);
}

/**
 * Keeps the joysticks axis reads no higher than 70 (of a possible 127 or -128)
 * Will also pull the reading towards the centre.
 */
s8 input_clamp_stick_mag(s8 stickMag) {
    if (stickMag < JOYSTICK_DEADZONE && stickMag > -JOYSTICK_DEADZONE) {
        return 0;
    }
    if (stickMag > 0) {
        stickMag -= JOYSTICK_DEADZONE;
        if (stickMag > JOYSTICK_MAX_RANGE) {
            stickMag = JOYSTICK_MAX_RANGE;
        }
    } else {
        stickMag += JOYSTICK_DEADZONE;
        if (stickMag < -JOYSTICK_MAX_RANGE) {
            stickMag = -JOYSTICK_MAX_RANGE;
        }
    }
    return stickMag;
}

/**
 * Used when anti-cheat/anti-tamper has failed in init_level_globals()
 * Official Name: joySetSecurity
 */
void drm_disable_input(void) {
    gButtonMask = 0;
}
