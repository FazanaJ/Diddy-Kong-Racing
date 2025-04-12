#ifndef CONFIG_H
#define CONFIG_H

/*** This file should contain defines used for modded repos. ***/


#define SKIP_TITLE 1

/* -------------------------Memory------------------------ */
#define EXPANSION_PAK_SUPPORT 0  // Allow the game to use the expansion pak if it's inserted.
//#define FORCE_4MB_MEMORY  // Force the game to use 4MB of memory, even if an expansion pak is inserted.

#define UNLOCK_ALL
#define SKIP_INTRO SKIP_TITLE
#define AUTOPLAY
#define SKIP_NEW_GAME

#ifndef NON_MATCHING
#define NON_MATCHING 1
#endif

#ifndef AVOID_UB
#define AVOID_UB 1
#endif

#endif // CONFIG_H
