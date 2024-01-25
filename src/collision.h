#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "types.h"
#include "structs.h"

void func_80031130(s32, f32 *, f32*, s32);
s32 func_800314DC(LevelModelSegmentBoundingBox *a0, s32 a1, s32 a2, s32 a3, s32 t5);
s32 func_80031600(f32 *arg0, f32 *arg1, f32 *arg2, s8 *arg3, s32, s32 *arg5);

void func_80031130_2(s32, f32*, f32*, s32);
s32 func_80031600_2(f32*, f32*, f32*, s8* surface, s32, s32*);
s32 func_80061D30_2(Object *);

#endif
