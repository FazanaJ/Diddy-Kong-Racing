#ifndef CONFIG_H
#define CONFIG_H

/*** This file should contain defines used for modded repos. ***/


#define SKIP_TITLE 1

#define DEBUG

/* -------------------------Memory------------------------ */
#define EXPANSION_PAK_SUPPORT 0  // Allow the game to use the expansion pak if it's inserted. 0 Means never use, 1 means use if available, 2 means disallow 4MB
//#define FORCE_4MB_MEMORY  // Force the game to use 4MB of memory, even if an expansion pak is inserted.

#define UNLOCK_ALL
#define SKIP_INTRO SKIP_TITLE
#define AUTOPLAY_DEFAULT AUTOPLAY_OFF
#define SKIP_NEW_GAME
#define OPEN_ALL_DOORS
#define FIFO_4MB
//#define DISABLE_AUDIO
//#define USE_DYNLIGHTS     // Not used in vanilla, so optionally stubbed out until somebody figures out how they work

#define NUM_DEBUG_GFX 2000
#define NUM_GFX_COMMANDS 5000
#define NUM_VTX_COMMANDS 300
#define NUM_MTX_COMMANDS 400
#define NUM_TRI_COMMANDS 12

#ifndef NON_MATCHING
#define NON_MATCHING 1
#endif

#ifndef AVOID_UB
#define AVOID_UB 1
#endif

#endif // CONFIG_H
