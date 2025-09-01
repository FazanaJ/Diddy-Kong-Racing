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
#include "src/textures_sprites.h"
#include "src/object_functions.h"
#include "src/object_models.h"
#include "src/objects.h"
#include "src/asset_loading.h"
#include "src/tracks.h"
#include "src/math_util.h"
#include "src/gzip.h"
#include "src/memory.h"
#include "src/thread0_epc.h"

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
extern Mtx *sMenuCurrHudMat;
extern s32 gPauseOptionScroll;
extern Vertex *sMenuCurrHudVerts;
extern FadeTransition sMenuTransitionFadeOut;
extern s32 gMenuOptionCap;
extern s8 gControllersXAxis[4];
extern s8 gControllersYAxis[4];

s32 currentModelIndex = -1;
s32 actionDelay = 0;
ObjectTransform viewModelTransform;
s32 sDebugModelViewPitch;
f32 scale = 1.0f;
f32 targetMaxLength = 74.0f;
f32 offsetY = 0; // Used to center the model vertically.
s32 numberOfAnimations = 0;
s32 animationID = 0;
s32 animationFrame = 0;
s32 animationFrameCount = 0;
s32 animResult = -1;
u16 gDebugModelTris;
u16 gDebugModelVtx;
u16 gDebugModelTex;
u16 gDebugModelSizeMdl;
u16 gDebugModelSizeAnim;
u16 gDebugModelSizeTex;
ModelInstance *viewModel = NULL;
Object fakeObjectForModel;
ObjectHeader fakeObjectHeaderForModel;
ShadeProperties gDebugModelShading;
s16 gDebugModelHeadAngle;
s16 gDebugModelHeadAngleTarget;
s8 gDebugModelMiscCounter;
s8 gDebugModelWheelFrame;
s8 gDebugModelPropellorFrame;
Sprite *testSprite[8];
ObjectTransform fakeSpritetrans;
Mtx spriteMatrix; // Unsure of needed length.
Vertex spriteVertex; // Unsure of needed length.

void init_fake_sprite() {
    testSprite[0] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_TYRE0, 0);
    testSprite[1] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_TYRE1, 0);
    testSprite[2] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_TYRE2, 0);
    testSprite[3] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_TYRE3, 0);
    testSprite[4] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_PROPELLER0, 0);
    testSprite[5] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_PROPELLER1, 0);
    testSprite[6] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_FAN0, 0);
    testSprite[7] = tex_load_sprite(ASSET_SPRITE_VEHICLE_PARTS_FAN1, 0);
    fakeSpritetrans.scale = 2.66f;
    gDebugModelWheelFrame = 0;
    gDebugModelPropellorFrame = 0;
}

// 0: wheel0
// 1: wheel1
// 2: wheel2
// 3: wheel3
// 4: propellor0
// 5: propellor1
// 6: fan0
// 7: fan1

void render_test_sprite(f32 x, f32 y, f32 z, s32 spriteID) {
    s32 frame;

    if (spriteID > 3) {
        frame = spriteID + gDebugModelPropellorFrame;
        fakeSpritetrans.rotation.y_rotation = -0x4000;
    } else {
        frame = spriteID + gDebugModelWheelFrame;
        fakeSpritetrans.rotation.y_rotation = 0;
    }

    fakeSpritetrans.x_position = x;
    fakeSpritetrans.y_position = y;
    fakeSpritetrans.z_position = z;
    render_sprite_billboard(&sMenuCurrDisplayList, &sMenuCurrHudMat, &sMenuCurrHudVerts, (Object*)&fakeObjectForModel, testSprite[frame], RENDER_Z_COMPARE | RENDER_FOG_ACTIVE | RENDER_Z_UPDATE | RENDER_VEHICLE_PART);
}

void menu_debug_root_init(void) {
    gOptionBlinkTimer = 0;
    gMenuDelay = 0;
    gPauseSubmenu = 0;
    gMenuOption = 1;
    gPauseOptionScroll = 0;
    viewModelTransform.rotation.y_rotation = -0x5000;
    transition_begin(&sMenuTransitionFadeOut);
    music_voicelimit_set(24);
    music_play(SEQUENCE_MAIN_MENU);
}

char *gDebugMenuSubStrings[] = {
    "DEBUG MENU",
    "MODEL VIEWER",
    "SPRITE VIEWER",
    "LEVEL VIEWER",
    "UNLOAD EVERYTHING",
    "BACK",
};

void debugmenu_root(UNUSED s32 updateRate, s32 input) {
    s32 i;
    s32 y;
    s32 alpha;
    s32 al;
    s32 prevOpt;

    alpha = gOptionBlinkTimer * 8;
    if (alpha > 255) {
        alpha = 511 - alpha;
    }

    y = 76;
    gMenuOptionCap = ARRAY_COUNT(gDebugMenuSubStrings) - 1;
    prevOpt = gMenuOption;
    
    if (gMenuStickY[PLAYER_MENU] < 0 && gMenuOption < ARRAY_COUNT(gDebugMenuSubStrings) - 1) {
        gMenuOption++;
    }
    if (gMenuStickY[PLAYER_MENU] > 0 && gMenuOption > 1) {
        gMenuOption--;
    }

    if (prevOpt != gMenuOption) {
        sound_play(SOUND_MENU_PICK2, NULL);
    }

    if (input & A_BUTTON) {
        sound_play(SOUND_SELECT2, NULL);
        if (gMenuOption == gMenuOptionCap) {
            menu_init(MENU_TITLE);
        } else {
            gPauseSubmenu = gMenuOption;
            gMenuOption = 0;
            gPauseOptionScroll = 0;
        }
    }

    set_text_font(ASSET_FONTS_FUNFONT);
    for (i = 1; i < ARRAY_COUNT(gDebugMenuSubStrings); i++) {
        if (i == gMenuOption) {
            al = alpha;
        } else {
            al = 0;
        }
        set_text_colour(255, 255, 255, al, 255);
        draw_text(&sMenuCurrDisplayList, SCREEN_WIDTH_HALF, y, gDebugMenuSubStrings[i], ALIGN_MIDDLE_CENTER);
        y += 18;
    }
}

void animate_model(s32 updateRate) {
    if(numberOfAnimations < 1) {
        animationFrame = 0;
        return;
    }
    
    
    fakeObjectForModel.animationID = animationID;
    fakeObjectForModel.animFrame = animationFrame;
    
    obj_seek_anim(&fakeObjectForModel, fakeObjectForModel.modelInstances[0]);

    animationFrame += updateRate;
    animationFrameCount = (fakeObjectForModel.modelInstances[0]->objModel->animations[animationID].animLength - 1) * 16;

    if(animationFrame >= animationFrameCount) {
        animationFrame = 0;
    } else if(animationFrame < 0) {
        animationFrame = animationFrameCount - 1;
    }
    
}

void set_animation(int animIndex) {
    
    if((viewModel == NULL) || (viewModel->objModel == NULL)) {
        return;
    }
    
    if(numberOfAnimations < 1) {
        animationFrame = 0;
        animationFrameCount = 0;
        animationID = 0;
        return;
    }
    
    animationID = animIndex;
    animationFrame = 0;
    
    if(animationID >= numberOfAnimations) {
        animationID = 0;
    } else if(animationID < 0) {
        animationID = numberOfAnimations - 1;
    }
    
}

s32 prevID = -1;

void debugmodel_shade(ObjectModel *model, Object *object, s32 arg2, f32 intensity) {
    s16 environmentMappingEnabled;
    s32 dynamicLightingEnabled;
    s16 i;

    dynamicLightingEnabled = 0;
    environmentMappingEnabled = 0;

    for (i = 0; i < model->numberOfBatches; i++) {
        if (model->batches[i].miscData != 0xFF) {
            dynamicLightingEnabled = -1; // This is a bit weird, but I guess it works.
        }
        if (model->batches[i].flags & RENDER_ENVMAP) {
            environmentMappingEnabled = -1;
        }
    }

    if (dynamicLightingEnabled && model->normals != NULL) {
        calc_dynamic_lighting_for_object_2(&fakeObjectForModel, model, arg2, intensity);
    }

    if (environmentMappingEnabled) {
        // Calculates environment mapping for the object
        calc_env_mapping_for_object(model, object->trans.rotation.z_rotation,
                                    object->trans.rotation.x_rotation,
                                    object->trans.rotation.y_rotation);
    }
}

void render_model(s32 updateRate) {
    TextureHeader *tex;
    Triangle *tris;
    s32 triOffset;
    s32 vertOffset;
    Vertex *verts;
    s32 numVerts;
    s32 numTris;
    s32 texOffset;
    s32 texEnabled;
    s32 i, mode;
    ObjectModel *model;
    s32 isTexTransparent;
    Vertex *currentVertices;
    ObjectTransform tempForm;
    s32 renderFlags;
    s32 shouldShade;
    s32 offset;
    
    if((viewModel == NULL) || (viewModel->objModel == NULL)) {
        return;
    }
    
    model = viewModel->objModel;
    
    if(model->unk4A > 0) {
        animate_model(updateRate);
        currentVertices = viewModel->vertices[viewModel->animationTaskNum];
    } else {
        currentVertices = model->vertices;
    }

    fakeObjectForModel.curVertData = currentVertices;
    fakeObjectForModel.trans.rotation.y_rotation = viewModelTransform.rotation.y_rotation;
    fakeObjectForModel.trans.rotation.x_rotation = viewModelTransform.rotation.x_rotation;
    fakeObjectForModel.trans.rotation.z_rotation = sDebugModelViewPitch;
    gDebugModelMiscCounter += updateRate;

    if (gDebugModelMiscCounter > 90) {
        gDebugModelMiscCounter = 0;
        gDebugModelHeadAngleTarget = rand_range(-0x2000, 0x2000);
    }
    
    gDPSetPrimColor(sMenuCurrDisplayList++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(sMenuCurrDisplayList++, 255, 255, 255, 0);
    mtx_world_origin(&sMenuCurrDisplayList, &sMenuCurrHudMat);
    viewModelTransform.rotation.x_rotation = 0;
    bzero(&tempForm, sizeof(ObjectTransform));
    tempForm.rotation.z_rotation = sDebugModelViewPitch;
    tempForm.scale = 1.25f;
    tempForm.x_position = 200.0f;
    tempForm.y_position = -175.0f - offsetY;
    tempForm.z_position = 30.0f;
    mtx_cam_push(&sMenuCurrDisplayList, &sMenuCurrHudMat, &tempForm, 1.0f, 0);
    mtx_cam_push(&sMenuCurrDisplayList, &sMenuCurrHudMat, &viewModelTransform, 1.0f, 0);

    
    if (animationID == 0 || currentModelIndex < ASSET_OBJECTMODEL_DIDDYCAR_0 || currentModelIndex > ASSET_OBJECTMODEL_MOUSEPLANE_5) {
        if (currentModelIndex >= ASSET_OBJECTMODEL_DIDDYCAR_0 && currentModelIndex <= ASSET_OBJECTMODEL_MOUSEPLANE_5) {
            animationFrame = 40;
        }
        gDebugModelHeadAngle += ((gDebugModelHeadAngleTarget - gDebugModelHeadAngle) * updateRate) >> 3;
        mtx_head_push(&sMenuCurrDisplayList, &sMenuCurrHudMat, viewModel, gDebugModelHeadAngle);
        offset = TRUE;
    } else {
        offset = FALSE;
    }

    for (i = 0; model->unk50 > 0 && i < model->numberOfBatches; i++) {
        s32 sp5C;
        if (model->batches[i].flags & RENDER_TEX_ANIM) {
            if (model->batches[i].textureIndex != TEX_INDEX_NO_TEXTURE) {
                tex = model->textures[model->batches[i].textureIndex].texture;
                sp5C = model->batches[i].texOffset;
                sp5C <<= 6;
                tex_animate_texture(tex, &model->batches[i].flags, &sp5C, updateRate);
                model->batches[i].texOffset = (sp5C >> 6) & 0xFF;
            }
        }
    }

    shouldShade = FALSE;
    if (viewModel->modelType != MODELTYPE_BASIC) {
        shouldShade = TRUE;
    } else {
        for (i = 0; i < model->numberOfBatches; i++) {
            if (model->batches[i].flags & RENDER_ENVMAP) {
                shouldShade = TRUE;
                break;
            }
        }
    }

    if (shouldShade) {
        debugmodel_shade(model, (Object *) &viewModelTransform, -1, 1.0f);
    }
    
    // Mode 0 is opaque geometry, Mode 1 is transparent geometry.
    for(mode = 0; mode < 2; mode++) {
        for (i = 0; i < model->numberOfBatches; i++) {
            if (!(model->batches[i].flags & RENDER_Z_UPDATE)) {
                s32 isDecal;
                s32 vtxOff;
                vertOffset = model->batches[i].verticesOffset;
                triOffset = model->batches[i].facesOffset;
                numVerts = model->batches[i + 1].verticesOffset - vertOffset;
                numTris = model->batches[i + 1].facesOffset - triOffset;
                verts = &currentVertices[vertOffset]; //&model->vertices[vertOffset];
                tris = &model->triangles[triOffset];
                
                vtxOff = (offset) ? model->batches[i].vertOverride: numVerts;
                if (model->batches[i].textureIndex == 0xFF) {
                    tex = NULL;
                    texEnabled = FALSE;
                    texOffset = 0;
                } else {
                    tex = model->textures[model->batches[i].textureIndex].texture;
                    texEnabled = TRUE;
                    texOffset = model->batches[i].texOffset << 14;
                }
                
                isTexTransparent = tex != NULL && (TEX_RENDERMODE(tex->format) == 0 || model->batches[i].flags & RENDER_DECAL);

                if(((mode == 0) && isTexTransparent) || ((mode == 1) && !isTexTransparent)) {
                    continue;
                }
                
                isDecal = FALSE;
                if (mode == 1) {
                    if (model->batches[i].flags & RENDER_DECAL) {
                        isDecal = TRUE;
                    }
                }

                renderFlags = RENDER_ANTI_ALIASING | RENDER_Z_COMPARE | RENDER_Z_UPDATE;

                if (isDecal) {
                    renderFlags |= RENDER_DECAL;
                }

                if (mode == 0) {
                    renderFlags |= RENDER_Z_UPDATE;
                } else {
                    renderFlags |= RENDER_CUTOUT;
                }

                material_set(&sMenuCurrDisplayList, tex, renderFlags, texOffset);                

                if (vtxOff == numVerts) {
                    gSPVertexDKR(sMenuCurrDisplayList++, OS_K0_TO_PHYSICAL(verts), numVerts, 0);
                } else {
                    if (vtxOff > 0) {
                        gSPVertexDKR(sMenuCurrDisplayList++, OS_K0_TO_PHYSICAL(verts), vtxOff, 0);
                        gSPSelectMatrixDKR(sMenuCurrDisplayList++, G_MTX_DKR_INDEX_2);
                        gSPVertexDKR(sMenuCurrDisplayList++, OS_K0_TO_PHYSICAL(&verts[vtxOff]),
                                     (numVerts - vtxOff), 1);
                    } else {
                        gSPSelectMatrixDKR(sMenuCurrDisplayList++, G_MTX_DKR_INDEX_2);
                        gSPVertexDKR(sMenuCurrDisplayList++, OS_K0_TO_PHYSICAL(verts), numVerts, 0);
                    }
                    gSPSelectMatrixDKR(sMenuCurrDisplayList++, G_MTX_DKR_INDEX_1);
                }
                gSPPolygon(sMenuCurrDisplayList++, OS_PHYSICAL_TO_K0(tris), numTris, texEnabled);
            }
        }
    }

    gDebugModelWheelFrame++;
    if (gDebugModelWheelFrame > 3) {
        gDebugModelWheelFrame = 0;
    }
    gDebugModelPropellorFrame ^= 1;;

    if (model->unk18 > 0) {
        if (model->unk18 == 1) { // Hovercraft
            render_test_sprite(model->vertices[model->unk14[0]].x, model->vertices[model->unk14[0]].y, model->vertices[model->unk14[0]].z, 6);
        } else {
            Vertex *attachPoint[2];
            attachPoint[0] = &model->vertices[model->unk14[0]];
            attachPoint[1] = &model->vertices[model->unk14[2]];
            
            if (ABS(attachPoint[0]->y - attachPoint[1]->y) >= 8) { // Plane
                render_test_sprite(model->vertices[model->unk14[0]].x, model->vertices[model->unk14[0]].y, model->vertices[model->unk14[0]].z, 4);
                render_test_sprite(model->vertices[model->unk14[2]].x, model->vertices[model->unk14[2]].y, model->vertices[model->unk14[2]].z, 0);
                render_test_sprite(model->vertices[model->unk14[3]].x, model->vertices[model->unk14[3]].y, model->vertices[model->unk14[3]].z, 0);
            } else { // Car
                render_test_sprite(model->vertices[model->unk14[0]].x, model->vertices[model->unk14[0]].y, model->vertices[model->unk14[0]].z, 0);
                render_test_sprite(model->vertices[model->unk14[1]].x, model->vertices[model->unk14[1]].y, model->vertices[model->unk14[1]].z, 0);
                render_test_sprite(model->vertices[model->unk14[2]].x, model->vertices[model->unk14[2]].y, model->vertices[model->unk14[2]].z, 0);
                render_test_sprite(model->vertices[model->unk14[3]].x, model->vertices[model->unk14[3]].y, model->vertices[model->unk14[3]].z, 0);
            }
        }
    }

    // Render attach points using the debug red sphere sprite.
    /*for(i = 0; i < model->unk18; i++) {
        Vertex *attachPoint = &model->vertices[model->unk14[i]];
        render_test_sprite(attachPoint->x, attachPoint->y, attachPoint->z, 0);
    }*/

    if (prevID != currentModelIndex) {
        prevID = currentModelIndex;
    }
    
    mtx_pop(&sMenuCurrDisplayList);
    mtx_pop(&sMenuCurrDisplayList);
    gDPSetPrimColor(sMenuCurrDisplayList++, 0, 0, 255, 255, 255, 255);
}

void calculate_model_scale() {
    Vertex *vertices;
    s16 numberOfVertices;
    f32 minX, minY, minZ;
    f32 maxX, maxY, maxZ;
    f32 midPointY;
    f32 width, height, depth;
    f32 maxLength;
    s32 i;
    
    if((viewModel == NULL) || (viewModel->objModel == NULL)) {
    failedReturn:
        scale = 1.0f;
        return;
    }
    
    vertices = viewModel->objModel->vertices;
    numberOfVertices = viewModel->objModel->numberOfVertices;

    gDebugModelVtx = numberOfVertices;
    gDebugModelTris = viewModel->objModel->numberOfTriangles;
    gDebugModelTex = viewModel->objModel->numberOfTextures;
    
    if(numberOfVertices == 0) {
        goto failedReturn;
    }
    
    minX = maxX = vertices[0].x;
    minY = maxY = vertices[0].y;
    minZ = maxZ = vertices[0].z;
    
    for(i = 1; i < numberOfVertices; i++) {
        f32 x = vertices[i].x;
        f32 y = vertices[i].y;
        f32 z = vertices[i].z;
        
        if(x < minX) minX = x;
        if(x > maxX) maxX = x;
        if(y < minY) minY = y;
        if(y > maxY) maxY = y;
        if(z < minZ) minZ = z;
        if(z > maxZ) maxZ = z;
    }
    
    width = (maxX - minX);
    height = (maxY - minY);
    depth = (maxZ - minZ);
    
    maxLength = width;
    if(height>maxLength)maxLength = height;
    if(depth>maxLength)maxLength = depth;
    
    scale = targetMaxLength / maxLength;
    
    midPointY = minY + ((maxY - minY) / 2);
    offsetY = midPointY * scale;
}

void set_object_model(s32 modelId) {
    s32 offset;
    s32 size;
    s32 start;
    s32 end;
    s32 i;
    ObjectHeader *tempHeader;


    if(modelId < 0) {
        modelId = ASSET_OBJECT_MODELS_COUNT - 1;
    } else if (modelId == ASSET_OBJECT_MODELS_COUNT) {
        modelId = 0;
    } else if(modelId == currentModelIndex) {
        return;
    }
    
    currentModelIndex = modelId;
    gDebugModelMiscCounter = 0;
    
    if(viewModel != NULL) {
        free_3d_model(viewModel);
    }
    
    viewModel = object_model_init(modelId, OBJECT_BEHAVIOUR_ANIMATION);

    
    gDebugModelSizeTex = 0;
    gDebugModelSizeAnim = 0;
    assettable_seek_s32(modelId, &offset, &size, ASSET_OBJECT_MODELS_TABLE);
    gDebugModelSizeMdl = gzip_size_uncompressed(ASSET_OBJECT_MODELS, offset) + sizeof(ObjectModel);
    assettable_seek_s16(modelId, &start, &end, ASSET_ANIMATION_IDS);
    if (start != end) {
        do {
            assettable_seek_s32(modelId, &offset, &size, ASSET_OBJECT_ANIMATIONS_TABLE);
            gDebugModelSizeAnim += gzip_size_uncompressed(ASSET_OBJECT_ANIMATIONS, offset) + 0x80;
            start++;
        } while (start < end);
    }

    for (i = 0; i < viewModel->objModel->numberOfTextures; i++) {
        if (viewModel->objModel->textures[i].texture) {
            gDebugModelSizeTex += viewModel->objModel->textures[i].texture->textureSize;
        }
    }
    
    numberOfAnimations = viewModel->objModel->numberOfAnimations;

    calculate_model_scale();
    
    viewModelTransform.scale = scale;
    
    //viewModelTransform.x_position = 200.0f;
    //viewModelTransform.y_position = -175.0f - offsetY;
    //viewModelTransform.z_position = -10.0f;
    
    fakeObjectForModel.modelInstances = &viewModel;
    fakeObjectForModel.modelIndex = 0;

    tempHeader = load_object_header(ASSET_OBJECT_MOUSESELECT);
    fakeObjectForModel.header = tempHeader;
    init_object_shading(&fakeObjectForModel, &gDebugModelShading);
    try_free_object_header(ASSET_OBJECT_MOUSESELECT);
    fakeObjectForModel.header = &fakeObjectHeaderForModel;
    set_animation(0);
}

void debugmenu_model_viewer(s32 updateRate, s32 input) {
    char textBytes[64];
    u32 buttonsDown;
    s32 contX;
    s32 contY;
    s32 i;
    char *tagStr[] = {"B", "KB", "MB"};
    s32 tag;

    if (input & START_BUTTON) {
        for (i = 0; i < 8; i++) {
            mempool_free(testSprite);
        }
        gPauseSubmenu = 0;
        gPauseOptionScroll = 0;
        gMenuOption = 1;
    }

    if (gPauseOptionScroll == 0) {
        fakeObjectHeaderForModel.numberOfModelIds = 1;
        fakeObjectForModel.modelIndex = 0;
        fakeObjectForModel.header = &fakeObjectHeaderForModel;
        init_fake_sprite();
        sDebugModelViewPitch = -0x1000;
        
        set_object_model(0);
        gPauseOptionScroll = 1;
    }
    
    if(actionDelay < 1) {
        buttonsDown = input_held(0);
        contX = gControllersXAxis[0];
        contY = gControllersYAxis[0];
        
        if(contX > 0) contX = 1;
        else if(contX < 0) contX = -1;
        if(contY > 50) contY = 1;
        else if(contY < -50) contY = -1;
        
        if(buttonsDown & A_BUTTON) {
            set_object_model(currentModelIndex + 1);
            actionDelay = 10;
        } else if(buttonsDown & B_BUTTON) {
            set_object_model(currentModelIndex - 1);
            actionDelay = 10;
        } else if(buttonsDown & L_TRIG) {
            set_animation(animationID - 1);
            actionDelay = 10;
        } else if(buttonsDown & R_TRIG) {
            set_animation(animationID + 1);
            actionDelay = 10;
        }
        
        if(contX < 0) {
            viewModelTransform.rotation.y_rotation -= 0x200 * updateRate;
        } else if(contX > 0) {
            viewModelTransform.rotation.y_rotation += 0x200 * updateRate;
        }
        
        if(contY < 0) {
            sDebugModelViewPitch -= 0x200 * updateRate;
            if (sDebugModelViewPitch < -0x4000) {
                sDebugModelViewPitch = -0x4000;
            }
        } else if(contY > 0) {
            sDebugModelViewPitch += 0x200 * updateRate;

            if (sDebugModelViewPitch > 0x4000) {
                sDebugModelViewPitch = 0x4000;
            }
        }
    } else {
        contX = contY = buttonsDown = 0;
        actionDelay--;
    }
    
    render_model(updateRate);
    
    // Draw text 
    set_text_font(ASSET_FONTS_FUNFONT);
    set_text_colour(255, 255, 255, 0, 255);
    
    //yaw = ((f32)viewModelTransform.rotation.y_rotation / (f32)0x10000) * 360.0f;
    //pitch = ((f32)sDebugModelViewPitch / (f32)0x10000) * 360.0f;
    
    sprintf(textBytes, "%d: %s", currentModelIndex, assettable_name(ASSET_OBJECT_MODELS, currentModelIndex));
    draw_text(&sMenuCurrDisplayList, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 32, textBytes, ALIGN_TOP_CENTER);

    set_text_font(ASSET_FONTS_SMALLFONT);
    if (numberOfAnimations > 0) {
        sprintf(textBytes, "Anim: %d of %d", (animationID + 1), numberOfAnimations);
        set_text_colour(0, 0, 0, 255, 255);
        draw_text(&sMenuCurrDisplayList, 56 + 1, 76 + 1, textBytes, ALIGN_TOP_CENTER);
        set_text_colour(255, 255, 255, 0, 255);
        draw_text(&sMenuCurrDisplayList, 56, 76, textBytes, ALIGN_TOP_CENTER);

        sprintf(textBytes, "%d of %d (Key %d)", (animationFrame + 1), animationFrameCount, animationFrame >> 4);
        set_text_colour(0, 0, 0, 255, 255);
        draw_text(&sMenuCurrDisplayList, 56 + 1, 86 + 1, textBytes, ALIGN_TOP_CENTER);
        set_text_colour(255, 255, 255, 0, 255);
        draw_text(&sMenuCurrDisplayList, 56, 86, textBytes, ALIGN_TOP_CENTER);
    }
    
    sprintf(textBytes, "Tri: %d Vtx: %d", gDebugModelTris, gDebugModelVtx);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 96 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 96, textBytes, ALIGN_TOP_CENTER);
    
    sprintf(textBytes, "Materials: %d", gDebugModelTex);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 106 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 106, textBytes, ALIGN_TOP_CENTER);

    sprintf(textBytes, "Size");
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 116 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 116, textBytes, ALIGN_TOP_CENTER);

    sprintf(textBytes, "Mesh: %2.3f%s", memsize_float(gDebugModelSizeMdl, &tag), tagStr[tag]);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 126 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 126, textBytes, ALIGN_TOP_CENTER);
    
    sprintf(textBytes, "Anims: %2.3f%s", memsize_float(gDebugModelSizeAnim, &tag), tagStr[tag]);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 136 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 136, textBytes, ALIGN_TOP_CENTER);
    
    sprintf(textBytes, "Tex: %2.3f%s", memsize_float(gDebugModelSizeTex, &tag), tagStr[tag]);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 146 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 146, textBytes, ALIGN_TOP_CENTER);
    
    /*if(numberOfAnimations > 0) {
        sprintf(textBytes, "\n PTR: %08X\n Scale: %f\n\n Anim: %d of %d\n Frame: %d of %d\n Keyframe: %d\n\n Yaw: %f\n Pitch: %f", 
            (s32)(&viewModel->objModel), scale, (animationID + 1), numberOfAnimations, (animationFrame + 1), animationFrameCount, animationFrame >> 4, yaw, pitch);
    } else {
        sprintf(textBytes, "\n PTR: %08X\n Scale: %f\n\n No animations\n\n Yaw: %f\n Pitch: %f", 
            (s32)(&viewModel->objModel), scale, yaw, pitch);
    }*/
}

void debugmenu_sprite_viewer(s32 updateRate, s32 input) {
    ObjectTransform tempForm;
    char textBytes[64];
    char *tagStr[] = {"B", "KB", "MB"};
    s32 tag;
    s32 i;
    u32 buttonsDown;
    s32 newSprite;

    newSprite = FALSE;
    if (gPauseOptionScroll == 0) {
        currentModelIndex = 0;
        newSprite = TRUE;
        gPauseOptionScroll = 1;
    }


    if(actionDelay < 1) {
        buttonsDown = input_held(0);
        if (buttonsDown & A_BUTTON) {
            actionDelay = 10;
            currentModelIndex++;
            if (currentModelIndex >= ASSET_SPRITES_COUNT) {
                currentModelIndex = 0;
            }
            newSprite = TRUE;
        } else if (buttonsDown & B_BUTTON) {
            actionDelay = 10;
            currentModelIndex--;
            if (currentModelIndex < 0) {
                currentModelIndex = ASSET_SPRITES_COUNT - 1;
            }
            newSprite = TRUE;
        }
    } else {
        actionDelay -= updateRate;
        if (actionDelay < 0) {
            actionDelay = 0;
        }
    }

    if (newSprite) {
        sprite_free(testSprite[0]);
        testSprite[0] = tex_load_sprite(currentModelIndex, 0);
        gDebugModelTex = testSprite[0]->numberOfTextures;
        gDebugModelTris = testSprite[0]->numberOfFrames;
        animationFrame = 0;
        offsetY = 0;
        gDebugModelSizeTex = 0;
        for (i = 0; i < gDebugModelTex; i++) {
            gDebugModelSizeTex += testSprite[0]->textures[i]->textureSize;
        }
    }

    if (gDebugModelTris > 1) {
        if (input & R_TRIG) {
            animationFrame++;
            if (animationFrame > gDebugModelTris - 1) {
                animationFrame = 0;
            }
        }
        if (input & L_TRIG) {
            animationFrame--;
            if (animationFrame < 0) {
                animationFrame = gDebugModelTris - 1;
            }
        }
    }

    if (input & Z_TRIG) {
        gDebugModelMiscCounter ^= 1;
    }

    if (gDebugModelMiscCounter) {
        gDebugModelWheelFrame++;
        if (gDebugModelWheelFrame > 4) {
            gDebugModelWheelFrame = 0;
            animationFrame++;
            if (animationFrame > gDebugModelTris - 1) {
                animationFrame = 0;
            }
        }
    }

    if (input & START_BUTTON) {
        mempool_free(testSprite[0]);
        gPauseSubmenu = 0;
        gPauseOptionScroll = 0;
        gMenuOption = 1;
    }
    
    fakeSpritetrans.x_position = 200.0f;
    fakeSpritetrans.y_position = -175.0f - offsetY;
    fakeSpritetrans.z_position = 30.0f;
    fakeSpritetrans.scale = 1.0f;
    fakeObjectForModel.animFrame = ((f32) (animationFrame + 1) / (f32) gDebugModelTris) * 0xFF;

    gDPSetPrimColor(sMenuCurrDisplayList++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(sMenuCurrDisplayList++, 255, 255, 255, 0);
    mtx_world_origin(&sMenuCurrDisplayList, &sMenuCurrHudMat);
    viewModelTransform.scale = 1.0f;
    viewModelTransform.rotation.x_rotation = 0;
    bzero(&tempForm, sizeof(ObjectTransform));
    tempForm.rotation.z_rotation = sDebugModelViewPitch;
    tempForm.scale = 1.0f;
    tempForm.x_position = 200.0f;
    tempForm.y_position = -175.0f - offsetY;
    tempForm.z_position = 30.0f;

    render_sprite_billboard(&sMenuCurrDisplayList, &sMenuCurrHudMat, &sMenuCurrHudVerts, (Object*)&fakeObjectForModel, testSprite[0], 0);

    
    // Draw text 
    set_text_font(ASSET_FONTS_FUNFONT);
    set_text_colour(255, 255, 255, 0, 255);
    
    sprintf(textBytes, "%d: %s", currentModelIndex, assettable_name(ASSET_SPRITES, currentModelIndex));
    draw_text(&sMenuCurrDisplayList, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 32, textBytes, ALIGN_TOP_CENTER);

    set_text_font(ASSET_FONTS_SMALLFONT);

    sprintf(textBytes, "Frames: %d/%d", animationFrame + 1, gDebugModelTris);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 96 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 96, textBytes, ALIGN_TOP_CENTER);
    
    sprintf(textBytes, "Images: %d", gDebugModelTex);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 106 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 106, textBytes, ALIGN_TOP_CENTER);

    sprintf(textBytes, "Size");
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 116 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 116, textBytes, ALIGN_TOP_CENTER);
    
    sprintf(textBytes, "Tex: %2.3f%s", memsize_float(gDebugModelSizeTex, &tag), tagStr[tag]);
    set_text_colour(0, 0, 0, 255, 255);
    draw_text(&sMenuCurrDisplayList, 56 + 1, 126 + 1, textBytes, ALIGN_TOP_CENTER);
    set_text_colour(255, 255, 255, 0, 255);
    draw_text(&sMenuCurrDisplayList, 56, 126, textBytes, ALIGN_TOP_CENTER);
}

extern s32 gActiveCameraID;
extern Camera gCameras[8];

char** levelNames = NULL;
s32 longestLevelNameWidth = 0;

s32 loadTracksStartIndex = 0;
s32 loadTrackSelectedIndex = 0;

s32 trackSelectDelay = 0;
s32 switchDelay = 0;
s32 inLoadTrackMenu = FALSE;
s32 numberOfLevels = 0;
s32 currentLevelIndex = 2;
f32 inputDelay = 0.0f;

void cam_move(f32 x, f32 y, f32 z);
void cam_move_dir(f32 x, f32 y, f32 z);
void cam_rotate(s32 angleX, s32 angleY, s32 angleZ);

void menu_level_preview_load_level(s32 levelToLoad) {
    Settings *settings;
    s32 prevFlags;

    currentLevelIndex = levelToLoad;
    settings = get_settings();
    prevFlags = settings->courseFlagsPtr[levelToLoad];
    settings->courseFlagsPtr[levelToLoad] |= 0x7;
    
    load_level_for_menu(currentLevelIndex, 0, 2);
    camera_reset(0,0,0,0,0,0);
    inLoadTrackMenu = FALSE;
    set_current_dialogue_box_coords(1, 0, 0, 0, 0);
    dialogue_clear(1);
    gMenuStopUpdating = TRUE;
    
    settings->courseFlagsPtr[levelToLoad] = prevFlags;
}

void menu_level_preview_load_level_names(void) {
    char *assetLevelNames;
    char *curLevelName;
    char *curLevelNameEnd;
    u32 *assetLevelNamesTable;
    s32 i;
    s32 levelNameLength;
    s32 levelNameWidth;
    
    if(levelNames != NULL) {
        return;
    }
    
    assetLevelNames = (char *) asset_table_load(ASSET_LEVEL_NAMES);
    assetLevelNamesTable = asset_table_load(ASSET_LEVEL_NAMES_TABLE);
    
    numberOfLevels = 0;
    while(assetLevelNamesTable[numberOfLevels] != 0xFFFFFFFF) {
        numberOfLevels++;
    }
    numberOfLevels--;
    
    levelNames = mempool_alloc_safe(sizeof(char*) * numberOfLevels, 0);
    
    for(i = 0; i < numberOfLevels; i++) {
        curLevelName = &assetLevelNames[assetLevelNamesTable[i]];
        curLevelNameEnd = curLevelName;
        while(*curLevelNameEnd != '\0') curLevelNameEnd++;
        levelNameLength = ((s32)curLevelNameEnd - (s32)curLevelName) + 1; // +1 for null terminator
        levelNameWidth = get_text_width(curLevelName, 0, ASSET_FONTS_FUNFONT);
        if(levelNameWidth > longestLevelNameWidth) {
            longestLevelNameWidth = levelNameWidth;
        }
        levelNames[i] = mempool_alloc_safe(levelNameLength, 0);
        bcopy(curLevelName, levelNames[i], levelNameLength);
    }
    
}

// Call this when switching to another menu!
void menu_level_preview_cleanup(void) {
    s32 i;
    
    if(levelNames == NULL) {
        return;
    }
    
    for(i = 0; i < numberOfLevels; i++) {
        mempool_free(levelNames[i]);
    }
    mempool_free(levelNames);
    levelNames = NULL;
}

void menu_level_preview_init(void) {
    menu_level_preview_load_level_names();
    menu_level_preview_load_level(0);
    gCameras[gActiveCameraID].trans.x_position = -1374.0f;
    gCameras[gActiveCameraID].trans.y_position = 625.0f;
    gCameras[gActiveCameraID].trans.z_position = -873.0f;
    gCameras[gActiveCameraID].trans.rotation.y_rotation = 0xE2B4;
    gCameras[gActiveCameraID].trans.rotation.x_rotation = 0x0DAC;
    gCameras[gActiveCameraID].trans.rotation.z_rotation = 0x0000;
}

void toggle_load_track_submenu() {
    inLoadTrackMenu = !inLoadTrackMenu;
    switchDelay = 20;
    
    if(inLoadTrackMenu) {
        open_dialogue_box(1);
        set_current_dialogue_background_colour(1, 0, 0, 0, 128);
        set_dialogue_font(1, ASSET_FONTS_FUNFONT);
        set_current_dialogue_box_coords(1, 160 - (longestLevelNameWidth / 2) - 24, 8, 160 + (longestLevelNameWidth / 2), 240 - 8);
    } else {
        set_current_dialogue_box_coords(1, 0, 0, 0, 0);
        dialogue_clear(1);
    }
}

void cam_move(f32 x, f32 y, f32 z) {
    gCameras[gActiveCameraID].trans.x_position += x;
    gCameras[gActiveCameraID].trans.y_position += y;
    gCameras[gActiveCameraID].trans.z_position += z;
    gCameras[gActiveCameraID].cameraSegmentID = get_level_segment_index_from_position(
        gCameras[gActiveCameraID].trans.x_position, gCameras[gActiveCameraID].trans.y_position,
        gCameras[gActiveCameraID].trans.z_position);
}

/**
 * Move the camera with velocities accounting for face direction.
 * Also recalculates which block it's in.
 */
void cam_move_dir(f32 x, UNUSED f32 y, f32 z) {
    gCameras[gActiveCameraID].trans.x_position -= x * coss_f(gCameras[gActiveCameraID].trans.rotation.y_rotation);
    gCameras[gActiveCameraID].trans.z_position -= x * sins_f(gCameras[gActiveCameraID].trans.rotation.y_rotation);
    gCameras[gActiveCameraID].trans.x_position -= z * sins_f(gCameras[gActiveCameraID].trans.rotation.y_rotation);
    gCameras[gActiveCameraID].trans.z_position += z * coss_f(gCameras[gActiveCameraID].trans.rotation.y_rotation);
    gCameras[gActiveCameraID].cameraSegmentID = get_level_segment_index_from_position(
        gCameras[gActiveCameraID].trans.x_position, gCameras[gActiveCameraID].trans.y_position,
        gCameras[gActiveCameraID].trans.z_position);
}

/**
 * Rotate the camera with the given angles.
 */
void cam_rotate(s32 angleX, s32 angleY, s32 angleZ) {
    gCameras[gActiveCameraID].trans.rotation.y_rotation += angleX;
    gCameras[gActiveCameraID].trans.rotation.x_rotation += angleY;
    gCameras[gActiveCameraID].trans.rotation.z_rotation += angleZ;
}

void menu_level_preview_handle_input(s32 updateRate) {
    u32 buttonsDown;
    s32 contX, contY;
    f32 moveForward, moveVertical, moveSideways;
    f32 speed;
    s32 yawRotate, pitchRotate;
    
    if(switchDelay > 0) {
        switchDelay -= updateRate;
        return;
    }
    
    buttonsDown = input_held(0);
    contX = gControllersXAxis[0];
    contY = gControllersYAxis[0];
    
    if(buttonsDown & Z_TRIG) {
        toggle_load_track_submenu();
        return;
    }
    
    moveForward = 0.0f;
    moveVertical = 0.0f;
    moveSideways = 0.0f;
    speed = 25.0f;
    
    yawRotate = 0;
    pitchRotate = 0;
    
    if(buttonsDown & R_TRIG) {
        speed *= 2;
    }
    
    if(contY < 0) {
        moveForward = -speed * updateRate;
    } else if(contY > 0) {
        moveForward = speed * updateRate;
    }
    
    if(contX < 0) {
        moveSideways = -speed * updateRate;
    } else if(contX > 0) {
        moveSideways = speed * updateRate;
    }
    
    if(buttonsDown & A_BUTTON) {
        moveVertical = speed * updateRate;
    } else if(buttonsDown & B_BUTTON) {
        moveVertical = -speed * updateRate;
    }
    
    cam_move_dir(moveSideways, 0, moveForward);
    
    // cam_move_dir doesn't do anything with the y component, so gotta use cam_move.
    if(moveVertical != 0.0f) {
        cam_move(0, moveVertical, 0);
    }
    
    if(buttonsDown & R_CBUTTONS) {
        yawRotate = speed * updateRate * 10;
    } else if(buttonsDown & L_CBUTTONS) {
        yawRotate = -speed * updateRate * 10;
    }
    if(buttonsDown & U_CBUTTONS) {
        pitchRotate = -speed * updateRate * 10;
    } else if(buttonsDown & D_CBUTTONS) {
        pitchRotate = speed * updateRate * 10;
    }
    
    if(yawRotate != 0 || pitchRotate != 0) {
        cam_rotate(yawRotate, pitchRotate, 0);
    }
}

void menu_level_preview_handle_track_select(s32 updateRate, UNUSED s32 inputPressed) {
    u32 buttonsDown;
    s32 contY;
    
    if(switchDelay > 0) {
        switchDelay -= updateRate;
        return;
    }
    
    if(trackSelectDelay > 0) {
        trackSelectDelay -= updateRate;
        return;
    }
    
    buttonsDown = input_held(0);
    contY = gControllersYAxis[0];

    
    
    if(buttonsDown & B_BUTTON) {
        toggle_load_track_submenu();
        return;
    }
    
    if(buttonsDown & A_BUTTON) {
        menu_level_preview_load_level(loadTrackSelectedIndex);
        return;
    }
    
    if(contY < 0) {
        loadTrackSelectedIndex++;
        if(loadTrackSelectedIndex > numberOfLevels - 1) {
            loadTrackSelectedIndex = 0;
        }
        trackSelectDelay = 4;
    } else if(contY > 0) {
        loadTrackSelectedIndex--;
        if(loadTrackSelectedIndex < 0) {
            loadTrackSelectedIndex = numberOfLevels - 1;
        }
        trackSelectDelay = 4;
    }
}

const s32 OFFSET_AMOUNT = 13;

void debugmenu_level_viewer(s32 updateRate, s32 inputPressed) {
    //char debugText[256];
    s32 i;

    if (gPauseOptionScroll == 0) {
        menu_level_preview_init();
        gPauseOptionScroll = 1;
    }

    if (inputPressed & START_BUTTON) {
        gPauseSubmenu = 0;
        load_level_for_menu(ASSET_LEVEL_OPTIONSBACKGROUND, -1, 0);
        gMenuStopUpdating = FALSE;
        gPauseOptionScroll = 0;
        gMenuOption = 1;
        return;
    }
    
    if(inLoadTrackMenu) {
        //s32 end;
        
        menu_level_preview_handle_track_select(updateRate, inputPressed);
        
        dialogue_clear(1);
        set_current_text_colour(1, 0, 255, 0, 128, 255);
        render_dialogue_text(1, POS_CENTRED, 6, "Select Level", 1, HORZ_ALIGN_CENTER);
        
        if(loadTrackSelectedIndex >= (loadTracksStartIndex + OFFSET_AMOUNT - 1)) {
            loadTracksStartIndex = (loadTrackSelectedIndex - OFFSET_AMOUNT + 2);
        } else if(loadTrackSelectedIndex < loadTracksStartIndex) {
            loadTracksStartIndex = loadTrackSelectedIndex;
        }
        
        //end = loadTracksStartIndex + OFFSET_AMOUNT;
        
        for(i = 0; i < OFFSET_AMOUNT; i++) {
            if((loadTracksStartIndex + i) > numberOfLevels) {
                break;
            }
            
            if(i == (loadTrackSelectedIndex - loadTracksStartIndex)) {
                set_current_text_colour(1, 255, 255, 255, 128, 255);
            } else {
                set_current_text_colour(1, 255, 255, 255, 0, 255);
            }
            
            render_dialogue_text(1, POS_CENTRED, 24 + (i * 16), levelNames[loadTracksStartIndex + i], 1, HORZ_ALIGN_CENTER);
        }
        
        return;
    }
    
    menu_level_preview_handle_input(updateRate);
    
    /*sprintf(debugText, "\n %s\n POS: %d,%d,%d\n ANG: %x %x %x",
        levelNames[currentLevelIndex],
        (s32)gCameras[gActiveCameraID].trans.x_position,
        (s32)gCameras[gActiveCameraID].trans.y_position,
        (s32)gCameras[gActiveCameraID].trans.z_position,
        gCameras[gActiveCameraID].trans.rotation.y_rotation,
        gCameras[gActiveCameraID].trans.rotation.x_rotation,
        gCameras[gActiveCameraID].trans.rotation.z_rotation
    );
    
    set_text_font(ASSET_FONTS_FUNFONT);
    set_text_colour(255, 255, 255, 0, 255);
    
    draw_text(&sMenuCurrDisplayList, 8, 0, debugText, ALIGN_MIDDLE_LEFT);*/
}

void debugmenu_minimal(UNUSED s32 updateRate, s32 inputPressed) {
    if (gPauseOptionScroll == 0) {
        unload_level_menu();
        gPauseOptionScroll = 1;
    }

    if (inputPressed & START_BUTTON) {
        gPauseSubmenu = 0;
        load_level_for_menu(ASSET_LEVEL_OPTIONSBACKGROUND, -1, 0);
        music_play(SEQUENCE_MAIN_MENU);
        gPauseOptionScroll = 0;
        gMenuOption = 1;
        return;
    }

    debug_fillrect(&sMenuCurrDisplayList, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOUR_RGBA32(32, 32, 32, 255));
}

s32 menu_debug_root_loop(s32 updateRate) {
    s32 inputPressed;
    s32 i;

    gOptionBlinkTimer = (gOptionBlinkTimer + updateRate) & 0x3F;
    inputPressed = 0;
    gMenuStickX[PLAYER_MENU] = 0;
    gMenuStickY[PLAYER_MENU] = 0;
    for (i = 0; i < 4; i++) {
        gMenuStickX[PLAYER_MENU] += gControllersXAxisDirection[i];
        gMenuStickY[PLAYER_MENU] += gControllersYAxisDirection[i];
        inputPressed |= input_pressed(i);
    }

    switch (gPauseSubmenu) {
        case 0:
            debugmenu_root(updateRate, inputPressed);
            break;
        case 1:
            debugmenu_model_viewer(updateRate, inputPressed);
            break;
        case 2:
            debugmenu_sprite_viewer(updateRate, inputPressed);
            break;
        case 3:
            debugmenu_level_viewer(updateRate, inputPressed);
            break;
        case 4:
            debugmenu_minimal(updateRate, inputPressed);
            break;
    }

    if (gPauseSubmenu != 4) {
        set_text_font(ASSET_FONTS_BIGFONT);
        set_text_background_colour(0, 0, 0, 0);
        set_text_colour(0, 0, 0, 255, 128);
        draw_text(&sMenuCurrDisplayList, SCREEN_WIDTH_HALF + 1, 35, gDebugMenuSubStrings[gPauseSubmenu], ALIGN_MIDDLE_CENTER);
        set_text_colour(255, 255, 255, 0, 255);
        draw_text(&sMenuCurrDisplayList, SCREEN_WIDTH_HALF, 32, gDebugMenuSubStrings[gPauseSubmenu], ALIGN_MIDDLE_CENTER);
    }

    return MENU_RESULT_CONTINUE;
}
