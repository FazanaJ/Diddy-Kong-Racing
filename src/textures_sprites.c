/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x8007AC70 */

#include "textures_sprites.h"
#include "gzip.h"
#include "asset_loading.h"
#include "memory.h"
#include "math_util.h"
#include "game.h"
#include "main.h"
#include "tracks.h"

#define TEX_HEADER_COUNT 175
#define TEX_SPRITE_COUNT 50
#define TEX_PALLETE_COUNT 20

/************ .data ************/

u32 gTexColourTag = MEMP_MISC_TEXTURES;
s32 gSpriteOpaque = TRUE;

// See "include/f3ddkr.h" for the defines

Gfx dRenderSettingsVtxAlpha[][2] = {
    // Semitransparent Vertex Alpha'd surface (Zsorted)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA, DKR_CC_UNK0, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN0, G_RM_XLU_SURF2,
                     DKR_RM_UNKNOWN0, G_RM_AA_XLU_SURF2, DKR_RM_UNKNOWN0, G_RM_ZB_XLU_SURF2, DKR_RM_UNKNOWN0,
                     G_RM_AA_ZB_XLU_SURF2),
    // Semitransparent Vertex Alpha'd surface (No Zsort)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN1, G_RM_XLU_SURF2,
                     DKR_RM_UNKNOWN1, G_RM_AA_XLU_SURF2, DKR_RM_UNKNOWN1, G_RM_ZB_XLU_SURF2, DKR_RM_UNKNOWN1,
                     G_RM_AA_ZB_XLU_SURF2),
};

Gfx dRenderSettingsSpriteCld[][2] = {
    // Semitransparent Sprite (Preserve coverage)
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_CLD_SURF, G_RM_CLD_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_ZB_CLD_SURF,
                     G_RM_ZB_CLD_SURF2)
};

// Should probably be merged with dRenderSettingsSpriteCld
Gfx dRenderSettingsSpriteXlu[][2] = {
    // Semitransparent Sprite (Overwrite coverage)
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_XLU_SURF, G_RM_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, DKR_RM_UNKNOWN2_1,
                     DKR_RM_UNKNOWN2_2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, DKR_RM_UNKNOWN3_1,
                     DKR_RM_UNKNOWN3_2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_AA_ZB_XLU_INTER,
                     G_RM_AA_ZB_XLU_INTER2),
    // Semitransparent Sprite (Overwrite coverage) (Copy)
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_XLU_SURF, G_RM_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, DKR_RM_UNKNOWN2_1,
                     DKR_RM_UNKNOWN2_2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, DKR_RM_UNKNOWN3_1,
                     DKR_RM_UNKNOWN3_2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_POINT, G_RM_AA_ZB_XLU_INTER,
                     G_RM_AA_ZB_XLU_INTER2)
};

Gfx dRenderSettingsCommon[][2] = {
    // Opaque Surface
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_OPA_SURF, G_RM_OPA_SURF2,
                     G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2, G_RM_AA_ZB_OPA_SURF,
                     G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                     G_RM_AA_XLU_SURF, G_RM_XLU_SURF2, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2, G_RM_AA_ZB_XLU_SURF,
                     G_RM_AA_ZB_XLU_SURF2),
    // Opaque Surface with fog
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface with fog
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_XLU_SURF2),
    // Cutout Surface with primitive colour
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                     G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2,
                     G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour (Copy)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                     G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2,
                     G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour and fog
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, DKR_CC_UNK2, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_RA_ZB_TEX_EDGE2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour and fog (Zsorted interpenetrating)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, DKR_CC_UNK2, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_INTER2 | Z_UPD,
                     G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_INTER2),
};

Gfx dRenderSettingsCommon_ext[][2] = {
    // Opaque Surface
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_OPA_SURF, G_RM_OPA_SURF2,
                         G_RM_RA_OPA_SURF, G_RM_RA_OPA_SURF2, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2, G_RM_ZB_OPA_SURF,
                         G_RM_ZB_OPA_SURF2, G_RM_RA_ZB_OPA_SURF, G_RM_RA_ZB_OPA_SURF2, G_RM_AA_ZB_OPA_SURF,
                         G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                         G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2, G_RM_ZB_XLU_SURF,
                         G_RM_ZB_XLU_SURF2, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2, G_RM_AA_ZB_XLU_SURF,
                         G_RM_AA_ZB_XLU_SURF2),
    // Opaque Surface with fog
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2,
                         G_RM_FOG_SHADE_A, G_RM_RA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_RA_ZB_OPA_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface with fog
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                         G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_AA_ZB_XLU_SURF2),
};

Gfx dRenderSettingsCutout[][2] = {
    // Semitransparent Surface
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_RA_TEX_EDGE,
                     G_RM_RA_TEX_EDGE2, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE2, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2,
                     G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2),
    // Semitransparent Surface with fog
    DRAW_TABLE_GROUP(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_RA_TEX_EDGE2,
                     G_RM_FOG_SHADE_A, G_RM_AA_TEX_EDGE2, G_RM_FOG_SHADE_A, G_RM_RA_ZB_TEX_EDGE2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_TEX_EDGE2),
};

Gfx dRenderSettingsDecal[][2] = {
    // Opaque Decal.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_ZB_OPA_DECAL,
                     G_RM_ZB_OPA_DECAL2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_AA_ZB_OPA_DECAL,
                     G_RM_AA_ZB_OPA_DECAL2),
    // Semitransparent Decal.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_ZB_XLU_DECAL,
                     G_RM_ZB_XLU_DECAL2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_AA_ZB_XLU_DECAL,
                     G_RM_AA_ZB_XLU_DECAL2),
    // Opaque Decal with fog.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_DECAL2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_DECAL2),
    // Semitransparent Decal with fog.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_DECAL2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_DECAL2),
};

Gfx dRenderSettingsSolidColourVtxAlpha[][2] = {
    DRAW_TABLE_GROUP(DKR_CC_UNK3, G_CC_MODULATEIA_PRIM2, DKR_OMH_2CYC_POINT, G_RM_NOOP, G_RM_XLU_SURF2, G_RM_NOOP,
                     G_RM_AA_XLU_SURF2, G_RM_NOOP, G_RM_ZB_XLU_SURF2, G_RM_NOOP, G_RM_AA_ZB_XLU_SURF2),
};

Gfx dRenderSettingsSolidColour[][2] = {
    // Opaque Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK4, DKR_CC_UNK4, DKR_OMH_1CYC_POINT, G_RM_OPA_SURF, G_RM_OPA_SURF2, G_RM_AA_OPA_SURF,
                     G_RM_AA_OPA_SURF2, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2, G_RM_AA_ZB_OPA_INTER,
                     G_RM_AA_ZB_OPA_INTER2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK4, DKR_CC_UNK4, DKR_OMH_1CYC_POINT, G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_AA_XLU_SURF,
                     G_RM_AA_XLU_SURF2, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2),
    // Opaque Surface with fog
    DRAW_TABLE_GROUP(DKR_CC_UNK4, G_CC_MODULATEIA_PRIM2, DKR_OMH_2CYC_POINT, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK4, G_CC_MODULATEIA_PRIM2, DKR_OMH_2CYC_POINT, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_XLU_SURF2),
};

// Some kind of texture on top of a solid colour
Gfx dRenderSettingsPrimOverlay[][2] = {
    // Opaque Surface
    DRAW_TABLE_ENTRY(DKR_CC_DECALFADEPRIM, DKR_CC_DECALFADEPRIM, DKR_OMH_1CYC_BILERP, G_RM_ZB_OPA_DECAL,
                     G_RM_ZB_OPA_DECAL2),
    DRAW_TABLE_ENTRY(DKR_CC_DECALFADEPRIM, DKR_CC_DECALFADEPRIM, DKR_OMH_1CYC_BILERP, G_RM_AA_ZB_OPA_DECAL,
                     G_RM_AA_ZB_OPA_DECAL2),
    // Semitransparent Surface
    DRAW_TABLE_ENTRY(DKR_CC_DECALFADEPRIM, DKR_CC_DECALFADEPRIM, DKR_OMH_1CYC_BILERP, G_RM_ZB_XLU_DECAL,
                     G_RM_ZB_XLU_DECAL2),
    DRAW_TABLE_ENTRY(DKR_CC_DECALFADEPRIM, DKR_CC_DECALFADEPRIM, DKR_OMH_1CYC_BILERP, G_RM_AA_ZB_XLU_DECAL,
                     G_RM_AA_ZB_XLU_DECAL2),
};

// Not sure what it is specifically, but some onscreen actors like TT and Taj use it.
Gfx dRenderSettingsPrimCol[][2] = {
    // Opaque Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK5, DKR_CC_UNK6, DKR_OMH_2CYC_BILERP, G_RM_NOOP, G_RM_OPA_SURF2, G_RM_NOOP,
                     G_RM_AA_OPA_SURF2, G_RM_NOOP, G_RM_ZB_OPA_SURF2, G_RM_NOOP, G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK5, DKR_CC_UNK6, DKR_OMH_2CYC_BILERP, G_RM_NOOP, G_RM_XLU_SURF2, G_RM_NOOP,
                     G_RM_AA_XLU_SURF2, G_RM_NOOP, G_RM_ZB_XLU_SURF2, G_RM_NOOP, G_RM_AA_ZB_XLU_SURF2),
};

// Only opaque surface is actually used here.
Gfx dRenderSettingsBlinkingLights[][2] = {
    // Opaque Surface with fog
    DRAW_TABLE_GROUP(DKR_CC_UNK7, DKR_CC_UNK8, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
};

Gfx D_800DF3A8[] = {
    gsSPClearGeometryMode(G_ZBUFFER | G_FOG),
    gsDPPipeSync(),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

Gfx D_800DF3D8[] = {
    gsSPClearGeometryMode(G_FOG), gsSPSetGeometryMode(G_ZBUFFER), gsDPPipeSync(),       gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE), gsDPSetAlphaCompare(G_AC_NONE), gsSPEndDisplayList(),
};

Gfx D_800DF410[][2] = {
    DRAW_TABLE_ENTRY(DKR_CC_UNK9, DKR_CC_UNK9, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2),
    DRAW_TABLE_ENTRY(G_CC_BLENDPE, G_CC_BLENDPE, DKR_OMH_1CYC_BILERP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2),
    DRAW_TABLE_ENTRY(DKR_CC_UNK9, G_CC_PASS2, DKR_OMH_2CYC_BILERP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2)
};

/*******************************/

/************ .rodata ************/

/*********************************/

/************ .bss ************/

s32 *gTextureAssetTable[2];

s32 *gTextureCache;

u8 *gCiPalettes;
s32 gNumberOfLoadedTextures;
s32 D_80126334;
s32 gTextureAssetID[2];
s32 gCiPalettesSize;
s32 D_80126344;
s32 *gSpriteOffsetTable;

s32 *gSpriteCache;

Sprite *gCurrentSprite;
s32 gSpriteTableNum;
s32 D_80126358;
s32 D_8012635C;
Vertex *D_80126360;
Gfx *D_80126364;
Triangle *D_80126368;

TempTexHeader *gTempTextureHeader;
u8 *D_80126370;
s32 gCurrentRenderFlags;
s32 gBlockedRenderFlags;
TextureHeader *gCurrentTextureHeader;
s16 gUsingTexture;
s16 gForceFlags;
s16 gUsePrimColour;

/******************************/

/**
 * Load the texture table from ROM and allocate space for all the texture asset management.
 */
void tex_init_textures(void) {
    s32 i;

    gTextureCache = allocate_from_main_pool_safe(sizeof(TextureHeader) * TEX_HEADER_COUNT, MEMP_MISC_TEXTURES);
    gCiPalettes = allocate_from_main_pool_safe(0x280, MEMP_MISC_TEXTURES);
    gNumberOfLoadedTextures = 0;
    gCiPalettesSize = 0;
    gAssetColourTag = MEMP_MISC_TEXTURES;
    gTextureAssetTable[TEX_TABLE_2D] = (s32 *) load_asset_section_from_rom(ASSET_TEXTURES_2D_TABLE);
    gTextureAssetTable[TEX_TABLE_3D] = (s32 *) load_asset_section_from_rom(ASSET_TEXTURES_3D_TABLE);

    for (i = 0; gTextureAssetTable[TEX_TABLE_2D][i] != -1; i++) {}
    gTextureAssetID[TEX_TABLE_2D] = --i;

    for (i = 0; gTextureAssetTable[TEX_TABLE_3D][i] != -1; i++) {}
    gTextureAssetID[TEX_TABLE_3D] = --i;

    gSpriteCache = allocate_from_main_pool_safe(sizeof(Sprite) * TEX_SPRITE_COUNT, MEMP_MISC_TEXTURES);
    gCurrentSprite = allocate_from_main_pool_safe(sizeof(Sprite) * 32, MEMP_MISC_TEXTURES);
    D_80126358 = 0;
    gSpriteOffsetTable = (s32 *) load_asset_section_from_rom(ASSET_SPRITES_TABLE);
    gSpriteTableNum = 0;
    while (gSpriteOffsetTable[gSpriteTableNum] != -1) {
        gSpriteTableNum++;
    }
    gSpriteTableNum--;

    gTempTextureHeader = allocate_from_main_pool_safe(0x28, MEMP_MISC_TEXTURES);
    D_80126344 = 0;
    gAssetColourTag = COLOUR_TAG_GREY;
}

/**
 * Official Name: texDisableModes
 * Add flags to the block list so they are removed when drawn.
 */
void tex_disable_modes(s32 flags) {
    gBlockedRenderFlags |= flags;
}

/**
 * Official Name: texEnableModes
 * Remove flags to the block list so they are no longer removed when drawn.
 */
void tex_enable_modes(s32 flags) {
    gBlockedRenderFlags &= ~flags;
}

#ifdef NON_EQUIVALENT
// Minor matching issues with loops, but should be functionally the same.
// Official Name: texLoadTexture
TextureHeader *load_texture(s32 arg0) {
    s32 assetSection;
    s32 assetIndex;
    s32 assetOffset;
    s32 assetSize;
    s32 assetTable;
    s32 texIndex;
    s32 temp_a1;
    s32 paletteOffset;
    s32 i;
    u8 *alignedAddress;
    TextureHeader *tex;
    TextureHeader *texTemp;
    s32 numberOfTextures;
    s32 sp3C;
    s32 temp_a0;
    s32 temp_v0_5;

    arg0 &= 0xFFFF;
    assetIndex = arg0;
    assetSection = ASSET_TEXTURES_2D;
    assetTable = 0;
    if (arg0 & 0x8000) {
        assetTable = 1;
        assetIndex = arg0 & 0x7FFF;
        assetSection = ASSET_TEXTURES_3D;
    }
    if ((assetIndex >= gTextureAssetID[assetTable]) || (assetIndex < 0)) {
        arg0 = 0;
    }
    for (i = 0; i < gNumberOfLoadedTextures; i++) {
        if (arg0 == gTextureCache[(i << 1)]) {
            tex = gTextureCache[(i << 1) + 1];
            tex->numberOfInstances++;
            return tex;
        }
    }
    assetOffset = gTextureAssetTable[assetTable][assetIndex];
    assetSize = gTextureAssetTable[assetTable][assetIndex + 1] - assetOffset;
    load_asset_to_address(assetSection, gTempTextureHeader, assetOffset, 0x28);
    numberOfTextures = (gTempTextureHeader->header.numOfTextures >> 8) & 0xFFFF;

    if (!gTempTextureHeader->header.isCompressed) {
        tex = allocate_from_main_pool((numberOfTextures * 0x60) + assetSize, gTexColourTag);
        if (tex == NULL) {
            return NULL;
        }
        load_asset_to_address(assetSection, tex, assetOffset, assetSize);
    } else {
        temp_v0_5 = byteswap32(&gTempTextureHeader->uncompressedSize);
        temp_a0 = (numberOfTextures * 0x60) + temp_v0_5;
        sp3C = temp_v0_5 + 0x20;
        tex = allocate_from_main_pool(temp_a0 + 0x20, gTexColourTag);
        if (tex == NULL) {
            return NULL;
        }
        temp_a1 = ((s32) tex + sp3C) - assetSize;
        temp_a1 -= temp_a1 % 0x10;
        load_asset_to_address(assetSection, temp_a1, assetOffset, assetSize);
        gzip_inflate(temp_a1 + 0x20, tex);
        assetSize = sp3C - 0x20;
    }
    texIndex = -1;
    for (i = 0; i < gNumberOfLoadedTextures; i++) {
        if (gTextureCache[(i << 1)] == -1) {
            texIndex = i;
        }
    }
    if (texIndex == -1) {
        texIndex = gNumberOfLoadedTextures++;
    }
    gTextureCache[(texIndex << 1)] = arg0;
    gTextureCache[(texIndex << 1) + 1] = tex;
    paletteOffset = -1;
    if ((tex->format & 0xF) == TEX_FORMAT_CI4) {
        if (D_80126344 == 0) {
            load_asset_to_address(ASSET_EMPTY_14, &gCiPalettes[gCiPalettesSize], tex->ciPaletteOffset, 32);
            tex->ciPaletteOffset = gCiPalettesSize;
            gCiPalettesSize += 32; // (32 bytes / 2 bytes per color) = 16 colors.
        }
        paletteOffset = gCiPalettesSize - 32;
    }
    if ((tex->format & 0xF) == TEX_FORMAT_CI8) {
        if (D_80126344 == 0) {
            load_asset_to_address(ASSET_EMPTY_14, &gCiPalettesSize[gCiPalettes], tex->ciPaletteOffset, 128);
            tex->ciPaletteOffset = gCiPalettesSize;
            gCiPalettesSize += 128; // (128 bytes / 2 bytes per color) = 64 colors.
        }
        paletteOffset = gCiPalettesSize - 128;
    }
    D_80126344 = 0;
    texTemp = tex;
    alignedAddress = align16((u8 *) ((s32) texTemp + assetSize));
    for (i = 0; i < numberOfTextures; i++) {
        build_tex_display_list(texTemp, alignedAddress);
        if (paletteOffset >= 0) {
            texTemp->ciPaletteOffset = paletteOffset;
            alignedAddress += 0x30; // I'm guessing it takes 6 f3d commands to load the palette
        }
        alignedAddress += 0x60; // I'm guessing it takes 12 f3d commands to load the texture
        texTemp = (TextureHeader *) ((s32) texTemp + texTemp->textureSize);
    }
    if (gCiPalettesSize >= 0x280) {
        return NULL;
    }
    if (gNumberOfLoadedTextures >= 701) {
        return NULL;
    }
    return tex;
}
#else
GLOBAL_ASM("asm/non_matchings/textures_sprites/load_texture.s")
#endif

TextureHeader *load_texture(s32 arg0) {
    TextureHeader *tex;
    tex = load_texture2(arg0);
    return tex;
}

/**
 * This function frees textures
 * Official Name: texFreeTexture
 */
void free_texture(TextureHeader *tex) {
    s32 i;

    if (tex != 0) {
        if ((--tex->numberOfInstances) <= 0) {
            for (i = 0; i < gNumberOfLoadedTextures; i++) {
                if ((s32) tex == gTextureCache[(i << 1) + 1]) {
                    free_from_memory_pool(tex);

                    gTextureCache[(i << 1)] = -1;
                    gTextureCache[(i << 1) + 1] = -1;
                    return;
                }
            }
        }
    }
}

/**
 * Set the colour tag that determines which memory pool textures will be loaded into.
 * By default, this generally stays as COLOUR_TAG_MAGENTA
 * Official Name: setTexMemColour
 */
void set_texture_colour_tag(s32 tagID) {
    gTexColourTag = tagID;
}

/**
 * Resets all render settings to the default state.
 * The next draw call will be forced to apply all settings instead of skipping unecessary steps.
 * Official Name: texDPInit
 */
void reset_render_settings(Gfx **dlist) {
    gCurrentTextureHeader = NULL;
    gCurrentRenderFlags = RENDER_NONE;
    gUsingTexture = FALSE;
    gForceFlags = TRUE;
    gBlockedRenderFlags = RENDER_NONE;
    gUsePrimColour = FALSE;
    gSPSetGeometryMode((*dlist)++, G_SHADING_SMOOTH | G_SHADE | G_ZBUFFER);
    gDPPipeSync((*dlist)++);
}

/**
 * Enables usage of combiners utilising the indidual primitive colours.
 */
void enable_primitive_colour(void) {
    gUsePrimColour = TRUE;
    gForceFlags = TRUE;
}

/**
 * Disables usage of combiners utilising the indidual primitive colours.
 */
void disable_primitive_colour(void) {
    gUsePrimColour = FALSE;
    gForceFlags = TRUE;
}

/**
 * Shift the texture header by the offset and return the result.
 * Official Name: texFrame
 */
TextureHeader *set_animated_texture_header(TextureHeader *texHead, s32 offset) {
    if (offset > 0) {
        if (offset < texHead->numOfTextures << 8) {
            texHead = (TextureHeader *) (((u8 *) texHead) + ((offset >> 16) * texHead->textureSize));
        } else {
            texHead = (TextureHeader *) (((u8 *) texHead) + ((texHead->numOfTextures >> 8) - 1) * texHead->textureSize);
        }
    }
    return texHead;
}

/**
 * A version of the function below that chooses not to pass along an offset.
 */
void load_and_set_texture_no_offset(Gfx **dlist, TextureHeader *texhead, u32 flags) {
    load_and_set_texture(dlist, texhead, flags, 0);
}

s32 gIsObjectRender = FALSE;

/**
 * Load a texture from memory into texture memory.
 * Also set render mode, combine mode and othermodes based on flags.
 * Also tracks which modes are active, to prevent setting them again if they're already active.
 * A number can be attached that adds a texture address offset. An example of this being used is
 * the numbered doors in the hub, to change what number is written on it.
 */
void load_and_set_texture(Gfx **dlist, TextureHeader *texhead, s32 flags, s32 texOffset) {
    s32 forceFlags;
    s32 doPipeSync;
    s32 dlIndex;
    Gfx *dlID;
    profiler_begin_timer();

    forceFlags = gForceFlags;
    doPipeSync = TRUE;

    if (texhead != NULL) {
        if (texOffset && (texOffset < texhead->numOfTextures << 8)) {
            texhead = (TextureHeader *) ((s8 *) texhead + ((texOffset >> 16) * texhead->textureSize));
        }

        flags |= texhead->flags;
        if (texhead != gCurrentTextureHeader) {
            gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(texhead->cmd), texhead->numberOfCommands);
            gCurrentTextureHeader = texhead;
            doPipeSync = FALSE;
#ifdef PUPPYPRINT_DEBUG
            gPuppyPrint.textureLoads++;
#endif
        }
        if (gUsingTexture == FALSE) {
            forceFlags = TRUE;
            gUsingTexture = TRUE;
        }
    } else if (gUsingTexture) {
        forceFlags = TRUE;
        gUsingTexture = FALSE;
    }

    flags = (gUsePrimColour)
                ? (flags & (RENDER_DECAL | RENDER_ANTI_ALIASING | RENDER_Z_COMPARE | RENDER_SEMI_TRANSPARENT))
                : (flags & (RENDER_VTX_ALPHA | RENDER_DECAL | RENDER_Z_UPDATE | RENDER_CUTOUT | RENDER_FOG_ACTIVE |
                            RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE | RENDER_ANTI_ALIASING));
    flags &= ~gBlockedRenderFlags;
    if (gConfig.antiAliasing == -1 || gConfig.perfMode) {
        flags &= ~RENDER_ANTI_ALIASING;
    }
    flags = (flags & RENDER_VTX_ALPHA) ? flags & ~RENDER_FOG_ACTIVE : flags & ~RENDER_Z_UPDATE;
#ifdef PUPPYPRINT_DEBUG
    if (gPuppyPrint.showCvg && gConfig.antiAliasing == 0) {
        flags &= ~RENDER_SEMI_TRANSPARENT;
    }
#endif
    if (flags != gCurrentRenderFlags || forceFlags) {

        if (((flags & RENDER_VTX_ALPHA) != (gCurrentRenderFlags & RENDER_VTX_ALPHA)) || gForceFlags) {
            if (flags & RENDER_VTX_ALPHA || gUsePrimColour) {
                gSPClearGeometryMode((*dlist)++, G_FOG);
            } else {
                gSPSetGeometryMode((*dlist)++, G_FOG);
            }
        }

        if (((flags & RENDER_Z_COMPARE) != (gCurrentRenderFlags & RENDER_Z_COMPARE)) || gForceFlags) {
            if (flags & RENDER_Z_COMPARE) {
                gSPSetGeometryMode((*dlist)++, G_ZBUFFER);
            } else {
                gSPClearGeometryMode((*dlist)++, G_ZBUFFER);
            }
        }

        gForceFlags = FALSE;
        gCurrentRenderFlags = flags;
        if (!gUsingTexture) {
            if (flags & RENDER_VTX_ALPHA) {
                dlID = dRenderSettingsSolidColourVtxAlpha[flags & (RENDER_ANTI_ALIASING | RENDER_Z_COMPARE)];
                goto draw;
            }
            dlID = dRenderSettingsSolidColour[flags & (RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE |
                                                       RENDER_ANTI_ALIASING)];
            goto draw;
        }

        if (gUsePrimColour) {
            if (flags & RENDER_DECAL && flags & RENDER_Z_COMPARE) {
                dlIndex = (flags & RENDER_ANTI_ALIASING) | ((flags & RENDER_SEMI_TRANSPARENT) >> 1);
                dlID = dRenderSettingsPrimOverlay[dlIndex];
                goto draw;
            }
            dlID = dRenderSettingsPrimCol[flags];
            goto draw;
        }

        if (flags & RENDER_DECAL && flags & RENDER_Z_COMPARE) {
            dlIndex = (flags & RENDER_ANTI_ALIASING) | ((flags & (RENDER_SEMI_TRANSPARENT | RENDER_FOG_ACTIVE)) >> 1);
            dlID = dRenderSettingsDecal[dlIndex];
            goto draw;
        }

        if (flags & RENDER_CUTOUT) {
            dlIndex = (flags & RENDER_Z_COMPARE) | ((flags & RENDER_FOG_ACTIVE) >> 1);
            dlID = dRenderSettingsCutout[dlIndex];
            goto draw;
        }

        flags &= ~RENDER_DECAL;
        if (flags & RENDER_VTX_ALPHA) {
            dlIndex = flags & (RENDER_ANTI_ALIASING | RENDER_Z_COMPARE);
            if (flags & RENDER_Z_UPDATE) {
                dlIndex |= 4; // Z write
            } else {
                gSPSetGeometryMode((*dlist)++, G_ZBUFFER);
                gCurrentRenderFlags |= RENDER_Z_COMPARE;
            }
            dlID = dRenderSettingsVtxAlpha[dlIndex];
            goto draw;
        }
        if (gConfig.perfMode) {
            dlIndex = 0;
        } else {
#ifdef PUPPYPRINT_DEBUG
            dlIndex = gConfig.antiAliasing + 1;
            if (!gPuppyPrint.showCvg) {
                if (gIsObjectRender && gConfig.antiAliasing == 0) {
                    if (gIsObjectRender != 2) {
                        dlIndex++;
                    } else {
                        dlIndex = 0;
                    }
                }
            }
#else
            dlIndex = gConfig.antiAliasing + 1;
            if (gIsObjectRender && gConfig.antiAliasing == 0) {
                if (gIsObjectRender != 2) {
                    dlIndex++;
                } else {
                    dlIndex = 0;
                }
            }
#endif
        }
        if (flags & RENDER_Z_COMPARE) {
            dlIndex += 3;
        }
        if (flags & RENDER_SEMI_TRANSPARENT) {
            dlIndex += 6;
        }
        if (flags & RENDER_FOG_ACTIVE) {
            dlIndex += 12;
        }
        dlID = dRenderSettingsCommon_ext[dlIndex];
    draw:
        if (doPipeSync) {
            gDPPipeSync((*dlist)++);
        }
        gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(dlID), numberOfGfxCommands(dRenderSettingsCommon_ext[0]));
    }
    profiler_add(PP_TEXTURES, first);
}

/**
 * Loads the texture and render settings for the blinking lights seen in Spaceport Alpha.
 */
void load_blinking_lights_texture(Gfx **dlist, TextureHeader *texture_list, s32 texture_index) {
    u16 *mblock;
    u16 *tblock;
    s32 texW;
    s32 texH;
    s32 tileW;
    s32 tileH;
    if ((texture_index != 0) && (texture_index < (texture_list->numOfTextures * 256))) {
        texture_list = (TextureHeader *) ((s32) texture_list + ((texture_index >> 16) * texture_list->textureSize));
    }
    mblock = (u16 *) (texture_list + 1);
    tblock = mblock + 0x400;
    if (texture_list->width == 64) {
        texW = 64;
        texH = 16;
        tileW = 6;
        tileH = 4;
    } else {
        texW = 32;
        texH = 32;
        tileW = 5;
        tileH = 5;
    }
    gDPLoadMultiBlock((*dlist)++, OS_K0_TO_PHYSICAL(mblock), 256, 1, G_IM_FMT_RGBA, G_IM_SIZ_16b, texW, texH, 0, 0, 0,
                      tileW, tileH, 0, 0);
    gDPLoadTextureBlock((*dlist)++, OS_K0_TO_PHYSICAL(tblock), G_IM_FMT_RGBA, G_IM_SIZ_16b, texW, texH, 0, 0, 0, tileW,
                        tileH, 0, 0);

    gCurrentTextureHeader = 0;
    gSPSetGeometryMode((*dlist)++, G_FOG);
    gForceFlags = TRUE;
    gCurrentRenderFlags = RENDER_NONE;
    gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(dRenderSettingsBlinkingLights[0]),
                       numberOfGfxCommands(dRenderSettingsBlinkingLights[0]));
}

/**
 * Set whether to use an opaque render mode for sprites.
 */
void sprite_opaque(s32 setting) {
    gSpriteOpaque = setting;
    gForceFlags = TRUE;
}

#ifdef NON_EQUIVALENT
// Reasonably certain the macros are correct, but definitely need to figure out the gDkrDmaDisplayList Gfx arguments
void func_8007BF34(Gfx **dlist, s32 arg1) {
    s32 temp_a1;
    s32 temp_t8;
    s32 temp_v0_3;

    if ((arg1 != gCurrentRenderFlags) || (gForceFlags != 0)) {
        gDPPipeSync((*dlist)++);
        if (((gCurrentRenderFlags * 16) < 0) || (gForceFlags != 0)) {
            gSPSetGeometryMode((*dlist)++, G_FOG);
        }
        temp_a1 = arg1 & 0xF7FFFFFF & ~gBlockedRenderFlags;
        temp_v0_3 = temp_a1 & 2;
        if (((gCurrentRenderFlags & 2) != temp_v0_3) || (gForceFlags != 0)) {
            if (temp_v0_3 != 0) {
                gSPSetGeometryMode((*dlist)++, G_ZBUFFER);
            } else {
                gSPClearGeometryMode((*dlist)++, G_ZBUFFER);
            }
        }
        gForceFlags = 0;
        gCurrentRenderFlags = temp_a1;
        temp_t8 = temp_a1 & ~0x800;
        if (gSpriteOpaque == 0) {
            if ((gCurrentRenderFlags & 0x200) != 0) {
                gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(dRenderSettingsSpriteCld[((temp_t8 >> 1) & 1) * 16]),
                                   numberOfGfxCommands(dRenderSettingsSpriteCld[0]));
            } else {
                gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(dRenderSettingsSpriteXlu[(temp_t8 - 16) * 16]),
                                   numberOfGfxCommands(dRenderSettingsSpriteXlu[0]));
            }
        } else {
            gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(dRenderSettingsCommon[temp_t8 * 16]),
                               numberOfGfxCommands(dRenderSettingsCommon[0]));
        }
        gCurrentTextureHeader = NULL;
        gUsingTexture = 1;
    }
}
#else
GLOBAL_ASM("asm/non_matchings/textures_sprites/func_8007BF34.s")
#endif

/**
 * Official Name: texLoadSprite
 */
GLOBAL_ASM("asm/non_matchings/textures_sprites/func_8007C12C.s")

Sprite *func_8007C52C(s32 arg0) {
    if ((arg0 < 0) || (arg0 >= D_80126358)) {
        return NULL;
    }

    if (gSpriteCache[(arg0 << 1) + 1] == -1) {
        return NULL;
    }
    return (Sprite *) gSpriteCache[(arg0 << 1) + 1];
}

s32 get_texture_size_from_id(s32 id) {
    s32 textureRomOffset;
    TempTexHeader *new_var2;
    u32 textureTable;
    s32 size;
    s32 new_var3;
    s32 textureTableType;
    s32 numOfTextures;
    TempTexHeader *new_var4;

    textureTable = ASSET_TEXTURES_2D;
    textureTableType = 0;
    if (id & 0x8000) {
        textureTable = ASSET_TEXTURES_3D;
        textureTableType = 1;
        id &= 0x7FFF;
    }
    if ((id >= gTextureAssetID[textureTableType]) || (id < 0)) {
        return 0;
    }
    textureRomOffset = gTextureAssetTable[textureTableType][id];
    new_var3 = textureRomOffset;
    size = gTextureAssetTable[textureTableType][id + 1] - new_var3;
    new_var2 = gTempTextureHeader;
    if (new_var2->header.isCompressed) {
        load_asset_to_address(textureTable, (u32) new_var2, textureRomOffset, 0x28);
        new_var4 = gTempTextureHeader;
        size = byteswap32((u8 *) (&new_var4->uncompressedSize));
    }
    new_var2 = gTempTextureHeader;
    numOfTextures = new_var2->header.numOfTextures;
    return (((numOfTextures >> 8) & 0xFFFF) * 0x60) + size;
}

s32 load_sprite_info(s32 spriteIndex, s32 *numOfInstancesOut, s32 *unkOut, s32 *numFramesOut, s32 *formatOut,
                     s32 *sizeOut) {
    TextureHeader *tex;
    s32 i;
    Sprite *new_var2;
    s32 j;
    s32 start;
    s32 size;
    s32 new_var;

    if ((spriteIndex < 0) || (spriteIndex >= gSpriteTableNum)) {
    textureCouldNotBeLoaded:
        *numOfInstancesOut = 0;
        *unkOut = 0;
        *numFramesOut = 0;
        return 0;
    }
    start = gSpriteOffsetTable[spriteIndex];
    size = gSpriteOffsetTable[spriteIndex + 1] - start;
    new_var2 = gCurrentSprite;
    new_var = size;
    load_asset_to_address(ASSET_SPRITES, (u32) new_var2, start, new_var);
    tex = load_texture(new_var2->unkC.val[0] + new_var2->baseTextureId);
    if (tex != NULL) {
        *formatOut = tex->format & 0xF;
        free_texture(tex);
        *sizeOut = 0;
        for (i = 0; i < new_var2->numberOfFrames; i++) {
            for (j = new_var2->unkC.val[i]; j < (s32) new_var2->unkC.val[i + 1]; j++) {
                *sizeOut += get_texture_size_from_id(new_var2->baseTextureId + j);
            }
        }
        *numFramesOut = new_var2->numberOfFrames;
        *numOfInstancesOut = new_var2->numberOfInstances;
        *unkOut = new_var2->unk6;
        return 1;
    }
    goto textureCouldNotBeLoaded;
}

GLOBAL_ASM("asm/non_matchings/textures_sprites/func_8007CA68.s")

/* Official name: texFreeSprite */
void free_sprite(Sprite *sprite) {
    s32 i;
    s32 frame;

    if (sprite != NULL) {
        sprite->numberOfInstances--;
        if (sprite->numberOfInstances <= 0) {
            for (i = 0; i < D_80126358; i++) {
                if ((s32) sprite == gSpriteCache[(i << 1) + 1]) {
                    for (frame = 0; frame < sprite->numberOfFrames; frame++) {
                        free_texture(sprite->frames[frame]);
                    }
                    free_from_memory_pool(sprite);
                    gSpriteCache[(i << 1) + 0] = -1;
                    gSpriteCache[(i << 1) + 1] = -1; // ?
                    break;
                }
            }
        }
    }
}

#ifdef NON_MATCHING
void func_8007CDC0(Sprite *sprite1, Sprite *sprite2, s32 arg2) {
    s32 sprUnk4;
    s32 sprUnk6;
    s32 y0;
    s32 x0;
    s32 x1;
    s32 y1;
    s32 xTemp;
    s32 yTemp;
    s32 left;
    s32 var_t5;
    s32 curVertIndex;
    s32 j;
    s32 texWidth;
    s32 texHeight;
    s32 i;
    Vertex *vertices;
    Vertex *curVerts;
    Triangle *triangles;
    u8 *temp_a3;
    Gfx *dlptr;
    TextureHeader *tex;

    temp_a3 = &sprite1->unkC.val[arg2];
    sprUnk4 = sprite1->numberOfInstances;
    sprUnk6 = sprite1->unk6;
    i = temp_a3[0];
    j = temp_a3[1];
    dlptr = D_80126364;
    vertices = D_80126360;
    triangles = D_80126368;
    if (i < j) {
        tex = sprite2->frames[i];
        sprite2->unk6 = ((tex->flags & 0xFFFF) & 0x3B);
    }
    curVertIndex = 0;
    var_t5 = 0;
    while (i < j) {
        tex = sprite2->frames[i];
        texWidth = tex->width;
        texHeight = tex->height;
        xTemp = (tex->unk3 - sprUnk4);
        x0 = xTemp;
        x1 = (xTemp + texWidth) - 1;
        yTemp = (sprUnk6 - tex->unk4);
        y0 = yTemp - 1;
        y1 = (yTemp - texHeight);
        curVerts = vertices;
        vertices[0].x = x0;
        vertices[0].y = y0;
        vertices[0].z = 0;
        vertices[1].x = x1;
        vertices[1].y = y0;
        vertices[1].z = 0;
        vertices[2].x = x1;
        vertices[2].y = y1;
        vertices[2].z = 0;
        vertices[3].x = x0;
        vertices[3].y = y1;
        vertices[3].z = 0;
        vertices += 4;
        gDkrDmaDisplayList(dlptr++, OS_K0_TO_PHYSICAL(tex->cmd), tex->numberOfCommands);
        if (var_t5 == 0) {
            left = j - i;
            if (left >= 6) {
                left = 5;
            }
            gSPVertexDKR(dlptr++, OS_K0_TO_PHYSICAL(curVerts), (left * 4), 1);
        }
        gSPPolygon(dlptr++, OS_K0_TO_PHYSICAL(triangles), 2, 1);
        triangles[0].flags = 0x40;
        triangles[0].vi0 = curVertIndex + 3;
        triangles[0].vi1 = curVertIndex + 2;
        triangles[0].vi2 = curVertIndex + 1;
        triangles[0].uv0.u = (texWidth - 1) << 5;
        triangles[0].uv0.v = (texHeight - 1) << 5;
        triangles[0].uv1.u = (texWidth - 1) << 5;
        triangles[0].uv1.v = 0;
        triangles[0].uv2.u = 1;
        triangles[0].uv2.v = 0;
        triangles[1].flags = 0x40;
        triangles[1].vi0 = curVertIndex + 4;
        triangles[1].vi1 = curVertIndex + 3;
        triangles[1].vi2 = curVertIndex + 1;
        triangles[1].uv0.u = 1;
        triangles[1].uv0.v = (texHeight - 1) << 5;
        triangles[1].uv1.u = (texWidth - 1) << 5;
        triangles[1].uv1.v = (texHeight - 1) << 5;
        triangles[1].uv2.u = 1;
        triangles[1].uv2.v = 0;
        triangles += 2;
        curVertIndex += 4;
        var_t5 += 1;
        i += 1;
        if (var_t5 >= 5) {
            var_t5 = 0;
            curVertIndex = 0;
        }
    }
    gDPPipeSync(dlptr++);
    gSPEndDisplayList(dlptr++);
    D_80126364 = dlptr;
    D_80126360 = vertices;
    D_80126368 = triangles;
}
#else
GLOBAL_ASM("asm/non_matchings/textures_sprites/func_8007CDC0.s")
#endif

s32 get_tile_bytes(s32 type, s32 siz) {
    if (type == 0) {
        switch (siz) {
            case G_IM_SIZ_4b:
                return G_IM_SIZ_4b_TILE_BYTES;
            case G_IM_SIZ_8b:
                return G_IM_SIZ_8b_TILE_BYTES;
            case G_IM_SIZ_16b:
                return G_IM_SIZ_16b_TILE_BYTES;
            case G_IM_SIZ_32b:
                return G_IM_SIZ_32b_TILE_BYTES;
        }
    } else if (type == 1) {
        switch (siz) {
            case G_IM_SIZ_4b:
                return G_IM_SIZ_4b_INCR;
            case G_IM_SIZ_8b:
                return G_IM_SIZ_8b_INCR;
            case G_IM_SIZ_16b:
                return G_IM_SIZ_16b_INCR;
            case G_IM_SIZ_32b:
                return G_IM_SIZ_32b_INCR;
        }
    } else if (type == 2) {
        switch (siz) {
            case G_IM_SIZ_4b:
                return G_IM_SIZ_4b_SHIFT;
            case G_IM_SIZ_8b:
                return G_IM_SIZ_8b_SHIFT;
            case G_IM_SIZ_16b:
                return G_IM_SIZ_16b_SHIFT;
            case G_IM_SIZ_32b:
                return G_IM_SIZ_32b_SHIFT;
        }
    } else if (type == 3) {
        switch (siz) {
            case G_IM_SIZ_4b:
                return G_IM_SIZ_4b_BYTES;
            case G_IM_SIZ_8b:
                return G_IM_SIZ_8b_BYTES;
            case G_IM_SIZ_16b:
                return G_IM_SIZ_16b_BYTES;
            case G_IM_SIZ_32b:
                return G_IM_SIZ_32b_BYTES;
        }
    }
    return 0;
}

void build_tex_display_list(TextureHeader *tex, Gfx *dlist) {
    s32 texFlags;
    s32 cms;
    s32 cmt;
    s32 texFormat;
    s32 i;
    s32 uClamp;
    s32 vClamp;
    s32 masks;
    s32 maskt;
    u8 height;
    u8 width;
    u8 *pal;
    s32 siz;
    s32 fmt;
    s32 dxt;
    s32 firstSiz;

    tex->cmd = (s32 *) dlist;
    texFormat = tex->format & 0xF;
    texFlags = (tex->format >> 4) & 0xF;
    height = tex->height;
    width = tex->width;
    siz = 1;
    masks = 1;
    maskt = 1;
    pal = 0;
    uClamp = TRUE;
    vClamp = TRUE;
    for (i = 0; i < 7; i++) {
        if (siz < width) {
            masks = i + 1;
        }
        if (siz == width) {
            uClamp = FALSE;
        }
        if (siz < height) {
            maskt = i + 1;
        }
        if (siz == height) {
            vClamp = FALSE;
        }
        siz <<= 1;
    }
    if (uClamp || tex->flags & RENDER_CLAMP_X) {
        cms = G_TX_CLAMP;
        masks = 0;
    } else {
        cms = 0;
    }
    if (vClamp || tex->flags & RENDER_CLAMP_Y) {
        cmt = G_TX_CLAMP;
        maskt = 0;
    } else {
        cmt = 0;
    }

    switch (texFormat) {
        case TEX_FORMAT_RGBA32:
            fmt = G_IM_FMT_RGBA;
            siz = G_IM_SIZ_32b;
            if ((texFlags == 0) || (texFlags == 2)) {
                tex->flags |= RENDER_SEMI_TRANSPARENT;
            }
            break;
        case TEX_FORMAT_RGBA16:
            fmt = G_IM_FMT_RGBA;
            siz = G_IM_SIZ_16b;
            if ((texFlags == 0) || (texFlags == 2)) {
                tex->flags |= RENDER_SEMI_TRANSPARENT;
            }
            break;
        case TEX_FORMAT_IA16:
            fmt = G_IM_FMT_IA;
            siz = G_IM_SIZ_16b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_IA8:
            fmt = G_IM_FMT_IA;
            siz = G_IM_SIZ_8b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_IA4:
            fmt = G_IM_FMT_IA;
            siz = G_IM_SIZ_4b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_I8:
            fmt = G_IM_FMT_I;
            siz = G_IM_SIZ_8b;
        case TEX_FORMAT_I4:
            fmt = G_IM_FMT_I;
            siz = G_IM_SIZ_4b;
            break;
        case TEX_FORMAT_CI8:
            fmt = G_IM_FMT_CI;
            siz = G_IM_SIZ_8b;
            pal = tex->ciPaletteOffset + gCiPalettes;
            gDPLoadTLUT_pal256(dlist++, pal);
            break;
        case TEX_FORMAT_CI4:
            fmt = G_IM_FMT_CI;
            siz = G_IM_SIZ_4b;
            pal = tex->ciPaletteOffset + gCiPalettes;
            gDPLoadTLUT_pal16(dlist++, 0, pal);
            break;
    }
    if (tex->flags & RENDER_LINE_SWAP) {
        dxt = 0;
    } else {
        dxt = CALC_DXT(width, get_tile_bytes(3, siz));
    }

    if (siz == G_IM_SIZ_32b) {
        firstSiz = G_IM_SIZ_32b;
    } else {
        firstSiz = G_IM_SIZ_16b;
    }

    gDPSetTextureImage(dlist++, fmt, firstSiz, 1, OS_PHYSICAL_TO_K0(tex + 1));
    gDPSetTile(dlist++, fmt, firstSiz, 0, 0, G_TX_LOADTILE, 0, cmt, maskt, 0, cms, masks, 0);
    gDPLoadSync(dlist++);
    gDPLoadBlock(dlist++, G_TX_LOADTILE, 0, 0,
                 (((width) * (height) + get_tile_bytes(1, siz)) >> get_tile_bytes(2, siz)) - 1, dxt);
    gDPSetTile(dlist++, fmt, siz, (((width) *get_tile_bytes(0, siz)) + 7) >> 3, 0, G_TX_RENDERTILE, pal, cmt, maskt, 0,
               cms, masks, 0);
    gDPSetTileSize(dlist++, G_TX_RENDERTILE, 0, 0, ((width) -1) << G_TEXTURE_IMAGE_FRAC,
                   ((height) -1) << G_TEXTURE_IMAGE_FRAC);
    tex->numberOfCommands = ((s32) ((u8 *) dlist) - (s32) ((u8 *) tex->cmd)) >> 3;
}

/**
 * Official Name: texAnimateTexture
 */
void tex_animate_texture(TextureHeader *texture, u32 *triangleBatchInfoFlags, s32 *arg2, s32 updateRate) {
    s32 bit23Set;
    s32 bit25Set;
    s32 bit26Set;
    s32 breakVar;

    bit23Set = *triangleBatchInfoFlags & BATCH_FLAGS_UNK00800000;
    bit26Set = *triangleBatchInfoFlags & BATCH_FLAGS_UNK04000000;
    bit25Set = *triangleBatchInfoFlags & BATCH_FLAGS_UNK02000000;
    if (bit23Set) {
        if (!bit25Set) {
            if (get_random_number_from_range(0, 1000) > 985) {
                *triangleBatchInfoFlags &= ~BATCH_FLAGS_UNK04000000;
                *triangleBatchInfoFlags |= BATCH_FLAGS_UNK02000000;
            }
        } else if (!bit26Set) {
            *arg2 += texture->frameAdvanceDelay * updateRate;
            if (*arg2 >= texture->numOfTextures) {
                *arg2 = ((texture->numOfTextures * 2) - *arg2) - 1;
                if (*arg2 < 0) {
                    *arg2 = 0;
                    *triangleBatchInfoFlags &= ~(BATCH_FLAGS_UNK02000000 | BATCH_FLAGS_UNK04000000);
                } else {
                    *triangleBatchInfoFlags |= BATCH_FLAGS_UNK04000000;
                }
            }
        } else {
            *arg2 -= texture->frameAdvanceDelay * updateRate;
            if (*arg2 < 0) {
                *arg2 = 0;
                *triangleBatchInfoFlags &= ~(BATCH_FLAGS_UNK02000000 | BATCH_FLAGS_UNK04000000);
            }
        }
    } else if (bit25Set) {
        if (!bit26Set) {
            *arg2 += texture->frameAdvanceDelay * updateRate;
        } else {
            *arg2 -= texture->frameAdvanceDelay * updateRate;
        }
        do {
            breakVar = FALSE;
            if (*arg2 < 0) {
                *arg2 = -*arg2;
                *triangleBatchInfoFlags &= ~BATCH_FLAGS_UNK04000000;
                breakVar = TRUE;
            }
            if (*arg2 >= texture->numOfTextures) {
                *arg2 = ((texture->numOfTextures * 2) - *arg2) - 1;
                *triangleBatchInfoFlags |= BATCH_FLAGS_UNK04000000;
                breakVar = TRUE;
            }
        } while (breakVar);
    } else if (!bit26Set) {
        *arg2 += texture->frameAdvanceDelay * updateRate;
        while (*arg2 >= texture->numOfTextures) {
            *arg2 -= texture->numOfTextures;
        }
    } else {
        *arg2 -= texture->frameAdvanceDelay * updateRate;
        while (*arg2 < 0) {
            *arg2 += texture->numOfTextures;
        }
    }
}

void func_8007F1E8(LevelHeader_70 *arg0) {
    s32 i;

    arg0->unk4 = 0;
    arg0->unk8 = 0;
    arg0->unkC = 0;
    arg0->red = arg0->red2;
    arg0->green = arg0->green2;
    arg0->blue = arg0->blue2;
    arg0->alpha = arg0->alpha2;
    for (i = 0; i < arg0->unk0; i++) {
        arg0->unkC += arg0->unk18[i].unk0;
    }
}

/**
 * Official name: updateColourCycle
 */
void update_colour_cycle(LevelHeader_70 *arg0, s32 updateRate) {
    s32 temp;
    s32 curIndex;
    s32 nextIndex;
    u32 next_red;
    u32 cur_red;
    u32 next_green;
    u32 next_blue;
    u32 next_alpha;
    u32 cur_green;
    u32 cur_blue;
    u32 cur_alpha;
    LevelHeader_70 *cur;
    LevelHeader_70 *next;

    if (arg0->unk0 >= 2) {
        arg0->unk8 += updateRate;
        while (arg0->unk8 >= arg0->unkC) {
            if (!temp) {}
            arg0->unk8 -= arg0->unkC;
        }
        while (arg0->unk8 >= arg0->unk18[arg0->unk4].unk0) {
            arg0->unk8 -= arg0->unk18[arg0->unk4].unk0;
            arg0->unk4++;
            if (arg0->unk4 >= arg0->unk0) {
                arg0->unk4 = 0;
            }
        }

        curIndex = arg0->unk4;
        nextIndex = curIndex + 1;
        if (nextIndex >= arg0->unk0) {
            nextIndex = 0;
        }

        cur = (LevelHeader_70 *) (&((LevelHeader_70_18 *) arg0)[curIndex]);
        temp = (arg0->unk8 << 16) / (cur->unk18->unk0);
        cur_red = cur->red2;
        cur_green = cur->green2;
        cur_blue = cur->blue2;
        cur_alpha = cur->alpha2;

        next = (LevelHeader_70 *) (&((LevelHeader_70_18 *) arg0)[nextIndex]);
        next_red = next->red2;
        next_green = next->green2;
        next_blue = next->blue2;
        next_alpha = next->alpha2;

        next = arg0;
        arg0->red = (((next_red - cur_red) * temp) >> 16) + cur_red;
        arg0->green = (((next_green - cur_green) * temp) >> 16) + cur_green;
        arg0->blue = (((next_blue - cur_blue) * temp) >> 16) + cur_blue;
        arg0->alpha = (((next_alpha - cur_alpha) * temp) >> 16) + cur_alpha;
    }
}

/**
 * Official name: resetMixCycle
 */
void init_pulsating_light_data(PulsatingLightData *data) {
    s32 i;
    data->currentFrame = 0;
    data->time = 0;
    data->totalTime = 0;
    data->outColorValue = data->frames[0].value;
    for (i = 0; i < data->numberFrames; i++) {
        data->totalTime += data->frames[i].time;
    }
}

/**
 * Official Name: updateMixCycle
 */
void update_pulsating_light_data(PulsatingLightData *data, s32 timeDelta) {
    s32 thisFrameIndex, nextFrameIndex;

    if (data->numberFrames > 1) {
        data->time += timeDelta;
        while (data->time >= data->totalTime) {
            data->time -= data->totalTime;
        }
        while (data->time >= data->frames[data->currentFrame].time) {
            data->time -= data->frames[data->currentFrame].time;
            data->currentFrame++;
            if (data->currentFrame >= data->numberFrames) {
                data->currentFrame = 0;
            }
        }
        thisFrameIndex = data->currentFrame;
        nextFrameIndex = thisFrameIndex + 1;
        if (nextFrameIndex >= data->numberFrames) {
            nextFrameIndex = 0;
        }

        data->outColorValue = data->frames[thisFrameIndex].value +
                              ((data->frames[nextFrameIndex].value * data->time) / data->frames[thisFrameIndex].time);
    }
}

#ifdef NON_EQUIVALENT
void func_8007F594(Gfx **dlist, u32 index, u32 primitiveColor, u32 environmentColor) {
    Gfx *tempDlist = D_800DF3A8;

    if (index >= 2) {
        index = 2;
        tempDlist = D_800DF3D8;
    }

    // There are issues with the structure, but this should be equivalent functionality-wise.
    gSPDisplayList((*dlist)++, tempDlist);
    gDkrDmaDisplayList((*dlist)++, OS_PHYSICAL_TO_K0(D_800DF410[index]), numberOfGfxCommands(D_800DF410[0]));
    gDPSetPrimColorRGBA((*dlist)++, primitiveColor);
    gDPSetEnvColorRGBA((*dlist)++, environmentColor);
}
#else
GLOBAL_ASM("asm/non_matchings/textures_sprites/func_8007F594.s")
#endif
