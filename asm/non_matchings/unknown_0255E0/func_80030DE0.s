glabel func_80030DE0
/* 0319E0 80030DE0 27BDFFF0 */  addiu $sp, $sp, -0x10
/* 0319E4 80030DE4 AFB1000C */  sw    $s1, 0xc($sp)
/* 0319E8 80030DE8 AFB00008 */  sw    $s0, 8($sp)
/* 0319EC 80030DEC 8FB00024 */  lw    $s0, 0x24($sp)
/* 0319F0 80030DF0 8FB10020 */  lw    $s1, 0x20($sp)
/* 0319F4 80030DF4 8FA30028 */  lw    $v1, 0x28($sp)
/* 0319F8 80030DF8 0211082A */  slt   $at, $s0, $s1
/* 0319FC 80030DFC 10200004 */  beqz  $at, .L80030E10
/* 031A00 80030E00 000470C0 */   sll   $t6, $a0, 3
/* 031A04 80030E04 02201025 */  move  $v0, $s1
/* 031A08 80030E08 02008825 */  move  $s1, $s0
/* 031A0C 80030E0C 00408025 */  move  $s0, $v0
.L80030E10:
/* 031A10 80030E10 2A010400 */  slti  $at, $s0, 0x400
/* 031A14 80030E14 14200002 */  bnez  $at, .L80030E20
/* 031A18 80030E18 01C47023 */   subu  $t6, $t6, $a0
/* 031A1C 80030E1C 241003FF */  li    $s0, 1023
.L80030E20:
/* 031A20 80030E20 2602FFFB */  addiu $v0, $s0, -5
/* 031A24 80030E24 0222082A */  slt   $at, $s1, $v0
/* 031A28 80030E28 14200002 */  bnez  $at, .L80030E34
/* 031A2C 80030E2C 000E70C0 */   sll   $t6, $t6, 3
/* 031A30 80030E30 00408825 */  move  $s1, $v0
.L80030E34:
/* 031A34 80030E34 3C0F8012 */  lui   $t7, %hi(D_8011D388) # $t7, 0x8012
/* 031A38 80030E38 25EFD388 */  addiu $t7, %lo(D_8011D388) # addiu $t7, $t7, -0x2c78
/* 031A3C 80030E3C 01CF1021 */  addu  $v0, $t6, $t7
/* 031A40 80030E40 8C590000 */  lw    $t9, ($v0)
/* 031A44 80030E44 0005C400 */  sll   $t8, $a1, 0x10
/* 031A48 80030E48 03194023 */  subu  $t0, $t8, $t9
/* 031A4C 80030E4C 0103001A */  div   $zero, $t0, $v1
/* 031A50 80030E50 8C4B0004 */  lw    $t3, 4($v0)
/* 031A54 80030E54 00065400 */  sll   $t2, $a2, 0x10
/* 031A58 80030E58 014B6023 */  subu  $t4, $t2, $t3
/* 031A5C 80030E5C 8C4F0008 */  lw    $t7, 8($v0)
/* 031A60 80030E60 00077400 */  sll   $t6, $a3, 0x10
/* 031A64 80030E64 01CFC023 */  subu  $t8, $t6, $t7
/* 031A68 80030E68 A0450028 */  sb    $a1, 0x28($v0)
/* 031A6C 80030E6C A0460029 */  sb    $a2, 0x29($v0)
/* 031A70 80030E70 A047002A */  sb    $a3, 0x2a($v0)
/* 031A74 80030E74 A451002C */  sh    $s1, 0x2c($v0)
/* 031A78 80030E78 A450002E */  sh    $s0, 0x2e($v0)
/* 031A7C 80030E7C 14600002 */  bnez  $v1, .L80030E88
/* 031A80 80030E80 00000000 */   nop   
/* 031A84 80030E84 0007000D */  break 7
.L80030E88:
/* 031A88 80030E88 2401FFFF */  li    $at, -1
/* 031A8C 80030E8C 14610004 */  bne   $v1, $at, .L80030EA0
/* 031A90 80030E90 3C018000 */   lui   $at, 0x8000
/* 031A94 80030E94 15010002 */  bne   $t0, $at, .L80030EA0
/* 031A98 80030E98 00000000 */   nop   
/* 031A9C 80030E9C 0006000D */  break 6
.L80030EA0:
/* 031AA0 80030EA0 00114400 */  sll   $t0, $s1, 0x10
/* 031AA4 80030EA4 AC430030 */  sw    $v1, 0x30($v0)
/* 031AA8 80030EA8 AC400034 */  sw    $zero, 0x34($v0)
/* 031AAC 80030EAC 00004812 */  mflo  $t1
/* 031AB0 80030EB0 AC490014 */  sw    $t1, 0x14($v0)
/* 031AB4 80030EB4 8C49000C */  lw    $t1, 0xc($v0)
/* 031AB8 80030EB8 0183001A */  div   $zero, $t4, $v1
/* 031ABC 80030EBC 01095023 */  subu  $t2, $t0, $t1
/* 031AC0 80030EC0 14600002 */  bnez  $v1, .L80030ECC
/* 031AC4 80030EC4 00000000 */   nop   
/* 031AC8 80030EC8 0007000D */  break 7
.L80030ECC:
/* 031ACC 80030ECC 2401FFFF */  li    $at, -1
/* 031AD0 80030ED0 14610004 */  bne   $v1, $at, .L80030EE4
/* 031AD4 80030ED4 3C018000 */   lui   $at, 0x8000
/* 031AD8 80030ED8 15810002 */  bne   $t4, $at, .L80030EE4
/* 031ADC 80030EDC 00000000 */   nop   
/* 031AE0 80030EE0 0006000D */  break 6
.L80030EE4:
/* 031AE4 80030EE4 00106400 */  sll   $t4, $s0, 0x10
/* 031AE8 80030EE8 00006812 */  mflo  $t5
/* 031AEC 80030EEC AC4D0018 */  sw    $t5, 0x18($v0)
/* 031AF0 80030EF0 8C4D0010 */  lw    $t5, 0x10($v0)
/* 031AF4 80030EF4 0303001A */  div   $zero, $t8, $v1
/* 031AF8 80030EF8 018D7023 */  subu  $t6, $t4, $t5
/* 031AFC 80030EFC 14600002 */  bnez  $v1, .L80030F08
/* 031B00 80030F00 00000000 */   nop   
/* 031B04 80030F04 0007000D */  break 7
.L80030F08:
/* 031B08 80030F08 2401FFFF */  li    $at, -1
/* 031B0C 80030F0C 14610004 */  bne   $v1, $at, .L80030F20
/* 031B10 80030F10 3C018000 */   lui   $at, 0x8000
/* 031B14 80030F14 17010002 */  bne   $t8, $at, .L80030F20
/* 031B18 80030F18 00000000 */   nop   
/* 031B1C 80030F1C 0006000D */  break 6
.L80030F20:
/* 031B20 80030F20 0000C812 */  mflo  $t9
/* 031B24 80030F24 AC59001C */  sw    $t9, 0x1c($v0)
/* 031B28 80030F28 00000000 */  nop   
/* 031B2C 80030F2C 0143001A */  div   $zero, $t2, $v1
/* 031B30 80030F30 14600002 */  bnez  $v1, .L80030F3C
/* 031B34 80030F34 00000000 */   nop   
/* 031B38 80030F38 0007000D */  break 7
.L80030F3C:
/* 031B3C 80030F3C 2401FFFF */  li    $at, -1
/* 031B40 80030F40 14610004 */  bne   $v1, $at, .L80030F54
/* 031B44 80030F44 3C018000 */   lui   $at, 0x8000
/* 031B48 80030F48 15410002 */  bne   $t2, $at, .L80030F54
/* 031B4C 80030F4C 00000000 */   nop   
/* 031B50 80030F50 0006000D */  break 6
.L80030F54:
/* 031B54 80030F54 00005812 */  mflo  $t3
/* 031B58 80030F58 AC4B0020 */  sw    $t3, 0x20($v0)
/* 031B5C 80030F5C 00000000 */  nop   
/* 031B60 80030F60 01C3001A */  div   $zero, $t6, $v1
/* 031B64 80030F64 14600002 */  bnez  $v1, .L80030F70
/* 031B68 80030F68 00000000 */   nop   
/* 031B6C 80030F6C 0007000D */  break 7
.L80030F70:
/* 031B70 80030F70 2401FFFF */  li    $at, -1
/* 031B74 80030F74 14610004 */  bne   $v1, $at, .L80030F88
/* 031B78 80030F78 3C018000 */   lui   $at, 0x8000
/* 031B7C 80030F7C 15C10002 */  bne   $t6, $at, .L80030F88
/* 031B80 80030F80 00000000 */   nop   
/* 031B84 80030F84 0006000D */  break 6
.L80030F88:
/* 031B88 80030F88 00007812 */  mflo  $t7
/* 031B8C 80030F8C AC4F0024 */  sw    $t7, 0x24($v0)
/* 031B90 80030F90 8FB1000C */  lw    $s1, 0xc($sp)
/* 031B94 80030F94 8FB00008 */  lw    $s0, 8($sp)
/* 031B98 80030F98 03E00008 */  jr    $ra
/* 031B9C 80030F9C 27BD0010 */   addiu $sp, $sp, 0x10

