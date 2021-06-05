/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x8006A6B0 */

#include "types.h"
#include "macros.h"
#include "structs.h"
#include "fast3d.h"

/************ .rodata ************/

const char D_800E70B0[] = "LOADLEVEL Error: Level out of range\n";
const char D_800E70D8[] = "BossLev problem\n";
const char D_800E70EC[] = "AITABLE Error: Table out of range\n";
const char D_800E7110[] = "1.1605";
const char D_800E7118[] = "02/10/97 16:03";
const char D_800E7128[] = "pmountain";

const char D_800E7134[] = "BBB\n";
const char D_800E713C[] = "CAR";
const char D_800E7140[] = "HOV";
const char D_800E7144[] = "PLN";
const char D_800E7148[] = "Swapping\n";

/*********************************/

/************ .data ************/

s8  D_800DD310 = 0; // Currently unknown, might be a different type. 
s8  D_800DD314 = -1;
s8  D_800DD318 = 0;
s32 D_800DD31C = 0;
s32 D_800DD320 = 0x585E;
s32 D_800DD324 = 0x154;
s16 D_800DD328 = 0;
s16 D_800DD32C = 0;

s8 D_800DD330 = 0;

// Unused?
char *D_800DD334[6] = {
    NULL, NULL, NULL, // These could be a file boundary.
    D_800E7110, D_800E7118, D_800E7128
};

// Unused?
char gBuildString[40] = "Version 7.7 29/09/97 15.00 L.Schuneman";

s8  D_800DD374 = 0;
s32 D_800DD378 = 1;
s32 D_800DD37C = 0;
s32 D_800DD380 = 0; // Currently unknown, might be a different type. 
s32 D_800DD384 = 0;
s32 D_800DD388 = 0; // Currently unknown, might be a different type. 
s8  D_800DD38C = 0;
u8  D_800DD390 = 0;
s16 D_800DD394 = 0;
u8  D_800DD398 = 0;
s32 D_800DD39C = 0; // Currently unknown, might be a different type. 
s8  D_800DD3A0 = 0;
s32 D_800DD3A4 = 0; // Currently unknown, might be a different type. 
s32 D_800DD3A8 = 0; // Currently unknown, might be a different type. 
s32 D_800DD3AC = 0; // Currently unknown, might be a different type. 
s32 D_800DD3B0 = 0x1194;
s32 D_800DD3B4 = 0x1B58;
s32 D_800DD3B8 = 0x2AF8;
s32 D_800DD3BC = 0x2AF8;
s32 D_800DD3C0 = 0x12C;
s32 D_800DD3C4 = 0x258;
s32 D_800DD3C8 = 0x352;
s32 D_800DD3CC = 0x384;
s32 D_800DD3D0 = 0x12C;
s32 D_800DD3D4 = 0x190;
s32 D_800DD3D8 = 0x226;
s32 D_800DD3DC = 0x258;
s32 D_800DD3E0 = 0x14;
s32 D_800DD3E4 = 0x1E;
s32 D_800DD3E8 = 0x28;
s32 D_800DD3EC = 0x32;

s8  D_800DD3F0 = 0;
s16 D_800DD3F4[8] = {
    -32768, 0, 20, 0,
    255, -1, 20, -1
};
s32 D_800DD404 = 12;
s32 D_800DD408 = 0x00FFFFFF;
s32 D_800DD40C = 0x001EFFFF; // Unused?

// Unused?
char *D_800DD410[3] = {
    D_800E713C, D_800E7140, D_800E7144
};

// Not sure if this is an array or a struct.
s32 D_800DD41C[2] = { 
    0, 0x001EFFFF
};

s32 D_800DD424[2] = { 
    0, 0x0104FFFF
};

/*******************************/


extern s32 D_800DFD94;
extern s32 D_80121164;

extern LevelHeader* D_80121168;

extern s32 D_80121170;
extern s32 D_80121174;

// Size: 6 bytes
typedef struct unk8012117C {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 pad3[3];
} unk8012117C;
extern unk8012117C *D_8012117C[2]; // Unknown number of entries

extern s32 D_801211C0;
extern s16 D_801211C8[8]; // Unknown number of entries
extern Gfx *D_801211F0[3]; // Unknown number of entries.
extern Gfx *D_801211F8;
extern s32 D_80121208;
extern s32 D_80121218;
extern s32 D_80121228;
extern s8  D_80121250[16]; // Unknown number of entries
extern s32 D_801234E8;
extern s32 D_801234EC;
extern s32 D_801234F0;
extern s32 D_801234F4;
extern s32 D_801234F8;
extern s32 D_801234FC;
extern s32 D_80123500;
extern s32 D_80123504;
extern s32 D_80123508;
extern Settings *gSettingsPtr;
extern s8  D_80123514;
extern s8  gIsPaused;
extern s8  D_80123516;
extern s32 D_80123518;
extern s32 D_8012351C;
extern s32 D_80123520;
extern u8  D_80123524;
extern u8  D_80123525;
extern u8  D_80123526;
extern s32* D_80123548; // This is actually an OSMesgQueue pointer.
extern s32 D_80123560;

void func_800014BC(f32 arg0);
s8* func_8001E29C(s32 arg0);
s32 func_8006A624(s8 arg0);
void func_8006F64C(s32*, f32, f32, f32, f32*, f32*, f32*);
void guPerspectiveF(s32*, s32*, f32, f32, f32, f32, f32);
void func_8006F870(s32*, s32*);
s16 func_80029F18(f32, f32, f32);    
void func_8006A50C(void);
void func_800665E8(s32 arg0);
void func_8006CB58(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_8006C164(void);
void func_8000E194(void);
f32 sqrtf(f32);
void osSetTime(u64);
void func_8001D5E0(f32 arg0, f32 arg1, f32 arg2);
void func_800705F8(s32, f32, f32, f32);
void func_8006ECFC(s32 arg0);


GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006A6B0.s")

s32 func_8006B0AC(s32 arg0) {
    if (arg0 > 0 && arg0 < D_80121170) {
        return (*D_8012117C)[arg0].unk2 & 0xF;
    }
    return 0;
}

s32 func_8006B0F8(s32 arg0) {
    if (arg0 > 0 && arg0 < D_80121170) {
        s32 temp = (*D_8012117C)[arg0].unk2;
        if(temp != 0) {
            return (temp >> 4) & 0xF;
        }
    }
    return 1;
}

s8 func_8006B14C(s32 arg0) {
    if (arg0 >= 0 && arg0 < D_80121170) {
        return (*D_8012117C)[arg0].unk1;
    }
    return -1;
}

s8 func_8006B190(s32 arg0) {
    if (arg0 >= 0 && arg0 < D_80121170) {
        return (*D_8012117C)[arg0].unk0;
    }
    return 0;
}

s32 func_8006B1D4(s32 arg0) {
    s8* temp;
    
    if (arg0 < 0 || arg0 >= D_80121174) {
        arg0 = 0;
    }
    temp = func_8001E29C(0x1B);
    
    return temp[arg0];
}

void func_8006B224(s32* arg0, s32* arg1) {
    *arg0 = D_80121170;
    *arg1 = D_80121174;
}


s32 func_8006B240(void) {
    return D_800DD31C;
}

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006B250.s")
#else

extern s32 *D_80121160;
extern s32 D_800DD314;

Settings *get_settings(void);
s32 *func_80076C58(s32);
LevelHeader *func_80070C9C(s32, s32);
void func_8006C1AC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_8006C2E4(void);
s16 func_8006C2F0(void);
void func_8006DB20(s32 arg0);
void func_80072708(void);

// load_level(levelID, numOfPlayers, entrance, vehicle, ???);
void func_8006B250(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 sp48;
    s32 sp44;
    Settings *settings;
    s32 temp_s0_2;
    s32 temp_s0_5;
    s32 temp_t8_3;
    s32 temp_v0_5;
    s8 *temp_v0_4;
    s32 phi_v1;
    s32 phi_v1_2;
    s32 phi_v0_2;
    s32 phi_s0;
    s32 phi_v1_5;
    s8 phi_s0_2;
    s32 phi_at;
    s32 phi_s0_3;
    s32 phi_s0_5;
    s32 phi_v1_9;
    s32 temp, temp2;

    func_80072708();
    if (arg4 == -1) {
        arg4 = 0;
    }
    if (arg1 == -1) {
        sp48 = 1;
        arg1 = 0;
    } else {
        sp48 = 0;
    }
    if (arg1 == 0) {
        func_8000318C(0x8);
    } else if (arg1 == 1) {
        func_8000318C(0xC);
    } else {
        func_8000318C(0x10);
    }
    
    settings = get_settings();
    D_80121160 = func_80076C58(0x16);
    phi_v1 = 0;
    while (-1 != D_80121160[phi_v1]) {
        phi_v1++;
    }
    phi_v1--;
    if (arg0 >= phi_v1) {
        arg0 = 0;
    }
    temp2 = D_80121160[arg0];
    temp = D_80121160[arg0 + 1] - temp2;
    D_80121168 = func_80070C9C(temp, 0xFFFF00FF);
    func_80076E68(0x17, D_80121168, temp2, temp);
    D_800DD330 = 0;
    sp44 = arg0;
    if (D_80121168->race_type == 0) {
        func_8006C2E4();
    }
    if (func_8006C2F0() == 0) {
        if (D_800DD32C == 0) {
            if (D_80121168->race_type == 8) {
                phi_v1_2 = settings->courseFlagsPtr[temp2] & 1;
                phi_v0_2 = 0;
                if (D_80121168->world == 0 || D_80121168->world == 5) {
                    phi_v0_2 = 1;
                }
                if (phi_v1_2 == 0 || phi_v0_2 != 0) {
                    func_8006C1AC(arg0, arg2, arg3, arg4);
                    if ((settings->bosses & (1 << settings->worldId)) != 0) {
                        arg4 = 7;
                    } else {
                        arg4 = 3;
                    }
                    if (phi_v0_2 != 0) {
                        arg4 = 0;
                        if (phi_v1_2 != 0) {
                            D_800DD330 = (u8)2;
                        }
                    }
                    temp_v0_4 = func_8001E29C(0x43);
                    phi_s0 = 0;
                    while(arg0 != temp_v0_4[phi_s0 + 1]) {
                        phi_s0++;
                    }
                    arg0 = temp_v0_4[phi_s0 + 1];
                    arg2 = arg4;
                }
                
            }
            if (D_80121168->race_type == 5) {
                if (D_80121168->world > 0 && D_80121168->world < 5) {
                    if ((settings->keys & (1 << D_80121168->world)) != 0) {
                        temp_v0_5 = 0x4000 << (D_80121168->world + 0x1F);
                        if ((settings->cutsceneFlags & temp_v0_5) == 0) {
                            func_8006C1AC(arg0, arg2, arg3, arg4);
                            settings->cutsceneFlags |= temp_v0_5;
                            arg2 = 0;
                            arg4 = 5;
                            arg0 = func_8001E29C(0x44)[D_80121168->world - 1];
                        }
                    }
                }
            }
            if (D_80121168->race_type == 5 && D_80121168->world == 0 && (settings->cutsceneFlags & 0x2000) == 0 && settings->wizpigAmulet >= 4) {
                s32 temp_a1 = arg2;
                s32 temp_a3 = arg4;
                arg4 = 0;
                arg2 = 0;
                func_8006C1AC(arg0, temp_a1, arg3, temp_a3);
                settings->cutsceneFlags |= 0x2000;
                arg0 = (s32) func_8001E29C(0x44)[1];
            }
        }
    }
    D_800DD32C = 0;
    if (sp44 != arg0) {
        func_80071140(D_80121168);
        temp2 = D_80121160[arg0];
        temp = D_80121160[arg0 + 1] - temp2;
        D_80121168 = func_80070C9C(temp, 0xFFFF00FF);
        func_80076E68(0x17, D_80121168, temp2, temp);
    }
    func_80071140(D_80121160);
    func_8006BFC8(&D_80121168->unk20);
    func_8000CBC0();
    D_80121164 = arg0;
    for (phi_v1_5 = 0; phi_v1_5 < 7; phi_v1_5++) {
        if ((s32)D_80121168->unk74[phi_v1_5] != -1) {
            D_80121168->unk74[phi_v1_5] = func_8001E29C((s32)D_80121168->unk74[phi_v1_5]);
            func_8007F1E8(D_80121168->unk74[phi_v1_5]);
        }
    }
    if (arg4 == 0x64) {
        if(get_trophy_race_world_id() != 0 && D_80121168->race_type == 0) {
            arg4 = 0;
        } else if(is_in_tracks_mode() == 1 && D_80121168->race_type == 0) {
            arg4 = 0;
        }
    }
    if (D_80121168->race_type == 0 || D_80121168->race_type == 8) {
        D_800DD31C = 1;
    } else {
        D_800DD31C = 0;
    }
    if (sp48 != 0 && D_80121168->race_type != 7) {
        D_80121168->race_type = 6;
    }
    func_80000BE0(D_80121168->unkB3);
    func_80000CBC();
    func_80031BB8(0x20);
    phi_s0_2 = (u8)0;
    if (arg3 >= 0 && arg3 < 3) {
        phi_s0_2 = D_80121168->unk4F[arg3];
    }
    func_80017E74(phi_s0_2);
    settings->courseId = arg0;
    if (D_80121168->world != -1) {
        settings->worldId = D_80121168->world;
    }
    if (settings->worldId == 0 && settings->worldId > 0) {
        D_800DD314 = func_8006DB2C();
    }
    if (settings->worldId == 0 && settings->worldId > 0 && D_800DD314 != -1) {
        arg3 = D_800DD314;
    }
    func_8006DB20(arg3);
    if (D_80121168->race_type == 5) {
        if (settings->worldId > 0) {
            temp_s0_5 = 8 << (settings->worldId + 31);
            if (settings->worldId == 5) {
                if (settings->balloonsPtr[0] >= 47) {
                    if (settings->ttAmulet >= 4) {
                        if ((settings->cutsceneFlags & temp_s0_5) == 0) {
                            settings->cutsceneFlags |= temp_s0_5;
                            arg4 = 5;
                        }
                    }
                }
            } else {
                if (settings->balloonsPtr[settings->worldId] >= 4) {
                    if ((settings->cutsceneFlags & temp_s0_5) == 0) {
                        settings->cutsceneFlags |= temp_s0_5;
                        arg4 = 5;
                    }
                }
                temp_t8_3 = temp_s0_5 << 5;
                if (settings->balloonsPtr[settings->worldId] >= 8) {
                    if ((settings->cutsceneFlags & temp_t8_3) == 0) {
                        settings->cutsceneFlags |= temp_t8_3;
                        arg4 = 5;
                    }
                }
            }
        }
    }
    if (arg1 != 0 && D_80121168->race_type == 0) {
        arg4 = 0x64;
    }
    if (D_80121168->race_type == 0 || ((D_80121168->race_type & 0x40) != 0 && is_in_two_player_adventure(D_80121168->race_type) != 0)) {
        D_800DD318 = (u8)1;
        arg4 = 0x64;
    } else {
        D_800DD318 = (u8)0;
    }
    if (D_80121168->race_type == 0 && sp48 == 0 && func_8000E4C8(arg4) != 0) {
        arg4 = 0x64;
    }
    func_8001E450(arg4);
    func_800249F0(D_80121168->geometry, D_80121168->skybox, arg1, arg3, arg2, D_80121168->collectables, D_80121168->unkBA);
    if (D_80121168->unk3A == 0 && D_80121168->unk3C == 0 && D_80121168->fogR == 0 && D_80121168->fogG == 0 && D_80121168->fogB == 0) {
        phi_s0_3 = 0;
        while(phi_s0_3 < 4) {
            func_800307BC(phi_s0_3);
            phi_s0_3++;
        }
    } else {
        phi_s0_3 = 0;
        while(phi_s0_3 < 4) {
            func_80030664(phi_s0_3, D_80121168->unk3A, D_80121168->unk3C, (u8)D_80121168->fogR, D_80121168->fogG, D_80121168->fogB);
            phi_s0_3++;
        }
    }
    settings = get_settings();
    if (D_80121168->world != -1) {
        settings->worldId = D_80121168->world;
    }
    settings->courseId = arg0;
    if (D_80121168->weather_enable > 0) {
        func_800AB4A8(
            D_80121168->unk92, 
            D_80121168->weather_enable, 
            D_80121168->unk96 << 8, 
            D_80121168->unk98 << 8, 
            D_80121168->unk9A << 8, 
            D_80121168->unk94 * 0x101, 
            D_80121168->unk95 * 0x101
        );
        func_800AB308(-1, -0x200);
    }
    if (D_80121168->unk49 == -1) {
        D_80121168->unkA4 = func_8007AE74(D_80121168->unkA4);
        D_80121168->unkA8 = (u16)0;
        D_80121168->unkAA = (u16)0;
    }
    if ((s32)D_80121168->unkAC != -1) {
        D_80121168->unkAC = func_8001E29C((s32)D_80121168->unkAC);
        func_8007F414(D_80121168->unkAC);
    }
    func_800660EC((f32)D_80121168->unk9C);
    func_80077B34(D_80121168->unk9D, D_80121168->unk9E, D_80121168->unk9F);
    func_8007A974();
    func_8007AB24(D_80121168->unk4[arg1]);
}
#endif

void func_8006BD10(f32 arg0) {
    if (D_80121168->music != 0) {
        func_800012E8();
        func_80000B34(D_80121168->music);
        func_800014BC(arg0);
        func_80001074(D_80121168->instruments);
    }
}

s32 func_8006BD88(void) {
    return D_80121164;
}

u8 func_8006BD98(void) {
    return D_80121168->race_type;
}

LevelHeader* func_8006BDB0(void) {
    return D_80121168;
}

/* Unused? */
u8 func_8006BDC0(void) {
    return D_80121170 - 1;
}

GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006BDDC.s")

void func_8006BEFC(void) {
    func_8006C164();
    func_80077B34(0, 0, 0);
    func_80071140(D_80121168);
    func_800049D8();
    func_80001844();
    func_800018E0();
    func_800012E8();
    func_80031B60();
    func_8002C7D4();
    func_80008174();
    func_80000968(0);
    if (D_80121168->weather_enable > 0) {
        func_800AB35C();
    }
    if (D_80121168->unk49 == 0xFF) {
       func_8007B2BC(D_80121168->unkA4);
    }
}

extern s32 *D_80121160;
Settings *get_settings(void);
s32 *func_80076C58(s32);

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006BFC8.s")
#else
void func_8006BFC8(s8 *arg0) {
    s32 temp, temp2;
    s16 phi_v1;
    s8 phi_s0 = 0;
    
    if (!is_in_tracks_mode()) {
        Settings *settings = get_settings();
        // Regalloc issue: temp3 should be a3, not v1.
        s32 temp3 = settings->courseFlagsPtr[settings->courseId];
        if (temp3 & 2) {
            phi_s0 = 1;
        }
        if (temp3 & 4) {
            phi_s0 = 2;
        }
    } else {
        phi_s0 = 3;
    }
    
    if (get_trophy_race_world_id() != 0) {
        phi_s0 = 4;
    }
    
    if(is_in_adventure_two()){
        phi_s0 += 5;
    }
    
    phi_s0 = arg0[phi_s0];
    
    if ((func_8009C30C() << 6) < 0) {
        phi_s0 = 9;
    }
    if (func_8006DA0C() == 1) {
        phi_s0 = 5;
    }
    D_80121160 = func_80076C58(1);
    phi_v1 = 0;
    while (-1 != (s32)D_80121160[phi_v1]) {
        phi_v1++;
    }
    phi_v1--;
    if (phi_s0 >= phi_v1) {
        phi_s0 = 0;
    }
    temp2 = D_80121160[phi_s0];
    temp = D_80121160[phi_s0 + 1] - temp2;
    D_801211C0 = func_80070C9C(temp, 0xFFFF00FF, D_80121160);
    func_80076E68(0, D_801211C0, temp2, temp);
    func_80071140(D_80121160);
}
#endif

void func_8006C164(void) {
    func_80071140(D_801211C0);
}

s32 func_8006C18C(void) {
    return D_801211C0;
}

s8 func_8006C19C(void) {
    return D_800DD318;
}

void func_8006C1AC(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_801211C8[D_800DD328++] = arg0;
    D_801211C8[D_800DD328++] = arg1;
    D_801211C8[D_800DD328++] = arg2;
    D_801211C8[D_800DD328++] = arg3;
}

void func_8006C22C(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    s16 temp_v1;
    
    D_800DD328--;
    *arg3 = D_801211C8[D_800DD328];
    D_800DD328--;
    temp_v1 = D_801211C8[D_800DD328];
    D_800DD328--;
    *arg1 = D_801211C8[D_800DD328];
    D_800DD328--;
    *arg0 = D_801211C8[D_800DD328];
    
    if (temp_v1 != -1) {
        *arg2 = temp_v1;
    }
    
    D_800DD32C = 1;
}


void func_8006C2E4(void) {
    D_800DD328 = 0;
}

s16 func_8006C2F0(void) {
    return D_800DD328;
}

s32 func_8006C300(void) {
    if (D_800DD330 >= 2) {
        D_800DD330 = 1;
        return 0;
    } else {
        return D_800DD330;
    }
}

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/D_8006C330.s")
#else
void func_8006C330(s32 arg0) {
    func_8006C3E0();
    D_800DD37C = func_8006A1C4(D_800DD37C, 0);
    D_80123520 = 0;
    D_801234EC = -1;
    while(1) {
        if (func_8006EAC0() != 0) {
            func_80072708();
            audioStopThread();
            func_800C73BC();
            __osSpSetStatus(0xAAAA82);
            func_800CD250(0x1D6);
            while(1); // Infinite loop
        }
        func_8006C60C();
        func_80065E30();
    }
    // This infinite loop creates too many nops! Needs to be 0x10 aligned, not 0x20 aligned.
}
#endif

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006C3E0.s")
#else
extern Gfx *D_801211F0;
extern u8 D_800DD374;
extern u8 D_800DD3A0;
extern u16 D_80121260;
extern u8 D_80123538;

extern s32 D_80123544;
extern s32 D_801234E8;

void osCreateScheduler(u16*, s32*, s32, u8, s32);
void osScAddClient(u16*, u8*, s32**, s32);

void func_8006C3E0(void) {
    s32 sp24;

    func_80070B30();
    func_800C6170();
    D_800DD374 = 1;
    if (func_8006F4EC() != 0) {
        D_800DD374 = 0;
    }
    D_80123514 = 0;
    D_80123518 = 0;
    
    if (osTvType == 0) {
        sp24 = 0xE;
    } else if (osTvType == 1) {
        sp24 = 0;
    } else if (osTvType == 2) {
        sp24 = 0x1C;
    }
    
    osCreateScheduler(&D_80121260, &D_801234E8, 0xD, sp24, 1);
    D_800DD3A0 = 0;
    if (func_8006EFB8() == 0) {
        D_800DD3A0 = 1;
    }
    func_8007A310(1, &D_80121260);
    func_80076BA0();
    func_80078100(&D_80121260);
    audio_init(&D_80121260);
    func_80008040();
    D_800DD384 = func_8006A10C();
    func_8007AC70();
    func_8005F850();
    func_8000BF8C();
    func_800B5E88();
    func_800598D0();
    func_800AE530();
    func_800AB1F0();
    func_8006E3BC();
    func_8006EFDC();
    func_800C3C00();
    func_80075B18();
    func_80081218();
    func_800C7350();
    osCreateMesgQueue(&D_80123548, &D_80123544, 1);
    osScAddClient(&D_80121260, &D_80123538, &D_80123548, 3);
    D_80123560 = 0;
    D_80123504 = 0;
    D_80123508 = 0;
    D_801234E8 = 0;
    
    // ???
    D_801211F8 = D_801211F0;
    fast3d_cmd(D_801211F8++, 0xE9000000, 0x00000000)
    fast3d_cmd(D_801211F8++, 0xB8000000, 0x00000000)
    
    osSetTime(0);
}
#endif

GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006C60C.s")

void func_8006CAE4(s32 arg0, s32 arg1, s32 arg2) {
    D_80123500 = arg0 - 1;
    if (arg1 == -1) {
        D_801234F4 = func_8009C1B0();
    } else {
        D_801234F4 = arg1;
    }
    func_8006CB58(D_801234F4, D_80123500, D_80123504, arg2);
}

void func_8006CB58(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_8006ECFC(arg1);
    func_800710B0(0);
    func_80065EA0();
    func_800C3048();
    func_8006B250(arg0, arg1, arg2, arg3, D_80123508);
    func_8009ECF0(func_80066210());
    func_800AE728(8, 0x10, 0x96, 0x64, 0x32, 0);
    func_8001BF20();
    osSetTime(0);
    func_800710B0(2);
    func_80072298(1);
}

void func_8006CC14(void) {
    func_800710B0(0);
    if (D_800DD38C == 0) {
        if (D_800DD3F0 != 1) {
            func_80077A54();
        }
        D_800DD38C = 1;
    }
    func_8006BEFC();
    func_800C01D8(&D_800DD3F4);
    func_800AE270();
    func_800A003C();
    func_800C30CC();
    D_801211F8 = D_801211F0[D_801234E8];
    fast3d_cmd(D_801211F8++, 0xE9000000, 0x00000000)
    fast3d_cmd(D_801211F8++, 0xB8000000, 0x00000000)
    func_800710B0(2);
}

// Has a jump table
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006CCF0.s")

void func_8006D8A4(void) {
    D_800DD390 = 0x2C;
    gIsPaused = 0;
    func_800945E4();
    func_800C01D8(&D_800DD408);
}

void func_8006D8E0(s32 arg0) {
    D_80123516 = arg0 + 1;
}

void func_8006D8F0(s32 arg0) {
    s32 temp;
    if (D_801234EC != 4) {
        D_801234F4 = D_80121250[0];
        D_80123504 = 0;
        D_80123508 = 0x64;
        temp = D_80121250[1];
        if (D_80121250[15] >= 0) {
            D_80123504 = D_80121250[15];
        }
        if (D_80121250[temp + 8] >= 0) {
            D_80123508 = D_80121250[temp + 8];
        }
        D_801234F8 = 1;
    }
}

void func_8006D968(s8 *arg0) {
    s32 i;
    if (D_801234EC != 4) {
        D_80121250[0] = D_801234F4;
        for(i = 0; i < 2; i++) {
            D_80121250[i + 2] = arg0[i + 8];
            D_80121250[i + 4] = arg0[i + 10];
            D_80121250[i + 6] = arg0[i + 12];
            D_80121250[i + 8] = arg0[i + 14];
            D_80121250[i + 10] = arg0[i + 18];
            D_80121250[i + 12] = arg0[i + 20];
        }
        D_80121250[14] = arg0[22];
        D_80121250[15] = arg0[23];
        D_801234FC = 1;
    }
}

s32 func_8006DA0C(void) {
    return D_801234EC;
}

/* Unused? */
void func_8006DA1C(s32 arg0) {
    D_801234EC = arg0;
}

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006DA28.s")
#else

// For some reason this function is storing arg0 in s0.

void func_8006DB3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_800813C0(s32);
void func_80004A60(s32, s32);
void MenuInit(s32);

void func_8006DA28(s32 arg0, s32 arg1, s32 arg2) {
    func_8006ECFC(0);
    D_801234EC = 1;
    D_801234F0 = 1;
    func_80004A60(0, 0x7FFF);
    func_80004A60(1, 0x7FFF);
    func_80004A60(2, 0x7FFF);
    func_80065EA0();
    
    if (D_80123514 == 0) {
        D_80123514 = 0; // Someone at rare: "I know I just checked if this was zero, but I need to be sure!"
        if (arg1 < 0) {
            D_80123514 = 1;
        } else {
            func_8006DB3C(arg1, -1, 0, 2, arg2);
        }
    }
    if (arg0 == 0 || arg0 == 1 || arg0 == 2) {
        func_800813C0(arg0);
    }
    MenuInit(arg0);
    D_80123504 = 0;
}
#endif

void func_8006DB14(s32 arg0) {
    D_80123518 = arg0;
}

void func_8006DB20(s32 arg0) {
    D_8012351C = arg0;
}

s32 func_8006DB2C(void) {
    return D_80123518;
}

void func_8006DB3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    func_800710B0(0);
    func_80065EA0();
    func_800C3048();
    func_8006B250(arg0, arg1, arg2, arg3, arg4);
    func_8009ECF0(func_80066210());
    func_800AE728(4, 4, 0x6E, 0x30, 0x20, 0);
    func_8001BF20();
    osSetTime(0);
    func_800710B0(2);
}

void func_8006DBE4(void) {
    if (D_80123514 == 0) {
        D_80123514 = 1;
        func_800710B0(0);
        func_8006BEFC();
        func_800C01D8(&D_800DD3F4);
        func_800AE270();
        func_800A003C();
        func_800C30CC();
        func_800710B0(2);
    }
    D_80123514 = 0;
}

void func_8006DC58(s32 arg0) {
    if (func_800C73E0() == 0) {
        func_80010994(arg0);
        particlePtrList_flush();
        func_8001BF20();
        D_80024D54(&D_801211F8, &D_80121208, &D_80121218, &D_80121228, arg0);
        func_800C3440(arg0);
        func_80078054(&D_801211F8);
        func_80077050(&D_801211F8);
        func_80077268(&D_801211F8);
    }
}

// Has a jump table
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006DCF8.s")

void func_8006E2E8(s32 arg0, s32 arg1, s32 arg2) {
    if (D_80123514 == 0) {
        func_8006DBE4();
        if (func_800C73E0() == 0) {
            D_801211F8 = D_801211F0[D_801234E8];
            fast3d_cmd(D_801211F8++, 0xE9000000, 0x00000000)
            fast3d_cmd(D_801211F8++, 0xB8000000, 0x00000000)
        }
    }
    if (arg0 != -1) {
        func_8006DB3C(arg0, arg1, 0, 2, arg2);
        D_80123514 = (u8)0;
        return;
    }
    D_80123514 = (u8)1;
}

GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006E3BC.s")

void func_8006E5BC(void) {
    s32 i, j;
    gSettingsPtr->playerCount = func_8009C3C8();
    for (i = 0; i < 8; i++) {
        gSettingsPtr->racers[i].best_times = 0;
        gSettingsPtr->racers[i].character = func_8009C228(i);
        if (gSettingsPtr->playerCount >= 2) {
            gSettingsPtr->racers[i].starting_position = i;
        } else if (is_in_two_player_adventure()) {
            gSettingsPtr->racers[i].starting_position = 5 - i;
        } else {
            gSettingsPtr->racers[i].starting_position = 7 - i;
        }
        gSettingsPtr->racers[i].unk7 = 0;
        for(j = 0; j < 4; j++) {
            gSettingsPtr->racers[i].placements[j] = 0;
        }
        gSettingsPtr->racers[i].course_time = 0;
        for(j = 0; j < 3; j++) {
            gSettingsPtr->racers[i].lap_times[j] = 0;
        }
    }
    gSettingsPtr->timeTrialRacer = 0;
    gSettingsPtr->unk115[0] = 0;
    gSettingsPtr->unk115[1] = 0;
    gSettingsPtr->display_times = 0;
    gSettingsPtr->worldId = 0;
    gSettingsPtr->courseId = 0;
}

GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006E770.s")

void func_8006E994(Settings* settings) {
    s32 i;
    s32 sp20;
    s32 sp1C;

    func_8006B224(&sp1C, &sp20);
    settings->newGame = 1;
    
    for (i = 0; i < sp20; i++) {
        settings->balloonsPtr[i] = 0;
    }
    for(i = 0; i < sp1C; i++) {
        settings->courseFlagsPtr[i] = 0;
    }
    
    settings->keys = 0;
    settings->unkA = 0;
    settings->bosses = 0;
    settings->trophies = 0;
    settings->cutsceneFlags = 0;
    settings->tajFlags = 0;
    settings->ttAmulet = 0;
    settings->wizpigAmulet = 0;
}

// Unused?
void func_8006EA58(void) {
    func_8006E770(gSettingsPtr, 3);
    func_8006E994(gSettingsPtr);
}

Settings* get_settings(void) {
    return gSettingsPtr;
}

s8 func_8006EAA0(void) {
    return gIsPaused;
}

s8 func_8006EAB0(void) {
    return D_80123516;
}

s32 func_8006EAC0(void) {
    if (D_80123560 == 0) {
        D_80123560 = (s32) ((osRecvMesg(&D_80123548, NULL, 0) + 1) != 0);
    }
    return D_80123560;
}

s32 func_8006EB14(void) {
    return D_801234F4;
}

/* Unused? */
void func_8006EB24(void) {
    D_800DD37C |= 0x01;
}

/* Unused? */
void func_8006EB40(void) {
    D_800DD37C |= 0x02;
}

void func_8006EB5C(void) {
    D_800DD37C |= 0x03;
}

void func_8006EB78(s32 arg0) {
    D_800DD37C &= -0x301;
    D_800DD37C |= (0x04 | ((arg0 & 3) << 8));
}

void func_8006EBA8(void) {
    D_800DD37C |= 0x08;
}

void func_8006EBC4(void) {
    D_800DD37C |= 0x10;
}

void func_8006EBE0(void) {
    D_800DD37C |= 0x20;
}

void func_8006EBFC(void) {
    D_800DD37C |= 0x30;
}

void func_8006EC18(s32 arg0) {
    D_800DD37C &= -0xC01;
    D_800DD37C |= (0x40 | ((arg0 & 3) << 0xA));
}

void func_8006EC48(s32 arg0) {
    if (D_801234EC == 0 && !is_in_tracks_mode()) {
        D_800DD37C &= -0xC01;
        D_800DD37C |= (0x40 | ((arg0 & 3) << 0xA));
    }
}

void func_8006ECAC(s32 arg0) {
    D_800DD37C = ((arg0 & 0x03) << 10) | 0x80;
}

/* Unused? */
void func_8006ECC4(void) {
    D_800DD37C |= 0x100;
}

void func_8006ECE0(void) {
    D_800DD37C |= 0x200;
}

GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006ECFC.s")

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006EFB8.s")
#else
s32 func_8006EFB8(void) {
    if (D_A4000000 != -1) { // regalloc issue with D_A4000000
        return 0;
    }
    return 1;
}
#endif

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006EFDC.s")
#else
    
/* Size: 0x40 bytes, I think this might be a 4x4 matrix. */
typedef struct unk80121200 {
    u8 pad0[0x40];
} unk80121200;

/* Size: 10 bytes */
typedef struct unk80121210 {
    u8 pad0[10];
} unk80121210;

/* Size: 16 bytes */
typedef struct unk80121220 {
    u8 pad0[16];
} unk80121220;

extern s32 D_8012350C;

extern s32 D_800DD3BC;
extern s32 D_800DD3CC;
extern s32 D_800DD3DC;
extern s32 D_800DD3EC;

extern s32 D_80123528;
extern s32 D_8012352C;
extern s32 D_80123530;
extern s32 D_80123534;

extern unk80121200 *D_80121200[3];
extern unk80121210 *D_80121210[3];
extern unk80121220 *D_80121220[3];

void *func_80070C9C(s32, s32); // Allocates memory?

void func_8006EFDC(void) {
    s32 size;
    s8* current;

    D_8012350C = 3;
    
    size = (D_800DD3EC * sizeof(unk80121220)) + 
           (D_800DD3BC * sizeof(Gfx)) + 
           (D_800DD3DC * sizeof(unk80121200)) + 
           (D_800DD3CC * sizeof(unk80121210));
    
    current = (s8*)func_80070C9C(size, 0xFF0000FF);
    D_801211F0[0] = (Gfx*)current;
    current += (D_800DD3BC * sizeof(Gfx));
    D_80121200[0] = (unk80121200*)current;
    current += (D_800DD3DC * sizeof(unk80121200));
    D_80121210[0] = (unk80121210*)current;
    current += (D_800DD3CC * sizeof(unk80121210));
    D_80121220[0] = (unk80121220*)current;
    
    current = (s8*)func_80070C9C(size, 0xFFFF00FF);
    D_801211F0[1] = (Gfx*)current;
    current += (D_800DD3BC * sizeof(Gfx));
    D_80121200[1] = (unk80121200*)current;
    current += (D_800DD3DC * sizeof(unk80121200));
    D_80121210[1] = (unk80121210*)current;
    current += (D_800DD3CC * sizeof(unk80121210));
    D_80121220[1] = (unk80121220*)current;
    
    D_80123528 = D_800DD3BC;
    D_8012352C = D_800DD3DC;
    D_80123530 = D_800DD3EC;
    D_80123534 = D_800DD3CC;
}
#endif

void func_8006F140(s32 arg0) {
    if (D_800DD394 == 0) {
        D_800DD394 = 0x28;
        D_80123524 = 0;
        D_80123526 = 0;
        if (arg0 == 1) {
            func_800C01D8(&D_800DD41C);
        }
        if (arg0 == 3) {
            D_800DD394 = 0x11A;
            func_800C01D8(&D_800DD424);
        }
        if (arg0 == 4) {
            D_800DD394 = 0x168;
            func_800C01D8(&D_800DD424);
        }
        if (arg0 == 0) {
            D_800DD394 = 2;
        }
    }
}

// Unused?
void func_8006F20C(void) {
    if (D_800DD394 == 0) {
        func_800C01D8(&D_800DD41C);
        D_800DD394 = 0x28;
        D_80123524 = 1;
    }
}

void func_8006F254(void) {
    if (D_800DD394 == 0) {
        func_800C01D8(&D_800DD41C);
        D_800DD394 = 0x28;
        D_80123524 = 2;
    }
}

void func_8006F29C(void) {
    if (D_800DD394 == 0) {
        if ((gSettingsPtr->trophies & 0xFF) == 0xFF && !(gSettingsPtr->cutsceneFlags & 1) && gSettingsPtr->bosses & 1) {
            gSettingsPtr->cutsceneFlags |= 1;
            func_800C01D8(&D_800DD41C);
            D_800DD394 = 0x28;
            D_80123525 = 0x2D;
            D_80123524 = 3;
        }
    }
}

void func_8006F338(s32 arg0) {
    if (D_800DD394 == 0) {
        D_80123525 = arg0;
        func_800C01D8(&D_800DD41C);
        D_800DD394 = 0x28;
        D_80123524 = 4;
    }
}

void func_8006F388(u8 arg0) {
    D_800DD398 = arg0;
}

void func_8006F398(void) {
    s32 i;
    u8 temp;
    u8 *first_racer_data;
    u8 *second_racer_data;

    func_8006A50C();
    func_8000E194();
    
    first_racer_data = (u8*)(gSettingsPtr->racers);
    second_racer_data = (u8*)(gSettingsPtr->racers + 1);
    
    for(i = 0; i < sizeof(Racer); i++) {
        temp = first_racer_data[i];
        first_racer_data[i] = second_racer_data[i];
        second_racer_data[i] = temp;
    }
}

void func_8006F42C(void) {
    D_800DD3F0 = 2;
}

void func_8006F43C(void) {
    s32 phi_s1 = 0;
    s32 phi_s0 = 0;
    while(phi_s0 != 4) {
        phi_s1 |= func_8006A528(phi_s0);
        phi_s0++;
    }
    if (phi_s1 & 0x1000) {
        D_800DFD94 = 1;
    }
    D_80123520++;
    if (D_80123520 >= 8) {
        func_8006DA28(0x1A, 0x27, 2);
    }
}

s32 func_8006F4C8(void) {
    if (D_800DD384 == -1) {
        return 1;
    } else {
        return 0;
    }
}

#if 1
GLOBAL_ASM("asm/non_matchings/unknown_066AA0/func_8006F4EC.s")
#else
s32 func_8006F4EC(void) {
    if (SP_IMEM != 0x17D7) { // Same regalloc issue as func_8006EFB8
        return 0;
    }
    return 1;
}
#endif
