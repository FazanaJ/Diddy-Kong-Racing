.include "macro.inc"

/* assembler directives */
.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */
.set gp=64     /* allow use of 64-bit general purpose registers */

.section .text, "ax"

/* Official Name: mathMtxFastXFMF */
leaf mtxf_transform_dir
lwc1       $f4, 0x0($a1)
lwc1       $f10, 0x0($a0)
lwc1       $f6, 0x4($a1)
lwc1       $f12, 0x10($a0)
mul.s      $f10, $f4, $f10
lwc1       $f8, 0x8($a1)
lwc1       $f14, 0x20($a0)
mul.s      $f12, $f6, $f12
add.s      $f12, $f10, $f12
mul.s      $f14, $f8, $f14
lwc1       $f10, 0x4($a0)
mul.s      $f10, $f4, $f10
add.s      $f16, $f12, $f14
lwc1       $f12, 0x14($a0)
lwc1       $f14, 0x24($a0)
mul.s      $f12, $f6, $f12
swc1       $f16, 0x0($a2)
mul.s      $f14, $f8, $f14
add.s      $f12, $f10, $f12
lwc1       $f10, 0x8($a0)
add.s      $f16, $f12, $f14
mul.s      $f10, $f4, $f10
lwc1       $f12, 0x18($a0)
lwc1       $f14, 0x28($a0)
swc1       $f16, 0x4($a2)
mul.s      $f12, $f6, $f12
add.s      $f12, $f10, $f12
mul.s      $f14, $f8, $f14
add.s      $f14, $f12, $f14
jr         $ra
 swc1      $f14, 0x8($a2)
.end mtxf_transform_dir

leaf bcopy
        sltu    $2,$5,$4
        beq     $2,$0,$L2_bcopy
        addiu   $7,$6,-1

        move    $2,$0
$LVL1_bcopy = .
$L3_bcopy:
$LBB2_bcopy = .
        beq     $2,$6,$L8_bcopy
        addu    $3,$4,$2

$LVL2_bcopy = .
        lbu     $7,0($3)
        addu    $3,$5,$2
        sb      $7,0($3)
$LVL3_bcopy = .
        b       $L3_bcopy
        addiu   $2,$2,1

$LVL4_bcopy = .
$L2_bcopy:
$LBE2_bcopy = .
$LBB3_bcopy = .
        move    $2,$0
        subu    $6,$0,$6
$LVL5_bcopy = .
$L5_bcopy:
        bne     $2,$6,$L6_bcopy
        addu    $3,$7,$2

$L8_bcopy:
$LBE3_bcopy = .
        jr      $31
        nop

$L6_bcopy:
$LBB4_bcopy = .
$LVL6_bcopy = .
        addu    $8,$4,$3
        lbu     $8,0($8)
        addu    $3,$5,$3
        sb      $8,0($3)
$LVL7_bcopy = .
        b       $L5_bcopy
        addiu   $2,$2,-1

$LBE4_bcopy = .
.end bcopy

leaf wcopy
        move    $2,$0
$LVL1_wcopy = .
$L2_wcopy:
        subu    $3,$6,$2
$LVL2_wcopy = .
        bgtz    $3,$L3_wcopy
        addu    $3,$4,$2

        jr      $31
        nop

$L3_wcopy:
$LVL3_wcopy = .
        lw      $7,0($3)
        addu    $3,$5,$2
        sw      $7,0($3)
$LVL4_wcopy = .
        b       $L2_wcopy
        addiu   $2,$2,4
.end wcopy
leaf dcopy
        addu    $a2, $a1, $a2          # Calculate end address: end = dst + size
.Lcopy_loop:
        ld      $t0, 0($a0)            # Load 8 bytes from src
        ld      $t1, 8($a0)            # Load next 8 bytes from src
        addiu   $a0, $a0, 16           # Increment src by 16 bytes
        sd      $t0, 0($a1)            # Store 8 bytes to dst
        sd      $t1, 8($a1)            # Store next 8 bytes to dst
        addiu   $a1, $a1, 16           # Increment dst by 16 bytes
        bne     $a1, $a2, .Lcopy_loop  # Repeat until dst == end
        nop
        jr      $ra                    # Return
        nop
.end dcopy