/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80070B30 */

#include "memory.h"
#include "printf.h"
#include "thread0_epc.h"
#include "controller.h"
#include "video.h"
#include "main.h"

/************ .bss ************/

#ifndef _ALIGN16
#define _ALIGN16(a) (((u32) (a) & ~0xF) + 0x10)
#endif

MemoryPool gMemoryPools[4]; // Only two are used.
s32 gNumberOfMemoryPools;
FreeQueueSlot gFreeQueue[256];
s32 gFreeQueueCount;
s32 gFreeQueueTimer; //Official Name: mmDelay

extern MemoryPoolSlot gMainMemoryPool;

/******************************/

/**
 * Creates the main memory pool. 
 * Starts at 0x8012D3F0. Ends at 0x80400000. Contains 1600 allocation slots.
 * Official Name: mmInit
 */
void init_main_memory_pool(void) {
    s32 ramEnd;

    gNumberOfMemoryPools = -1;
    if (gUseExpansionMemory) {
        ramEnd = EXPANSION_RAM_END;
    } else {
        ramEnd = RAM_END;
    }
    new_memory_pool(&gMainMemoryPool, ramEnd - (s32)(&gMainMemoryPool), MAIN_POOL_SLOT_COUNT);
    set_free_queue_state(2);
    gFreeQueueCount = 0;
}

/**
 * Creates a new memory pool that is sectioned off the main one.
 * Official name: mmAllocRegion
 */
MemoryPoolSlot *new_sub_memory_pool(s32 poolDataSize, s32 numSlots) {
    s32 size;
    MemoryPoolSlot *slots;
    s32 *flags = clear_status_register_flags();
    MemoryPoolSlot *newPool;

    size = poolDataSize + (numSlots * sizeof(MemoryPoolSlot));
    slots = (MemoryPoolSlot *) allocate_from_main_pool_safe(size, COLOUR_TAG_WHITE);
    newPool = new_memory_pool(slots, size, numSlots);
    set_status_register_flags(flags);
    return newPool;
}

/**
 * Create and initialise a memory pool in RAM that will act as the place where arbitrary allocations can go.
 * Will return the location of the first free slot in that pool.
*/
MemoryPoolSlot *new_memory_pool(MemoryPoolSlot *slots, s32 poolSize, s32 numSlots) {
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
    if ((s32) slots & 0xF) {
        firstSlot->data = (u8 *) _ALIGN16(slots);
    } else {
        firstSlot->data = (u8 *) slots;
    }
    firstSlot->size = firstSlotSize;
    firstSlot->flags = 0;
    firstSlot->prevIndex = -1;
    firstSlot->nextIndex = -1;
    gMemoryPools[poolCount].curNumSlots++;
    return gMemoryPools[poolCount].slots;
}

/**
 * Reserves and returns memory from the main memory pool. Has 2 assert checks.
 * Will cause an exception if the size is 0 or if memory cannot be reserved.
 */
void *allocate_from_main_pool_safe(s32 size, u32 colourTag) {
    return allocate_from_memory_pool(0, size, colourTag);
}

/**
 * Reserves and returns memory from the main memory pool. Has no assert checks.
 */
MemoryPoolSlot *allocate_from_main_pool(s32 size, u32 colourTag) {
    return allocate_from_memory_pool(0, size, colourTag);
}

/**
 * Search the existing empty slots and try to find one that can meet the size requirement.
 * Afterwards, write the new allocation data to the slot in question and return the address.
*/
MemoryPoolSlot *allocate_from_memory_pool(s32 poolIndex, s32 size, u32 colourTag) {
    s32 slotSize;
    MemoryPoolSlot *curSlot;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    s32 *flags;
    s32 nextIndex;
    s32 currIndex;
        
    flags = clear_status_register_flags();
    pool = &gMemoryPools[poolIndex];
    if ((pool->curNumSlots + 1) == (*pool).maxNumSlots) {
        set_status_register_flags(flags);
        puppyprint_assert("Out of slots");
        return NULL;
    }
    currIndex = -1;
    if (size & 0xF) {
        size = (size & ~0xF);
        size += 0x10;
    }
    slots = pool->slots;
    slotSize = 0x7FFFFFFF;
    nextIndex = 0;
    do {
        curSlot = &slots[nextIndex];
        if (curSlot->flags == 0) {
            if (curSlot->size >= size && curSlot->size < slotSize) {
                slotSize = curSlot->size;
                currIndex = nextIndex;
            }
        }
        nextIndex = curSlot->nextIndex;
    } while (nextIndex != -1);
    if (currIndex != -1) {
        allocate_memory_pool_slot(poolIndex, (s32) currIndex, size, 1, 0, colourTag);
        set_status_register_flags(flags);
        return (MemoryPoolSlot *) (slots + currIndex)->data;
    }
    set_status_register_flags(flags);
    
    puppyprint_assert("Out of memory");
    return NULL;
}

/* Official name: mmAllocR */
void *allocate_from_pool_containing_slots(MemoryPoolSlot *slots, s32 size) {
    s32 i;
    for (i = gNumberOfMemoryPools; i != 0; i--) {
        if (slots == gMemoryPools[i].slots) {
            return allocate_from_memory_pool(i, size, 0);
        }
    }
    return (void *)NULL;
}

/* Official Name: mmAllocAtAddr */
void *allocate_at_address_in_main_pool(s32 size, u8 *address, u32 colorTag) {
    s32 i;
    MemoryPoolSlot *curSlot;
    MemoryPoolSlot *slots;
    s32 *flags;
    
    flags = clear_status_register_flags();
    if ((gMemoryPools[0].curNumSlots + 1) == gMemoryPools[0].maxNumSlots) {
        set_status_register_flags(flags);
    } else {
        if (size & 0xF) {
            size = _ALIGN16(size);
        }
        slots = gMemoryPools[0].slots;
        for (i = 0; i != -1; i = curSlot->nextIndex) {
            curSlot = &slots[i];
            if (curSlot->flags == 0) {
                if ((u32) address >= (u32) curSlot->data && (u32)address + size <= (u32) curSlot->data + curSlot->size)  {
                    if (address == (u8 *) curSlot->data) {
                        allocate_memory_pool_slot(0, i, size, 1, 0, colorTag);
                        set_status_register_flags(flags);
                        return curSlot->data;
                    } else {
                        i = allocate_memory_pool_slot(0, i, (u32) address - (u32) curSlot->data, 0, 1, colorTag);
                        allocate_memory_pool_slot(0, i, size, 1, 0, colorTag);
                        set_status_register_flags(flags);
                        return (slots + i)->data;
                    }
                }
            }
        }
        set_status_register_flags(flags);
    }
    return NULL;
}

/**
 * Sets the state of the free queue. State is either 0, 1, or 2.
 * The free queue will get flushed if the state is set to 0.
 * Official name: mmSetDelay
 */
void set_free_queue_state(s32 state) {
    s32 *flags = clear_status_register_flags();
    gFreeQueueTimer = state;
    if (state == 0) { // flush free queue if state is 0.
        while (gFreeQueueCount > 0) {
            free_slot_containing_address(gFreeQueue[--gFreeQueueCount].dataAddress);
        }
    }
    set_status_register_flags(flags);
}

/**
 * Unallocates data from the pool that contains the data. Will free immediately if the free queue
 * state is set to 0, otherwise the data will just be marked for deletion.
 * Official Name: mmFree
 */
void free_from_memory_pool(void *data) {
    s32 *flags = clear_status_register_flags();
    if (gFreeQueueTimer == 0) {
        free_slot_containing_address(data);
    } else {
        add_to_free_queue(data);
    }
    set_status_register_flags(flags);
}

/**
 * Frees all the addresses in the free queue.
 * Official Name: mmFreeTick
 */
void clear_free_queue(void) {
    s32 i;
    s32 *flags;

    flags = clear_status_register_flags();

    for (i = 0; i < gFreeQueueCount;) {
        gFreeQueue[i].freeTimer--;
        if (gFreeQueue[i].freeTimer == 0) {
            free_slot_containing_address(gFreeQueue[i].dataAddress);
            gFreeQueue[i].dataAddress = gFreeQueue[gFreeQueueCount - 1].dataAddress;
            gFreeQueue[i].freeTimer = gFreeQueue[gFreeQueueCount - 1].freeTimer;
            gFreeQueueCount--;
        } else {
            i++;
        }
    }

    set_status_register_flags(flags);
}

/* Official name: heapFree */
void free_slot_containing_address(u8 *address) {
    s32 slotIndex;
    s32 poolIndex;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    MemoryPoolSlot *slot;
    
    poolIndex = get_memory_pool_index_containing_address(address);
    pool = gMemoryPools;
    slots = pool[poolIndex].slots;
    for (slotIndex = 0; slotIndex != -1; slotIndex = slot->nextIndex) {
        slot = &slots[slotIndex];
        
        if (address == (u8 *) slot->data) {
            if (slot->flags == 1 || slot->flags == 4) {
                free_memory_pool_slot(poolIndex, slotIndex);
            }
            break;
        }
        slot = &slots[slotIndex];
    }
}

/**
 * Adds the current memory address to the back of the queue, so it can be freed.
*/
void add_to_free_queue(void *dataAddress) {
    gFreeQueue[gFreeQueueCount].dataAddress = dataAddress;
    gFreeQueue[gFreeQueueCount].freeTimer = gFreeQueueTimer;
    gFreeQueueCount++;
}

s32 func_80071478(u8 *address) {
    s32 slotIndex;
    MemoryPoolSlot *slot;
    MemoryPool *pool;
    s32 *flags;

    flags = clear_status_register_flags();
    pool = &gMemoryPools[get_memory_pool_index_containing_address(address)];
    slotIndex = 0;
    while (slotIndex != -1) {
        slot = slotIndex + pool->slots; // `slot = &pool->slots[slotIndex];` does not match.
        if (address == (u8 *) slot->data) {
            if (slot->flags == 1 || slot->flags == 4) {
                slot->flags |= 2;
                set_status_register_flags(flags);
                return TRUE;
            }
        }
        slotIndex = slot->nextIndex;
    }
    set_status_register_flags(flags);
    return FALSE;
}

s32 func_80071538(u8 *address) {
    s32 slotIndex;
    MemoryPoolSlot *slot;
    MemoryPool *pool;
    s32 *flags;

    flags = clear_status_register_flags();
    pool = &gMemoryPools[get_memory_pool_index_containing_address(address)];
    slotIndex = 0;
    while (slotIndex != -1) {
        slot = slotIndex + pool->slots; // `slot = &pool->slots[slotIndex];` does not match.
        if (address == (u8 *)slot->data) {
            if (slot->flags & 2) {
                slot->flags ^= 2;
                set_status_register_flags(flags);
                return TRUE;
            }
        }
        slotIndex = slot->nextIndex;
    }
    set_status_register_flags(flags);
    return FALSE;
}

/**
 * Returns the index of the memory pool containing the memory address.
 */
s32 get_memory_pool_index_containing_address(u8 *address) {
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
void free_memory_pool_slot(s32 poolIndex, s32 slotIndex) {
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
    slot->flags = 0;
    if (nextIndex != -1) {
        if (nextSlot->flags == 0) {
            slot->size += nextSlot->size;
            tempNextIndex = nextSlot->nextIndex;
            slot->nextIndex = tempNextIndex;
            if (tempNextIndex != -1) {
                slots[tempNextIndex].prevIndex = slotIndex;
            }
            pool->curNumSlots--;
            slots[pool->curNumSlots].index = nextIndex;
        }
    }
    if (prevIndex != -1) {
        if (prevSlot->flags == 0) {
            prevSlot->size += slot->size;
            tempNextIndex = slot->nextIndex;
            prevSlot->nextIndex = tempNextIndex;
            if (tempNextIndex != -1) {
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
s32 allocate_memory_pool_slot(s32 poolIndex, s32 slotIndex, s32 size, s32 slotIsTaken, s32 newSlotIsTaken, u32 colourTag) {
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
        if (nextIndex != -1) {
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
    s32 remainder = (s32)address & 0xF;
    if (remainder > 0) {
        address = (u8 *)(((s32)address - remainder) + 16);
    }
    return address;
}

#ifdef PUPPYPRINT_DEBUG
void calculate_ram_total(s32 poolIndex, u32 colourTag) {
    s32 index;
    MemoryPoolSlot *slots;
    MemoryPoolSlot *curSlot;
    s32 i;

    switch (colourTag) {
    case COLOUR_TAG_RED:
        index = MEMP_TOTAL + 0;
        break;
    case COLOUR_TAG_BLACK:
        index = MEMP_TOTAL + 1;
        break;
    case COLOUR_TAG_BLUE:
        index = MEMP_TOTAL + 2;
        break;
    case COLOUR_TAG_CYAN:
        index = MEMP_TOTAL + 3;
        break;
    case COLOUR_TAG_GREEN:
        index = MEMP_TOTAL + 4;
        break;
    case COLOUR_TAG_GREY:
        index = MEMP_TOTAL + 5;
        break;
    case COLOUR_TAG_MAGENTA:
        index = MEMP_TOTAL + 6;
        break;
    case COLOUR_TAG_SEMITRANS_GREY:
        index = MEMP_TOTAL + 7;
        break;
    case COLOUR_TAG_WHITE:
        index = MEMP_TOTAL + 8;
        break;
    case COLOUR_TAG_YELLOW:
        index = MEMP_TOTAL + 9;
        break;
    case COLOUR_TAG_ORANGE:
        index = MEMP_TOTAL + 10;
        break;
    case COLOUR_TAG_BROWN:
        return;
    default:
        if (colourTag > MEMP_TOTAL || colourTag == 0) {
            return;
        }
        index = colourTag;
    }

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