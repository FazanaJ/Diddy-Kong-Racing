.include "macro.inc"

/* assembler directives */
.set noat      /* allow manual use of $at */
.set noreorder /* don't insert nops after branches */
.set gp=64     /* allow use of 64-bit general purpose registers */

.section .bss

.section .text, "ax"

leaf entrypoint
lui        $t0, %hi(__BSS_SECTION_START)
lui        $t1, %hi(__BSS_SECTION_SIZE)
addiu      $t0, $t0, %lo(__BSS_SECTION_START)
ori        $t1, $t1, %lo(__BSS_SECTION_SIZE)
.clear_bytes:
addi       $t1, $t1, -0x8
sw         $zero, 0x0($t0)
sw         $zero, 0x4($t0)
bnez       $t1, .clear_bytes
 addi      $t0, $t0, 0x8
lui        $t2, %hi(mainproc)
lui        $sp, %hi(0x80300000 + 20)
addiu      $t2, $t2, %lo(mainproc)
jr         $t2
 addiu     $sp, $sp, %lo(0x80300000 + 20)
nop
nop
nop
nop
nop
nop
.end entrypoint

.global get_clockspeed
.balign 32
get_clockspeed:
    mfc0 $v1, $9
    nop
    mfc0 $v0, $9
    nop
    subu $v0, $v0, $v1
    jr $ra
    srl $v0, $v0, 1
	
.global get_cachemiss
.balign 32
get_cachemiss:
	nop
	nop
    mfc0 $v1, $9
    nop
    mfc0 $v0, $9
    nop
    subu $v0, $v0, $v1
    jr $ra
    srl $v0, $v0, 1

.global reboot_disable_interrupts
.balign 32
reboot_disable_interrupts:
    mfc0 $t0, $12      # $t0 = status register flags
    andi $t0, $t0, 0xFFFE  # Disable interrupts flags
    jr $ra 
    mtc0 $t0, $12      # Write new status flags

.global emux_detect
.balign 32
emux_detect:
or $v0, $0, $0
jr $ra
tne $v0, $v0, 0x0