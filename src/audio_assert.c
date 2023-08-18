/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800B6F30 */

#include "audio_assert.h"

// Only called from two libultra funcs for some reason. 
// This is likely an assert gone wrong.
// Called from alEnvMixerPull, and _pullSubFrame
