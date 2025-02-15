/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80070B30 */

#include "memory.h"
#include "printf.h"
#include "thread0_epc.h"
#include "controller.h"
#include "video.h"
#include "main.h"
#include "math_util.h"

/************ .bss ************/

#ifndef _ALIGN16
#define _ALIGN16(a) (((u32) (a) & ~0x7) + 0x8)
#endif


#ifndef _ALIGN64
#define _ALIGN64(a) (((u32) (a) & ~0x3F) + 0x40)
#endif

MemoryPool gMemoryPools[POOL_COUNT]; // Only two are used.
s32 gNumberOfMemoryPools;
void *gFreeQueue[FREE_QUEUE_SIZE];
u8 gFreeQueueElementTimer[FREE_QUEUE_SIZE];
s32 gFreeQueueCount;
s32 gFreeQueueTimer; // Official Name: mmDelay

extern MemoryPoolSlot gMainMemoryPool;

/******************************/

/**
 * Creates the main memory pool.
 * Starts at 0x8012D3F0. Ends at 0x80400000. Contains 1600 allocation slots.
 * Official Name: mmInit
 */
void mempool_init_main(void) {
    u32 ramEnd;
    s32 i;

    gNumberOfMemoryPools = -1;
    if (gUseExpansionMemory) {
        if (__osBbIsBb) {
            ramEnd = 0xFFFFFFFF;
            for (i = 0; i < 4; i++) {
                if (__osBbPakAddress[i] != 0 && __osBbPakAddress[i] < ramEnd) {
                    ramEnd = __osBbPakAddress[i];
                }
            }
            if (__osBbEepromAddress != 0 && __osBbEepromAddress < ramEnd) {
                ramEnd = __osBbEepromAddress;
            }
            if (__osBbSramAddress != 0 && __osBbSramAddress < ramEnd) {
                ramEnd = __osBbSramAddress;
            }
            if (__osBbFlashAddress != 0 && __osBbFlashAddress < ramEnd) {
                 ramEnd = __osBbFlashAddress;
            }
            if (ramEnd > 0x807FC000) {
                ramEnd = 0x807FC000;
            }
        } else {
            ramEnd = K0BASE + osGetMemSize();
        }
    } else {
        ramEnd = RAM_END;
    }
    ramEnd -= (s32) (&gMainMemoryPool);
    mempool_init(&gMainMemoryPool, ramEnd, MAIN_POOL_SLOT_COUNT);
    mempool_free_timer(2);
    gFreeQueueCount = 0;
}

/**
 * Creates a new memory pool that's contained inside another one.
 * Official name: mmAllocRegion
 */
MemoryPoolSlot *mempool_new_sub(s32 poolDataSize, s32 numSlots) {
    s32 size;
    MemoryPoolSlot *slots;
    u32 intFlags = interrupts_disable();
    MemoryPoolSlot *newPool;

    size = poolDataSize + (numSlots * sizeof(MemoryPoolSlot));
    slots = (MemoryPoolSlot *) mempool_alloc_safe(size, MEMP_SUBPOOL);
    newPool = mempool_init(slots, size, numSlots);
    interrupts_enable(intFlags);
    return newPool;
}

/**
 * Create and initialise a memory pool in RAM that will act as the place where arbitrary allocations can go.
 * Will return the location of the first free slot in that pool.
 */
MemoryPoolSlot *mempool_init(MemoryPoolSlot *slots, s32 poolSize, s32 numSlots) {
    MemoryPoolSlot *firstSlot;
    s32 poolCount;
    s32 i;
    s32 firstSlotSize;

    poolCount = ++gNumberOfMemoryPools;
    firstSlotSize = poolSize - (numSlots * sizeof(MemoryPoolSlot));
    gMemoryPools[poolCount].maxNumSlots = numSlots;
    gMemoryPools[poolCount].curNumSlots = 0;
    gMemoryPools[poolCount].slots = slots;
    gMemoryPools[poolCount].size = poolSize;
    firstSlot = slots;
    for (i = 0; i < gMemoryPools[poolCount].maxNumSlots; i++) {
        firstSlot->index = i;
        firstSlot++;
    }
    firstSlot = &gMemoryPools[poolCount].slots[0];
    slots += numSlots;
    if ((s32) slots & 0x7) {
        firstSlot->data = (u8 *) _ALIGN16(slots);
    } else {
        firstSlot->data = (u8 *) slots;
    }
    firstSlot->size = firstSlotSize;
    firstSlot->flags = SLOT_FREE;
    firstSlot->prevIndex = MEMSLOT_NONE;
    firstSlot->nextIndex = MEMSLOT_NONE;
    gMemoryPools[poolCount].curNumSlots++;
    return gMemoryPools[poolCount].slots;
}

/**
 * Reserves and returns memory from the main memory pool. Has 2 assert checks.
 * Will cause an exception if the size is 0 or if memory cannot be reserved,
 * dumping the function stack contents onto a controller pak for debugging.
 */
void *mempool_alloc_safe(s32 size, u32 colourTag) {
    return mempool_slot_find(0, size, colourTag);
}

/**
 * Reserves and returns memory from the main memory pool. Has no assert checks.
 */
MemoryPoolSlot *mempool_alloc(s32 size, u32 colourTag) {
    return mempool_slot_find(POOL_MAIN, size, colourTag);
}

/**
 * Search the existing empty slots and try to find one that can meet the size requirement.
 * Afterwards, write the new allocation data to the slot in question and return the address.
 */
MemoryPoolSlot *mempool_slot_find(MemoryPools poolIndex, s32 size, u32 colourTag) {
    s32 slotSize;
    MemoryPoolSlot *curSlot;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    u32 intFlags;
    s32 nextIndex;
    s32 currIndex;
    profiler_begin_timer();

    intFlags = interrupts_disable();
    pool = &gMemoryPools[poolIndex];
    if ((pool->curNumSlots + 1) == (*pool).maxNumSlots) {
        interrupts_enable(intFlags);
        puppyprint_assert("Out of slots (%X)", colourTag);
        puppyprint_load_snapshot(PP_LOAD_MALLOC, profiler_get_timer());
        return NULL;
    }
    currIndex = MEMSLOT_NONE;
    if (size & 0x7) {
        size = _ALIGN16(size);
    }
    slots = pool->slots;
    slotSize = 0x7FFFFFFF;
    nextIndex = 0;
    do {
        curSlot = &slots[nextIndex];
        if (curSlot->flags == SLOT_FREE) {
            if (curSlot->size >= size && curSlot->size < slotSize) {
                slotSize = curSlot->size;
                currIndex = nextIndex;
                break;
            }
        }
        nextIndex = curSlot->nextIndex;
    } while (nextIndex != MEMSLOT_NONE);
    if (currIndex != MEMSLOT_NONE) {
        mempool_slot_assign(poolIndex, (s32) currIndex, size, 1, 0, colourTag);
        interrupts_enable(intFlags);
        puppyprint_load_snapshot(PP_LOAD_MALLOC, profiler_get_timer());
        return (MemoryPoolSlot *) (slots + currIndex)->data;
    }
    interrupts_enable(intFlags);

    puppyprint_assert("Out of memory (%X %s)", colourTag, sPuppyprintMemColours[colourTag]);
    
    puppyprint_load_snapshot(PP_LOAD_MALLOC, profiler_get_timer());
    return NULL;
}

/**
 * Allocate memory from a specific pool.
 * Official name: mmAllocR
 */
void *mempool_alloc_pool(MemoryPoolSlot *slots, s32 size) {
    s32 i;
    for (i = gNumberOfMemoryPools; i != 0; i--) {
        if (slots == gMemoryPools[i].slots) {
            return mempool_slot_find(i, size, MEMP_OBJECTS);
        }
    }
    return (void *) NULL;
}

// Temporary while the above is used by assembly funcs
void *mempool_alloc_audio(MemoryPoolSlot *slots, s32 size, s32 colourTag) {
    s32 i;
    for (i = gNumberOfMemoryPools; i != 0; i--) {
        if (slots == gMemoryPools[i].slots) {
            return mempool_slot_find(i, size, colourTag);
        }
    }
    return (void *) NULL;
}

/**
 * Allocates memory from the main pool at a fixed address.
 * Rearranges the memory slots to place one at that address if possible.
 * Official Name: mmAllocAtAddr
 */
void *mempool_alloc_fixed(s32 size, u8 *address, u32 colorTag) {
    s32 i;
    MemoryPoolSlot *curSlot;
    MemoryPoolSlot *slots;
    u32 intFlags;

    intFlags = interrupts_disable();
    if ((gMemoryPools[POOL_MAIN].curNumSlots + 1) == gMemoryPools[POOL_MAIN].maxNumSlots) {
        interrupts_enable(intFlags);
    } else {
        if (size & 0x7) {
            size = _ALIGN16(size);
        }
        slots = gMemoryPools[POOL_MAIN].slots;
        for (i = 0; i != MEMSLOT_NONE; i = curSlot->nextIndex) {
            curSlot = &slots[i];
            if (curSlot->flags == SLOT_FREE) {
                if ((u32) address >= (u32) curSlot->data &&
                    (u32) address + size <= (u32) curSlot->data + curSlot->size) {
                    if (address == (u8 *) curSlot->data) {
                        mempool_slot_assign(POOL_MAIN, i, size, 1, 0, colorTag);
                        interrupts_enable(intFlags);
                        return curSlot->data;
                    } else {
                        i = mempool_slot_assign(POOL_MAIN, i, (u32) address - (u32) curSlot->data, 0, 1, colorTag);
                        mempool_slot_assign(POOL_MAIN, i, size, 1, 0, colorTag);
                        interrupts_enable(intFlags);
                        return (slots + i)->data;
                    }
                }
            }
        }
        interrupts_enable(intFlags);
    }
    return NULL;
}

/**
 * Sets the tick timer for the free queue.
 * If it's set to 0, then it clears the existing queue.
 * Nonzero amounts set any future frees to wait that many ticks
 * before clearing from memory.
 */
void mempool_free_timer(s32 state) {
    u32 intFlags = interrupts_disable();
    gFreeQueueTimer = state;
    if (state == 0) { // flush free queue if state is 0.
        while (gFreeQueueCount > 0) {
            mempool_free_addr(gFreeQueue[--gFreeQueueCount]);
        }
    }
    interrupts_enable(intFlags);
}

/**
 * Unallocates data from the pool that contains the data. Will free immediately if the free queue
 * state is set to 0, otherwise the data will just be marked for deletion.
 * Official Name: mmFree
 */
void mempool_free(void *data) {
    u32 intFlags = interrupts_disable();
    if (gFreeQueueTimer == 0) {
        mempool_free_addr(data);
    } else {
        mempool_free_queue(data);
    }
    interrupts_enable(intFlags);
}

/**
 * Frees all the addresses in the free queue.
 * Official Name: mmFreeTick
 */
void mempool_free_queue_clear(void) {
    s32 i;
    u32 intFlags;

    intFlags = interrupts_disable();

    for (i = 0; i < gFreeQueueCount;) {
        gFreeQueueElementTimer[i]--;
        if (gFreeQueueElementTimer[i] == 0) {
            mempool_free_addr(gFreeQueue[i]);
            gFreeQueue[i] = gFreeQueue[gFreeQueueCount - 1];
            gFreeQueueElementTimer[i] = gFreeQueueElementTimer[gFreeQueueCount - 1];
            gFreeQueueCount--;
        } else {
            i++;
        }
    }

    interrupts_enable(intFlags);
}

/**
 * Searches the memory pools for a slot matching the given address.
 * If a slot is found, free it.
 * Official name: heapFree
 */
void mempool_free_addr(u8 *address) {
    s32 slotIndex;
    s32 poolIndex;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    MemoryPoolSlot *slot;

    poolIndex = mempool_get_pool(address);
    pool = gMemoryPools;
    slots = pool[poolIndex].slots;
    for (slotIndex = 0; slotIndex != MEMSLOT_NONE; slotIndex = slot->nextIndex) {
        slot = &slots[slotIndex];

        if (address == (u8 *) slot->data) {
            if (slot->flags) {
                mempool_slot_clear(poolIndex, slotIndex);
            }
            break;
        }
        slot = &slots[slotIndex];
    }
    stubbed_printf("\n*** mm Error *** ---> No match found for mmFree.\n");
}

/**
 * Adds the current memory address to the back of the queue, so it can be freed.
 */
void mempool_free_queue(void *dataAddress) {
    gFreeQueue[gFreeQueueCount] = dataAddress;
    gFreeQueueElementTimer[gFreeQueueCount] = gFreeQueueTimer;
    gFreeQueueCount++;
}

/**
 * Returns the index of the memory pool containing the memory address.
 */
s32 mempool_get_pool(u8 *address) {
    s32 i;
    MemoryPool *pool;

    for (i = gNumberOfMemoryPools; i > 0; i--) {
        pool = &gMemoryPools[i];
        if ((u8 *) pool->slots >= address) {
            continue;
        }
        if (address < pool->size + (u8 *) pool->slots) {
            break;
        }
    }
    return i;
}

/**
 * Clears the current slot of all information, effectively freeing the allocated memory.
 * Unused slots before and after will be merged with this slot
 */
void mempool_slot_clear(MemoryPools poolIndex, s32 slotIndex) {
    s32 nextIndex;
    s32 prevIndex;
    s32 tempNextIndex;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    MemoryPoolSlot *slot;
    MemoryPoolSlot *nextSlot;
    MemoryPoolSlot *prevSlot;

    pool = &gMemoryPools[poolIndex];
    slots = pool->slots;
    slot = &slots[slotIndex];
    nextIndex = slot->nextIndex;
    prevIndex = slot->prevIndex;
    nextSlot = &slots[nextIndex];
    prevSlot = &slots[prevIndex];
    slot->flags = SLOT_FREE;
    if (nextIndex != MEMSLOT_NONE) {
        if (nextSlot->flags == SLOT_FREE) {
            slot->size += nextSlot->size;
            tempNextIndex = nextSlot->nextIndex;
            slot->nextIndex = tempNextIndex;
            if (tempNextIndex != MEMSLOT_NONE) {
                slots[tempNextIndex].prevIndex = slotIndex;
            }
            pool->curNumSlots--;
            slots[pool->curNumSlots].index = nextIndex;
        }
    }
    if (prevIndex != MEMSLOT_NONE) {
        if (prevSlot->flags == SLOT_FREE) {
            prevSlot->size += slot->size;
            tempNextIndex = slot->nextIndex;
            prevSlot->nextIndex = tempNextIndex;
            if (tempNextIndex != MEMSLOT_NONE) {
                slots[tempNextIndex].prevIndex = prevIndex;
            }
            pool->curNumSlots--;
            slots[pool->curNumSlots].index = slotIndex;
        }
    }
    calculate_ram_total(poolIndex, slot->colourTag);
}

/**
 * Initialise and attempts to fit the new memory block in the slot given.
 * Updates the linked list with any entries before and after then returns the new slot index.
 * If the region cannot fit, return the old slot instead.
 */
s32 mempool_slot_assign(MemoryPools poolIndex, s32 slotIndex, s32 size, s32 slotIsTaken, s32 newSlotIsTaken,
                        u32 colourTag) {
    MemoryPool *pool;
    MemoryPoolSlot *poolSlots;
    s32 index;
    s32 nextIndex;
    s32 poolSize;

    pool = &gMemoryPools[poolIndex];
    poolSlots = pool->slots;
    poolSlots[slotIndex].flags = slotIsTaken;
    poolSize = poolSlots[slotIndex].size;
    poolSlots[slotIndex].size = size;
#ifdef PUPPYPRINT_DEBUG
    poolSlots[slotIndex].colourTag = colourTag;
#endif
    index = poolSlots[pool->curNumSlots].index;
    if (size < poolSize) {
        index = (pool->curNumSlots + poolSlots)->index;
        pool->curNumSlots++;
        poolSlots[index].data = &poolSlots[slotIndex].data[size];
        poolSlots[index].size = poolSize;
        poolSlots[index].size -= size;
        poolSlots[index].flags = newSlotIsTaken;
        poolSize = poolSlots[slotIndex].nextIndex;
        nextIndex = poolSize;
        poolSlots[index].prevIndex = slotIndex;
        poolSlots[index].nextIndex = nextIndex;
        poolSlots[slotIndex].nextIndex = index;
        if (nextIndex != MEMSLOT_NONE) {
            poolSlots[nextIndex].prevIndex = index;
        }
        calculate_ram_total(poolIndex, colourTag);
        return index;
    }
    return slotIndex;
}

/**
 * Returns the passed in address aligned to the next 16-byte boundary.
 * Official name: mmAlign16
 */
u8 *align16(u8 *address) {
    s32 remainder = (s32) address & 0xF;
    if (remainder > 0) {
        address = (u8 *) (((s32) address - remainder) + 16);
    }
    return address;
}

#ifdef PUPPYPRINT_DEBUG
s32 puppyprint_colourtag(s32 colourTag) {
switch (colourTag) {
    case COLOUR_TAG_RED:
        return MEMP_TOTAL + 0;
    case COLOUR_TAG_BLACK:
        return MEMP_TOTAL + 1;
    case COLOUR_TAG_BLUE:
        return MEMP_TOTAL + 2;
    case COLOUR_TAG_CYAN:
        return MEMP_TOTAL + 3;
    case COLOUR_TAG_GREEN:
        return MEMP_TOTAL + 4;
    case COLOUR_TAG_GREY:
        return MEMP_TOTAL + 5;
    case COLOUR_TAG_MAGENTA:
        return MEMP_TOTAL + 6;
    case COLOUR_TAG_SEMITRANS_GREY:
        return MEMP_TOTAL + 7;
    case COLOUR_TAG_WHITE:
        return MEMP_TOTAL + 8;
    case COLOUR_TAG_YELLOW:
        return MEMP_TOTAL + 9;
    case COLOUR_TAG_ORANGE:
        return MEMP_TOTAL + 10;
    case COLOUR_TAG_SEMITRANS_GREEN:
        return MEMP_TOTAL + 11;
    default:
        if (colourTag > MEMP_TOTAL || colourTag == 0) {
            return 0;
        }
        return colourTag;
    }
}

void calculate_ram_total(s32 poolIndex, u32 colourTag) {
    s32 index;
    MemoryPoolSlot *slots;
    MemoryPoolSlot *curSlot;
    s32 i;

    if (colourTag == MEMP_SUBPOOL) {
        gPuppyPrint.ramPools[MEMP_SUBPOOL] = 0;
        return;
    }

    index = puppyprint_colourtag(colourTag);

    slots = gMemoryPools[poolIndex].slots;
    gPuppyPrint.ramPools[MEMP_OVERALL] -= gPuppyPrint.ramPools[index];
    gPuppyPrint.ramPools[index] = 0;

    for (i = 0; i != -1; i = curSlot->nextIndex) {
        curSlot = &slots[i];
        if (curSlot->flags != 0 && curSlot->colourTag == colourTag) {
            gPuppyPrint.ramPools[index] += curSlot->size;
            gPuppyPrint.ramPools[MEMP_OVERALL] += curSlot->size;
        }
    }
}
#endif
