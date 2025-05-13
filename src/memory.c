#include "memory.h"
#include "printf.h"
#include "thread0_epc.h"
#include "math_util.h"
#include "main.h"

/************ .bss ************/

#define MEMALIGN 0x8

#ifndef _ALIGN16
#define _ALIGN16(a) (((u32) (a) & ~0xF) + 0x10)
#endif
#ifndef _ALIGN8
#define ALIGNCHECK (MEMALIGN - 1)
#define _ALIGN8(a) (((u32) (a) & ~ALIGNCHECK) + MEMALIGN)
#endif

MemoryPool gMemoryPools[POOL_COUNT]; // Only two are used.
s32 gNumberOfMemoryPools;
void *gFreeQueueAddr[FREE_QUEUE_SIZE];
u8 gFreeQueueTicks[FREE_QUEUE_SIZE];
s32 gFreeQueueCount;
s32 gFreeQueueTimer; // Official Name: mmDelay

/******************************/

/**
 * Creates the main memory pool.
 * Starts at 0x8012D3F0. Ends at 0x80400000. Contains 1600 allocation slots.
 * Official Name: mmInit
 */
void mempool_init_main(void) {
    s32 ramEnd;

    gNumberOfMemoryPools = -1;
    if (gUseExpansionMemory) {
        ramEnd = EXPANSION_RAM_END;
    } else {
        ramEnd = RAM_END;
    }
    mempool_init(&gMainMemoryPool, ramEnd - (s32) (&gMainMemoryPool), MAIN_POOL_SLOT_COUNT);
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
    UNUSED s32 unused_2;
    u32 intFlags = interrupts_disable();
    MemoryPoolSlot *newPool;

    slots = (MemoryPoolSlot *) mempool_alloc_safe(numSlots * sizeof(MemoryPoolSlot), PP_RAM_SLOTS);
    // Good thing we're not using Rust :)
    slots[numSlots].data = (u8 *) mempool_alloc_safe(poolDataSize, PP_RAM_SUBPOOLS);
    newPool = mempool_init(slots, poolDataSize + (numSlots * sizeof(MemoryPoolSlot)), numSlots);
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
    if ((s32) slots & ALIGNCHECK) {
        firstSlot->data = (u8 *) _ALIGN8(slots);
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
    void *addr;
    addr = mempool_slot_find(POOL_MAIN, size, colourTag, 0);
    return addr;
}

/**
 * Reserves and returns memory from the main memory pool. Has no assert checks.
 */
MemoryPoolSlot *mempool_alloc(s32 size, u32 colourTag) {
    return mempool_slot_find(POOL_MAIN, size, colourTag, 0);
}

/**
 * Find the largest existing slot and then allocate it.
*/
MemoryPoolSlot *mempool_alloc_largest(u32 colourTag) {
    return mempool_slot_find(POOL_MAIN, 0x10, colourTag, 1);
}

/**
 * Resize the memory block while preserving the contents.
 * Can only go lower, not higher.
*/
void mempool_realloc(void *addr, s32 size, s32 colourTag) {
    mempool_free_timer(0);
    mempool_free(addr);
    addr = mempool_alloc_fixed(size, (u8 *) addr, colourTag, FALSE);
    mempool_free_timer(2);
}

u32 biggestSize = 0;

/**
 * Search the existing empty slots and try to find one that can meet the size requirement.
 * Afterwards, write the new allocation data to the slot in question and return the address.
 */
MemoryPoolSlot *mempool_slot_find(MemoryPools poolIndex, s32 size, u32 colourTag, s32 findLargest) {
    s32 slotSize;
    MemoryPoolSlot *curSlot;
    UNUSED s32 pad;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    u32 intFlags;
    s32 nextIndex;
    s32 currIndex;

    crash_assert(size == 0, "Alloc size 0");
    intFlags = interrupts_disable();
    pool = &gMemoryPools[poolIndex];
    if (pool->maxNumSlots == pool->curNumSlots + 1) {
        interrupts_enable(intFlags);
        gMemoryPools[poolIndex].curNumSlots++;
        crash_nomemory(0, COLOUR_TAG_NONE);
        return NULL;
    }
    currIndex = MEMSLOT_NONE;
    if (size & ALIGNCHECK) {
        size = _ALIGN8(size);
    }
    slots = pool->slots;
    nextIndex = 0;
    if (findLargest == FALSE) {
        slotSize = 0x7FFFFFFF;
        do {
            curSlot = &slots[nextIndex];
            if (curSlot->flags == SLOT_FREE) {
                if (curSlot->size >= size && curSlot->size < slotSize) {
                    slotSize = curSlot->size;
                    currIndex = nextIndex;
                }
            }
            nextIndex = curSlot->nextIndex;
        } while (nextIndex != MEMSLOT_NONE);
    } else {
        slotSize = 0;
        do {
            curSlot = &slots[nextIndex];
            if (curSlot->flags == SLOT_FREE) {
                if (curSlot->size > slotSize) {
                    slotSize = curSlot->size;
                    currIndex = nextIndex;
                }
            }
            nextIndex = curSlot->nextIndex;
        } while (nextIndex != MEMSLOT_NONE);
        size = slotSize;
        if (size & ALIGNCHECK) {
            size = _ALIGN8(size);
        }
    }
    if (currIndex != MEMSLOT_NONE) {
        mempool_slot_assign(poolIndex, (s32) currIndex, size, 1, 0, colourTag);
        interrupts_enable(intFlags);
        debug_ram(size, colourTag);
        return (MemoryPoolSlot *) (slots + currIndex)->data;
    }
    interrupts_enable(intFlags);
    crash_nomemory(size, colourTag);
    return NULL;
}

/**
 * Allocate memory from a specific pool.
 * Official name: mmAllocR
 */
void *mempool_alloc_pool(MemoryPoolSlot *slots, s32 size) {
    s32 i;
    return mempool_alloc(size, PP_RAM_OBJECTS);
    for (i = gNumberOfMemoryPools; i != 0; i--) {
        if (slots == gMemoryPools[i].slots) {
            return mempool_slot_find(i, size, PP_RAM_OBJECTS, 0);
        }
    }
    return (void *) NULL;
}

/**
 * Allocate memory from a specific pool.
 * Official name: mmAllocR
 */
void *mempool_alloc_pool_tag(MemoryPoolSlot *slots, s32 size, s32 colourTag) {
    s32 i;
    return mempool_alloc(size, colourTag);
    /*for (i = gNumberOfMemoryPools; i != 0; i--) {
        if (slots == gMemoryPools[i].slots) {
            return mempool_slot_find(i, size, colourTag, 0);
        }
    }*/
    return (void *) NULL;
}

/**
 * Allocates memory from the main pool at a fixed address.
 * Rearranges the memory slots to place one at that address if possible.
 * Official Name: mmAllocAtAddr
 */
void *mempool_alloc_fixed(s32 size, u8 *address, u32 colorTag, s32 markFixed) {
    s32 i;
    MemoryPoolSlot *curSlot;
    MemoryPoolSlot *slots;
    u32 intFlags;

    crash_assert(size == 0, "Alloc size 0");
    intFlags = interrupts_disable();
    if ((gMemoryPools[POOL_MAIN].curNumSlots + 1) == gMemoryPools[POOL_MAIN].maxNumSlots) {
        interrupts_enable(intFlags);
        crash_nomemory(0, COLOUR_TAG_NONE);
    } else {
        if (size & ALIGNCHECK) {
            size = _ALIGN8(size);
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
                        debug_ram(size, colorTag);
                        if (markFixed) {
                            curSlot->flags = SLOT_FIXED;
                        } else {
                            curSlot->flags = SLOT_USED;
                        }
                        return curSlot->data;
                    } else {
                        i = mempool_slot_assign(POOL_MAIN, i, (u32) address - (u32) curSlot->data, 0, 1, colorTag);
                        mempool_slot_assign(POOL_MAIN, i, size, 1, 0, colorTag);
                        interrupts_enable(intFlags);
                        debug_ram(size, colorTag);
                        if (markFixed) {
                            (slots + i)->flags = SLOT_FIXED;
                        } else {
                            (slots + i)->flags = SLOT_USED;
                        }
                        return (slots + i)->data;
                    }
                }
            }
        }
        interrupts_enable(intFlags);
    }
    crash_nomemory(size, colorTag);
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
            mempool_free_addr(gFreeQueueAddr[--gFreeQueueCount]);
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
        gFreeQueueTicks[i]--;
        if (gFreeQueueTicks[i] == 0) {
            mempool_free_addr(gFreeQueueAddr[i]);
            gFreeQueueAddr[i] = gFreeQueueAddr[gFreeQueueCount - 1];
            gFreeQueueTicks[i] = gFreeQueueTicks[gFreeQueueCount - 1];
            gFreeQueueCount--;
        } else {
            stubbed_printf("\n*** mm Error *** ---> Can't free ram at this location: %x\n", gFreeQueueAddr[i]);
            i++;
        }
    }

    interrupts_enable(intFlags);
}

/**
 * Searches the memory pools for a slot matching the given address.
 * If a slot is found, free it.
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
    gFreeQueueAddr[gFreeQueueCount] = dataAddress;
    gFreeQueueTicks[gFreeQueueCount] = gFreeQueueTimer;
    gFreeQueueCount++;

    if (gFreeQueueCount >= FREE_QUEUE_SIZE) {
        stubbed_printf("\n*** mm Error *** ---> stbf stack too deep!\n");
    }
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
    pool = pool; // Fakematch
    slot = &slots[slotIndex];
    nextIndex = slot->nextIndex;
    prevIndex = slot->prevIndex;
    slot = slot; // Fakematch
    nextSlot = &slots[nextIndex];
    prevSlot = &slots[prevIndex];
    slot->flags = SLOT_FREE;
    debug_ram(-slot->size, slot->colourTag);
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
    pool = pool; // Fakematch
    poolSlots[slotIndex].flags = slotIsTaken;
    poolSize = poolSlots[slotIndex].size;
    poolSlots[slotIndex].size = size;
#ifdef DEBUG
    poolSlots[slotIndex].colourTag = debug_tag_index(colourTag);
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