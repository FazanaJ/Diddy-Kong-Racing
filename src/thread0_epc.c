#include "thread0_epc.h"
#include "common.h"
#include "types.h"
#include "macros.h"
#include "menu.h"
#include "save_data.h"
#include "printf.h"
#include "objects.h"
#include "joypad.h"
#include "game.h"
#include "stacks.h"
#include "PR/os_internal_thread.h"

/**
 * Mark the object type given, so if the game crashes while processing it, the debug screen will tell you which object
 * ID is to blame. Split into three sections, for spawning an object, updating an object and for rendering an object.
 */
void update_object_stack_trace(s32 index, s32 value) {
    if (index >= OBJECT_SPAWN && index <= OBJECT_DRAW) {
        gObjectStackTrace[index] = value;
    }
}

#include "video.h"
#include "string.h"
#include "stdarg.h"
#include "audiomgr.h"

u64 gCrashThreadStack[0x200];
OSThread gCrashThread;
OSMesgQueue gCrashQueue;
OSMesg gCrashQueueBuf[2];
u16 *gCrashFB;
char *gCrashFuncName;
char gCrashAssert[127];
u8 gCrashAssetTripped;

u16 gScreenWidth = 320;
u16 gScreenHeight = 240;

const u8 gCrashScreenCharToGlyph[] = {
    41, -1, 45, -1, 43, -1, -1, 37, 38, -1, 42, -1, 39, 44, -1, 0,  1,  2,  3,
    4,  5,  6,  7,  8,  9,  36, -1, -1, -1, -1, 40, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
};

// Bit-compressed font. '#' = 1, '.' = 0
const u32 gCrashScreenFont[7 * 10] = {
    0x70871c30, // .###.. ..#... .###.. .###.. ..##.. ..
    0x8988a250, // #...#. .##... #...#. #...#. .#.#.. ..
    0x88808290, // #...#. ..#... ....#. ....#. #..#.. ..
    0x88831c90, // #...#. ..#... ..##.. .###.. #..#.. ..
    0x888402f8, // #...#. ..#... .#.... ....#. #####. ..
    0x88882210, // #...#. ..#... #..... #...#. ...#.. ..
    0x71cf9c10, // .###.. .###.. #####. .###.. ...#.. ..

    0xf9cf9c70, // #####. .###.. #####. .###.. .###.. ..
    0x8228a288, // #..... #...#. #...#. #...#. #...#. ..
    0xf200a288, // ####.. #..... ....#. #...#. #...#. ..
    0x0bc11c78, // ....#. ####.. ...#.. .###.. .####. ..
    0x0a222208, // ....#. #...#. ..#... #...#. ....#. ..
    0x8a222288, // #...#. #...#. ..#... #...#. #...#. ..
    0x71c21c70, // .###.. .###.. ..#... .###.. .###.. ..

    0x23c738f8, // ..#... ####.. .###.. ###... #####. ..
    0x5228a480, // .#.#.. #...#. #...#. #..#.. #..... ..
    0x8a282280, // #...#. #...#. #..... #...#. #..... ..
    0x8bc822f0, // #...#. ####.. #..... #...#. ####.. ..
    0xfa282280, // #####. #...#. #..... #...#. #..... ..
    0x8a28a480, // #...#. #...#. #...#. #..#.. #..... ..
    0x8bc738f8, // #...#. ####.. .###.. ###... #####. ..

    0xf9c89c08, // #####. .###.. #...#. .###.. ....#. ..
    0x82288808, // #..... #...#. #...#. ..#... ....#. ..
    0x82088808, // #..... #..... #...#. ..#... ....#. ..
    0xf2ef8808, // ####.. #.###. #####. ..#... ....#. ..
    0x82288888, // #..... #...#. #...#. ..#... #...#. ..
    0x82288888, // #..... #...#. #...#. ..#... #...#. ..
    0x81c89c70, // #..... .###.. #...#. .###.. .###.. ..

    0x8a08a270, // #...#. #..... #...#. #...#. .###.. ..
    0x920da288, // #..#.. #..... ##.##. #...#. #...#. ..
    0xa20ab288, // #.#... #..... #.#.#. ##..#. #...#. ..
    0xc20aaa88, // ##.... #..... #.#.#. #.#.#. #...#. ..
    0xa208a688, // #.#... #..... #...#. #..##. #...#. ..
    0x9208a288, // #..#.. #..... #...#. #...#. #...#. ..
    0x8be8a270, // #...#. #####. #...#. #...#. .###.. ..

    0xf1cf1cf8, // ####.. .###.. ####.. .###.. #####. ..
    0x8a28a220, // #...#. #...#. #...#. #...#. ..#... ..
    0x8a28a020, // #...#. #...#. #...#. #..... ..#... ..
    0xf22f1c20, // ####.. #...#. ####.. .###.. ..#... ..
    0x82aa0220, // #..... #.#.#. #.#... ....#. ..#... ..
    0x82492220, // #..... #..#.. #..#.. #...#. ..#... ..
    0x81a89c20, // #..... .##.#. #...#. .###.. ..#... ..

    0x8a28a288, // #...#. #...#. #...#. #...#. #...#. ..
    0x8a28a288, // #...#. #...#. #...#. #...#. #...#. ..
    0x8a289488, // #...#. #...#. #...#. .#.#.. #...#. ..
    0x8a2a8850, // #...#. #...#. #.#.#. ..#... .#.#.. ..
    0x894a9420, // #...#. .#.#.. #.#.#. .#.#.. ..#... ..
    0x894aa220, // #...#. .#.#.. #.#.#. #...#. ..#... ..
    0x70852220, // .###.. ..#... .#.#.. #...#. ..#... ..

    0xf8011000, // #####. ...... ...#.. .#.... ...... ..
    0x08020800, // ....#. ...... ..#... ..#... ...... ..
    0x10840400, // ...#.. ..#... .#.... ...#.. ...... ..
    0x20040470, // ..#... ...... .#.... ...#.. .###.. ..
    0x40840400, // .#.... ..#... .#.... ...#.. ...... ..
    0x80020800, // #..... ...... ..#... ..#... ...... ..
    0xf8011000, // #####. ...... ...#.. .#.... ...... ..

    0x70800000, // .###.. ..#... ...... ...... ...... ..
    0x88822200, // #...#. ..#... ..#... #...#. ...... ..
    0x08820400, // ....#. ..#... ..#... ...#.. ...... ..
    0x108f8800, // ...#.. ..#... #####. ..#... ...... ..
    0x20821000, // ..#... ..#... ..#... .#.... ...... ..
    0x00022200, // ...... ...... ..#... #...#. ...... ..
    0x20800020, // ..#... ..#... ...... ...... ..#... ..

    0x00000000,
    0x00000000,
    0x48000000,
    0x30000000,
    0x30000000,
    0x48000000,
    0x00000000,
};

char *gCauseDesc[] = {
    "Interrupt",
    "TLB modification",
    "TLB exception on load",
    "TLB exception on store",
    "Address error on load",
    "Address error on store",
    "Bus error on inst.",
    "Bus error on data",
    "System call exception",
    "Breakpoint exception",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap exception",
    "Virtual coherency on inst.",
    "Floating point exception",
    "Watchpoint exception",
    "Virtual coherency on data",
    "Stack overflow or underflow",
    "Asset tripped"
};

char *gFpcsrDesc[] = {
    "Unimplemented operation", "Invalid operation", "Division by zero", "Overflow", "Underflow",
    "Inexact operation",
};

char *write_to_buf(char *buffer, const char *data, size_t size) {
    char *ret = buffer;
    bcopy((char *) data, buffer, size);
    return ret + size;
}

typedef char *outfun(char*,const char*,size_t);
s32 _Printf(outfun prout, char *dst, const char *fmt, va_list args);

void crash_assert(s32 cond, const char *str, ...) {
    char *ptr;
    va_list args;
    s32 len;

    if (cond == FALSE) {
        return;
    }

    va_start(args, str);
    if ((_Printf(write_to_buf, gCrashAssert, str, args)) <= 0) {
        va_end(args);
        return;
    }
    va_end(args);
    gCrashAssetTripped = TRUE;
    *(volatile int *) 0 = 0;
}


void crash_screen_draw_glyph(s32 x, s32 y, s32 glyph, u16 colour) {
    const u32 *data;
    u16 *ptr;
    u32 bit;
    u32 rowMask;
    s32 i, j;

    data = &gCrashScreenFont[glyph / 5 * 7];
    ptr = gCrashFB + gScreenWidth * y + x;

    for (i = 0; i < 7; i++) {
        bit = 0x80000000U >> ((glyph % 5) * 6);
        rowMask = *data++;

        for (j = 0; j < 6; j++) {
            if ((bit & rowMask)) {
                *ptr = colour;
            }
            ptr++;
            bit >>= 1;
        }
        ptr += gScreenWidth - 6;
    }
}

void crash_text(s32 x, s32 y, u16 colour, const char *fmt, ...) {
    char *ptr;
    char buf[127];
    u32 glyph;
    s32 startX;
    va_list args;

    va_start(args, fmt);
    startX = x;
    if (_Printf(write_to_buf, buf, fmt, args) > 0) {
        ptr = buf;
        while (*ptr) {
            if (*ptr == '\n') {
                y += 8;
                x = startX;
            } else if (*ptr == ' ') {
                x += 6;
            } else {
                glyph = gCrashScreenCharToGlyph[(*ptr - '!') % (sizeof(gCrashScreenCharToGlyph))];
                if (glyph != 0xFF) {
                    crash_screen_draw_glyph(x, y, glyph, colour);
                }
                x += 6;
            }
            if (x > gScreenWidth || y > gScreenHeight) {
                break;
            }
            ptr++;
        }
    }
    va_end(args);
}

s32 crash_strwidth(const char *fmt, ...) {
    char *ptr;
    char buf[127];
    va_list args;
    s32 len;
    s32 lenRecord;

    va_start(args, fmt);
    len = 0;
    lenRecord = 0;
    if (_Printf(write_to_buf, buf, fmt, args) > 0) {
        ptr = buf;
        while (*ptr) {
            if (*ptr == '\n') {
                len = 0;
            } else {
                len += 6;
                if (len > lenRecord) {
                    lenRecord = len;
                }
            }
            ptr++;
        }
    }
    va_end(args);
    return lenRecord;
}

/**
 * Draw a rectangle on screen, supports arbitrary colour and alpha
*/
void crash_rectangle(s32 x, s32 y, s32 w, s32 h, s32 r, s32 g, s32 b, s32 a) {
    u16 *ptr;
    s32 i, j;
    u8 alpha;
    u16 dst;
    u8 dr, dg, db;
    u8 br, bg, bb;
    u16 blended;

    alpha = a;
    ptr = gCrashFB + gScreenWidth * y + x;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            dst = *ptr;

            dr = (dst >> 11) & 0x1F;
            dg = (dst >> 6) & 0x1F;
            db = (dst >> 1) & 0x1F;

            dr = (dr << 3) | (dr >> 2);
            dg = (dg << 3) | (dg >> 2);
            db = (db << 3) | (db >> 2);

            br = ((r * alpha) + (dr * (255 - alpha))) / 255;
            bg = ((g * alpha) + (dg * (255 - alpha))) / 255;
            bb = ((b * alpha) + (db * (255 - alpha))) / 255;

            blended = ((br & 0xF8) << 8) | ((bg & 0xF8) << 3) | ((bb & 0xF8) >> 2) | 1;

            *ptr++ = blended;
        }
        ptr += gScreenWidth - w;
    }
}

/**
 *  Draws a line using Bresenham's algorithm. Supports arbitrary colour and alpha
*/
void crash_line(s32 x0, s32 y0, s32 x1, s32 y1, s32 r, s32 g, s32 b, s32 a) {
    s32 dx, dy, sx, sy, err, e2;
    u16 *ptr;
    u16 dst;
    u8 dr, dg, db;
    u8 br, bg, bb;
    u16 blended;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = dx - dy;

    while (1) {
        // Draw the current pixel
        if (x0 >= 0 && x0 < gScreenWidth && y0 >= 0 && y0 < gScreenHeight) {
            ptr = gCrashFB + gScreenWidth * y0 + x0;
            dst = *ptr;

            // Extract RGB components from the destination color
            dr = (dst >> 11) & 0x1F;
            dg = (dst >> 6) & 0x1F;
            db = (dst >> 1) & 0x1F;

            // Convert 5-bit components to 8-bit
            dr = (dr << 3) | (dr >> 2);
            dg = (dg << 3) | (dg >> 2);
            db = (db << 3) | (db >> 2);

            // Blend the colors
            br = ((r * a) + (dr * (255 - a))) / 255;
            bg = ((g * a) + (dg * (255 - a))) / 255;
            bb = ((b * a) + (db * (255 - a))) / 255;

            // Convert back to 5-bit components and combine into a 16-bit color
            blended = ((br & 0xF8) << 8) | ((bg & 0xF8) << 3) | ((bb & 0xF8) >> 2) | 1;

            *ptr = blended;
        }

        // Check if we've reached the end of the line
        if (x0 == x1 && y0 == y1) {
            break;
        }

        // Update the error term and coordinates
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

char *sThreadNames[] = {
    "Unknown",
    "Main",
    "Audio",
    "Sched",
    "BGLoad",
    "USB"
};

s32 crash_thread_name(s32 threadID) {
    s32 id = threadID - 2;
    if (id == 28) {
        id = 4;
    } else if (id == 67) {
        id = 5;
    } else if (id >= 2) {
        id = 0;
    }

    return id;
}

/**
 * High chance the framebuffer size doesn't match the crash screens, so copy it with scaling applied.
*/
void framebuffer_scale(u16 *srcFB, u16 *dstFB, s32 srcW, s32 srcH, s32 dstW, s32 dstH) {
    s32 y;
    s32 x;
    for (y = 0; y < dstH; y++) {
        for (x = 0; x < dstW; x++) {
            s32 srcX = x * srcW / dstW;
            s32 srcY = y * srcH / dstH;
            u16 pixel = srcFB[srcY * srcW + srcX];
            dstFB[y * dstW + x] = pixel;
        }
    }
}

char *sGPRegisterNames[] = {
    "at", "v0", "v1", "v2", "a0", "a1", "a2",
    "a3", "t0", "t1", "t2", "t3", "t4", "t5",
    "t6", "t7", "s0", "s1", "s2", "s3", "s4",
    "s5", "s6", "s7", "t8", "t9",
};

extern u64 gThread1Stack[STACKSIZE(STACK_IDLE)];
extern u64 gThread3Stack[STACKSIZE(STACK_GAME)];
extern u64 audioStack[STACKSIZE(STACK_AUD)];
extern u64 gSchedStack[STACKSIZE(STACK_SCHED)];
extern u64 *gThread30Stack;
extern u64 gThreadUsbStack[STACKSIZE(STACK_USB)];

#define CRASH_BORDER_X 20

u32 crash_stack_pos(s32 threadID) {
    switch(threadID) {
        case 1:
            return (u32) &gThread1Stack[STACKSIZE(STACK_IDLE) - 1];
        case 3:
            return (u32) &gThread3Stack[STACKSIZE(STACK_GAME) - 1];
        case 4:
            return (u32) &audioStack[STACKSIZE(STACK_AUD) - 1];
        case 5:
            return (u32) &gSchedStack[STACKSIZE(STACK_SCHED) - 1];
        case 30:
            return (u32) &gThread30Stack[STACKSIZE(STACK_BGLOAD) - 1];
        case 69:
            return (u32) &gThreadUsbStack[STACKSIZE(STACK_USB) - 1];
        default:
            return 0;
    }
    return 0;
}

s32 crash_check_stack(void) {
    if ((gThread1Stack[STACKSIZE(STACK_IDLE) - 1] != gThread1Stack[0])) {
        return 1;
    }
    if ((gThread3Stack[STACKSIZE(STACK_GAME) - 1] != gThread3Stack[0])) {
        return 3;
    }
    if ((audioStack[STACKSIZE(STACK_AUD) - 1] != audioStack[0])) {
        return 4;
    }
    if ((gSchedStack[STACKSIZE(STACK_SCHED) - 1] != gSchedStack[0])) {
        return 5;
    }
    if (gThread30Stack && (gThread30Stack[STACKSIZE(STACK_BGLOAD) - 1] != gThread30Stack[0])) {
        return 30;
    }
    if ((gThreadUsbStack[STACKSIZE(STACK_USB) - 1] != gThreadUsbStack[0])) {
        return 69;
    }

    return 0;
}

extern OSThread gThread1;
extern OSThread gThread3;
extern OSSched gMainSched;
extern OSThread gThread30;

void crash_render(OSThread *t) {
    s32 i;
    s32 x;
    s32 y;
    s32 midPoint;
    s32 midCount;
    s32 initialX;
    u64 *reg;
    f64 *regF;
    s32 cause;
    s32 threadID;
    s32 stackSize;
    __OSThreadContext *c;
    
    crash_line(CRASH_BORDER_X - 1, 11, gScreenWidth - CRASH_BORDER_X, 11, 255, 255, 255, 160);
    crash_line(CRASH_BORDER_X - 1, gScreenHeight - 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 12, 255, 255, 255, 160);
    crash_line(CRASH_BORDER_X - 1, 12, CRASH_BORDER_X - 1, gScreenHeight - 13, 255, 255, 255, 160);
    crash_line(gScreenWidth - CRASH_BORDER_X, 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 13, 255, 255, 255, 160);
    crash_rectangle(CRASH_BORDER_X, 12, gScreenWidth - (CRASH_BORDER_X * 2), 33, 127, 0, 0, 160);
    
    crash_line(CRASH_BORDER_X, 45, gScreenWidth - CRASH_BORDER_X - 1, 45, 255, 255, 255, 160);
    
    crash_rectangle(CRASH_BORDER_X, 46, gScreenWidth - (CRASH_BORDER_X * 2), gScreenHeight - 58, 0, 0, 0, 160);

    // iykyk
    crash_rectangle(9, 4, 8, 6, 255, 0, 0, 255);
    crash_rectangle(10, 5, 6, 4, 255, 255, 255, 255);

    if (gCrashAssetTripped == FALSE) {
        cause = -1;
        switch(crash_check_stack()) {
            case 1:
                t = &gThread1;
                stackSize = STACK_IDLE;
                cause = 18;
                break;
            case 3:
                t = &gThread3;
                stackSize = STACK_GAME;
                cause = 18;
                break;
            case 4:
                t = audioGetThread();
                stackSize = STACK_AUD;
                cause = 18;
                break;
            case 5:
                t = &gMainSched.thread;
                stackSize = STACK_SCHED;
                cause = 18;
                break;
            case 30:
                t = &gThread30;
                stackSize = STACK_BGLOAD;
                cause = 18;
                break;
        }
    } else {
        cause = 19;
    }
    c = &t->context;
    if (cause == -1) {
        cause = (c->cause >> 2) & 0x1F;
    }
    crash_text(CRASH_BORDER_X + 16, 16, GPACK_RGBA5551(255, 255, 0, 1), "Thread:%s(%d)", sThreadNames[crash_thread_name(t->id)], t->id);
    crash_text(CRASH_BORDER_X + 16 + 144, 16, GPACK_RGBA5551(255, 255, 0, 1), "PC:0#%8X", (u32) c->pc);
    crash_text(CRASH_BORDER_X + 16 + 288, 16, GPACK_RGBA5551(255, 255, 0, 1), "RA:0#%8X", (u32) c->ra);
    crash_text(CRASH_BORDER_X + 16, 25, GPACK_RGBA5551(255, 255, 0, 1), "Cause:%s", gCauseDesc[cause]);
    if (gCrashFuncName) {
        crash_text(CRASH_BORDER_X + 16, 34, GPACK_RGBA5551(255, 255, 0, 1), "Func Name:%s", gCrashFuncName);
    }

    if (cause == 19) {
        crash_text(CRASH_BORDER_X + 8, 54, GPACK_RGBA5551(255, 255, 255, 1), gCrashAssert);
    } else if (cause == 18) {        
        if (t) {
            crash_text(CRASH_BORDER_X + 8, 54, GPACK_RGBA5551(255, 255, 255, 1), "Thread %d stack write out of bounds\nIncrease stack size in stacks.h", threadID);
            crash_text(CRASH_BORDER_X + 8, 80, GPACK_RGBA5551(255, 255, 255, 1), "Stack Pos: 0#%X", (u32) (crash_stack_pos(threadID) - t->context.sp));
            crash_text(CRASH_BORDER_X + 8, 89, GPACK_RGBA5551(255, 255, 255, 1), "Stack Size:0#%X", stackSize);
        }
    } else {
        x = CRASH_BORDER_X + 12;
        midPoint = 1;
        while (1) {
            if (x + 288 < gScreenWidth - CRASH_BORDER_X) {
                x += 144;
                midPoint++;
            } else {
                break;
            }
        }
        initialX = (gScreenWidth / 2) - ((144 * midPoint) / 2);
    
        crash_text(initialX, 54, GPACK_RGBA5551(255, 255, 255, 1), "GP:0#%08X", (u32) c->gp);
        crash_text(initialX + 144, 54, GPACK_RGBA5551(255, 255, 255, 1), "SP:0#%08X", (u32) c->sp);
        crash_text(initialX + 288, 54, GPACK_RGBA5551(255, 255, 255, 1), "Stack Pos:0#%X", (u32) (crash_stack_pos(t->id) - c->sp));
        crash_text(initialX, 63, GPACK_RGBA5551(255, 255, 255, 1), "VA:0#%08X", (u32) c->badvaddr);
        crash_text(initialX + 144, 63, GPACK_RGBA5551(255, 255, 255, 1), "SR:0#%08X", (u32) c->sr);
        x = initialX;
        y = 72;
        midCount = 0;
        if (cause == 15) {
            crash_text(initialX, y + 4, GPACK_RGBA5551(255, 255, 255, 1), "Cause:%s", gFpcsrDesc[c->fpcsr & 0x1F]);
            y += 16;
            regF = (f64 *) &c->fp0;
            for (i = 0; i < 32; i += 1) {
                crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "FPR%02d:%2.4f", i, (f64) regF[i]);
                midCount++;
                if (midCount >= 3) {
                    midCount = 0;
                    y += 9;
                    x = initialX;
                } else {
                    x += 144;
                }
            }

        } else {
            reg = (u64 *) c;
            for (i = 0; i < 26; i++) {
                u32 val = reg[i];
                if (val >= 0x80000000 && val < 0x80800000) {
                    crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X (addr?)", sGPRegisterNames[i], (u32) val);
                } else if (val > 0x10000000 && val < 0xFFFFFFFF - 0x10000000) {
                    crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X", sGPRegisterNames[i], (u32) val);
                } else {
                    crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X (%d)", sGPRegisterNames[i], (u32) val, (s32) reg[i]);
                }
                midCount++;
                if (midCount >= 3) {
                    midCount = 0;
                    y += 9;
                    x = initialX;
                } else {
                    x += 144;
                }
            }
        }
    }
    osWritebackDCacheAll();
    osViBlack(FALSE);
    osViSwapBuffer(gCrashFB);
}

void crash_screen_sleep(s32 ms) {
    u32 cycles = ms * 1000 * osClockRate / 1000000;
    osSetTime(0);
    while (osGetTime() < cycles) {}
}

extern OSThread *__osFaultedThread;

OSThread *crash_error_thread(void) {
    OSThread *thread;

    thread = __osFaultedThread;
    while (thread->priority != -1) {
        if (thread->priority > OS_PRIORITY_IDLE && thread->priority < OS_PRIORITY_APPMAX && (thread->flags & 3) != 0) {
            return thread;
        }
        thread = thread->tlnext;
    }
    return NULL;
}

void crash_thread(UNUSED void *var) {
    OSMesg msg;
    s32 oldW;
    s32 oldH;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashQueue, (OSMesg) 2);
    osSetEventMesg(OS_EVENT_FAULT, &gCrashQueue, (OSMesg) 8);

    gCrashAssetTripped = FALSE;

    osRecvMesg(&gCrashQueue, &msg, OS_MESG_BLOCK);
    osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
    if (gVideoDepthBuffer == NULL) {
        gCrashFB = (u16 *) 0x803000000;
    } else {
        gCrashFB = gVideoDepthBuffer;
    }
    crash_screen_sleep(500);
    oldW = gScreenWidth;
    oldH = gScreenHeight;
    gScreenWidth = 512;
    gScreenHeight = 240;
    vi_change(gScreenWidth, gScreenHeight);
    if (gVideoCurrFramebuffer == NULL) {
        gVideoCurrFramebuffer = (u16 *) 0x802000000;
    }
    framebuffer_scale(gVideoCurrFramebuffer, gCrashFB, oldW, oldH, gScreenWidth, gScreenHeight);

    crash_render(crash_error_thread());
    while (1) { 
        msg = 0;
        osRecvMesg(&gCrashQueue, &msg, OS_MESG_NOBLOCK);
        if ((s32) msg == 64) {
            vi_change(oldW, oldH);
            osViSwapBuffer(gCrashFB);
            break;
        }
    }

    while (1) {}
}

void crash_init(void) {
    osCreateMesgQueue(&gCrashQueue, gCrashQueueBuf, ARRAY_COUNT(gCrashQueueBuf));
    osCreateThread(&gCrashThread, 1, &crash_thread, 0, &gCrashThreadStack[0x200], 30);
    osStartThread(&gCrashThread);
}