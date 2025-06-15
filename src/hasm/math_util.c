#include "math_util.h"

#include <math.h>
#include "types.h"
#include "macros.h"
#include "structs.h"
#include "game.h"
#include "string.h"
#include "PR/os_internal_reg.h"

extern u8 gIntDisFlag;
extern s32 gCurrentRNGSeed; // Official Name: rngSeed
extern s32 gPrevRNGSeed;
extern s16 gSineTable[];
extern s16 gArcTanTable[];

#undef NON_MATCHING

/**
 * All of the functions below are handwritten assembly. Because of this, matching C code is impossible.
 * Nonmatching is not, so functionally equivalent C code can be here to replace these handwritten functions in
 * nonmatching builds. Variables cannot be declared here because of the way they're aligned, so they have to stay in an
 * assembly file.
 */

/******************************/


GLOBAL_ASM("asm/math_util/mtxf_transform_dir.s")

GLOBAL_ASM("asm/math_util/sins_f.s")
GLOBAL_ASM("asm/math_util/coss_f.s")
GLOBAL_ASM("asm/math_util/coss.s")
GLOBAL_ASM("asm/math_util/sins_2.s")
