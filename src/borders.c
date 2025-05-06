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
void divider_draw(Gfx **dList) {
    u32 width, height;
    LevelHeader *levelHeader;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 heightHalf;
    s32 widthHalf;

    width = fb_size();
    height = GET_VIDEO_HEIGHT(width);
    width = GET_VIDEO_WIDTH(width);
    heightHalf = height / 2;
    widthHalf = width / 2;
    width--;
    height--;
    gDPSetCycleType((*dList)++, G_CYC_FILL);
    gDPSetFillColor((*dList)++, GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)); // Black fill color
    switch (get_viewport_count()) {
        case VIEWPORTS_COUNT_3_PLAYERS:
            levelHeader = get_current_level_header();
            // Draw black square in the bottom-right corner.
            if (hud_setting() || levelHeader->race_type & RACETYPE_CHALLENGE) {
                gDPFillRectangle((*dList)++, widthHalf + 1, heightHalf + 1, width, height);
            }
            // There is no break statement here. This is intentional.
        case VIEWPORTS_COUNT_4_PLAYERS:
            gDPFillRectangle((*dList)++, widthHalf - 1, 0, widthHalf, height);
            // Fallthrough
        case VIEWPORTS_COUNT_2_PLAYERS:
            x1 = 0;
            y1 = heightHalf - 1;
            x2 = width;
            y2 = heightHalf + 1;
            break;
    }
    gDPFillRectangle((*dList)++, x1, y1, x2, y2 - 1);
}

/**
 * Draws the multiplayer borders again, but fully invisible using the XLU render mode.
 * The effect here is that it will strip coverage for anything beneath, eliminating pixel bleed.
 */
void divider_clear_coverage(Gfx **dList) {
    u32 screenWidth;
    u32 screenHeight;
    u32 height;
    u32 width;
    u32 tempX;
    u32 tempY;

    width = fb_size();
    height = GET_VIDEO_HEIGHT(width);
    width = GET_VIDEO_WIDTH(width);
    height = (screenHeight / 128) << 1 << 1;
    width = (screenWidth / 256) << 1 << 1;
    gDPSetCycleType((*dList)++, G_CYC_1CYCLE);
    gDPSetCombineMode((*dList)++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode((*dList)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetPrimColor((*dList)++, 0, 0, 0, 0, 0, 0);
    switch (get_viewport_count()) {
        case VIEWPORTS_COUNT_3_PLAYERS:
        case VIEWPORTS_COUNT_4_PLAYERS:
            tempX = (screenWidth / 2) - 2;
            gDPFillRectangle((*dList)++, tempX, 0, tempX + width, screenHeight);
            // Fallthrough
        case VIEWPORTS_COUNT_2_PLAYERS:
            tempY = (screenHeight / 2) - 2;
            gDPFillRectangle((*dList)++, 0, tempY, screenWidth, tempY + height);
            break;
    }
}
