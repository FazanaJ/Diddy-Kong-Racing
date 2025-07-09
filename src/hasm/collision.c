#include "collision.h"

#include "macros.h"
#include "PR/R4300.h"
#include "textures_sprites.h"
#include "types.h"

/*******************************/

extern LevelModel *gCurrentLevelModel;

extern f32 gCollisionNormalX;
extern f32 gCollisionNormalY;
extern f32 gCollisionNormalZ;
extern s32 gHitWall;
extern s32 gCollisionMode;

extern s32 *gCollisionCandidates;
extern s8 *gCollisionSurfaces;
extern s32 gNumCollisionCandidates;

// All handwritten assembly, below.
