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

s32 autoplay_drive(f32 x, f32 y, f32 z) {
    Object *obj = get_racer_object(0);
    f32 dist;
    s16 angleDiff;
    s16 base = obj->segment.trans.rotation.y_rotation + 0x8000;

    render_printf("X: %2.2f\n", obj->segment.trans.x_position);
    render_printf("Y: %2.2f\n", obj->segment.trans.y_position);
    render_printf("Z: %2.2f\n", obj->segment.trans.z_position);

    angleDiff = base - atan2s(obj->segment.trans.x_position - x, obj->segment.trans.z_position - z);

    if (ABSF(angleDiff) > 0x400) {
        if (angleDiff > 0) {
            gControllerCurrData[sPlayerID[0]].stick_x = -70;
        } else {
            gControllerCurrData[sPlayerID[0]].stick_x = 70;
        }
    } else {
        gControllerCurrData[sPlayerID[0]].stick_x = 0;
    }

    gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;

    dist = (((obj->segment.trans.x_position - x) * (obj->segment.trans.x_position - x)) + 
            ((obj->segment.trans.y_position - y) * (obj->segment.trans.y_position - y)) + 
            ((obj->segment.trans.z_position - z) * (obj->segment.trans.z_position - z)));

    if (dist < 300.0f * 300.0f) {
        return 1;
    }

    return 0;
}

extern u8 gAutoDrive;

void autoplay_inputs(void) {
    static u8 sControllerFlip = 0;
    static u8 sCharSelectInputs = 0;
    static u8 sTransform = 0;
    static u8 sCheckpoint = 0;
    static u8 sCheckpointID = 0;
    static u8 sBootTime = 0;
    static u8 sPrevMapID;
    Object *obj;
    Object_Racer *racer;

    if (sBootTime < 60) {
        sBootTime++;
        return;
    }
    sControllerFlip ^= 1;
    if (get_game_mode() == GAMEMODE_MENU) {
        switch (gCurrentMenuId) {
            case MENU_TITLE:
            case MENU_GAME_SELECT:
            case MENU_FILE_SELECT:
                sCharSelectInputs = 0;
                if (sControllerFlip) {
                    gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                    gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                } else {
                    gControllerButtonsReleased[sPlayerID[0]] |= A_BUTTON;
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
        if (map != sPrevMapID) {
            sCheckpointID = 0;
            sPrevMapID = map;
        }
        Settings *settings = get_settings();
        s32 b = *settings->balloonsPtr;
        gAutoDrive = FALSE;
        switch (map) {
            case ASSET_LEVEL_CENTRALAREAHUB:
                switch (sCheckpointID) {
                    case 0:
                    if (autoplay_drive(-135.0f, 250.0f, 700.0f)) {
                        sCheckpointID++;
                    }
                    break;
                    case 1:
                    if (autoplay_drive(-2410.0f, 520.0f, 1500.0f)) {
                        sCheckpointID++;
                    }
                    break;
                    case 2:
                    if (autoplay_drive(-4000.0f, 270.0f, 2800.0f)) {
                        sCheckpointID++;
                    }
                    break;
                }
                break;
            case ASSET_LEVEL_DINODOMAINHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(-42, -40, 0)) {
                        sCheckpointID++;
                    }
                } else {
                    switch(b) {
                        case 1:
                        autoplay_drive(850, 0, 400);
                        break;
                    }
                }
                break;
            default:
                if (get_race_start_timer() == 0 && get_current_level_race_type() == RACETYPE_DEFAULT) {
                    gAutoDrive = TRUE;
                }

                break;
        }
        /*gControllerButtonsReleased[sPlayerID[0]] = 0;
        gControllerCurrData[sPlayerID[0]].button = 0;
        if (sTransform == 0) {
            gControllerButtonsPressed[sPlayerID[0]] |= Z_TRIG;
            if (gDoneTalkingToNPC[0] == FALSE) {

            } else {
                sTransform = 1;
            }
        }*/
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
