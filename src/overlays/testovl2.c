#include "asset_enums.h"
#include "src/printf.h"
#include "src/main.h"

s32 gOverlayVarTest2 = 2;
s32 gOverlayBssTest2;
const char *gOverlayRodataTest2 = "This is a differet overlay!\n";

void we_do_thos_again(void) {
    gOverlayVarTest2++;
    gOverlayBssTest2++;
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
    debug_printf(gOverlayRodataTest2);
}

void secohahahahahahnd_func(void){
    gOverlayVarTest2++;
    gOverlayBssTest2++;
}