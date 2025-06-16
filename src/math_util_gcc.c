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

s16 *gArcTanTable;

void trigtable_generate(void) {
    s32 i;
    const f32 scale = 10491.0f;

    gArcTanTable = mempool_alloc(sizeof(s16) * 1024, PP_RAM_TRIGTABLE);

    for (i = 0; i < 1024; ++i) {
        f32 x = (f32)i / 1024.0f; // x in [0, 1)
        f32 y = x / (1.0f + 0.28f * x * x);
        s16 value = (s16)(y * scale + 0.5f);
        gArcTanTable[i] = value;
    }
}

u32 interrupts_disable(void) {
    if (gIntDisFlag) {
        return __osDisableInt();
    }
}

void interrupts_enable(u32 flags) {
    if (gIntDisFlag) {
        __osRestoreInt(flags);
    }
}

void set_gIntDisFlag(u8 setting) {
    gIntDisFlag = setting;
}

u8 get_gIntDisFlag(void) {
    return gIntDisFlag;
}

static inline void mtx_invalidate(void* addr) {
    asm volatile (
        "cache 0xD, 0x00(%0);"
        "cache 0xD, 0x10(%0);"
        "cache 0xD, 0x20(%0);"
        "cache 0xD, 0x30(%0);"
        :            
        : "r"(addr)        
    );
}

void mtxf_to_mtxs(MtxF *mf, MtxS *mi) {
    s32 i, j;
    mtx_invalidate(mi);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            (*mi)[i][j] = FTOFIX32((*mf)[i][j]);
        }
    }
}

void mtxf_transform_point(MtxF *mf, float x, float y, float z, float *ox, float *oy, float *oz) {
    *ox = (*mf)[0][0] * x + (*mf)[1][0] * y + (*mf)[2][0] * z + (*mf)[3][0];
    *oy = (*mf)[0][1] * x + (*mf)[1][1] * y + (*mf)[2][1] * z + (*mf)[3][1];
    *oz = (*mf)[0][2] * x + (*mf)[1][2] * y + (*mf)[2][2] * z + (*mf)[3][2];
}

/*void mtxf_transform_dir(MtxF *mf, Vec3f *in, Vec3f *out) {
    out->f[0] = (in->f[0] * (*mf)[0][0]) + (in->f[1] * (*mf)[1][0]) + (in->f[2] * (*mf)[2][0]);
    out->f[1] = (in->f[0] * (*mf)[0][1]) + (in->f[1] * (*mf)[1][1]) + (in->f[2] * (*mf)[2][1]);
    out->f[2] = (in->f[0] * (*mf)[0][2]) + (in->f[1] * (*mf)[1][2]) + (in->f[2] * (*mf)[2][2]);
}*/

void mtxf_mul(MtxF *mat1, MtxF *mat2, MtxF *output) {
    s32 i, j, k;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            /*
            (*output)[i][j] = 0.0f;
            for (k = 0; k < 4; k++) {
                (*output)[i][j] += (*mat1)[i][k] * (*mat2)[k][j];
            }
            */
            // Reordered addition to preserve exact bitwise result
            (*output)[i][j] = ((*mat1)[i][1] * (*mat2)[1][j] + (*mat1)[i][2] * (*mat2)[2][j]) +
                              ((*mat1)[i][0] * (*mat2)[0][j] + (*mat1)[i][3] * (*mat2)[3][j]);
        }
    }
}

void mtxf_to_mtx(MtxF *mf, Mtx *m) {
    s32 i, j;
    s32 e1, e2;
    s32 *ai, *af;
    mtx_invalidate(m);

    ai = &m->m[0][0];
    af = &m->m[2][0];

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j += 2) {
            e1 = FTOFIX32((*mf)[i][j]);
            e2 = FTOFIX32((*mf)[i][j + 1]);
            *ai++ = (e1 & 0xFFFF0000) | ((e2 >> 16) & 0xFFFF);
            *af++ = ((e1 << 16) & 0xFFFF0000) | (e2 & 0xFFFF);
        }
    }
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

s32 rand_range(s32 min, s32 max) {
    s64 temp = gCurrentRNGSeed;

    temp = (temp << 32) | (temp >> 1);
    temp = temp ^ ((gCurrentRNGSeed & 0xFFFFF) << 12);
    gCurrentRNGSeed = temp ^ ((temp >> 20) & 0xFFF);

    return (u32) (gCurrentRNGSeed - min) % (max - min + 1) + min;
}

void vec3s_reflect(Vec3s *vec, Vec3s *n) {
    s32 proj_x2 = (vec->x * n->x + vec->y * n->y + vec->z * n->z) >> 12;

    vec[1].x = ((proj_x2 * n->x) >> 13) - vec->x;
    vec[1].y = ((proj_x2 * n->y) >> 13) - vec->y;
    vec[1].z = ((proj_x2 * n->z) >> 13) - vec->x; //!@bug: should be vec->z
}

void mtxs_transform_dir(MtxS *mi, Vec3s *vec) {
    s16 x = vec->x;
    s16 y = vec->y;
    s16 z = vec->z;

    vec->x = ((*mi)[0][0] * x + (*mi)[1][0] * y + (*mi)[2][0] * z) >> 16;
    vec->y = ((*mi)[0][1] * x + (*mi)[1][1] * y + (*mi)[2][1] * z) >> 16;
    vec->z = ((*mi)[0][2] * x + (*mi)[1][2] * y + (*mi)[2][2] * z) >> 16;
}

void mtxf_from_transform(MtxF *mtx, ObjectTransform *trans) {
    f32 yRotSine;
    f32 yRotCosine;
    f32 xRotSine;
    f32 xRotCosine;
    f32 zRotSine;
    f32 zRotCosine;
    f32 scale;

    yRotSine = sins_s16(trans->rotation.y_rotation) * (1.0f / 0x10000);
    yRotCosine = coss_s16(trans->rotation.y_rotation) * (1.0f / 0x10000);
    xRotSine = sins_s16(trans->rotation.x_rotation) * (1.0f / 0x10000);
    xRotCosine = coss_s16(trans->rotation.x_rotation) * (1.0f / 0x10000);
    zRotSine = sins_s16(trans->rotation.z_rotation) * (1.0f / 0x10000);
    zRotCosine = coss_s16(trans->rotation.z_rotation) * (1.0f / 0x10000);
    scale = trans->scale;

    (*mtx)[0][0] = (xRotSine * yRotSine * zRotSine + zRotCosine * yRotCosine) * scale;
    (*mtx)[0][1] = (zRotSine * xRotCosine) * scale;
    (*mtx)[0][2] = (xRotSine * yRotCosine * zRotSine - zRotCosine * yRotSine) * scale;
    (*mtx)[0][3] = 0;
    (*mtx)[1][0] = (xRotSine * yRotSine * zRotCosine - zRotSine * yRotCosine) * scale;
    (*mtx)[1][1] = (zRotCosine * xRotCosine) * scale;
    (*mtx)[1][2] = (xRotSine * yRotCosine * zRotCosine + zRotSine * yRotSine) * scale;
    (*mtx)[1][3] = 0;
    (*mtx)[2][0] = (xRotCosine * yRotSine) * scale;
    (*mtx)[2][1] = -(xRotSine * scale);
    (*mtx)[2][2] = (xRotCosine * yRotCosine) * scale;
    (*mtx)[2][3] = 0;
    (*mtx)[3][0] = trans->x_position;
    (*mtx)[3][1] = trans->y_position;
    (*mtx)[3][2] = trans->z_position;
    (*mtx)[3][3] = 1.0f;
}

void mtxf_scale_y(MtxF *input, f32 scale) {
    (*input)[1][0] *= scale;
    (*input)[1][1] *= scale;
    (*input)[1][2] *= scale;
}

void mtxf_translate_y(MtxF *input, f32 offset) {
    (*input)[3][0] += (*input)[1][0] * offset;
    (*input)[3][1] += (*input)[1][1] * offset;
    (*input)[3][2] += (*input)[1][2] * offset;
}

void mtxf_from_inverse_transform(MtxF *mtx, ObjectTransform *trans) {
    f32 yRotSine;
    f32 yRotCosine;
    f32 xRotSine;
    f32 xRotCosine;
    f32 zRotSine;
    f32 zRotCosine;

    yRotCosine = coss_s16(trans->rotation.y_rotation) * (1.0f / 0x10000);
    yRotSine = sins_s16(trans->rotation.y_rotation) * (1.0f / 0x10000);
    xRotCosine = coss_s16(trans->rotation.x_rotation) * (1.0f / 0x10000);
    xRotSine = sins_s16(trans->rotation.x_rotation) * (1.0f / 0x10000);
    zRotCosine = coss_s16(trans->rotation.z_rotation) * (1.0f / 0x10000);
    zRotSine = sins_s16(trans->rotation.z_rotation) * (1.0f / 0x10000);

    (*mtx)[0][0] = yRotCosine * zRotCosine - xRotSine * zRotSine * yRotSine;
    (*mtx)[0][1] = xRotSine * zRotCosine * yRotSine + yRotCosine * zRotSine;
    (*mtx)[0][2] = -(yRotSine * xRotCosine);
    (*mtx)[0][3] = 0;
    (*mtx)[1][0] = -(xRotCosine * zRotSine);
    (*mtx)[1][1] = xRotCosine * zRotCosine;
    (*mtx)[1][2] = xRotSine;
    (*mtx)[1][3] = 0;
    (*mtx)[2][0] = xRotSine * zRotSine * yRotCosine + yRotSine * zRotCosine;
    (*mtx)[2][1] = yRotSine * zRotSine - xRotSine * zRotCosine * yRotCosine;
    (*mtx)[2][2] = yRotCosine * xRotCosine;
    (*mtx)[2][3] = 0;
    (*mtx)[3][0] =
        ((*mtx)[0][0] * trans->x_position) + ((*mtx)[1][0] * trans->y_position) + ((*mtx)[2][0] * trans->z_position);
    (*mtx)[3][1] =
        ((*mtx)[0][1] * trans->x_position) + ((*mtx)[1][1] * trans->y_position) + ((*mtx)[2][1] * trans->z_position);
    (*mtx)[3][2] =
        ((*mtx)[0][2] * trans->x_position) + ((*mtx)[1][2] * trans->y_position) + ((*mtx)[2][2] * trans->z_position);
    (*mtx)[3][3] = 1.0f;
}

void mtxf_billboard(MtxF *mtx, s32 angle, f32 scale, f32 scaleY) {
    f32 cosine, sine;

    sine = sins_s16(angle) * (1.0f / 0x10000);
    cosine = coss_s16(angle) * (1.0f / 0x10000);
    (*mtx)[0][0] = cosine * scale;
    (*mtx)[0][1] = sine * scale;
    (*mtx)[0][2] = 0;
    (*mtx)[0][3] = 0;
    (*mtx)[1][0] = -sine * scale;
    (*mtx)[1][1] = (cosine * scale) * scaleY;
    (*mtx)[1][2] = 0;
    (*mtx)[1][3] = 0;
    (*mtx)[2][0] = 0;
    (*mtx)[2][1] = 0;
    (*mtx)[2][2] = scale;
    (*mtx)[2][3] = 0;
    (*mtx)[3][0] = 0;
    (*mtx)[3][1] = 0;
    (*mtx)[3][2] = 0;
    (*mtx)[3][3] = 1.0f;
}

void vec3s_rotate_rpy(RPYAngles *rotation, Vec3s *vec) {
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 sine, cosine;

    x1 = vec->x;
    y1 = vec->y;
    z1 = vec->z;

    sine = sins_s16(rotation->z_rotation);
    cosine = coss_s16(rotation->z_rotation);
    x2 = (x1 * cosine - y1 * sine) >> 16;
    y2 = (y1 * cosine + x1 * sine) >> 16;
    z2 = z1;

    sine = sins_s16(rotation->x_rotation);
    cosine = coss_s16(rotation->x_rotation);
    x1 = x2;
    y1 = (y2 * cosine - z2 * sine) >> 16;
    z1 = (z2 * cosine + y2 * sine) >> 16;

    sine = sins_s16(rotation->y_rotation);
    cosine = coss_s16(rotation->y_rotation);
    x2 = (x1 * cosine + z1 * sine) >> 16;
    y2 = y1;
    z2 = (z1 * cosine - x1 * sine) >> 16;

    vec->x = x2;
    vec->y = y2;
    vec->z = z2;
}

void vec3f_rotate(Vec3s *rotation, Vec3f *vec) {
    f32 sine;
    f32 cosine;
    f32 x1, y1, z1;
    f32 x2, y2, z2;

    x1 = vec->x;
    y1 = vec->y;
    z1 = vec->z;

    sine = sins_f(rotation->z_rotation);
    cosine = coss_f(rotation->z_rotation);
    x2 = x1 * cosine - y1 * sine;
    y2 = y1 * cosine + x1 * sine;
    z2 = z1;

    sine = sins_f(rotation->x_rotation);
    cosine = coss_f(rotation->x_rotation);
    x1 = x2;
    y1 = y2 * cosine - z2 * sine;
    z1 = z2 * cosine + y2 * sine;

    sine = sins_f(rotation->y_rotation);
    cosine = coss_f(rotation->y_rotation);
    x2 = x1 * cosine + z1 * sine;
    y2 = y1;
    z2 = z1 * cosine - x1 * sine;

    vec->x = x2;
    vec->y = y2;
    vec->z = z2;
}

void vec3f_rotate_ypr(Vec3s *rotation, Vec3f *vec) {
    f32 sine;
    f32 cosine;
    f32 x1, y1, z1;
    f32 x2, y2, z2;

    x1 = vec->x;
    y1 = vec->y;
    z1 = vec->z;

    sine = sins_f(rotation->y_rotation);
    cosine = coss_f(rotation->y_rotation);
    x2 = x1 * cosine + z1 * sine;
    y2 = y1;
    z2 = z1 * cosine - x1 * sine;

    sine = sins_f(rotation->x_rotation);
    cosine = coss_f(rotation->x_rotation);
    x1 = x2;
    y1 = y2 * cosine - z2 * sine;
    z1 = z2 * cosine + y2 * sine;

    sine = sins_f(rotation->z_rotation);
    cosine = coss_f(rotation->z_rotation);
    x2 = x1 * cosine - y1 * sine;
    y2 = y1 * cosine + x1 * sine;
    z2 = z1;

    vec->x = x2;
    vec->y = y2;
    vec->z = z2;
}

void vec3f_rotate_py(Vec3s *rotation, Vec3f *vec) {
    f32 sinX;
    f32 cosX;
    f32 sinY;
    f32 cosY;
    f32 z;

    sinX = sins_f(rotation->x_rotation);
    cosX = coss_f(rotation->x_rotation);
    sinY = sins_f(rotation->y_rotation);
    cosY = coss_f(rotation->y_rotation);

    z = vec->z;

    vec->x = z * cosX * sinY;
    vec->y = -z * sinX;
    vec->z = z * cosX * cosY;
}

s32 tri2d_xz_contains_point(s32 x, s32 z, Vec3s *pointA, Vec3s *pointB, Vec3s *pointC) {
    s32 aX, aZ, bX, bZ, cX, cZ;
    s32 var_a1;
    s32 var_a2;
    s32 var_a3;

    aX = pointA->x;
    aZ = pointA->z;
    bX = pointB->x;
    bZ = pointB->z;
    cX = pointC->x;
    cZ = pointC->z;

    var_a3 = (x - aX) * (bZ - aZ) - (bX - aX) * (z - aZ) >= 0;
    var_a2 = (x - bX) * (cZ - bZ) - (cX - bX) * (z - bZ) >= 0;
    var_a1 = (x - cX) * (aZ - cZ) - (aX - cX) * (z - cZ) >= 0;
    return var_a3 == var_a2 && var_a2 == var_a1;
}

void mtxf_from_translation(MtxF *mtx, f32 x, f32 y, f32 z) {
    s32 i, j;

    // Clear matrix
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            (*mtx)[i][j] = 0;
        }
    }
    (*mtx)[0][0] = 1.0f;
    (*mtx)[1][1] = 1.0f;
    (*mtx)[2][2] = 1.0f;
    (*mtx)[3][3] = 1.0f;
    (*mtx)[3][0] = x;
    (*mtx)[3][1] = y;
    (*mtx)[3][2] = z;
}

void mtxf_from_scale(MtxF *mtx, f32 scaleX, f32 scaleY, f32 scaleZ) {
    s32 i, j;

    // Clear matrix
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            (*mtx)[i][j] = 0;
        }
    }

    (*mtx)[0][0] = scaleX;
    (*mtx)[1][1] = scaleY;
    (*mtx)[2][2] = scaleZ;
    (*mtx)[3][3] = 1.0f;
}

static u16 atan2_lookup(f32 y, f32 x) {
    u16 ret;

    if (x == 0) {
        ret = gArcTanTable[0]; 
    } else {
        ret = gArcTanTable[(s32) (y / x * 1024 + 0.5f)];
    }
    return ret;
}

s32 atan2s(s32 xDelta, s32 zDelta) {
    u16 ret;

    if (xDelta >= 0) {
        if (zDelta >= 0) {
            if (zDelta >= xDelta) {
                ret = atan2_lookup(xDelta, zDelta);
            } else {
                ret = 0x4000 - atan2_lookup(zDelta, xDelta);
            }
        } else {
            zDelta = -zDelta;
            if (zDelta < xDelta) {
                ret = 0x4000 + atan2_lookup(zDelta, xDelta);
            } else {
                ret = 0x8000 - atan2_lookup(xDelta, zDelta);
            }
        }
    } else {
        xDelta = -xDelta;
        if (zDelta < 0) {
            zDelta = -zDelta;
            if (zDelta >= xDelta) {
                ret = 0x8000 + atan2_lookup(xDelta, zDelta);
            } else {
                ret = 0xC000 - atan2_lookup(zDelta, xDelta);
            }
        } else {
            if (zDelta < xDelta) {
                ret = 0xC000 + atan2_lookup(zDelta, xDelta);
            } else {
                ret = -atan2_lookup(xDelta, zDelta);
            }
        }
    }
    return ret;
}

u16 arctan2_f(f32 y, f32 x) {
    return atan2s((s32) (y * 255.0f), (s32) (x * 255.0f));
}

f32 area_triangle_2d(f32 x0, f32 z0, f32 x1, f32 z1, f32 x2, f32 z2) {
    f32 dx0 = x1 - x0;
    f32 dz0 = z1 - z0;
    f32 dx1 = x2 - x1;
    f32 dz1 = z2 - z1;
    f32 dx2 = x0 - x2;
    f32 dz2 = z0 - z2;
    f32 d0 = sqrtf((dx0 * dx0) + (dz0 * dz0)); // Distance between points 0 & 1
    f32 d1 = sqrtf((dx1 * dx1) + (dz1 * dz1)); // Distance between points 1 & 2
    f32 d2 = sqrtf((dx2 * dx2) + (dz2 * dz2)); // Distance between points 2 & 0
    f32 m = 0.5f * (d0 + d1 + d2);             // Half the sum of the distances?
    f32 result = m * (m - d0) * (m - d1) * (m - d2);
    if (result < 0.0f) {
        result = 0.0f;
    }
    return sqrtf(result);
}