#include "asset_loading.h"

#include "common.h"
#include "macros.h"
#include "ultra64.h"
#include "main.h"

/************ .bss ************/

#undef VERSION
#define VERSION VERSION_80

OSIoMesg gAssetsDmaIoMesg;
OSMesg gDmaMesg;
OSMesgQueue gDmaMesgQueue;
OSMesg gPIMesgBuf[16];
OSMesgQueue gPIMesgQueue;
#if VERSION >= VERSION_79
OSMesg gAssetsLookupTableMesgBuf;
OSMesgQueue gDmaMesgQueueV2;
#define dmacopy_internal dmacopy_v1
#else
#define dmacopy_internal dmacopy
#endif
u32 gAssetTableCount;
u32 gAssetTableTag;

/*******************************/

// These are both defined in the generated dkr.ld file.
extern u8 __ASSETS_LUT_START[], __ASSETS_LUT_END[];

/**
 * Set up the peripheral interface message queues and scheduling.
 * This will send messages when DMA reads are finished.
 * After, allocate space and load the asset table into RAM.
 * Official Name: piInit
 */
void init_PI_mesg_queue(void) {
    u32 assetTableSize;
    u32 *table;

    gAssetTableTag = COLOUR_TAG_GREY;
    osCreateMesgQueue(&gPIMesgQueue, gPIMesgBuf, ARRAY_COUNT(gPIMesgBuf));
    osCreateMesgQueue(&gDmaMesgQueue, &gDmaMesg, 1);
    osCreatePiManager((OSPri) 150, &gPIMesgQueue, gPIMesgBuf, ARRAY_COUNT(gPIMesgBuf));

#if VERSION >= VERSION_79
    osCreateMesgQueue(&gDmaMesgQueueV2, &gAssetsLookupTableMesgBuf, 1);
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
#endif

    assetTableSize = __ASSETS_LUT_END - __ASSETS_LUT_START;
    table = (u32 *) mempool_alloc_safe(assetTableSize, PP_RAM_ASSETTABLE);
    dmacopy_internal((u32) __ASSETS_LUT_START, (u32) table, (s32) assetTableSize);
    gAssetTableCount = table[0];
    mempool_free(table);
}

void assettable_seek(s32 assetID, u32 *table0, s32 *table1) {
    s32 table[2];
    dmacopy_internal((u32) __ASSETS_LUT_START + (assetID * sizeof(s32)), (u32) &table, (s32) 2 * sizeof(s32));
    *table0 = table[0];
    *table1 = table[1] - table[0];
}

void assettable_tag(u32 tag) {
    gAssetTableTag = tag;
}

/**
 * Returns the memory address containing an asset section loaded from ROM.
 * Official Name: piRomLoad
 */
u32 *load_asset_section_from_rom(u32 assetIndex) {
    u32 *index;
    u32 *out;
    s32 size;
    u32 start;

#if VERSION >= VERSION_79
    OSMesg msg = NULL;
    osRecvMesg(&gDmaMesgQueueV2, &msg, OS_MESG_BLOCK);
#endif

    if (gAssetTableCount < assetIndex) {
        return 0;
    }
    assetIndex++;
    assettable_seek(assetIndex, &start, &size);
    out = (u32 *) mempool_alloc_safe(size, gAssetTableTag);
    if (out == 0) {
        return 0;
    }

    dmacopy_internal((u32) (start + __ASSETS_LUT_END), (u32) out, size);

#if VERSION >= VERSION_79
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
#endif

    return out;
}

/**
 * Loads part of an asset section to a specific memory address.
 * Returns the size argument.
 * Official name: piRomLoadSection
 */
s32 load_asset_to_address(u32 assetIndex, u32 address, s32 assetOffset, s32 size) {
    u32 *index;
    u32 start;
    s32 pad;

#if VERSION >= VERSION_79
    OSMesg msg = NULL;
    osRecvMesg(&gDmaMesgQueueV2, &msg, OS_MESG_BLOCK);
#endif

    if (size == 0 || gAssetTableCount < assetIndex) {
        return 0;
    }

    assetIndex++;
    assettable_seek(assetIndex, &start, &pad);
    start += assetOffset;

    dmacopy_internal((u32) (start + __ASSETS_LUT_END), address, size);

#if VERSION >= VERSION_79
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
#endif

    return size;
}

/**
 * Returns a rom offset of an asset given its asset section and a local offset.
 * Official name: piRomGetSectionPtr
 */
u8 *get_rom_offset_of_asset(u32 assetIndex, u32 assetOffset) {
    u32 *index;
    u32 start;
    s32 pad;

#if VERSION >= VERSION_79
    OSMesg msg = NULL;
    osRecvMesg(&gDmaMesgQueueV2, &msg, OS_MESG_BLOCK);
#endif

    if (gAssetTableCount < assetIndex) {
        return NULL;
    }

    assetIndex++;
    assettable_seek(assetIndex, &start, &pad);
    start += assetOffset;

#if VERSION >= VERSION_79
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
#endif

    return start + __ASSETS_LUT_END;
}

/**
 * Returns the size of an asset section.
 * Official name: piRomGetFileSize
 */
s32 get_size_of_asset_section(u32 assetIndex) {
    u32 *index;
    u32 pad;
    s32 size;

#if VERSION >= VERSION_79
    OSMesg msg = NULL;
    osRecvMesg(&gDmaMesgQueueV2, &msg, OS_MESG_BLOCK);
#endif

    if (gAssetTableCount < assetIndex) {
        return 0;
    }

    assetIndex++;
    assettable_seek(assetIndex, &pad, &size);

#if VERSION >= VERSION_79
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
#endif

    return size;
}

#define MAX_TRANSFER_SIZE 0x5000

/**
 * Copies data from the game cartridge to a ram address.
 * Official name: romCopy
 */
void dmacopy(u32 romOffset, u32 ramAddress, s32 numBytes) {
#if VERSION >= VERSION_79
    OSMesg msg = NULL;
    osRecvMesg(&gDmaMesgQueueV2, &msg, OS_MESG_BLOCK);
    dmacopy_internal(romOffset, ramAddress, numBytes);
    osSendMesg(&gDmaMesgQueueV2, (OSMesg) 1, OS_MESG_NOBLOCK);
}

// Looks like v2 ROMs made an alternate version of this function, and this is the original.
void dmacopy_internal(u32 romOffset, u32 ramAddress, s32 numBytes) {
#endif
    OSMesg dmaMesg;
    s32 numBytesToDMA;

    osInvalDCache((u32 *) ramAddress, numBytes);
    numBytesToDMA = MAX_TRANSFER_SIZE;
    while (numBytes > 0) {
        if (numBytes < numBytesToDMA) {
            numBytesToDMA = numBytes;
        }
        osPiStartDma(&gAssetsDmaIoMesg, OS_MESG_PRI_NORMAL, OS_READ, romOffset, (u32 *) ramAddress, numBytesToDMA,
                     &gDmaMesgQueue);
        osRecvMesg(&gDmaMesgQueue, &dmaMesg, OS_MESG_BLOCK);
        numBytes -= numBytesToDMA;
        romOffset += numBytesToDMA;
        ramAddress += numBytesToDMA;
    }
}

void assettable_seek_s32(s32 modelID, s32 *offset, s32 *size, s32 assetIndex) {
    s32 table[2];
    load_asset_to_address(assetIndex, (u32) &table, modelID * sizeof(s32), 2 * (sizeof(s32)));
    *offset = table[0];
    *size = table[1] - table[0];
}

void assettable_seek_s16(s32 modelID, s32 *offset, s32 *size, s32 assetIndex) {
    s16 table[4];
    load_asset_to_address(assetIndex, (u32) &table, modelID * (sizeof(s16)), 2 * (sizeof(s16)));
    *offset = table[0];
    *size = table[1];
}