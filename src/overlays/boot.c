#include "src/main.h"
#include "src/game.h"
#include "src/memory.h"
#include "src/stacks.h"
#include "src/thread0_epc.h"
#include "src/thread3_main.h"
#include "PR/os_internal.h"
#include "PRinternal/piint.h"
#include "src/joypad.h"
#include "src/video.h"
#include "string.h"
#include "src/asset_loading.h"
#include "src/overlay.h"

#define debug_print(x) ((void)(x))

// GLOBAL_ASM

extern u8 __osContPifRam[];
extern u8 __osContLastCmd;
void __osSiGetAccess(void);
void __osSiRelAccess(void);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
u32 get_clockspeed(void);
void skGetId(u32 *arg);
u32 emux_detect(void); // defined in asm/emux.s

void get_pj64_version() {
    u32 *pifRam32;
    s32 i;
    u8 pifCheck;
    const u8 cicTest[] = {
        0x0F, 0x0F,
        0xEC, 0x3C, 0xB6, 0x76, 0xB8, 0x1D, 0xBB, 0x8F,
        0x6B, 0x3A, 0x80, 0xEC, 0xED, 0xEA, 0x5B
    };
    // When calling this function, we know that the emulator is some version of Project 64,
    // and it isn't using the PJ64 4.0 interpreter core. Figure out which version it is.
    
    // PJ64 4.0 dynarec core doesn't update the COUNT register correctly within recompiled functions
    if (get_clockspeed() == 0) {
        gPlatform |= PJ64_4;
        return;
    }

    // Instead of implementing this PIF command correctly, PJ64 versions prior to 3.0 just have
    // a set of hardcoded values for some requests. At least one of these hardcoded values has a
    // typo in it, making it give an incorrect result.
    __osSiGetAccess();
    pifRam32 = (u32*)__osContPifRam;
    for (i = 0; i < 15; i++) pifRam32[i] = 0;
    pifRam32[15] = 2;    

    bcopy(cicTest, &__osContPifRam[46], 17);

    __osSiRawStartDma(OS_WRITE, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    __osContLastCmd = 254;

    __osSiRawStartDma(OS_READ, __osContPifRam);
    osRecvMesg(si_mesg(), NULL, OS_MESG_BLOCK);
    pifCheck = __osContPifRam[54];
    __osSiRelAccess();

    if (pifCheck == 0xB0) {
        gPlatform |= PJ64_1;
    } else {
        gPlatform |= PJ64_3;
    }
}

u8 check_cache_emulation() {
    u8 cacheEmulated;
    // Disable interrupts to ensure that nothing evicts the variable from cache while we're using it.
    u32 saved = __osDisableInt();
    // Create a variable with an initial value of 1. This value will remain cached.
    volatile u8 sCachedValue = 1;
    // Overwrite the variable directly in RDRAM without going through cache.
    // This should preserve its value of 1 in dcache if dcache is emulated correctly.
    *(u8*)(K0_TO_K1(&sCachedValue)) = 0;
    // Read the variable back from dcache, if it's still 1 then cache is emulated correctly.
    // If it's zero, then dcache is not emulated correctly.
    cacheEmulated = sCachedValue;
    // Restore interrupts
    __osRestoreInt(saved);
    return cacheEmulated;
}

void get_platform(void) {
    u32 cf;
    u32 magic;
    u16 halfMagic;
    if ((u32)IO_READ(DPC_PIPEBUSY_REG) | (u32)IO_READ(DPC_TMEM_REG) | (u32)IO_READ(DPC_BUFBUSY_REG)) {
        gPlatform |= emux_detect() ? ARES : CONSOLE;

        if (gPlatform & CONSOLE) {
            if (__osBbIsBb) {
                gPlatform = IQUE | CONSOLE;
                debug_printf("iQue Player detected.\n");
                return;
            } else {
                char region[5];
                if (osTvType == OS_TV_PAL) {
                    sprintf(region, "PAL");
                } else if (osTvType == OS_TV_NTSC) {
                    sprintf(region, "NTSC");
                } else {
                    sprintf(region, "MPAL");
                }
                debug_printf("%s N64 Console detected.\n", region);
            }
        } else {
            debug_printf("Ares N64 Emulator Detected.\n");
        }
        return;
    }

    gPlatform |= EMULATOR;
    cf = get_clockspeed();
    if (cf > 1) {
        gPlatform |= CF_2;
    }

    // Perform a read from unmapped PIF ram.
    // On console and well behaved emulators, this echos back the lower half of
    // the requested memory address, repeating it if a whole word is requested.
    // So in this case, it should result in 0x01040104
    osPiRawReadIo(0x1fd00104u, &magic);
    if (magic == 0u) {
        // Older versions of mupen (and pre-2.12 ParallelN64) just always read 0
        gPlatform |= MUPEN_OLD;
        debug_printf("Mupen64 Emulator Detected.\n");
    } else {
        __osPiGetAccess();
        while (IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY|PI_STATUS_IO_BUSY));
        halfMagic = *((volatile u16*)0xbfd00106u);
        __osPiRelAccess();

        // Now do a halfword read instead.
        switch (halfMagic) {
            // This is the correct result (echo back the lower half of the requested address)
            case 0x0106: {
                // Test to see if the libpl emulator extension is present.
                osPiRawWriteIo(0x1ffb0000u, 0u);
                osPiRawReadIo(0x1ffb0000u, &magic);
                if (magic == 0x00500000u) {
                    // libpl is supported. Must be ParallelN64
                    gPlatform |= PARALLEL_LAUNCHER;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }
                
                // If the cache is emulated, it's Ares
                if (check_cache_emulation()) {
                    gPlatform |= ARES;
                    debug_printf("Ares N64 Emulator Detected.\n");
                    break;
                }

                // its the Project64 4.0 interpreter core
                gPlatform |= PJ64_4;
                debug_printf("Project64 Emulator Detected.\n");
                break;
            }
            // This looks like it should be the expected result considering what we got when we
            // requested the whole word, but that's actually wrong. Later versions of mupen
            // (and the Simple64 fork of it) get this wrong.
            case 0x0104:
                if (check_cache_emulation()) {
                    gPlatform |= SIMPLE64;
                    debug_printf("Simple64 Emulator Detected.\n");
                } else {
                    gPlatform |= MUPEN_NEXT;
                    debug_printf("Mupen64 Emulator Detected.\n");
                }
                break;
            // If reading a word gives the correct response, but reading a halfword always gives 0,
            // then we are dealing with some version of Project 64. Call into this helper function
            // to find out which version we're dealing with.
            case 0x0000:
                get_pj64_version();
                debug_printf("Project64 Emulator Detected.\n");
                break;
            // No known emulator gives any other value. If we somehow manage to get here, just return 0
            default:
                debug_printf("Cannot determine run environment.\n");
                gPlatform = 0;
                break;
        }
    }
    debug_printf("Counter Factor Setting: %d.\n", cf);
} 
