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
#include "PRinternal/osint.h"
#include "math_util.h"
#include "main.h"
#include "usb/usb.h"
#include "video.h"
#include "string.h"
#include "stdarg.h"
#include "audiomgr.h"
#include "autoplay.h"

#define MAP_PARSE

u16 gObjectStackTrace[3];

/**
 * Mark the object type given, so if the game crashes while processing it, the debug screen will tell you which object
 * ID is to blame. Split into three sections, for spawning an object, updating an object and for rendering an object.
 */
void update_object_stack_trace(s32 index, s32 value) {
    if (index >= OBJECT_SPAWN && index <= OBJECT_DRAW) {
        gObjectStackTrace[index] = value;
    }
}


u64 *gCrashThreadStack;
u64 *gCrashThreadStack2;
OSThread gCrashThread;
OSThread *gCrashThread2;
OSMesgQueue gCrashQueue;
OSMesgQueue gCrashQueue2;
OSMesg gCrashQueueBuf[2];
OSMesg gCrashQueueBuf2[2];
MapSymbol *gMapSymbols;
u16 *gCrashFB;
char gCrashFuncName[32];
u8 gCrashFuncFound;
char gCrashAssert[127];
s32 gCrashInput;
u8 gCrashAssetTripped;
u8 gCrashPage;
u8 gCrashCause = -1;
u8 gCrashFBFlip;
u8 gCrashFBUpdate;
s16 gCrashSelection;
s8 gCrashScrollCursor;
s16 gCrashAltView;
s16 gCrashScroll;
s16 gMaxScroll;
s16 gScrollSize;
s16 gCrashAltSelection;
s16 gCrashAltScrollCursor;
s16 gCrashAltScroll;
s16 gMaxAltScroll;
s16 gAltScrollSize;
s16 gAltNumValids;
s8 gMemoryCapID = -1;
u16 gThreadStackSize;
u32 gMemoryCapOffset;


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

u32 gCrashFont[] = {
    0x746318c5, 0xc4610842, 0x39d10991, 0x0fba2170,
    0x0062e32a, 0x52f885f8, 0x78218b9d, 0x187a3177,
    0xe2111084, 0x7462e8c5, 0xce8c5e18, 0xb88a8c7f,
    0x18fa31f4, 0x63e74610, 0x845dc946, 0x31973f08,
    0x7a10ffe1, 0x0f421074, 0x6178c5d1, 0x8c7f18c5,
    0xc4210847, 0x04210c62, 0xe8ca98a4, 0xa3084210,
    0x87e3bad6, 0x318c639a, 0xce317463, 0x18c5de8c,
    0x7d0841d1, 0x8c6b26fa, 0x31f52517, 0x460e0c5d,
    0xf2108421, 0x2318c631, 0x746318a9, 0x448c635a,
    0xd5518a88, 0xa8c6318a, 0x88427c22, 0x2221f000,
    0x80200022, 0x21082088, 0x20842220, 0x00000001,
    0xc0001d10, 0x88802108, 0x42100401, 0x09f21011,
    0x11111110, 0x45445440, 0x11108884, 0x40000000
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
    "Asset tripped",
    "Out of memory"
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

void crash_nomemory(s32 size, s32 colourTag) {
    if (colourTag != COLOUR_TAG_NONE) {
        gMemoryCapID = colourTag;
        gMemoryCapOffset = size;
        debug_ram(size, colourTag);
    }
    gCrashCause = 20;
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
        debug_printf("%s\n", buf);
        while (*ptr) {
            if (*ptr == '\n') {
                y += 8;
                x = startX;
            } else if (*ptr == ' ') {
                x += 6;
            } else if (*ptr != '\t') {
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
    "Idle",
    "Crash",
    "Main",
    "Audio",
    "Sched",
    "BGLoad",
    "USB",
};

s32 crash_thread_name(s32 threadID) {
    s32 id = threadID;
    if (id == 30) {
        id = 6;
    } else if (id == 69) {
        id = 7;
    } else if (id > 5) {
        id = 0;
    }

    return id;
}

/**
 * High chance the framebuffer size doesn't match the crash screens, so copy it with scaling applied.
*/
void framebuffer_scale_16b(u16 *srcFB, u16 *dstFB, s32 srcW, s32 srcH, s32 dstW, s32 dstH) {
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

void framebuffer_scale_32b(u32 *srcFB, u16 *dstFB, s32 srcW, s32 srcH, s32 dstW, s32 dstH) {
    s32 y;
    s32 x;
    for (y = 0; y < dstH; y++) {
        for (x = 0; x < dstW; x++) {
            s32 srcX = x * srcW / dstW;
            s32 srcY = y * srcH / dstH;
            u32 pixel32 = srcFB[srcY * srcW + srcX];

            s32 r = (pixel32 >> 24) & 0xFF;
            s32 g = (pixel32 >> 16) & 0xFF;
            s32 b = (pixel32 >> 8) & 0xFF;

            u16 pixel16 = GPACK_RGBA5551(r, g, b, 1);

            dstFB[y * dstW + x] = pixel16;
        }
    }
}

char *sGPRegisterNames[] = {
    "at", "v0", "v1", "v2", "a0", "a1", "a2",
    "a3", "t0", "t1", "t2", "t3", "t4", "t5",
    "t6", "t7", "s0", "s1", "s2", "s3", "s4",
    "s5", "s6", "s7", "t8", "t9",
};

extern u64 *gThread3Stack;
extern u64 *audioStack;
extern u64 *gSchedStack;
extern u64 *gThread30Stack;
extern u64 *gThreadUsbStack;

#define CRASH_BORDER_X 20

u32 crash_stack_pos(s32 threadID) {
    switch(threadID) {
        case 2:
            if (gCrashThreadStack) {
                return (u32) (gCrashThreadStack + (STACKSIZE(STACK_CRASH) - 1));
            } else {
                return 0;
            }
        case 3:
            if (gThread3Stack) {
                return (u32) (gThread3Stack + (STACKSIZE(STACK_GAME) - 1));
            } else {
                return 0;
            }
        case 4:
            if (audioStack) {
                return (u32) (audioStack + (STACKSIZE(STACK_AUD) - 1));
            } else {
                return 0;
            }
        case 5:
            if (gSchedStack) {
                return (u32) (gSchedStack + (STACKSIZE(STACK_SCHED) - 1));
            } else {
                return 0;
            }
        case 30:
            if (gThread30Stack) {
                return (u32) (gThread30Stack + (STACKSIZE(STACK_BGLOAD) - 1));
            } else {
                return 0;
            }
        case 69:
            if (gThreadUsbStack) {
                return (u32) (gThreadUsbStack + (STACKSIZE(STACK_USB) - 1));
            } else {
                return 0;
            }
        default:
            return 0;
    }
    return 0;
}

s32 crash_check_stack(void) {
    if (gThread3Stack && gThread3Stack[STACKSIZE(STACK_GAME) - 1] != gThread3Stack[0]) {
        return 3;
    }
    if (audioStack && (audioStack[STACKSIZE(STACK_AUD) - 1] != audioStack[0])) {
        return 4;
    }
    if (gSchedStack && (gSchedStack[STACKSIZE(STACK_SCHED) - 1] != gSchedStack[0])) {
        return 5;
    }
    if (gThread30Stack && (gThread30Stack[STACKSIZE(STACK_BGLOAD) - 1] != gThread30Stack[0])) {
        return 30;
    }
    if (gThreadUsbStack && (gThreadUsbStack[STACKSIZE(STACK_USB) - 1] != gThreadUsbStack[0])) {
        return 69;
    }

    return 0;
}

extern OSThread gThread1;
extern OSThread gThread3;
extern OSSched gMainSched;
extern OSThread *gThread30;
extern OSThread gThreadUsb;

void crash_reg_common(OSThread *t, s32 x) {
    __OSThreadContext *c;

    c = &t->context;
    crash_text(x, 54, GPACK_RGBA5551(255, 255, 255, 1), "GP:0#%08X", (u32) c->gp);
    crash_text(x + 144, 54, GPACK_RGBA5551(255, 255, 255, 1), "SP:0#%08X", (u32) c->sp);
    crash_text(x + 288, 54, GPACK_RGBA5551(255, 255, 255, 1), "Stack Pos:0#%X", (u32) (crash_stack_pos(t->id) - c->sp));
    crash_text(x, 63, GPACK_RGBA5551(255, 255, 255, 1), "VA:0#%08X", (u32) c->badvaddr);
    crash_text(x + 144, 63, GPACK_RGBA5551(255, 255, 255, 1), "SR:0#%08X", (u32) c->sr);
}

void crash_page_gpregs(OSThread *t) {
    __OSThreadContext *c;
    s32 initialX;
    s32 x;
    s32 y;
    s32 midPoint;
    s32 midCount;
    s32 i;
    u64 *reg;

    c = &t->context;
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

    crash_reg_common(t, initialX);

    x = initialX;
    y = 72;
    midCount = 0;
    reg = (u64 *) c;
    for (i = 0; i < 26; i++) {
        u32 val = reg[i];
        if (val >= 0x80000000 && val < 0x80800000) {
            crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X (addr?)", sGPRegisterNames[i], (u32) val);
        } else if (val > 0x10000000 && val < 0xFFFFFFFF - 0x10000000) {
            crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X", sGPRegisterNames[i], (u32) val);
        } else {
            crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "%s:0#%08X (%d)", sGPRegisterNames[i], (u32) val, (s32) val);
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

void crash_page_fpregs(OSThread *t) {
    __OSThreadContext *c;
    s32 initialX;
    s32 x;
    s32 y;
    s32 midPoint;
    s32 midCount;
    s32 i;
    f64 *reg;

    c = &t->context;
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

    crash_reg_common(t, initialX);

    x = initialX;
    y = 72;
    midCount = 0;
    if (gCrashCause == 15) {
        crash_text(initialX, y + 4, GPACK_RGBA5551(255, 255, 255, 1), "Cause:%s", gFpcsrDesc[c->fpcsr & 0x1F]);
    }
    y += 16;
    reg = (f64 *) &c->fp0;
    for (i = 0; i < 32; i += 1) {
        crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "FPR%02d:%2.4f", i, (f64) reg[i]);
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

u8 gStackThreadIDs[] = {
    2, 3, 4, 5, 30, 69
};

u32 crash_stack_size(s32 threadID) {
    switch(threadID) {
        case 1:
            return STACK_GAME;
        case 2:
            return STACK_CRASH;
        case 3:
            return STACK_GAME;
        case 4:
            return STACK_AUD;
        case 5:
            return STACK_SCHED;
        case 30:
            return STACK_BGLOAD;
        case 69:
            return STACK_USB;
        default:
            return 0;
    }
}

OSThread *crash_thread_id(s32 threadID) {
    switch(threadID) {
        case 1:
            return &gThread1;
        case 2:
            return &gCrashThread;
        case 3:
            return &gThread3;
        case 4:
            return audioGetThread();
        case 5:
            return &gMainSched.thread;
        case 30:
            if (gThread30) {
                return gThread30;
            } else {
                return NULL;
            }
        case 69:
            return &gThreadUsb;
        default:
            return NULL;
    }
}

void crash_page_stacks(OSThread *t) {
    s32 i;
    u32 colour;
    s32 y;
    OSThread *stackT;
    s32 stackMin;
    s32 stackMax;

    y = 54;
    for (i = 0; i < (s32) sizeof(gStackThreadIDs); i++) {
        stackT = crash_thread_id(gStackThreadIDs[i]);
        if (stackT == NULL) {
            stackMin = 0;
        } else {
            if (stackT->context.sp < 0x80000000) {
                stackMin = 0;
            } else {
                stackMin = (u32) (crash_stack_pos(gStackThreadIDs[i]) - stackT->context.sp);
                if (stackMin > 0xFFFF) {
                    stackMin = 0xFFFF;
                }
            }
        }
        stackMax = (u32) crash_stack_size(gStackThreadIDs[i]);
        if (stackMin >= stackMax) {
            colour = GPACK_RGBA5551(255, 64, 64, 1);
        } else {
            colour = GPACK_RGBA5551(255, 255, 255, 1);
        }
        crash_text((gScreenWidth / 2) - 148, y, colour, "Thread:%s(%d)", sThreadNames[crash_thread_name(gStackThreadIDs[i])], gStackThreadIDs[i]);
        if (stackMin != 0) {
            crash_text((gScreenWidth / 2) - 40, y, colour, "Stack Pos:0#%04X - Size:0#%04X", stackMin, stackMax);
        } else {
            crash_text((gScreenWidth / 2) + 16, y, colour, "--Inactive--");
        }
        y += 9;
    }
}

void crash_page_assert(void) {
    if (gCrashAssetTripped) {
        crash_text(CRASH_BORDER_X + 16, 54, GPACK_RGBA5551(255, 255, 255, 1), gCrashAssert);
    } else {
        crash_text(CRASH_BORDER_X + 16, 54, GPACK_RGBA5551(255, 255, 255, 1), "No assert triggered.");
    }
}

extern char *sPuppyprintMemColours[];

const char *sMemLabels[] = {
    "B",
    "KB",
    "MB"
};

const u16 sMemColour5s[] = {
    // Original 12 colors
    GPACK_RGBA5551(255, 0, 0, 1),    // Bright red
    GPACK_RGBA5551(0, 255, 0, 1),    // Bright green
    GPACK_RGBA5551(0, 0, 255, 1),    // Bright blue
    GPACK_RGBA5551(255, 255, 0, 1),  // Bright yellow
    GPACK_RGBA5551(255, 0, 255, 1),  // Bright magenta
    GPACK_RGBA5551(0, 255, 255, 1),  // Bright cyan
    GPACK_RGBA5551(255, 255, 255, 1),// White
    GPACK_RGBA5551(127, 127, 127, 1),// Gray
    GPACK_RGBA5551(127, 127, 0, 1),  // Olive
    GPACK_RGBA5551(255, 127, 0, 1),  // Orange
    GPACK_RGBA5551(0, 0, 0, 1),      // Black
    GPACK_RGBA5551(255, 127, 64, 1), // Peach
    GPACK_RGBA5551(00, 255, 64, 1),  // Lime

    // Additional 64 vibrant colors
    GPACK_RGBA5551(255, 64, 64, 1),   // Light red
    GPACK_RGBA5551(64, 64, 255, 1),   // Light blue
    GPACK_RGBA5551(64, 255, 64, 1),   // Light green
    GPACK_RGBA5551(255, 128, 0, 1),   // Bright orange
    GPACK_RGBA5551(255, 0, 128, 1),   // Hot pink
    GPACK_RGBA5551(128, 0, 255, 1),   // Bright purple
    GPACK_RGBA5551(0, 128, 255, 1),   // Sky blue
    GPACK_RGBA5551(0, 255, 128, 1),   // Bright teal
    GPACK_RGBA5551(128, 255, 0, 1),   // Lime green
    GPACK_RGBA5551(255, 255, 128, 1), // Light yellow
    GPACK_RGBA5551(255, 128, 255, 1), // Light magenta
    GPACK_RGBA5551(128, 255, 255, 1), // Light cyan
    GPACK_RGBA5551(192, 0, 0, 1),     // Deep red
    GPACK_RGBA5551(0, 192, 0, 1),     // Deep green
    GPACK_RGBA5551(0, 0, 192, 1),     // Deep blue
    GPACK_RGBA5551(192, 192, 0, 1),   // Deep yellow
    GPACK_RGBA5551(192, 0, 192, 1),   // Deep magenta
    GPACK_RGBA5551(0, 192, 192, 1),   // Deep cyan
    GPACK_RGBA5551(255, 96, 0, 1),    // Bright tangerine
    GPACK_RGBA5551(255, 0, 96, 1),    // Bright rose
    GPACK_RGBA5551(96, 0, 255, 1),    // Bright indigo
    GPACK_RGBA5551(0, 96, 255, 1),    // Bright azure
    GPACK_RGBA5551(0, 255, 96, 1),    // Bright aquamarine
    GPACK_RGBA5551(96, 255, 0, 1),    // Bright chartreuse
    GPACK_RGBA5551(255, 192, 0, 1),   // Goldenrod
    GPACK_RGBA5551(255, 0, 192, 1),   // Fuchsia
    GPACK_RGBA5551(192, 0, 255, 1),   // Violet
    GPACK_RGBA5551(0, 192, 255, 1),   // Cerulean
    GPACK_RGBA5551(0, 255, 192, 1),   // Turquoise
    GPACK_RGBA5551(192, 255, 0, 1),   // Yellow-green
    GPACK_RGBA5551(255, 255, 192, 1), // Pale yellow
    GPACK_RGBA5551(255, 192, 255, 1), // Pale magenta
    GPACK_RGBA5551(192, 255, 255, 1), // Pale cyan
    GPACK_RGBA5551(255, 160, 0, 1),   // Amber
    GPACK_RGBA5551(255, 0, 160, 1),   // Bright raspberry
    GPACK_RGBA5551(160, 0, 255, 1),   // Bright lavender
    GPACK_RGBA5551(0, 160, 255, 1),   // Bright sky blue
    GPACK_RGBA5551(0, 255, 160, 1),   // Bright sea green
    GPACK_RGBA5551(160, 255, 0, 1),   // Bright lime
    GPACK_RGBA5551(255, 96, 96, 1),   // Light coral
    GPACK_RGBA5551(96, 255, 96, 1),   // Light mint
    GPACK_RGBA5551(96, 96, 255, 1),   // Light periwinkle
    GPACK_RGBA5551(255, 192, 128, 1), // Light peach
    GPACK_RGBA5551(255, 128, 192, 1), // Light pink
    GPACK_RGBA5551(192, 255, 128, 1), // Light lime
    GPACK_RGBA5551(128, 255, 192, 1), // Light aqua
    GPACK_RGBA5551(192, 128, 255, 1), // Light lavender
    GPACK_RGBA5551(128, 192, 255, 1), // Light sky blue
    GPACK_RGBA5551(255, 64, 128, 1),  // Bright rose pink
    GPACK_RGBA5551(128, 64, 255, 1),  // Bright violet
    GPACK_RGBA5551(64, 128, 255, 1),  // Bright azure
    GPACK_RGBA5551(64, 255, 128, 1),  // Bright mint
    GPACK_RGBA5551(128, 255, 64, 1),  // Bright lime yellow
    GPACK_RGBA5551(255, 128, 64, 1),  // Bright orange peach
    GPACK_RGBA5551(255, 64, 192, 1),  // Bright fuchsia pink
    GPACK_RGBA5551(192, 64, 255, 1),  // Bright purple lavender
    GPACK_RGBA5551(64, 192, 255, 1),  // Bright cerulean blue
    GPACK_RGBA5551(64, 255, 192, 1),  // Bright turquoise green
    GPACK_RGBA5551(192, 255, 64, 1),  // Bright yellow-green
    GPACK_RGBA5551(255, 192, 64, 1),  // Bright goldenrod
};

f32 memsize_float(s32 size, s32 *tag) {
    f32 sizeF = size;
    if (size < 1024) {
        *tag = 0;
        return size;
    } else {
        if (size < (1024 * 1024)) {
            *tag = 1;
            return ((f32) size) / 1024.0f;
        } else {
            *tag = 2;
            return ((f32) size) / (1024.0f * 1024.0f);
        }
    }
}

u8 gCrashMemPrintOrder[PP_RAM_TOTAL];

void crash_memory_chart(s32 x, s32 y, s32 width, s32 height) {
    DebugData *d = gDebug;
    u32 ramSize;
    s32 i;
    s32 totalHeight = 0;
    s32 barHeight;
    s32 tag;
    s32 r;
    s32 g;
    s32 b;
    s32 a;

    // Determine total RAM size
    if (gUseExpansionMemory) {
        ramSize = 0x800000; // 8 MB
    } else {
        ramSize = 0x400000; // 4 MB
    }

    // Calculate the total height of the bar chart
    for (i = 0; i < PP_RAM_TOTAL; i++) {
        if (d->ramSegments[gCrashMemPrintOrder[i]]) {
            if (gCrashMemPrintOrder[i] == gMemoryCapID) {
                d->ramSegments[gCrashMemPrintOrder[i]] -= gMemoryCapOffset;
            }
            totalHeight += d->ramSegments[gCrashMemPrintOrder[i]];
        }
    }

    crash_rectangle(x, y, width, 2, 0, 0, 0, 255);
    crash_rectangle(x, y + height - 2, width, 2, 0, 0, 0, 255);
    crash_rectangle(x, y + 2, 2, height - 4, 0, 0, 0, 255);
    crash_rectangle(x + width - 2, y + 2, 2, height - 4, 0, 0, 0, 255);

    x += 2;
    y += 2;
    width -= 4;
    height -= 4;
    // Draw each segment as a bar
    for (i = 0; i < PP_RAM_TOTAL; i++) {
        if (d->ramSegments[gCrashMemPrintOrder[i]]) {
            // Calculate the height of the bar proportional to the memory segment size
            barHeight = (d->ramSegments[gCrashMemPrintOrder[i]] * width) / ramSize;

            if (gCrashSelection == i) {
                r = ((sMemColour5s[gCrashMemPrintOrder[i]] >> 8) & 0xF8);
                g = ((sMemColour5s[gCrashMemPrintOrder[i]] >> 3) & 0xF8);
                b = ((sMemColour5s[gCrashMemPrintOrder[i]] << 2) & 0xF8);
                a = 255;
                crash_rectangle(x, y, barHeight, height, 255, 255, 255, 255);
                crash_rectangle(x + 1, y + 1, barHeight - 2, height - 2, r, g, b, a);
            } else {
                r = ((sMemColour5s[gCrashMemPrintOrder[i]] >> 8) & 0xF8) / 2;
                g = ((sMemColour5s[gCrashMemPrintOrder[i]] >> 3) & 0xF8) / 2;
                b = ((sMemColour5s[gCrashMemPrintOrder[i]] << 2) & 0xF8) / 2;
                a = 160;
                crash_rectangle(x, y, barHeight, height, r, g, b, a);
            }



            // Move the y-coordinate down for the next bar
            x += barHeight;
        }
        if (gCrashMemPrintOrder[i] == gMemoryCapID) {
            d->ramSegments[gCrashMemPrintOrder[i]] += gMemoryCapOffset;
        }
    }
}

void crash_reorder_ram(DebugData *d) {
    s32 i, j;

    // Initialize the gCrashMemPrintOrder array with indices
    for (i = 0; i < PP_RAM_TOTAL; i++) {
        gCrashMemPrintOrder[i] = i;
    }

    // Sort the indices in gCrashMemPrintOrder based on the size of d->ramSegments
    for (i = 0; i < PP_RAM_TOTAL - 1; i++) {
        for (j = i + 1; j < PP_RAM_TOTAL; j++) {
            if (d->ramSegments[gCrashMemPrintOrder[i]] < d->ramSegments[gCrashMemPrintOrder[j]]) {
                // Swap the indices to order by descending size
                u8 temp = gCrashMemPrintOrder[i];
                gCrashMemPrintOrder[i] = gCrashMemPrintOrder[j];
                gCrashMemPrintOrder[j] = temp;
            }
        }
    }
}

extern u8 *main_TEXT_START[];
extern u8 *main_TEXT_END[];
extern u8 *main_DATA_START[];
extern u8 *main_DATA_END[];
extern u8 *main_RODATA_START[];
extern u8 *main_RODATA_END[];
extern u8 *main_BSS_START[];
extern u8 *main_BSS_END[];

extern u8 *tex2d_ROM_START[];
extern u8 *tex2d_ROM_END[];
extern u8 *tex3d_ROM_START[];
extern u8 *tex3d_ROM_END[];
extern u8 *sprites_ROM_START[];
extern u8 *sprites_ROM_END[];
extern u8 *objmdl_ROM_START[];
extern u8 *objmdl_ROM_END[];
extern u8 *objanim_ROM_START[];
extern u8 *objanim_ROM_END[];

extern u8 *main_TEXT_SIZE[];
extern u8 *main_DATA_SIZE[];
extern u8 *main_RODATA_SIZE[];
extern u8 *main_BSS_SIZE[];
extern s32 *gTextureCache;
extern s32 gNumberOfLoadedTextures;
extern s32 *gSpriteCache;
extern s32 gSpriteCacheCount;
extern s32 *gModelCache;
extern s32 gModelCacheCount;
extern void *gMusicSequenceData;
extern void *gJingleSequenceData;
extern u64 *gGfxSPTaskOutputBuffer;
extern Gfx *gDisplayLists[2];
extern u8 *gAudioHeapStack;

char sAssetName[32];

char *debug_asset_name(s32 assetType, s32 assetID, s32 extra) {
    u32 searchAddr;
    char *typeStr;
    s32 dmaCount;

    switch (assetType) {
        case ASSET_TEXTURES_2D:
            typeStr = "Tex 2D";
            searchAddr = (u32) tex2d_ROM_START;
            break;
        case ASSET_TEXTURES_3D:
            typeStr = "Tex 3D";
            searchAddr = (u32) tex3d_ROM_START;
            break;
        case ASSET_SPRITES:
            typeStr = "Sprite";
            searchAddr = (u32) sprites_ROM_START;
            break;
        case ASSET_OBJECT_MODELS:
            typeStr = "Object Model";
            searchAddr = (u32) objmdl_ROM_START;
            break;
        case ASSET_OBJECT_ANIMATIONS:
            typeStr = "Object Animation";
            searchAddr = (u32) objanim_ROM_START;
            break;
    }

    searchAddr += (assetID * 32);

    if (extra) {
        dmaCount = 32;
    } else {
        dmaCount = 20;
    }
    dmacopy(searchAddr, (u32) sAssetName, dmaCount);
    if (extra) {
        crash_text(CRASH_BORDER_X + 280, 25, GPACK_RGBA5551(255, 255, 0, 1), "%s:%X", typeStr, assetID);
        crash_text(CRASH_BORDER_X + 280, 34, GPACK_RGBA5551(255, 255, 0, 1), sAssetName);
    }
    sAssetName[20] = 0;

    return sAssetName;
}

void crash_mem_info_text(MemoryPoolSlot *slot, s32 x, s32 y, u16 col, s32 useExtra) {
    s32 i;
    s32 texID;
    s32 tag = slot->colourTag;
    TextureHeader *texHeader;
    Sprite *sprite;
    ObjectHeader *objHeader;
    Object *obj;
    ObjectModel *objModel;
    ModelInstance *objGfx;
    ObjectModel_44 *objAnim;

    switch (tag) {
        case PP_RAM_OBJHEADERS:
            objHeader = (ObjectHeader *) slot->data;
            crash_text(x + 40, y, col, "%s", objHeader->internalName);
            break;
        case PP_RAM_OBJECTS:
            obj = (Object *) slot->data;
            crash_text(x + 40, y, col, "%s", obj->header->internalName);
            break;
        case PP_RAM_ANIMATIONS:
            crash_text(x + 40, y, col, "Unknown");
            break;
        default:
            if ((s32) slot->data == (s32) gVideoFramebuffers[0] || 
                (s32) slot->data == (s32) gVideoFramebuffers[1] ||
                (s32) slot->data == (s32) gVideoFramebuffers[2]) {
                crash_text(x + 40, y, col, "Colour Buffer");
                return;
            } else if ((s32) slot->data == (s32) gVideoDepthBuffer) {
                crash_text(x + 40, y, col, "Depth Buffer");
                return;
            } else if ((s32) slot->data == (s32) gMusicSequenceData) {
                crash_text(x + 40, y, col, "Music");
                return;
            } else if ((s32) slot->data == (s32) gJingleSequenceData) {
                crash_text(x + 40, y, col, "Jingle");
                return;
            } else if ((s32) slot->data == (s32) gGfxSPTaskOutputBuffer) {
                crash_text(x + 40, y, col, "FIFO Task Buf");
                return;
            } else if ((s32) slot->data == (s32) gDisplayLists[0] || 
                       (s32) slot->data == (s32) gDisplayLists[1]) {
                crash_text(x + 40, y, col, "Displaylists");
                return;
            } else if ((s32) slot->data == (s32) gAudioHeapStack) {
                crash_text(x + 40, y, col, "alHeap Stack");
                return;
            }

            texHeader = (TextureHeader *) slot->data;
            texID = -200;
            // First see if it's a texture
            for (i = 0; i < gNumberOfLoadedTextures; i++) {
                if ((TextureHeader *) gTextureCache[(i << 1) + 1] == texHeader) {
                    texID = gTextureCache[i << 1];
                }
            }
            if (texID != -200) {
                if (texID & 0x8000) {
                    crash_text(x + 40, y, col, debug_asset_name(ASSET_TEXTURES_3D, texID & 0x7FFF, useExtra));
                } else {
                    crash_text(x + 40, y, col, debug_asset_name(ASSET_TEXTURES_2D, texID, useExtra));
                }
                return;
            }
            // Okay, lets try for a sprite?
            sprite = (Sprite *) slot->data;
            for (i = 0; i < gSpriteCacheCount; i++) {
                if ((Sprite *) gSpriteCache[(i << 1) + 1] == sprite) {
                    texID = gSpriteCache[i << 1];
                }
            }
            if (texID != -200) {
                crash_text(x + 40, y, col, debug_asset_name(ASSET_SPRITES, texID, useExtra));
                return;
            }
            // Try object models
            objModel = (ObjectModel *) slot->data;
            for (i = 0; i < gModelCacheCount; i++) {
                if ((ObjectModel *) gModelCache[(i << 1) + 1] == objModel) {
                    texID = gModelCache[i << 1];
                }
            }
            if (texID != -200) {
                crash_text(x + 40, y, col, debug_asset_name(ASSET_OBJECT_MODELS, texID, useExtra));
                return;
            }
            // I give up :(
            crash_text(x + 40, y, col, "Unknown");
            break;
    }
}

const u32 sCrashCodeSizes[] = {
    (u32) main_TEXT_SIZE,
    (u32) main_DATA_SIZE,
    (u32) main_RODATA_SIZE,
    (u32) main_BSS_SIZE
};

const u32 sCrashCodeAddr[] = {
    (u32) main_TEXT_START,
    (u32) main_DATA_START,
    (u32) main_RODATA_START,
    (u32) main_BSS_START
};

const char *sCrashCodeStrings[] = {
    "Text",
    "Data",
    "Rodata",
    "Bss"
};

void crash_mem_details(void) {
    s32 x;
    s32 y;
    s32 numSlots;
    s32 stopCounting;
    s32 i;
    int flags;
    int nextIndex;
    MemoryPoolSlot *slot;
    f32 size;
    s32 tag;
    s32 col;
    s32 useScroll;
    f32 scrollLen;
    s32 scrollSize;
    s32 j;
    s32 extra;

    x = 240;
    y = 77 - (gCrashAltScroll * 9);
    numSlots = 0;
    stopCounting = FALSE;
    scrollSize = 0;
    useScroll = FALSE;
    if (gCrashMemPrintOrder[gCrashSelection] == PP_RAM_CODE) {
        for (i = 0; i < 4; i++) {
            numSlots++;
            scrollSize++;
            
            if (gCrashAltSelection + 1 == scrollSize && gCrashAltView == 1) {
                crash_rectangle(x + 37, y - 1, 192, 9, 255, 255, 255, 144);
                col = GPACK_RGBA5551(0, 0, 0, 1);
                crash_text(x + 40, y, col, sCrashCodeStrings[i]);
                crash_text(x + 166, y, col, "0#%X", sCrashCodeAddr[i]);
            } else {
                col = GPACK_RGBA5551(255, 255, 255, 1);
                crash_text(x + 40, y, col, sCrashCodeStrings[i]);
                size = memsize_float(sCrashCodeSizes[i], &tag);
                crash_text(x + 172, y, col, "%2.3f%s", (f64) size, sMemLabels[tag]);
            }
            y += 9;
        }
    } else {
        for (i = 0; i <= gNumberOfMemoryPools; i++) {
            slot = &gMemoryPools[i].slots[0];
            
            do {
                flags = slot->flags;
                nextIndex = slot->nextIndex;
    
                if (flags != SLOT_FREE) {
                    if (debug_tag_index(slot->colourTag) == gCrashMemPrintOrder[gCrashSelection]) {
                        scrollSize++;
                        if (y < 77) {
                            y += 9;
                            useScroll = TRUE;
                            slot = &gMemoryPools[i].slots[slot->nextIndex];
                            continue;
                        }
                        if (y > gScreenHeight - 50 || stopCounting) {
                            useScroll = TRUE;
                            stopCounting = TRUE;
                            slot = &gMemoryPools[i].slots[slot->nextIndex];
                            continue;
                        }
                        if (gCrashAltSelection + 1 == scrollSize && gCrashAltView == 1) {
                            crash_rectangle(x + 37, y - 1, 192, 9, 255, 255, 255, 144);
                            col = GPACK_RGBA5551(0, 0, 0, 1);
                            crash_text(x + 166, y, col, "0#%X", (u32) slot->data);
                            extra = 1;
                        } else {
                            col = GPACK_RGBA5551(255, 255, 255, 1);
                            size = memsize_float(slot->size, &tag);
                            crash_text(x + 172, y, col, "%2.3f%s", (f64) size, sMemLabels[tag]);
                            extra = 0;
                        }
                        crash_mem_info_text(slot, x, y, col, extra);
                        y += 9;
                        numSlots++;
                    }
                }
    
                if (nextIndex == -1) {
                    continue;
                } else {
                    slot = &gMemoryPools[i].slots[slot->nextIndex];
                }
            } while (nextIndex != -1);
        }
    }

    gAltNumValids = scrollSize;
    if (useScroll) {
        gMaxAltScroll = scrollSize - numSlots;
        gAltScrollSize = numSlots;
        crash_rectangle(x + 232, 68, 8, gScreenHeight - 50 - 68, 127, 127, 127, 144);
        scrollLen =  (f32) (gScreenHeight - 50 - 68) / (f32) scrollSize;
        crash_rectangle(x + 232, 68 + (scrollLen * gCrashAltScroll), 8, (scrollLen * numSlots) + 1, 255, 255, 255, 255);
    }
    
    crash_text(x + 100, 66, GPACK_RGBA5551(255, 255, 255, 1), "Entries:%d", scrollSize);
    crash_text(CRASH_BORDER_X + 128, gScreenHeight - 20, GPACK_RGBA5551(255, 255, 255, 1), "Press A to dump RAM over USB");
}

void crash_page_memory(void) {
    s32 i;
    DebugData *d = gDebug;
    f32 size;
    s32 y;
    s32 x;
    u32 ramSize;
    s32 tag;
    s32 textWidth;
    s32 col;
    s32 useScroll;
    s32 scrollNum;
    s32 scrollSize;
    f32 scrollLen;
    s32 stopCounting;
    s32 numValids;
    s32 prevOpt;

    if (gUseExpansionMemory) {
        ramSize = 0x800000;
    } else {
        ramSize = 0x400000;
    }

    x = CRASH_BORDER_X + 16;
    size = memsize_float(ramSize, &tag);
    crash_text(x, 54, GPACK_RGBA5551(255, 255, 255, 1), "Total: %2.3f%s", (f64) size, sMemLabels[tag]);
    size = memsize_float(d->ramTotal, &tag);
    if (d->ramTotal >= ramSize) {
        col = GPACK_RGBA5551(255, 0, 0, 1);
    } else {
        col = GPACK_RGBA5551(255, 255, 255, 1);
    }
    crash_text(x + 112, 54, col, "Used: %2.3f%s", (f64) size, sMemLabels[tag]);
    size = memsize_float(ramSize - d->ramTotal, &tag);
    if (size < 0.0f) {
        size = 0.0f;
    }
    crash_text(x + 224, 54, col, "Free: %2.3f%s", (f64) size, sMemLabels[tag]);
    if (gMemoryPools[POOL_MAIN].curNumSlots >= gMemoryPools[POOL_MAIN].maxNumSlots) {
        col = GPACK_RGBA5551(255, 0, 0, 1);
    } else {
        col = GPACK_RGBA5551(255, 255, 255, 1);
    }
    crash_text(x + 336, 54, col, "Slots:%d of %d", gMemoryPools[POOL_MAIN].curNumSlots, gMemoryPools[POOL_MAIN].maxNumSlots);
    crash_reorder_ram(d);

    scrollNum = 0;
    scrollSize = 0;
    useScroll = FALSE;
    stopCounting = FALSE;
    y = 68 - (gCrashScroll * 9);
    for (i = 0; i < PP_RAM_TOTAL; i++) {
        //if (d->ramSegments[gCrashMemPrintOrder[i]]) {
            scrollSize++;
            if (y < 68) {
                y += 9;
                useScroll = TRUE;
                continue;
            }
            if (y > gScreenHeight - 50 || stopCounting) {
                stopCounting = TRUE;
                useScroll = TRUE;
                continue;
            }
            scrollNum++;
            if (gCrashSelection == i) {
                if (gCrashAltView == 0) {
                    crash_rectangle(x - 13, y - 1, 224, 9, 255, 255, 255, 144);
                } else {
                    crash_rectangle(x - 13, y - 1, 224, 9, 127, 127, 127, 144);
                }
                col = GPACK_RGBA5551(0, 0, 0, 1);
            } else {
                col = GPACK_RGBA5551(255, 255, 255, 1);
            }
            crash_rectangle(x - 12, y, 9, 7, 
                (sMemColour5s[gCrashMemPrintOrder[i]] >> 8) & 0xF8,
                (sMemColour5s[gCrashMemPrintOrder[i]] >> 3) & 0xF8,
                (sMemColour5s[gCrashMemPrintOrder[i]] << 2) & 0xF8, 255);
            size = memsize_float(d->ramSegments[gCrashMemPrintOrder[i]], &tag);
            textWidth = crash_strwidth(sPuppyprintMemColours[gCrashMemPrintOrder[i]]);
            if (gMemoryCapID == gCrashMemPrintOrder[i]) {
                col = GPACK_RGBA5551(255, 0, 0, 1);
            }
            crash_text(x + 45 - (textWidth / 2), y, col, sPuppyprintMemColours[gCrashMemPrintOrder[i]]);
            crash_text(x + 90, y, col, "%2.3f%s", (f64) size, sMemLabels[tag]);
            crash_text(x + 156, y, col, "(%2.3f%%)", (f64) (((f32) d->ramSegments[gCrashMemPrintOrder[i]] / (f32) ramSize) * 100.0f));
            y += 9;
        //}
    }
    
    if (useScroll) {
        gMaxScroll = scrollSize - scrollNum;
        gScrollSize = scrollNum;
        crash_rectangle(x + 216, 68, 8, gScreenHeight - 50 - 68, 127, 127, 127, 144);
        scrollLen =  (f32) (gScreenHeight - 50 - 68) / (f32) scrollSize;
        crash_rectangle(x + 216, 68 + (scrollLen * gCrashScroll), 8, (scrollLen * scrollNum) + 1, 255, 255, 255, 255);
    }

    crash_mem_details();

    crash_memory_chart(32, gScreenHeight - 40, gScreenWidth - 64, 12);
}

void crash_screen_sleep(s32 ms) {
    u32 cycles = ms * 1000 * osClockRate / 1000000;
    osSetTime(0);
    while (osGetTime() < cycles) {}
}

u8 viSetOnce = 0;

#ifdef MAP_PARSE
extern u8 *map_ROM_START[];
extern u8 *map_ROM_END[];

/* Relies on the linker being different which is a little annoying until custom linker support is added
    map_ROM_START = __romPos;
    map_VRAM = ADDR(.map);
    .map assets_VRAM_END : AT(map_ROM_START) SUBALIGN(16)
    {
        FILL(0x00000000);
        map_DATA_START = .;
        build/assets/map.bin.o(.data);
        map_DATA_END = .;
        map_DATA_SIZE = ABSOLUTE(map_DATA_END - map_DATA_START);
    }
    __romPos += SIZEOF(.map);
    map_ROM_END = __romPos;
    map_VRAM_END = .;

    goes beneath the assets section at the bottom of .ld that looks very similar.*/
s32 func_name_find(u32 addr) {
    u32 searchAddr = (u32) map_ROM_START;
    s32 symbolFlip;
    u32 symbolAddr;
    u32 symbolAddrPrev;
    MapSymbol symbol[2];
    bzero(&symbol, sizeof(MapSymbol) * 2);

    addr = (addr & 0x1FFFFFFF) | 0xA0000000;
    symbolFlip = 0;
    dmacopy(searchAddr, (u32) &symbol[1], sizeof(MapSymbol));
    searchAddr += sizeof(MapSymbol);
    symbolAddr = (symbol[1].address & 0x1FFFFFFF) | 0xA0000000;

    while (1) {
        symbolAddrPrev = symbolAddr;
        dmacopy(searchAddr, (u32) &symbol[symbolFlip], sizeof(MapSymbol));
        symbolAddr = (symbol[symbolFlip].address & 0x1FFFFFFF) | 0xA0000000;
        if (addr >= symbolAddrPrev && addr < symbolAddr) {
            bcopy(symbol[symbolFlip ^ 1].name, gCrashFuncName, 32);
            return TRUE;
        }
        symbolFlip ^= 1;
        searchAddr += sizeof(MapSymbol);
        if (searchAddr > (u32) map_ROM_END) {
            return FALSE;
        }
    }
}
#else
s32 func_name_find(u32 addr) {
    return FALSE;
}
#endif

void crash_render(OSThread *t) {
    s32 i;
    __OSThreadContext *c;
    s32 prevOpt;
    s32 numValids;
    s32 repeat;
    u32 first = osGetCount();

    c = &t->context;

    gCrashInput = 0;
    for (i = 0; i < 4; i++) {
        gCrashInput |= input_pressed(i);
    }

    if (gCrashInput & R_TRIG) {
        gCrashFBUpdate = TRUE;
        gCrashSelection = 0;
        gCrashAltView = 0;
        gCrashScroll = 0;
        gCrashPage++;
        if (gCrashPage == CRASH_PAGE_COUNT) {
            gCrashPage = 0;
        }
    } else if (gCrashInput & L_TRIG) {
        gCrashPage--;
        if (gCrashPage >= CRASH_PAGE_COUNT) {
            gCrashPage = CRASH_PAGE_COUNT - 1;
        }
        gCrashFBUpdate = TRUE;
        gCrashSelection = 0;
        gCrashAltView = 0;
        gCrashScroll = 0;
        gCrashScrollCursor = 0;
    }

    switch (gCrashPage) {
        case CRASH_PAGE_GPREGS:
            break;
        case CRASH_PAGE_FPREGS:
            break;
        case CRASH_PAGE_STACKS:
            break;
        case CRASH_PAGE_ASSERTS:
            break;
        case CRASH_PAGE_MEMORY:
            numValids = 0;
            if (gCrashInput & A_BUTTON) {
                debug_ram_dump();
            }
            for (i = 0; i < PP_RAM_TOTAL; i++) {
                //if (gDebug->ramSegments[i] != 0) {
                    numValids++;
                //}
            }
            if (gCrashInput & U_JPAD || gCrashInput & U_CBUTTONS) {
                if (gCrashInput & U_JPAD) {
                    repeat = 1;
                } else {
                    repeat = 8;
                }
                for (i = 0; i < repeat; i++) {
                    if (gCrashAltView == 0) {
                        prevOpt = gCrashSelection;
                        if (gCrashSelection > 0) {
                            gCrashSelection--;
                            gCrashAltSelection = 0;
                            gCrashAltScrollCursor = 0;
                            gCrashAltScroll = 0;
                            if (numValids > gScrollSize && gCrashSelection < gCrashScroll + 4 && gCrashScroll > 0) {
                                gCrashScroll--;
                            }
                            gCrashScrollCursor = gCrashSelection;
                        }
                        if (prevOpt != gCrashSelection) {
                            gCrashFBUpdate = TRUE;
                        }
                    } else {
                        prevOpt = gCrashAltSelection;
                        if (gCrashAltSelection > 0) {
                            gCrashAltSelection--;
                            if (gAltNumValids > gAltScrollSize && gCrashAltSelection < gCrashAltScroll + 4 && gCrashAltScroll > 0) {
                                gCrashAltScroll--;
                            }
                            gCrashAltScrollCursor = gCrashAltSelection;
                        }
                        if (prevOpt != gCrashAltSelection) {
                            gCrashFBUpdate = TRUE;
                        }
                    }
                }
            } else if (gCrashInput & D_JPAD || gCrashInput & D_CBUTTONS) {
                if (gCrashInput & D_JPAD) {
                    repeat = 1;
                } else {
                    repeat = 8;
                }
                for (i = 0; i < repeat; i++) {
                    if (gCrashAltView == 0) {
                        prevOpt = gCrashSelection;
                        if (gCrashSelection < numValids - 1) {
                            gCrashSelection++;
                            gCrashAltSelection = 0;
                            gCrashAltScrollCursor = 0;
                            gCrashAltScroll = 0;
                            if (numValids > gScrollSize && gCrashSelection >= gCrashScroll + gScrollSize - 4 && gCrashScroll < gMaxScroll) {
                                gCrashScroll++;
                            }
                            gCrashScrollCursor = gCrashSelection;
                        }
                        if (prevOpt != gCrashSelection) {
                            gCrashFBUpdate = TRUE;
                        }
                    } else {
                        prevOpt = gCrashAltSelection;
                        if (gCrashAltSelection < gAltNumValids - 1) {
                            gCrashAltSelection++;
                            if (gAltNumValids > gAltScrollSize && gCrashAltSelection >= gCrashAltScroll + gAltScrollSize - 4 && gCrashAltScroll < gMaxAltScroll) {
                                gCrashAltScroll++;
                            }
                            gCrashAltScrollCursor = gCrashAltSelection;
                        }
                        if (prevOpt != gCrashAltSelection) {
                            gCrashFBUpdate = TRUE;
                        }
                    }
                }
            }
            if (gCrashInput & L_JPAD) {
                if (gCrashAltView != 0) {
                    gCrashAltView = 0;
                    gCrashFBUpdate = TRUE;
                }
            } else if (gCrashInput & R_JPAD) {
                if (gCrashAltView != 1) {
                    gCrashAltView = 1;
                    gCrashFBUpdate = TRUE;
                }
            } 
            break;
    }

    if (gCrashFBUpdate == FALSE) {
        return;
    }

    dcopy(gVideoDepthBuffer, gCrashFB, (gScreenWidth * gScreenHeight) * 2);
    
    if (gCrashPage != CRASH_PAGE_EMPTY) {
        crash_line(CRASH_BORDER_X - 1, 11, gScreenWidth - CRASH_BORDER_X, 11, 255, 255, 255, 160);
        crash_line(CRASH_BORDER_X - 1, gScreenHeight - 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 12, 255, 255, 255, 160);
        crash_line(CRASH_BORDER_X - 1, 12, CRASH_BORDER_X - 1, gScreenHeight - 13, 255, 255, 255, 160);
        crash_line(gScreenWidth - CRASH_BORDER_X, 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 13, 255, 255, 255, 160);
        crash_rectangle(CRASH_BORDER_X, 12, gScreenWidth - (CRASH_BORDER_X * 2), 33, 127, 0, 0, 160);
        
        crash_line(CRASH_BORDER_X, 45, gScreenWidth - CRASH_BORDER_X - 1, 45, 255, 255, 255, 160);
        
        crash_rectangle(CRASH_BORDER_X, 46, gScreenWidth - (CRASH_BORDER_X * 2), gScreenHeight - 70, 0, 0, 0, 160);
        crash_rectangle(CRASH_BORDER_X, gScreenHeight - 24, gScreenWidth - (CRASH_BORDER_X * 2) - 128, 12, 0, 0, 0, 160);
        crash_rectangle(gScreenWidth - (CRASH_BORDER_X * 2) - 107, gScreenHeight - 23, 127, 11, 255, 255, 255, 160);
        crash_line(gScreenWidth - (CRASH_BORDER_X * 2) - 108, gScreenHeight - 24, gScreenWidth - (CRASH_BORDER_X * 2) - 108, gScreenHeight - 13, 255, 255, 255, 160);
        crash_line(gScreenWidth - (CRASH_BORDER_X * 2) - 107, gScreenHeight - 24, gScreenWidth - (CRASH_BORDER_X) - 1, gScreenHeight - 24, 255, 255, 255, 160);

        crash_text(gScreenWidth - (CRASH_BORDER_X) - 120, gScreenHeight - 21, GPACK_RGBA5551(0, 0, 0, 1), "Page %d of %d", gCrashPage + 1, CRASH_PAGE_COUNT);

        // iykyk
        crash_rectangle(9, 4, 8, 6, 255, 0, 0, 255);
        crash_rectangle(10, 5, 6, 4, 255, 255, 255, 255);

        crash_text(CRASH_BORDER_X + 16, 16, GPACK_RGBA5551(255, 255, 0, 1), "Thread:%s(%d)", sThreadNames[crash_thread_name(t->id)], t->id);
        crash_text(CRASH_BORDER_X + 16 + 144, 16, GPACK_RGBA5551(255, 255, 0, 1), "PC:0#%8X", (u32) c->pc);
        crash_text(CRASH_BORDER_X + 16 + 288, 16, GPACK_RGBA5551(255, 255, 0, 1), "RA:0#%8X", (u32) c->ra);
        crash_text(CRASH_BORDER_X + 16, 25, GPACK_RGBA5551(255, 255, 0, 1), "Cause:%s", gCauseDesc[gCrashCause]);
        if (gCrashFuncFound) {
            crash_text(CRASH_BORDER_X + 16, 34, GPACK_RGBA5551(255, 255, 0, 1), "Func Name: %s", gCrashFuncName);
        }
    }

    switch (gCrashPage) {
        case CRASH_PAGE_GPREGS:
            crash_page_gpregs(t);
            break;
        case CRASH_PAGE_FPREGS:
            crash_page_fpregs(t);
            break;
        case CRASH_PAGE_STACKS:
            crash_page_stacks(t);
            break;
        case CRASH_PAGE_ASSERTS:
            crash_page_assert();
            break;
        case CRASH_PAGE_MEMORY:
            crash_page_memory();
            break;
    }

    crash_text(CRASH_BORDER_X + 16, gScreenHeight - 20, GPACK_RGBA5551(255, 255, 255, 1), "%2.3fms", (f64) ((f32) OS_CYCLES_TO_USEC(osGetCount() - first) / 1000.0f));

    osWritebackDCacheAll();
    osViSwapBuffer(gCrashFB);
    if (viSetOnce == 0) {
        vi_change(gScreenWidth, gScreenHeight);
        osViBlack(FALSE);
    }
    viSetOnce = 1;
    gCrashFB = gVideoFramebuffers[(gCrashFBFlip ^= 1) + 1];
    gCrashFBUpdate = FALSE;
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

void crash_default_page(OSThread *t) {
    s32 threadID;
    __OSThreadContext *c;

    if (gCrashAssetTripped) {
        gCrashPage = CRASH_PAGE_ASSERTS;
    } else if (gCrashCause == 20) {
        gCrashPage = CRASH_PAGE_MEMORY;
    } else {
        threadID = crash_check_stack();
        if (threadID) {
            switch(threadID) {
                case 1:
                    __osFaultedThread = &gThread1;
                    gThreadStackSize = STACK_GAME;
                    gCrashCause = 18;
                    break;
                case 3:
                    __osFaultedThread = &gThread3;
                    gThreadStackSize = STACK_GAME;
                    gCrashCause = 18;
                    break;
                case 4:
                    __osFaultedThread = audioGetThread();
                    gThreadStackSize = STACK_AUD;
                    gCrashCause = 18;
                    break;
                case 5:
                    __osFaultedThread = &gMainSched.thread;
                    gThreadStackSize = STACK_SCHED;
                    gCrashCause = 18;
                    break;
                case 30:
                    __osFaultedThread = gThread30;
                    gThreadStackSize = STACK_BGLOAD;
                    gCrashCause = 18;
                    break;
                case 69:
                    __osFaultedThread = &gThreadUsb;
                    gThreadStackSize = STACK_USB;
                    gCrashCause = 18;
                    break;
            }
            gCrashPage = CRASH_PAGE_STACKS;
        } else {
            c = &t->context;
            gCrashCause = (c->cause >> 2) & 0x1F;

            if (gCrashCause == 15) {
                gCrashPage = CRASH_PAGE_FPREGS;
            }
        }
    }
}

void crash2_render(void) {
    u32 stackMin;
    u32 stackMax;
    __OSThreadContext *c;
    OSThread *t = &gCrashThread;

    c = &t->context;

    stackMin = (u32) (crash_stack_pos(t->id) - c->sp);
    if (stackMin > 0xFFFF) {
        stackMin = 0xFFFF;
    }
    stackMax = (u32) crash_stack_size(t->id);
    crash_rectangle(0, 0, gScreenWidth, gScreenHeight, 0, 0, 255, 255);
    crash_text(40, 32, 0xFFFF, "Well done, you crashed the crash screen");
    crash_text(40, 50, 0xFFFF, "PC:0#%08X", (u32) c->pc);
    crash_text(40, 60, 0xFFFF, "RA:0#%08X", (u32) c->ra);
    crash_text(40, 70, 0xFFFF, "Stack:0#%X of 0#%X", stackMin, stackMax);
    if (gCrashFuncFound) {
        crash_text(40, 80, 0xFFFF, "Func Name: %s", gCrashFuncName);
    }
    osWritebackDCacheAll();
    osViSwapBuffer(gCrashFB);
    osViBlack(FALSE);
    vi_change(gScreenWidth, gScreenHeight);
}

void crash_thread2(UNUSED void *var) {
    OSMesg msg;
    s32 oldW;
    s32 oldH;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashQueue2, (OSMesg) 2);
    osSetEventMesg(OS_EVENT_FAULT, &gCrashQueue2, (OSMesg) 8);

    osRecvMesg(&gCrashQueue2, &msg, OS_MESG_BLOCK);
    osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
    osStopThread(&gMainSched.thread);
    osStopThread(&gCrashThread);
    while (__osDpDeviceBusy() == 1) {}
    while (__osSpDeviceBusy() == 1) {}
    crash_screen_sleep(50);
    gScreenWidth = 320;
    gScreenHeight = 240;
    gCrashFB = (u16 *) gVideoCurrFramebuffer;
    gCrashFuncFound = func_name_find((u32) gCrashThread.context.pc);
    crash2_render();

    while (1) {}
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
    osStopThread(&gMainSched.thread);
    osCreateMesgQueue(&gCrashQueue2, gCrashQueueBuf2, ARRAY_COUNT(gCrashQueueBuf2));
    gCrashThreadStack2 = (u64 *) mempool_alloc(STACK_CRASH2, PP_RAM_STACK);
    if (gCrashThreadStack2 == NULL) {
        gCrashThreadStack2 = (u64 *) 0x803C0000;
    }
    gCrashThread2 = (OSThread *) mempool_alloc(STACK_CRASH2, PP_RAM_DEBUG);
    if (gCrashThread2 == NULL) {
        gCrashThread2 = (OSThread *) 0x803C1000;
    }
    osCreateThread(gCrashThread2, 9, &crash_thread2, 0, gCrashThreadStack2 + (STACKSIZE(STACK_CRASH2)), 30);
    osStartThread(gCrashThread2);
    while (__osDpDeviceBusy() == 1) {}
    while (__osSpDeviceBusy() == 1) {}
    while (__osDpDeviceBusy() == 1) {}
    while (__osSpDeviceBusy() == 1) {}
    input_init();
    gCrashFBUpdate = TRUE;
    oldW = gScreenWidth;
    oldH = gScreenHeight;
    gScreenWidth = 512;
    gScreenHeight = 240;
    DEBUG_VAR(gAutoplayTest, 0);
    if (gVideoCurrFramebuffer != NULL) {
        if (gBitDepth == G_IM_SIZ_16b) {
            framebuffer_scale_16b(gVideoCurrFramebuffer, gVideoDepthBuffer, oldW, oldH, gScreenWidth, gScreenHeight);
        } else {
            framebuffer_scale_32b((u32 *) gVideoCurrFramebuffer, gVideoDepthBuffer, oldW, oldH, gScreenWidth, gScreenHeight);
        }
    }
    gConfig.screenBits = SCREENBITS_16b;
    if (gVideoFramebuffers[1] == NULL) {
        gVideoFramebuffers[1] = (u16 *) 0x80300000;
    }
    if (gVideoFramebuffers[2] == NULL) {
        gVideoFramebuffers[2] = (u16 *) 0x80380000;
    }
    gCrashFB = gVideoFramebuffers[1];

    crash_default_page(crash_error_thread());
    gCrashFuncFound = func_name_find((u32) crash_error_thread()->context.pc);
    while (1) { 
        input_update(0, LOGIC_30FPS);
        crash_render(crash_error_thread());
        crash_screen_sleep(10);
    }
}

void crash_init(void) {
    osCreateMesgQueue(&gCrashQueue, gCrashQueueBuf, ARRAY_COUNT(gCrashQueueBuf));
    gCrashThreadStack = (u64 *) mempool_alloc(STACK_CRASH, PP_RAM_STACK);
    osCreateThread(&gCrashThread, 2, &crash_thread, 0, gCrashThreadStack + STACKSIZE(STACK_CRASH), 30);
    osStartThread(&gCrashThread);
}