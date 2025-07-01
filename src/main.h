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

enum PlatformFlags {
    CONSOLE =           (1 << 0), // Original Nintendo 64 hardware
    EMULATOR =          (1 << 1), // Generic N64 emulator
    IQUE =              (1 << 3), // iQue Player.
    ARES =              (1 << 4), // More accurate N64 Emulator.
    CF_2 =              (1 << 5), // Counter factor 2 is enabled.
    FBE =               (1 << 6), // Framebuffer emulation is enabled.
    DBE =               (1 << 7), // Depthbuffer emulation is enabled.
    PJ64_1 =            (1 << 8), // Sound the alarms, defcon1, PJ64 has been detected behind enemy lines.
    PJ64_3 =            (1 << 9),
    PJ64_4 =            (1 << 10),
    MUPEN_OLD =         (1 << 11),
    MUPEN_NEXT =        (1 << 12),
    SIMPLE64 =          (1 << 13),
    PARALLEL_LAUNCHER = (1 << 14),
};


typedef struct ConfigBits {
    unsigned magic : 8;     // Checksum-ish
    // General
    unsigned sameStats : 1;
    unsigned screenRegion : 2;
    // Graphics
    unsigned screenWidth : 2;
    signed antiAliasing : 2;
    signed screenPosX : 5;
    signed screenPosY : 5;
    unsigned screenRes : 3;
    unsigned dedither : 1;
    unsigned frameCap : 2;
    unsigned screenBits : 1;
    unsigned terrainQuality : 1;
    // Multiplayer - every option needs to be 2 bits, because the option will be 0-3, representing how many players before it's disabled
    unsigned multiMusic : 2;
    unsigned multiObjects : 2;
    unsigned multiWaves : 2;
    unsigned multiParticles : 2;
    unsigned multiWeather : 2;
    unsigned multiModels : 2;
    unsigned multiSky : 2;
    unsigned multiAA : 2;
} ConfigBits;

typedef struct UserConfig {
    // General
    s8 sameStats;       // All characters use T.T stats.
    s8 screenRegion;    // What VI mode to base the game off of.
    // Graphics
    s8 screenWidth;     // Widescreen or no
    s8 antiAliasing;    // Anti Aliasing mode
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
    s8 multiAA;         // Multiplayer anti aliasing setting
} UserConfig;

extern u8 gExpansionPak;
extern u8 gUseExpansionMemory;
extern UserConfig gConfig;
extern u16 gPlatform;

void mainproc(void);
void thread1_main(void *);
void thread3_verify_stack(void);
void get_platform(void);


typedef enum DebugPages {
    PAGE_MINIMAL,
    PAGE_MEMORY,
    PAGE_MISC,
    PAGE_ASSETS,
    PAGE_OVERVIEW,
    PAGE_GENERAL,
    PAGE_BREAKDOWN,
    PAGE_AUDIO,
    PAGE_LOG,
    PAGE_VISCVG,
} DebugPages;

typedef enum DebugProfiles {
    PP_THREAD5,
    PP_THREAD4,
    PP_THREAD3,
    PP_YIELD3,

    PP_RSP_GFX,
    PP_RSP_AUD,

    PP_RDP_BUF,
    PP_RDP_TMM,
    PP_RDP_BUS,
    PP_RDP_CLK,

    PP_TOTAL
} DebugProfiles;

typedef enum DebugRSP {
    RSP_GFX_START,
    RSP_GFX_END,
    RSP_AUD_START,
    RSP_AUD_END,
    RSP_GFX_YIELD,
    RSP_GFX_RESUME,

    RSP_CONTEXT_COUNT
} DebugRSP;

// Entries must be ordered in thread priority, the profiler will compare times with higher prio threads
typedef enum DebugThreads {
    THREAD5_START,
    THREAD5_END,
    THREAD4_START,
    THREAD4_END,
    THREAD3_START,
    THREAD3_END,

    THREAD_CONTEXT_COUNT
} DebugThreads;

typedef enum DebugRam {
    PP_RAM_RED,
    PP_RAM_GREEN,
    PP_RAM_BLUE,
    PP_RAM_YELLOW,
    PP_RAM_MAGENTA,
    PP_RAM_CYAN,
    PP_RAM_WHITE,
    PP_RAM_GREY,
    PP_RAM_GREY_XLU,
    PP_RAM_ORANGE,
    PP_RAM_BLACK,
    PP_RAM_LIGHT_ORANGE,
    PP_RAM_LIME,

    PP_RAM_CODE,
    PP_RAM_FRAMEBUFFERS,
    PP_RAM_TASKBUFFER,
    PP_RAM_ANIMATIONS,
    PP_RAM_WEATHER,
    PP_RAM_LEVELTEX,
    PP_RAM_LEVELMDL,
    PP_RAM_OBJTEX,
    PP_RAM_OBJMDL,
    PP_RAM_MISCTEX,
    PP_RAM_MISCMDL,
    PP_RAM_SPRITES,
    PP_RAM_SLOTS,
    PP_RAM_CMDBUF,
    PP_RAM_SAVES,
    PP_RAM_TEMP,
    PP_RAM_SHADOWS,
    PP_RAM_SUBPOOLS,
    PP_RAM_AUDIOHEAP,
    PP_RAM_ASSETTABLE,
    PP_RAM_STACK,
    PP_RAM_AUD_EMITTERS,
    PP_RAM_AUD_TABLE,
    PP_RAM_SEQUENCES,
    PP_RAM_SOUNDBANK,
    PP_RAM_HUD,
    PP_RAM_FONTS,
    PP_RAM_TEXT,
    PP_RAM_LIGHTS,
    PP_RAM_ASSET_CACHE,
    PP_RAM_OBJECTS,
    PP_RAM_CPAK,
    PP_RAM_WAVES,
    PP_RAM_GHOSTS,
    PP_RAM_MENU,
    PP_RAM_TRANSITIONS,
    PP_RAM_UNKNOWN,
    PP_RAM_OBJLISTS,
    PP_RAM_OBJHEADERS,
    PP_RAM_MODELINSTANCE,
    PP_RAM_DEBUG,
    PP_RAM_AUDIOLINE,
    PP_RAM_PARTICLES,
    PP_RAM_MISCASSET,
    PP_RAM_SFXPROPTABLE,
    PP_RAM_TRIGTABLE,
    PP_RAM_SPRITE_TEX,
    PP_RAM_ALFX,
    PP_RAM_VOID,
    PP_RAM_COLLISION,
    PP_RAM_NORMALS,
    PP_RAM_TEMPOBJLIST,
    PP_RAM_OBJMAPS,

    PP_RAM_TOTAL,
} DebugRam;


extern u8 *main_BSS_START[];

#define MEMSTRINGS \
    "Red\t", \
    "Green\t", \
    "Blue\t", \
    "Yellow\t", \
    "Magenta\t", \
    "Cyan\t", \
    "White\t", \
    "Grey\t", \
    "GreyXLU\t", \
    "Orange\t", \
    "Black\t", \
    "L. Orange", \
    "Lime\t", \
            \
    "Code\t", \
    "Framebuffers", \
    "Task Buffer", \
    "Animations", \
    "Weather\t", \
    "Lvl Textures", \
    "Lvl Models", \
    "Obj Textures", \
    "Obj Models", \
    "Misc Textures", \
    "Misc Models", \
    "Sprites\t", \
    "Slots\t", \
    "Gfx Buffer", \
    "Saves\t", \
    "Temp\t", \
    "Shadows\t", \
    "Subpools\t", \
    "Audio Heap", \
    "Asset Tables", \
    "Stack\t", \
    "SFX Emitters", \
    "Aud Table", \
    "Sequences", \
    "Sound Banks", \
    "HUD\t", \
    "Fonts\t", \
    "Text\t", \
    "Lights\t", \
    "Asset Cache", \
    "Objects\t", \
    "Cpak\t", \
    "Waves\t", \
    "Ghost Data", \
    "Menu\t", \
    "Transitions", \
    "Unknown\t", \
    "Obj Lists", \
    "Obj Headers", \
    "Model Inst", \
    "Debug\t", \
    "Aud Lines", \
    "Particles", \
    "Misc Assets", \
    "SFX Params", \
    "Trig Tables", \
    "Sprite Tex", \
    "AudLib FX", \
    "Void\t", \
    "Collision", \
    "Normals\t", \
    "Temp ObjList", \
    "Obj Maps\t"

#define NUM_PERF_ITERATIONS 60
#define PERF_AGGREGATE NUM_PERF_ITERATIONS
#define PERF_TOTAL (NUM_PERF_ITERATIONS + 1)
#define NUM_THREAD_ITERATIONS 12
#define IQUE_DIVISOR 0.6510416667f

typedef u32 DebugTimer[NUM_PERF_ITERATIONS + 2];

typedef struct DebugMiscVars {
    unsigned drawBG : 1;
    unsigned invertBG : 1;

    u16 texLoads;
} DebugMiscVars;

typedef struct DebugLoadVars {
    u8 active;
    f32 dma;
    f32 decompress;
    f32 objectSpawns;
    f32 trackBuild;
    f32 malloc;
    f32 unknown;
    f32 total;
} DebugLoadVars;

typedef struct DebugData {
    u8 enabled;
    u8 pageCurrent;
    u8 pagePrev;
    u8 pageSelected;
    u8 pageViewMode;
    u8 pageMenuOpen;
    u8 pauseGame;
    u8 iter;
    u8 prevIter;
    u8 rspGfxIter;
    u8 rspAudIter;
    u8 threadIter[THREAD_CONTEXT_COUNT];
    u8 threadReset[THREAD_CONTEXT_COUNT];

    s16 pageScroll;
    s16 pageScrollMax;

    u32 cpuTotal;
    u32 rspTotal;
    u32 rdpTotal;
    u32 fpsGraph[NUM_PERF_ITERATIONS];
    u32 rspTimers[RSP_CONTEXT_COUNT][4];
    u32 threadTimers[THREAD_CONTEXT_COUNT][NUM_THREAD_ITERATIONS];
    DebugTimer timers[PP_TOTAL];
    u32 ramSegments[PP_RAM_TOTAL];
    u32 ramTotal;

    DebugMiscVars misc;
    DebugLoadVars loading;
} DebugData;

typedef struct DebugPage {
    char name[16];
    u8 index;
    void (*updateFunc)(struct DebugData *d);
    void (*renderFunc)(struct DebugData *d, Gfx **dList, s32 updateRate);
} DebugPage;

extern DebugData *gDebug;
extern char *sPuppyprintMemColours[];
extern f32 gFPS;

void debug_init();
s32 memsize_get(void);
void debug_log(s32 logLevel, char *str, ...);
void debug_render(Gfx **dList, s32 updateRate);
void debug_update(s32 updateRate);
void debug_rsp(s32 context);
void debug_rdp(void);
void debug_thread(s32 field, s32 offset);
void debug_newframe(s32 updateRate);
void debug_ram(s32 size, s32 tag);
s32 debug_tag_index(s32 colourTag);
void debug_printf(const char* message, ...);
void crash_assert(s32 cond, const char *str, ...);
void debug_ram_dump(void);
void debug_fillrect(Gfx **gfx, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour);
void debug_dump_hex(u8 *var, s32 size, s32 lineWidth);
char *assettable_name(s32 assetType, s32 assetID);

#define DEBUG_VAR(x, value) \
    { if (gDebug) {(x = value);}}

#endif
