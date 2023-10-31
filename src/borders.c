/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80077050 */

#include "borders.h"

#include "camera.h"
#include "types.h"
#include "macros.h"
#include "structs.h"
#include "f3ddkr.h"
#include "video.h"
#include "game.h"
#include "game_ui.h"

/**
 * Renders the black borders that separate each viewport during multiplayer.
 * 2 player has a single horizontal line, while 3 and 4 player splits the screen into quadrants.
 * 3 player will completely fill in where player 4 would normally be.
 */
void render_borders_for_multiplayer(Gfx **dlist) {
    u32 width, height;
    LevelHeader *levelHeader;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 heightHalf;
    s32 widthHalf;

    width = gScreenWidth;
    height = gScreenHeight;
    heightHalf = height / 2;
    widthHalf = width / 2;
    gDPSetCycleType((*dlist)++, G_CYC_FILL);
    gDPSetFillColor((*dlist)++, GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)); // Black fill color
    switch (gNumberOfViewports) {
        case VIEWPORTS_COUNT_3_PLAYERS:
            levelHeader = gCurrentLevelHeader;
            // Draw black square in the bottom-right corner.
            if (gHudToggleSettings[gHUDNumPlayers] || levelHeader->race_type & RACETYPE_CHALLENGE) {
                gDPFillRectangle((*dlist)++, widthHalf + 2, heightHalf + 2, width, height);
            }
            // There is no break statement here. This is intentional.
        case VIEWPORTS_COUNT_4_PLAYERS:
            gDPFillRectangle((*dlist)++, widthHalf - 2 , 0, widthHalf + 2, height);
            // Fallthrough
        case VIEWPORTS_COUNT_2_PLAYERS:
            x1 = 0;
            y1 = heightHalf - 2;
            x2 = width;
            y2 = heightHalf + 2;
            break;
    }
    gDPFillRectangle((*dlist)++, x1, y1, x2, y2);
}
