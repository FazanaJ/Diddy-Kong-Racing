#ifndef _OVERLAY_H_
#define _OVERLAY_H_

#include "types.h"

void overlay_init(void);
void *overlay_symbol(s32 overlayID, const char *symbol);
void *overlay_load(s32 overlayID);
void *overlay_load_dep(s32 curOvlID, s32 newOvlID);
void overlay_free(s32 overlayID);
s32 overlay_run(s32 overlayID, const char *funcName);

#endif