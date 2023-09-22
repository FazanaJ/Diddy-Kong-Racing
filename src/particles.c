/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800AE270 */

#include "particles.h"
#include "types.h"
#include "macros.h"
#include "asset_enums.h"
#include "memory.h"
#include "textures_sprites.h"
#include "thread0_epc.h"
#include "objects.h"
#include "asset_loading.h"
#include "math_util.h"
#include "tracks.h"

/************ .rodata ************/

const char D_800E8840[] = "\n\nUnknown trigger type in initParticleTrigger %d, Max %d.\n\n";
const char D_800E887C[] = "\n\nUnknown particle type in initParticleTrigger %d, Max %d.\n\n";
const char D_800E88BC[] = "\n\nUnknown trigger type in initParticleTrigger %d, Max %d.\n\n";
const char D_800E88F8[] = "Sprite Particle buffer is full.\n";
const char D_800E891C[] = "Triangle Particle buffer is full.\n";
const char D_800E8940[] = "Rectangle Particle buffer is full.\n";
const char D_800E8964[] = "Line Particle buffer is full.\n";
const char D_800E8980[] = "Point Particle buffer is full.\n";
const char D_800E89A4[] = "\n\nCan't allocate space for unknown particle type.";
const char D_800E89D8[] = "\n\nParticle has been freed twice, this is Super Safe, Honest!\n";
const char D_800E8A18[] = "\n\nSprite Particle Buffer is empty.\n\n";
const char D_800E8A40[] = "\n\nTriangle Particle Buffer is empty.\n\n";
const char D_800E8A68[] = "\n\nRectangle Particle Buffer is empty.\n\n";
const char D_800E8A90[] = "\n\nLine Particle buffer is empty.\n\n";
const char D_800E8AB4[] = "\n\nPoint Particle buffer is empty.\n\n";
const char D_800E8AD8[] = "\n\nCan't deallocate space for unknown particle type.\n\n";
const char D_800E8B10[] = "\nError :: trigger %x has no reference to point %x";
const char D_800E8B44[] = "\nError :: particle %x is not indexed correctly in trigger list %x (%d >> %p)";

/*********************************/

/************ .data ************/

// I woundn't be suprised if most of these zeroes are really just null pointers.
s32 D_800E2CA0 = 0;
s32 D_800E2CA4 = 0;
ParticleType *D_800E2CA8 = NULL;
s32 D_800E2CAC = 0;
s32 D_800E2CB0 = 0;
ParticleType *D_800E2CB4 = NULL;
s32 D_800E2CB8 = 0;
s32 D_800E2CBC = 0;
ParticleType *D_800E2CC0 = NULL;
s32 D_800E2CC4 = 0;
s32 D_800E2CC8 = 0;
ParticleType *D_800E2CCC = NULL;
s32 D_800E2CD0 = 0;
s32 D_800E2CD4 = 0;
Particle *D_800E2CD8 = NULL;
UNUSED s32 D_800E2CDC = 0; // Only ever and checked for being less than 512.
s32 *D_800E2CE0 = NULL;
s32 *D_800E2CE4 = NULL;
s32 gParticlesAssetTableCount = 0;
s32 *gParticlesAssets = NULL;

unk800E2CF0 **gParticlesAssetTable = NULL;
s32 gParticleBehavioursAssetTableCount = 0;
s32 *gParticleBehavioursAssets = NULL;
ParticleBehavior **gParticleBehavioursAssetTable = NULL;
ColourRGBA D_800E2D00[2] = {{{{ 0 }}}, {{{ 0 }}}};

// Are these just Triangles?
unk800E2D08 D_800E2D08[5] = {
    { 0x4000, 0x0102, 0x0100, 0x0000, 0x0000, 0x01E0, 0x0100, 0x01E0 },
    { 0x4000, 0x0203, 0x0100, 0x0000, 0x0100, 0x01E0, 0x01E0, 0x01E0 },
    { 0x4001, 0x0204, 0x0000, 0x0000, 0x0100, 0x0000, 0x0000, 0x01E0 },
    { 0x4002, 0x0305, 0x0100, 0x0000, 0x01E0, 0x0000, 0x01E0, 0x01E0 },
    { 0x4000, 0x0103, 0x0100, 0x0000, 0x0000, 0x01E0, 0x01E0, 0x01E0 },
};

Vec3s D_800E2D58[5] = {
    {{{ 0x0000, 0x01FF, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
};

// Are these just Triangles?
unk800E2D08 D_800E2D78[8] = {
    { 0x0000, 0x0105, 0x0000, 0x0000, 0x0080, 0x0000, 0x0080, 0x01FF },
    { 0x0000, 0x0504, 0x0000, 0x0000, 0x0080, 0x01FF, 0x0000, 0x01FF },
    { 0x0001, 0x0206, 0x0080, 0x0000, 0x0100, 0x0000, 0x0100, 0x01FF },
    { 0x0001, 0x0605, 0x0080, 0x0000, 0x0100, 0x01FF, 0x0080, 0x01FF },
    { 0x0002, 0x0307, 0x0100, 0x0000, 0x0180, 0x0000, 0x0180, 0x01FF },
    { 0x0002, 0x0706, 0x0100, 0x0000, 0x0180, 0x01FF, 0x0100, 0x01FF },
    { 0x0003, 0x0004, 0x0180, 0x0000, 0x0200, 0x0000, 0x0200, 0x01FF },
    { 0x0003, 0x0407, 0x0180, 0x0000, 0x0200, 0x01FF, 0x0180, 0x01FF },
};

Vec3s D_800E2DF8[8] = {
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
    {{{ 0x0000, 0x0000, 0x01FF }}},
    {{{ 0x0000, 0x01FF, 0x01FF }}},
};

s16 D_800E2E28 = 0;

f32 gParticleGravityTable[8] = {
    0.0f, 0.1, 0.2f, 0.3f, 0.45f, 0.525f, 0.6f, 0.8f
};

s32 D_800E2E4C = 0;
s32 D_800E2E50 = 0;
s32 D_800E2E54 = 0;
s32 D_800E2E58 = 0;
s32 D_800E2E5C = 0;
Sprite **D_800E2E60 = NULL;
s32 D_800E2E64 = 0;

XYStruct D_800E2E68[3] = {
    { 0,  8 },
    { 7, -4 },
    { -7, -4 },
};

XYStruct D_800E2E74[4] = {
    { -6,  6 },
    { 6,  6 },
    { 6, -6 },
    { -6, -6 },
};

s32 D_800E2E84[16] = {
    0x00000000, 0xC00840FF, 0xFF6008FF, 0x404040FF, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x1070FFFF, 0x00000000, 0x00000000
};

s32 D_800E2EC4[10] = {
    0x404040FF, 0x404040FF, 0x002D00FF, 0x002D00FF, 
    0x403C0AFF, 0x403C0AFF, 0x4040FFFF, 0x4040FFFF, 
    0x404040FF, 0x404040FF
};

s32 D_800E2EEC = 0x100;

/*******************************/

/************ .bss ************/

s32 gParticleUpdateRate;
s32 D_80127C84;
s32 D_80127C88[6];
//printf.c
//thread0_epc
/******************************/

void func_800AE270(void) {
    func_800AE374();
    func_800AE438();
    func_800AE2D8();
}

void func_800AE2A0(void) {
    func_800AE374();
    func_800AE438();
    func_800AE490();
    func_800AE2D8();
}

void func_800AE2D8(void) {
    s32 i;

    if (D_800E2E60 != NULL) {
        for (i = 0; i < D_800E2E64; i++) {
            free_sprite(D_800E2E60[i]);
        }
        free_from_memory_pool(D_800E2E60);
        D_800E2E60 = 0;
    }
}

void func_800AE374(void) {
    if (D_800E2CA8 != NULL) {
        free_from_memory_pool(D_800E2CA8);
        D_800E2CA8 = NULL;
    }
    if (D_800E2CB4 != NULL) {
        free_from_memory_pool(D_800E2CB4);
        D_800E2CB4 = NULL;
    }
    if (D_800E2CC0 != NULL) {
        free_from_memory_pool(D_800E2CC0);
        D_800E2CC0 = NULL;
    }
    if (D_800E2CCC != NULL) {
        free_from_memory_pool(D_800E2CCC);
        D_800E2CCC = NULL;
    }
    if (D_800E2CD8 != NULL) {
        free_from_memory_pool(D_800E2CD8);
        D_800E2CD8 = NULL;
    }
}

void func_800AE438(void) {
    if (D_800E2CE0 != NULL) {
        free_from_memory_pool(D_800E2CE0);
        D_800E2CE0 = NULL;
    }
    if (D_800E2CE4 != NULL) {
        free_from_memory_pool(D_800E2CE4);
        D_800E2CE4 = NULL;
    }
}

void func_800AE490(void) {
    if (gParticlesAssets != NULL) {
        free_from_memory_pool(gParticlesAssets);
        gParticlesAssets = NULL;
    }
    if (gParticlesAssetTable != NULL) {
        free_from_memory_pool(gParticlesAssetTable);
        gParticlesAssetTable = NULL;
    }
    if (gParticleBehavioursAssets != NULL) {
        free_from_memory_pool(gParticleBehavioursAssets);
        gParticleBehavioursAssets = NULL;
    }
    if (gParticleBehavioursAssetTable != NULL) {
        free_from_memory_pool(gParticleBehavioursAssetTable);
        gParticleBehavioursAssetTable = NULL;
    }
}

void init_particle_assets(void) {
    s32 *new_var2;
    s32 i;
    u32 new_var;

    func_800AE490();
    gParticlesAssetTable = (unk800E2CF0 **) load_asset_section_from_rom(ASSET_PARTICLES_TABLE);
    gParticlesAssetTableCount = -1; 
    while (((s32) gParticlesAssetTable[gParticlesAssetTableCount + 1]) != -1) {
        gParticlesAssetTableCount++;
    }

    gParticlesAssets = (s32 *) load_asset_section_from_rom(ASSET_PARTICLES);
    for (i = 0; i < gParticlesAssetTableCount; i++) {
    gParticlesAssetTable[i] = (unk800E2CF0 *) (((u8 *) gParticlesAssets) + ((s32) gParticlesAssetTable[i]));
    }

  gParticleBehavioursAssetTable = (ParticleBehavior **) load_asset_section_from_rom(ASSET_PARTICLE_BEHAVIORS_TABLE);
  gParticleBehavioursAssetTableCount = -1; 
  while (((s32) gParticleBehavioursAssetTable[gParticleBehavioursAssetTableCount + 1]) != -1) {
      gParticleBehavioursAssetTableCount++;
  }

    gParticleBehavioursAssets = (s32 *) load_asset_section_from_rom(ASSET_PARTICLE_BEHAVIORS);
    for (i = 0; i < gParticleBehavioursAssetTableCount; i++) {
        new_var = -1;
        gParticleBehavioursAssetTable[i] = (ParticleBehavior *) (((u8 *) gParticleBehavioursAssets) + ((s32) gParticleBehavioursAssetTable[i]));
        if (((u32) gParticleBehavioursAssetTable[i]->unk9C) != new_var) {
            new_var2 = gParticleBehavioursAssetTable[i]->unk9C;
            gParticleBehavioursAssetTable[i]->unk9C = (s32 *) get_misc_asset((s32) new_var2);
        }
    }
}

GLOBAL_ASM("asm/non_matchings/particles/func_800AE728.s")

void func_800AEE14(unk800AF024 *arg0, Vertex **arg1, Triangle **arg2) {
    s16 i;
    Vertex *temp;
    Triangle *tri;
    XYStruct *temp2;

    arg0->unk4 = 3;
    arg0->unk8 = *arg1;
    temp = *arg1;
    temp2 = &D_800E2E68[0];
    for (i = 0; i < 3; i++) {
        temp->x = temp2->x;
        temp->y = temp2->y;
        temp2 += 1;
        temp->z = 0;
        temp->r = 255;
        temp->g = 255;
        temp->b = 255;
        temp->a = 255;
        temp++;
    }
    *arg1 = temp;
    arg0->unk6 = 1;
    arg0->unkC = *arg2;
    tri = *arg2;
    tri->flags = 0x40;
    tri->vi0 = 2;
    tri->vi1 = 1;
    tri->vi2 = 0;
    tri++;
    *arg2 = tri;
}

void func_800AEEB8(unk800AF024 *arg0, Vertex **arg1, Triangle **arg2) {
    s16 i;
    Vertex *temp;
    Triangle *tri;
    XYStruct *temp2;

    arg0->unk4 = 4;
    arg0->unk8 = *arg1;
    temp = *arg1;
    temp2 = &D_800E2E74[0];
    for (i = 0; i < 4; i++) {
        temp->x = temp2->x;
        temp->y = temp2->y;
        temp2 += 1;
        temp->z = 0;
        temp->r = 255;
        temp->g = 255;
        temp->b = 255;
        temp->a = 255;
        temp++;
    }
    *arg1 = temp;
    arg0->unk6 = 2;
    arg0->unkC = *arg2;
    tri = *arg2;
    tri[0].flags = 0x40;
    tri[0].vi0 = 3;
    tri[0].uv0.u = 0;
    tri[0].vi1 = 1;
    tri[0].uv1.v = 0;
    tri[0].vi2 = 0;
    tri[0].uv2.u = 0;
    tri[0].uv2.v = 0;
    tri[1].flags = 0x40;
    tri[1].vi0 = 3;
    tri[1].uv0.u = 0;
    tri[1].vi1 = 2;
    tri[1].vi2 = 1;
    tri[1].uv2.v = 0;
    tri += 2;
    *arg2 = tri;
}

void func_800AEF88(unk800AF024 *arg0, Vertex **arg1, Triangle **arg2) {
    s32 i;
    Vertex *temp;

    arg0->unk4 = 6;
    arg0->unk6 = 4;
    arg0->unk8 = *arg1;
    arg0->unkC = *arg2;
    temp = *arg1;
    for (i = 0; i < 6; i++) {
        temp->r = 255;
        temp->g = 255;
        temp->b = 255;
        temp->a = 255;
        temp++;
    }
    *arg1 = temp;
}

void func_800AF024(unk800AF024 *arg0, Vertex **arg1, Triangle **arg2) {
    s32 i;
    Vertex *temp;

    arg0->unk4 = 8;
    arg0->unk6 = 8;
    arg0->unk8 = *arg1;
    arg0->unkC = *arg2;
    temp = *arg1;
    for (i = 0; i < 16; i++) {
        temp->r = 255;
        temp->g = 255;
        temp->b = 255;
        temp->a = 255;
        temp++;
    }
    *arg1 = temp;
}

void func_800AF0A4(Particle *particle) {
    Object_44 *temp_v0;
    Object_44_C *temp_v1;
    s16 temp_t1, temp_t8_0;
    s32 temp_t8;

    temp_v0 = (Object_44 *) particle->modelData;
    temp_v1 = temp_v0->unkC;
    temp_t8_0 = (temp_v0->unk0->unk0 - 1) << 5;
    temp_t8 = (s32)temp_t8_0;
    temp_t1 = (temp_v0->unk0->unk1 - 1) << 5;
    temp_v1->unk4 = temp_t8 >> 1;
    temp_v1->unk6 = 0;
    temp_v1->unk8 = 0;
    temp_v1->unkA = temp_t1;
    temp_v1->unkC = temp_t8;
    temp_v1->unkE = temp_t1;
}

void func_800AF0F0(Particle *particle) {
    Object_44 *temp_v0;
    Object_44_C *temp_v1;
    s16 temp_t8, temp_t1;

    temp_v0 = (Object_44 *) particle->modelData;
    temp_v1 = temp_v0->unkC;
    temp_t8 = (temp_v0->unk0->unk0 - 1) << 5;
    temp_t1 = (temp_v0->unk0->unk1 - 1) << 5;
    temp_v1->unk6 = temp_t1;
    temp_v1->unk8 = temp_t8;
    temp_v1->unk16 = temp_t8;
    temp_v1->unk18 = temp_t8;
    temp_v1->unk1A = temp_t1;
    temp_v1->unk1C = temp_t8;
}

void func_800AF134(Particle *arg0, s32 arg1, s32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    ParticleBehavior *temp;
    if (arg2 >= gParticlesAssetTableCount) {
        arg2 = 0;
    }
    if (arg1 >= gParticleBehavioursAssetTableCount) {
        arg1 = 0;
    }
    temp = gParticleBehavioursAssetTable[arg1];
    if (arg0->data.unk8 != arg2 || temp != arg0->data.behaviour) {
        func_800B2260(arg0);
        func_800AF29C(arg0, arg1, arg2, arg3, arg4, arg5);
    }
}

void partInitTrigger(Particle *particle, s32 behaviourID, s32 arg2) {
    ParticleBehavior *behaviour;

    if (behaviourID < gParticleBehavioursAssetTableCount) {
        behaviour = gParticleBehavioursAssetTable[behaviourID];
        func_800AF29C(particle, behaviourID, arg2, behaviour->velX, behaviour->velY, behaviour->velZ);
    }
}

void func_800AF29C(Particle *arg0, s32 behaviourID, s32 arg2, s16 velX, s16 velY, s16 velZ) {
    ParticleBehavior *temp_v1;
    s32 temp_v0_2;
    s32 flags;

    temp_v1 = gParticleBehavioursAssetTable[behaviourID];
    arg0->data.unk8 = arg2;
    arg0->data.baseVelX = velX;
    arg0->data.behaviour = temp_v1;
    arg0->data.baseVelY = velY;
    arg0->data.baseVelZ = velZ;
    arg0->data.unk1E = 0;

    flags = temp_v1->flags;

    if (flags & 0x4000) {
        arg0->data.flags = 0x4000;
        arg0->data.unk6 = 0;
        arg0->data.pos.x = 0.0f;
        arg0->data.pos.y = 0.0f;
        arg0->data.pos.z = 0.0f;
    } else if (flags & 0x400) {
        arg0->data.unk6 = 0;
        arg0->data.flags = 0x400;
        temp_v0_2 = gParticlesAssetTable[arg2]->lifeTime;
        if (temp_v0_2 < 0x100) {
            arg0->data.unk7 = temp_v0_2;
        } else {
            arg0->data.unk7 = 0xFF;
        }
        arg0->data.unkC_400.unkC = (s32 *) allocate_from_main_pool_safe(arg0->data.unk7 * 4, COLOUR_TAG_SEMITRANS_GREY);
        arg0->data.unkC_400.unk10 = temp_v1->unk14;
        arg0->data.unkC_400.unk12 = temp_v1->unk16;
        arg0->data.unkC_400.unk14 = temp_v1->unk22;
        arg0->data.unkC_400.unk16 = temp_v1->unk24;
    } else {
        arg0->data.flags = 0;
        arg0->data.unkC.unkC = temp_v1->unk14;
        arg0->data.unkC.unkE = temp_v1->unk16;
        arg0->data.unkC.unk10 = temp_v1->unk18;
        arg0->data.unkC.unk12 = temp_v1->unk22;
        arg0->data.unkC.unk14 = temp_v1->unk24;
        arg0->data.unkC.unk16 = temp_v1->unk26;
    }
}

#ifdef NON_EQUIVALENT

// Should be functionally equivalent.
void func_800AF404(s32 arg0) {
    s32 i;

    D_800E2E28 = (D_800E2E28 + (arg0 * 64)) & 0x1FF;
    for (i = 0; i < 5; i++) {
        D_800E2D08[i].unk6 = D_800E2D58[i].y_rotation + D_800E2E28;
        D_800E2D08[i].unkA = D_800E2D58[i].x_rotation + D_800E2E28;
        D_800E2D08[i].unkE = D_800E2D58[i].z_rotation + D_800E2E28;
    }
    for (i = 0; i < 8; i++) {
        D_800E2D78[i].unk6 = D_800E2DF8[i].y_rotation + D_800E2E28;
        D_800E2D78[i].unkA = D_800E2DF8[i].x_rotation + D_800E2E28;
        D_800E2D78[i].unkE = D_800E2DF8[i].z_rotation + D_800E2E28;
    }
}

#else
GLOBAL_ASM("asm/non_matchings/particles/func_800AF404.s")
#endif

void func_800AF52C(Object *obj, s32 arg1) {
    s32 i;
    Particle *temp_v0;
    ParticleBehavior *temp_v1;
    Particle *temp;

    temp_v0 = (Particle *) &obj->unk6C[arg1];
    temp_v1 = temp_v0->data.behaviour;
    temp_v0->data.unkA = 0;
    if (temp_v0->data.flags & 0x4000) {
        ((ObjectSegment *) temp_v0)->trans.x_position = obj->segment.trans.x_position;
        ((ObjectSegment *) temp_v0)->trans.y_position = obj->segment.trans.y_position;
        ((ObjectSegment *) temp_v0)->trans.z_position = obj->segment.trans.z_position;
    } else if (temp_v0->data.flags & 0x400) {
        temp_v0->data.unkA = gParticlesAssetTable[temp_v0->data.unk8]->colour.a << 8;

        if (temp_v0->data.unk6 > 0) { // Useless if statement, since the loop already does this.
            for (i = 0; i < temp_v0->data.unk6; i++){
                temp = temp_v0->data.unkC_60[i];
                temp->segment.particle.destroyTimer = 0;
            }
        }
        if (temp_v1->flags & 1) {
            temp_v0->data.unkC.unk10 = temp_v1->unk14;
            temp_v0->data.unkC.unk12 = temp_v1->unk16;
        }
        if (temp_v1->flags & 4) {
            temp_v0->data.unkC.unk14 = temp_v1->unk22;
            temp_v0->data.unkC.unk16 = temp_v1->unk24;
        }
    } else {
        if (temp_v1->flags & 1) {
            temp_v0->data.unk6 = 0;
            temp_v0->data.unkC.unkC = temp_v1->unk14;
            temp_v0->data.unkC.unkE = temp_v1->unk16;
            temp_v0->data.unkC.unk10 = temp_v1->unk18;
        }
        if (temp_v1->flags & 4) {
            temp_v0->data.unk7 = 0;
            temp_v0->data.unkC.unk12 = temp_v1->unk22;
            temp_v0->data.unkC.unk14 = temp_v1->unk24;
            temp_v0->data.unkC.unk16 = temp_v1->unk26;
        }
    }
    temp_v0->data.flags &= 0xFDFF;
    temp_v0->data.flags |= 0xA000;
    obj->segment.unk1A++;
}

void func_800AF6E4(Object *obj, s32 arg1) {
    Object_6C *obj6C;

    obj6C = &obj->unk6C[arg1];

    obj6C->unk4 &= 0x7FFF;
    obj->segment.unk1A--;
}

GLOBAL_ASM("asm/non_matchings/particles/func_800AF714.s")
GLOBAL_ASM("asm/non_matchings/particles/func_800AFC3C.s")

void func_800AFE5C(Particle *arg0, Particle *arg1) {
    Particle *temp_s0;
    Particle *tempObj;
    Particle *tempObj2;
    s32 i;
    ParticleBehavior *temp_s4;

    temp_s4 = arg1->data.behaviour;
    if (arg1->data.flags & 0x4000) {
        tempObj = func_800B0BAC();
        if (tempObj != NULL) {
            func_8000E9D0((Object *) tempObj);
        }
        arg1->data.flags &= 0xDFFF;
        if (arg1->data.unk6 + 64 > 255) {
            arg1->data.unk6 = 255;
        } else {
            arg1->data.unk6 += 64;
        }
    } else if (arg1->data.flags & 0x400) {
        if (arg1->data.unk6 < arg1->data.unk7) {
            temp_s0 = func_800B0698(arg0, arg1);
            arg1->data.flags &= 0xDFFF;
            if (temp_s0 != NULL) {
                func_8000E9D0((Object *) temp_s0);
                temp_s0->unk74 = arg1->data.unk6;
                temp_s0->segment.unk40 |= 0x2000;
                arg1->data.unkC_60[arg1->data.unk6] = temp_s0;
                arg1->data.unk6++;
            }
        }
    } else {
        while (arg1->data.unkA >= temp_s4->unk40) {
            arg1->data.unkA -= temp_s4->unk40;
            for (i = 0; i < temp_s4->unk42; i++) {
                tempObj2 = func_800B1130(arg0, arg1);
                if (tempObj2 != NULL) {
                    func_8000E9D0((Object *) tempObj2);
                    func_800B22FC(tempObj2, arg1->data.unkA);
                }
                arg1->data.flags &= ~0x2000;
            }
        }
    }
}

void func_800B0010(Particle *arg0, Particle *arg1, Particle *arg2, ParticleBehavior *arg3) {
    s32 flags;
    Vec3f velocityPos;
    Vec3s angle;

    if (arg3->flags & 0x70) {
        arg0->segment.x_velocity = arg3->unk30;
        arg0->segment.y_velocity = arg3->unk34;
        arg0->segment.z_velocity = arg3->unk38;
    } else {
        arg0->segment.x_velocity = 0.0f;
        arg0->segment.y_velocity = 0.0f;
        arg0->segment.z_velocity = 0.0f;
    }
    flags = arg3->behaviourFlags & (PARTICLE_VEL_Z | PARTICLE_VEL_Y | PARTICLE_VEL_X);
    if (flags) {
        if (flags & PARTICLE_VEL_X) {
            arg0->segment.x_velocity += (f32) get_random_number_from_range(-arg3->velocityRangeX1, arg3->velocityRangeX1) * 0.00001525878906;
        }
        if (flags & PARTICLE_VEL_Y) {
            arg0->segment.y_velocity += (f32) get_random_number_from_range(-arg3->velocityRangeY1, arg3->velocityRangeY1) * 0.00001525878906;
        }
        if (flags & PARTICLE_VEL_Z) {
            arg0->segment.z_velocity += (f32) get_random_number_from_range(-arg3->velocityRangeZ1, arg3->velocityRangeZ1) * 0.00001525878906;
        }
    }
    switch (arg3->flags & (0x40 | 0x20 | 0x10)) {
        case 0x10:
            arg0->segment.x_velocity += arg1->segment.x_velocity;
            arg0->segment.y_velocity += arg1->segment.y_velocity;
            arg0->segment.z_velocity += arg1->segment.z_velocity;
            break;
        case 0x40:
            arg0->segment.x_velocity *= arg1->segment.x_velocity;
            arg0->segment.y_velocity *= arg1->segment.y_velocity;
            arg0->segment.z_velocity *= arg1->segment.z_velocity;
            break;
    }
    if (arg3->flags & 4) {
        velocityPos.x = 0.0f;
        velocityPos.y = 0.0f;
        velocityPos.z = -arg3->unk3C;
        flags = arg3->behaviourFlags;
        if (flags & PARTICLE_UNK00000010) {
            velocityPos.z += (f32) get_random_number_from_range(-arg3->gravityRange2, arg3->gravityRange2) * 0.00001525878906;
        }
        if (flags & (PARTICLE_UNK00000040 | PARTICLE_UNK00000020)) {
            angle.y_rotation = arg2->data.angle.unk12;
            if (flags & PARTICLE_UNK00000020) {
                angle.y_rotation += get_random_number_from_range(-arg3->angleRangeY2, arg3->angleRangeY2);
            }
            angle.x_rotation = arg2->data.angle.unk14;
            if (flags & PARTICLE_UNK00000040) {
                angle.x_rotation += get_random_number_from_range(-arg3->angleRangeX2, arg3->angleRangeX2);
            }
            f32_vec3_apply_object_rotation3((ObjectTransform* ) &angle, (f32*) &velocityPos);
        } else {
            f32_vec3_apply_object_rotation3((ObjectTransform* ) &arg2->data.angle.unk12, (f32*) &velocityPos);
        }
        f32_vec3_apply_object_rotation((ObjectTransform* ) arg0->segment.unk3C, (f32*) &velocityPos);
        arg0->segment.x_velocity += velocityPos.x;
        arg0->segment.y_velocity += velocityPos.y;
        arg0->segment.z_velocity += velocityPos.z;
    }
}

void func_800B03C0(Particle *arg0, Particle *arg1, Particle *arg2, ParticleBehavior *behaviour) {
    s32 flags;
    Vec3f posVel;
    Vec3s angle;

    arg0->baseVelX = arg2->data.baseVelX;
    arg0->baseVelY = arg2->data.baseVelY;
    arg0->baseVelZ = arg2->data.baseVelZ;
    arg0->forwardVel = behaviour->forwardVel;
    if (behaviour->behaviourFlags & PARTICLE_FORWARDVEL) {
        arg0->forwardVel += (f32) get_random_number_from_range(-behaviour->velocityRange, behaviour->velocityRange) * 0.00001525878906; // 0.00001525878906 ~= 1.0/65536.0
    }
    if (behaviour->flags & 1) {
        posVel.x = 0.0f;
        posVel.y = 0.0f;
        posVel.z = -behaviour->unk10;
        flags = behaviour->behaviourFlags;
        if (flags & PARTICLE_UNK00000001) {
            posVel.z += (f32) get_random_number_from_range(-behaviour->gravityRange1, behaviour->gravityRange1) * 0.00001525878906;
        }
        if (flags & (PARTICLE_UNK00000004 | PARTICLE_UNK00000002)) {
            angle.y_rotation = arg2->data.angle.y_rotation;
            if (flags & PARTICLE_UNK00000002) {
                angle.y_rotation += get_random_number_from_range(-behaviour->angleRangeY1, behaviour->angleRangeY1);
            }
            angle.x_rotation = arg2->data.angle.x_rotation;
            if (flags & PARTICLE_UNK00000004) {
                angle.x_rotation += get_random_number_from_range(-behaviour->angleRangeX1, behaviour->angleRangeX1);
            }
            f32_vec3_apply_object_rotation3((ObjectTransform* ) &angle, (f32 *) &posVel);
        } else {
            f32_vec3_apply_object_rotation((ObjectTransform* ) &arg2->data.angle.y_rotation, (f32 *) &posVel);
        }
        arg0->baseVelX += posVel.x;
        arg0->baseVelY += posVel.y;
        arg0->baseVelZ += posVel.z;
    }
    if (arg0->segment.particle.movementType != PARTICLE_MOVEMENT_BASIC_PARENT) {
        f32_vec3_apply_object_rotation((ObjectTransform *) arg1, &arg0->baseVelX);
    }
    arg0->segment.trans.x_position = arg0->baseVelX;
    arg0->segment.trans.y_position = arg0->baseVelY;
    arg0->segment.trans.z_position = arg0->baseVelZ;
    if (arg0->segment.particle.movementType == PARTICLE_MOVEMENT_BASIC_PARENT) {
        f32_vec3_apply_object_rotation((ObjectTransform *) arg1, &arg0->segment.trans.x_position);
    }
    arg0->segment.trans.x_position += arg1->segment.trans.x_position;
    arg0->segment.trans.y_position += arg1->segment.trans.y_position;
    arg0->segment.trans.z_position += arg1->segment.trans.z_position;
}

Particle *func_800B0698(Particle *arg0, Particle *arg1) {
    unk800E2CF0 *sp2C;
    Particle *var_v0;
    TextureHeader **sp24;
    ParticleBehavior *sp20;
    f32 temp_f0;
    f32 temp_f14;
    f32 temp_f2;

    sp2C = gParticlesAssetTable[arg1->data.unk8];
    if (sp2C->unk0 != 4) {
        return NULL;
    }
    sp20 = arg1->data.behaviour;
    var_v0 = func_800B1CB8(4);
    if (var_v0 == NULL) {
        return var_v0;
    }
    var_v0->segment.particle.blockID = arg0->segment.particle.blockID;
    var_v0->segment.trans.flags = -OBJ_FLAGS_DEACTIVATED;
    var_v0->segment.particle.movementType = sp2C->movementType;
    var_v0->segment.unk40 = sp2C->unk2;
    var_v0->segment.unk3C = arg0;
    var_v0->unk70 = arg1;
    var_v0->segment.trans.scale = sp2C->scale * sp20->unk50;
    var_v0->segment.scaleVel = sp2C->scale * sp20->unk54;
    var_v0->segment.particle.destroyTimer = sp2C->lifeTime;
    var_v0->segment.particle.unk34 = 0.0f;
    var_v0->segment.particle.unk38 = 0.0f;
    var_v0->colour.r = sp2C->colour.r;
    var_v0->colour.g = sp2C->colour.g;
    var_v0->colour.b = sp2C->colour.b;
    var_v0->colour.a = sp2C->colour.a;
    if (var_v0->segment.unk40 & 0x800 && arg0->unk54_ptr != NULL) {
        var_v0->brightness = *arg0->unk54_ptr * 255.0f;
    } else {
        var_v0->brightness = 255;
    }
    var_v0->opacityTimer = sp2C->opacityTimer;
    if (arg1->data.flags & 0x100) {
        var_v0->opacity = arg1->data.unkA;
    } else {
        var_v0->opacity = sp2C->unkC << 8;
    }
    if (sp2C->unkC < 255) {
        if (var_v0->segment.unk40 & 0x1000) {
            var_v0->segment.trans.flags |= 0x100;
        } else {
            var_v0->segment.trans.flags |= 0x80;
        }
    }
    if (var_v0->opacityTimer < var_v0->segment.particle.destroyTimer) {
        var_v0->opacityVel = ((sp2C->unkD << 8) - (var_v0->opacity & 0xFFFF)) / (var_v0->segment.particle.destroyTimer - var_v0->opacityTimer);
    } else {
        var_v0->opacityVel = 0;
    }
    func_800B03C0(var_v0, arg0, arg1, sp20);
    if (sp20->flags & 0x80) {
        var_v0->segment.trans.y_rotation = sp20->angleOffsetY;
        var_v0->segment.trans.x_rotation = sp20->angleOffsetX;
        var_v0->segment.trans.z_rotation = sp20->angleOffsetZ;
    } else {
        var_v0->segment.trans.y_rotation = arg0->segment.trans.y_rotation + sp20->angleOffsetY;
        var_v0->segment.trans.x_rotation = arg0->segment.trans.x_rotation + sp20->angleOffsetX;
        var_v0->segment.trans.z_rotation = arg0->segment.trans.z_rotation + sp20->angleOffsetZ;
    }
    var_v0->angleVelY = sp20->angleVelY;
    var_v0->angleVelX = sp20->angleVelX;
    var_v0->angleVelZ = sp20->angleVelZ;
    func_800B0010(var_v0, arg0, arg1, sp20);
    var_v0->gravity = gParticleGravityTable[(var_v0->segment.unk40 >> 4) & 7];
    if (var_v0->segment.particle.movementType == PARTICLE_MOVEMENT_VELOCITY) {
        temp_f0 = var_v0->segment.x_velocity;
        temp_f2 = var_v0->segment.y_velocity;
        temp_f14 = var_v0->segment.z_velocity;
        var_v0->forwardVel = sqrtf((temp_f0 * temp_f0) + (temp_f2 * temp_f2) + (temp_f14 * temp_f14));
    }
    if (sp20->flags & 2) {
        arg1->data.unkC.unk10 += sp20->unk1C;
        arg1->data.unkC.unk12 += sp20->unk1E;
    }
    if (sp20->flags & 8) {
        arg1->data.unkC.unk14 += sp20->unk2A;
        arg1->data.unkC.unk16 += sp20->unk2C;
    }
    var_v0->segment.unk1A = sp2C->unk6;
    var_v0->segment.textureFrame = 0;
    sp24 = (TextureHeader **) var_v0->modelData;
    if (sp2C->unk4 == -1) {
        *sp24 = NULL;
    } else {
        *sp24 = load_texture(sp2C->unk4);
        if ((*sp24) != NULL) {
            if ((*sp24)->flags & 4) {
                if (var_v0->segment.unk40 & 0x1000) {
                    var_v0->segment.trans.flags |= 0x100;
                } else {
                    var_v0->segment.trans.flags |= 0x80;
                }
            }
            if ((var_v0->segment.unk40 & 3) == 2) {
                var_v0->segment.textureFrame = (*sp24)->numOfTextures - 1;
            }
        }
    }
    var_v0->unk75 = 0;
    var_v0->unk76 = ((u32) (u16) sp2C->lifeTimeRange) >> 10;
    var_v0->unk77 = 0;
    if (*sp24 == NULL) {
        func_800B2040(var_v0);
        return NULL;
    }
    return var_v0;
}

GLOBAL_ASM("asm/non_matchings/particles/func_800B0BAC.s")

Particle *func_800B1130(Particle *arg0, Particle *arg1) {
    s32 flags;
    unk800E2CF0 *sp38;
    Particle* var_v0;
    TextureHeader **sp30;
    ParticleBehavior *partBeh;
    unk800B1130_SP28 *sp28;
    f32 sp24;
    s8 sp23;
    
    sp38 = gParticlesAssetTable[arg1->data.unk8];
    if (sp38->unk0 == 3 || sp38->unk0 == 4) {
        return NULL;
    }
    partBeh = arg1->data.behaviour;
    sp28 = (unk800B1130_SP28 *) partBeh->unk9C;
    
    var_v0 = func_800B1CB8(sp38->unk0);
    if (var_v0 == NULL) {
        return var_v0;
    }
    var_v0->segment.particle.blockID = arg0->segment.particle.blockID;
    var_v0->segment.trans.flags = -OBJ_FLAGS_DEACTIVATED;
    var_v0->segment.particle.movementType = sp38->movementType;
    var_v0->segment.unk40 = sp38->unk2;
    var_v0->segment.unk3C = arg0;
    if (var_v0->segment.unk40 & 0x800 && arg0->unk54_ptr != NULL) {
        var_v0->brightness = *arg0->unk54_ptr * 255.0f;
    } else {
        var_v0->brightness = 255;
    }
    sp24 = partBeh->unk50;
    if (partBeh->behaviourFlags & PARTICLE_UNK00020000) {
        sp24 += (f32) get_random_number_from_range(-partBeh->unk8C, partBeh->unk8C) * 0.00001525878906;
    }
    var_v0->segment.trans.scale = sp38->scale * sp24;
    sp24 = partBeh->unk54;
    if (partBeh->behaviourFlags & PARTICLE_UNK00040000) {
        sp24 += (f32) get_random_number_from_range(-partBeh->unk90, partBeh->unk90) * 0.00001525878906;
    }
    if (partBeh->flags & 0x1000) {
        var_v0->segment.scaleVel = sqrtf((arg0->segment.x_velocity * arg0->segment.x_velocity) + (arg0->segment.y_velocity * arg0->segment.y_velocity) + (arg0->segment.z_velocity * arg0->segment.z_velocity)) * sp24 * 0.1f;
    } else {
        var_v0->segment.scaleVel = sp38->scale * sp24;
    }
    var_v0->segment.particle.destroyTimer = get_random_number_from_range(-sp38->lifeTimeRange, sp38->lifeTimeRange) + sp38->lifeTime;
    var_v0->segment.particle.unk38 = 0.0f;
    var_v0->segment.particle.unk34 = 0.0f;
    if (D_800E2D00[0].word != 0) {
        var_v0->colour.word = D_800E2D00[0].word;
    } else if ((s32) sp28 != -1) {
        arg1->data.unk1E++;
        if (arg1->data.unk1E >= sp28->unk0) {
            arg1->data.unk1E = 0;
        }
        var_v0->colour.r = sp28[arg1->data.unk1E + 2].r;
        var_v0->colour.g = sp28[arg1->data.unk1E + 2].g;
        var_v0->colour.b = sp28[arg1->data.unk1E + 2].b;
        var_v0->colour.a = sp28[arg1->data.unk1E + 2].a;
    } else {
        var_v0->colour.r = sp38->colour.r;
        var_v0->colour.g = sp38->colour.g;
        var_v0->colour.b = sp38->colour.b;
        var_v0->colour.a = sp38->colour.a;
    }
    flags = partBeh->behaviourFlags & (PARTICLE_COLOURVEL_ALPHA | PARTICLE_COLOURVEL_BLUE | PARTICLE_COLOURVEL_GREEN | PARTICLE_COLOURVEL_RED);
    if (flags) {
        if (flags & PARTICLE_COLOURVEL_RED) {
            var_v0->colour.r += get_random_number_from_range(-partBeh->colourRangeR, partBeh->colourRangeR);
        }
        if (flags & PARTICLE_COLOURVEL_GREEN) {
            var_v0->colour.g += get_random_number_from_range(-partBeh->colourRangeG, partBeh->colourRangeG);
        }
        if (flags & PARTICLE_COLOURVEL_BLUE) {
            var_v0->colour.b += get_random_number_from_range(-partBeh->colourRangeB, partBeh->colourRangeB);
        }
        if (flags & PARTICLE_COLOURVEL_ALPHA) {
            var_v0->colour.a += get_random_number_from_range(-partBeh->colourRangeA, partBeh->colourRangeA);
        }
    }
    var_v0->opacityTimer = sp38->opacityTimer;
    var_v0->opacity = sp38->unkC * D_800E2EEC;
    if (sp38->unkC < 0xFF) {
        if (var_v0->segment.unk40 & 0x1000) {
            var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0100;
        } else {
            var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0080;
        }
    }
    if (var_v0->opacityTimer < var_v0->segment.particle.destroyTimer) {
        var_v0->opacityVel = (s16) (((sp38->unkD - sp38->unkC) * D_800E2EEC) / (var_v0->segment.particle.destroyTimer - var_v0->opacityTimer));
    } else {
        var_v0->opacityVel = 0;
    }
    sp23 = 0;
    var_v0->segment.unk1A = sp38->unk6;
    if (var_v0->segment.particle.unk2C == 0x80) {
        var_v0->modelData = (ParticleModel *) func_8007C12C(sp38->unk4, 0);
        if (var_v0->modelData) {
            if ((*var_v0->unk44_1->unk8)->unk6 & 4) {
                if (var_v0->segment.unk40 & 0x1000) {
                    var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0100;
                } else {
                    var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0080;
                }
            }
            if (partBeh->flags & 0x800) {
                var_v0->segment.textureFrame = get_random_number_from_range(0, var_v0->unk44_1->unk0 - 1) << 8;
                if ((var_v0->segment.unk40 & 3) == 2) {
                    var_v0->segment.textureFrame |= 0xFF;
                }
            } else if ((var_v0->segment.unk40 & 3) == 2) {
                var_v0->segment.textureFrame = (var_v0->unk44_1->unk0 << 8) - 1;
            } else {
                var_v0->segment.textureFrame = 0;
            }
        } else {
            sp23 = 1;
        }
    } else if ((var_v0->segment.particle.unk2C == 2) || (var_v0->segment.particle.unk2C == 1)) {
        sp30 = &var_v0->modelData->texture;
        if (sp30 != 0) {
            (*sp30) = load_texture(sp38->unk4);
            if ((*sp30) != NULL) {
                if ((*sp30)->flags & 4) {
                    if (var_v0->segment.unk40 & 0x1000) {
                        var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0100;
                    } else {
                        var_v0->segment.trans.flags |= OBJ_FLAGS_UNK_0080;
                    }
                }
                if (partBeh->flags & 0x800) {
                    var_v0->segment.textureFrame = get_random_number_from_range(0, ((*sp30)->numOfTextures >> 8) - 1) << 8;
                    if ((var_v0->segment.unk40 & 3) == 2) {
                        var_v0->segment.textureFrame |= 0xFF;
                    }
                } else if ((var_v0->segment.unk40 & 3) == 2) {
                    var_v0->segment.textureFrame = (*sp30)->numOfTextures - 1;
                } else {
                    var_v0->segment.textureFrame = 0;
                }
            } else {
                sp23 = 1;
            }
        }
    }
    if (sp23 == 0) {
        if (var_v0->segment.particle.unk2C == 1) {
            func_800AF0A4(var_v0);
        }
        if (var_v0->segment.particle.unk2C == 2) {
            func_800AF0F0(var_v0);
        }
    }
    func_800B03C0(var_v0, arg0, arg1, partBeh);
    if (partBeh->flags & 0x80) {
        var_v0->segment.trans.y_rotation = partBeh->angleOffsetY;
        var_v0->segment.trans.x_rotation = partBeh->angleOffsetX;
        var_v0->segment.trans.z_rotation = partBeh->angleOffsetZ;
    } else {
        var_v0->segment.trans.y_rotation = arg0->segment.trans.y_rotation + partBeh->angleOffsetY;
        var_v0->segment.trans.x_rotation = arg0->segment.trans.x_rotation + partBeh->angleOffsetX;
        var_v0->segment.trans.z_rotation = arg0->segment.trans.z_rotation + partBeh->angleOffsetZ;
    }
    flags = partBeh->behaviourFlags & (PARTICLE_ANGLE_Z | PARTICLE_ANGLE_X | PARTICLE_ANGLE_Y);
    if (flags != 0) {
        if (flags & PARTICLE_ANGLE_Y) {
            var_v0->segment.trans.y_rotation += get_random_number_from_range(-partBeh->angleRangeY3, partBeh->angleRangeY3);
        }
        if (flags & PARTICLE_ANGLE_X) {
            var_v0->segment.trans.x_rotation += get_random_number_from_range(-partBeh->angleRangeX3, partBeh->angleRangeX3);
        }
        if (flags & PARTICLE_ANGLE_Z) {
            var_v0->segment.trans.z_rotation += get_random_number_from_range(-partBeh->angleRangeZ3, partBeh->angleRangeZ3);
        }
    }
    var_v0->angleVelY = partBeh->angleVelY;
    var_v0->angleVelX = partBeh->angleVelX;
    var_v0->angleVelZ = partBeh->angleVelZ;
    flags = partBeh->behaviourFlags & (PARTICLE_ANGLEVEL_Z | PARTICLE_ANGLEVEL_X | PARTICLE_ANGLEVEL_Y);
    if (flags) {
        if (flags & PARTICLE_ANGLEVEL_Y) {
            var_v0->angleVelY += get_random_number_from_range(-partBeh->unk86, partBeh->unk86);
        }
        if (flags & PARTICLE_ANGLEVEL_X) {
            var_v0->angleVelX += get_random_number_from_range(-partBeh->unk88, partBeh->unk88);
        }
        if (flags & PARTICLE_ANGLEVEL_Z) {
            var_v0->angleVelZ += get_random_number_from_range(-partBeh->unk8A, partBeh->unk8A);
        }
    }
    func_800B0010(var_v0, arg0, arg1, partBeh);
    var_v0->gravity = gParticleGravityTable[(var_v0->segment.unk40 >> 4) & 7];
    if (var_v0->segment.particle.movementType == PARTICLE_MOVEMENT_VELOCITY) {
        var_v0->forwardVel = sqrtf((var_v0->segment.x_velocity * var_v0->segment.x_velocity) + (var_v0->segment.y_velocity * var_v0->segment.y_velocity) + (var_v0->segment.z_velocity * var_v0->segment.z_velocity));
    }
    if (partBeh->flags & 2) {
        arg1->data.unk6++;
        if (arg1->data.unk6 >= partBeh->unk1A) {
            arg1->data.unkC.unkC += partBeh->unk1C;
            arg1->data.unkC.unkE += partBeh->unk1E;
            arg1->data.unkC.unk10 += partBeh->unk18;
            arg1->data.unk6 -= partBeh->unk1A;
        }
    }
    if (partBeh->flags & 8) {
        arg1->data.unk7++;
        if (arg1->data.unk7 >= partBeh->unk28) {
            arg1->data.unkC.unk12 += partBeh->unk2A;
            arg1->data.unkC.unk14 += partBeh->unk2C;
            arg1->data.unkC.unk16 += partBeh->unk2E;
            arg1->data.unk7 -= partBeh->unk28;
        }
    }
    if (var_v0->modelData == NULL) {
        func_800B2040(var_v0);
        return NULL;
    } else {   
        return var_v0;
    }
}


Particle *func_800B1CB8(s32 arg0) {
    s32 i;
    Particle *var_v1;

    var_v1 = NULL;
    i = 0;

    switch(arg0) {
        case 0x80:
            if ((D_800E2CC0 != NULL)) {
                if (D_800E2CB8 >= D_800E2E54 - 1) {
                    if (D_800E2CBC == 0) {
                        D_800E2CBC = 1;
                    }
                } else {
                    while(D_800E2CC0[i].unk2C != 0) {
                        i++;
                    }
                    D_800E2CC0[i].unk2C = 0x80;
                    D_800E2CB8++;
                    var_v1 = (Particle *) &D_800E2CC0[i];
                }
            }
            break;
        case 1:
            if (D_800E2CA8 != NULL) {
                if (D_800E2CA0 >= D_800E2E4C - 1) {
                    if (D_800E2CA4 == 0) {
                        D_800E2CA4 = 1;
                    }
                } else {
                    while (D_800E2CA8[i].unk2C != 0) {
                        i++;
                    }
                    D_800E2CA8[i].unk2C = 1;
                    D_800E2CA0++;
                    var_v1 = (Particle *) &D_800E2CA8[i];
                }
            }
            break;
        case 2:
            if (D_800E2CB4 != NULL) {
                if (D_800E2CAC >= D_800E2E50 - 1) {
                    if (D_800E2CB0 == 0) {
                        D_800E2CB0 = 1;
                    }
                } else {
                    while (D_800E2CB4[i].unk2C != 0) {
                        i++;
                    }
                    D_800E2CB4[i].unk2C = 2;
                    D_800E2CAC++;
                    var_v1 = (Particle *) &D_800E2CB4[i];
                }
            }
            break;
        case 3:
            if (D_800E2CCC != NULL) {
                if (D_800E2CC4 >= D_800E2E58 - 1) {
                    if (D_800E2CC8 == 0) {
                        D_800E2CC8 = 1;
                    }
                } else {
                    while (D_800E2CCC[i].unk2C != 0) {
                        i++;
                    }
                    D_800E2CCC[i].unk2C = 3;
                    D_800E2CC4++;
                    var_v1 = (Particle *) &D_800E2CCC[i];
                }
            }
            break;
        case 4:
            if (D_800E2CD8 != NULL) {
                if (D_800E2CD0 >= D_800E2E5C - 1) {
                    if (D_800E2CD4 == 0) {
                        D_800E2CD4 = 1;
                    }
                } else {
                    while (D_800E2CD8[i].segment.particle.unk2C != 0) {
                        i++;
                    }
                    D_800E2CD8[i].segment.particle.unk2C = 4;
                    D_800E2CD0++;
                    var_v1 = (Particle *) &D_800E2CD8[i];
                }
            }
            break;
    }
    if (var_v1 != NULL) {
        var_v1->behaviorId = -1;
    }
    return var_v1;
}

void func_800B2040(Particle *arg0) {
    TextureHeader *tex;

    switch (arg0->segment.particle.unk2C) {
    case 0x80:
        if (D_800E2CB8 > 0) {
            if (arg0->modelData) {
                free_sprite((Sprite *) &arg0->modelData->texture);
            }
            D_800E2CB8--;
            arg0->segment.particle.unk2C = 0;
        }
        break;
    case 0:
        return;
    case 1:
        if (D_800E2CA0 > 0) {
            tex = arg0->modelData->texture;
            if (tex != NULL) {
                free_texture(tex);
            }
            D_800E2CA0--;
            arg0->segment.particle.unk2C = 0;
        }
        break;
    case 2:
        if (D_800E2CAC > 0) {
            tex = arg0->modelData->texture;
            if (tex != NULL) {
                free_texture(tex);
            }
            D_800E2CAC--;
            arg0->segment.particle.unk2C = 0;
        }
        break;
    case 3:
        if (D_800E2CC4 > 0) {
            tex = arg0->modelData->texture;
            if (tex != NULL) {
                free_texture(tex);
            }
            D_800E2CC4--;
            arg0->segment.particle.unk2C = 0;
        }
        break;
    case 4:
        if (D_800E2CD0 > 0) {
            func_800B263C(arg0);
            tex = arg0->modelData->texture;
            if (tex != NULL) {
                free_texture(tex);
            }
            D_800E2CD0--;
            arg0->segment.particle.unk2C = 0;
        }
        break;
    }
}


void func_800B2260(Particle *arg0) {
    Particle *temp_v0;
    s32 i;

    if (arg0->data.flags & 0x400) {
        if (arg0->data.unkC_60 != NULL) {
            for (i = 0; i < arg0->data.unk6; i++) {
                temp_v0 = arg0->data.unkC_60[i];
                temp_v0->segment.particle.destroyTimer = 0;
                temp_v0->unk70 = 0;
            }
            free_from_memory_pool(arg0->data.unkC_60);
            arg0->data.unkC_60 = NULL;
        }
    }
}

void func_800B22FC(Particle *arg0, s32 arg1) {
    LevelModelSegmentBoundingBox *boundingBox;
    Particle *sp20;

    gParticleUpdateRate = arg1;
    sp20 = NULL;
    if (arg0->segment.particle.unk2C == 3) {
        func_800B26E0();
    } else {
        if (arg0->segment.unk40 & 3) {
            if (gParticleUpdateRate > 0) {
                sp20 = NULL;
                func_800B2FBC(arg0);
                sp20 = NULL;
            }
        }
        if (arg0->segment.particle.unk2C == 4) {
            sp20 = arg0;
            arg0->unk75 = 1 - arg0->unk75;
            arg0->unk77 = 0;
        }
        if (sp20 == NULL || (sp20 != NULL && sp20->unk70 != 0)) {
            if (arg0->segment.particle.movementType == PARTICLE_MOVEMENT_VELOCITIES) {
                move_particle_with_velocities(arg0);
            } else if (arg0->segment.particle.movementType == PARTICLE_MOVEMENT_VELOCITY_PARENT) {
                move_particle_velocity_parent(arg0);
            } else if (arg0->segment.particle.movementType == PARTICLE_MOVEMENT_BASIC_PARENT) {
                move_particle_basic_parent(arg0);
            } else if (arg0->segment.particle.movementType == PARTICLE_MOVEMENT_VELOCITY) {
                move_particle_with_velocity(arg0);
            } else {
                move_particle_basic(arg0);
            }
        }
        boundingBox = get_segment_bounding_box(arg0->segment.particle.blockID);
        if (boundingBox != NULL) {
            if (arg0->segment.trans.x_position < boundingBox->x1 || boundingBox->x2 < arg0->segment.trans.x_position || 
                arg0->segment.trans.y_position < boundingBox->y1 || boundingBox->y2 < arg0->segment.trans.y_position || 
                arg0->segment.trans.z_position < boundingBox->z1 || boundingBox->z2 < arg0->segment.trans.z_position) {
                arg0->segment.particle.blockID = get_level_segment_index_from_position(arg0->segment.trans.x_position, arg0->segment.trans.y_position, arg0->segment.trans.z_position);
            }
        } else {
            arg0->segment.particle.blockID = get_level_segment_index_from_position(arg0->segment.trans.x_position, arg0->segment.trans.y_position, arg0->segment.trans.z_position);
        }
        arg0->segment.particle.destroyTimer -= gParticleUpdateRate;
        if (arg0->segment.particle.destroyTimer <= 0) {
            gParticlePtrList_addObject((Object*) arg0);
        } else {
            if (arg0->opacityTimer == 0) {
                arg0->opacity += gParticleUpdateRate * arg0->opacityVel;
                if (arg0->opacity < 0xFF) {
                    if (arg0->segment.unk40 & 0x1000) {
                        arg0->segment.trans.flags |= OBJ_FLAGS_UNK_0100;
                    } else {
                        arg0->segment.trans.flags |=  OBJ_FLAGS_UNK_0080;
                    }
                }
            } else {
                arg0->opacityTimer -= gParticleUpdateRate;
                if (arg0->opacityTimer < 0) {
                    arg0->opacity -= (arg0->opacityTimer * arg0->opacityVel);
                    arg0->opacityTimer = 0;
                }
            }
        }
    }
}


void func_800B263C(Particle *arg0) {
    Particle *new_var;
    Particle *new_var2;
    Particle *temp_v0;
    s32 i;

    temp_v0 = arg0->unk70;
    if (temp_v0 != NULL) {
        new_var = temp_v0;
        if (temp_v0->data.unk6 != 0) {
            if (arg0 == temp_v0->data.unkC_60[arg0->unk74]) {
                temp_v0->data.unk6--;
                for (i = arg0->unk74; i < temp_v0->data.unk6; i++) {
                    new_var->data.unkC_60[i] = new_var->data.unkC_60[i + 1];
                    new_var2 = new_var->data.unkC_60[i];
                    new_var2->unk74 = i;
                }
            }
        }
    }
}

GLOBAL_ASM("asm/non_matchings/particles/func_800B26E0.s")

void func_800B2FBC(Particle *arg0) {
    s32 someFlag;
    s32 keepGoing;
    s32 i;
    s32 var_a1;
    s32 someFlag2;
    s32 someFlag3;
    s32 someFlag4;
    
    keepGoing = -1;

    i = 128; // This is needed to match.
    if (arg0->segment.particle.unk2C == i) {
        var_a1 = arg0->unk44_1->unk0 * 256;
    } else {
        var_a1 = arg0->modelData->texture->numOfTextures;
    }

    someFlag4 = arg0->segment.unk40 & 1;
    someFlag2 = arg0->segment.unk40 & 2;
    someFlag3 = arg0->segment.unk40 & 4;
    someFlag = arg0->segment.unk40 & 8;
    for (i = 0; (i++ < gParticleUpdateRate) && keepGoing;) {
        if (!someFlag) {
            arg0->segment.textureFrame += arg0->segment.unk1A;
            if (arg0->segment.textureFrame >= var_a1) {
                if (someFlag2) {
                    arg0->segment.textureFrame = ((var_a1 * 2) - arg0->segment.textureFrame) - 1;
                    someFlag = TRUE;
                    arg0->segment.unk40 |= 8;
                } else if (someFlag3) {
                    arg0->segment.textureFrame -= var_a1;
                } else {
                    arg0->segment.textureFrame = var_a1 - 1;
                    keepGoing = FALSE;
                    arg0->segment.unk40 &= ~3;
                }
            }
        } else {
            arg0->segment.textureFrame -= arg0->segment.unk1A;
            if (arg0->segment.textureFrame < 0) {
                if (someFlag3) {
                    if (someFlag4) {
                        arg0->segment.textureFrame = -arg0->segment.textureFrame;
                        someFlag = FALSE;
                        arg0->segment.unk40 &= ~8;
                    } else {
                        arg0->segment.textureFrame += var_a1;
                    }
                } else {
                    arg0->segment.textureFrame = 0;
                    keepGoing = FALSE;
                    arg0->segment.unk40 &= ~3;
                }
            }
        }
    }
}

/**
 * Apply translation, rotation and scale based off the velocities of the particle.
 * Applies further translation based on the position of the parent.
*/
void move_particle_basic_parent(Particle *particle) {
    s32 i;
    ObjectSegment *parent;

    i = gParticleUpdateRate;
    while (i-- > 0) {
        particle->baseVelX += particle->segment.x_velocity;
        particle->baseVelY += particle->segment.y_velocity;
        particle->segment.y_velocity -= particle->gravity;
        particle->baseVelZ += particle->segment.z_velocity;
        particle->segment.trans.y_rotation += particle->angleVelY;
        particle->segment.trans.x_rotation += particle->angleVelX;
        particle->segment.trans.z_rotation += particle->angleVelZ;
        particle->segment.trans.scale += particle->segment.scaleVel;
    }
    particle->segment.trans.x_position = particle->baseVelX;
    particle->segment.trans.y_position = particle->baseVelY;
    particle->segment.trans.z_position = particle->baseVelZ;
    parent = particle->segment.unk3C;
    if (parent) {
        particle->segment.trans.x_position += parent->trans.x_position;
        particle->segment.trans.y_position += parent->trans.y_position;
        particle->segment.trans.z_position += parent->trans.z_position;
    }
}


/**
 * Apply translation, rotation and scale based off the velocities of the particle.
 * Velocity is set from forward velocity.
 * Applies further translation based on the position of the parent.
*/
void move_particle_velocity_parent(Particle *particle) {
    s32 i;
    ObjectSegment *parent;

    i = gParticleUpdateRate;
    while (i-- > 0) {
        particle->segment.trans.y_rotation += particle->angleVelY;
        particle->segment.trans.x_rotation += particle->angleVelX;
        particle->segment.trans.z_rotation += particle->angleVelZ;
        particle->segment.trans.scale += particle->segment.scaleVel;
    }
    particle->segment.trans.x_position = 0.0f;
    particle->segment.trans.y_position = -particle->forwardVel;
    particle->segment.trans.z_position = 0.0f;
    f32_vec3_apply_object_rotation(&particle->segment.trans, &particle->segment.trans.x_position);
    particle->segment.trans.x_position += particle->baseVelX;
    particle->segment.trans.y_position += particle->baseVelY;
    particle->segment.trans.z_position += particle->baseVelZ;
    parent = particle->segment.unk3C;
    if (parent) {
        particle->segment.trans.x_position += parent->trans.x_position;
        particle->segment.trans.y_position += parent->trans.y_position;
        particle->segment.trans.z_position += parent->trans.z_position;
    }
}

/**
 * Apply translation, rotation and scale based off the velocities of the particle.
 * Uses base velocity, then applies additional velocity based off the forward moving direction.
*/
void move_particle_with_velocities(Particle *particle) {
    s32 i;
    Vec3f vel;

    for (i = 0; gParticleUpdateRate > i++;) {
        particle->segment.trans.x_position += particle->segment.x_velocity;
        particle->segment.trans.y_position += particle->segment.y_velocity;
        particle->segment.trans.z_position += particle->segment.z_velocity;
        particle->segment.trans.scale += particle->segment.scaleVel;
        particle->segment.trans.y_rotation += particle->angleVelY;
        particle->segment.trans.x_rotation += particle->angleVelX;
        particle->segment.trans.z_rotation += particle->angleVelZ;
        vel.x = 0.0f;
        vel.y = -particle->forwardVel;
        vel.z = 0.0f;
        f32_vec3_apply_object_rotation(&particle->segment.trans, vel.f);
        particle->segment.x_velocity += vel.x;
        particle->segment.y_velocity += vel.y;
        particle->segment.y_velocity -= particle->gravity;
        particle->segment.z_velocity += vel.z;
    }
}

/**
 * Apply translation, rotation and scale based off the velocities of the particle.
*/
void move_particle_basic(Particle *particle) {
    s32 i;

    for (i = 0; gParticleUpdateRate > i++;) {
        particle->segment.trans.x_position += particle->segment.x_velocity;
        particle->segment.trans.y_position += particle->segment.y_velocity;
        particle->segment.y_velocity = particle->segment.y_velocity - particle->gravity;
        particle->segment.trans.z_position += particle->segment.z_velocity;
        particle->segment.trans.scale += particle->segment.scaleVel;
        particle->segment.trans.y_rotation += particle->angleVelY;
        particle->segment.trans.x_rotation += particle->angleVelX;
        particle->segment.trans.z_rotation += particle->angleVelZ;
    }
}

/**
 * Apply translation, rotation and scale based off the velocities of the particle.
 * Velocity is set from forward velocity.
*/
void move_particle_with_velocity(Particle *particle) {
    s32 i;

    for (i = 0; gParticleUpdateRate > i++;) {
        particle->segment.x_velocity = 0.0f;
        particle->segment.y_velocity = 0.0f;
        particle->segment.z_velocity = -particle->forwardVel;
        f32_vec3_apply_object_rotation3(&particle->segment.trans, &particle->segment.x_velocity);
        particle->segment.trans.x_position += particle->segment.x_velocity;
        particle->segment.trans.y_position += particle->segment.y_velocity - particle->gravity;
        particle->segment.trans.z_position += particle->segment.z_velocity;
        particle->segment.trans.scale += particle->segment.scaleVel;
        particle->segment.trans.y_rotation += particle->angleVelY;
        particle->segment.trans.x_rotation += particle->angleVelX;
        particle->segment.trans.z_rotation += particle->angleVelZ;
    }
}

/**
 * Iterate through every object and render it as particle if applicable.
*/
UNUSED void render_active_particles(Gfx **dList, MatrixS **arg1, Vertex **arg2) {
    UNUSED s32 pad;
    UNUSED s32 pad2;
    Object **objects;
    s32 iObj;
    s32 nObjs;

    objects = objGetObjList(&iObj, &nObjs);
    for (; iObj < nObjs; iObj++) {
        if (objects[iObj]->segment.trans.flags & OBJ_FLAGS_DEACTIVATED) {
            if ((s32) objects[iObj]->segment.header & 0x8000) {
                render_particle((Particle *) objects[iObj], dList, arg1, arg2, 0);
            }
        }
    }
}

/**
 * Load a texture then render a sprite or a billboard.
*/
void render_particle(Particle *particle, Gfx **dlist, MatrixS **mtx, Vertex **vtx, s32 flags) {
    s32 renderFlags;
    s32 alpha;
    s32 temp;
    ParticleModel *modelData;
    Vertex *tempvtx;

    renderFlags = (RENDER_FOG_ACTIVE | RENDER_Z_COMPARE);
    
    // Never true
    if (particle->segment.unk40 & flags && D_800E2CDC < 512) {
        return;
    }
    alpha = (particle->opacity >> 8) & 0xFF;
    if (alpha <= 0) {
        return;
    }
    if (particle->segment.object.unk2C != 4 && particle->segment.object.unk2C != 3) {
        gDPSetEnvColor((*dlist)++, particle->colour.r, particle->colour.g, particle->colour.b, particle->colour.a);
        if (alpha != 255) {
            renderFlags = (RENDER_Z_UPDATE | RENDER_FOG_ACTIVE | RENDER_SEMI_TRANSPARENT | RENDER_Z_COMPARE);
            gDPSetPrimColor((*dlist)++, 0, 0, particle->brightness, particle->brightness, particle->brightness, alpha);
        } else {
            gDPSetPrimColor((*dlist)++, 0, 0, 255, 255, 255, 255);
        }
        if (particle->segment.object.unk2C == 0x80) {
            temp = particle->segment.textureFrame;
            particle->segment.textureFrame >>= 8;
            particle->segment.textureFrame = (particle->segment.textureFrame * 255) / (particle->unk44_1->unk0);
            render_sprite_billboard(dlist, mtx, vtx, (Object *) particle, (unk80068514_arg4 *) particle->modelData, renderFlags);
            particle->segment.textureFrame = temp;
        } else {
            modelData = particle->modelData;
            if (modelData->texture) {
                camera_push_model_mtx(dlist, mtx, &particle->segment.trans, 1.0f, 0.0f);
                load_and_set_texture(dlist, (TextureHeader *) modelData->texture, renderFlags, particle->segment.textureFrame << 8);
                gSPVertexDKR((*dlist)++, OS_K0_TO_PHYSICAL(modelData->vertices), modelData->vertexCount, 0);
                gSPPolygon((*dlist)++, OS_K0_TO_PHYSICAL(modelData->triangles), modelData->triangleCount, TRIN_ENABLE_TEXTURE);
                func_80069A40(dlist);
            }
        }
        if (alpha != 255 || particle->brightness != 255) {
            gDPSetPrimColor((*dlist)++, 0, 0, 255, 255, 255, 255);
        }
        if (particle->colour.a) {
            gDPSetEnvColor((*dlist)++, 255, 255, 255, 0);
        }
    } else {
        renderFlags = (RENDER_VTX_ALPHA | RENDER_Z_UPDATE | RENDER_FOG_ACTIVE | RENDER_Z_COMPARE | RENDER_ANTI_ALIASING);
        gDPSetEnvColor((*dlist)++, 255, 255, 255, 0);
        if (particle->segment.object.unk2C == 4) {
            if (particle->segment.camera.unk3A > 0) {
                gDPSetPrimColor((*dlist)++, 0, 0, particle->brightness, particle->brightness, particle->brightness, 255);
                if (particle->unk77 == 0) {
                    func_800B3E64((Object *) particle);
                }
                modelData = particle->modelData;
                temp = particle->unk75;
                temp <<= 3;
                tempvtx = &modelData->vertices[temp];
                load_and_set_texture(dlist, modelData->texture, renderFlags, particle->segment.textureFrame << 8);
                gSPVertexDKR((*dlist)++, OS_K0_TO_PHYSICAL(tempvtx), modelData->vertexCount, 0);
                gSPPolygon((*dlist)++, OS_K0_TO_PHYSICAL(modelData->triangles), modelData->triangleCount, TRIN_ENABLE_TEXTURE);
                if (particle->brightness != 255) {
                    gDPSetPrimColor((*dlist)++, 0, 0, 255, 255, 255, 255);
                }
            }
        } else if (particle->segment.object.unk2C == 3) {
            gDPSetPrimColor((*dlist)++, 0, 0, particle->brightness, particle->brightness, particle->brightness, alpha);
            if (particle->unk68b >= 2) {
                modelData = particle->modelData;
                load_and_set_texture(dlist, modelData->texture, renderFlags, particle->segment.textureFrame << 8);
                gSPVertexDKR((*dlist)++, OS_K0_TO_PHYSICAL(modelData->vertices), modelData->vertexCount, 0);
                gSPPolygon((*dlist)++, OS_K0_TO_PHYSICAL(modelData->triangles),  modelData->triangleCount, 1);
            } else if (particle->unk68b > 0) {
                modelData = particle->modelData;
                load_and_set_texture(dlist, modelData->texture, renderFlags, particle->segment.textureFrame << 8);
                gSPVertexDKR((*dlist)++, OS_K0_TO_PHYSICAL(modelData->vertices), 4, 0);
                gSPPolygon((*dlist)++, OS_K0_TO_PHYSICAL(&modelData->triangles[modelData->triangleCount]), 1, TRIN_ENABLE_TEXTURE);
            }
            if (alpha != 255 || particle->brightness != 255) {
                gDPSetPrimColor((*dlist)++, 0, 0, 255, 255, 255, 255);
            }
        }
    }
}

GLOBAL_ASM("asm/non_matchings/particles/func_800B3E64.s")

/**
 * Return a specific particle asset table from the main table.
*/
UNUSED unk800E2CF0 *get_particle_asset_table(s32 idx) {
    if (idx < gParticlesAssetTableCount) {
        return gParticlesAssetTable[idx];
    }
    return gParticlesAssetTable[gParticlesAssetTableCount - 1];
}

/**
 * Return the next particle table after the index.
 * Make sure the index is in range by wrapping it.
*/
UNUSED unk800E2CF0 *get_next_particle_table(s32 *idx) {
    *idx = *idx + 1;
    while (*idx >= gParticlesAssetTableCount) {
        *idx = *idx - gParticlesAssetTableCount;
    }
    return gParticlesAssetTable[*idx];
}

/**
 * Return the previous particle table before the index.
 * Make sure the index is in range by wrapping it.
*/
UNUSED unk800E2CF0 *get_previous_particle_table(s32 *idx) {
    *idx = *idx - 1;
    while (*idx < 0) {
        *idx += gParticlesAssetTableCount;
    }
    return gParticlesAssetTable[*idx];
}

/**
 * Return the particle behaviour ID from the behaviour table.
*/
UNUSED ParticleBehavior *get_particle_behaviour(s32 idx) {
    if (idx < gParticleBehavioursAssetTableCount) {
        return gParticleBehavioursAssetTable[idx];
    }
    return gParticleBehavioursAssetTable[gParticleBehavioursAssetTableCount - 1];
}

UNUSED ParticleBehavior *func_800B45C4(s32 *idx) {
    *idx += 1;
    while (*idx >= gParticleBehavioursAssetTableCount) {
        *idx -= gParticleBehavioursAssetTableCount;
    }
    return gParticleBehavioursAssetTable[*idx];
}

UNUSED ParticleBehavior *func_800B461C(s32 *idx) {
    *idx -= 1;
    while (*idx < 0) {
        *idx += gParticleBehavioursAssetTableCount;
    }
    return gParticleBehavioursAssetTable[*idx];
}

void func_800B4668(Object *obj, s32 idx, s32 arg2, s32 arg3) {
    s32 temp_v0;

    arg3 <<= 8; 
    temp_v0 = (obj->unk6C[idx].unkA & 0xFFFF) + arg2;
    if (arg3 < temp_v0) {
        obj->unk6C[idx].unkA = arg3;
    } else {
        obj->unk6C[idx].unkA = temp_v0;
    }
    obj->unk6C[idx].unk4 |= 0x100;
}

void func_800B46BC(Object *obj, s32 idx, s32 arg2, s32 arg3) {
    s32 temp_v0;

    arg3 <<= 8; 
    temp_v0 = (obj->unk6C[idx].unkA & 0xFFFF) - arg2;
    if (temp_v0 < arg3) {
        obj->unk6C[idx].unkA = arg3;
    } else {
        obj->unk6C[idx].unkA = temp_v0;
    }
    obj->unk6C[idx].unk4 |= 0x100;
}
