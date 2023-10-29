glabel __umoddi3
    addiu $sp, -16
    sw    $a0, ($sp)
    sw    $a1, 4($sp)
    sw    $a2, 8($sp)
    sw    $a3, 0xc($sp)
    ld    $t7, 8($sp)
    ld    $t6, ($sp)
    ddivu $zero, $t6, $t7
    bnez  $t7, .do_umoddi3
     nop
    break 7
.do_umoddi3:
    mfhi  $v0
    dsll32 $v1, $v0, 0
    dsra32 $v1, $v1, 0
    dsra32 $v0, $v0, 0
    jr    $ra
     addiu $sp, 16

glabel __moddi3
    addiu $sp, -16
    sw    $a0, ($sp)
    sw    $a1, 4($sp)
    sw    $a2, 8($sp)
    sw    $a3, 0xc($sp)
    ld    $t7, 8($sp)
    ld    $t6, ($sp)
    ddivu $zero, $t6, $t7
    bnez  $t7, .do_moddi3
     nop
    break 7
.do_moddi3:
    mfhi  $v0
    dsll32 $v1, $v0, 0
    dsra32 $v1, $v1, 0
    dsra32 $v0, $v0, 0
    jr    $ra
     addiu $sp, 16

glabel __divdi3
    addiu $sp, -16
    sw    $a0, ($sp)
    sw    $a1, 4($sp)
    sw    $a2, 8($sp)
    sw    $a3, 0xc($sp)
    ld    $t7, 8($sp)
    ld    $t6, ($sp)
    ddiv  $zero, $t6, $t7
    nop
    bnez  $t7, .do_divdi3_1
     nop
    break 7
.do_divdi3_1:
    daddiu $at, $zero, -1
    bne   $t7, $at, .do_divdi3_2
     daddiu $at, $zero, 1
    dsll32 $at, $at, 0x1f
    bne   $t6, $at, .do_divdi3_2
     nop
    break 6
.do_divdi3_2:
    mflo  $v0
    dsll32 $v1, $v0, 0
    dsra32 $v1, $v1, 0
    dsra32 $v0, $v0, 0
    jr    $ra
     addiu $sp, 16