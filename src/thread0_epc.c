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

/************ .rodata ************/

UNUSED const char D_800E8E70[] = "\nAssertion failed: '%s' in file %s, line %d\n";
UNUSED const char D_800E8EA0[] = "\nAssertion failed: '%s' in file %s, line %d\n";

/*********************************/

/************ .data ************/

s32 sLockupStatus = -1;
s32 sLockupPage = EPC_PAGE_REGISTER;
s32 sLockupDelay = 0;

/*******************************/

u64 gEPCStack[0x200];
OSThread gEPCThread;
OSMesgQueue D_80129790;
OSMesg D_801297A8[8];
OSMesg D_801297C8[8];
OSMesgQueue D_801297E8;

/**
 * Start the exception program counter thread.
 * Official Name: diCpuTraceInit
 */
void thread0_create(void) {
    s32 i;

    osCreateThread(&gEPCThread, 0, thread0_Main, 0, &gEPCStack[0x200], OS_PRIORITY_MAX);
    osStartThread(&gEPCThread);
    for (i = 0; i < 3; i++) {
        gObjectStackTrace[i] = -1;
    }
}

/**
 * Main thread for the epc lockup screen. Thread 0.
 */
void thread0_Main(UNUSED void *unused) {
    s32 sp34;
    s32 s0 = 0;

    osCreateMesgQueue(&D_80129790, D_801297A8, ARRAY_COUNT(D_801297A8));
    osSetEventMesg(OS_EVENT_FAULT, &D_80129790, (OSMesg) 8);
    osSetEventMesg(OS_EVENT_CPU_BREAK, &D_80129790, (OSMesg) 2);
    osCreatePiManager(150, &D_801297E8, D_801297C8, ARRAY_COUNT(D_801297C8));

    while (1) {
        osRecvMesg(&D_80129790, (OSMesg) &sp34, OS_MESG_BLOCK);
#if VERSION < VERSION_80
        if (!(get_filtered_cheats() & CHEAT_EPC_LOCK_UP_DISPLAY)) {
            continue;
        }
        s0 |= sp34;
        if ((s0 & 8) == 0 && (s0 & 2) == 0) {
            continue;
        }
#endif
        s0 &= ~8;
        stubbed_printf(">fault< ");
        enable_interupts_on_main();
        stop_all_threads_except_main();
        write_epc_data_to_cpak();
    }
}

/**
 * Enable interrupts on all idle priority threads, which should just be the main thread 1.
 */
void enable_interupts_on_main(void) {
    OSThread *node = __osGetActiveQueue();
    while (node->priority != -1) {
        if (node->priority == OS_PRIORITY_IDLE) {
            // Clear all existing interrupts and disable them.
            node->context.sr &= ~(SR_IMASK | SR_IE);

            // Enable interrupts
            // IP3 Int1 pin (N64: Cartridge)
            // IP4 Int2 pin (N64: Pre-NMI (Reset button))
            // IP6 Int4 pin (N64: RDB Write)
            // IP7 Timer interrupt
            node->context.sr |= (SR_IBIT7 | SR_IBIT6 | SR_IBIT4 | SR_IBIT3 | SR_IE);
            break;
        }
        node = node->tlnext;
    }
}

/**
 * Stop all threads except for the main thread 1
 */
void stop_all_threads_except_main(void) {
    OSThread *node = __osGetActiveQueue();
    while (node->priority != -1) {
        if (node->priority > OS_PRIORITY_IDLE && node->priority <= OS_PRIORITY_APPMAX) {
            osStopThread((OSThread *) &node->next);
        }
        node = node->tlnext;
    }
}

/**
 * Writes epc data to the controller pak so that it can be read back later.
 */
void write_epc_data_to_cpak(void) {
    OSThread *thread;
    UNUSED s16 sp444[0x200];
    u8 sp244[0x200];
    u8 sp44[0x200];
    s16 *v0;
    s32 i;
    s32 currentCount;
    s32 maxCount;
    u8 zero;

    for (thread = __osGetActiveQueue(); thread->priority != -1; thread = thread->tlnext) {
        if (thread->priority > OS_PRIORITY_IDLE && thread->priority <= OS_PRIORITY_APPMAX) {
            if (thread->flags & 2 || thread->flags & 1) {
                break;
            }
        }
    }

    if (thread->priority != -1) {
        thread->context.fp0.f.f_odd = gObjectStackTrace[0];
        thread->context.fp0.f.f_even = gObjectStackTrace[1];
        thread->context.fp2.f.f_odd = gObjectStackTrace[2];
        bcopy(thread, sp44, sizeof(epcInfo));
        bcopy((void *) (u32) thread->context.sp, sp244, sizeof(sp244));
        zero = 0; // Why is this needed to match?
        v0 = func_80024594(&currentCount, &maxCount);
        for (i = zero; i < maxCount; i++) {
            sp444[i] = v0[currentCount];
            currentCount--;
            if (currentCount < zero) {
                currentCount += maxCount;
            }
        }
        write_controller_pak_file(0, -1, "CORE", "", sp44, sizeof(sp44) + sizeof(sp244) + sizeof(sp444));
    }
    while (1) {} // Infinite loop
}

/**
 * Writes epc data to the controller pak when a memory allocation has failed,
 * but only if CHEAT_EPC_LOCK_UP_DISPLAY is active.
 */
void dump_memory_to_cpak(s32 epc, s32 size, u32 colourTag) {
    epcInfo epcinfo;
    s16 sp440[0x200];
    u8 sp240[0x200];
    u8 sp40[0x200];
    s16 *v0;
    s32 currentCount;
    s32 i;
    UNUSED s32 pad;

    // This is checking if the EPC cheat is active
    if (get_filtered_cheats() & CHEAT_EPC_LOCK_UP_DISPLAY) {
        bzero(&epcinfo, sizeof(epcInfo));
        epcinfo.epc = epc & 0xFFFFFFFFFFFFFFFF; // fakematch
        epcinfo.a0 = size;
        epcinfo.a1 = colourTag;
        epcinfo.cause = -1;
        epcinfo.objectStackTrace[0] = gObjectStackTrace[0];
        epcinfo.objectStackTrace[1] = gObjectStackTrace[1];
        epcinfo.objectStackTrace[2] = gObjectStackTrace[2];
        bcopy(&epcinfo, &sp40, sizeof(epcInfo));
        bzero(&sp240, sizeof(sp240));
        v0 = func_80024594(&currentCount, &size);
        for (i = 0; i < size; i++) {
            sp440[i] = v0[currentCount];
            currentCount--;
            if (currentCount < 0) {
                currentCount += size;
            }
        }
        i = sizeof(sp40) + sizeof(sp240) + sizeof(sp440); // fakematch?
        write_controller_pak_file(0, -1, "CORE", "", sp40, i);
        while (1) {} // Infinite loop; waiting for the player to reset the console?
    }
}

/**
 * Mark the object type given, so if the game crashes while processing it, the debug screen will tell you which object
 * ID is to blame. Split into three sections, for spawning an object, updating an object and for rendering an object.
 */
void update_object_stack_trace(s32 index, s32 value) {
    if (index >= OBJECT_SPAWN && index <= OBJECT_DRAW) {
        gObjectStackTrace[index] = value;
    }
}

/**
 * Called as a check to see if render_epc_lock_up_display should be called.
 */
s32 get_lockup_status(void) {
    s32 fileNum;
    s32 controllerIndex = 0;
    s64 sp420[128]; // Overwrite epcStack?
    s64 sp220[64];
    u8 dataFromControllerPak[512]; // Looks to be sizeof(epcInfo), aligned to 64
    extern epcInfo gEpcInfo;
    extern s32 D_801299B0[64];

    if (sLockupStatus != -1) {
        return sLockupStatus;
    } else {
        sLockupStatus = 0;
        // Looks like it reads EpcInfo data from the controller pak, which is interesting
        if ((get_si_device_status(controllerIndex) == CONTROLLER_PAK_GOOD) &&
            (get_file_number(controllerIndex, "CORE", "", &fileNum) == CONTROLLER_PAK_GOOD) &&
            (read_data_from_controller_pak(controllerIndex, fileNum, dataFromControllerPak, 0x800) ==
             CONTROLLER_PAK_GOOD)) {
            bcopy(&dataFromControllerPak, &gEpcInfo, sizeof(epcInfo));
            bcopy(&sp220, &D_801299B0, sizeof(sp220));
            bcopy(&sp420, &D_80129BB0, sizeof(sp420));
            sLockupStatus = 1;
        }
        start_reading_controller_data(controllerIndex);
        if (sLockupStatus) {
            delete_file(controllerIndex, fileNum);
        }
    }

    return sLockupStatus;
}

/**
 * Counts up. Automatically switches page every second.
 * Official name: diCpuTraceTick
 */
void mode_lockup(s32 updateRate) {
    sLockupDelay += updateRate;
    if (sLockupDelay > 60) {
        sLockupDelay = 0;
        sLockupPage++;
    }
}

#define GET_REG(reg) (s32) gEpcInfo.reg

/**
 * Draw onscreen the four pages of the crash screen.
 * Page 0 shows the address of the instruction that crashed as well as fixed point registers
 * Page 1-3 show the stack dump of the crashed thread.
 * Page 4 appears to show the data of the EPC stack itself?
 */
void render_epc_lock_up_display(void) {
    u16 *temp;
    char *objStatusString[3] = { "setup", "control", "print" };
    s32 offset;
    s32 s3;
    s32 j;
    s32 i;
    static epcInfo gEpcInfo;
    static s32 D_801299B0[64];

    s3 = 0;

    set_render_printf_position(16, 32);

    switch (sLockupPage) {
        case EPC_PAGE_REGISTER:
            gObjectStackTrace[OBJECT_SPAWN] = gEpcInfo.objectStackTrace[OBJECT_SPAWN];
            gObjectStackTrace[OBJECT_UPDATE] = gEpcInfo.objectStackTrace[OBJECT_UPDATE];
            gObjectStackTrace[OBJECT_DRAW] = gEpcInfo.objectStackTrace[OBJECT_DRAW];
            if (((u32 *) gEpcInfo.objectStackTrace)[-4] == -1U) { // TODO: find better solution
                render_printf(" epc\t\t0x%08x\n", gEpcInfo.epc);
                render_printf(" cause\t\tmmAlloc(%d,0x%8x)\n", GET_REG(a0), GET_REG(a1));
                for (i = 0; i < 3; i++) {
                    if (gObjectStackTrace[i] != OBJECT_CLEAR) {
                        if (!s3) {
                            s3 = TRUE;
                            render_printf(" object\t\t");
                        }
                        render_printf("%s %d ", objStatusString[i], gObjectStackTrace[i]);
                    }
                }
                render_printf("\n");
                render_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                break;
            }
            render_printf(" Fault in thread %d\n", gEpcInfo.thread[0]);
            render_printf(" epc\t\t0x%08x\n", gEpcInfo.epc);
            render_printf(" cause\t\t0x%08x\n", gEpcInfo.cause);
            render_printf(" sr\t\t0x%08x\n", gEpcInfo.sr);
            render_printf(" badvaddr\t0x%08x\n", gEpcInfo.badvaddr);
            for (i = 0; i < 3; i++) {
                if (gObjectStackTrace[i] != OBJECT_CLEAR) {
                    if (!s3) {
                        s3 = TRUE;
                        render_printf(" object\t\t");
                    }
                    render_printf("%s %d ", objStatusString[i], gObjectStackTrace[i]);
                }
            }
            render_printf("\n");
            render_printf(" at 0x%08x v0 0x%08x v1 0x%08x\n", GET_REG(at), GET_REG(v0), GET_REG(v1));
            render_printf(" a0 0x%08x a1 0x%08x a2 0x%08x\n", GET_REG(a0), GET_REG(a1), GET_REG(a2));
            render_printf(" a3 0x%08x t0 0x%08x t1 0x%08x\n", GET_REG(a3), GET_REG(t0), GET_REG(t1));
            render_printf(" t2 0x%08x t3 0x%08x t4 0x%08x\n", GET_REG(t2), GET_REG(t3), GET_REG(t4));
            render_printf(" t5 0x%08x t6 0x%08x t7 0x%08x\n", GET_REG(t5), GET_REG(t6), GET_REG(t7));
            render_printf(" s0 0x%08x s1 0x%08x s2 0x%08x\n", GET_REG(s0), GET_REG(s1), GET_REG(s2));
            render_printf(" s3 0x%08x s4 0x%08x s5 0x%08x\n", GET_REG(s3), GET_REG(s4), GET_REG(s5));
            render_printf(" s6 0x%08x s7 0x%08x t8 0x%08x\n", GET_REG(s6), GET_REG(s7), GET_REG(t8));
            render_printf(" t9 0x%08x gp 0x%08x sp 0x%08x\n", GET_REG(t9), GET_REG(gp), GET_REG(sp));
            render_printf(" s8 0x%08x ra 0x%08x\n\n", GET_REG(s8), GET_REG(ra));
            break;
        case EPC_PAGE_STACK_TOP:
        case EPC_PAGE_STACK_MIDDLE:
        case EPC_PAGE_STACK_BOTTOM:
            offset = (sLockupPage - 1) * 48;
            for (j = (s32) &D_801299B0[offset], i = 0; i < 16; i++) {
                render_printf("   %08x %08x %08x\n", ((u8 **) j)[0], ((u8 **) j)[16], ((u8 **) j)[32]);
                j = (s32) ((s32 *) j + 1);
            }
            break;
        case EPC_PAGE_UNK04:
            offset = (sLockupPage - 4) * 128;
            for (temp = (u16 *) &D_80129BB0[offset], i = 0; i < 16; i++) {
                render_printf("  ");
                for (j = 0; j < 8; j++) {
                    render_printf("%04x ", temp[0]);
                    temp++;
                }
                render_printf("\n");
            }
            break;
        case EPC_PAGE_EXIT:
            sLockupPage = 0;
            return;
        default:
            return;
    }
}

#include "video.h"
#include "string.h"
#include "stdarg.h"

u64 gCrashThreadStack[0x200];
OSThread gCrashThread;
OSMesgQueue gCrashQueue;
OSMesg gCrashQueueBuf[2];
u16 *gCrashFB;
char *gCrashFuncName;

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

char *gCauseDesc[18] = {
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

char *gFpcsrDesc[6] = {
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

/**
 * Draw a rectangle on screen, supports arbitrary colour and alpha
*/
void crash_rectangle(s32 x, s32 y, s32 w, s32 h, s32 r, s32 g, s32 b, s32 a) {
    u16 *ptr;
    s32 i, j;

    u8 alpha = a;

    ptr = gCrashFB + gScreenWidth * y + x;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            u16 dst = *ptr;

            u8 dr = (dst >> 11) & 0x1F;
            u8 dg = (dst >> 6) & 0x1F;
            u8 db = (dst >> 1) & 0x1F;

            dr = (dr << 3) | (dr >> 2);
            dg = (dg << 3) | (dg >> 2);
            db = (db << 3) | (db >> 2);

            u8 br = ((r * alpha) + (dr * (255 - alpha))) / 255;
            u8 bg = ((g * alpha) + (dg * (255 - alpha))) / 255;
            u8 bb = ((b * alpha) + (db * (255 - alpha))) / 255;

            u16 blended = ((br & 0xF8) << 8) | ((bg & 0xF8) << 3) | ((bb & 0xF8) >> 2) | 1;

            *ptr++ = blended;
        }
        ptr += gScreenWidth - w;
    }
}

/**
 *  Draws a line using Bresenham's algorithm. Supports arbitrary colour and alpha
*/
void crash_line(s32 x0, s32 y0, s32 x1, s32 y1, s32 r, s32 g, s32 b, s32 a) {
    s32 dx = abs(x1 - x0);
    s32 dy = abs(y1 - y0);
    s32 sx = (x0 < x1) ? 1 : -1;
    s32 sy = (y0 < y1) ? 1 : -1;
    s32 err = dx - dy;

    while (1) {
        // Draw the current pixel
        if (x0 >= 0 && x0 < gScreenWidth && y0 >= 0 && y0 < gScreenHeight) {
            u16 *ptr = gCrashFB + gScreenWidth * y0 + x0;
            u16 dst = *ptr;

            // Extract RGB components from the destination color
            u8 dr = (dst >> 11) & 0x1F;
            u8 dg = (dst >> 6) & 0x1F;
            u8 db = (dst >> 1) & 0x1F;

            // Convert 5-bit components to 8-bit
            dr = (dr << 3) | (dr >> 2);
            dg = (dg << 3) | (dg >> 2);
            db = (db << 3) | (db >> 2);

            // Blend the colors
            u8 br = ((r * a) + (dr * (255 - a))) / 255;
            u8 bg = ((g * a) + (dg * (255 - a))) / 255;
            u8 bb = ((b * a) + (db * (255 - a))) / 255;

            // Convert back to 5-bit components and combine into a 16-bit color
            u16 blended = ((br & 0xF8) << 8) | ((bg & 0xF8) << 3) | ((bb & 0xF8) >> 2) | 1;

            *ptr = blended;
        }

        // Check if we've reached the end of the line
        if (x0 == x1 && y0 == y1) {
            break;
        }

        // Update the error term and coordinates
        s32 e2 = 2 * err;
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
    "BGLoad"
};

s32 crash_thread_name(s32 threadID) {
    s32 id = threadID - 2;
    if (id >= 2) {
        id = 0;
    } else if (id == 28) {
        id = 4;
    }

    return id;
}

/**
 * High chance the framebuffer size doesn't match the crash screens, so copy it with scaling applied.
*/
void framebuffer_scale(u16 *srcFB, u16 *dstFB, s32 srcW, s32 srcH, s32 dstW, s32 dstH) {
    for (s32 y = 0; y < dstH; y++) {
        for (s32 x = 0; x < dstW; x++) {
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

#define CRASH_BORDER_X 20

void crash_render(OSThread *t) {
    s32 i;
    s32 x;
    s32 y;
    u64 *reg;
    f64 *regF;
    s32 cause;
    __OSThreadContext *c = &t->context;
    crash_line(CRASH_BORDER_X - 1, 11, gScreenWidth - CRASH_BORDER_X, 11, 255, 255, 255, 160);
    crash_line(CRASH_BORDER_X - 1, gScreenHeight - 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 12, 255, 255, 255, 160);
    crash_line(CRASH_BORDER_X - 1, 12, CRASH_BORDER_X - 1, gScreenHeight - 13, 255, 255, 255, 160);
    crash_line(gScreenWidth - CRASH_BORDER_X, 12, gScreenWidth - CRASH_BORDER_X, gScreenHeight - 13, 255, 255, 255, 160);
    crash_rectangle(CRASH_BORDER_X, 12, gScreenWidth - (CRASH_BORDER_X * 2), 33, 127, 0, 0, 160);
    
    crash_line(CRASH_BORDER_X, 45, gScreenWidth - CRASH_BORDER_X - 1, 45, 255, 255, 255, 160);
    
    crash_rectangle(CRASH_BORDER_X, 46, gScreenWidth - (CRASH_BORDER_X * 2), gScreenHeight - 58, 0, 0, 0, 160);

    cause = (c->cause >> 2) & 0x1F;
    crash_text(CRASH_BORDER_X + 12, 16, GPACK_RGBA5551(255, 255, 0, 1), "Thread:%s(%d)", sThreadNames[crash_thread_name(t->id)], t->id);
    crash_text(CRASH_BORDER_X + 12 + 100, 16, GPACK_RGBA5551(255, 255, 0, 1), "PC:0#%8X", c->pc);
    crash_text(CRASH_BORDER_X + 12 + 200, 16, GPACK_RGBA5551(255, 255, 0, 1), "RA:0#%8X", c->ra);
    crash_text(CRASH_BORDER_X + 12, 25, GPACK_RGBA5551(255, 255, 0, 1), "Cause:%s", gCauseDesc[cause]);
    if (gCrashFuncName) {
        crash_text(CRASH_BORDER_X + 12, 34, GPACK_RGBA5551(255, 255, 0, 1), "Func Name:%s", gCrashFuncName);
    }

    crash_text(CRASH_BORDER_X + 12, 54, GPACK_RGBA5551(255, 255, 255, 1), "GP:0#%08X", (u32) c->gp);
    crash_text(CRASH_BORDER_X + 12 + 144, 54, GPACK_RGBA5551(255, 255, 255, 1), "SP:0#%08X", (u32) c->sp);
    crash_text(CRASH_BORDER_X + 12, 63, GPACK_RGBA5551(255, 255, 255, 1), "VA:0#%08X", (u32) c->badvaddr);
    crash_text(CRASH_BORDER_X + 12 + 144, 63, GPACK_RGBA5551(255, 255, 255, 1), "SR:0#%08X", (u32) c->sr);
    x = CRASH_BORDER_X + 12;
    y = 72;
    if (cause == 15) {
        regF = (f64 *) &c->fp0;
        for (i = 0; i < 32; i += 1) {
            crash_text(x, y, GPACK_RGBA5551(255, 255, 255, 1), "FPR%02d:%2.4f", i, (f64) regF[i]);
            if ((i % 2) == 0) {
                x += 144;
            } else {
                y += 9;
                x = CRASH_BORDER_X + 12;
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
            if ((i % 2) == 0) {
                x += 144;
            } else {
                y += 9;
                x = CRASH_BORDER_X + 12;
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

    osRecvMesg(&gCrashQueue, &msg, OS_MESG_BLOCK);
    osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
    gCrashFB = gVideoDepthBuffer;
    crash_screen_sleep(500);
    oldW = gScreenWidth;
    oldH = gScreenHeight;
    gScreenWidth = 512;
    gScreenHeight = 240;
    vi_change(gScreenWidth, gScreenHeight);
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