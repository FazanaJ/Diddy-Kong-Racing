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

// GLOBAL_ASM

extern s8 gSetupVideo;

s32 sBootDelayTimer = 0;

/**
 * Give the player 8 frames to enter the CPak menu with start, then load the intro sequence.
 */
void mode_intro(s32 updateRate) {
    s32 i;
    s32 buttonInputs = 0;
    s32 menuID;
    s32 sceneID;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        buttonInputs |= input_held(i);
    }
    if (buttonInputs & START_BUTTON) {
        gShowControllerPakMenu = TRUE;
    }
    
    if (buttonInputs & B_BUTTON) {
        gSetupVideo = TRUE;
    }
#ifndef SKIP_INTRO
    sBootDelayTimer++;
#else
    sBootDelayTimer++;
    //sBootDelayTimer = 8;
#endif
#if EXPANSION_PAK_SUPPORT == 2
    if (gExpansionPak == FALSE) {
        load_menu_with_level_background(MENU_EXPANSION_ERROR, ASSET_LEVEL_OPTIONSBACKGROUND, 0);
        return;
    }
#endif
    if (sBootDelayTimer >= 8) {
        if (gSetupVideo) {
            menuID = MENU_REGION;
            sceneID = 0;
        } else {
            menuID = BOOT_LVL;
            sceneID = 2;
        }
        load_menu_with_level_background(menuID, ASSET_LEVEL_OPTIONSBACKGROUND, sceneID);
    }
}
