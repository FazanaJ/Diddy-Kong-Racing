#include "collision.h"

#include "types.h"
#include "macros.h"
#include "textures_sprites.h"

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
