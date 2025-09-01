#include "asset_enums.h"
#include "src/printf.h"
#include "src/main.h"

s32 gOverlayVarTest = 2;
s32 gOverlayBssTest;

const char gOverlayRodataTest[] = "If you see this, overlays are working!!!!, %d\n";
const char gOverlayRodataTest22[] = "This function also works too I guess.\n";


void second_func(void);

void overlay_run_test(void) {
    gOverlayVarTest++;
    gOverlayBssTest++;
    debug_printf(gOverlayRodataTest, gOverlayVarTest);
    second_func();
}

void second_func(void){
    debug_printf(gOverlayRodataTest22);
    gOverlayVarTest++;
    gOverlayBssTest++;
}