#include "math_util.h"
#include "macros.h"
#include "autoplay.h"
#include "objects.h"
#include "thread3_main.h"
#include "printf.h"
#include "thread30_bgload.h"
#include "ultra64.h"
#include "game.h"
#include "menu.h"
#include "textures_sprites.h"

extern s32 gCurrentMenuId;
extern s8 gDoneTalkingToNPC[];
extern s32 sLogicUpdateRate;
extern OSContPad gControllerCurrData[MAXCONTROLLERS];
extern u16 gControllerButtonsPressed[MAXCONTROLLERS];
extern u16 gControllerButtonsReleased[MAXCONTROLLERS];
extern u8 sPlayerID[16];
extern u8 gAutoDrive;
extern Settings *gSavefileData[4];
extern s8 gDialogueSubmenu;
extern s8 sCurrentMenuID;
extern s8 gPostraceFinishState;
extern s32 gMenuOption;
extern s16 gCutsceneID;
extern s32 gMenuStage;
extern s32 gMultiplayerSelectedNumberOfRacers;
extern s32 gTrackSelectCursorX;
extern s32 gTrackSelectCursorY;
extern s32 gMenuCurIndex;

u8 gAutoplayTest = AUTOPLAY_DEFAULT;

u8 sControllerFlip = 0;
u8 sCharSelectInputs = 0;
u8 sTransform = 0;
u8 sCheckpoint = 0;
u8 sCheckpointID = 0;
u8 sBootTime = 0;
u8 sFileSelectInputs = 0;
u8 sPrevMapID;
u8 sPauseStage;
u8 override = 0;
u8 sAutoplayTrackStage;
u8 sAutoplayActiveTrack;

s32 autoplay_drive(f32 x, f32 y, f32 z, f32 distCheck) {
    Object *obj = get_racer_object(0);
    Object_Racer *racer = (Object_Racer *) obj->racer;
    f32 dist;
    s16 angleDiff;
    s16 base = obj->trans.rotation.y_rotation - 0x4000;
    static u8 sATap = 0;
    s16 absDiff;
    static u8 sDrift = FALSE;

    angleDiff = base + atan2s(obj->trans.z_position - z, obj->trans.x_position - x);

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
        f32 mag = MIN(ABSF((y - obj->trans.y_position) / 4.0f), 70);
        //render_printf("B: %2.2f\n", mag);
        if (y > obj->trans.y_position) {
            gControllerCurrData[sPlayerID[0]].stick_y = -mag;
        } else {
            gControllerCurrData[sPlayerID[0]].stick_y = mag;
        }
    }


    dist = (((obj->trans.x_position - x) * (obj->trans.x_position - x)) + 
            ((obj->trans.y_position - y) * (obj->trans.y_position - y)) + 
            ((obj->trans.z_position - z) * (obj->trans.z_position - z)));

    if (racer->vehicleID == VEHICLE_PLANE) {
        if (dist > 3000.0f * 3000.0f) {
            if (ABS(gControllerCurrData[sPlayerID[0]].stick_x) < 15) {
                gControllerButtonsPressed[sPlayerID[0]] |= R_TRIG;
                gControllerCurrData[sPlayerID[0]].stick_x = 70;
                gControllerCurrData[sPlayerID[0]].stick_y = 0;
            }
        }
    }

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
            if (!(tempObj->trans.flags & OBJ_FLAGS_PARTICLE) && tempObj->behaviorId == BHV_GOLDEN_BALLOON) {
                Object_NPC *balloon = (Object_NPC *) tempObj->npc;
                if (tempObj->level_entry->goldenBalloon.balloonID == balloonID) {
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
            if (!(tempObj->trans.flags & OBJ_FLAGS_PARTICLE) && tempObj->behaviorId == BHV_GOLDEN_BALLOON) {
                diffX = tempObj->trans.x_position - x;
                diffZ = tempObj->trans.z_position - z;
                distance = ((diffX * diffX) + (diffZ * diffZ));
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

void autoplay_single_player(void) {
    Object *obj;
    Object_Racer *racer;
    Settings *settings;
    Object *balloon;
    if (get_game_mode() == GAMEMODE_MENU) {
        switch (gCurrentMenuId) {
            case MENU_GAME_SELECT:
                sTransform = 0;
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
        s32 map = level_id();
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
            racer = (Object_Racer *) obj->racer;
            //render_printf("\n\n\n\nX: %2.2f\n", obj->trans.x_position);
            //render_printf("Y: %2.2f\n", obj->trans.y_position);
            //render_printf("Z: %2.2f\n", obj->trans.z_position);
        } else {
            return;
        }
        //return;
        gAutoDrive = FALSE;
        switch (map) {
            case ASSET_LEVEL_CENTRALAREAHUB:
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
                    if (sCurrentMenuID != 0) {
                        if (sControllerFlip) {
                            gControllerCurrData[sPlayerID[0]].button |= B_BUTTON;
                            gControllerButtonsPressed[sPlayerID[0]] |= B_BUTTON;
                        }
                    }
                    if (override == 0) {
                        switch (b) {
                            case 0:
                                obj = get_racer_object(0);
        
                                if (obj == NULL) {
                                    break;
                                }
                                racer = (Object_Racer *) obj->racer;
        
                                //balloon = autoplay_find_balloon2(obj->trans.x_position, obj->trans.z_position);
        
                                //if (balloon) {
                                    //render_printf("%d\n", balloon->segment.level_entry->goldenBalloon.balloonID);
                                //}
                                // dino domain: 10
                                // cliff: 14
                                // ocean: 2
                                // trees: 6
        
                                switch (sCheckpointID) {
                                    case 0:
                                        if (autoplay_drive(-135.0f, 600.0f, 700.0f, 400.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 1:
                                        balloon = autoplay_find_balloon(14);
                                        if (balloon) {
                                            //render_printf("Balloon X: %2.2f\n", balloon->trans.x_position);
                                            //render_printf("Balloon Y: %2.2f\n", balloon->trans.y_position);
                                            //render_printf("Balloon Z: %2.2f\n", balloon->trans.z_position);
                                            autoplay_drive(balloon->trans.x_position, balloon->trans.y_position + 75.0f, balloon->trans.z_position, 1.0f);
                                        }
                                        break;
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
                                            autoplay_drive(balloon->trans.x_position, balloon->trans.y_position + 75.0f, balloon->trans.z_position, 1.0f);
                                        }
                                        break;
                                }
                                break;
                            case 2:
                                switch (sCheckpointID) {
                                    case 4:
                                        if (autoplay_drive(3300.0f, 300.0f, -3808.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 5:
                                        balloon = autoplay_find_balloon(6);
                                        if (balloon) {
                                            autoplay_drive(balloon->trans.x_position, balloon->trans.y_position + 75.0f, balloon->trans.z_position, 1.0f);
                                        }
                                        break;
                                }
                                break;
                            case 3:
                                switch (sCheckpointID) {
                                    case 5:
                                        if (autoplay_drive(3537.0f, 300.0f, -2457.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 6:
                                        if (autoplay_drive(3650.0f, 200.0f, 2109.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 7:
                                        if (autoplay_drive(2032.0f, 350.0f, 2040.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 8:
                                        if (autoplay_drive(400.0f, 500.0f, 1257.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 9:
                                        if (autoplay_drive(-2000.0f, 600.0f, 1321.0f, 300.0f)) {
                                            sCheckpointID++;
                                        }
                                        break;
                                    case 10:
                                        balloon = autoplay_find_balloon(10);
                                        if (balloon) {
                                            autoplay_drive(balloon->trans.x_position, balloon->trans.y_position + 75.0f, balloon->trans.z_position, 1.0f);
                                        }
                                        break;
                                }
                                break;
                            case 4:
                                if (autoplay_drive(-4000.0f, 600.0f, 2800.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                            break;
                        }
                    } else if (override == 1 || override == 2) {
                        switch (sCheckpointID) {
                            case 0:
                                if (autoplay_drive(-1217.0f, 250.0f, 800.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 1:
                                if (autoplay_drive(-901.0f, 550.0f, -399.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 2:
                                if (autoplay_drive(-2033.0f, 525.0f, -811.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 3:
                                if (autoplay_drive(-2726.0f, 525.0f, -833.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 4:
                                if (autoplay_drive(-3451.0f, 500.0f, -1264.0f, 300.0f)) {
                                    sCheckpointID++;
                                    if (override != 1) {
                                        override += 2;
                                        sCheckpointID = 0;
                                    }
                                }
                                break;
                            case 5:
                                if (autoplay_drive(-3794.0f, 500.0f, -3456.0f, 1.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                        }
                    } else if (override == 3 || override == 4) {
                        switch (sCheckpointID) {
                            case 0:
                                if (autoplay_drive(-3000.0f, 200.0f, -3231.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 1:
                                if (autoplay_drive(-2053.0f, 200.0f, -3214.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 2:
                                if (autoplay_drive(1287.0f, 250.0f, -3088.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 3:
                                if (autoplay_drive(5487.0f, 150.0f, -1500.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 4:
                                if (autoplay_drive(3290.0f, 100.0f, -1835.0f, 300.0f)) {
                                    sCheckpointID++;
                                    if (override != 3) {
                                        override += 2;
                                        sCheckpointID = 0;
                                    }
                                }
                                break;
                            case 5:
                                if (autoplay_drive(4723.0f, 100.0f, -300.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 6:
                                if (autoplay_drive(5066.0f, -200.0f, -1188.0f, 1.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                        }
                    } else if (override == 5) {
                        switch (sCheckpointID) {
                            case 0:
                                if (autoplay_drive(4187.0f, 300.0f, 1382.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 1:
                                if (autoplay_drive(3420.0f, 325.0f, 1992.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 2:
                                if (autoplay_drive(2406.0f, 300.0f, 2019.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 3:
                                if (autoplay_drive(862.0f, 250.0f, 2979.0f, 200.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 4:
                                if (autoplay_drive(561.0f, 600.0f, 3899.0f, 300.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 5:
                                if (autoplay_drive(620.0f, 600.0f, 4451.0f, 2.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                        }
                    } else if (override == 6) {
                        switch (sCheckpointID) {
                            case 0:
                                if (autoplay_drive(640.0f, 500.0f, 3553.0f, 250.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 1:
                                if (autoplay_drive(866.0f, 250.0f, 2959.0f, 250.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 2:
                                if (autoplay_drive(1472.0f, 350.0f, 2232.0f, 250.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 3:
                                if (autoplay_drive(2318.0f, 300.0f, 2064.0f, 250.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 4:
                                if (autoplay_drive(3368.0f, -25.0f, 2132.0f, 250.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                            case 5:
                                if (autoplay_drive(3676.0f, -100.0f, 2222.0f, 1.0f)) {
                                    sCheckpointID++;
                                }
                                break;
                        }
                    }
                }
                break;
            case ASSET_LEVEL_DINODOMAINHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(-42, -40, 0, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    s32 flag;
                    if ((settings->bosses & 2) == 0) {
                        flag = RACE_CLEARED;
                    } else {
                        sCheckpointID = 10;
                        goto actuallyJustLeave;
                        flag = RACE_CLEARED_SILVER_COINS;
                    }
                    if ((settings->courseFlagsPtr[ASSET_LEVEL_ANCIENTLAKE]) == 0) {
                        autoplay_drive(850, 0, 400, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_FOSSILCANYON]) == 0) {
                        autoplay_drive(950, 0, -365, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_JUNGLEFALLS]) == 0) {
                        autoplay_drive(375, 0, 930, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_HOTTOPVOLCANO]) == 0) {
                        autoplay_drive(-360, 0, -910, 50.0f);
                    } else {
                        autoplay_drive(-800, 0, 1800, 50.0f);
                    }
                    return;
                    actuallyJustLeave:
                    if (sCheckpointID == 10) {
                        override = 1;
                        if (autoplay_drive(-1500, 0, -700, 1.0f)) {
                            sCheckpointID++;
                        }
                    }
                }
                break;
            case ASSET_LEVEL_SNOWFLAKEMOUNTAINHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(53, 50, 30, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    s32 flag;
                    if ((settings->bosses & 8) == 0) {
                        flag = RACE_CLEARED;
                    } else {
                        sCheckpointID = 10;
                        goto actuallyJustLeave2;
                        flag = RACE_CLEARED_SILVER_COINS;
                    }
                    if ((settings->courseFlagsPtr[ASSET_LEVEL_WALRUSCOVE]) == 0) {
                        autoplay_drive(-608, 125, -938, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_FROSTYVILLAGE]) == 0) {
                        autoplay_drive(-247, 125, 1168, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_EVERFROSTPEAK]) == 0) {
                        autoplay_drive(-1208, 125, -254, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_SNOWBALLVALLEY]) == 0) {
                        autoplay_drive(-1020, 125, 515, 50.0f);
                    } else {
                        autoplay_drive(1965, 200, -1401, 50.0f);
                    }
                    return;
                    actuallyJustLeave2:
                    if (sCheckpointID == 10) {
                        override = 3;
                        if (autoplay_drive(1311, 125, 1952, 1.0f)) {
                            sCheckpointID++;
                        }
                    }
                }
                break;
            case ASSET_LEVEL_SHERBETISLANDHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(-53, 50, -10, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    s32 flag;
                    if ((settings->bosses & 4) == 0) {
                        flag = RACE_CLEARED;
                    } else {
                        sCheckpointID = 10;
                        goto actuallyJustLeave3;
                        flag = RACE_CLEARED_SILVER_COINS;
                    }
                    if ((settings->courseFlagsPtr[ASSET_LEVEL_WHALEBAY]) == 0) {
                        autoplay_drive(1274, 75, 715, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_TREASURECAVES]) == 0) {
                        autoplay_drive(-1263, 75, 750, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_CRESCENTISLAND]) == 0) {
                        autoplay_drive(730, 75, 1276, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_PIRATELAGOON]) == 0) {
                        autoplay_drive(-716, 75, 1318, 50.0f);
                    } else {
                        autoplay_drive(33, 150, -3766, 50.0f);
                    }
                    return;
                    actuallyJustLeave3:
                    if (sCheckpointID == 10) {
                        override = 5;
                        if (autoplay_drive(-1790, 125, -474, 1.0f)) {
                            sCheckpointID++;
                        }
                    }
                }
                break;
            case ASSET_LEVEL_DRAGONFORESTHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(-70, 0, -30, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    s32 flag;
                    if ((settings->bosses & 0x10) == 0) {
                        flag = RACE_CLEARED;
                    } else {
                        sCheckpointID = 10;
                        goto actuallyJustLeave4;
                        flag = RACE_CLEARED_SILVER_COINS;
                    }
                    if ((settings->courseFlagsPtr[ASSET_LEVEL_BOULDERCANYON]) == 0) {
                        autoplay_drive(1024, 0, 396, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_WINDMILLPLAINS]) == 0) {
                        autoplay_drive(-441, 0, -1034, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_GREENWOODVILLAGE]) == 0) {
                        autoplay_drive(1003, 0, -451, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_HAUNTEDWOODS]) == 0) {
                        autoplay_drive(460, 0, 1027, 50.0f);
                    } else {
                        autoplay_drive(-786, 0, 1866, 50.0f);
                    }
                    return;
                    actuallyJustLeave4:
                    if (sCheckpointID == 10) {
                        override = 6;
                        if (autoplay_drive(-2430, 0, -1035, 1.0f)) {
                            sCheckpointID++;
                        }
                    }
                }
                break;
            case ASSET_LEVEL_FUTUREFUNLANDHUB:
                if (sCheckpointID == 0) {
                    if (autoplay_drive(1699, -150, 1387, 300.0f)) {
                        sCheckpointID++;
                    }
                } else {
                    s32 flag;
                    if ((settings->bosses & 0x20) == 0) {
                        flag = RACE_CLEARED;
                    } else {
                        sCheckpointID = 10;
                        goto actuallyJustLeave5;
                        flag = RACE_CLEARED_SILVER_COINS;
                    }
                    if ((settings->courseFlagsPtr[ASSET_LEVEL_SPACEDUSTALLEY]) == 0) {
                        autoplay_drive(2600, -150, 2062, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_DARKMOONCAVERNS]) == 0) {
                        autoplay_drive(2085, -150, 2670, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_SPACEPORTALPHA]) == 0) {
                        autoplay_drive(775, -150, 2428, 50.0f);
                    } else if ((settings->courseFlagsPtr[ASSET_LEVEL_STARCITY]) == 0) {
                        autoplay_drive(468, -150, 1621, 50.0f);
                    } else {
                        autoplay_drive(3104, -150, -2074, 50.0f);
                    }
                    return;
                    actuallyJustLeave5:
                    if (sCheckpointID == 10) {
                        override = 6;
                        if (autoplay_drive(-2430, 0, -1035, 1.0f)) {
                            sCheckpointID++;
                        }
                    }
                }
                break;
            default:
                if (level_type() != RACETYPE_HUBWORLD) {
                    if (get_race_start_timer() == 0) {
                        obj = get_racer_object(0);
                        racer = (Object_Racer *) obj->racer;
                        if (racer->racePosition > 1) {
                            racer->boostTimer = 1;
                            racer->boostType = BOOST_SMALL;
                        }
                        if (racer->lap_times[racer->lap] > 2 * (60 * 60)) {
                            if (sControllerFlip) {
                                switch (sPauseStage) {
                                    case 0:
                                        gControllerCurrData[sPlayerID[0]].button |= START_BUTTON;
                                        gControllerButtonsPressed[sPlayerID[0]] |= START_BUTTON;
                                        sPauseStage++;
                                    break;
                                    case 1:
                                        gControllerCurrData[sPlayerID[0]].stick_y = -70;
                                        sPauseStage++;
                                    break;
                                    case 2:
                                        gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                                        gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                                        sPauseStage++;
                                    break;
                                }
                            }
                        } else {
                            sPauseStage = 0;
                        }
                        gAutoDrive = TRUE;
                        racer->bananas = 20;
                        if (racer->balloon_type == WEAPON_NITRO_LEVEL_1 && sControllerFlip) {
                            gControllerCurrData[sPlayerID[0]].button |= Z_TRIG;
                            gControllerButtonsPressed[sPlayerID[0]] |= Z_TRIG;
                        }
                        if (level_type() == RACETYPE_BOSS && map != ASSET_LEVEL_TRICKYTOPS1 && map != ASSET_LEVEL_TRICKYTOPS2 && map != ASSET_LEVEL_SMOKEY1 && map != ASSET_LEVEL_SMOKEY2) {
                            racer->boostTimer = 1;
                            racer->boostType = BOOST_SMALL;
                        }
                        if (racer->raceFinished) {
                            if (sControllerFlip) {
                                gMenuOption = 1;
                                gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                                gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                            }
                        }
                        if (map == ASSET_LEVEL_WIZPIG2 && racer->raceFinished && racer->finishPosition == 1) {
                            gAutoplayTest = AUTOPLAY_OFF;
                        }
                    }
                }

                break;
        }
    }
}

void autoplay_charselect_multi(s32 playerCount) {
    s32 i;
    
    sAutoplayTrackStage = 0;
    sAutoplayActiveTrack = 0;
    if (sControllerFlip) {
        for (i = 0; i < playerCount; i++) {
            if (sCharSelectInputs < 1) {
                gControllerCurrData[sPlayerID[i]].button |= START_BUTTON;
            } else {
                gControllerCurrData[sPlayerID[i]].button |= A_BUTTON;
            }
        }
        sCharSelectInputs++;
    }
}

void autoplay_tracks(s32 playerCount) {
    static u8 trackTime = 0;
    const u8 skipCheck = FALSE;
    s32 trackX;
    s32 trackY;
    s32 i;

    if (sControllerFlip) {
        gControllerCurrData[sPlayerID[0]].stick_x = 0;
        gControllerCurrData[sPlayerID[0]].stick_y = 0;
        return;
    }
    trackX = (sAutoplayActiveTrack % 6);
    trackY = sAutoplayActiveTrack / 6;
    if (trackX == 4 && trackY == 4) {
        sAutoplayTrackStage = AUTOPLAY_TRACKS_FINISHED;
    }
    switch (sAutoplayTrackStage) {
        case AUTOPLAY_TRACKS_PREVIEW:
            if (skipCheck == TRUE) {
                sAutoplayTrackStage = AUTOPLAY_TRACKS_RESET;
            }
            if (bgload_active() == FALSE) {
                if (trackTime++ > 30) {
                    if ((gTrackSelectCursorY % 2) == 0) {
                        if (gTrackSelectCursorX < 5) {
                            gControllerCurrData[sPlayerID[0]].stick_x = 70;
                        } else {
                            gControllerCurrData[sPlayerID[0]].stick_y = -70;
                        }
                    } else {
                        if (gTrackSelectCursorX > 0) {
                            gControllerCurrData[sPlayerID[0]].stick_x = -70;
                        } else {
                            gControllerCurrData[sPlayerID[0]].stick_y = -70;
                        }
                    }
                    if (gTrackSelectCursorY == 4 && gTrackSelectCursorX == 4) {
                        sAutoplayTrackStage = AUTOPLAY_TRACKS_RESET;
                    }
                    trackTime = 0;
                }
            } else {
                trackTime = 0;
            }
            break;
        case AUTOPLAY_TRACKS_RESET:
        case AUTOPLAY_TRACKS_FINISHED:
            if (sAutoplayTrackStage == AUTOPLAY_TRACKS_FINISHED) {
                trackX = 0;
                trackY = 0;
            }
            if (gTrackSelectCursorX > trackX) {
                gControllerCurrData[sPlayerID[0]].stick_x = -70;
            } else if (gTrackSelectCursorX < trackX) {
                gControllerCurrData[sPlayerID[0]].stick_x = 70;
            } else {
                gControllerCurrData[sPlayerID[0]].stick_x = 0;
            }
            if (gTrackSelectCursorY > trackY) {
                gControllerCurrData[sPlayerID[0]].stick_y = 70;
            } else if (gTrackSelectCursorY < trackY) {
                gControllerCurrData[sPlayerID[0]].stick_y = -70;
            } else {
                gControllerCurrData[sPlayerID[0]].stick_y = 0;
            }
            if (gTrackSelectCursorX == trackX && gTrackSelectCursorY == trackY) {
                if (sAutoplayTrackStage == AUTOPLAY_TRACKS_FINISHED) {
                    gControllerCurrData[sPlayerID[i]].button |= B_BUTTON;
                    gControllerButtonsPressed[sPlayerID[i]] |= B_BUTTON;
                } else {
                    sAutoplayTrackStage = AUTOPLAY_TRACKS_PLAY;
                }
            }
            break;
        case AUTOPLAY_TRACKS_PLAY:
            switch (gMenuStage) {
                case TRACKMENU_MIRROR:
                case TRACKMENU_OPT_1:
                case TRACKMENU_CHOOSE:
                case TRACKMENU_OPT_3:
                    for (i = 0; i < playerCount; i++) {
                        gControllerCurrData[sPlayerID[i]].button |= A_BUTTON;
                        gControllerButtonsPressed[sPlayerID[i]] |= A_BUTTON;
                    }
                    break;
                case TRACKMENU_OPT_2:
                    if (gMultiplayerSelectedNumberOfRacers < 2 && trackX < 4 && playerCount < 3) {
                        gControllerCurrData[sPlayerID[0]].stick_x = 70;
                    } else {
                        gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                        gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                    }
                    break;
            }
            break;
            
    }
}

void autoplay_multiplayer(s32 playerCount) {
    Object *obj;
    Object_Racer *racer;
    s32 i;
    static u8 increasedTrack = 0;
    static u8 counting;
    static u8 ttCamCounter = 0;

    //render_printf("%d\n", gCurrentMenuId);

    if (get_game_mode() == GAMEMODE_MENU) {
        switch (gCurrentMenuId) {
            case MENU_GAME_SELECT:
                    sAutoplayTrackStage = 0;
                    sAutoplayActiveTrack = 0;
                if (sControllerFlip) {
                    if (gMenuCurIndex != 2) {
                        gControllerCurrData[sPlayerID[0]].stick_y = -70;
                    } else {
                        gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                        gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                    }
                }
                break;
            case MENU_CHARACTER_SELECT:
                autoplay_charselect_multi(playerCount);
                break;
            case MENU_TRACK_SELECT:
                increasedTrack = 0;
                counting = 0;
                autoplay_tracks(playerCount);
                break;
            case MENU_RESULTS:
            case MENU_TROPHY_RACE_ROUND:
            case MENU_TROPHY_RACE_RANKINGS:
                if ((counting++ % 60) == 0) {
                    gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                    gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                }
                if (increasedTrack == 0) {
                    sAutoplayTrackStage = AUTOPLAY_TRACKS_RESET;
                    sAutoplayActiveTrack++;
                    increasedTrack = 1;
                }
                break;
        }
    } else if (get_game_mode() == GAMEMODE_INGAME) {
        if (get_race_start_timer() == 0) {
            if (ttCamCounter++ > 60) {
                ttCamCounter = 0;
                gControllerCurrData[sPlayerID[0]].button |= R_CBUTTONS;
                gControllerButtonsPressed[sPlayerID[0]] |= R_CBUTTONS;
                for (i = 0; i < playerCount; i++) {
                    gControllerCurrData[sPlayerID[i]].button |= D_CBUTTONS;
                    gControllerButtonsPressed[sPlayerID[i]] |= D_CBUTTONS;
                }

            }
            if (level_type() & RACETYPE_CHALLENGE_BATTLE) {
                for (i = 0; i < 4; i++) {
                    obj = get_racer_object(i);
                    racer = (Object_Racer *) obj->racer;
                    if (racer->bananas > 1 && level_id() != ASSET_LEVEL_SMOKEYCASTLE) {
                        racer->bananas = 1;
                    }
                    racer->lap_times[0] += sLogicUpdateRate;
                    if (racer->lap_times[0] > (60 * 60) * 2) {
                        for (i = 0; i < 4; i++) {
                            obj = get_racer_object(i);
                            racer = (Object_Racer *) obj->racer;
                            if (level_id() != ASSET_LEVEL_SMOKEYCASTLE) {
                                racer->bananas = 0;
                            } else {
                                racer->raceFinished = TRUE;
                            }
                        }
                    }
                }
            }
            for (i = 0; i < playerCount; i++) {
                obj = get_racer_object(i);
                racer = (Object_Racer *) obj->racer;
                gAutoDrive = TRUE;
                if (i == 0) {
                    if (racer->raceFinished) {
                        if (sControllerFlip) {
                            gMenuOption = 1;
                            gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
                            gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
                        }
                    }
                }
            }
        }
    }
}

void autoplay_common(void) {
    sControllerFlip ^= 1;
    if (get_game_mode() == GAMEMODE_MENU && gCurrentMenuId == MENU_TITLE) {
        sTransform = 0;
        sCharSelectInputs = 0;
        sFileSelectInputs = 0;
        if (sControllerFlip) {
            gControllerCurrData[sPlayerID[0]].button |= A_BUTTON;
            gControllerButtonsPressed[sPlayerID[0]] |= A_BUTTON;
        } else {
            gControllerButtonsReleased[sPlayerID[0]] |= A_BUTTON;
        }
    }
}

void autoplay_inputs(void) {
    if (sBootTime < 60) {
        sBootTime++;
        return;
    }

    if (gAutoplayTest == AUTOPLAY_OFF) {
        return;
    }
    
    autoplay_common();

    switch (gAutoplayTest) {
        case AUTOPLAY_SINGLE_PLAYER:
            autoplay_single_player();
            break;
        case AUTOPLAY_TRACKS_MODE:
            break;
        case AUTOPLAY_TIME_TRIAL:
            break;
        case AUTOPLAY_OPTIONS:
            break;
        case AUTOPLAY_2_PLAYER:
            autoplay_multiplayer(2);
            break;
        case AUTOPLAY_3_PLAYER:
            autoplay_multiplayer(3);
            break;
        case AUTOPLAY_4_PLAYER:
            autoplay_multiplayer(4);
            break;
    }
    
}
