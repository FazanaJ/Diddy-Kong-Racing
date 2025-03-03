#ifndef _RCP_DKR_H_
#define _RCP_DKR_H_

#include "types.h"
#include "structs.h"
#include "f3ddkr.h"
#include "sched.h"
#include <ultra64.h>

#include "game_ui.h"
#include "video.h"

#define FIFO_BUFFER_SIZE 0x18000

enum TextureRectangleFlags {
    TEXRECT_BILERP,
    TEXRECT_POINT = (1 << 0),
    TEXRECT_FLIP_X = (1 << 12),
    TEXRECT_FLIP_Y = (1 << 13),
};

typedef union {
    void (*function)(Gfx*, Matrix *);
    void *ptr;
} BackgroundFunction;

typedef struct DKR_OSTask {
    struct DKR_OSTask *next;
    u32 state;
    u32 flags;
    void *frameBuffer;
    OSTask_t task; // Size: 0x40 bytes
    OSMesgQueue *mesgQueue;
    OSMesg mesg;
} DKR_OSTask;

extern u64 *gGfxSPTaskOutputBuffer;

// Defined in dkr.ld
extern s8 rspF3DDKRDramStart[];
extern s8 rspF3DDKRBootStart[];
extern s8 rspF3DDKRXbusStart[];
extern s8 rspF3DDKRDataXbusStart[];
extern s8 rspF3DDKRFifoStart[];
extern s8 rspF3DDKRDataFifoStart[];

void gfxtask_wait(void);
void func_80077AAC(void *bufPtr, s32 arg1, UNUSED s32 arg2);
void bgdraw_primcolour(u8 red, u8 green, u8 blue);
void bgdraw_fillcolour(s32 red, s32 green, s32 blue);
void rdp_init(Gfx **dList);
void rsp_init(Gfx **dList);
void bgdraw_texture_init(TextureHeader *tex1, TextureHeader *tex2, u32 shiftX);
void gfxtask_run_xbus(Gfx *dlBegin, Gfx *dlEnd);
void gfxtask_run_fifo(Gfx *dlBegin, Gfx *dlEnd);
void texrect_draw(Gfx **dList, DrawTexture *element, s32 xPos, s32 yPos, u8 red, u8 green, u8 blue,
                               u8 alpha);
void bgdraw_chequer(Gfx** dList);
void bgdraw_render(Gfx **dList, Matrix *mtx, s32 drawBG);
void bgdraw_set_func(void *func);
void texrect_draw_scaled(Gfx **dlist, DrawTexture *element, f32 xPos, f32 yPos, f32 xScale, f32 yScale, u32 colour, s32 flags);

//Non Matching
void func_80078190(Gfx **dlist);

#endif
