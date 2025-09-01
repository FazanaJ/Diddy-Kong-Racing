#include "src/game_ui.h"
#include "src/asset_loading.h"
#include "src/audio.h"
#include "src/audio_spatial.h"
#include "src/audiosfx.h"
#include "src/camera.h"
#include "common.h"
#include "f3ddkr.h"
#include "src/game.h"
#include "src/game_text.h"
#include "src/joypad.h"
#include "macros.h"
#include "src/math_util.h"
#include "src/menu.h"
#include "src/object_models.h"
#include "src/objects.h"
#include "PRinternal/viint.h"
#include "src/printf.h"
#include "src/racer.h"
#include "src/rcp_dkr.h"
#include "structs.h"
#include "src/textures_sprites.h"
#include "src/thread3_main.h"
#include "src/tracks.h"
#include "types.h"
#include "src/main.h"

// GLOBAL_ASM

#define HUDSCALE(x) (x * 0x4000)

extern s8 gHudToggleSettings[4];
extern s8 gPrevToggleSetting;
extern s8 gMinimapOpacity;
extern s8 gMinimapFade;
extern s8 gShowHUD;
extern s8 gMinimapXlu;
extern s8 gRaceStartShowHudStep;
extern u8 *gAssetHudElementStaleCounter;
extern HudData *gCurrentHud;
extern HudData *gPlayerHud[MAXCONTROLLERS]; // One per active player
extern s16 *gAssetHudElementIds;
extern HudElements *gAssetHudElements;
extern s32 gAssetHudElementIdsCount;
extern s32 gHUDNumPlayers;
extern s32 gHudOffsetX; // Offset value to do the slide in animation.
extern u16 gHudBounceTimer;
extern f32 gHudBounceMag;
extern u8 gHudRaceStart;
extern u8 gHudSlide;
extern u8 gHideRaceTimer;
extern u8 gNumActivePlayers;
extern u8 gWrongWayNagPrefix;
extern AudioPoint *gRaceStartSoundMask;
extern SoundHandle gHudBalloonSoundMask;
extern u16 gHudTTSoundID;
extern s32 D_80126D4C;
extern s32 D_80126D50;
extern LevelHeader *gHudLevelHeader;
extern u8 D_80126D64;
extern u8 D_80126D65;
extern u8 D_80126D66;
extern u8 gStopwatchAnimID;
extern u8 D_80126D68;
extern s8 D_80126D69;
extern s32 gWrongWayNagTimer;
extern u8 gHudAudioReset;
extern u8 gHudTimeTrialGhost;
extern s32 gHudVoiceTimer;
extern u16 gHudVoiceID;
extern DrawTexture *gHudSprites;
extern Settings *gHudSettings;
extern u8 gHudSilverCoinRace;
extern u8 gAdventurePlayerFinish;
extern u8 gAdvRaceStartedByP2;
extern u8 gMinimapOpacityTarget;
extern s32 gStopwatchErrorX;
extern s32 gStopwatchErrorY;
extern LevelHeader_70 *D_80127194;
extern HudAudio gHudAudioData[2];

typedef struct HudElementStarts {
    Vec2s pos;
    u16 scale;
    s16 hudAsset;
} HudElementStarts;

HudElementStarts gHudElementBase[HUD_ELEMENT_COUNT] = {
    {{{{53, 16}}}, HUDSCALE(1.0f), HUD_ASSET_0},
    {{{{54, 18}}}, HUDSCALE(1.0f), HUD_ASSET_3},
    {{{{-120, -68}}}, HUDSCALE(0.4f), HUD_SPRITE_WEAPONS},
    {{{{104, 16}}}, HUDSCALE(1.0f), HUD_ASSET_6},
    {{{{93, 31}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{104, 32}}}, HUDSCALE(1.0f), HUD_ASSET_4},
    {{{{114, 31}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{-3, 73}}}, HUDSCALE(0.5f), HUD_SPRITE_BANANA_ANIM},
    {{{{182, 22}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{192, 22}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{239, 16}}}, HUDSCALE(1.0f), HUD_ASSET_13},
    {{{{206, 31}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_GO_BIG},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_GET_READY},
    {{{{-200, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_FINISH},
    {{{{0}}}, HUDSCALE(1.0f), HUD_SPRITE_MAP_DOT},
    {{{{-50, 87}}}, HUDSCALE(1.0f), HUD_SPRITE_CHEQUER_FLAG},
    {{{{-50, 87}}}, HUDSCALE(2.0f), HUD_SPRITE_RETICLE},
    {{{{167, 22}}}, HUDSCALE(1.0f), HUD_ASSET_18},
    {{{{-127, 88}}}, HUDSCALE(1.0f), HUD_SPRITE_GOLD_BALLOON},
    {{{{52, 27}}}, HUDSCALE(1.0f), HUD_ASSET_18},
    {{{{67, 27}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{77, 27}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{221, 48}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS},
    {{{{188, 48}}}, HUDSCALE(1.0f), HUD_ASSET_6},
    {{{{206, 48}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS_SMALL},
    {{{{-105, -98}}}, HUDSCALE(0.4f), HUD_ASSET_20},
    {{{{-3, 73}}}, HUDSCALE(0.5f), HUD_SPRITE_BANANA_STATIC},
    {{{{-8, 97}}}, HUDSCALE(1.0f), HUD_ASSET_23},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_LAP_FINAL},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_LAP_LAP},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_LAP_2},
    {{{{25, 48}}}, HUDSCALE(1.0f), HUD_ASSET_2},
    {{{{-120, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_INDICATOR_TURN_90},
    {{{{-105, -98}}}, HUDSCALE(0.4f), HUD_ASSET_34},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_WRONG},
    {{{{0, 30}}}, HUDSCALE(1.0f), HUD_SPRITE_WAY},
    /*{{{{80, -60}}}, HUDSCALE(1.0f), HUD_SPRITE_PRO_AM},
    {{{{122, -71}}}, HUDSCALE(0.5f), HUD_SPRITE_SPEEDOMETRE_ARROW},
    {{{{247, 212}}}, HUDSCALE(0.75f), HUD_ASSET_47},
    {{{{234, 196}}}, HUDSCALE(1.0f), HUD_ASSET_48},
    {{{{232, 177}}}, HUDSCALE(1.0f), HUD_ASSET_49},
    {{{{240, 159}}}, HUDSCALE(1.0f), HUD_ASSET_50},
    {{{{256, 146}}}, HUDSCALE(1.0f), HUD_ASSET_51},
    {{{{275, 143}}}, HUDSCALE(1.0f), HUD_ASSET_52},
    {{{{246, 156}}}, HUDSCALE(1.0f), HUD_ASSET_53},*/
    {{{{27, 142}}}, HUDSCALE(0.7272f), HUD_ASSET_54},
    {{{{-200, 70}}}, HUDSCALE(1.0f), HUD_ASSET_0},
    {{{{-175, 72}}}, HUDSCALE(1.0f), HUD_ASSET_3},
    {{{{-99, -89}}}, HUDSCALE(1.0f), HUD_SPRITE_WEAPON_NUMBER},
    {{{{43, 20}}}, HUDSCALE(0.76f), HUD_SPRITE_PORTRAIT},
    {{{{40, 54}}}, HUDSCALE(1.0f), HUD_ASSET_66},
    {{{{-117, 46}}}, HUDSCALE(0.375f), HUD_SPRITE_BANANA_ANIM},
    {{{{51, 56}}}, HUDSCALE(1.0f), HUD_ASSET_28},
    {{{{60, 57}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS_SMALL},
    {{{{69, 57}}}, HUDSCALE(1.0f), HUD_ASSET_NUMBERS_SMALL},
    {{{{-160, 5}}}, HUDSCALE(1.0f), HUD_SPRITE_PLACE_1},
    {{{{260, 16}}}, HUDSCALE(1.0f), HUD_SPRITE_PORTRAIT},
    {{{{160, 5}}}, HUDSCALE(1.0f), HUD_SPRITE_PLACE_ST},
};

HudPresets2P gHudPresets2P[] = {
    { HUD_RACE_POSITION, 53, 16 },
    { HUD_RACE_POSITION_END, 54, 18 },
    { HUD_LAP_COUNT_CURRENT, 203, 19 },
    { HUD_LAP_COUNT_SEPERATOR, 211, 19 },
    { HUD_LAP_COUNT_TOTAL, 220, 19 },
    { HUD_WEAPON_DISPLAY, -120, -31 },
    { HUD_BANANA_COUNT_ICON_STATIC, 33, 22 },
    { HUD_BANANA_COUNT_NUMBER_1, 211, 19 },
    { HUD_BANANA_COUNT_NUMBER_2, 220, 19 },
    { HUD_RACE_TIME_NUMBER, 167, 19 },
    { HUD_RACE_END_FINISH, 0, 10 },
    { HUD_LAP_COUNT_FLAG, 59, 35 },
    { HUD_MAGNET_RETICLE, 59, 35 },
    { HUD_BANANA_COUNT_ICON_SPIN, 33, 22 },
    { HUD_BANANA_COUNT_SPARKLE, 30, 42 },
    { HUD_BANANA_COUNT_X, 202, 19 },
    { HUD_RACE_START_READY, 0, 10 },
    { HUD_RACE_START_GO, 0, 10 },
    { HUD_WRONGWAY_1, 0, 10 },
    { HUD_WRONGWAY_2, 0, 10 },
    { HUD_LAP_TEXT_LAP, 0, 10 },
    { HUD_LAP_TEXT_TWO, 0, 10 },
    { HUD_LAP_TEXT_FINAL, 0, 10 },
    { HUD_CHALLENGE_FINISH_POS_1, -200, 35 },
    { HUD_CHALLENGE_FINISH_POS_2, -175, 37 },
    { HUD_WEAPON_QUANTITY, -106, -45 },
    { HUD_LAP_COUNT_LABEL, 180, 19 },
    { HUD_CHALLENGE_PORTRAIT, 263, 17 },
    { HUD_TREASURE_METRE, 247, 45 },
    { HUD_EGG_CHALLENGE_ICON, 259, 49 },
    { HUD_SILVER_COIN_TALLY, 34, 71 },
    { -1, -1, -1 },
};

HudPresets4P gHudPresets4P[] = {
    { HUD_RACE_POSITION, 45, 16, 135 },
    { HUD_LAP_COUNT_CURRENT, 94, 19, 78 },
    { HUD_LAP_COUNT_SEPERATOR, 102, 19, 86 },
    { HUD_LAP_COUNT_TOTAL, 111, 19, 95 },
    { HUD_WEAPON_DISPLAY, -47, -36, 36 },
    { HUD_BANANA_COUNT_ICON_STATIC, 0, 22, -29 },
    { HUD_BANANA_COUNT_ICON_SPIN, 0, 22, -29 },
    { HUD_BANANA_COUNT_NUMBER_1, 97, 19, 68 },
    { HUD_BANANA_COUNT_NUMBER_2, 106, 19, 77 },
    { HUD_RACE_TIME_NUMBER, 55, 19, 26 },
    { HUD_RACE_START_GO, 0, 10, 0 },
    { HUD_RACE_START_READY, 0, 10, 0 },
    { HUD_RACE_END_FINISH, -160, 10, -160 },
    { HUD_LAP_COUNT_FLAG, 102, 19, 86 },
    { HUD_MAGNET_RETICLE, 102, 19, 86 },
    { HUD_BANANA_COUNT_X, 88, 19, 59 },
    { HUD_BANANA_COUNT_SPARKLE, -3, 42, -32 },
    { HUD_TREASURE_METRE, 18, 45, 120 },
    { HUD_LAP_TEXT_LAP, -22, 10, -22 },
    { HUD_LAP_TEXT_TWO, 12, 10, 16 },
    { HUD_LAP_TEXT_FINAL, 0, 10, -39 },
    { HUD_WRONGWAY_1, 0, 10, 0 },
    { HUD_WRONGWAY_2, 0, 10, 0 },
    { HUD_CHALLENGE_FINISH_POS_1, -200, 25, -40 },
    { HUD_CHALLENGE_FINISH_POS_2, -175, 27, -15 },
    { HUD_WEAPON_QUANTITY, -33, -50, 50 },
    { HUD_LAP_COUNT_LABEL, 71, 19, 55 },
    { HUD_EGG_CHALLENGE_ICON, 60, 20, 60 },
    { -1, -1, -1, -1 },
};

/**
 * Done on race start, set all the hud elements to their default values.
 * Numerous override cases for different game modes and multiplayer.
 */
void hud_init_element(void) {
    s32 i;
    s32 j;
    s32 activePlayers;
    HudPresets4P *preset4P;
    HudPresets2P *preset2P;
    s32 k;
    s32 temp;

    temp = level_type();
    if (temp == RACETYPE_UNK1 || temp == RACETYPE_HUBWORLD) {
        gHudOffsetX = 0;
        gHudRaceStart = TRUE;
    } else {
        if (gHUDNumPlayers < 2) {
            gHudOffsetX = SCREEN_WIDTH;
            gHudBounceMag = 2.0f;
        } else {
            gHudOffsetX = SCREEN_WIDTH - SCREEN_HEIGHT_HALF;
            gHudBounceMag = 2.0f;
        }
        gHudRaceStart = FALSE;
    }
    gHudBounceTimer = 0;
    gRaceStartShowHudStep = 0;
    gHideRaceTimer = 0;
    gWrongWayNagPrefix = 0;
    gHudSlide = 0;
    gMinimapOpacity = 0;
#if VERSION == VERSION_80
    gMinimapOpacityTarget = 51;
#else
    gMinimapOpacityTarget = 50;
#endif
    gMinimapFade = 0;
    gShowHUD = FALSE;
    gAdventurePlayerFinish = FALSE;
    D_80126D4C = -100;
    D_80126D50 = rand_range(120, 360);
    gAdvRaceStartedByP2 = is_race_started_by_player_two();
    gStopwatchErrorX = 55;
    gStopwatchErrorY = 179;
    gHudTimeTrialGhost = timetrial_ghost_staff() == NULL ? FALSE : TRUE;
    activePlayers = gNumActivePlayers != 3 ? gNumActivePlayers : 4;
    for (j = 0; j < activePlayers; j++) {
        gCurrentHud = gPlayerHud[j];
        for (k = 0; k < ARRAY_COUNT(gHudElementBase); k++) {
            gCurrentHud->entry[k].pos.f[0] = gHudElementBase[k].pos.x;
            gCurrentHud->entry[k].pos.f[1] = gHudElementBase[k].pos.y;
            gCurrentHud->entry[k].pos.f[2] = 0.0f;
            gCurrentHud->entry[k].scale = ((f32) gHudElementBase[k].scale) / (f32) 0x4000;
            gCurrentHud->entry[k].rotation.s[2] = 0;
            gCurrentHud->entry[k].rotation.s[1] = 0;
            gCurrentHud->entry[k].rotation.s[0] = 0;
            gCurrentHud->entry[k].spriteID = gHudElementBase[k].hudAsset;
            gCurrentHud->entry[k].spriteOffset = 0;
            gCurrentHud->entry[k].timer.minutes = 0;
            if (k == HUD_RACE_POSITION) {
                gCurrentHud->entry[k].timer.seconds = 127;
            } else if (k == HUD_CHALLENGE_FINISH_POS_1) {
                gCurrentHud->entry[k].timer.seconds = 129;
            } else {
                gCurrentHud->entry[k].timer.seconds = 0;
            }
            gCurrentHud->entry[k].timer.hundredths = 0;
            gCurrentHud->entry[k].timer.unk1D = 0;
        }
        gCurrentHud->entry[HUD_SILVER_COIN_TALLY].silverCoinTally.offsetY = 5;
        if (gHUDNumPlayers == TWO_PLAYERS) {
            for (preset2P = gHudPresets2P; preset2P->assetID != -1; preset2P++) {
                gCurrentHud->entry[preset2P->assetID].pos.x = preset2P->x;
                if ((gAssetHudElementIds[gCurrentHud->entry[preset2P->assetID].spriteID] & ASSET_MASK_TEXTURE) ==
                    ASSET_MASK_TEXTURE) {
                    if (j == PLAYER_ONE) { // Top viewport
                        gCurrentHud->entry[preset2P->assetID].pos.y = preset2P->y;
                    } else { // Bottom viewport
                        gCurrentHud->entry[preset2P->assetID].pos.y = preset2P->y + 108;
                    }
                } else if (j == PLAYER_ONE) { // Top viewport
                    gCurrentHud->entry[preset2P->assetID].pos.y = preset2P->y + 60;
                } else { // Bottom viewport
                    gCurrentHud->entry[preset2P->assetID].pos.y = preset2P->y - 48;
                }
            }
            gCurrentHud->entry[HUD_CHALLENGE_PORTRAIT].pos.x = 263.0f;
            gCurrentHud->entry[HUD_CHALLENGE_PORTRAIT].pos.y = 17.0f;
            gCurrentHud->entry[HUD_TREASURE_METRE].pos.x = 247.0f;
            gCurrentHud->entry[HUD_TREASURE_METRE].pos.y = 45.0f;
            gCurrentHud->entry[HUD_EGG_CHALLENGE_ICON].pos.x = 259.0f;
            gCurrentHud->entry[HUD_EGG_CHALLENGE_ICON].pos.y = 49.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_ICON].pos.x = 103.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_ICON].pos.y = 49.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_X].pos.x = 264.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_X].pos.y = 50.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_COUNT_1].pos.x = 265.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_COUNT_1].pos.y = 51.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_COUNT_2].pos.x = 274.0f;
            gCurrentHud->entry[HUD_BATTLE_BANANA_COUNT_2].pos.y = 51.0f;
            gCurrentHud->entry[HUD_CHALLENGE_PORTRAIT].pos.y = 17.0f;
            gCurrentHud->entry[HUD_SILVER_COIN_TALLY].silverCoinTally.offsetY = 3;
        } else if (gHUDNumPlayers == THREE_PLAYERS || gHUDNumPlayers == FOUR_PLAYERS) {
            for (preset4P = gHudPresets4P; preset4P->assetID != -1; preset4P++) {
                if ((gAssetHudElementIds[gCurrentHud->entry[preset4P->assetID].spriteID] & ASSET_MASK_TEXTURE) ==
                    ASSET_MASK_TEXTURE) {
                    if (j == PLAYER_ONE || j == PLAYER_THREE) { // Left viewports
                        gCurrentHud->entry[preset4P->assetID].pos.x = preset4P->xLeft;
                    } else { // Right viewports
                        gCurrentHud->entry[preset4P->assetID].pos.x = preset4P->xRight + 160;
                    }
                    if (j == PLAYER_ONE || j == PLAYER_TWO) { // Top viewports
                        gCurrentHud->entry[preset4P->assetID].pos.y = preset4P->y;
                    } else { // Bottom viewports
                        gCurrentHud->entry[preset4P->assetID].pos.y = preset4P->y + 108;
                    }
                } else {
                    if (j == PLAYER_ONE || j == PLAYER_THREE) { // Left viewports
                        gCurrentHud->entry[preset4P->assetID].pos.x = preset4P->xLeft - 80;
                    } else { // Right viewports
                        gCurrentHud->entry[preset4P->assetID].pos.x = preset4P->xRight + 80;
                    }
                    if (j == PLAYER_ONE || j == PLAYER_TWO) { // Top viewports
                        gCurrentHud->entry[preset4P->assetID].pos.y = preset4P->y + 60;
                    } else { // Bottom viewports
                        gCurrentHud->entry[preset4P->assetID].pos.y = preset4P->y - 48;
                    }
                }
            }
        }
        if (gHUDNumPlayers != ONE_PLAYER) {
            for (i = 0; i < HUD_ELEMENT_COUNT; i++) {
                if (gCurrentHud->entry[i].spriteID == HUD_ASSET_NUMBERS) {
                    gCurrentHud->entry[i].spriteID = HUD_ASSET_NUMBERS_SMALL;
                } else if (gCurrentHud->entry[i].spriteID == HUD_ASSET_4) {
                    gCurrentHud->entry[i].spriteID = HUD_ASSET_15;
                } else if (gCurrentHud->entry[i].spriteID == HUD_ASSET_SEPERATOR) {
                    gCurrentHud->entry[i].spriteID = HUD_ASSET_SEPERATOR_SMALL;
                } else if (gCurrentHud->entry[i].spriteID == HUD_ASSET_18) {
                    gCurrentHud->entry[i].spriteID = HUD_ASSET_28;
                } else if (gCurrentHud->entry[i].spriteID == HUD_ASSET_54) {
                    gCurrentHud->entry[i].scale *= 0.6;
                } else if (i != HUD_RACE_POSITION && i != HUD_RACE_POSITION_END && i != HUD_WEAPON_DISPLAY &&
                           i != HUD_CHALLENGE_FINISH_POS_1 && i != HUD_CHALLENGE_FINISH_POS_2 &&
                           i != HUD_LAP_COUNT_LABEL && i != HUD_CHALLENGE_PORTRAIT && i != HUD_EGG_CHALLENGE_ICON &&
                           ((i < HUD_BATTLE_BANANA_ICON || i > HUD_BATTLE_BANANA_COUNT_2) ||
                            i == HUD_BANANA_COUNT_SPARKLE || i == HUD_BANANA_COUNT_NUMBER_2)) {
                    gCurrentHud->entry[i].scale *= 0.75;
                }
            }
        }
        if (gHUDNumPlayers >= THREE_PLAYERS) {
            gCurrentHud->entry[HUD_LAP_TEXT_LAP].scale = 1.0f;
            gCurrentHud->entry[HUD_LAP_TEXT_TWO].scale = 1.0f;
            gCurrentHud->entry[HUD_LAP_TEXT_FINAL].scale = 1.0f;
            gCurrentHud->entry[HUD_RACE_START_GO].scale = 1.0f;
            gCurrentHud->entry[HUD_RACE_START_READY].scale = 1.0f;
            gCurrentHud->entry[HUD_WRONGWAY_1].scale = 1.0f;
            gCurrentHud->entry[HUD_WRONGWAY_2].scale = 1.0f;
            gCurrentHud->entry[HUD_RACE_END_FINISH].scale = 1.0f;
            gCurrentHud->entry[HUD_LAP_TEXT_LAP].spriteID = HUD_SPRITE_LAP_LAP_SMALL;
            gCurrentHud->entry[HUD_LAP_TEXT_TWO].spriteID = HUD_SPRITE_LAP_2_SMALL;
            gCurrentHud->entry[HUD_LAP_TEXT_FINAL].spriteID = HUD_SPRITE_LAP_FINAL_SMALL;
            gCurrentHud->entry[HUD_RACE_START_GO].spriteID = HUD_SPRITE_GO_SMALL;
            gCurrentHud->entry[HUD_RACE_START_READY].spriteID = HUD_SPRITE_GET_READY_SMALL;
            gCurrentHud->entry[HUD_WRONGWAY_1].spriteID = HUD_SPRITE_WRONG_SMALL;
            gCurrentHud->entry[HUD_WRONGWAY_2].spriteID = HUD_SPRITE_WAY_SMALL;
            gCurrentHud->entry[HUD_RACE_END_FINISH].spriteID = HUD_SPRITE_FINISH_SMALL;
        } else {
            gCurrentHud->entry[HUD_WEAPON_DISPLAY].weaponDisplay.prevLevel = 1;
            // while weaponDisplay is not correct union "type" we need the value to be set to be a s8 instead of u8
            gCurrentHud->entry[HUD_BANANA_COUNT_ICON_STATIC].weaponDisplay.prevLevel = 1;
        }
        if (is_in_time_trial()) {
            gCurrentHud->entry[HUD_BANANA_COUNT_ICON_SPIN].pos.x += -25.0f;
            gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_1].pos.x += -25.0f;
            gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_2].pos.x += -25.0f;
            gCurrentHud->entry[HUD_BANANA_COUNT_SPARKLE].pos.x += -25.0f;
            gCurrentHud->entry[HUD_BANANA_COUNT_ICON_STATIC].pos.x += -25.0f;
            gCurrentHud->entry[HUD_BANANA_COUNT_X].pos.x += -25.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_CURRENT].pos.x += -58.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_SEPERATOR].pos.x += -58.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_TOTAL].pos.x += -58.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_LABEL].pos.x += -58.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_FLAG].pos.x += -58.0f;
        }
        switch (level_type()) {
            case RACETYPE_BOSS:
                gCurrentHud->entry[HUD_BANANA_COUNT_ICON_SPIN].pos.x += -120.0f;
                gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_1].pos.x += -120.0f;
                gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_2].pos.x += -120.0f;
                gCurrentHud->entry[HUD_BANANA_COUNT_SPARKLE].pos.x += -120.0f;
                gCurrentHud->entry[HUD_BANANA_COUNT_ICON_STATIC].pos.x += -120.0f;
                gCurrentHud->entry[HUD_BANANA_COUNT_X].pos.x += -120.0f;
                gCurrentHud->entry[HUD_LAP_COUNT_LABEL].pos.x += 28.0f;
                gCurrentHud->entry[HUD_LAP_COUNT_CURRENT].pos.x += 28.0f;
                gCurrentHud->entry[HUD_LAP_COUNT_SEPERATOR].pos.x += 28.0f;
                gCurrentHud->entry[HUD_LAP_COUNT_TOTAL].pos.x += 28.0f;
                gCurrentHud->entry[HUD_LAP_COUNT_FLAG].pos.x += 28.0f;
                break;
            case RACETYPE_CHALLENGE_BANANAS:
                if (gNumActivePlayers >= FOUR_PLAYERS) {
                    temp = (j == PLAYER_ONE) || (j == PLAYER_THREE) ? 15 : -10;
                } else {
                    temp = gNumActivePlayers == TWO_PLAYERS ? -10 : 0;
                }
                gCurrentHud->entry[HUD_BANANA_COUNT_ICON_STATIC].pos.x += temp;
                gCurrentHud->entry[HUD_BANANA_COUNT_ICON_SPIN].pos.x += temp;
                gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_1].pos.x += temp;
                gCurrentHud->entry[HUD_BANANA_COUNT_NUMBER_2].pos.x += temp;
                gCurrentHud->entry[HUD_BANANA_COUNT_SPARKLE].pos.x += temp;
                gCurrentHud->entry[HUD_BANANA_COUNT_X].pos.x += temp;
                break;
        }
        if (is_taj_challenge()) {
            gCurrentHud->entry[HUD_LAP_COUNT_LABEL].pos.x += 28.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_CURRENT].pos.x += 28.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_SEPERATOR].pos.x += 28.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_TOTAL].pos.x += 28.0f;
            gCurrentHud->entry[HUD_LAP_COUNT_FLAG].pos.x += 28.0f;
        }
        gCurrentHud->entry[HUD_BALLOON_COUNT_ICON].scale = 1.0f;
        gCurrentHud->entry[HUD_BALLOON_COUNT_X].spriteID = HUD_ASSET_18;
        gCurrentHud->entry[HUD_BALLOON_COUNT_NUMBER_1].spriteID = 9;
        gCurrentHud->entry[HUD_BALLOON_COUNT_NUMBER_2].spriteID = 9;
    }
    if (level_type() & RACETYPE_CHALLENGE) {
        gPrevToggleSetting = gHudToggleSettings[1];
        gHudToggleSettings[1] = 0;
    } else {
        gHudToggleSettings[1] = gPrevToggleSetting;
    }
}

void hudinit_init(void) {
    s32 i;
    s32 playerCount;

    gHUDNumPlayers = cam_get_viewport_layout();
    gNumActivePlayers = cam_set_layout(gHUDNumPlayers);
    gHudSettings = get_settings();
    gHudSilverCoinRace = check_if_silver_coin_race();
    gAssetHudElementIds = (s16 *) asset_table_load(ASSET_HUD_ELEMENT_IDS);
    gAssetHudElementIdsCount = 0;

    while (gAssetHudElementIds[gAssetHudElementIdsCount] != -1) {
        gAssetHudElementIdsCount++;
    }
    gAssetHudElements = mempool_alloc_safe(gAssetHudElementIdsCount * (sizeof(void *) + 1), PP_RAM_HUD);
    // Evil pointer shenanigans to store the timer in that last byte in the struct above.
    gAssetHudElementStaleCounter = (u8 *) ((gAssetHudElementIdsCount + (s32 *) gAssetHudElements));
    for (i = 0; i < gAssetHudElementIdsCount; i++) {
        gAssetHudElementStaleCounter[i] = 0;
        gAssetHudElements->entry[i] = NULL;
    }
    if (level_type() != RACETYPE_HUBWORLD && is_time_trial_enabled() == FALSE) {
        gAssetHudElements->entry[HUD_ELEMENT_UNK_01] =
            tex_load_sprite(gAssetHudElementIds[HUD_ELEMENT_UNK_01] & 0x3FFF, 1); // ID: 86 - 0x56
        gAssetHudElements->entry[HUD_ELEMENT_UNK_17] =
            tex_load_sprite(gAssetHudElementIds[HUD_ELEMENT_UNK_17] & 0x3FFF, 1); // ID: 291 - 0x123
        gAssetHudElements->entry[HUD_ELEMENT_UNK_08] =
            tex_load_sprite(gAssetHudElementIds[HUD_ELEMENT_UNK_08] & 0x3FFF, 1); // ID: 156 - 0x9C
        gAssetHudElements->entry[HUD_ELEMENT_UNK_11] =
            tex_load_sprite(gAssetHudElementIds[HUD_ELEMENT_UNK_11] & 0x3FFF, 1); // ID: 60 - 0x3C
    }
    if (gNumActivePlayers != 3) {
        playerCount = gNumActivePlayers;
    } else {
        playerCount = 4; // Create four viewports if there are three players.
    }
    gHudSprites = mempool_alloc_safe((32 * playerCount) * sizeof(DrawTexture), PP_RAM_HUD);
    gPlayerHud[PLAYER_ONE] = mempool_alloc_safe(playerCount * sizeof(HudData), PP_RAM_HUD);
    gPlayerHud[PLAYER_TWO] = (HudData *) ((u8 *) gPlayerHud[PLAYER_ONE] + sizeof(HudData));
    gPlayerHud[PLAYER_THREE] = (HudData *) ((u8 *) gPlayerHud[PLAYER_TWO] + sizeof(HudData));
    gPlayerHud[PLAYER_FOUR] = (HudData *) ((u8 *) gPlayerHud[PLAYER_THREE] + sizeof(HudData));
    hud_init_element();
    D_80126D64 = 0;
    gWrongWayNagTimer = 0;
    gHudTTSoundID = SOUND_NONE;
    D_80126D66 = 4;
    D_80126D65 = 0;
    gStopwatchAnimID = 4;
    D_80126D69 = 1;
    D_80126D68 = 1;
    gHudAudioReset = FALSE;
    gHudVoiceID = 0;
    gHudVoiceTimer = 0;
    gRaceStartSoundMask = NULL;
    gHudBalloonSoundMask = NULL;
    gMinimapXlu = 0;
    D_80127194 = (LevelHeader_70 *) get_misc_asset(ASSET_MISC_58);
    func_8007F1E8(D_80127194);
    sndp_set_group_volume(0, AL_SNDP_GROUP_VOLUME_MAX);
    sndp_set_group_volume(2, AL_SNDP_GROUP_VOLUME_MAX);
    for (i = 0; i < 2; i++) {
        gHudAudioData[i].volume = 0;
        gHudAudioData[i].volumeRamp = 0;
        gHudAudioData[i].playerIndex = PLAYER_COMPUTER;
        if (gHudAudioData[i].soundMask) {
            sndp_stop(gHudAudioData[i].soundMask);
        }
    }
}

void hudinit_free(void) {
    s32 i;
    for (i = 0; i < gAssetHudElementIdsCount; i++) {
        if (gAssetHudElements->entry[i] != NULL) {
            if ((gAssetHudElementIds[i] & ASSET_MASK_TEXTURE) == ASSET_MASK_TEXTURE) {
                tex_free(gAssetHudElements->entry[i]);
            } else if (gAssetHudElementIds[i] & ASSET_MASK_SPRITE) {
                sprite_free((Sprite *) gAssetHudElements->entry[i]);
            } else if (gAssetHudElementIds[i] & ASSET_MASK_OBJECT) {
                free_object((Object *) gAssetHudElements->entry[i]);
            } else {
                free_3d_model((ModelInstance *) gAssetHudElements->entry[i]);
            }
            gAssetHudElements->entry[i] = NULL;
        }
    }
    mempool_free(gHudSprites);
    gHudSprites = NULL;
    mempool_free(*gPlayerHud);
    mempool_free(gAssetHudElementIds);
    gAssetHudElementIdsCount = 0;
    mempool_free(gAssetHudElements);
    gParticlePtrList_flush();
}