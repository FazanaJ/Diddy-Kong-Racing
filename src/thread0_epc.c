/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800B6F50 */

#include "thread0_epc.h"
#include "types.h"
#include "macros.h"
#include "menu.h"
#include "save_data.h"
#include "lib/src/libc/xprintf.h"
#include "printf.h"
#include "objects.h"
#include "controller.h"
#include "game.h"
#include "stdarg.h"
#include "string.h"
#include "video.h"
#include "main.h"
#include "audiomgr.h"

s32 _Printf(outfun prout, char *dst, const char *fmt, va_list args);

u16 sCrashX;
u16 sCrashY;
Object *sCrashObjID;
s16 sCrashObjAct;
s16 sCrashScroll;
s16 sCrashMaxScroll;
u8 gAssert;
u8 sCrashPage;
u8 sCrashUpdate;
char gAssertString[127];
char *sObjectStrings[] = { "INIT", "LOOP", "DRAW" };

extern s32 gVideoCurrFbIndex;

static inline char *write_to_buf(char *buffer, const char *data, size_t size) {
    char *ret = buffer;
    bcopy((char *) data, buffer, size);
    return ret + size;
}

void puppyprint_assert(char *str, ...) {
    char textBytes[127];
    va_list arguments;
    gAssert = TRUE;

    bzero(textBytes, sizeof(textBytes));
    va_start(arguments, str);
    if ((_Printf(write_to_buf, textBytes, str, arguments)) <= 0) {
        va_end(arguments);
        return;
    }
    bcopy(textBytes, gAssertString, sizeof(textBytes));
    va_end(arguments);
    *(volatile int *) 0 = 0;
}

void puppyprint_assert_nonblocking(char *str, ...) {
    char textBytes[127];
    va_list arguments;
    gAssert = TRUE;

    bzero(textBytes, sizeof(textBytes));
    va_start(arguments, str);
    if ((_Printf(write_to_buf, textBytes, str, arguments)) <= 0) {
        va_end(arguments);
        return;
    }
    bcopy(textBytes, gAssertString, sizeof(textBytes));
    va_end(arguments);
}

const char *const gCauseDesc[18] = {
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
};

const char *const gFpcsrDesc[6] = {
    "Unimplemented operation", "Invalid operation", "Division by zero", "Overflow", "Underflow", "Inexact operation",
};

const u8 gCrashScreenCharToGlyph[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 41, -1, -1, -1, 43, -1, -1, 37, 38, -1, 42, -1, 39, 44, -1, 0,  1,  2,  3,
    4,  5,  6,  7,  8,  9,  36, -1, -1, -1, -1, 40, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
};

// Bit-compressed font. '#' = 1, '.' = 0
const u32 gCrashScreenFont[7 * 9] = {
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
};

extern u64 osClockRate;

CrashData gCrashScreen;

void crash_screen_draw_rect(s32 x, s32 y, s32 w, s32 h) {
    u16 *ptr;
    s32 i, j;

    ptr = gCrashScreen.framebuffer + gScreenWidth * y + x;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            // 0xe738 = 0b1110011100111000
            *ptr = ((*ptr & 0xe738) >> 2) | 1;
            ptr++;
        }
        ptr += gScreenWidth - w;
    }
}

void crash_screen_draw_glyph(s32 x, s32 y, s32 glyph) {
    const u32 *data;
    u16 *ptr;
    u32 bit;
    u32 rowMask;
    s32 i, j;

    data = &gCrashScreenFont[glyph / 5 * 7];
    ptr = gCrashScreen.framebuffer + gScreenWidth * y + x;

    for (i = 0; i < 7; i++) {
        bit = 0x80000000U >> ((glyph % 5) * 6);
        rowMask = *data++;

        for (j = 0; j < 6; j++) {
            if ((bit & rowMask)) {
                *ptr = 0xFFFF;
            }
            ptr++;
            bit >>= 1;
        }
        ptr += gScreenWidth - 6;
    }
}

void crash_screen_sleep(s32 ms) {
    u32 cycles = ms * 1000 * osClockRate / 1000000U;
    osSetTime(0);
    while (osGetTime() < cycles) {}
}

void crash_screen_print(s32 x, s32 y, const char *fmt, ...) {
    char *ptr;
    char buf[127];
    u32 glyph;
    va_list args;

    va_start(args, fmt);
    if (_Printf(write_to_buf, buf, fmt, args) > 0) {
        ptr = buf;
        while (*ptr) {
            glyph = gCrashScreenCharToGlyph[*ptr & 0x7f];
            if (glyph != 0xff) {
                crash_screen_draw_glyph(x, y, glyph);
            }
            ptr++;
            x += 6;
        }
    }
    va_end(args);
}

void crash_screen_print_float_reg(s32 x, s32 y, s32 regNum, void *addr) {
    u32 bits;
    s32 exponent;

    bits = *(u32 *) addr;
    exponent = ((bits & 0x7f800000U) >> 0x17) - 0x7f;
    if ((exponent >= -0x7e && exponent <= 0x7f) || bits == 0) {
        crash_screen_print(x, y, "F%02d:%2.4f", regNum, (f64) * (f32 *) addr);
    } else {
        crash_screen_print(x, y, "F%02d:---------", regNum);
    }
}

void crash_screen_print_fpcsr(u32 fpcsr) {
    s32 i;
    u32 bit;

    bit = 1 << 17;
    crash_screen_print(sCrashX + 10, sCrashY + 130, "FPCSR:%08XH", fpcsr);
    for (i = 0; i < 6; i++) {
        if (fpcsr & bit) {
            crash_screen_print(sCrashX + 110, sCrashY + 135, "(%s)", gFpcsrDesc[i]);
            return;
        }
        bit >>= 1;
    }
}

char *sGPRegisterNames[] = {
    "at", "v0", "v1", "v2", "a0", "a1", "a2",
    "a3", "t0", "t1", "t2", "t3", "t4", "t5",
    "t6", "t7", "s0", "s1", "s2", "s3", "s4",
    "s5", "s6", "s7", "t8", "t9",
};

void crash_page_registers(OSThread *thread) {
    s32 cause;
    __OSThreadContext *tc = &thread->context;
    s32 i;
    s32 y;
    u64 *reg;
    f32 *reg2;

    cause = (tc->cause >> 2) & 0x1F;
    if (cause == 23) { // EXC_WATCH
        cause = 16;
    } else if (cause == 31) { // EXC_VCED
        cause = 17;
    }
    crash_screen_draw_rect(sCrashX, sCrashY, 270, 205);
    crash_screen_print(sCrashX + 10, sCrashY + 5, "THREAD:%d  (%s)", thread->id, gCauseDesc[cause]);

#ifdef DETAILED_CRASH
    crash_screen_print(sCrashX + 10, sCrashY + 15, "PC:%08XH   SR:%08XH   GP:%08XH", (u32) tc->pc, (u32) tc->sr, (u32) tc->gp);
    crash_screen_print(sCrashX + 10, sCrashY + 25, "VA:%08XH   RA:%08XH   SP:%08XH", (u32) tc->badvaddr, (u32) tc->ra, (u32) tc->sp);
    reg = (u64 *) tc;
    y = sCrashY - sCrashScroll + 40;
    for (i = 0; i < 26; i++) {
        if (y < sCrashY + 40 || y > gScreenHeight - 30) {
            y += 10;
            continue;
        }
        crash_screen_print(sCrashX + 10, y, "%s:     %d", sGPRegisterNames[i], (u32) reg[i]);
        crash_screen_print(sCrashX + 150, y, "0x%X", (u32) reg[i]);
        y += 10;
    }
    reg2 = (f32 *) &tc->fp0;
    for (i = 0; i < 32; i += 2) {
        if (y < sCrashY + 40 || y > gScreenHeight - 30) {
            y += 10;
            continue;
        }
        crash_screen_print_float_reg(sCrashX + 30, y, i, (f64 *) (f32 *) &reg2[i]);
        crash_screen_print_float_reg(sCrashX + 160, y, i + 1, (f64 *) (f32 *) &reg2[i + 1]);
        y += 10;
    }
    sCrashMaxScroll = 260;
#else
    crash_screen_print(sCrashX + 10, sCrashY + 15, "PC:%08XH   SR:%08XH   VA:%08XH", tc->pc, tc->sr, tc->badvaddr);
    crash_screen_print(sCrashX + 10, sCrashY + 30, "AT:%08XH   V0:%08XH   V1:%08XH", (u32) tc->at, (u32) tc->v0,
    crash_screen_print(sCrashX + 10, sCrashY + 30, "AT:%08XH   V0:%08XH   V1:%08XH", (u32) tc->at, (u32) tc->v0,
                       (u32) tc->v1);
    crash_screen_print(sCrashX + 10, sCrashY + 40, "A0:%08XH   A1:%08XH   A2:%08XH", (u32) tc->a0, (u32) tc->a1,
                       (u32) tc->a2);
    crash_screen_print(sCrashX + 10, sCrashY + 50, "A3:%08XH   T0:%08XH   T1:%08XH", (u32) tc->a3, (u32) tc->t0,
                       (u32) tc->t1);
    crash_screen_print(sCrashX + 10, sCrashY + 60, "T2:%08XH   T3:%08XH   T4:%08XH", (u32) tc->t2, (u32) tc->t3,
                       (u32) tc->t4);
    crash_screen_print(sCrashX + 10, sCrashY + 70, "T5:%08XH   T6:%08XH   T7:%08XH", (u32) tc->t5, (u32) tc->t6,
                       (u32) tc->t7);
    crash_screen_print(sCrashX + 10, sCrashY + 80, "S0:%08XH   S1:%08XH   S2:%08XH", (u32) tc->s0, (u32) tc->s1,
                       (u32) tc->s2);
    crash_screen_print(sCrashX + 10, sCrashY + 90, "S3:%08XH   S4:%08XH   S5:%08XH", (u32) tc->s3, (u32) tc->s4,
                       (u32) tc->s5);
    crash_screen_print(sCrashX + 10, sCrashY + 100, "S6:%08XH   S7:%08XH   T8:%08XH", (u32) tc->s6, (u32) tc->s7,
                       (u32) tc->t8);
    crash_screen_print(sCrashX + 10, sCrashY + 110, "T9:%08XH   GP:%08XH   SP:%08XH", (u32) tc->t9, (u32) tc->gp,
                       (u32) tc->sp);
    crash_screen_print(sCrashX + 10, sCrashY + 120, "S8:%08XH", (u32) tc->s8);
    crash_screen_print_fpcsr(tc->fpcsr);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 145, 0, &tc->fp0.f.f_even);
    crash_screen_print_float_reg(sCrashX + 100, sCrashY + 145, 2, &tc->fp2.f.f_even);
    crash_screen_print_float_reg(sCrashX + 190, sCrashY + 145, 4, &tc->fp4.f.f_even);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 155, 6, &tc->fp6.f.f_even);
    crash_screen_print_float_reg(sCrashX + 100, sCrashY + 155, 8, &tc->fp8.f.f_even);
    crash_screen_print_float_reg(sCrashX + 190, sCrashY + 155, 10, &tc->fp10.f.f_even);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 165, 12, &tc->fp12.f.f_even);
    crash_screen_print_float_reg(sCrashX + 100, sCrashY + 165, 14, &tc->fp14.f.f_even);
    crash_screen_print_float_reg(sCrashX + 190, sCrashY + 165, 16, &tc->fp16.f.f_even);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 175, 18, &tc->fp18.f.f_even);
    crash_screen_print_float_reg(sCrashX + 100, sCrashY + 175, 20, &tc->fp20.f.f_even);
    crash_screen_print_float_reg(sCrashX + 190, sCrashY + 175, 22, &tc->fp22.f.f_even);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 185, 24, &tc->fp24.f.f_even);
    crash_screen_print_float_reg(sCrashX + 100, sCrashY + 185, 26, &tc->fp26.f.f_even);
    crash_screen_print_float_reg(sCrashX + 190, sCrashY + 185, 28, &tc->fp28.f.f_even);
    crash_screen_print_float_reg(sCrashX + 10, sCrashY + 195, 30, &tc->fp30.f.f_even);
#endif
}

#ifdef DETAILED_CRASH
void crash_page_object(void) {
    crash_screen_draw_rect(sCrashX, sCrashY, 270, 205);
    if (sCrashObjID) {
        Object *o = sCrashObjID;
        crash_screen_print(sCrashX + 10, sCrashY + 5, "OBJ: %s (ID: %d) DURING %s", o->segment.header->internalName,
                           o->behaviorId, sObjectStrings[sCrashObjAct - 1]);
        crash_screen_print(sCrashX + 10, sCrashY + 15, "X: %2.2f, Y: %2.2f, Z: %2.2f",
                           (f64) o->segment.trans.x_position, (f64) o->segment.trans.y_position,
                           (f64) o->segment.trans.z_position);
        crash_screen_print(sCrashX + 10, sCrashY + 25, "Y: 0x%X, P: 0x%X, R: 0x%X", (u16) o->segment.trans.y_rotation,
                           (u16) o->segment.trans.x_rotation, (u16) o->segment.trans.z_rotation);
        crash_screen_print(sCrashX + 10, sCrashY + 35, "FLAGS: %X, SCALE: %2.3f", o->segment.trans.flags,
                           (f64) o->segment.trans.scale);
        crash_screen_print(sCrashX + 10, sCrashY + 50, "MTX POS: %d", gModelMatrixStackPos);
    } else {
        crash_screen_print(sCrashX + 10, sCrashY + 5, "NO EXTRA INFORMATION");
    }
}

#ifdef PUPPYPRINT_DEBUG
void crash_page_log(void) {
    s32 i;
    s32 y = 0;
    crash_screen_draw_rect(sCrashX, sCrashY, 270, 205);
    for (i = NUM_LOG_LINES; i > 0; i--) {
        if (gPuppyPrint.logText[i][0] == NULL) {
            continue;
        }
        crash_screen_print(sCrashX + 10, sCrashY + 5 + y, "%s", gPuppyPrint.logText[i]);
        y += 10;
    }
}
#endif
#endif

void crash_page_assert(void) {
    crash_screen_draw_rect(sCrashX, sCrashY, 270, 205);
    if (gAssert) {
        crash_screen_print(sCrashX + 10, sCrashY + 5, "ASSERT TRIPPED");
        crash_screen_print(sCrashX + 10, sCrashY + 20, gAssertString);
    } else {
        crash_screen_print(sCrashX + 10, sCrashY + 5, "NO ASSERT TO REPORT");
    }
}

void crash_page_memory(void) {
    s32 i;
    s32 y;
    crash_screen_draw_rect(sCrashX, sCrashY, 270, 205);
    crash_screen_print(sCrashX + 10, sCrashY + 5, "Free: 0x%06X (%2.2f%%)", TOTALRAM - gPuppyPrint.ramPools[MEMP_OVERALL] - gPuppyPrint.ramPools[MEMP_CODE],
                (f64) (((f32) (TOTALRAM - gPuppyPrint.ramPools[MEMP_OVERALL] - gPuppyPrint.ramPools[MEMP_CODE]) / (f32) TOTALRAM) * 100.0f));
    
    sCrashMaxScroll = -((gScreenHeight - 24));
    y = sCrashY + 5 - sCrashScroll;
    for (i = 1; i < MEMP_TOTAL + 12; i++) {
        if (gPuppyPrint.ramPools[sRAMPrintOrder[i]] == 0) {
            continue;
        }
        sCrashMaxScroll += 10;
        if (y < 24 || y > gScreenHeight - 16) {
            y += 10;
            continue;
        }
        crash_screen_print(sCrashX + 10, y, "%02X %s", i, sPuppyprintMemColours[sRAMPrintOrder[i]]);
        crash_screen_print(sCrashX + 120, y, "0x%X (%2.2f%%)", gPuppyPrint.ramPools[sRAMPrintOrder[i]],
        (f64) (((f32) gPuppyPrint.ramPools[sRAMPrintOrder[i]] / (f32) TOTALRAM) * 100.0f));
        y += 10;
    }
}

extern OSThread *__osFaultedThread;

OSThread *get_crashed_thread(void) {
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

#ifdef DETAILED_CRASH
void crash_screen_input(void) {
    s32 i;
    s32 prevScroll = sCrashScroll;
    for (i = 0; i < 4; i++) {
        if (get_buttons_pressed_from_player(i) & (Z_TRIG | L_TRIG)) {
            sCrashPage--;
            sCrashScroll = 0;
            if (sCrashPage == 255) {
                sCrashPage = CRASH_PAGE_COUNT - 1;
            }
            sCrashUpdate = TRUE;
        } else if (get_buttons_pressed_from_player(i) & R_TRIG) {
            sCrashPage++;
            sCrashScroll = 0;
            if (sCrashPage >= CRASH_PAGE_COUNT) {
                sCrashPage = 0;
            }
            sCrashUpdate = TRUE;
        }
        if (sCrashUpdate && sCrashPage == CRASH_PAGE_REGISTERS) {
            s32 cause;
            OSThread *thread = get_crashed_thread();
            __OSThreadContext *tc = &thread->context;
            cause = (tc->cause >> 2) & 0x1F;
            if (cause == 15) {
                sCrashScroll = 260;
            }
        }
        if (sCrashPage == CRASH_PAGE_MEMORY) {
            if (get_buttons_pressed_from_player(i) & U_JPAD) {
                sCrashScroll -= 10;
                if (sCrashScroll < 0) {
                    sCrashScroll = 0;
                }
            } else if (get_buttons_pressed_from_player(i) & D_JPAD) {
                sCrashScroll += 10;
                if (sCrashScroll > sCrashMaxScroll) {
                    sCrashScroll = sCrashMaxScroll;
                }
            }
        } else if (sCrashPage == CRASH_PAGE_REGISTERS) {
            if (get_buttons_pressed_from_player(i) & U_JPAD) {
                sCrashScroll -= 10;
                if (sCrashScroll < 0) {
                    sCrashScroll = 0;
                }
            } else if (get_buttons_pressed_from_player(i) & D_JPAD) {
                sCrashScroll += 10;
                if (sCrashScroll > sCrashMaxScroll) {
                    sCrashScroll = sCrashMaxScroll;
                }
            }
        }
    }
    if (prevScroll != sCrashScroll) {
        sCrashUpdate = TRUE;
    }
}
#endif

void draw_crash_screen(OSThread *thread) {
#ifdef DETAILED_CRASH
    crash_screen_input();
#endif

    if (sCrashUpdate == FALSE) {
        return;
    }

    //wcopy(gVideoCurrDepthBuffer, gCrashScreen.framebuffer, (gScreenWidth * gScreenHeight) * 2);

    sCrashX = (SCREEN_WIDTH - 270) / 2;
    sCrashY = (SCREEN_HEIGHT - 205) / 2;

    switch (sCrashPage) {
        case CRASH_PAGE_REGISTERS:
            crash_page_registers(thread);
            break;
        case CRASH_PAGE_ASSERT:
            crash_page_assert();
            break;
#ifdef DETAILED_CRASH
        case CRASH_PAGE_OBJECT:
            crash_page_object();
            break;
#ifdef PUPPYPRINT_DEBUG
        case CRASH_PAGE_LOG:
            crash_page_log();
            break;
        case CRASH_PAGE_MEMORY:
            crash_page_memory();
            break;
#endif
#endif
    }

    osWritebackDCacheAll();
    osViBlack(FALSE);
    change_vi(&gGlobalVI, SCREEN_WIDTH, SCREEN_HEIGHT);
    osViSwapBuffer(gCrashScreen.framebuffer);
    gVideoCurrFbIndex ^= 1;
    if (gVideoCurrFramebuffer) {
        gCrashScreen.framebuffer = (u16 *) gVideoFramebuffers[gVideoCurrFbIndex];
    }
    sCrashUpdate = FALSE;
}

extern OSThread gThread3;
extern OSThread gThread30;

void thread2_crash_screen(UNUSED void *arg) {
    OSMesg mesg;
    OSThread *thread = NULL;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashScreen.mesgQueue, (OSMesg) 1);
    osSetEventMesg(OS_EVENT_FAULT, &gCrashScreen.mesgQueue, (OSMesg) 2);

    gAssert = FALSE;
    osRecvMesg(&gCrashScreen.mesgQueue, &mesg, OS_MESG_BLOCK);
    switch ((s32) mesg) {
    case MESG_RSP_AUD_HUNG:
        puppyprint_assert_nonblocking("RSP (Aud) stopped responding!");
        break;
    case MESG_RSP_GFX_HUNG:
        puppyprint_assert_nonblocking("RSP (Gfx) stopped responding!");
        break;
    case MESG_RDP_HUNG:
        puppyprint_assert_nonblocking("RDP stopped responding!");
        break;
    case MESG_TASK_FAILED:
        puppyprint_assert_nonblocking("Sched is ded :()");
        break;
    default:
        thread = get_crashed_thread();
        if (thread == NULL) {
            puppyprint_assert_nonblocking("No thread found???");
        } else {
            if (thread->id == 4) { // Audio thread crashed, so go top prio.
                gCrashScreen.thread.priority = 255;
            } else {
                gCrashScreen.thread.priority = 11;
                sound_play(SOUND_VOICE_BANJO_WOAH, NULL);
                music_play(SEQUENCE_NONE);
            }
        }
        break;
    }
    if (gVideoCurrFramebuffer) {
        gCrashScreen.framebuffer = (u16 *) gVideoFramebuffers[gVideoCurrFbIndex];
    }
    gVideoCurrFbIndex = 0;
    osStopThread(&gThread3);
    osStopThread(&gThread30);
    if (gAssert) {
        sCrashPage = CRASH_PAGE_ASSERT;
    }
    sCrashUpdate = TRUE;
    wcopy(gVideoCurrFramebuffer, gVideoCurrDepthBuffer, (gScreenWidth * gScreenHeight) * 2);
    crash_screen_sleep(500);
    calculate_ram_print_order();
    while (TRUE) {
        handle_save_data_and_read_controller(0, LOGIC_30FPS);
        draw_crash_screen(thread);
    }
}

void crash_screen_init(void) {
    osCreateMesgQueue(&gCrashScreen.mesgQueue, &gCrashScreen.mesg, 1);
    osCreateThread(&gCrashScreen.thread, 2, thread2_crash_screen, NULL,
                   (u8 *) gCrashScreen.stack + sizeof(gCrashScreen.stack), 15);
    osStartThread(&gCrashScreen.thread);
}

#ifdef DETAILED_CRASH
void set_crash_object(Object *objectID, s32 act) {
    sCrashObjID = objectID;
    sCrashObjAct = act;
}
#endif
