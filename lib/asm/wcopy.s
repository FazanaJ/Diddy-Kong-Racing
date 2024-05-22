/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800C9DA0 */

.include "macros.inc"

.set noat      # allow manual use of $at
.set noreorder # dont insert nops after branches
.set gp=64     # 64-bit instructions are used

glabel wcopy
        move    $2,$0
$LVL1 = .
$L2:
        subu    $3,$6,$2
$LVL2 = .
        bgtz    $3,$L3
        addu    $3,$4,$2

        jr      $31
        nop

$L3:
$LVL3 = .
        lw      $7,0($3)
        addu    $3,$5,$2
        sw      $7,0($3)
$LVL4 = .
        b       $L2
        addiu   $2,$2,4
        