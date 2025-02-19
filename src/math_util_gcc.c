/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x8006F510 */

#include "math_util.h"

#include <math.h>
#include "types.h"
#include "macros.h"
#include "structs.h"
#include "game.h"

extern s32 gIntDisFlag;
extern s32 gCurrentRNGSeed; // Official Name: rngSeed
extern s32 gPrevRNGSeed;
extern s16 gSineTable[];
extern s16 gArcTanTable[];

/**
 * Zero out the interrupt mask. This stops this thread
 * from being interrupted by others, letting you safely
 * work with delicate areas in memory. Kind of like a mutex.
 * Returns what the interrupt mask wask before.
 * Official Name: disableInterrupts */
u32 interrupts_disable(void) {
    if (gIntDisFlag) {
        return __osDisableInt();
    } else {
        return -1;
    }
}

/**
 * Set the interrupt mask to whichever flags were given.
 * Required after zeroing them out, otherwise system
 * operation won't work as normal.
 * Official Name: enableInterrupts */
void interrupts_enable(u32 flags) {
    if (gIntDisFlag) {
        __osRestoreInt(flags);
    }
}

void set_gIntDisFlag(s8 setting) {
    gIntDisFlag = setting;
}

void guMtxXFMF(Matrix mf, float x, float y, float z, float *ox, float *oy, float *oz) {
    *ox = mf[0][0] * x + mf[1][0] * y + mf[2][0] * z + mf[3][0];
    *oy = mf[0][1] * x + mf[1][1] * y + mf[2][1] * z + mf[3][1];
    *oz = mf[0][2] * x + mf[1][2] * y + mf[2][2] * z + mf[3][2];
}

/* Official Name: mathSeed */
void set_rng_seed(s32 num) {
    gCurrentRNGSeed = num;
}

void save_rng_seed(void) {
    gPrevRNGSeed = gCurrentRNGSeed;
}

void load_rng_seed(void) {
    gCurrentRNGSeed = gPrevRNGSeed;
}

s32 get_rng_seed(void) {
    return gCurrentRNGSeed;
}

s32 get_random_number_from_range(s32 min, s32 max) {
    s32 newSeed;
    u64 curSeed;

    curSeed = (((u64) ((s64) gCurrentRNGSeed << 0x3F) >> 0x1F) | ((u64) ((s64) gCurrentRNGSeed << 0x1F) >> 0x20)) ^
              ((u64) ((s64) gCurrentRNGSeed << 0x2C) >> 0x20);
    newSeed = ((curSeed >> 0x14) & 0xFFF) ^ curSeed;
    gCurrentRNGSeed = newSeed;
    return ((u32) (newSeed - min) % (u32) ((max - min) + 1)) + min;
}

/**
 * Signed distance field calculation. It's used to calculate the level of intersection between a point and a triangle.
 */
f32 area_triangle_2d(f32 x0, f32 z0, f32 x1, f32 z1, f32 x2, f32 z2) {
    f32 d0_sq = (x1 - x0) * (x1 - x0) + (z1 - z0) * (z1 - z0);
    f32 d1_sq = (x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1);
    f32 d2_sq = (x0 - x2) * (x0 - x2) + (z0 - z2) * (z0 - z2);

    f32 d0 = sqrtf(d0_sq);
    f32 d1 = sqrtf(d1_sq);
    f32 d2 = sqrtf(d2_sq);

    f32 m = 0.5f * (d0 + d1 + d2);
    f32 area_sq = fabsf(m * (m - d0) * (m - d1) * (m - d2));

    return sqrtf(area_sq);
}
