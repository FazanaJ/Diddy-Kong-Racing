#include "camera.h"
#include "audio.h"
#include "game.h"
#include "libultra/src/libc/rmonPrintf.h"
#include "math_util.h"
#include "menu.h"
#include "objects.h"
#include "PRinternal/piint.h"
#include "PRinternal/viint.h"
#include "main.h"
#include "textures_sprites.h"
#include "tracks.h"
#include "video.h"
#include "weather.h"

#define CAMERA_MODEL_STACK_SIZE 5

/************ .data ************/

s8 gAntiPiracyViewport = FALSE;

// x1, y1, x2, y2
// posX, posY, width, height
// scissorX1, scissorY1, scissorX2, scissorY2
// flags
#define DEFAULT_VIEWPORT                                                                                         \
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, \
        SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 0

#define SCISSOR_INTERLACE G_SC_NON_INTERLACE

ScreenViewport gScreenViewports[4] = {
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
};

u32 gViewportWithBG = FALSE;

Vertex gVehiclePartVertex = { 0, 0, 0, 255, 255, 255, 255 };

// The viewport z-range below is half of the max (511)
#define G_HALFZ (G_MAXZ / 2) /* 9 bits of integer screen-Z precision */

// RSP Viewports
Vp gViewportStack[20] = {
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } }, { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
};

ObjectTransform D_800DD288 = {
    { { { 0, 0, 0 } } },
    { 0 },
    1.0f,
    { { { { 0.0f, 0.0f, -281.0f } } } },
};

ObjectTransform D_800DD2A0 = {
    { { { 0, 0, 0 } } },
    { 0 },
    1.0f,
    { { { { 0.0f, 0.0f, 0.0f } } } },
};

MtxF gOrthoMatrixF = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 160.0f },
};

u8 gCameraZoomLevels[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

/*******************************/

/************ .bss ************/

f32 gSpriteWidth;
Camera gCameras[8];
s32 gViewportLayout;
s32 gActiveCameraID;
s32 gNumCameras;
ObjectTransform gCameraTransform;
s32 gMtxOriginID;
s32 gSpriteAnimMode;
f32 gCurCamFOV;
s8 gCutsceneCameraActive;
s32 gNoCamShake;
s32 gModelMatrixStackPos;
s32 gCameraMatrixPos;
f32 gCameraRelPosStackX[CAMERA_MODEL_STACK_SIZE + 1];
f32 gCameraRelPosStackY[CAMERA_MODEL_STACK_SIZE + 1];
f32 gCameraRelPosStackZ[CAMERA_MODEL_STACK_SIZE];
u16 perspNorm;
MtxF *gModelMatrixF[CAMERA_MODEL_STACK_SIZE + 1];
Mtx *gModelMatrix[CAMERA_MODEL_STACK_SIZE + 1];
f32 D_80120DA0[CAMERA_MODEL_STACK_SIZE * 16];
MtxF gPerspectiveMatrixF;
MtxF gViewProjMatrixF;
MtxF gViewMatrixF;
MtxF gInverseViewMatrixF;
Mtx gPerspectiveMatrix;
MtxF gCurrentModelMatrixF;
MtxF gCurrentMVPMatrixF;

/******************************/

void cam_persp_init(void) {
    guPerspectiveF(gPerspectiveMatrixF, &perspNorm, CAMERA_DEFAULT_FOV, gVideoAspectRatio, CAMERA_NEAR, CAMERA_FAR,
                   CAMERA_SCALE);
    mtxf_to_mtx(&gPerspectiveMatrixF, &gPerspectiveMatrix);
    gCurCamFOV = CAMERA_DEFAULT_FOV;
    gSpriteWidth = ((4.0f / 3.0f) / gVideoAspectRatio);
}

/**
 * Official Name: camInit
 */
void cam_init(void) {
    s32 i;
    s32 j;

    // clang-format off
    for (i = 0; i < 5; i++) { gModelMatrixF[i] = (MtxF*)&D_80120DA0[i << 4]; }
    // clang-format on

    for (j = 0; j < 8; j++) {
        gActiveCameraID = j;
        camera_reset(200, 200, 200, 0, 0, 180);
    }

    gCutsceneCameraActive = FALSE;
    gActiveCameraID = 0;
    gModelMatrixStackPos = 0;
    gCameraMatrixPos = 0;
    gViewportLayout = 0;
    gSpriteAnimMode = SPRITE_ANIM_NORMALIZED;
    gNoCamShake = FALSE;
    gAntiPiracyViewport = FALSE;
    cam_persp_init();

}

void cam_set_zoom(s32 cameraID, s32 zoomLevel) {
    if (cameraID >= 0 && cameraID <= 3) {
        gCameraZoomLevels[cameraID] = zoomLevel;
        gCameras[cameraID].zoom = zoomLevel;
    }
}

/**
 * Disable camera shake.
 */
void cam_shake_off(void) {
    gNoCamShake = TRUE;
}

/**
 * Enable camera shake. Camera wiggles up and down with it enabled.
 */
void cam_shake_on(void) {
    gNoCamShake = FALSE;
}

/**
 * Set the FoV of the viewspace, then recalculate the perspective matrix.
 * Official Name: camSetFOV
 */
void cam_set_fov(f32 camFieldOfView) {
    if (CAMERA_MIN_FOV < camFieldOfView && camFieldOfView < CAMERA_MAX_FOV && camFieldOfView != gCurCamFOV) {
        gCurCamFOV = camFieldOfView;
        guPerspectiveF(gPerspectiveMatrixF, &perspNorm, camFieldOfView, gVideoAspectRatio, CAMERA_NEAR, CAMERA_FAR,
                       CAMERA_SCALE);
        mtxf_to_mtx(&gPerspectiveMatrixF, &gPerspectiveMatrix);
    }
}

/**
 * Returns the number of active viewports.
 */
s32 cam_get_viewport_layout(void) {
    return gViewportLayout;
}

/**
 * Return the index of the active view.
 * 0-3 is players 1-4, and 4-7 is the same, but with 4 added on for cutscenes.
 * Official Name: camGetMode
 */
s32 get_current_viewport(void) {
    return gActiveCameraID;
}

/**
 * Initialises the camera object for the tracks menu.
 */
void camera_init_tracks_menu(Gfx **dList, Mtx **mtxS) {
    Camera *cam;
    s16 angleY;
    s16 angleX;
    s16 angleZ;
    s16 sp24;
    f32 posX;
    f32 posY;
    f32 posZ;

    cam_set_layout(VIEWPORT_LAYOUT_1_PLAYER);
    set_active_camera(0);
    cam = cam_get_active_camera();
    angleY = cam->trans.rotation.y_rotation;
    angleX = cam->trans.rotation.x_rotation;
    angleZ = cam->trans.rotation.z_rotation;
    posX = cam->trans.x_position;
    posY = cam->trans.y_position;
    posZ = cam->trans.z_position;
    sp24 = cam->pitch;
    cam->trans.rotation.z_rotation = 0;
    cam->trans.rotation.x_rotation = 0;
    cam->trans.rotation.y_rotation = -0x8000;
    cam->pitch = 0;
    cam->trans.x_position = 0.0f;
    cam->trans.y_position = 0.0f;
    cam->trans.z_position = 0.0f;
    update_envmap_position(0.0f, 0.0f, -1.0f);
    viewport_main(dList, mtxS);
    cam->pitch = sp24;
    cam->trans.rotation.y_rotation = angleY;
    cam->trans.rotation.x_rotation = angleX;
    cam->trans.rotation.z_rotation = angleZ;
    cam->trans.x_position = posX;
    cam->trans.y_position = posY;
    cam->trans.z_position = posZ;
}

/**
 * Compare the coordinates passed through to the active camera and return the distance between them.
 */
f32 get_distance_to_active_camera(f32 xPos, f32 yPos, f32 zPos) {
    s32 index;
    f32 dx, dz, dy;

    index = gActiveCameraID;

    if (gCutsceneCameraActive) {
        index += 4;
    }

    dz = zPos - gCameras[index].trans.z_position;
    dx = xPos - gCameras[index].trans.x_position;
    dy = yPos - gCameras[index].trans.y_position;
    return ((dz * dz) + ((dx * dx) + (dy * dy)));
}

/**
 * Sets the position and angle of the active camera.
 * Also sets the other properties of the camera to a default.
 */
void camera_reset(s32 xPos, s32 yPos, s32 zPos, s32 angleZ, s32 angleX, s32 angleY) {
    gCameras[gActiveCameraID].trans.rotation.z_rotation = angleZ * (0x7FFF / 180);
    gCameras[gActiveCameraID].trans.x_position = xPos;
    gCameras[gActiveCameraID].trans.y_position = yPos;
    gCameras[gActiveCameraID].trans.z_position = zPos;
    gCameras[gActiveCameraID].trans.rotation.x_rotation = angleX * (0x7FFF / 180);
    gCameras[gActiveCameraID].pitch = 0;
    gCameras[gActiveCameraID].x_velocity = 0.0f;
    gCameras[gActiveCameraID].y_velocity = 0.0f;
    gCameras[gActiveCameraID].z_velocity = 0.0f;
    gCameras[gActiveCameraID].shakeMagnitude = 0.0f;
    gCameras[gActiveCameraID].boomLength = 160.0f;
    gCameras[gActiveCameraID].trans.rotation.y_rotation = angleY * (0x7FFF / 180);
    gCameras[gActiveCameraID].zoom = gCameraZoomLevels[gActiveCameraID];
}

/**
 * Write directly to the second set of object stack indeces.
 * The first 4 are reserved for the 4 player viewports, so the misc views, used in the title screen
 * and course previews instead use the next 4.
 */
void write_to_object_render_stack(s32 stackPos, f32 xPos, f32 yPos, f32 zPos, s16 arg4, s16 arg5, s16 arg6) {
    stackPos += 4;
    gCameras[stackPos].pitch = 0;
    gCameras[stackPos].trans.x_position = xPos;
    gCameras[stackPos].trans.y_position = yPos;
    gCameras[stackPos].trans.z_position = zPos;
    gCameras[stackPos].trans.rotation.y_rotation = arg4;
    gCameras[stackPos].trans.rotation.x_rotation = arg5;
    gCameras[stackPos].trans.rotation.z_rotation = arg6;
    gCameras[stackPos].cameraSegmentID = get_level_segment_index_from_position(xPos, yPos, zPos);
    gCutsceneCameraActive = TRUE;
}

/**
 * Check if the misc camera view is active.
 * Official name: camIsUserView
 */
s8 check_if_showing_cutscene_camera(void) {
    return gCutsceneCameraActive;
}

/**
 * Disable the cutscene camera, returning it to the conventional mode.
 */
void disable_cutscene_camera(void) {
    gCutsceneCameraActive = FALSE;
}

/**
 * Sets the current layout and returns the number of active cameras for that layout.
 * The layoutID argument must be from the ViewportCount enumeration.
 */
s32 cam_set_layout(s32 layoutID) {
    if (layoutID >= VIEWPORT_LAYOUT_1_PLAYER && layoutID <= VIEWPORT_LAYOUT_4_PLAYERS) {
        gViewportLayout = layoutID;
    } else {
        gViewportLayout = VIEWPORT_LAYOUT_1_PLAYER;
    }
    switch (gViewportLayout) {
        case VIEWPORT_LAYOUT_1_PLAYER:
            gNumCameras = 1;
            break;
        case VIEWPORT_LAYOUT_2_PLAYERS:
            gNumCameras = 2;
            break;
        case VIEWPORT_LAYOUT_3_PLAYERS:
            gNumCameras = 3;
            break;
        case VIEWPORT_LAYOUT_4_PLAYERS:
            gNumCameras = 4;
            break;
    }
    if (gActiveCameraID >= gNumCameras) {
        gActiveCameraID = 0;
    }
    return gNumCameras;
}

/**
 * Sets the active viewport ID to the passed number.
 * If it's not within 1-4, then it's set to 0.
 * Official name: camSetView
 */
void set_active_camera(s32 num) {
    if (num >= 0 && num < 4) {
        gActiveCameraID = num;
    } else {
        gActiveCameraID = 0;
    }
}

/**
 * Takes the size of each view frame and writes them to the viewport stack, using values compatable with the RSP.
 * Only does this if extended backgrounds are enabled.
 */
void copy_viewports_to_stack(void) {
    s32 width;
    s32 height;
    s32 port;
    s32 yPos;
    s32 xPos;
    s32 i;

    gViewportWithBG = 1 - gViewportWithBG;
    for (i = 0; i < 4; i++) {
        if (gScreenViewports[i].flags & VIEWPORT_UNK_04) {
            gScreenViewports[i].flags &= ~VIEWPORT_EXTRA_BG;
        } else if (gScreenViewports[i].flags & VIEWPORT_UNK_02) {
            gScreenViewports[i].flags |= VIEWPORT_EXTRA_BG;
        }
        gScreenViewports[i].flags &= ~(VIEWPORT_UNK_02 | VIEWPORT_UNK_04);

        if (gScreenViewports[i].flags & VIEWPORT_EXTRA_BG) {
            if (!(gScreenViewports[i].flags & VIEWPORT_X_CUSTOM)) {
                xPos = (((gScreenViewports[i].x2 - gScreenViewports[i].x1) + 1) << 1) + (gScreenViewports[i].x1 * 4);
            } else {
                xPos = gScreenViewports[i].posX;
                xPos *= 4;
            }
            if (!(gScreenViewports[i].flags & VIEWPORT_Y_CUSTOM)) {
                yPos = (((gScreenViewports[i].y2 - gScreenViewports[i].y1 + 1)) << 1) + (gScreenViewports[i].y1 * 4);
            } else {
                yPos = gScreenViewports[i].posY;
                yPos *= 4;
            }
            if (!(gScreenViewports[i].flags & VIEWPORT_WIDTH_CUSTOM)) {
                width = gScreenViewports[i].x2 - gScreenViewports[i].x1;
                width += 1;
                width *= 2;
            } else {
                width = gScreenViewports[i].width;
                width *= 2;
            }
            if (!(gScreenViewports[i].flags & VIEWPORT_HEIGHT_CUSTOM)) {
                height = (gScreenViewports[i].y2 - gScreenViewports[i].y1) + 1;
                height *= 2;
            } else {
                height = gScreenViewports[i].height;
                height *= 2;
            }
            port = i + (gViewportWithBG * 5);
            port += 10;
            if (get_filtered_cheats() & CHEAT_MIRRORED_TRACKS) {
                if (0) {} // Fakematch
                width = -width;
            }
            gViewportStack[port].vp.vtrans[0] = xPos;
            gViewportStack[port].vp.vtrans[1] = yPos;
            gViewportStack[port].vp.vscale[0] = width;
            gViewportStack[port].vp.vscale[1] = height;
        }
    }
}

void camEnableUserView(s32 viewPortIndex, s32 arg1) {
    if (arg1) {
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_EXTRA_BG;
    } else {
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_UNK_02;
    }
    gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_UNK_04;
}

void camDisableUserView(s32 viewPortIndex, s32 arg1) {
    if (arg1) {
        gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_EXTRA_BG;
    } else {
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_UNK_04;
    }
    gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_UNK_02;
}

/**
 * Return's the current viewport's flag status for extended backgrounds.
 * Required to draw some extra things used in menus.
 */
s32 check_viewport_background_flag(s32 viewPortIndex) {
    return gScreenViewports[viewPortIndex].flags & VIEWPORT_EXTRA_BG;
}

/**
 * Sets the intended viewport to the size passed through by arguments.
 * Official Name: camSetUserView
 */
void viewport_menu_set(s32 viewPortIndex, s32 x1, s32 y1, s32 x2, s32 y2) {
    s32 widthAndHeight, width, height;
    s32 temp;

    widthAndHeight = fb_size();
    height = GET_VIDEO_HEIGHT(widthAndHeight) & 0xFFFF;
    width = GET_VIDEO_WIDTH(widthAndHeight);

    if (x2 < x1) {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y2 < y1) {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (x1 >= width || x2 < 0 || y1 >= height || y2 < 0) {
        gScreenViewports[viewPortIndex].scissorX1 = 0;
        gScreenViewports[viewPortIndex].scissorY1 = 0;
        gScreenViewports[viewPortIndex].scissorX2 = 0;
        gScreenViewports[viewPortIndex].scissorY2 = 0;
    } else {
        if (x1 < 0) {
            gScreenViewports[viewPortIndex].scissorX1 = 0;
        } else {
            gScreenViewports[viewPortIndex].scissorX1 = x1;
        }
        if (y1 < 0) {
            gScreenViewports[viewPortIndex].scissorY1 = 0;
        } else {
            gScreenViewports[viewPortIndex].scissorY1 = y1;
        }
        if (x2 >= width) {
            gScreenViewports[viewPortIndex].scissorX2 = width - 1;
        } else {
            gScreenViewports[viewPortIndex].scissorX2 = x2;
        }
        if (y2 >= height) {
            gScreenViewports[viewPortIndex].scissorY2 = height - 1;
        } else {
            gScreenViewports[viewPortIndex].scissorY2 = y2;
        }
    }
    gScreenViewports[viewPortIndex].y1 = y1;
    gScreenViewports[viewPortIndex].x1 = x1;
    gScreenViewports[viewPortIndex].x2 = x2;
    gScreenViewports[viewPortIndex].y2 = y2;
}

/**
 * Set the selected viewport's coordinate offsets and view size.
 * If you pass VIEWPORT_AUTO through, then the property will be automatically set when the game creates the viewports.
 */
void set_viewport_properties(s32 viewPortIndex, s32 posX, s32 posY, s32 width, s32 height) {
    if (posX != VIEWPORT_AUTO) {
        gScreenViewports[viewPortIndex].posX = posX;
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_X_CUSTOM;
    } else {
        gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_X_CUSTOM;
    }
    if (posY != VIEWPORT_AUTO) {
        //!@bug Viewport Y writes to the X value. Luckily, all cases this function is called use VIEWPORT_AUTO,
        // so this bug doesn't happen in practice.
        gScreenViewports[viewPortIndex].posY = posY;
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_Y_CUSTOM;
    } else {
        gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_Y_CUSTOM;
    }
    if (width != VIEWPORT_AUTO) {
        gScreenViewports[viewPortIndex].width = width;
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_WIDTH_CUSTOM;
    } else {
        gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_WIDTH_CUSTOM;
    }
    if (height != VIEWPORT_AUTO) {
        gScreenViewports[viewPortIndex].height = height;
        gScreenViewports[viewPortIndex].flags |= VIEWPORT_HEIGHT_CUSTOM;
    } else {
        gScreenViewports[viewPortIndex].flags &= ~VIEWPORT_HEIGHT_CUSTOM;
    }
}

/**
 * Sets the passed values to be equal to the selected viewports scissor size, before drawing the background elements.
 * Usually, this is the same size as the viewport's size.
 * Official name: camGetVisibleUserView
 */
s32 copy_viewport_background_size_to_coords(s32 viewPortIndex, s32 *x1, s32 *y1, s32 *x2, s32 *y2) {
    // gDPFillRectangle values
    *x1 = gScreenViewports[viewPortIndex].scissorX1;
    *x2 = gScreenViewports[viewPortIndex].scissorX2;
    *y1 = gScreenViewports[viewPortIndex].scissorY1;
    *y2 = gScreenViewports[viewPortIndex].scissorY2;
    if ((*x1 | *x2 | *y1 | *y2) == 0) {
        return 0;
    }
    return 1;
}

/**
 * Sets the passed values to the coordinate size of the passed viewport.
 * Official name: camGetUserView
 */
void copy_viewport_frame_size_to_coords(s32 viewPortIndex, s32 *x1, s32 *y1, s32 *x2, s32 *y2) {
    *x1 = gScreenViewports[viewPortIndex].x1;
    *y1 = gScreenViewports[viewPortIndex].y1;
    *x2 = gScreenViewports[viewPortIndex].x2;
    *y2 = gScreenViewports[viewPortIndex].y2;
}

void viewport_main(Gfx **dlist, Mtx **mats) {
    s32 videoHeight;
    s32 videoWidth;
    s32 savedCameraID;
    s32 originalCameraID;
    s32 tempCameraID;
    s32 viewports;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 pos[2];
    s32 size[2];

    originalCameraID = gActiveCameraID;
    savedCameraID = gActiveCameraID;

    if (is_player_two_in_control() && gViewportLayout == VIEWPORT_LAYOUT_1_PLAYER) {
        gActiveCameraID = 1;
        savedCameraID = 0;
    }
    videoHeight = SCREEN_HEIGHT;
    videoWidth = SCREEN_WIDTH;
    if (gScreenViewports[savedCameraID].flags & VIEWPORT_EXTRA_BG) {
        tempCameraID = gActiveCameraID;
        gActiveCameraID = savedCameraID;
        gDPSetScissor((*dlist)++, G_SC_NON_INTERLACE, gScreenViewports[gActiveCameraID].scissorX1,
                      gScreenViewports[gActiveCameraID].scissorY1, gScreenViewports[gActiveCameraID].scissorX2,
                      gScreenViewports[gActiveCameraID].scissorY2);
        viewport_rsp_set(dlist, 0, 0, 0, 0);
        gActiveCameraID = tempCameraID;
        if (mats != NULL) {
            func_80067D3C(dlist, mats);
        }
        gActiveCameraID = originalCameraID;
        return;
    }

    viewports = gViewportLayout;
    if (viewports == VIEWPORT_LAYOUT_3_PLAYERS) {
        viewports = VIEWPORT_LAYOUT_4_PLAYERS;
    }

    pos[0] = videoWidth / 2;
    pos[1] = videoHeight / 2;
    size[0] = pos[0];
    size[1] = pos[1];

    // Cursed usage of the pos vars, but that's to save redoing width and height divide by 2.
    switch (viewports) {
        default:
            x1 = 0;
            y1 = 0;
            x2 = videoWidth;
            y2 = videoHeight;
            break;
        case VIEWPORT_LAYOUT_2_PLAYERS:
            x1 = 0;
            x2 = videoWidth;
            if (gActiveCameraID == 0) {
                y1 = 0;
                y2 = pos[1] - 1;
                pos[1] /= 2;
            } else {
                y1 = pos[1] + 1;
                y2 = videoHeight;
                pos[1] += size[1] / 2;
            }
            break;
        case VIEWPORT_LAYOUT_4_PLAYERS:
            size[0] /= 2;
            size[1] /= 2;
            switch (gActiveCameraID) {
                default:
                    x1 = 0;
                    y1 = 0;
                    x2 = pos[0] - 1;
                    y2 = pos[1] - 1;
                    pos[0] /= 2;
                    pos[1] /= 2;
                    break;
                case 1:
                    x1 = pos[0] + 1;
                    y1 = 0;
                    x2 = videoWidth;
                    y2 = pos[1] - 1;
                    pos[0] += size[0];
                    pos[1] /= 2;
                    break;
                case 2:
                    x1 = 0;
                    y1 = pos[1] + 1;
                    x2 = pos[0] - 1;
                    y2 = videoHeight;
                    pos[0] /= 2;
                    pos[1] += size[1];
                    break;
                case 3:
                    x1 = pos[0] + 1;
                    y1 = pos[1] + 1;
                    x2 = videoWidth;
                    y2 = videoHeight;
                    pos[0] += size[0];
                    pos[1] += size[1];
                    break;
            }
            break;
    }
    gDPSetScissor((*dlist)++, G_SC_NON_INTERLACE, x1, y1, x2, y2);

    viewport_rsp_set(dlist, size[0], size[1], pos[0], pos[1]);
    if (mats != NULL) {
        func_80067D3C(dlist, mats);
    }
    gActiveCameraID = originalCameraID;
}

/**
 * Takes the size of the screen as depicted by the active menu viewport, then sets the RDP scissor to match it.
 * Official Name: camSetScissor
 */
void viewport_scissor(Gfx **dList) {
    s32 size;
    s32 lrx;
    s32 lry;
    s32 ulx;
    s32 uly;
    s32 numViewports;
    s32 temp;
    s32 temp2;
    s32 temp3;
    s32 temp4;
    s32 width;
    s32 height;

    size = fb_size();
    height = (u16) GET_VIDEO_HEIGHT(size);
    width = (u16) size;
    numViewports = gViewportLayout;

    if (numViewports != VIEWPORT_LAYOUT_1_PLAYER) {
        if (numViewports == VIEWPORT_LAYOUT_3_PLAYERS) {
            numViewports = VIEWPORT_LAYOUT_4_PLAYERS;
        }
        lrx = ulx = 0;
        lry = uly = 0;
        // clang-format off
        lrx += width;\
        lry += height;
        // clang-format on
        temp = lry >> 7;
        temp2 = lrx >> 8;
        temp4 = lrx >> 1;
        temp3 = lry >> 1;
        switch (numViewports) {
            case 1:
                switch (gActiveCameraID) {
                    case 0:
                        lry = temp3 - temp;
                        break;
                    default:
                        uly = temp3 + temp;
                        lry -= temp;
                        break;
                }
                break;
            case 2:
                switch (gActiveCameraID) {
                    case 0:
                        lrx = temp4 - temp2;
                        break;
                    default:
                        ulx = temp4 + temp2;
                        lrx -= temp2;
                        break;
                }
                break;
            case 3:
                // clang-format off
                switch (gActiveCameraID) {
                    case 0:
                        lrx = temp4 - temp2;\
                        lry = temp3 - temp;
                        break;
                    case 1:
                        ulx = temp4 + temp2;
                        lrx -= temp2;
                        lry = temp3 - temp;
                        break;
                    case 2:
                        uly = temp3 + temp;
                        lrx = temp4 - temp2;\
                        lry -= temp;
                        break;
                    case 3:
                        ulx = temp4 + temp2;\
                        uly = temp3 + temp;
                        lrx -= temp2;\
                        lry -= temp;
                        break;
                }
                // clang-format on
                break;
        }
        gDPSetScissor((*dList)++, 0, ulx, uly, lrx, lry);
    } else {
        gDPSetScissor((*dList)++, 0, 0, 0, width, height);
    }
}

// Official Name: camGetPlayerProjMtx / camSetProjMtx - ??
void func_80067D3C(Gfx **dList, UNUSED Mtx **mtx) {
    s32 originalCamID;

    gSPPerspNormalize((*dList)++, perspNorm);

    originalCamID = gActiveCameraID;
    if (gCutsceneCameraActive) {
        gActiveCameraID += 4;
    }

    gCameraTransform.rotation.y_rotation = 0x8000 + gCameras[gActiveCameraID].trans.rotation.y_rotation;
    gCameraTransform.rotation.x_rotation =
        gCameras[gActiveCameraID].trans.rotation.x_rotation + gCameras[gActiveCameraID].pitch;
    gCameraTransform.rotation.z_rotation = gCameras[gActiveCameraID].trans.rotation.z_rotation;

    gCameraTransform.x_position = -gCameras[gActiveCameraID].trans.x_position;
    gCameraTransform.y_position = -gCameras[gActiveCameraID].trans.y_position;
    if (gNoCamShake) {
        gCameraTransform.y_position -= gCameras[gActiveCameraID].shakeMagnitude;
    }
    gCameraTransform.z_position = -gCameras[gActiveCameraID].trans.z_position;

    mtxf_from_inverse_transform(&gViewMatrixF, &gCameraTransform);
    mtxf_mul(&gViewMatrixF, &gPerspectiveMatrixF, &gViewProjMatrixF);

    gCameraTransform.rotation.y_rotation = -0x8000 - gCameras[gActiveCameraID].trans.rotation.y_rotation;
    gCameraTransform.rotation.x_rotation =
        -(gCameras[gActiveCameraID].trans.rotation.x_rotation + gCameras[gActiveCameraID].pitch);
    gCameraTransform.rotation.z_rotation = -gCameras[gActiveCameraID].trans.rotation.z_rotation;
    gCameraTransform.scale = 1.0f;
    gCameraTransform.x_position = gCameras[gActiveCameraID].trans.x_position;
    gCameraTransform.y_position = gCameras[gActiveCameraID].trans.y_position;
    if (gNoCamShake) {
        gCameraTransform.y_position += gCameras[gActiveCameraID].shakeMagnitude;
    }
    gCameraTransform.z_position = gCameras[gActiveCameraID].trans.z_position;

    mtxf_from_transform(&gInverseViewMatrixF, &gCameraTransform);

    gActiveCameraID = originalCamID;
}

/**
 * Sets the Y value of the Y axis in the matrix to the passed value.
 * This is used to vertically scale ortho geometry to look identical across NTSC and PAL systems.
 * Official Name: camOrthoYAspect
 */
void set_ortho_matrix_height(f32 value) {
    gOrthoMatrixF[1][1] = value;
}

/**
 * Sets the current matrix to represent an orthogonal view.
 * Used for drawing triangles on screen as HUD.
 * Official Name: camStandardOrtho
 */
void mtx_ortho(Gfx **dList, Mtx **mtx) {
    u32 widthAndHeight;
    s32 width, height;
    s32 i, j;

    widthAndHeight = fb_size();
    height = GET_VIDEO_HEIGHT(widthAndHeight);
    width = GET_VIDEO_WIDTH(widthAndHeight);
    mtxf_to_mtx(&gOrthoMatrixF, *mtx);
    gModelMatrix[0] = *mtx;
    gViewportStack[gActiveCameraID + 5].vp.vscale[0] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vscale[1] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vtrans[0] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vtrans[1] = height * 2;
    gSPViewport((*dList)++, OS_K0_TO_PHYSICAL(&gViewportStack[gActiveCameraID + 5]));
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_0);
    gModelMatrixStackPos = 0;
    gMtxOriginID = G_MTX_DKR_INDEX_0;

    for (i = 0; i < 4; i++) {
        // clang-format off
        // Required to be one line, but the "\" fixes that.
        for (j = 0; j < 4; j++) { \
            gViewProjMatrixF[i][j] = gOrthoMatrixF[i][j];
        }
        // clang-format on
    }
}

/**
 * Sets the current matrix to represent a perspective view.
 * Necessary for setting up any 3D scene.
 * Official Name: camStandardPersp?
 */
void mtx_perspective(Gfx **dList, Mtx **mtx) {
    mtxf_from_inverse_transform(&gCurrentModelMatrixF, &D_800DD288);
    mtxf_mul(&gCurrentModelMatrixF, &gPerspectiveMatrixF, &gViewProjMatrixF);
    mtxf_from_inverse_transform(gModelMatrixF[0], &D_800DD2A0);
    mtxf_mul(gModelMatrixF[0], &gViewProjMatrixF, &gCurrentModelMatrixF);
    mtxf_to_mtx(&gCurrentModelMatrixF, *mtx);
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_0);
    gModelMatrixStackPos = 0;
    gMtxOriginID = G_MTX_DKR_INDEX_0;
}

/**
 * Sets the RSP viewport onscreen to the given properties.
 * Viewports have a centre position and a scale factor, rather than a standard four corners.
 * Official Name: camSetViewport
 */
void viewport_rsp_set(Gfx **dList, s32 halfWidth, s32 halfHeight, s32 centerX, s32 centerY) {
    s32 tempWidth = (get_filtered_cheats() & CHEAT_MIRRORED_TRACKS) ? -halfWidth : halfWidth;
#ifdef ANTI_TAMPER
    // Antipiracy measure. Flips the screen upside down.
    if (gAntiPiracyViewport) {
        halfHeight = -halfHeight;
        tempWidth = -halfWidth;
    }
#endif
    if (!(gScreenViewports[gActiveCameraID].flags & VIEWPORT_EXTRA_BG)) {
        gViewportStack[gActiveCameraID].vp.vtrans[0] = centerX * 4;
        gViewportStack[gActiveCameraID].vp.vtrans[1] = centerY * 4;
        gViewportStack[gActiveCameraID].vp.vscale[0] = tempWidth * 4;
        gViewportStack[gActiveCameraID].vp.vscale[1] = halfHeight * 4;
        gSPViewport((*dList)++, OS_K0_TO_PHYSICAL(&gViewportStack[gActiveCameraID]));
    } else {
        gSPViewport((*dList)++, OS_K0_TO_PHYSICAL(&gViewportStack[gActiveCameraID + 10 + (gViewportWithBG * 5)]));
    }
}

/**
 * Resets the viewport back to default.
 * If in the track menu, or post-race, set it to a small screen view instead.
 * Official Name: camResetView?
 */
void viewport_reset(Gfx **dList) {
    u32 widthAndHeight, width, height;
    gActiveCameraID = 4;
    widthAndHeight = fb_size();
    height = GET_VIDEO_HEIGHT(widthAndHeight);
    width = GET_VIDEO_WIDTH(widthAndHeight);
    if (!(gScreenViewports[gActiveCameraID].flags & VIEWPORT_EXTRA_BG)) {
        gDPSetScissor((*dList)++, G_SC_NON_INTERLACE, 0, 0, width, height);
        viewport_rsp_set(dList, width >> 1, height >> 1, width >> 1, height >> 1);
    } else {
        viewport_scissor(dList);
        viewport_rsp_set(dList, 0, 0, 0, 0);
    }
    gActiveCameraID = 0;
}

/**
 * Sets the matrix position to the world origin (0, 0, 0)
 * Used when the next thing rendered relies on there not being any matrix offset.
 * Official Name: camOffsetZero?
 */
void mtx_world_origin(Gfx **dList, Mtx **mtx) {
    mtxf_from_translation(gModelMatrixF[gModelMatrixStackPos], 0.0f, 0.0f, 0.0f);
    mtxf_mul(gModelMatrixF[gModelMatrixStackPos], &gViewProjMatrixF, &gCurrentModelMatrixF);
    mtxf_to_mtx(&gCurrentModelMatrixF, *mtx);
    gModelMatrix[gModelMatrixStackPos] = *mtx;
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), gMtxOriginID);
}

/**
 * Sets the sprite animation mode.
 * This determines how the animation frame is interpreted: either as a frame index,
 * or as a normalized progress value from 0 to 255.
 */
void cam_set_sprite_anim_mode(s32 setting) {
    gSpriteAnimMode = setting;
}

/**
 * Renders a sprite in 3D space as a billboard.
 *
 * If the sprite represents a vehicle part (e.g., wheel, propeller, fan),
 * the function calculates the appropriate animation frame based on the object's
 * orientation relative to the camera, ensuring the sprite correctly matches
 * the viewing angle. It also orients the sprite properly in 3D space with tilt.
 *
 * For regular sprites, it sets up a standard billboard that always faces the camera.
 */
s32 render_sprite_billboard(Gfx **dList, Mtx **mtx, Vertex **vtx, Object *obj, Sprite *sprite, s32 flags) {
    f32 diffX;
    f32 diffY;
    Vertex *v;
    f32 lateralDist;
    f32 sineY;
    f32 cosY;
    f32 temp;
    f32 diffZ;
    s32 tanX;
    s32 tanY;
    s32 tiltAngle;
    s32 result;
    s32 frameID;

    crash_assert(obj == NULL, "Null object passed");

    result = TRUE;
    if (flags & RENDER_VEHICLE_PART) {
        // Vehicle parts like wheels, propellers, and fans are implemented as sprites,
        // each with 16 frames representing different orientations.
        // This requires calculating the correct frame based on the relative orientation
        // between the object and the camera, as well as properly orienting the sprite in 3D space.

        // Calculate camera position relative to the object's position
        diffX = gCameraRelPosStackX[gCameraMatrixPos] - obj->trans.x_position;
        diffY = gCameraRelPosStackY[gCameraMatrixPos] - obj->trans.y_position;
        diffZ = gCameraRelPosStackZ[gCameraMatrixPos] - obj->trans.z_position;

        // Rotate camera coordinates by the object's yaw rotation
        // to get the camera orientation relative to the vehicle,
        // e.g., to determine if we view the wheel from the side.
        sineY = sins_f(obj->trans.rotation.y_rotation);
        cosY = coss_f(obj->trans.rotation.y_rotation);

        temp = (diffX * cosY) + (diffZ * sineY);
        diffZ = (diffZ * cosY) - (diffX * sineY);
        diffX = temp;

        // Calculate the angle between the camera direction and the vertical plane of the vehicle
        tanY = arctan2_f(diffX, sqrtf((diffY * diffY) + (diffZ * diffZ)));

        tanX = -sins_f(arctan2_f(diffX, diffZ)) * 256.0f;
        if (diffZ < 0.0f) {
            diffZ = -diffZ;
            tanX = 1 - tanX;
            tanY = -tanY;
        }

        tiltAngle = arctan2_f(diffY, diffZ);
        if (tiltAngle > 0x8000) {
            tiltAngle -= 0x10000;
        }
        tiltAngle = (tiltAngle * tanX) >> 8;
        frameID = (tanY >> 7) & 0xFF;
        if (frameID > 127) {
            frameID = 255 - frameID;
            tiltAngle += 0x8000;
            result = FALSE;
        }
        frameID *= 2;

        // Construct the model matrix for the sprite,
        // orienting it perpendicular to the camera and applying the calculated tilt angle.
        diffX = gCameraRelPosStackX[gCameraMatrixPos] - obj->trans.x_position;
        diffY = gCameraRelPosStackY[gCameraMatrixPos] - obj->trans.y_position;
        diffZ = gCameraRelPosStackZ[gCameraMatrixPos] - obj->trans.z_position;
        lateralDist = sqrtf((diffX * diffX) + (diffZ * diffZ));
        gCameraTransform.rotation.y_rotation = arctan2_f(diffX, diffZ);
        gCameraTransform.rotation.x_rotation = -arctan2_f(diffY, lateralDist);
        gCameraTransform.rotation.z_rotation = tiltAngle;
        gCameraTransform.scale = obj->trans.scale;
        gCameraTransform.x_position = obj->trans.x_position;
        gCameraTransform.y_position = obj->trans.y_position;
        gCameraTransform.z_position = obj->trans.z_position;
        mtxf_from_transform(&gCurrentModelMatrixF, &gCameraTransform);
        gModelMatrixStackPos++;
        mtxf_mul(&gCurrentModelMatrixF, gModelMatrixF[gModelMatrixStackPos - 1], gModelMatrixF[gModelMatrixStackPos]);
        mtxf_mul(gModelMatrixF[gModelMatrixStackPos], &gViewProjMatrixF, &gCurrentModelMatrixF);
        mtxf_to_mtx(&gCurrentModelMatrixF, *mtx);
        gModelMatrix[gModelMatrixStackPos] = *mtx;
        gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_2);
        // Push an empty vertex because sprite vertices are hardcoded to start from index 1,
        // and billboard mode is not enabled for vehicle parts.
        gSPVertexDKR((*dList)++, OS_K0_TO_PHYSICAL(&gVehiclePartVertex), 1, 0);
    } else {
        // For non-vehicle parts, set up a standard billboard.

        // Push the anchor vertex at the object's position
        v = *vtx;
        v->x = obj->trans.x_position;
        v->y = obj->trans.y_position;
        v->z = obj->trans.z_position;
        v->r = 255; // These don't actually do anything since vertex colours are disabled anyway.
        v->g = 255;
        v->b = 255;
        v->a = 255;
        gSPVertexDKR((*dList)++, OS_K0_TO_PHYSICAL(*vtx), 1, 0);
        (*vtx)++;

        // Create a billboard matrix that compensates for camera tilt,
        // so the sprite tilts consistently with other objects relative to the camera.
        // Aspect ratio compensation is applied to maintain proper sprite proportions on screen.
        if (!gCutsceneCameraActive) {
            tiltAngle = gCameras[gActiveCameraID].trans.rotation.z_rotation + obj->trans.rotation.z_rotation;
        } else {
            tiltAngle = gCameras[gActiveCameraID + 4].trans.rotation.z_rotation + obj->trans.rotation.z_rotation;
        }
        frameID = obj->animFrame;
        gModelMatrixStackPos++;
        mtxf_billboard(gModelMatrixF[gModelMatrixStackPos], tiltAngle, obj->trans.scale * gSpriteWidth, gVideoAspectRatio);
        mtxf_to_mtx(gModelMatrixF[gModelMatrixStackPos], *mtx);
        gModelMatrix[gModelMatrixStackPos] = *mtx;
        gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_2);
        // Enable billboard mode; subsequent vertices will be rendered relative to the anchor point
        gDkrEnableBillboard((*dList)++);
    }
    // Calculate the correct animation frame based on the current animation mode.
    if (gSpriteAnimMode == SPRITE_ANIM_NORMALIZED) {
        frameID = ((frameID & 0xFF) * sprite->numberOfFrames) >> 8;
    }

    // The RENDER_ANTI_ALIASING flag shares the same value as RENDER_VEHICLE_PART,
    // so we must clear RENDER_VEHICLE_PART from flags to avoid conflicts.
    // Anti-aliasing is automatically enabled for semi-transparent sprites.
    flags &= ~RENDER_VEHICLE_PART;
    if (flags & RENDER_SEMI_TRANSPARENT) {
        flags |= RENDER_ANTI_ALIASING;
    }

    // Load material and set render flags including fog, transparency, z-compare, and anti-aliasing
    material_load_simple(dList, sprite->drawFlags | (flags & (RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT |
                                                              RENDER_Z_COMPARE | RENDER_ANTI_ALIASING)));

    if (!(flags & RENDER_Z_UPDATE)) {
        gDPSetPrimColor((*dList)++, 0, 0, 255, 255, 255, 255);
    }
    gSPDisplayList((*dList)++, sprite->frames[frameID]);
    DEBUG_VAR(gDebug->misc.texLoads, gDebug->misc.texLoads + 1);

    // Pop the model matrix stack and select the appropriate matrix index for rendering
    gModelMatrixStackPos--;
    if (gModelMatrixStackPos == 0) {
        frameID = G_MTX_DKR_INDEX_0;
    } else {
        frameID = G_MTX_DKR_INDEX_1;
    }
    gSPSelectMatrixDKR((*dList)++, frameID);

    // Disable billboard mode
    gDkrDisableBillboard((*dList)++);

    return result;
}

/**
 * Sets transform and scale matrices to set position and size, loads the texture, sets the rendermodes, then draws the
 * result onscreen.
 */
void render_ortho_triangle_image(Gfx **dList, Mtx **mtx, Vertex **vtx, ObjectSegment *segment, Sprite *sprite,
                                 s32 flags) {
    UNUSED s32 pad;
    f32 scale;
    s32 index;
    Vertex *vertex;
    MtxF aspectMtxF;

    if (sprite == NULL) {
        return;
    }

    vertex = *vtx;
    vertex->x = segment->trans.x_position;
    vertex->y = segment->trans.y_position;
    vertex->z = segment->trans.z_position;
    vertex->r = 255; // These don't actually do anything since vertex colours are disabled anyway.
    vertex->g = 255;
    vertex->b = 255;
    vertex->a = 255;

    gSPVertexDKR((*dList)++, OS_K0_TO_PHYSICAL(*vtx), 1, 0);
    (*vtx)++; // Can't be done in the macro?
    index = segment->animFrame;

    gModelMatrixStackPos++;
    gCameraTransform.rotation.y_rotation = -segment->trans.rotation.y_rotation;
    gCameraTransform.rotation.x_rotation = -segment->trans.rotation.x_rotation;
    gCameraTransform.rotation.z_rotation =
        gCameras[gActiveCameraID].trans.rotation.z_rotation + segment->trans.rotation.z_rotation;
    gCameraTransform.x_position = 0.0f;
    gCameraTransform.y_position = 0.0f;
    gCameraTransform.z_position = 0.0f;
    scale = segment->trans.scale;
    mtxf_from_scale(&gCurrentModelMatrixF, scale, scale, 1.0f);
    mtxf_from_inverse_transform(&aspectMtxF, &gCameraTransform);
    mtxf_mul(&gCurrentModelMatrixF, &aspectMtxF, gModelMatrixF[gModelMatrixStackPos]);
    mtxf_to_mtx(gModelMatrixF[gModelMatrixStackPos], *mtx);
    gModelMatrix[gModelMatrixStackPos] = *mtx;
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_2);
    gDkrEnableBillboard((*dList)++);
    if (gSpriteAnimMode == SPRITE_ANIM_NORMALIZED) {
        index = (((u8) index) * sprite->numberOfFrames) >> 8;
    }
    material_load_simple(dList, sprite->drawFlags | flags);
    if (index >= sprite->numberOfFrames) {
        index = sprite->numberOfFrames - 1;
    }
    gSPDisplayList((*dList)++, sprite->frames[index]);
    DEBUG_VAR(gDebug->misc.texLoads, gDebug->misc.texLoads + 1);
    if (--gModelMatrixStackPos == 0) {
        index = G_MTX_DKR_INDEX_0;
    } else {
        index = G_MTX_DKR_INDEX_1;
    }
    gSPSelectMatrixDKR((*dList)++, index);
    gDkrDisableBillboard((*dList)++);
}

/**
 * Generate a matrix with rotation, scaling and shearing and run it.
 * Used for wavy type effects like the shield.
 */
void mtx_shear_push(Gfx **dList, Mtx **mtx, Object *obj, Object *objBase, f32 shear) {
    UNUSED s32 pad;
    f32 cossf_x_arg2;
    f32 cossf_y_arg2;
    f32 sinsf_x_arg2;
    f32 sinsf_y_arg2;
    f32 sinsf_y_arg3;
    f32 sinsf_z_arg3;
    f32 arg2_scale;
    f32 cossf_x_arg3;
    f32 sinsf_x_arg3;
    f32 cossf_y_arg3;
    f32 cossf_z_arg3;
    f32 arg2_xPos;
    f32 arg2_yPos;
    f32 arg2_zPos;
    f32 arg3_xPos;
    f32 arg3_yPos;
    f32 arg3_zPos;
    MtxF matrix_mult;

    cossf_x_arg2 = coss_f(obj->trans.rotation.x_rotation);
    sinsf_x_arg2 = sins_f(obj->trans.rotation.x_rotation);
    cossf_y_arg2 = coss_f(obj->trans.rotation.y_rotation);
    sinsf_y_arg2 = sins_f(obj->trans.rotation.y_rotation);
    arg2_xPos = obj->trans.x_position;
    arg2_yPos = obj->trans.y_position;
    arg2_zPos = obj->trans.z_position;
    cossf_z_arg3 = coss_f(objBase->trans.rotation.z_rotation);
    sinsf_z_arg3 = sins_f(objBase->trans.rotation.z_rotation);
    cossf_x_arg3 = coss_f(objBase->trans.rotation.x_rotation);
    sinsf_x_arg3 = sins_f(objBase->trans.rotation.x_rotation);
    cossf_y_arg3 = coss_f(objBase->trans.rotation.y_rotation);
    sinsf_y_arg3 = sins_f(objBase->trans.rotation.y_rotation);
    arg3_xPos = objBase->trans.x_position;
    arg3_yPos = objBase->trans.y_position;
    arg3_zPos = objBase->trans.z_position;
    arg2_scale = obj->trans.scale;
    shear *= arg2_scale;
    matrix_mult[0][0] =
        ((((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * cossf_y_arg2) +
         (-sinsf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))) *
        arg2_scale;
    matrix_mult[0][1] = (((sinsf_z_arg3 * cossf_x_arg3) * cossf_y_arg2) + (-sinsf_y_arg2 * -sinsf_x_arg3)) * arg2_scale;
    matrix_mult[0][2] =
        ((((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * cossf_y_arg2) +
         (-sinsf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))) *
        arg2_scale;
    matrix_mult[0][3] = 0.0f;
    matrix_mult[1][0] =
        ((((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * cossf_x_arg2) +
         (sinsf_x_arg2 *
          ((sinsf_y_arg2 * ((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) +
           (cossf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))))) *
        shear;
    matrix_mult[1][1] =
        (((cossf_z_arg3 * cossf_x_arg3) * cossf_x_arg2) +
         (sinsf_x_arg2 * ((sinsf_y_arg2 * (sinsf_z_arg3 * cossf_x_arg3)) + (cossf_y_arg2 * -sinsf_x_arg3)))) *
        shear;
    matrix_mult[1][2] =
        ((((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * cossf_x_arg2) +
         (sinsf_x_arg2 *
          ((sinsf_y_arg2 * ((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) +
           (cossf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))))) *
        shear;
    matrix_mult[1][3] = 0.0f;
    matrix_mult[2][0] =
        ((-sinsf_x_arg2 * ((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) +
         (cossf_x_arg2 *
          ((sinsf_y_arg2 * ((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) +
           (cossf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))))) *
        arg2_scale;
    matrix_mult[2][1] =
        ((-sinsf_x_arg2 * (cossf_z_arg3 * cossf_x_arg3)) +
         (cossf_x_arg2 * ((sinsf_y_arg2 * (sinsf_z_arg3 * cossf_x_arg3)) + (cossf_y_arg2 * -sinsf_x_arg3)))) *
        arg2_scale;
    matrix_mult[2][2] =
        ((-sinsf_x_arg2 * ((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) +
         (cossf_x_arg2 *
          ((sinsf_y_arg2 * ((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) +
           (cossf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))))) *
        arg2_scale;
    matrix_mult[2][3] = 0.0f;
    matrix_mult[3][0] =
        (((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * arg2_xPos) +
        (arg2_yPos * ((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) +
        (arg2_zPos * (cossf_x_arg3 * sinsf_y_arg3)) + arg3_xPos;
    matrix_mult[3][1] = ((sinsf_z_arg3 * cossf_x_arg3) * arg2_xPos) + (arg2_yPos * (cossf_z_arg3 * cossf_x_arg3)) +
                        (arg2_zPos * -sinsf_x_arg3) + arg3_yPos;
    matrix_mult[3][2] =
        (((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * arg2_xPos) +
        (arg2_yPos * ((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) +
        (arg2_zPos * (cossf_x_arg3 * cossf_y_arg3)) + arg3_zPos;
    matrix_mult[3][3] = 1.0f;

    mtxf_mul(&matrix_mult, &gViewProjMatrixF, &gCurrentMVPMatrixF);
    mtxf_to_mtx(&gCurrentMVPMatrixF, *mtx);
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_1);
}

/**
 * Pushes a model matrix onto the model matrix stack, generates an MVP matrix from it, and uploads it to the RSP.
 * Also computes the camera's position relative to the new model (at the top of the stack),
 * and pushes it onto the camera-relative position stack.
 * This relative camera position can later be used for sprite positioning.
 *
 * Official Name: camPushModelMtx
 */
s32 mtx_cam_push(Gfx **dList, Mtx **mtx, ObjectTransform *trans, f32 scaleY, f32 offsetY) {
    f32 camRelX, camRelY, camRelZ;
    s32 index;
    f32 scaleFactor;

    // Generate model transformation matrix from input transform
    mtxf_from_transform(&gCurrentModelMatrixF, trans);

    // Apply optional Y-axis translation
    if (offsetY != 0.0f) {
        mtxf_translate_y(&gCurrentModelMatrixF, offsetY);
    }

    // Apply optional Y-axis scaling
    if (scaleY != 1.0f) {
        mtxf_scale_y(&gCurrentModelMatrixF, scaleY);
    }

    // Multiply model matrix with parent matrix (top of the model stack)
    mtxf_mul(&gCurrentModelMatrixF, gModelMatrixF[gModelMatrixStackPos], gModelMatrixF[gModelMatrixStackPos + 1]);

    // Compute the model-view-projection matrix
    mtxf_mul(gModelMatrixF[gModelMatrixStackPos + 1], &gViewProjMatrixF, &gCurrentMVPMatrixF);

    // Convert the MVP matrix to fixed-point format and upload to RSP
    mtxf_to_mtx(&gCurrentMVPMatrixF, *mtx);
    gModelMatrixStackPos++;
    gModelMatrix[gModelMatrixStackPos] = *mtx;

    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_1);

    // Compute world-space position of the model's origin (0, 0, 0)
    mtxf_transform_point(*gModelMatrixF[gModelMatrixStackPos], 0.0f, 0.0f, 0.0f, &camRelX, &camRelY, &camRelZ);

    index = gActiveCameraID;
    if (gCutsceneCameraActive) {
        // Use cutscene camera if active
        index += 4;
    }

    // Compute camera position relative to the model's origin in world space
    camRelX = gCameras[index].trans.x_position - camRelX;
    camRelY = gCameras[index].trans.y_position - camRelY;
    camRelZ = gCameras[index].trans.z_position - camRelZ;

    // Convert camera position from world space to the model's local coordinate space
    gCameraTransform.rotation.y_rotation = -trans->rotation.y_rotation;
    gCameraTransform.rotation.x_rotation = -trans->rotation.x_rotation;
    gCameraTransform.rotation.z_rotation = -trans->rotation.z_rotation;
    gCameraTransform.x_position = 0.0f;
    gCameraTransform.y_position = 0.0f;
    gCameraTransform.z_position = 0.0f;
    gCameraTransform.scale = 1.0f;

    mtxf_from_inverse_transform(&gCurrentModelMatrixF, &gCameraTransform);
    mtxf_transform_point(gCurrentModelMatrixF, camRelX, camRelY, camRelZ, &camRelX, &camRelY, &camRelZ);

    // Adjust for model scale
    scaleFactor = 1.0f / trans->scale;
    camRelX *= scaleFactor;
    camRelY *= scaleFactor;
    camRelZ *= scaleFactor;

    // Push camera position relative to the current model onto the stack
    gCameraMatrixPos++;
    gCameraRelPosStackX[gCameraMatrixPos] = camRelX;
    gCameraRelPosStackY[gCameraMatrixPos] = camRelY;
    gCameraRelPosStackZ[gCameraMatrixPos] = camRelZ;

#ifdef AVOID_UB
    // Likely void in original code, but returns 0 to match waves_render.
    return 0;
#endif
}

/**
 * Calculate the rotation matrix for an actors head, then run it.
 */
void mtx_head_push(Gfx **dList, Mtx **mtx, ModelInstance *modInst, s16 headAngle) {
    f32 coss_headAngle;
    f32 sins_headAngle;
    f32 offsetX;
    f32 offsetY;
    f32 offsetZ;
    f32 coss_unk1C;
    f32 sins_unk1C;
    MtxF rotationMtxF;
    MtxF headMtxF;

    offsetX = (f32) modInst->offsetX;
    offsetY = (f32) modInst->offsetY;
    offsetZ = (f32) modInst->offsetZ;
    coss_unk1C = coss_f(modInst->headTilt);
    sins_unk1C = sins_f(modInst->headTilt);
    coss_headAngle = coss_f(headAngle);
    sins_headAngle = sins_f(headAngle);
    headMtxF[0][0] = (coss_headAngle * coss_unk1C);
    headMtxF[0][1] = (coss_headAngle * sins_unk1C);
    headMtxF[0][2] = -sins_headAngle;
    headMtxF[0][3] = 0.0f;
    headMtxF[1][0] = -sins_unk1C;
    headMtxF[1][1] = coss_unk1C;
    headMtxF[1][2] = 0.0f;
    headMtxF[1][3] = 0.0f;
    headMtxF[2][0] = (sins_headAngle * coss_unk1C);
    headMtxF[2][1] = (sins_headAngle * sins_unk1C);
    headMtxF[2][2] = coss_headAngle;
    headMtxF[2][3] = 0.0f;
    headMtxF[3][0] = (-offsetX * (coss_headAngle * coss_unk1C)) + (-offsetY * -sins_unk1C) +
                     (-offsetZ * (sins_headAngle * coss_unk1C)) + offsetX;
    headMtxF[3][1] = (-offsetX * (coss_headAngle * sins_unk1C)) + (-offsetY * coss_unk1C) +
                     (-offsetZ * (sins_headAngle * sins_unk1C)) + offsetY;
    headMtxF[3][2] = (-offsetX * -sins_headAngle) + (-offsetZ * coss_headAngle) + offsetZ;
    headMtxF[3][3] = 1.0f;
    mtxf_mul(&headMtxF, &gCurrentMVPMatrixF, &rotationMtxF);
    mtxf_to_mtx(&rotationMtxF, *mtx);
    gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL((*mtx)++), G_MTX_DKR_INDEX_2);
    gSPSelectMatrixDKR((*dList)++, G_MTX_DKR_INDEX_1);
}

/**
 * Run a matrix from the top of the stack and pop it.
 * If the stack pos is less than zero, set the RSP stack pos to 0.
 */
void mtx_pop(Gfx **dList) {

    gCameraMatrixPos--;
    gModelMatrixStackPos--;

    if (gModelMatrixStackPos > 0) {
        gSPMatrixDKR((*dList)++, OS_K0_TO_PHYSICAL(gModelMatrix[gModelMatrixStackPos]), G_MTX_DKR_INDEX_1);
    } else {
        gSPSelectMatrixDKR((*dList)++, G_MTX_DKR_INDEX_0);
    }
}

/**
 * Returns the segment data of the active camera, but won't apply the offset for cutscenes.
 */
Camera *cam_get_active_camera_no_cutscenes(void) {
    return &gCameras[gActiveCameraID];
}

/**
 * Returns the active camera.
 */
Camera *cam_get_active_camera(void) {
    if (gCutsceneCameraActive) {
        return &gCameras[gActiveCameraID + 4];
    }
    return &gCameras[gActiveCameraID];
}

/**
 * Returns the segment data of the active cutscene camera.
 * If no cutscene is active, return player 1's camera.
 */
Camera *cam_get_cameras(void) {
    if (gCutsceneCameraActive) {
        return &gCameras[4];
    }
    return &gCameras[0];
}

/**
 * Return the current floating point projection matrix.
 */
MtxF *get_projection_matrix_f32(void) {
    return &gInverseViewMatrixF;
}

/**
 * Return the current fixed point projection matrix.
 */
Mtx *get_projection_matrix_s16(void) {
    return &gPerspectiveMatrix;
}

/**
 * Return the current camera matrix.
 */
MtxF *get_camera_matrix(void) {
    return &gViewMatrixF;
}

/**
 * Return the screenspace distance to the camera.
 */
f32 get_distance_to_camera(f32 x, f32 y, f32 z) {
    f32 ox, oy, oz;

    mtxf_transform_point(gViewMatrixF, x, y, z, &ox, &oy, &oz);

    return oz;
}

/**
 * Apply a shake to the camera based on the distance to the source.
 */
void set_camera_shake_by_distance(f32 x, f32 y, f32 z, f32 dist, f32 magnitude) {
    f32 diffX;
    f32 distance;
    f32 diffZ;
    f32 diffY;
    s32 i;

    for (i = 0; i <= gViewportLayout; i++) {
        diffX = x - gCameras[i].trans.x_position;
        diffY = y - gCameras[i].trans.y_position;
        diffZ = z - gCameras[i].trans.z_position;
        distance = sqrtf(((diffX * diffX) + (diffY * diffY)) + (diffZ * diffZ));
        if (distance < dist) {
            gCameras[i].shakeMagnitude = ((dist - distance) * magnitude) / dist;
        }
    }
}

/**
 * Apply a shake to all active cameras.
 */
void set_camera_shake(f32 magnitude) {
    s32 i;
    for (i = 0; i <= gViewportLayout; i++) {
        gCameras[i].shakeMagnitude = magnitude;
    }
}

