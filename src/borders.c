#include "borders.h"

#include "camera.h"
#include "f3ddkr.h"
#include "game.h"
#include "game_ui.h"
#include "macros.h"
#include "structs.h"
#include "types.h"
#include "video.h"
#include "main.h"
#include "tracks.h"

void (*gDividerDraw)(Gfx **dList);
void (*gDividerClear)(Gfx **dList);

void divider_draw(Gfx **dList) {
    if (cam_get_viewport_layout() == VIEWPORT_LAYOUT_1_PLAYER) {
        return;
    }
    if (gDividerDraw == NULL) {
        overlay_load(OVERLAY_BORDERS);

        gDividerDraw = overlay_symbol(OVERLAY_BORDERS, "dividerovl_draw");
    }

    (*gDividerDraw)(dList);
}

void divider_clear_coverage(Gfx **dList) {
    if (cam_get_viewport_layout() == VIEWPORT_LAYOUT_1_PLAYER) {
        return;
    }
    if (gDividerClear == NULL) {
        gDividerClear = overlay_symbol(OVERLAY_BORDERS, "dividerovl_clear");
    }

    (*gDividerClear)(dList);
}

void divider_free(void) {
    overlay_free(OVERLAY_BORDERS);
    gDividerDraw = NULL;
    gDividerClear = NULL;
}