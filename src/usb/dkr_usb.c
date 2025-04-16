#include "config.h"
#include "main.h"

#ifdef DEBUG

#include "dkr_usb.h"
#include "usb.h"
#include "reset.h"
#include "string.h"
#include "stdarg.h"
#include "src/stacks.h"

/*
    TODO:
        * Check for USB timeouts
        * Create functions for sending/recieving data to/from PC
*/

#define RETURN_IF_USB_NOT_VALID() \
    if (usbState != 1)            \
    return
#define RETURN_IF_CART_NOT_VALID() \
    if (cartType <= 0)             \
    return

// Funny number
#define USB_THREAD_ID 69

// Copied from UNFLoader debug.h
#define THREADUSB_PRIORITY 126

#define MSG_FAULT 0x10
#define MSG_READ  0x11
#define MSG_WRITE 0x12
    
#define USBERROR_NONE    0
#define USBERROR_NOTTEXT 1
#define USBERROR_UNKNOWN 2
#define USBERROR_TOOMUCH 3
#define USBERROR_CUSTOM  4
    
#define HASHTABLE_SIZE 7
#define COMMAND_TOKENS 10
#define BUFFER_SIZE    512

// Copied from Thread30
OSThread gThreadUsb;
OSMesgQueue gThreadUsbMesgQueue;
OSMesg gThreadUsbMessage;
u64 gThreadUsbStack[STACKSIZE(STACK_USB)];
ALIGNED16 char debug_buffer[BUFFER_SIZE];
u8 sUSBEnabled = FALSE;
int usbState = -1;
int cartType = -1;
char textBuffer[64];
u8 usbBuffer[512]; // UNFLoader only supports reads/writes of up to 512 bytes.
s32 isHotReloading = FALSE;

void threadusb_loop(UNUSED void *arg);

s32 _Printf(outfun prout, char *dst, const char *fmt, va_list args);

static char *proutSprintf(char *dst, const char *src, size_t count) {
    char *ret = dst;
    bcopy((char *) src, dst, count);
    return ret + count;
}

void debug_printf(const char* message, ...) {
    int len = 0;
    usbMesg msg;
    va_list args;

    if (sUSBEnabled == FALSE) {
        return;
    }
    
    // use the internal libultra printf function to format the string
    va_start(args, message);
    len = _Printf(&proutSprintf, debug_buffer, message, args);
    va_end(args);
    
    // Attach the '\0' if necessary
    if (0 <= len) {
        debug_buffer[len] = '\0';
    }
    
    // Send the printf to the usb thread
    msg.msgtype = (OSMesg) MSG_WRITE;
    msg.datatype = DATATYPE_TEXT;
    msg.buff = debug_buffer;
    msg.size = len;
    osSendMesg(&gThreadUsbMesgQueue, (OSMesg)&msg, OS_MESG_BLOCK);
}

void init_usb_thread(void) {
    usbState = usb_initialize();
    RETURN_IF_USB_NOT_VALID();
    cartType = usb_getcart();
    RETURN_IF_CART_NOT_VALID();
    bzero(&debug_buffer, sizeof(debug_buffer));
    isHotReloading = FALSE;

    // Create USB thread.
    osCreateMesgQueue(&gThreadUsbMesgQueue, &gThreadUsbMessage, 1);
    osCreateThread(&gThreadUsb, USB_THREAD_ID, &threadusb_loop, NULL, &gThreadUsbStack[STACKSIZE(STACK_USB)],
                   THREADUSB_PRIORITY);
    gThreadUsbStack[STACKSIZE(STACK_USB) - 1] = 0;
    gThreadUsbStack[0] = 0;
    osStartThread(&gThreadUsb);
}

// Called from main thread.
void tick_usb_thread(void) {
    usbMesg msg;
    // Update USB thread
    msg.msgtype = (OSMesg) OS_MESG_TYPE_LOOPBACK;
    osSendMesg(&gThreadUsbMesgQueue, (OSMesg *) &msg, OS_MESG_NOBLOCK);
}

void dkr_usb_poll(void) {
    while (usb_poll()) {
        int header, numBytesToRead;
        UNUSED int dataType;
        header = usb_poll();
        dataType = USBHEADER_GETTYPE(header);
        numBytesToRead = USBHEADER_GETSIZE(header);
        // Read text data sent from computer. Note: Data sent from computer must be 4-byte aligned!
        if (numBytesToRead > 32) {
            usb_skip(numBytesToRead);
            continue;
        }
        usb_read((u8 *) textBuffer, numBytesToRead);
    }
    usb_purge(); // Not sure if this is needed?
}

#ifdef USB_HOT_RELOAD

int numBytesReadHR = 0;

void check_hot_reload(void) {
    s32 header;
    s32 numBytesToRead;

    // Check if the message is "hot", if it is then start the hot reloading process!
    if (!isHotReloading) {
        if (textBuffer[0] != 'h' || textBuffer[1] != 'o' || textBuffer[2] != 't' || textBuffer[3] != 0) {
            return;
        }

        // This is here to make it so that the USB data ends up at the start of the ROM file.
        usb_set_debug_address(0);

        // Send message to PC that it should send the 1MB of ROM.
        usb_write(DATATYPE_TEXT, (u8 *) "SEND_HOTRELOAD\n", 16);

        numBytesReadHR = 0;
        isHotReloading = 1;
        return;
    }

    while (usb_poll()) {
        header = usb_poll();
        numBytesToRead = USBHEADER_GETSIZE(header);
        if (numBytesToRead > 0) {
            if (numBytesToRead > 512) {
                numBytesToRead = 512;
            }

            // Note: I don't do anything with the usbBuffer, the data is already at the start of the ROM.
            usb_read(usbBuffer, numBytesToRead);
            numBytesReadHR += numBytesToRead;
        }

        // Finished sending over 1MB of data.
        if ((numBytesReadHR > 0) && (USBHEADER_GETSIZE(usb_poll()) < 1)) {
            usb_restore_debug_address();                             // Move the debug address back to the end of ROM.
            usb_write(DATATYPE_TEXT, (u8 *) "HOTRELOAD_DONE\n", 16); // Tell the computer that we are done.
            reset();                                                 // Reset the game.
        }
    }
}
#endif

void game_reset(void) {
    osSendMesg(&gThreadUsbMesgQueue, (OSMesg) 50, OS_MESG_BLOCK);
}

void threadusb_loop(UNUSED void *arg) {
    usbMesg *mesg = NULL;
    sUSBEnabled = TRUE;
    while (TRUE) {
        // Wait for a tick signal from the main thread
        osRecvMesg(&gThreadUsbMesgQueue, (OSMesg *) &mesg, OS_MESG_BLOCK);
        RETURN_IF_USB_NOT_VALID();
        RETURN_IF_CART_NOT_VALID();
#ifdef USB_HOT_RELOAD
        if (!isHotReloading) {
            dkr_usb_poll();
        }
        check_hot_reload();
#else
        dkr_usb_poll();
#endif
        
        switch ((s32) mesg->msgtype) {
        case MSG_WRITE:
            if (usb_timedout()) {
                usb_sendheartbeat();
            }
            usb_write(mesg->datatype, mesg->buff, mesg->size);
            break;
        case 50:
            //reset();
            break;
        }
    }
}

#ifdef SHOW_USB_INFO
// Called from main thread.
void render_usb_info(void) {
    set_render_printf_background_colour(0, 0, 0, 128);
    render_printf("Does USB work? %d\n", usbState);
    RETURN_IF_USB_NOT_VALID();
    render_printf("Cart type: %d\n", cartType);
    RETURN_IF_CART_NOT_VALID();

#ifdef USB_HOT_RELOAD
    if (isHotReloading) {
        render_printf("HOT RELOADING!\n");
    }
#endif

    // Show message sent from the computer.
    render_printf("%s\n", textBuffer);
}

#endif
#endif