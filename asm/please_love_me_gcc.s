/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x80065D50 */

# assembler directives
.set noat      # allow manual use of $at
.set noreorder # don't insert nops after branches
.set gp=64

.include "macros.inc"

.section .text, "ax"

.include "asm/gcc_asm/__ll_div.s"
.include "asm/gcc_asm/__ll_lshift.s"
.include "asm/gcc_asm/__ll_mul.s"
.include "asm/gcc_asm/__ull_div.s"
.include "asm/gcc_asm/__ull_divremi.s"
.include "asm/gcc_asm/__ull_rem.s"
.include "asm/gcc_asm/__ull_rshift.s"
.include "asm/gcc_asm/__others.s"
