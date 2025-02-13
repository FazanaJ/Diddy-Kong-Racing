#ifndef _DKRUSB_H_
#define _DKRUSB_H_

#include "types.h"
#include "printf.h"

typedef struct  {
    OSMesg msgtype;
    int datatype;
    void* buff;
    int size;
} usbMesg;

void init_usb_thread(void);
void tick_usb_thread(void);
void render_usb_info(void);
#ifdef ENABLE_USB
void debug_printf(const char* message, ...);
#else
#define debug_printf(...)
#endif

#endif

