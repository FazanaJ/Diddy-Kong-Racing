#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "structs.h"
#include "types.h"
#include "macros.h"
#include "config.h"

typedef enum MemoryPools {
    POOL_MAIN,
    POOL_OBJECT,
    POOL_AUDIO,
    POOL_TEMP,

    POOL_COUNT
} MemoryPools;

typedef enum MempoolFlags {
    SLOT_FREE = 0,              // The slot is free.
    SLOT_USED = (1 << 0),       // The slot is used.
    SLOT_LOCKED = (1 << 1),     // The slot is used, and cannot be freed by normal means.
    SLOT_SAFEGUARD = (1 << 2),  // The slot is used, and marks the stopping point of a global pool clear.
} MempoolFlags;

#define RAM_END 0x80400000
#define EXPANSION_RAM_END 0x80800000
#define MAIN_POOL_SLOT_COUNT 1200
#define OBJ_POOL_SLOT_COUNT 512
#define AUD_POOL_SLOT_COUNT 300
#define FREE_QUEUE_SIZE 256
#define MEMSLOT_NONE -1

// Animation related?
#define COLOUR_TAG_RED 0xFF0000FF
// Model headers
#define COLOUR_TAG_GREEN 0x00FF00FF
// Tracks
#define COLOUR_TAG_YELLOW 0xFFFF00FF
// Textures
#define COLOUR_TAG_MAGENTA 0xFF00FFFF
// Audio
#define COLOUR_TAG_CYAN 0x00FFFFFF
// Buffers and heaps
#define COLOUR_TAG_WHITE 0xFFFFFFFF
// Assets
#define COLOUR_TAG_GREY 0x7F7F7FFF
// Particles
#define COLOUR_TAG_SEMITRANS_GREY 0x80808080
// Model data
#define COLOUR_TAG_ORANGE 0xFF7F7FFF
// Controller Pak
#define COLOUR_TAG_BLACK 0x000000FF
// Weather
#define COLOUR_TAG_LIGHT_ORANGE 0xFFAA55FF
// ???
#define COLOUR_TAG_SEMITRANS_GREEN 0x00FF0163
// Blue Baby
#define COLOUR_TAG_BLUE 0x0000FFFF

enum MemoryTags {
    MEMP_OVERALL,
    MEMP_CODE,
    MEMP_ANIMATION,
    MEMP_MODELS,
    MEMP_AUDIO,
    MEMP_OBJECT_MODELS,
    MEMP_LEVEL_MODELS,
    MEMP_OBJECT_TEXTURES,
    MEMP_LEVEL_TEXTURES,
    MEMP_MISC_TEXTURES,
    MEMP_GHOST_DATA,
    MEMP_OBJECTS,
    MEMP_HEADERS,
    MEMP_FRAMEBUFFERS,
    MEMP_TASKBUFFER,
    MEMP_SHADOWS,
    MEMP_MISC,
    MEMP_GFXBUFFERS,
    MEMP_PARTICLES,
    MEMP_MENU,
    MEMP_LIGHTS,
    MEMP_HUD,
    MEMP_TEXT,
    MEMP_AUDIO_POOL,
    MEMP_WEATHER,
    MEMP_AUDIO_BANK,
    MEMP_SEQUENCE,
    MEMP_MISCASSET,
    MEMP_TEMP,
    MEMP_SAVES,
    MEMP_SUBPOOL,
    MEMP_POOLSLOTS,
    MEMP_WAVES,
    MEMP_RED,
    MEMP_BLACK,
    MEMP_BLUE,
    MEMP_CYAN,
    MEMP_GREEN,
    MEMP_GREY,
    MEMP_MAGENTA,
    MEMP_GREYXLU,
    MEMP_WHITE,
    MEMP_YELLOW,
    MEMP_ORANGE,
    MEMP_GREENXLU,

    MEMP_TOTAL
};
    
/* Size: 0x14 bytes */
typedef struct MemoryPoolSlot {
/* 0x00 */ u8 *data; 
/* 0x04 */ s32 size;
/* 0x08 */ u8 flags;
    // 0x00 = Slot is free 
    // 0x01 = Slot is being used?
/* 0x09 */ u8 colourTag;
/* 0x0A */ s16 prevIndex;
/* 0x0C */ s16 nextIndex;
/* 0x0E */ s16 index;
} MemoryPoolSlot;

/* Size: 0x10 bytes */
typedef struct MemoryPool {
/* 0x00 */ s32 maxNumSlots;
/* 0x04 */ s32 curNumSlots;
/* 0x08 */ MemoryPoolSlot *slots;
           void *slotData;
/* 0x0C */ s32 size;
} MemoryPool;

/* Unknown size */
typedef struct StackInfo {
    u32 var[5];
    u32 sp;
} StackInfo;

extern MemoryPool gMemoryPools[POOL_COUNT];
extern s32 gNumberOfMemoryPools;
extern u32 gRamEnd;
// This variable doesn't truly exist in memory.
// It's just defined as the end of BSS, and it's 
// symbol needs to be in the undefined syms place.
extern MemoryPoolSlot gMainMemoryPool;

void mempool_init_main(void);
MemoryPoolSlot *mempool_new_sub(s32 poolDataSize, s32 numSlots);
void *mempool_alloc_safe(s32 size, u32 colourTag);
MemoryPoolSlot *mempool_alloc(s32 size, u32 colourTag);
void *mempool_alloc_pool(MemoryPoolSlot *slots, s32 size);
void mempool_free_timer(s32 state);
void mempool_free(void *data);
void mempool_free_queue_clear(void);
void mempool_free_queue(void *dataAddress);
s32 mempool_get_pool(u8 *address);
s32 get_memory_colour_tag_count(u32 colourTag);
void *mempool_alloc_fixed(s32 size, u8 *address, u32 colorTag);
u8 *align16(u8 *address);
s32 puppyprint_colourtag(s32 colourTag);
void *mempool_alloc_audio(MemoryPoolSlot *slots, s32 size, s32 colourTag);
void calculate_pool_remainder(void);
void mempool_free_sub(MemoryPoolSlot *pool);

#endif
