/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800C9DA0 */

.include "macros.inc"

.set noat      # allow manual use of $at
.set noreorder # dont insert nops after branches
.set gp=64     # 64-bit instructions are used

glabel bcopy
        sltu    $2,$5,$4
        beq     $2,$0,$L2
        addiu   $7,$6,-1

        move    $2,$0
$LVL1 = .
$L3:
$LBB2 = .
        beq     $2,$6,$L8
        addu    $3,$4,$2

$LVL2 = .
        lbu     $7,0($3)
        addu    $3,$5,$2
        sb      $7,0($3)
$LVL3 = .
        b       $L3
        addiu   $2,$2,1

$LVL4 = .
$L2:
$LBE2 = .
$LBB3 = .
        move    $2,$0
        subu    $6,$0,$6
$LVL5 = .
$L5:
        bne     $2,$6,$L6
        addu    $3,$7,$2

$L8:
$LBE3 = .
        jr      $31
        nop

$L6:
$LBB4 = .
$LVL6 = .
        addu    $8,$4,$3
        lbu     $8,0($8)
        addu    $3,$5,$3
        sb      $8,0($3)
$LVL7 = .
        b       $L5
        addiu   $2,$2,-1

$LBE4 = .
