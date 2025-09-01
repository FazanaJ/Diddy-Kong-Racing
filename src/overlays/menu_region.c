#include "src/menu.h"
#include "src/main.h"
#include "src/fade_transition.h"
#include "src/thread3_main.h"
#include "src/camera.h"
#include "PR/os.h"
#include "PR/rcp.h"
#include "src/video.h"
#include "src/joypad.h"
#include "src/audio.h"

// GLOBAL_ASM

extern u8 gPauseSubmenu;
extern s32 gMenuDelay;
extern s32 gMenuOption;
extern Gfx *sMenuCurrDisplayList;
extern s32 gOptionBlinkTimer;
extern s16 gMenuStickX[5];
extern s16 gMenuStickY[5];
extern struct FadeTransition sMenuTransitionFadeIn;
extern s8 gControllersXAxisDirection[4];
extern s8 gControllersYAxisDirection[4];
extern s8 gDialogueSubmenu;

char *sRegionBootStrings[] = {
    "VIDEO MODE",
    "VIDEO MODE2",
    "VIDEO MODE3",

    "(Hold B on boot to get this screen again)",
    "(Hold B on boot to get this screen again)2",
    "(Hold B on boot to get this screen again)3",

    "Brazil Brazil Brazil Brazil",
    "Brazil Brazil Brazil Brazil2",
    "Brazil Brazil Brazil Brazil3",

    "Use this if PAL60 does not work right.",
    "Use this if PAL60 does not work right.2",
    "Use this if PAL60 does not work right.3",

    "Use this if your TV doesn't support 60Hz.",
    "Use this if your TV doesn't support 60Hz.2",
    "Use this if your TV doesn't support 60Hz.3",
    
    "Use this if possible.",
    "Use this if possible.2",
    "Use this if possible.3",

    "OK",
    "OK2",
    "OK3",

    "CURRENT",
    "CURRENT2",
    "CURRENT3",
};

char *sRegionLanguageStrings[] = {
    "ENGLISH",
    "GERMAN",
    "FRENCH",
};

char *sRegionValues[] = {
    "MPAL",
    "NTSC",
    "PAL50",
    "PAL60"
};

void menu_region_init(void) {
    gPauseSubmenu = 0;
    gMenuDelay = 0;
    gDialogueSubmenu = get_language();
    switch(gConfig.screenRegion) {
        case REGIONMODE_MPAL:
            gMenuOption = 0;
            break;
        default:
            gMenuOption = 1;
            break;
        case REGIONMODE_PAL50:
            gMenuOption = 2;
            break;
        case REGIONMODE_PAL60:
            gMenuOption = 3;
            break;
    }
}

s32 menu_region_loop(s32 updateRate) {
    Gfx **gfx = &sMenuCurrDisplayList;
    s32 lang;
    s32 i;
    s32 x;
    s32 alpha;
    s32 al;
    s32 inputPressed;
    s32 curVideo;
    char textBytes[16];

    lang = gDialogueSubmenu;
    
    gOptionBlinkTimer = (gOptionBlinkTimer + updateRate) & 0x3F;
    alpha = gOptionBlinkTimer * 8;
    if (alpha > 255) {
        alpha = 511 - alpha;
    }

    set_current_dialogue_background_colour(7, 0, 0, 0, 160);
    set_current_dialogue_box_coords(7, SCREEN_WIDTH_HALF - 112, SCREEN_HEIGHT_HALF - 16, SCREEN_WIDTH_HALF + 112, SCREEN_HEIGHT_HALF + 64);
    clear_dialogue_box_open_flag(7);
    dialogue_clear(7);
    render_dialogue_box(gfx, NULL, NULL, 7);

    set_text_font(ASSET_FONTS_BIGFONT);
    set_text_background_colour(0, 0, 0, 0);
    set_text_colour(0, 0, 0, 255, 128);
    draw_text(gfx, SCREEN_WIDTH_HALF + 1, 35, sRegionBootStrings[lang], ALIGN_MIDDLE_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(gfx, SCREEN_WIDTH_HALF, 32, sRegionBootStrings[lang], ALIGN_MIDDLE_CENTER);

    set_text_font(ASSET_FONTS_SMALLFONT);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(gfx, SCREEN_WIDTH_HALF + 1, SCREEN_HEIGHT - 17, sRegionBootStrings[lang + 3], ALIGN_MIDDLE_CENTER);
    draw_text(gfx, SCREEN_WIDTH_HALF + 1, SCREEN_HEIGHT - 63, sRegionBootStrings[lang + 6 + (gMenuOption * 3)], ALIGN_MIDDLE_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(gfx, SCREEN_WIDTH_HALF, SCREEN_HEIGHT - 18, sRegionBootStrings[lang + 3], ALIGN_MIDDLE_CENTER);
    draw_text(gfx, SCREEN_WIDTH_HALF, SCREEN_HEIGHT - 64, sRegionBootStrings[lang + 6 + (gMenuOption * 3)], ALIGN_MIDDLE_CENTER);

    set_text_font(ASSET_FONTS_FUNFONT);
    x = SCREEN_WIDTH_HALF - ((56 * (4 - 1)) / 2);
    for (i = 0; i < 4; i++) {
        set_text_colour(0, 0, 0, 255, 128);
        draw_text(gfx, x + 1, SCREEN_HEIGHT_HALF + 2, sRegionValues[i], ALIGN_MIDDLE_CENTER);
        if (gMenuOption == i && gPauseSubmenu == 0) {
            al = alpha;
        } else {
            al = 0;
        }
        set_text_colour(255, 255, 255, al, 255);
        draw_text(gfx, x, SCREEN_HEIGHT_HALF, sRegionValues[i], ALIGN_MIDDLE_CENTER);
        x += 56;
    }

    x = SCREEN_WIDTH_HALF - ((64 * (3 - 1)) / 2);
    for (i = 0; i < 3; i++) {
        set_text_colour(0, 0, 0, 255, 128);
        draw_text(gfx, x + 1, SCREEN_HEIGHT_HALF + 20 + 2, sRegionLanguageStrings[i], ALIGN_MIDDLE_CENTER);
        if (gDialogueSubmenu == i && gPauseSubmenu == 1) {
            al = alpha;
        } else {
            al = 0;
        }
        set_text_colour(255, 255, 255, al, 255);
        draw_text(gfx, x, SCREEN_HEIGHT_HALF + 20, sRegionLanguageStrings[i], ALIGN_MIDDLE_CENTER);
        x += 64;
    }

    switch(gConfig.screenRegion) {
        case REGIONMODE_MPAL:
            curVideo = 0;
            break;
        default:
            curVideo = 1;
            break;
        case REGIONMODE_PAL50:
            curVideo = 2;
            break;
        case REGIONMODE_PAL60:
            curVideo = 3;
            break;
    }

    sprintf(textBytes, "%s: %s", sRegionBootStrings[lang + 21], sRegionValues[curVideo]);

    set_text_colour(0, 0, 0, 255, 128);
    draw_text(gfx, SCREEN_WIDTH_HALF + 1, SCREEN_HEIGHT_HALF + 40 + 2, sRegionBootStrings[lang + 18], ALIGN_MIDDLE_CENTER);
    draw_text(gfx, SCREEN_WIDTH_HALF + 1, SCREEN_HEIGHT_HALF - 28 + 2, textBytes, ALIGN_MIDDLE_CENTER);
    if (gPauseSubmenu == 2) {
        al = alpha;
    } else {
        al = 0;
    }
    set_text_colour(255, 255, 255, al, 255);
    draw_text(gfx, SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF + 40, sRegionBootStrings[lang + 18], ALIGN_MIDDLE_CENTER);
    set_text_colour(160, 255, 160, 128, 255);
    draw_text(gfx, SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF - 28, textBytes, ALIGN_MIDDLE_CENTER);

    inputPressed = 0;
    gMenuStickX[PLAYER_MENU] = 0;
    gMenuStickY[PLAYER_MENU] = 0;
    for (i = 0; i < 4; i++) {
        gMenuStickX[PLAYER_MENU] += gControllersXAxisDirection[i];
        gMenuStickY[PLAYER_MENU] += gControllersYAxisDirection[i];
        inputPressed |= input_pressed(i);
    }

    if (gPauseSubmenu != 2) {
        if (gMenuStickX[PLAYER_MENU] > 0) {
            if (gPauseSubmenu == 0 && gMenuOption < 3) {
                sound_play(SOUND_MENU_PICK2, NULL);
                gMenuOption++;
            }
            if (gPauseSubmenu == 1 && gDialogueSubmenu < 2) {
                sound_play(SOUND_MENU_PICK2, NULL);
                gDialogueSubmenu++;
                set_language(gDialogueSubmenu);
            }
        }
        if (gMenuStickX[PLAYER_MENU] < 0 && gMenuOption > 0) {
            
            if (gPauseSubmenu == 0 && gMenuOption > 0) {
                sound_play(SOUND_MENU_PICK2, NULL);
                gMenuOption--;
            }
            if (gPauseSubmenu == 1 && gDialogueSubmenu > 0) {
                sound_play(SOUND_MENU_PICK2, NULL);
                gDialogueSubmenu--;
                set_language(gDialogueSubmenu);
            }
        }
    }
    if (gMenuStickY[PLAYER_MENU] < 0 && gPauseSubmenu < 2) {
        gPauseSubmenu++;
    }
    if (gMenuStickY[PLAYER_MENU] > 0 && gPauseSubmenu > 0) {
        gPauseSubmenu--;
    }

    if (gMenuDelay == 0 && inputPressed & A_BUTTON) {
        sound_play(SOUND_SELECT2, NULL);
        if (gPauseSubmenu == 0) {
            switch (gMenuOption) {
                case 0:
                    gConfig.screenRegion = REGIONMODE_MPAL;
                    break;
                case 1:
                    gConfig.screenRegion = REGIONMODE_NTSC;
                    break;
                case 2:
                    gConfig.screenRegion = REGIONMODE_PAL50;
                    break;
                case 3:
                    gConfig.screenRegion = REGIONMODE_PAL60;
                    break;
            }
            vi_change(SCREEN_WIDTH, SCREEN_HEIGHT);
        } else if (gPauseSubmenu == 2) {
            gMenuDelay = 1;
            audio_reinit();
            userconfig_write();
            transition_begin(&sMenuTransitionFadeIn);
        }
    }

    if (gMenuDelay) {
        gMenuDelay += updateRate;
        if (gMenuDelay >= 32) {
            gDialogueSubmenu = 0;
            menu_init(BOOT_LVL);
        }
    }

    sMenuCurrDisplayList = *gfx;

    return MENU_RESULT_CONTINUE;
}
