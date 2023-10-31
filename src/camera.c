/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80065EA0 */

#include "camera.h"
#include "audio.h"
#include "objects.h"
#include "game.h"
#include "tracks.h"
#include "video.h"
#include "lib/src/libc/rmonPrintf.h"
#include "math_util.h"
#include "weather.h"
#include "main.h"
#include "lib/src/os/piint.h"
#include "printf.h"


/************ .data ************/

s8 gAntiPiracyViewport = FALSE;

// x1, y1, x2, y2
// posX, posY, width, height
// scissorX1, scissorY1, scissorX2, scissorY2
// flags
#define DEFAULT_VIEWPORT                                                \
    0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,                                  \
    SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF, SCREEN_WIDTH, SCREEN_HEIGHT, \
    0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,                          \
    0

ScreenViewport gScreenViewports[4] = {
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
    { DEFAULT_VIEWPORT },
};

u32 gViewportWithBG = FALSE;

Vertex gVehiclePartVertex = {
    0, 0, 0, 255, 255, 255, 255
};

//The viewport z-range below is half of the max (511)
#define G_HALFZ (G_MAXZ / 2) /* 9 bits of integer screen-Z precision */

// RSP Viewports
Vp gViewportStack[20] = {
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
    { { { 0, 0, G_HALFZ, 0 }, { 0, 0, G_HALFZ, 0 } } },
};

ObjectTransform D_800DD288 = {
    0, 0, 0, 0, 1.0f, 0.0f, 0.0f, -281.0f,
};

ObjectTransform D_800DD2A0 = {
    0, 0, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f,
};

Matrix gOrthoMatrixF = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 160.0f },
};

u8 D_800DD2F8[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

/*******************************/

/************ .bss ************/

ObjectSegment gCameraSegment[8];
s32 gNumberOfViewports;
s32 gActiveCameraID;
s32 gViewportCap;
ObjectTransform gCameraTransform;
s32 gMatrixType;
s32 D_80120D0C;
f32 gCurCamFOV;
s8 gCutsceneCameraActive;
s8 gAdjustViewportHeight;
u16 perspNorm;
s32 D_80120D18;
s32 gModelMatrixStackPos;
s32 gCameraMatrixPos;
f32 gModelMatrixViewX[6];
f32 gModelMatrixViewY[6];
f32 gModelMatrixViewZ[5];
Matrix *gModelMatrixF[6];
MatrixS *gModelMatrixS[6];
Matrix D_80120DA0[5];
Matrix gPerspectiveMatrixF;
Matrix gViewMatrixF;
Matrix gCameraMatrixF;
Matrix gProjectionMatrixF;
MatrixS gProjectionMatrixS;
Matrix gCurrentModelMatrixF;
Matrix D_801210A0;

/******************************/

extern s32 D_B0000578;
/**
 * Official Name: camInit
*/
void camera_init(void) {
    s32 i;
    s32 j;
 
    // This loop is not cooperating. 
    for (i = 0; i < 5; i++) { gModelMatrixF[i] = D_80120DA0 + i + i*0; } 

    for (j = 0; j < 8; j++) {
        gActiveCameraID = j;
        func_800663DC(200, 200, 200, 0, 0, 180);
    }
    
    gCutsceneCameraActive = FALSE; 
    gActiveCameraID = 0;
    gModelMatrixStackPos = 0;
    gCameraMatrixPos = 0;
    gNumberOfViewports = 0;
    D_80120D0C = 0;
    D_80120D18 = 0;
    gAdjustViewportHeight = 0;
#ifndef NO_ANTIPIRACY
    gAntiPiracyViewport = 0;
    
    WAIT_ON_IOBUSY(stat);
    
    //0xB0000578 is a direct read from the ROM as opposed to RAM
    if (((D_B0000578 & 0xFFFF) & 0xFFFF) != 0x8965) {
        gAntiPiracyViewport = TRUE;
    }
#endif
    guPerspectiveF(gPerspectiveMatrixF, &perspNorm, CAMERA_DEFAULT_FOV, CAMERA_ASPECT, CAMERA_NEAR, CAMERA_FAR, CAMERA_SCALE);
    f32_matrix_to_s16_matrix(&gPerspectiveMatrixF, &gProjectionMatrixS);
    gCurCamFOV = CAMERA_DEFAULT_FOV;
}

void reset_perspective_matrix(void) {
    guPerspectiveF(gPerspectiveMatrixF, &perspNorm, CAMERA_DEFAULT_FOV, CAMERA_ASPECT, CAMERA_NEAR, CAMERA_FAR, CAMERA_SCALE);
    f32_matrix_to_s16_matrix(&gPerspectiveMatrixF, &gProjectionMatrixS);
}

void func_80066060(s32 arg0, s32 arg1) {
    if (arg0 >= 0 && arg0 < 4) {
        D_800DD2F8[arg0] = arg1;
        gCameraSegment[arg0].object.animationID = arg1;
    }
}

/**
 * Set gAdjustViewportHeight to PAL mode if necessary, if setting is 1.
 * Otherwise, set it to 0, regardless of TV type.
*/
void set_viewport_tv_type(s8 setting) {
    if (osTvType == TV_TYPE_PAL) {
        gAdjustViewportHeight = setting;
    }
}

void func_800660C0(void) {
    D_80120D18 = 1;
}

void func_800660D0(void) {
    D_80120D18 = 0;
}

/**
 * Set the FoV of the viewspace, then recalculate the perspective matrix.
 * Official Name: camSetFOV
 */
void update_camera_fov(f32 camFieldOfView) {
    if (CAMERA_MIN_FOV < camFieldOfView && camFieldOfView < CAMERA_MAX_FOV && camFieldOfView != gCurCamFOV) {
        gCurCamFOV = camFieldOfView;
        guPerspectiveF(gPerspectiveMatrixF, &perspNorm, camFieldOfView, CAMERA_ASPECT, CAMERA_NEAR, CAMERA_FAR, CAMERA_SCALE);
        f32_matrix_to_s16_matrix(&gPerspectiveMatrixF, &gProjectionMatrixS);
    }
}

/**
 * Returns the number of active viewports.
*/
s32 get_viewport_count(void) {
    return gNumberOfViewports;
}

/**
 * Return the index of the active view.
 * 0-3 is players 1-4, and 4-7 is the same, but with 4 added on for cutscenes.
*/
s32 get_current_viewport(void) {
    return gActiveCameraID;
}

void func_80066230(Gfx **dlist, MatrixS **mats) {
    ObjectSegment *cam;
    s16 angleY;
    s16 angleX;
    s16 angleZ;
    s16 sp24;
    f32 posX;
    f32 posY;
    f32 posZ;

    set_active_viewports_and_max(0);
    set_active_camera(0);
    cam = get_active_camera_segment();
    angleY = cam->trans.y_rotation;
    angleX = cam->trans.x_rotation;
    angleZ = cam->trans.z_rotation;
    posX = cam->trans.x_position;
    posY = cam->trans.y_position;
    posZ = cam->trans.z_position;
    sp24 = cam->camera.unk38;
    cam->trans.z_rotation = 0;
    cam->trans.x_rotation = 0;
    cam->trans.y_rotation = -0x8000;
    cam->camera.unk38 = 0;
    cam->trans.x_position = 0.0f;
    cam->trans.y_position = 0.0f;
    cam->trans.z_position = 0.0f;
    update_envmap_position(0.0f, 0.0f, -1.0f);
    func_80066CDC(dlist, mats);
    cam->camera.unk38 = sp24;
    cam->trans.y_rotation = angleY;
    cam->trans.x_rotation = angleX;
    cam->trans.z_rotation = angleZ;
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

    dz = zPos - gCameraSegment[index].trans.z_position;
    dx = xPos - gCameraSegment[index].trans.x_position;
    dy = yPos - gCameraSegment[index].trans.y_position;
    return ((dz * dz) + ((dx * dx) + (dy * dy)));
}

void func_800663DC(s32 xPos, s32 yPos, s32 zPos, s32 arg3, s32 arg4, s32 arg5) {
    gCameraSegment[gActiveCameraID].trans.z_rotation = (s16) (arg3 * 0xB6);
    gCameraSegment[gActiveCameraID].trans.x_position = (f32) xPos;
    gCameraSegment[gActiveCameraID].trans.y_position = (f32) yPos;
    gCameraSegment[gActiveCameraID].trans.z_position = (f32) zPos;
    gCameraSegment[gActiveCameraID].trans.x_rotation = (s16) (arg4 * 0xB6);
    gCameraSegment[gActiveCameraID].camera.unk38 = (s16) 0;
    gCameraSegment[gActiveCameraID].z_velocity = 0.0f;
    gCameraSegment[gActiveCameraID].unk28 = 0.0f;
    gCameraSegment[gActiveCameraID].camera.unk2C = 0.0f;
    gCameraSegment[gActiveCameraID].camera.distanceToCamera = 0.0f;
    gCameraSegment[gActiveCameraID].x_velocity = 160.0f;
    gCameraSegment[gActiveCameraID].trans.y_rotation = (s16) (arg5 * 0xB6);
    gCameraSegment[gActiveCameraID].object.animationID = D_800DD2F8[gActiveCameraID];
}

/**
 * Write directly to the second set of object stack indeces.
 * The first 4 are reserved for the 4 player viewports, so the misc views, used in the title screen
 * and course previews instead use the next 4.
*/
void write_to_object_render_stack(s32 stackPos, f32 xPos, f32 yPos, f32 zPos, s16 arg4, s16 arg5, s16 arg6) {
    stackPos += 4;
    gCameraSegment[stackPos].camera.unk38 = 0;
    gCameraSegment[stackPos].trans.x_position = xPos;
    gCameraSegment[stackPos].trans.y_position = yPos;
    gCameraSegment[stackPos].trans.z_position = zPos;
    gCameraSegment[stackPos].trans.y_rotation = arg4;
    gCameraSegment[stackPos].trans.x_rotation = arg5;
    gCameraSegment[stackPos].trans.z_rotation = arg6;
    gCameraSegment[stackPos].object.cameraSegmentID = get_level_segment_index_from_position(xPos, yPos, zPos);
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
 * Sets the cap for the viewports. Usually reflecting how many there are.
 * If the number passed is within 1-4, then the stack cap is set to
 * how many active viewports there are.
*/
s32 set_active_viewports_and_max(s32 num) {
    if (num >= 0 && num < 4) {
        gNumberOfViewports = num;
    } else {
        gNumberOfViewports = 0;
    }
    switch (gNumberOfViewports) {
        case VIEWPORTS_COUNT_1_PLAYER:
            gViewportCap = 1;
            break;
        case VIEWPORTS_COUNT_2_PLAYERS:
            gViewportCap = 2;
            break;
        case VIEWPORTS_COUNT_3_PLAYERS:
            gViewportCap = 3;
            break;
        case VIEWPORTS_COUNT_4_PLAYERS:
            gViewportCap = 4;
            break;
    }
    if (gActiveCameraID >= gViewportCap) {
        gActiveCameraID = 0;
    }
    return gViewportCap;
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
void resize_viewport(s32 viewPortIndex, s32 x1, s32 y1, s32 x2, s32 y2) {
    s32 width, height;
    s32 temp;

    height = gScreenHeight;
    width = gScreenWidth;

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
        gScreenViewports[viewPortIndex].posX = posY;
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
    //gDPFillRectangle values
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

void func_80066CDC(Gfx **dlist, MatrixS **mats) {
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

    if (func_8000E184() && gNumberOfViewports == VIEWPORTS_COUNT_1_PLAYER) {
        gActiveCameraID = 1;
        savedCameraID = 0;
    }
    videoHeight = gScreenHeight;
    videoWidth = gScreenWidth;
    if (gScreenViewports[savedCameraID].flags & VIEWPORT_EXTRA_BG) {
        tempCameraID = gActiveCameraID;
        gActiveCameraID = savedCameraID;
        gDPSetScissor((*dlist)++, G_SC_NON_INTERLACE,
            gScreenViewports[gActiveCameraID].scissorX1,
            gScreenViewports[gActiveCameraID].scissorY1,
            gScreenViewports[gActiveCameraID].scissorX2,
            gScreenViewports[gActiveCameraID].scissorY2
        );
        func_80068158(dlist, 0, 0, 0, 0);
        gActiveCameraID = tempCameraID;
        if (mats != 0) {
            func_80067D3C(dlist, mats);
        }
        gActiveCameraID = originalCameraID;
        return;
    }
    viewports = gNumberOfViewports;
    if (viewports == VIEWPORTS_COUNT_3_PLAYERS) {
        viewports = VIEWPORTS_COUNT_4_PLAYERS;
    }

    pos[0] = videoWidth / 2;
    pos[1] = videoHeight / 2;
    size[0] = pos[0];
    size[1] = pos[1];

    // Cursed usage of the pos vars, but that's to save redoing width and height divide by 2.
    switch (viewports) {
    case VIEWPORTS_COUNT_1_PLAYER:
        x1 = 0;
        y1 = 0;
        x2 = videoWidth;
        y2 = videoHeight;
        break;
    case VIEWPORTS_COUNT_2_PLAYERS:
        x1 = 0;
        x2 = videoWidth;
        if (gActiveCameraID == 0) {
            y1 = 0;
            y2 = pos[1] - 2;
            pos[1] /= 2;
        } else {
            y1 = pos[1] + 2;
            y2 = videoHeight;
            pos[1] += size[1] / 2;
        }
        break;
    case VIEWPORTS_COUNT_4_PLAYERS:
        size[0] /= 2;
        size[1] /= 2;
        switch (gActiveCameraID) {
        case 0:
            x1 = 0;
            y1 = 0;
            x2 = pos[0] - 2;
            y2 = pos[1] - 2;
            pos[0] /= 2;
            pos[1] /= 2;
            break;
        case 1:
            x1 = pos[0] + 2;
            y1 = 0;
            x2 = videoWidth;
            y2 = pos[1] - 2;
            pos[0] += size[0];
            pos[1] /= 2;
            break;
        case 2:
            x1 = 0;
            y1 = pos[1] + 2;
            x2 = pos[0]- 2;
            y2 = videoHeight;
            pos[0] /= 2;
            pos[1] += size[1];
            break;
        case 3:
            x1 = pos[0] + 2;
            y1 = pos[1] + 2;
            x2 = videoWidth;
            y2 = videoHeight;
            pos[0] += size[0];
            pos[1] += size[1];
            break;
        }
        break;
    }
    gDPSetScissor((*dlist)++, G_SC_NON_INTERLACE, x1, y1, x2, y2);

    func_80068158(dlist, size[0], size[1], pos[0], pos[1]);
    if (mats != NULL) {
        func_80067D3C(dlist, mats);
    }
    gActiveCameraID = originalCameraID;
}

/**
 * Takes the size of the screen as depicted by an active screen viewport, then sets the RDP scissor to match it.
 * Official Name: camSetScissor
*/
void set_viewport_scissor(Gfx **dlist) {
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

    height = gScreenHeight;
    width = gScreenWidth;
    numViewports = gNumberOfViewports;

    if (numViewports != 0) {
        if (numViewports == VIEWPORTS_COUNT_3_PLAYERS) {
            numViewports = VIEWPORTS_COUNT_4_PLAYERS;
        }
        lrx = ulx = 0;
        lry = uly = 0;
        lrx += width;\
        lry += height;
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
                        ulx = temp4 + temp2; \
                        uly = temp3 + temp;
                        lrx -= temp2; \
                        lry -= temp;
                        break;
                }
                break;
        }
        gDPSetScissor((*dlist)++, 0, ulx, uly, lrx, lry);
        return;
    }
    gDPSetScissor((*dlist)++, 0, 0, 0, width, height);
}

//Official Name: camGetPlayerProjMtx / camSetProjMtx - ??
void func_80067D3C(Gfx **dlist, UNUSED MatrixS **mats) {
    s32 temp;

    gSPPerspNormalize((*dlist)++, perspNorm);

    temp = gActiveCameraID;
    if (gCutsceneCameraActive) {
        gActiveCameraID += 4;
    }

    gCameraTransform.y_rotation = 0x8000 + gCameraSegment[gActiveCameraID].trans.y_rotation;
    gCameraTransform.x_rotation = gCameraSegment[gActiveCameraID].trans.x_rotation + gCameraSegment[gActiveCameraID].camera.unk38;
    gCameraTransform.z_rotation = gCameraSegment[gActiveCameraID].trans.z_rotation;

    gCameraTransform.x_position = -gCameraSegment[gActiveCameraID].trans.x_position;
    gCameraTransform.y_position = -gCameraSegment[gActiveCameraID].trans.y_position;
    if (D_80120D18 != 0) {
        gCameraTransform.y_position -= gCameraSegment[gActiveCameraID].camera.distanceToCamera;
    }
    gCameraTransform.z_position = -gCameraSegment[gActiveCameraID].trans.z_position;

    object_transform_to_matrix_2(gCameraMatrixF, &gCameraTransform);
    f32_matrix_mult(&gCameraMatrixF, &gPerspectiveMatrixF, &gViewMatrixF);

    gCameraTransform.y_rotation = -0x8000 - gCameraSegment[gActiveCameraID].trans.y_rotation;
    gCameraTransform.x_rotation = -(gCameraSegment[gActiveCameraID].trans.x_rotation + gCameraSegment[gActiveCameraID].camera.unk38);
    gCameraTransform.z_rotation = -gCameraSegment[gActiveCameraID].trans.z_rotation;
    gCameraTransform.scale = 1.0f;
    gCameraTransform.x_position = gCameraSegment[gActiveCameraID].trans.x_position;
    gCameraTransform.y_position = gCameraSegment[gActiveCameraID].trans.y_position;
    if (D_80120D18 != 0) {
        gCameraTransform.y_position += gCameraSegment[gActiveCameraID].camera.distanceToCamera;
    }
    gCameraTransform.z_position = gCameraSegment[gActiveCameraID].trans.z_position;

    object_transform_to_matrix(gProjectionMatrixF, &gCameraTransform);

    gActiveCameraID = temp;
}

/**
 * Sets the current matrix to represent an orthogonal view.
 * Used for drawing triangles on screen as HUD.
 * Official Name: camStandardOrtho
*/
void set_ortho_matrix_view(Gfx **dlist, MatrixS **mtx) {
    s32 width, height;
    s32 i, j;

    height = 240;
    width = 320;
    f32_matrix_to_s16_matrix(&gOrthoMatrixF, *mtx);
    gModelMatrixS[0] = *mtx;
    gViewportStack[gActiveCameraID + 5].vp.vscale[0] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vscale[1] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vtrans[0] = width * 2;
    gViewportStack[gActiveCameraID + 5].vp.vtrans[1] = height * 2;
    gSPViewport((*dlist)++, OS_K0_TO_PHYSICAL(&gViewportStack[gActiveCameraID + 5]));
    gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gModelMatrixStackPos = 0;
    gMatrixType = G_MTX_DKR_INDEX_0;

    for (i = 0; i < 4; i++) {
        //Required to be one line, but the "\" fixes that.
        for (j = 0; j < 4; j++){ \
            gViewMatrixF[i][j] = gOrthoMatrixF[i][j];
        }
    }
}

//Official Name: camStandardPersp?
void func_8006807C(Gfx **dlist, MatrixS **mtx) {
    object_transform_to_matrix_2(gCurrentModelMatrixF, &D_800DD288);
    f32_matrix_mult(&gCurrentModelMatrixF, &gPerspectiveMatrixF, &gViewMatrixF);
    object_transform_to_matrix_2((float (*)[4]) gModelMatrixF[0], &D_800DD2A0);
    f32_matrix_mult(gModelMatrixF[0], &gViewMatrixF, &gCurrentModelMatrixF);
    f32_matrix_to_s16_matrix(&gCurrentModelMatrixF, *mtx);
    gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gModelMatrixStackPos = 0;
    gMatrixType = G_MTX_DKR_INDEX_0;
}

//Official Name: camSetViewport?
void func_80068158(Gfx **dlist, s32 width, s32 height, s32 posX, s32 posY) {
    s32 tempWidth = (get_filtered_cheats() & CHEAT_MIRRORED_TRACKS) ? -width : width;
    Vp *vp;
#ifndef NO_ANTIPIRACY
    // Antipiracy measure. Flips the screen upside down.
    if (gAntiPiracyViewport) {
        height = -height;
        tempWidth = -width;
    }
#endif
    if (!(gScreenViewports[gActiveCameraID].flags & VIEWPORT_EXTRA_BG)) {
        gViewportStack[gActiveCameraID].vp.vtrans[0] = posX * 4;
        gViewportStack[gActiveCameraID].vp.vtrans[1] = posY * 4;
        gViewportStack[gActiveCameraID].vp.vscale[0] = tempWidth * 4;
        gViewportStack[gActiveCameraID].vp.vscale[1] = height * 4;
        vp = &gViewportStack[gActiveCameraID];
    } else {
        vp = &gViewportStack[gActiveCameraID + 10 + (gViewportWithBG * 5)];
    }
    gSPViewport((*dlist)++, OS_PHYSICAL_TO_K0(vp));
}

//Official Name: camResetView?
void func_800682AC(Gfx **dlist) {
    u32 width, height;
    gActiveCameraID = 4;
    height = gScreenHeight;
    width = gScreenWidth;
    if (!(gScreenViewports[gActiveCameraID].flags & VIEWPORT_EXTRA_BG)) {
        gDPSetScissor((*dlist)++, G_SC_NON_INTERLACE, 0, 0, width, height);
        func_80068158(dlist, width >> 1, height >> 1, width >> 1, height >> 1);
    } else {
        set_viewport_scissor(dlist);
        func_80068158(dlist, 0, 0, 0, 0);
    }
    gActiveCameraID = 0;
}

//Official Name: camOffsetZero?
void func_80068408(Gfx **dlist, MatrixS **mtx) {
    s32 i;
    for (i = 1; i < 14; i++) {
        (*gModelMatrixF)[gModelMatrixStackPos][0][i] = 0.0f;
    }
    for (i = 0; i < 4; i++) {
        (*gModelMatrixF)[gModelMatrixStackPos][i][i] = 1.0f;
    }
    f32_matrix_mult(gModelMatrixF[gModelMatrixStackPos], &gViewMatrixF, &gCurrentModelMatrixF);
    f32_matrix_to_s16_matrix(&gCurrentModelMatrixF, *mtx);
    gModelMatrixS[gModelMatrixStackPos] = *mtx;
    gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), gMatrixType << 6);
}

void func_80068508(s32 arg0) {
    D_80120D0C = arg0;
}

/**
 * Calculates angle from object to camera, then renders the sprite as a billboard, facing the camera.
 */
s32 render_sprite_billboard(Gfx **dlist, MatrixS **mtx, Vertex **vertexList, Object *obj, unk80068514_arg4 *arg4, s32 flags) {
    f32 diffX;
    f32 diffY;
    Vertex *v;
    f32 lateralDist;
    f32 sineY;
    f32 cosY;
    f32 sp44;
    f32 diffZ;
    s32 tanX;
    s32 tanY;
    s32 angleDiff;
    s32 result;
    s32 textureFrame;
    profiler_begin_timer();

    result = TRUE;
    if (flags & RENDER_VEHICLE_PART) {
        diffX = gModelMatrixViewX[gCameraMatrixPos] - obj->segment.trans.x_position;
        diffY = gModelMatrixViewY[gCameraMatrixPos] - obj->segment.trans.y_position;
        diffZ = gModelMatrixViewZ[gCameraMatrixPos] - obj->segment.trans.z_position;
        sineY = sins_f(obj->segment.trans.y_rotation);
        cosY = coss_f(obj->segment.trans.y_rotation);
        sp44 = (diffX * cosY) + (diffZ * sineY);
        diffZ = (diffZ * cosY) - (diffX * sineY);
        tanY = arctan2_f(sp44, sqrtf((diffY * diffY) + (diffZ * diffZ)));
        tanX = -sins(arctan2_f(sp44, diffZ)) >> 8;
        if (diffZ < 0.0f) {
            diffZ = -diffZ;
            tanX = 1 - tanX;
            tanY = -tanY;
        }
        angleDiff = arctan2_f(diffY, diffZ);
        if (angleDiff > 0x8000) {
            angleDiff -= 0x10000;
        }
        angleDiff = (angleDiff * tanX) >> 8;
        textureFrame = (tanY >> 7) & 0xFF;
        if (textureFrame > 127) {
            textureFrame = 255 - textureFrame;
            angleDiff += 0x8000;
            result = FALSE;
        }
        textureFrame *= 2;
        diffX = gModelMatrixViewX[gCameraMatrixPos] - obj->segment.trans.x_position;
        diffY = gModelMatrixViewY[gCameraMatrixPos] - obj->segment.trans.y_position;
        diffZ = gModelMatrixViewZ[gCameraMatrixPos] - obj->segment.trans.z_position;
        lateralDist = sqrtf((diffX * diffX) + (diffZ * diffZ));
        gCameraTransform.y_rotation = arctan2_f(diffX, diffZ);
        gCameraTransform.x_rotation = -arctan2_f(diffY, lateralDist);
        gCameraTransform.z_rotation = angleDiff;
        gCameraTransform.scale = obj->segment.trans.scale;
        gCameraTransform.x_position = obj->segment.trans.x_position;
        gCameraTransform.y_position = obj->segment.trans.y_position;
        gCameraTransform.z_position = obj->segment.trans.z_position;
        object_transform_to_matrix(gCurrentModelMatrixF, &gCameraTransform);
        gModelMatrixStackPos++;
        f32_matrix_mult(&gCurrentModelMatrixF, gModelMatrixF[gModelMatrixStackPos-1], gModelMatrixF[gModelMatrixStackPos]);
        f32_matrix_mult(gModelMatrixF[gModelMatrixStackPos], &gViewMatrixF, &gCurrentModelMatrixF);
        f32_matrix_to_s16_matrix(&gCurrentModelMatrixF, *mtx);
        gModelMatrixS[gModelMatrixStackPos] = *mtx;
        gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_2);
        gSPVertexDKR((*dlist)++, OS_K0_TO_PHYSICAL(&gVehiclePartVertex), 1, 0);
    } else {
        v = *vertexList;
        v->x = obj->segment.trans.x_position;
        v->y = obj->segment.trans.y_position;
        v->z = obj->segment.trans.z_position;
        v->r = 255;
        v->g = 255;
        v->b = 255;
        v->a = 255;
        gSPVertexDKR((*dlist)++, OS_PHYSICAL_TO_K0(*vertexList), 1, 0);
        (*vertexList)++;
        if (gCutsceneCameraActive == 0) {
            angleDiff = gCameraSegment[gActiveCameraID].trans.z_rotation + obj->segment.trans.z_rotation;
        } else {
            angleDiff = gCameraSegment[gActiveCameraID + 4].trans.z_rotation + obj->segment.trans.z_rotation;
        }
        textureFrame = obj->segment.animFrame;
        gModelMatrixStackPos++;
        f32_matrix_from_rotation_and_scale((f32 (*)[4]) gModelMatrixF[gModelMatrixStackPos], angleDiff, obj->segment.trans.scale, gVideoAspectRatio);
        f32_matrix_to_s16_matrix(gModelMatrixF[gModelMatrixStackPos], *mtx);
        gModelMatrixS[gModelMatrixStackPos] = *mtx;
        gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_2);
        gDkrEnableBillboard((*dlist)++);
    }
    if (D_80120D0C == 0) {
        textureFrame = ((textureFrame & 0xFF) * arg4->textureCount) >> 8;
    }
    flags &= ~RENDER_VEHICLE_PART;
    if (flags & RENDER_SEMI_TRANSPARENT) {
        flags |= RENDER_ANTI_ALIASING;
    }
    func_8007BF34(dlist, arg4->unk6 | (flags & (RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE | RENDER_ANTI_ALIASING)));
    if (!(flags & RENDER_Z_UPDATE)) {
        gDPSetPrimColor((*dlist)++, 0, 0, 255, 255, 255, 255);
    }
    gSPDisplayList((*dlist)++, arg4->unk8[textureFrame + 1]);
    gModelMatrixStackPos--;
    if (gModelMatrixStackPos == 0) {
        textureFrame = 0;
    } else {
        textureFrame = 1;
    }
    gDkrInsertMatrix((*dlist)++, 0, textureFrame << 6);
    gDkrDisableBillboard((*dlist)++);
    profiler_add(PP_BILLBOARD, first);
    return result;
}

void f32_matrix_from_scale(Matrix *mtx, f32 scaleX, f32 scaleY, f32 scaleZ) {
    s32 i;
    for (i = 1; i < 14; i++) {
        (*mtx)[0][i] = 0.0f;
    }
    (*mtx)[0][0] = scaleX;
    (*mtx)[1][1] = scaleY;
    (*mtx)[2][2] = scaleZ;
    (*mtx)[3][3] = 1.0f;
}

/**
 * Sets transform and scale matrices to set position and size, loads the texture, sets the rendermodes, then draws the result onscreen.
*/
void render_ortho_triangle_image(Gfx **dList, MatrixS **mtx, Vertex **vtx, ObjectSegment *segment, Sprite *sprite, s32 flags) {
    f32 scale;
    s32 index;
    Vertex *temp_v1;
    Matrix aspectMtxF;
    Matrix scaleMtxF;

    if (sprite != NULL) {
        temp_v1 = *vtx;
        temp_v1->x = segment->trans.x_position;
        temp_v1->y = segment->trans.y_position;
        temp_v1->z = segment->trans.z_position;
        temp_v1->r = 255;
        temp_v1->g = 255;
        temp_v1->b = 255;
        temp_v1->a = 255;
        gSPVertexDKR((*dList)++, OS_PHYSICAL_TO_K0(*vtx), 1, 0);
        (*vtx)++; // Can't be done in the macro?
        index = segment->animFrame;
        gModelMatrixStackPos ++;
        gCameraTransform.y_rotation = -segment->trans.y_rotation;
        gCameraTransform.x_rotation = -segment->trans.x_rotation;
        gCameraTransform.z_rotation = gCameraSegment[gActiveCameraID].trans.z_rotation + segment->trans.z_rotation;
        gCameraTransform.x_position = 0.0f;
        gCameraTransform.y_position = 0.0f;
        gCameraTransform.z_position = 0.0f;
        if (gAdjustViewportHeight) {
            scale = segment->trans.scale;
            f32_matrix_from_scale(&scaleMtxF, scale, scale, 1.0f);
            f32_matrix_from_rotation_and_scale(aspectMtxF, 0, 1.0f, gVideoAspectRatio);
            f32_matrix_mult(&aspectMtxF, &scaleMtxF, &gCurrentModelMatrixF);
        } else {
            scale = segment->trans.scale;
            f32_matrix_from_scale(&gCurrentModelMatrixF, scale, scale, 1.0f);
        }
        object_transform_to_matrix_2(aspectMtxF, &gCameraTransform);
        f32_matrix_mult(&gCurrentModelMatrixF, &aspectMtxF, gModelMatrixF[gModelMatrixStackPos]);
        f32_matrix_to_s16_matrix(gModelMatrixF[gModelMatrixStackPos], *mtx);
        gModelMatrixS[gModelMatrixStackPos] = *mtx;
        gSPMatrix((*dList)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_2);
        gDkrEnableBillboard((*dList)++);
        if (D_80120D0C == FALSE) {
            index =  (((u8) index) * sprite->baseTextureId) >> 8;
        }
        func_8007BF34(dList, sprite->unk6 | flags);
        if (index >= sprite->baseTextureId) {
            index = sprite->baseTextureId - 1;
        }
        gSPDisplayList((*dList)++, sprite->unkC.ptr[index]);
        if (--gModelMatrixStackPos == 0) {
            index = 0;
        } else {
            index = 1;
        }
        gDkrInsertMatrix((*dList)++, 0, index << 6);
        gDkrDisableBillboard((*dList)++);
    }
}

/**
 * Generate a matrix with rotation, scaling and shearing and run it.
 * Used for wavy type effects like the shield.
*/
void apply_object_shear_matrix(Gfx **dList, MatrixS **mtx, Object *arg2, Object *arg3, f32 shear) {
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
    Matrix matrix_mult;

    cossf_x_arg2 = coss_f(arg2->segment.trans.x_rotation);
    sinsf_x_arg2 = sins_f(arg2->segment.trans.x_rotation);
    cossf_y_arg2 = coss_f(arg2->segment.trans.y_rotation);
    sinsf_y_arg2 = sins_f(arg2->segment.trans.y_rotation);
    arg2_xPos = arg2->segment.trans.x_position;
    arg2_yPos = arg2->segment.trans.y_position;
    arg2_zPos = arg2->segment.trans.z_position;
    cossf_z_arg3 = coss_f(arg3->segment.trans.z_rotation);
    sinsf_z_arg3 = sins_f(arg3->segment.trans.z_rotation);
    cossf_x_arg3 = coss_f(arg3->segment.trans.x_rotation);
    sinsf_x_arg3 = sins_f(arg3->segment.trans.x_rotation);
    cossf_y_arg3 = coss_f(arg3->segment.trans.y_rotation);
    sinsf_y_arg3 = sins_f(arg3->segment.trans.y_rotation);
    arg3_xPos = arg3->segment.trans.x_position;
    arg3_yPos = arg3->segment.trans.y_position;
    arg3_zPos = arg3->segment.trans.z_position;
    arg2_scale = arg2->segment.trans.scale;
    shear *= arg2_scale;
    matrix_mult[0][0] = ((((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * cossf_y_arg2) + (-sinsf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))) * arg2_scale;
    matrix_mult[0][1] = (((sinsf_z_arg3 * cossf_x_arg3) * cossf_y_arg2) + (-sinsf_y_arg2 * -sinsf_x_arg3)) * arg2_scale;
    matrix_mult[0][2] = ((((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * cossf_y_arg2) + (-sinsf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))) * arg2_scale;
    matrix_mult[0][3] = 0.0f;
    matrix_mult[1][0] = ((((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * cossf_x_arg2) + (sinsf_x_arg2 * ((sinsf_y_arg2 * ((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) + (cossf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))))) * shear;
    matrix_mult[1][1] = (((cossf_z_arg3 * cossf_x_arg3) * cossf_x_arg2) + (sinsf_x_arg2 * ((sinsf_y_arg2 * (sinsf_z_arg3 * cossf_x_arg3)) + (cossf_y_arg2 * -sinsf_x_arg3)))) * shear;
    matrix_mult[1][2] = ((((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * cossf_x_arg2) + (sinsf_x_arg2 * ((sinsf_y_arg2 * ((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) + (cossf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))))) * shear;
    matrix_mult[1][3] = 0.0f;
    matrix_mult[2][0] = ((-sinsf_x_arg2 * ((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) + (cossf_x_arg2 * ((sinsf_y_arg2 * ((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) + (cossf_y_arg2 * (cossf_x_arg3 * sinsf_y_arg3))))) * arg2_scale;
    matrix_mult[2][1] = ((-sinsf_x_arg2 * (cossf_z_arg3 * cossf_x_arg3)) + (cossf_x_arg2 * ((sinsf_y_arg2 * (sinsf_z_arg3 * cossf_x_arg3)) + (cossf_y_arg2 * -sinsf_x_arg3)))) * arg2_scale;
    matrix_mult[2][2] = ((-sinsf_x_arg2 * ((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) + (cossf_x_arg2 * ((sinsf_y_arg2 * ((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) + (cossf_y_arg2 * (cossf_x_arg3 * cossf_y_arg3))))) * arg2_scale;
    matrix_mult[2][3] = 0.0f;
    matrix_mult[3][0] = (((cossf_z_arg3 * cossf_y_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3))) * arg2_xPos) + (arg2_yPos * ((-sinsf_z_arg3 * cossf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * sinsf_y_arg3)))) + (arg2_zPos * (cossf_x_arg3 * sinsf_y_arg3)) + arg3_xPos;
    matrix_mult[3][1] = ((sinsf_z_arg3 * cossf_x_arg3) * arg2_xPos) + (arg2_yPos * (cossf_z_arg3 * cossf_x_arg3)) + (arg2_zPos * -sinsf_x_arg3) + arg3_yPos;
    matrix_mult[3][2] = (((-sinsf_y_arg3 * cossf_z_arg3) + (sinsf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3))) * arg2_xPos) + (arg2_yPos * ((-sinsf_z_arg3 * -sinsf_y_arg3) + (cossf_z_arg3 * (sinsf_x_arg3 * cossf_y_arg3)))) + (arg2_zPos * (cossf_x_arg3 * cossf_y_arg3)) + arg3_zPos;
    matrix_mult[3][3] = 1.0f;

    f32_matrix_mult(&matrix_mult, &gViewMatrixF, &D_801210A0);
    f32_matrix_to_s16_matrix(&D_801210A0, *mtx);
    gSPMatrix((*dList)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_1);
}

/**
 * Official Name: camPushModelMtx
*/
void camera_push_model_mtx(Gfx **dList, MatrixS **mtx, ObjectTransform *trans, f32 scale, f32 scaleY) {
    f32 tempX;
    f32 tempY;
    f32 tempZ;
    s32 index;
    f32 scaleFactor;

    object_transform_to_matrix(gCurrentModelMatrixF, trans);
    if (scaleY != 0.0f) {
        gCurrentModelMatrixF[3][0] += gCurrentModelMatrixF[1][0] * scaleY;
        gCurrentModelMatrixF[3][1] += gCurrentModelMatrixF[1][1] * scaleY;
        gCurrentModelMatrixF[3][2] += gCurrentModelMatrixF[1][2] * scaleY;
    }
    if (scale != 1.0f) {
        gCurrentModelMatrixF[1][0] *= scaleY;
        gCurrentModelMatrixF[1][1] *= scaleY;
        gCurrentModelMatrixF[1][2] *= scaleY;
    }
    f32_matrix_mult(&gCurrentModelMatrixF, gModelMatrixF[gModelMatrixStackPos], gModelMatrixF[gModelMatrixStackPos + 1]);
    f32_matrix_mult(gModelMatrixF[gModelMatrixStackPos + 1], &gViewMatrixF, &D_801210A0);
    f32_matrix_to_s16_matrix(&D_801210A0, *mtx);
    gModelMatrixStackPos++;
    gModelMatrixS[gModelMatrixStackPos] = *mtx;
    gSPMatrix((*dList)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_1);
    guMtxXFMF(*gModelMatrixF[gModelMatrixStackPos], 0.0f, 0.0f, 0.0f, &tempX, &tempY, &tempZ);
    index = gActiveCameraID;
    if (gCutsceneCameraActive) {
        index += 4;
    }
    tempX = gCameraSegment[index].trans.x_position - tempX;
    tempY = gCameraSegment[index].trans.y_position - tempY;
    tempZ = gCameraSegment[index].trans.z_position - tempZ;
    gCameraTransform.y_rotation = -trans->y_rotation;
    gCameraTransform.x_rotation = -trans->x_rotation;
    gCameraTransform.z_rotation = -trans->z_rotation;
    gCameraTransform.x_position = 0.0f;
    gCameraTransform.y_position = 0.0f;
    gCameraTransform.z_position = 0.0f;
    gCameraTransform.scale = 1.0f;
    object_transform_to_matrix_2(gCurrentModelMatrixF, &gCameraTransform);
    guMtxXFMF(gCurrentModelMatrixF, tempX, tempY, tempZ, &tempX, &tempY, &tempZ);
    scaleFactor = 1.0f / trans->scale;
    tempX *= scaleFactor;
    tempY *= scaleFactor;
    tempZ *= scaleFactor;
    gCameraMatrixPos++;
    index = gCameraMatrixPos;
    gModelMatrixViewX[index] = tempX;
    gModelMatrixViewY[index] = tempY;
    gModelMatrixViewZ[index] = tempZ;
    if (0) {} // Fakematch
}

/**
 * Calculate the rotation matrix for an actors head, then run it.
*/
void apply_head_turning_matrix(Gfx **dlist, MatrixS **mtx, Object_68 *objGfx, s16 headAngle) {
    f32 coss_headAngle;
    f32 sins_headAngle;
    f32 f_unk16;
    f32 f_unk18;
    f32 f_unk1A;
    f32 coss_unk1C;
    f32 sins_unk1C;
    Matrix rotationMtxF;
    Matrix headMtxF;

    f_unk16 = (f32) objGfx->unk16;
    f_unk18 = (f32) objGfx->unk18;
    f_unk1A = (f32) objGfx->unk1A;
    coss_unk1C = coss_f(objGfx->unk1C);
    sins_unk1C = sins_f(objGfx->unk1C);
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
    headMtxF[3][0] = (-f_unk16 * (coss_headAngle * coss_unk1C)) + (-f_unk18 * -sins_unk1C) + (-f_unk1A * (sins_headAngle * coss_unk1C)) + f_unk16;
    headMtxF[3][1] = (-f_unk16 * (coss_headAngle * sins_unk1C)) + (-f_unk18 * coss_unk1C) + (-f_unk1A * (sins_headAngle * sins_unk1C)) + f_unk18;
    headMtxF[3][2] = (-f_unk16 * -sins_headAngle) + (-f_unk1A * coss_headAngle) + f_unk1A;
    headMtxF[3][3] = 1.0f;
    f32_matrix_mult(&headMtxF, &D_801210A0, &rotationMtxF);
    f32_matrix_to_s16_matrix(&rotationMtxF, *mtx);
    gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0((*mtx)++), G_MTX_DKR_INDEX_2);
    gDkrInsertMatrix((*dlist)++, G_MWO_MATRIX_XX_XY_I, G_MTX_DKR_INDEX_1);
}

/**
 * Run a matrix from the top of the stack and pop it.
 * If the stack pos is less than zero, add a matrix instead.
*/
void apply_matrix_from_stack(Gfx **dlist) {
    gCameraMatrixPos--;
    gModelMatrixStackPos--;

    if (gModelMatrixStackPos > 0) {
        gSPMatrix((*dlist)++, OS_PHYSICAL_TO_K0(gModelMatrixS[gModelMatrixStackPos]), G_MTX_DKR_INDEX_1);
    } else {
        gDkrInsertMatrix((*dlist)++, G_MWO_MATRIX_XX_XY_I, G_MTX_DKR_INDEX_0);
    }
}

/**
 * Returns the segment data of the active camera, but won't apply the offset for cutscenes.
*/
ObjectSegment *get_active_camera_segment_no_cutscenes(void) {
    return &gCameraSegment[gActiveCameraID];
}

/**
 * Returns the segment data of the active camera.
*/
ObjectSegment *get_active_camera_segment(void) {
    if (gCutsceneCameraActive) {
        return &gCameraSegment[gActiveCameraID + 4];
    }
    return &gCameraSegment[gActiveCameraID];
}

/**
 * Returns the segment data of the active cutscene camera.
 * If no cutscene is active, return player 1's camera.
*/
ObjectSegment *get_cutscene_camera_segment(void) {
    if (gCutsceneCameraActive) {
        return &gCameraSegment[4];
    }
    return &gCameraSegment[PLAYER_ONE];
}

/**
 * Return the current floating point projection matrix.
*/
Matrix *get_projection_matrix_f32(void) {
    return &gProjectionMatrixF;
}

/**
 * Return the current fixed point projection matrix.
*/
MatrixS *get_projection_matrix_s16(void) {
    return &gProjectionMatrixS;
}

/**
 * Return the current camera matrix.
*/
Matrix *get_camera_matrix(void) {
    return &gCameraMatrixF;
}

/**
 * Return the screenspace distance to the camera.
*/
f32 get_distance_to_camera(f32 x, f32 y, f32 z) {
    f32 ox, oy, oz;

    guMtxXFMF(gCameraMatrixF, x, y, z, &ox, &oy, &oz);

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
    dist *= dist;

    for (i = 0; i <= gNumberOfViewports; i++) {
        diffX = x - gCameraSegment[i].trans.x_position;
        diffY = y - gCameraSegment[i].trans.y_position;
        diffZ = z - gCameraSegment[i].trans.z_position;
        distance = (((diffX * diffX) + (diffY * diffY)) + (diffZ * diffZ));
        if (distance < dist) {
            gCameraSegment[i].camera.distanceToCamera = ((dist - distance) * magnitude) / dist;
        }
    }
}

/**
 * Apply a shake to all active cameras.
*/
void set_camera_shake(f32 magnitude) {
    s32 i;
    for (i = 0; i <= gNumberOfViewports; i++) {
        gCameraSegment[i].camera.distanceToCamera = magnitude;
    }
}