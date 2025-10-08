#include "math_util.h"

#include "game.h"
#include "macros.h"
#include "PR/os_internal_reg.h"
#include "string.h"
#include "structs.h"
#include "types.h"
#include <math.h>

/**
 * All of the functions below are handwritten assembly. Because of this, matching C code is impossible.
 * Nonmatching is not, so functionally equivalent C code can be here to replace these handwritten functions in
 * nonmatching builds. Variables cannot be declared here because of the way they're aligned, so they have to stay in an
 * assembly file.
 */

/******************************/


GLOBAL_ASM("asm/math_util/mtxf_transform_dir.s")
