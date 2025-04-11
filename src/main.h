#ifndef _MAIN_H_
#define _MAIN_H_

#include "types.h"
#include "macros.h"
#include <ultra64.h>

#define REGIONMODE_PAL50 0
#define REGIONMODE_NTSC 1
#define REGIONMODE_MPAL 2
#define REGIONMODE_PAL60 3

#define RESOLUTION_320x240 0    // Base framebuffer size
#define RESOLUTION_384x240 1    // 16:10 framebuffer size, expansion pak only
#define RESOLUTION_424x240 2    // 16:9 framebuffer size, expansion pak only
#define RESOLUTION_400x300 3    // Medium Res framebuffer size, expansion pak only
#define RESOLUTION_480x360 4    // High Res framebuffer size, expansion pak only

#define SCREENBITS_16b 0    // 16 bit framebuffer
#define SCREENBITS_32b 1    // 32 bit framebuffer, expansion pak only

#define AA_OFF -1   // Anti Aliasing disabled
#define AA_FAST 0   // Reduced aliasing for levels, Anti Aliasing for objects
#define AA_FANCY 1  // Anti Aliasing enabled

#define MAGIC_NUMBER 0x14   // Magic Number for a magic man

typedef struct ConfigBits {
    unsigned magic : 8;     // Checksum-ish
    // General
    unsigned sameStats : 1;
    unsigned screenRegion : 3;
    // Graphics
    unsigned antiAliasing : 2;
    signed screenPosX : 5;
    signed screenPosY : 5;
    unsigned screenRes : 3;
    unsigned dedither : 1;
    unsigned frameCap : 2;
    unsigned screenBits : 1;
    unsigned terrainQuality : 1;
    // Multiplayer
    unsigned multiMusic : 1;
    unsigned multiObjects : 1;
    unsigned multiWaves : 1;
    unsigned multiParticles : 1;
    unsigned multiWeather : 1;
    unsigned multiModels : 1;
    unsigned multiSky : 1;
} ConfigBits;

typedef struct UserConfig {
    // General
    s8 sameStats;       // All characters use T.T stats.
    s8 screenRegion;    // What VI mode to base the game off of.
    // Graphics
    s8 antiAliasing;     // Anti Aliasing mode
    s8 screenPosX;      // For sub 320 wide framebuffers, shifts it horizontally
    s8 screenPosY;      // For sub 240 high framebuffers, shifts it vertically
    s8 screenRes;       // Framebuffer size
    s8 dedither;        // Use the dedither filter
    s8 frameCap;        // Cap the framerate
    s8 screenBits;      // Use 16 or 32 bit framebuffer
    s8 terrainQuality;  // Set frustum ratio for higher quality shading.
    // Multiplayer
    s8 multiMusic;      // Enable music in multiplayer
    s8 multiObjects;    // Enable single player objects in multiplayer
    s8 multiWaves;      // Enable wavegen in multiplayer
    s8 multiParticles;  // Enable single player particles in multiplayer
    s8 multiWeather;    // Enable weather effects in multiplayer
    s8 multiModels;     // Use high quality models
    s8 multiSky;        // Use skydome rather than gradient
} UserConfig;

extern u8 gExpansionPak;
extern u8 gUseExpansionMemory;
extern UserConfig gConfig;

void mainproc(void);
void thread1_main(void *);
void thread3_verify_stack(void);

#endif
