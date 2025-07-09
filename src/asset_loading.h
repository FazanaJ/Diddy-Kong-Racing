#ifndef _ASSET_LOADING_H_
#define _ASSET_LOADING_H_

#include "types.h"

void pi_init(void);
u32 *asset_table_load(u32 assetIndex);
s32 asset_load(u32 assetIndex, u32 address, s32 assetOffset, s32 size);
u8 *asset_rom_offset(u32 assetIndex, u32 assetOffset);
s32 asset_table_size(u32 assetIndex);
void dmacopy(u32 romOffset, u32 ramAddress, s32 numBytes);
void dmacopy_v1(u32 romOffset, u32 ramAddress, s32 numBytes);
void assettable_seek_s32(s32 modelID, s32 *offset, s32 *size, s32 assetIndex);
void assettable_seek_s16(s32 modelID, s32 *offset, s32 *size, s32 assetIndex);
void assettable_tag(u32 tag);

#endif
