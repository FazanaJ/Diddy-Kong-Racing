#include "textures_sprites.h"
#include "gzip.h"
#include "asset_loading.h"
#include "memory.h"
#include "math_util.h"
#include "tracks.h"
#include "game_ui.h"
#include "main.h"

#define TEX_HEADER_COUNT 175
#define TEX_SPRITE_COUNT 50
#define TEX_PALLETE_COUNT 20
#define TEX_PALLETE_COLOURS 640

/************ .data ************/

u32 gTexColourTag = COLOUR_TAG_MAGENTA;
s32 gSpriteOpaque = TRUE;

// See "include/f3ddkr.h" for the defines

Gfx dRenderSettingsVtxAlpha[][2] = {
    // Semitransparent Vertex Alpha'd surface (Zsorted)
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA, DKR_CC_UNK0, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN0, G_RM_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA, DKR_CC_UNK0, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN0, G_RM_ZB_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN1, G_RM_XLU_SURF2),
    DRAW_TABLE_ENTRY(G_CC_MODULATEIA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, DKR_RM_UNKNOWN1, G_RM_ZB_XLU_SURF2),
    // Semitransparent Vertex Alpha'd surface (No Zsort)
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
    // Cutout Surface with primitive colour
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                     G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2,
                     G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour (Copy)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2,
                     G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2,
                     G_RM_AA_ZB_XLU_INTER, G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour and fog
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, DKR_CC_UNK2, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_RA_ZB_TEX_EDGE2, G_RM_FOG_SHADE_A,
                     G_RM_AA_ZB_XLU_INTER2),
    // Cutout Surface with primitive colour and fog (Zsorted interpenetrating)
    DRAW_TABLE_GROUP(G_CC_MODULATEIA_PRIM, DKR_CC_UNK2, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                     G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_INTER2 | Z_UPD,
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
                         G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_ZB_XLU_SURF,
                         G_RM_ZB_XLU_SURF2, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2, G_RM_ZB_XLU_SURF,
                         G_RM_ZB_XLU_SURF2),
    // Opaque Surface with fog
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2,
                         G_RM_FOG_SHADE_A, G_RM_RA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_RA_ZB_OPA_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface with fog
    DRAW_TABLE_GROUP_EXT(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2,
                         G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A,
                         G_RM_ZB_XLU_SURF2),
};

Gfx dRenderSettingsCutout[][2] = {
    // Cutout surface
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2),
    // Cutout surface ZB
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2),
    // Semitransparent surface
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2),
    // Semitransparent surface ZB
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM, DKR_OMH_1CYC_BILERP, G_RM_RA_ZB_TEX_EDGE, G_RM_RA_ZB_TEX_EDGE2),
    // Cutout surface with fog
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2),
    // Cutout surface ZB with fog
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_RA_ZB_TEX_EDGE2),
    // Semitransparent surface with fog
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2),
    // Semitransparent surface ZB with fog
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_RA_ZB_TEX_EDGE2),
};

Gfx dRenderSettingsDecal[][2] = {
    // Opaque Decal.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_ZB_OPA_DECAL, G_RM_ZB_OPA_DECAL2),
    // Semitransparent Decal.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA, DKR_OMH_1CYC_BILERP, G_RM_ZB_XLU_DECAL, G_RM_ZB_XLU_DECAL2),
    // Opaque Decal with fog.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_DECAL2),
    // Semitransparent Decal with fog.
    DRAW_TABLE_ENTRY(G_CC_MODULATEIDECALA, DKR_CC_UNK1, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_DECAL2),
};

Gfx dRenderSettingsSolidColourVtxAlpha[][2] = {
    DRAW_TABLE_ENTRY(DKR_CC_UNK3, G_CC_MODULATEIA_PRIM2, DKR_OMH_2CYC_POINT, G_RM_NOOP, G_RM_XLU_SURF2),
    DRAW_TABLE_ENTRY(DKR_CC_UNK3, G_CC_MODULATEIA_PRIM2, DKR_OMH_2CYC_POINT, G_RM_NOOP, G_RM_ZB_XLU_SURF2),
};

Gfx dRenderSettingsSolidColour[][2] = {
    // Opaque Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK4, DKR_CC_UNK4, DKR_OMH_1CYC_BILERP, G_RM_OPA_SURF, G_RM_OPA_SURF2, G_RM_AA_OPA_SURF,
                     G_RM_AA_OPA_SURF2, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2, G_RM_AA_ZB_OPA_INTER,
                     G_RM_AA_ZB_OPA_INTER2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK4, DKR_CC_UNK4, DKR_OMH_1CYC_BILERP, G_RM_XLU_SURF, G_RM_XLU_SURF2, G_RM_AA_XLU_SURF,
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
    // Opaque Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK7, DKR_CC_UNK8, DKR_OMH_2CYC_BILERP, G_RM_NOOP, G_RM_OPA_SURF2, G_RM_NOOP,
                     G_RM_AA_OPA_SURF2, G_RM_NOOP, G_RM_ZB_OPA_SURF2, G_RM_NOOP, G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface
    DRAW_TABLE_GROUP(DKR_CC_UNK7, DKR_CC_UNK8, DKR_OMH_2CYC_BILERP, G_RM_NOOP, G_RM_XLU_SURF2, G_RM_NOOP,
                     G_RM_AA_XLU_SURF2, G_RM_NOOP, G_RM_ZB_XLU_SURF2, G_RM_NOOP, G_RM_AA_ZB_XLU_SURF2),
    // Opaque Surface with fog
    DRAW_TABLE_GROUP(DKR_CC_UNK7, DKR_CC_UNK8, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_OPA_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_OPA_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    // Semitransparent Surface with fog
    DRAW_TABLE_GROUP(DKR_CC_UNK7, DKR_CC_UNK8, DKR_OMH_2CYC_BILERP, G_RM_FOG_SHADE_A, G_RM_XLU_SURF2, G_RM_FOG_SHADE_A,
                     G_RM_AA_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_ZB_XLU_SURF2, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2),
};

Gfx dBasicRenderSettingsZBOff[] = {
    gsDPPipeSync(),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPClearGeometryMode(G_ZBUFFER | G_FOG),
    gsSPEndDisplayList(),
};

Gfx dBasicRenderSettingsZBOn[] = {
    gsDPPipeSync(),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPClearGeometryMode(G_FOG),
    gsSPSetGeometryMode(G_ZBUFFER),
    gsSPEndDisplayList(),
};

Gfx dBasicRenderModes[][2] = {
    DRAW_TABLE_ENTRY(DKR_CC_UNK9, DKR_CC_UNK9, DKR_OMH_1CYC_BILERP_NOPERSP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2),
    DRAW_TABLE_ENTRY(G_CC_BLENDPE, G_CC_BLENDPE, DKR_OMH_1CYC_BILERP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2),
    DRAW_TABLE_ENTRY(DKR_CC_UNK9, G_CC_PASS2, DKR_OMH_2CYC_BILERP, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2)
};

/*******************************/

/************ .bss ************/

s32 *gTextureCache;

u8 *gCiPalettes;
s32 gNumberOfLoadedTextures;
s32 D_80126334;
s32 gTextureAssetID[2];
s32 gCiPalettesSize;
s32 D_80126344;

s32 *gSpriteCache;

s32 gSpriteTableSize;
s32 gSpriteCacheCount;
s32 D_8012635C;
Vertex *D_80126360;
Gfx *D_80126364;
Triangle *D_80126368;

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
    s32 *table;

    gTextureCache = mempool_alloc_safe(sizeof(TextureHeader) * TEX_HEADER_COUNT, PP_RAM_ASSET_CACHE);
    gCiPalettes = mempool_alloc_safe(TEX_PALLETE_COLOURS, PP_RAM_ASSET_CACHE);
    gNumberOfLoadedTextures = 0;
    gCiPalettesSize = 0;

    table = (s32 *) load_asset_section_from_rom(ASSET_TEXTURES_2D_TABLE);
    for (i = 0; table[i] != -1; i++) {}
    gTextureAssetID[TEX_TABLE_2D] = --i;
    mempool_free(table);

    table = (s32 *) load_asset_section_from_rom(ASSET_TEXTURES_3D_TABLE);
    for (i = 0; table[i] != -1; i++) {}
    gTextureAssetID[TEX_TABLE_3D] = --i;
    mempool_free(table);

    gSpriteCache = mempool_alloc_safe(sizeof(Sprite) * TEX_SPRITE_COUNT, PP_RAM_ASSET_CACHE);
    gSpriteCacheCount = 0;
    table = (s32 *) load_asset_section_from_rom(ASSET_SPRITES_TABLE);
    gSpriteTableSize = 0;
    while (table[gSpriteTableSize] != -1) {
        gSpriteTableSize++;
    }
    gSpriteTableSize--;
    mempool_free(table);
    D_80126344 = 0;
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

//  Official Name: texLoadTexture
TextureHeader *load_texture(s32 arg0) {
    TextureHeader *tex;
    TextureHeader *texTemp;
    u32 temp_a1;
    s32 assetIndex;
    s32 assetOffset;
    s32 assetSize;
    s32 paletteOffset;
    s32 assetSection;
    s32 texIndex;
    s32 assetTable;
    s32 i;
    u16 numberOfTextures;
    s32 sp3C;
    s32 tableID;
    u8 headerHeap[sizeof(TempTexHeader)];
    TempTexHeader *header = (TempTexHeader *) &headerHeap;

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
        if (arg0 == gTextureCache[ASSETCACHE_ID(i)]) {
            tex = (TextureHeader *) gTextureCache[ASSETCACHE_PTR(i)];
            tex->numberOfInstances++;
            return tex;
        }
    }
    if (assetTable == 0) {
        tableID = ASSET_TEXTURES_2D_TABLE;
    } else {
        tableID = ASSET_TEXTURES_3D_TABLE;
    }
    assettable_seek_s32(assetIndex, &assetOffset, &assetSize, tableID);
    load_asset_to_address(assetSection, (u32) header, assetOffset, sizeof(TempTexHeader));
    numberOfTextures = header->header.numOfTextures >> 8;
    if (!header->header.isCompressed) {
        tex = (TextureHeader *) mempool_alloc(numberOfTextures * (sizeof(Gfx) * 12) + assetSize, gTexColourTag);
        if (tex == NULL) {
            return NULL;
        }
        load_asset_to_address(assetSection, (u32) tex, assetOffset, assetSize);
    } else {
        sp3C = byteswap32((u8 *) &header->uncompressedSize) + sizeof(TextureHeader);
        tex = (TextureHeader *) mempool_alloc(numberOfTextures * (sizeof(Gfx) * 12) + sp3C, gTexColourTag);
        if (tex == NULL) {
            return NULL;
        }
        temp_a1 = (((s32) tex + sp3C) - assetSize);
        temp_a1 = (s32) temp_a1 - (s32) temp_a1 % 16;
        load_asset_to_address(assetSection, temp_a1, assetOffset, assetSize);
        gzip_inflate((u8 *) (temp_a1 + sizeof(TextureHeader)), (u8 *) tex);
        assetSize = sp3C - sizeof(TextureHeader);
    }
    texIndex = -1;
    for (i = 0; i < gNumberOfLoadedTextures; i++) {
        if (gTextureCache[ASSETCACHE_ID(i)] == -1) {
            texIndex = i;
        }
    }
    if (texIndex == -1) {
        texIndex = gNumberOfLoadedTextures;
        gNumberOfLoadedTextures++;
    }
    gTextureCache[ASSETCACHE_ID(texIndex)] = arg0;
    gTextureCache[ASSETCACHE_PTR(texIndex)] = (s32) tex;
    paletteOffset = -1;
    if ((tex->format & 0xF) == TEX_FORMAT_CI4) {
        if (D_80126344 == 0) {
            load_asset_to_address(ASSET_EMPTY_14, (u32) &gCiPalettes[gCiPalettesSize], tex->ciPaletteOffset, 32);
            tex->ciPaletteOffset = gCiPalettesSize;
            gCiPalettesSize += 32; // (32 bytes / 2 bytes per color) = 16 colors.
        }
        paletteOffset = gCiPalettesSize - 32;
    }
    if ((tex->format & 0xF) == TEX_FORMAT_CI8) {
        if (D_80126344 == 0) {
            load_asset_to_address(ASSET_EMPTY_14, (u32) &gCiPalettesSize[gCiPalettes], tex->ciPaletteOffset, 128);
            tex->ciPaletteOffset = gCiPalettesSize;
            gCiPalettesSize += 128; // (128 bytes / 2 bytes per color) = 64 colors.
        }
        paletteOffset = gCiPalettesSize - 128;
    }
    D_80126344 = 0;

    assetOffset = (s32) align16((u8 *) ((s32) tex + assetSize));
    texTemp = tex;
    for (i = 0; i < numberOfTextures; i++) {
        material_init(texTemp, (Gfx *) assetOffset);
        if (paletteOffset >= 0) {
            texTemp->ciPaletteOffset = paletteOffset;
            assetOffset += (sizeof(Gfx) * 6); // I'm guessing it takes 6 f3d commands to load the palette
        }
        assetOffset += (sizeof(Gfx) * 12); // I'm guessing it takes 12 f3d commands to load the texture
        texTemp = (TextureHeader *) ((s32) texTemp + texTemp->textureSize);
    }
    if (gCiPalettesSize >= 0x280) {
        return NULL;
    }
    return tex;
}

/**
 * This function attempts to free the texture from memory.
 * It checks if the refcount is zero, then finds the cache entry, before clearing it.
 * Official Name: texFreeTexture
 */
void tex_free(TextureHeader *tex) {
    s32 i;

    if (tex != 0) {
        if ((--tex->numberOfInstances) <= 0) {
            for (i = 0; i < gNumberOfLoadedTextures; i++) {
                if ((s32) tex == gTextureCache[ASSETCACHE_PTR(i)]) {
                    mempool_free(tex);

                    gTextureCache[ASSETCACHE_ID(i)] = -1;
                    gTextureCache[ASSETCACHE_PTR(i)] = -1;
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
void rendermode_reset(Gfx **dList) {
    gCurrentTextureHeader = NULL;
    gCurrentRenderFlags = RENDER_NONE;
    gUsingTexture = FALSE;
    gForceFlags = TRUE;
    gBlockedRenderFlags = RENDER_NONE;
    gUsePrimColour = FALSE;
    gSPSetGeometryMode((*dList)++, G_SHADING_SMOOTH | G_SHADE | G_ZBUFFER);
    gDPPipeSync((*dList)++);
}

/**
 * Enables usage of combiners utilising the indidual primitive colours.
 */
void tex_primcolour_on(void) {
    gUsePrimColour = TRUE;
    gForceFlags = TRUE;
}

/**
 * Disables usage of combiners utilising the indidual primitive colours.
 */
void tex_primcolour_off(void) {
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
void material_set_no_tex_offset(Gfx **dList, TextureHeader *texhead, u32 flags) {
    material_set(dList, texhead, flags, 0);
}

/**
 * Load a texture from memory into texture memory.
 * Also set render mode, combine mode and othermodes based on flags.
 * Also tracks which modes are active, to prevent setting them again if they're already active.
 * A number can be attached that adds a texture address offset. An example of this being used is
 * the numbered doors in the hub, to change what number is written on it.
 */
void material_set(Gfx **dList, TextureHeader *texhead, s32 flags, s32 texOffset) {
    s32 forceFlags;
    s32 doPipeSync;
    s32 dlIndex;
    Gfx *cmd;
    s32 loadTex;
    s32 aaMode;

    forceFlags = gForceFlags;
    doPipeSync = TRUE;
    loadTex = FALSE;

    if (texhead != NULL) {
        if (texOffset && (texOffset < texhead->numOfTextures << 8)) {
            texhead = (TextureHeader *) ((s8 *) texhead + ((texOffset >> 16) * texhead->textureSize));
        }

        flags |= texhead->flags;
        if (texhead != gCurrentTextureHeader) {
            gDkrDmaDisplayList((*dList)++, OS_PHYSICAL_TO_K0(texhead->cmd), texhead->numberOfCommands);
            DEBUG_VAR(gDebug->misc.texLoads, gDebug->misc.texLoads + 1);
            loadTex = TRUE;
            gCurrentTextureHeader = texhead;
            doPipeSync = FALSE;
        }
        if (gUsingTexture == FALSE) {
            forceFlags = TRUE;
            gUsingTexture = TRUE;
        }
    } else if (gUsingTexture) {
        forceFlags = TRUE;
        gUsingTexture = FALSE;
    }

    aaMode = gAntiAliasing + 1;

    flags = (gUsePrimColour) ? (flags & (RENDER_DECAL | RENDER_Z_COMPARE | RENDER_SEMI_TRANSPARENT))
                             : (flags & (RENDER_VTX_ALPHA | RENDER_DECAL | RENDER_Z_UPDATE | RENDER_CUTOUT | RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE));
    flags &= ~gBlockedRenderFlags;
    flags = (flags & RENDER_VTX_ALPHA) ? flags & ~RENDER_FOG_ACTIVE : flags & ~RENDER_Z_UPDATE;

    if (flags != gCurrentRenderFlags || forceFlags) {
        if ((flags & RENDER_VTX_ALPHA) != (gCurrentRenderFlags & RENDER_VTX_ALPHA) || gForceFlags) {
            if (flags & RENDER_VTX_ALPHA || gUsePrimColour) {
                gSPClearGeometryMode((*dList)++, G_FOG);
            } else {
                gSPSetGeometryMode((*dList)++, G_FOG);
            }
        }

        if ((flags & RENDER_Z_COMPARE) != (gCurrentRenderFlags & RENDER_Z_COMPARE) || gForceFlags) {
            if (flags & RENDER_Z_COMPARE) {
                gSPSetGeometryMode((*dList)++, G_ZBUFFER);
            } else {
                gSPClearGeometryMode((*dList)++, G_ZBUFFER);
            }
        }

        gForceFlags = FALSE;
        gCurrentRenderFlags = flags;
        if (!gUsingTexture) {
            if (flags & RENDER_VTX_ALPHA) {
                cmd = dRenderSettingsSolidColourVtxAlpha[(flags & RENDER_Z_COMPARE) >> 1];
                goto run;
            }
            cmd = dRenderSettingsSolidColour[flags & (RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE)];
            loadTex = FALSE;
            goto run;
        }

        if (gUsePrimColour) {
            if (flags & RENDER_DECAL && flags & RENDER_Z_COMPARE) {
                dlIndex = 0;
                if (flags & RENDER_ANTI_ALIASING) {
                    //dlIndex |= 1; // Anti Aliasing
                }
                if (flags & RENDER_SEMI_TRANSPARENT) {
                    dlIndex |= 2; // Z Compare
                }
                cmd = dRenderSettingsPrimOverlay[dlIndex];
                goto run;
            }
            cmd = dRenderSettingsPrimCol[flags];
            goto run;
        }

        if (flags & RENDER_DECAL && flags & RENDER_Z_COMPARE) {
            dlIndex = (flags & (RENDER_SEMI_TRANSPARENT | RENDER_FOG_ACTIVE)) >> 2;
            cmd = dRenderSettingsDecal[dlIndex];
            goto run;
        }

        if (flags & RENDER_CUTOUT) {
            dlIndex = (flags & (RENDER_Z_COMPARE | RENDER_SEMI_TRANSPARENT | RENDER_FOG_ACTIVE)) >> 1;
            cmd = dRenderSettingsCutout[dlIndex];
            goto run;
        }

        flags &= ~RENDER_DECAL;
        if (flags & RENDER_VTX_ALPHA) {
            dlIndex = (flags & (RENDER_Z_COMPARE)) >> 1;
            if (flags & RENDER_Z_UPDATE) {
                dlIndex |= 2; // Z write
            } else {
                gSPSetGeometryMode((*dList)++, G_ZBUFFER);
                gCurrentRenderFlags |= RENDER_Z_COMPARE;
            }
            cmd = dRenderSettingsVtxAlpha[dlIndex];
            goto run;
        }

        dlIndex = aaMode;
        if (flags & RENDER_Z_COMPARE) {
            dlIndex += 3;
        }
        if (flags & RENDER_SEMI_TRANSPARENT) {
            dlIndex += 6;
        }
        if (flags & RENDER_FOG_ACTIVE) {
            dlIndex += 12;
        }
        cmd = dRenderSettingsCommon_ext[dlIndex];
        run:
        if (doPipeSync) {
            gDPPipeSync((*dList)++);
        }
        gDkrDmaDisplayList((*dList)++, cmd, numberOfGfxCommands(dRenderSettingsCommon_ext[0]));
    }
}

/**
 * Loads the texture and render settings for the blinking lights seen in Spaceport Alpha.
 */
void material_set_blinking_lights(Gfx **dList, TextureHeader *texture_list, u32 flags, s32 texture_index) {
    u16 *mblock;
    u16 *tblock;
    s32 width;
    s32 height;
    s32 maskW;
    s32 maskH;
    if (texture_index != 0 && texture_index < (texture_list->numOfTextures * 256)) {
        texture_list = (TextureHeader *) ((s32) texture_list + ((texture_index >> 16) * texture_list->textureSize));
    }
    mblock = (u16 *) (texture_list + 1);
    tblock = mblock + 0x400;
    if (texture_list->width == 64) {
        width = 64;
        height = 16;
        maskW = 6;
        maskH = 4;
    } else {
        width = 32;
        height = 32;
        maskW = 5;
        maskH = 5;
    }
    gDPLoadMultiBlock((*dList)++, OS_K0_TO_PHYSICAL(mblock), 256, 1, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, 0, 0, 0, maskW, maskH, 0, 0);
    gDPLoadTextureBlock((*dList)++, OS_K0_TO_PHYSICAL(tblock), G_IM_FMT_RGBA, G_IM_SIZ_16b, width, height, 0, 0, 0, maskW, maskH, 0, 0);

    gCurrentTextureHeader = 0;
    flags &= (RENDER_ANTI_ALIASING | RENDER_Z_COMPARE | RENDER_SEMI_TRANSPARENT | RENDER_FOG_ACTIVE);
    gSPSetGeometryMode((*dList)++, G_FOG);

    if (flags & RENDER_Z_COMPARE) {
        gSPSetGeometryMode((*dList)++, G_ZBUFFER);
    } else {
        gSPClearGeometryMode((*dList)++, G_ZBUFFER);
    }
    gForceFlags = TRUE;
    gCurrentRenderFlags = RENDER_NONE;
    gDkrDmaDisplayList((*dList)++, OS_PHYSICAL_TO_K0(dRenderSettingsBlinkingLights[flags]),
                       numberOfGfxCommands(dRenderSettingsBlinkingLights[0]));
}

/**
 * Set whether to use an opaque render mode for sprites.
 */
void sprite_opaque(s32 setting) {
    gSpriteOpaque = setting;
    gForceFlags = TRUE;
}

/**
 * Load a texture from memory into texture memory.
 * Much simpler than the regular material function, only having modes for opaque and transparent.
 */
void material_load_simple(Gfx **dList, s32 flags) {
    Gfx *cmd;
    if (flags != gCurrentRenderFlags || gForceFlags) {
        gDPPipeSync((*dList)++);
        if ((gCurrentRenderFlags & RENDER_VTX_ALPHA) || gForceFlags) {
            gSPSetGeometryMode((*dList)++, G_FOG);
        }
        flags &= ~RENDER_VTX_ALPHA;
        flags &= ~gBlockedRenderFlags;
        if ((flags & RENDER_Z_COMPARE) != (gCurrentRenderFlags & RENDER_Z_COMPARE) || gForceFlags) {
            if (flags & RENDER_Z_COMPARE) {
                gSPSetGeometryMode((*dList)++, G_ZBUFFER);
            } else {
                gSPClearGeometryMode((*dList)++, G_ZBUFFER);
            }
        }
        gForceFlags = FALSE;
        gCurrentRenderFlags = flags;
        flags &= ~RENDER_DECAL;
        if (gSpriteOpaque == FALSE) {
            if (gCurrentRenderFlags & RENDER_PRESERVE_COVERAGE) {
                cmd = dRenderSettingsSpriteCld[(flags >> 1) & 1];
            } else {
                cmd = dRenderSettingsSpriteXlu[(flags - 16)];
            }
        } else {
            flags &= ~RENDER_CUTOUT | RENDER_ANTI_ALIASING;
            cmd = dRenderSettingsCommon[flags];
        }
        gDkrDmaDisplayList((*dList)++, cmd, numberOfGfxCommands(dRenderSettingsCommon[0]));
        gCurrentTextureHeader = NULL;
        gUsingTexture = TRUE;
    }
}

/**
 * Official Name: texLoadSprite
 */
Sprite *func_8007C12C(s32 spriteID, s32 arg1) {
    Sprite *refSprite;
    Sprite *newSprite;
    s32 cacheNum;
    Sprite* sprite;
    TextureHeader* tex;
    s32 i;
    s32 size;
    s8 allocFailed;
    s8 cacheFull;
    s16 frameCount;
    s32 allocSize;
    s32 offset;
    u8 spriteBuf[0x200];

    D_8012635C = arg1;
    if (spriteID < 0 || spriteID >= gSpriteTableSize) {
        return NULL;
    }
    
    for (i = 0, cacheFull = 0; i < gSpriteCacheCount; i++) {
        if (spriteID == gSpriteCache[ASSETCACHE_ID(i)]) {
            refSprite = gSpriteCache[ASSETCACHE_PTR(i)];
            refSprite->numberOfInstances++;
            return refSprite;
        }
    }
    cacheNum = -1;
    i = 0;
    while (i < gSpriteCacheCount) {
        // @fake
        if (newSprite) {}
        if (gSpriteCache[ASSETCACHE_ID(i)] == -1) {
            cacheNum = i;
        }
        i++;
    }
    if (cacheNum == -1) {
        cacheFull = TRUE;
        cacheNum = gSpriteCacheCount;
        gSpriteCacheCount++;
    }
    sprite = &spriteBuf;
    assettable_seek_s32(spriteID, &offset, &size, ASSET_SPRITES_TABLE);
    load_asset_to_address(12, sprite, offset, size);

    frameCount = sprite->unkC.val[sprite->numberOfFrames];
    size = frameCount * 4;
    allocSize = size * sizeof(Vertex);
    allocSize += size << 3;
    allocSize += sprite->numberOfFrames * sizeof(Gfx);
    allocSize += frameCount << 4 << 1;
    allocSize += size;
    allocSize += (s32) align16(0x10); 
    allocSize += (s32) align16((sprite->numberOfFrames * 4));
    newSprite = (Sprite *) mempool_alloc(allocSize, COLOUR_TAG_MAGENTA);
    if (newSprite == NULL) {
        if (cacheFull) {
            gSpriteCacheCount--;
        }
        return NULL;
    }

    size = (s32)newSprite + (s32)align16(sizeof(Sprite)) + (s32)align16(sprite->numberOfFrames * 4);\
    D_80126368 = (Triangle *) size;
    D_80126364 = (Gfx *)(&((u8*)D_80126368)[frameCount << 5]); // `<< 5` is `sizeof(Triangle) * 2`
    D_80126360 = (Vertex *) ((Gfx *)(((s32)&((u8*)D_80126364)[frameCount << 5]) + (sprite->numberOfFrames << 3)));
    newSprite->gfx[0] = (Gfx *) &((u8*)D_80126360)[frameCount * sizeof(Vertex) * 4];
    
    allocFailed = FALSE;
    for (i = 0; i < frameCount; i++) {
        gTexColourTag = COLOUR_TAG_LIME;
        tex = load_texture(sprite->baseTextureId + i);
        newSprite->frames[i] = tex;
        if (newSprite->frames[i] == NULL) {
            allocFailed = TRUE;
        }
        gTexColourTag = COLOUR_TAG_MAGENTA;
        D_80126344 = 1;
    }
    D_80126344 = 0;
    if (allocFailed) {
        for (i = 0; i < frameCount; i++) {
            tex = (TextureHeader *) newSprite->frames[i];
            if (tex != NULL) {
                tex_free(tex);
            }
        }
        if (cacheFull) {
            gSpriteCacheCount--;
        }
        mempool_free(newSprite);
        return NULL;
    }
    newSprite->numberOfFrames = frameCount;
    newSprite->baseTextureId = sprite->numberOfFrames;
    for (i = 0; i < sprite->numberOfFrames; i++) {
        newSprite->unkC.ptr[i] = (u8 *) D_80126364;
        func_8007CDC0(sprite, newSprite, i);
    }
    if (gSpriteCacheCount >= 100) {
        return NULL;
    }
    gSpriteCache[ASSETCACHE_ID(cacheNum)] = spriteID;
    gSpriteCache[ASSETCACHE_PTR(cacheNum)] = newSprite;
    newSprite->numberOfInstances = 1;
    return newSprite;
}

/**
 * Gets the sprite cache index from the argument.
 * Returns NULL if the entry is invalid.
 */
s32 sprite_cache_index(s32 cacheID) {
    if (cacheID < 0 || cacheID >= gSpriteCacheCount) {
        return NULL;
    }

    if (gSpriteCache[ASSETCACHE_PTR(cacheID)] == -1) {
        return NULL;
    }
    return gSpriteCache[ASSETCACHE_PTR(cacheID)];
}

s32 tex_asset_size(s32 id) {
    s32 textureRomOffset;
    UNUSED s32 pad;
    u32 textureTable;
    s32 size;
    s32 textureTableType;
    s32 numOfTextures;
    s32 tableID;
    u8 headerHeap[sizeof(TempTexHeader)];
    TempTexHeader *header = (TempTexHeader *) &headerHeap;

    textureTable = ASSET_TEXTURES_2D;
    textureTableType = TEX_TABLE_2D;
    if (id & ASSET_MASK_TEX3D) {
        textureTable = ASSET_TEXTURES_3D;
        textureTableType = TEX_TABLE_3D;
        id &= (ASSET_MASK_TEX3D - 1);
    }
    if (id >= gTextureAssetID[textureTableType] || id < 0) {
        return 0;
    }
    if (textureTable == TEX_TABLE_2D) {
        tableID = ASSET_TEXTURES_2D_TABLE;
    } else {
        tableID = ASSET_TEXTURES_3D_TABLE;
    }
    assettable_seek_s32(id, &textureRomOffset, &size, tableID);
    if (header->header.isCompressed) {
        load_asset_to_address(textureTable, (u32) header, textureRomOffset, sizeof(TempTexHeader));
        size = byteswap32((u8 *) (&header->uncompressedSize));
    }
    numOfTextures = header->header.numOfTextures;
    return (((numOfTextures >> 8) & 0xFFFF) * (sizeof(Gfx) * 12)) + size;
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
    u8 spriteBuf[0x200];

    if ((spriteIndex < 0) || (spriteIndex >= gSpriteTableSize)) {
    textureCouldNotBeLoaded:
        *numOfInstancesOut = 0;
        *unkOut = 0;
        *numFramesOut = 0;
        return 0;
    }
    new_var2 = &spriteBuf;
    assettable_seek_s32(spriteIndex, &start, &size, ASSET_SPRITES_TABLE);
    new_var = size;
    load_asset_to_address(ASSET_SPRITES, (u32) new_var2, start, size);
    set_texture_colour_tag(PP_RAM_SPRITES);
    tex = load_texture(new_var2->unkC.val[0] + new_var2->baseTextureId);
    set_texture_colour_tag(COLOUR_TAG_MAGENTA);
    if (tex != NULL) {
        *formatOut = TEX_FORMAT(tex->format);
        tex_free(tex);
        *sizeOut = 0;
        for (i = 0; i < new_var2->numberOfFrames; i++) {
            for (j = new_var2->unkC.val[i]; j < (s32) new_var2->unkC.val[i + 1]; j++) {
                *sizeOut += tex_asset_size(new_var2->baseTextureId + j);
            }
        }
        *numFramesOut = new_var2->numberOfFrames;
        *numOfInstancesOut = new_var2->numberOfInstances;
        *unkOut = new_var2->drawFlags;
        return 1;
    }
    goto textureCouldNotBeLoaded;
}

void func_8007CA68(s32 arg0, s32 arg1, s32 *arg2, s32 *arg3, s32 *arg4) {
    Sprite *sprite;
    TextureHeader *tex;
    s32 temp_a0;
    s32 temp_v1;
    s32 var_s1;
    s32 var_s3;
    s32 var_s4;
    s32 var_s5;
    s32 var_s6;
    s32 temp_a1;
    s32 temp_a2;
    s32 offset;
    s32 size;
    u8 spriteBuf[0x200];

    if ((arg0 < 0) || (arg0 >= gSpriteTableSize)) {
        *arg2 = 0;
        *arg3 = 0;
        return;
    }

    // Must be on the same line. (maybe a macro?)
    // clang-format off
    sprite = &spriteBuf;
    assettable_seek_s32(arg0, &offset, &size, ASSET_SPRITES_TABLE);
    load_asset_to_address(12, sprite, offset, size);
    // clang-format on

    if (sprite->numberOfFrames < arg1) {
    failedExit:
        *arg2 = 0;
        *arg3 = 0;
        *arg4 = 0;
        return;
    }
    tex = load_texture(sprite->unkC.val[arg1] + sprite->baseTextureId);
    if (tex == NULL) {
        goto failedExit;
    }
    *arg4 = tex_asset_size(sprite->unkC.val[arg1] + sprite->baseTextureId);
    var_s3 = tex->unk3 - sprite->numberOfInstances;
    var_s4 = sprite->drawFlags - tex->unk4;
    temp_a1 = tex->width;
    temp_a2 = tex->height;
    var_s5 = var_s3 + temp_a1;
    var_s6 = var_s4 - temp_a2;
    tex_free(tex);

    for (var_s1 = sprite->unkC.val[arg1] + 1; var_s1 < sprite->unkC.val[arg1 + 1]; var_s1++) {
        tex = load_texture(sprite->baseTextureId + var_s1);
        if (tex == NULL) {
            goto failedExit;
        }
        *arg4 += tex_asset_size(sprite->baseTextureId + var_s1);
        temp_v1 = tex->unk3 - sprite->numberOfInstances;
        temp_a0 = sprite->drawFlags - tex->unk4;
        temp_a1 = tex->width;
        temp_a2 = tex->height;
        if (temp_v1 < var_s3) {
            var_s3 = temp_v1;
        }
        if (var_s5 < temp_v1 + temp_a1) {
            var_s5 = temp_v1 + temp_a1;
        }
        if (temp_a0 - temp_a2 < var_s6) {
            var_s6 = temp_a0 - temp_a2;
        }
        if (var_s4 < temp_a0) {
            var_s4 = temp_a0;
        }
        tex_free(tex);
    }
    *arg2 = var_s5 - var_s3;
    *arg3 = var_s4 - var_s6;
}

/**
 * This function attempts to free the sprite from memory.
 * It checks if the refcount is zero, then finds the cache entry, before clearing it.
 * Official Name: texFreeSprite
 */
void sprite_free(Sprite *sprite) {
    s32 i;
    s32 frame;

    if (sprite != NULL) {
        sprite->numberOfInstances--;
        if (sprite->numberOfInstances <= 0) {
            for (i = 0; i < gSpriteCacheCount; i++) {
                if (sprite == (Sprite *) gSpriteCache[ASSETCACHE_PTR(i)]) {
                    for (frame = 0; frame < sprite->numberOfFrames; frame++) {
                        tex_free(sprite->frames[frame]);
                    }
                    mempool_free(sprite);
                    gSpriteCache[ASSETCACHE_ID(i)] = -1;
                    gSpriteCache[ASSETCACHE_PTR(i)] = -1;
                    break;
                }
            }
        }
    }
}

void func_8007CDC0(Sprite *sprite1, Sprite *sprite2, s32 arg2) {
    UNUSED s32 pad[2];
    s32 spriteCount;
    s32 spriteDrawFlags;
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
    Gfx *dlptr;
    TextureHeader *tex;

    spriteCount = sprite1->numberOfInstances;
    spriteDrawFlags = sprite1->drawFlags;
    i = sprite1->unkC.val[arg2];
    j = sprite1->unkC.val[arg2 + 1];
    dlptr = D_80126364;
    vertices = D_80126360;
    triangles = D_80126368;
    if (i < j) {
        tex = sprite2->frames[i];
        sprite2->drawFlags = ((tex->flags & 0xFFFF) & 0x3B);
    }
    curVertIndex = 0;
    var_t5 = 0;
    while (i < j) {
        curVerts = vertices;
        tex = sprite2->frames[i];
        texWidth = tex->width;
        texHeight = tex->height;
        xTemp = tex->unk3 - spriteCount;
        yTemp = spriteDrawFlags - tex->unk4;
        vertices->x = xTemp;
        vertices->y = yTemp - 1;
        vertices->z = 0;
        vertices++;
        vertices->x = xTemp + texWidth - 1;
        vertices->y = yTemp - 1;
        vertices->z = 0;
        vertices++;
        vertices->x = xTemp + texWidth - 1;
        vertices->y = yTemp - texHeight;
        vertices->z = 0;
        vertices++;
        vertices->x = xTemp;
        vertices->y = yTemp - texHeight;
        vertices->z = 0;
        vertices++;
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

s32 get_tile_bytes(s32 type, s32 siz) {
    if (type == 0) {
        switch (siz) {
            case G_IM_SIZ_8b:
                return 0;
            case G_IM_SIZ_16b:
                return 1;
            case G_IM_SIZ_32b:
                return 1;
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

/**
 * Build the display list for the texture.
 * Takes the texture properties from the header and then constructs the F3D gfx commands.
 * Certain texture types will also have draw mode flag overrides.
 */
void material_init(TextureHeader *tex, Gfx *_dList) {
    s32 texFormat;
    s32 texRenderMode;
    s32 width;
    s32 height;
    s32 cms;
    s32 cmt;
    s32 i;
    s32 uClamp;
    s32 vClamp;
    s32 masks;
    s32 maskt;
    u8 *pal;
    s32 size;
    s32 fmt;
    s32 dxt;
    s32 shiftWidth;
    s32 firstSiz;
    Gfx *dList;

    tex->cmd = dList = _dList;
    texFormat = TEX_FORMAT(tex->format);
    texRenderMode = TEX_RENDERMODE(tex->format);
    height = tex->height;
    width = tex->width;
    size = 1;
    masks = 1;
    maskt = 1;
    pal = 0;
    uClamp = TRUE;
    vClamp = TRUE;
    for (i = 0; i < 7; i++) {
        if (size < width) {
            masks = i + 1;
        }
        if (size == width) {
            uClamp = FALSE;
        }
        if (size < height) {
            maskt = i + 1;
        }
        if (size == height) {
            vClamp = FALSE;
        }
        size *= 2;
    }

    if (uClamp || tex->flags & RENDER_CLAMP_X) {
        cms = G_TX_CLAMP;
        masks = G_TX_NOMASK;
    } else {
        cms = G_TX_WRAP;
    }

    if (vClamp || tex->flags & RENDER_CLAMP_Y) {
        cmt = G_TX_CLAMP;
        maskt = G_TX_NOMASK;
    } else {
        cmt = G_TX_WRAP;
    }

    switch (texFormat) {
        case TEX_FORMAT_RGBA32:
            fmt = G_IM_FMT_RGBA;
            size = G_IM_SIZ_32b;
            if (texRenderMode == 0 || texRenderMode == 2) {
                tex->flags |= RENDER_SEMI_TRANSPARENT;
            }
            break;
        case TEX_FORMAT_RGBA16:
            fmt = G_IM_FMT_RGBA;
            size = G_IM_SIZ_16b;
            if (texRenderMode == 0 || texRenderMode == 2) {
                tex->flags |= RENDER_SEMI_TRANSPARENT;
            }
            break;
        case TEX_FORMAT_IA16:
            fmt = G_IM_FMT_IA;
            size = G_IM_SIZ_16b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_IA8:
            fmt = G_IM_FMT_IA;
            size = G_IM_SIZ_8b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_IA4:
            fmt = G_IM_FMT_IA;
            size = G_IM_SIZ_4b;
            tex->flags |= RENDER_SEMI_TRANSPARENT;
            break;
        case TEX_FORMAT_I8:
            fmt = G_IM_FMT_I;
            size = G_IM_SIZ_8b;
            break;
        case TEX_FORMAT_I4:
            fmt = G_IM_FMT_I;
            size = G_IM_SIZ_4b;
            break;
        case TEX_FORMAT_CI8:
            fmt = G_IM_FMT_CI;
            size = G_IM_SIZ_8b;
            pal = tex->ciPaletteOffset + gCiPalettes;
            gDPLoadTLUT_pal256(dList++, pal);
            break;
        case TEX_FORMAT_CI4:
            fmt = G_IM_FMT_CI;
            size = G_IM_SIZ_4b;
            pal = tex->ciPaletteOffset + gCiPalettes;
            gDPLoadTLUT_pal16(dList++, 0, pal);
            break;
    }
    if (tex->flags & RENDER_LINE_SWAP) {
        dxt = 0;
    } else {
        if (size == G_IM_SIZ_4b) {
            dxt = CALC_DXT_4b(width);
        } else {
            dxt = CALC_DXT(width, get_tile_bytes(3, size));
        }
    }

    if (size == G_IM_SIZ_32b) {
        firstSiz = G_IM_SIZ_32b;
    } else {
        firstSiz = G_IM_SIZ_16b;
    }

    if (size == G_IM_SIZ_4b) {
        shiftWidth = (width) >> 1;
    } else {
        shiftWidth = (width) << get_tile_bytes(0, size);
    }

    gDPSetTextureImage(dList++, fmt, firstSiz, 1, OS_PHYSICAL_TO_K0(tex + 1));
    gDPSetTile(dList++, fmt, firstSiz, 0, 0, G_TX_LOADTILE, 0, cmt, maskt, G_TX_NOLOD, cms, masks, G_TX_NOLOD);
    gDPLoadSync(dList++);
    gDPLoadBlock(dList++, G_TX_LOADTILE, 0, 0, ((width * height + get_tile_bytes(1, size)) >> get_tile_bytes(2, size)) - 1, dxt);
    //gDPTileSync(dList++);
    gDPSetTile(dList++, fmt, size, (shiftWidth + 7) >> 3, 0, G_TX_RENDERTILE, pal, cmt, maskt, 0, cms, masks, 0);
    gDPSetTileSize(dList++, G_TX_RENDERTILE, 0, 0, (width - 1) << G_TEXTURE_IMAGE_FRAC, (height -1) << G_TEXTURE_IMAGE_FRAC);
    if (texFormat == TEX_FORMAT_CI4 || texFormat == TEX_FORMAT_CI8) {
        gDPSetTextureLUT(dList++, G_TT_RGBA16);
    }

    tex->numberOfCommands = ((s32) ((u8 *) dList) - (s32) ((u8 *) tex->cmd)) >> 3;
}

/**
 * Returns the palette offset from the heap.
 */
s32 tex_palette_id(s16 paletteID) {
    return (s32) (paletteID + gCiPalettes);
}

/**
 * Official Name: texAnimateTexture
 */
void tex_animate_texture(TextureHeader *texture, u32 *triangleBatchInfoFlags, s32 *arg2, s32 updateRate) {
    s32 bit23Set;
    s32 bit25Set;
    s32 bit26Set;
    s32 breakVar;

    if (gMenuStopUpdating) {
        return;
    }

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
    arg0->rgba.r = arg0->rgba2.r;
    arg0->rgba.g = arg0->rgba2.g;
    arg0->rgba.b = arg0->rgba2.b;
    arg0->rgba.a = arg0->rgba2.a;
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
        cur_red = cur->rgba2.r;
        cur_green = cur->rgba2.g;
        cur_blue = cur->rgba2.b;
        cur_alpha = cur->rgba2.a;

        next = (LevelHeader_70 *) (&((LevelHeader_70_18 *) arg0)[nextIndex]);
        next_red = next->rgba2.r;
        next_green = next->rgba2.g;
        next_blue = next->rgba2.b;
        next_alpha = next->rgba2.a;

        next = arg0;
        arg0->rgba.r = (((next_red - cur_red) * temp) >> 16) + cur_red;
        arg0->rgba.g = (((next_green - cur_green) * temp) >> 16) + cur_green;
        arg0->rgba.b = (((next_blue - cur_blue) * temp) >> 16) + cur_blue;
        arg0->rgba.a = (((next_alpha - cur_alpha) * temp) >> 16) + cur_alpha;
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

/**
 * Initialises some draw modes for rendering semitransparent geometry.
 * Sets everything needed to render, but relies on the texture being set manually.
 */
void gfx_init_basic_xlu(Gfx **dList, u32 index, u32 primitiveColor, u32 environmentColor) {
    Gfx *gfxTemp;
    Gfx *tempDlist;
    u32 tempIndex;

    tempDlist = dBasicRenderSettingsZBOff;
    tempIndex = index;

    if (tempIndex >= DRAW_BASIC_2CYCLE) {
        tempIndex = DRAW_BASIC_2CYCLE;
        tempDlist = dBasicRenderSettingsZBOn;
    }

    gfxTemp = *dList;
    gSPDisplayList(gfxTemp++, tempDlist);
    gDkrDmaDisplayList(gfxTemp++, OS_PHYSICAL_TO_K0(dBasicRenderModes[tempIndex]),
                       numberOfGfxCommands(dBasicRenderModes[0]));
    gDPSetPrimColorRGBA(gfxTemp++, primitiveColor);
    gDPSetEnvColorRGBA(gfxTemp++, environmentColor);
    *dList = gfxTemp;
}
