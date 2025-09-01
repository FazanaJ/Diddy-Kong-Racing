#ifndef _BORDERS_H_
#define _BORDERS_H_

#include "PR/gbi.h"

void divider_draw(Gfx **dList);
void divider_clear_coverage(Gfx **dList);
void divider_free(void);

extern void (*gDividerDraw)(Gfx **dList);
extern void (*gDividerClear)(Gfx **dList);

#endif
