#ifndef CONFIG_H
#define CONFIG_H

/*** This file should contain defines used for modded repos. ***/

/* -------------------------Memory------------------------ */
#define EXPANSION_PAK_SUPPORT 0  // Allow the game to use the expansion pak if it's inserted.

#define SKIP_TITLE 1

#define SKIP_INTRO SKIP_TITLE

#ifndef NON_MATCHING
#define NON_MATCHING 1
#endif

#ifndef AVOID_UB
#define AVOID_UB 1
#endif

#endif // CONFIG_H
