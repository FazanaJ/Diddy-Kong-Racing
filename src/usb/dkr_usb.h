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

#ifdef DEBUG
void init_usb_thread(void);
void tick_usb_thread(void);
void render_usb_info(void);
void game_reset(void);
#else
#define init_usb_thread();
#define tick_usb_thread();
#define render_usb_info();
#define game_reset();
#endif

#endif