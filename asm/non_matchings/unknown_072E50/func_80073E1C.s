glabel func_80073E1C
/* 074A1C 80073E1C 27BDFFD0 */  addiu $sp, $sp, -0x30
/* 074A20 80073E20 AFBF001C */  sw    $ra, 0x1c($sp)
/* 074A24 80073E24 AFB00018 */  sw    $s0, 0x18($sp)
/* 074A28 80073E28 00808025 */  move  $s0, $a0
/* 074A2C 80073E2C AFA50034 */  sw    $a1, 0x34($sp)
/* 074A30 80073E30 0C01D637 */  jal   func_800758DC
/* 074A34 80073E34 AFA60038 */   sw    $a2, 0x38($sp)
/* 074A38 80073E38 10400008 */  beqz  $v0, .L80073E5C
/* 074A3C 80073E3C 02002025 */   move  $a0, $s0
/* 074A40 80073E40 02002025 */  move  $a0, $s0
/* 074A44 80073E44 0C01D6BB */  jal   func_80075AEC
/* 074A48 80073E48 AFA20028 */   sw    $v0, 0x28($sp)
/* 074A4C 80073E4C 8FA30028 */  lw    $v1, 0x28($sp)
/* 074A50 80073E50 00107780 */  sll   $t6, $s0, 0x1e
/* 074A54 80073E54 1000003D */  b     .L80073F4C
/* 074A58 80073E58 01C31025 */   or    $v0, $t6, $v1
.L80073E5C:
/* 074A5C 80073E5C 3C05800E */  lui   $a1, %hi(D_800E7670) # $a1, 0x800e
/* 074A60 80073E60 8FA60034 */  lw    $a2, 0x34($sp)
/* 074A64 80073E64 24A57670 */  addiu $a1, %lo(D_800E7670) # addiu $a1, $a1, 0x7670
/* 074A68 80073E68 0C01D93A */  jal   func_800764E8
/* 074A6C 80073E6C 27A70024 */   addiu $a3, $sp, 0x24
/* 074A70 80073E70 1440002D */  bnez  $v0, .L80073F28
/* 074A74 80073E74 00401825 */   move  $v1, $v0
/* 074A78 80073E78 8FA50024 */  lw    $a1, 0x24($sp)
/* 074A7C 80073E7C 02002025 */  move  $a0, $s0
/* 074A80 80073E80 0C01DA49 */  jal   func_80076924
/* 074A84 80073E84 27A60020 */   addiu $a2, $sp, 0x20
/* 074A88 80073E88 8FA40020 */  lw    $a0, 0x20($sp)
/* 074A8C 80073E8C 00401825 */  move  $v1, $v0
/* 074A90 80073E90 14800002 */  bnez  $a0, .L80073E9C
/* 074A94 80073E94 00000000 */   nop   
/* 074A98 80073E98 24030009 */  li    $v1, 9
.L80073E9C:
/* 074A9C 80073E9C 14600022 */  bnez  $v1, .L80073F28
/* 074AA0 80073EA0 00000000 */   nop   
/* 074AA4 80073EA4 0C01C327 */  jal   func_80070C9C
/* 074AA8 80073EA8 240500FF */   li    $a1, 255
/* 074AAC 80073EAC 8FA50024 */  lw    $a1, 0x24($sp)
/* 074AB0 80073EB0 8FA70020 */  lw    $a3, 0x20($sp)
/* 074AB4 80073EB4 AFA2002C */  sw    $v0, 0x2c($sp)
/* 074AB8 80073EB8 02002025 */  move  $a0, $s0
/* 074ABC 80073EBC 0C01D984 */  jal   func_80076610
/* 074AC0 80073EC0 00403025 */   move  $a2, $v0
/* 074AC4 80073EC4 14400013 */  bnez  $v0, .L80073F14
/* 074AC8 80073EC8 00401825 */   move  $v1, $v0
/* 074ACC 80073ECC 8FA6002C */  lw    $a2, 0x2c($sp)
/* 074AD0 80073ED0 3C014741 */  lui   $at, (0x47414D44 >> 16) # lui $at, 0x4741
/* 074AD4 80073ED4 8CCF0000 */  lw    $t7, ($a2)
/* 074AD8 80073ED8 34214D44 */  ori   $at, (0x47414D44 & 0xFFFF) # ori $at, $at, 0x4d44
/* 074ADC 80073EDC 15E1000D */  bne   $t7, $at, .L80073F14
/* 074AE0 80073EE0 24030009 */   li    $v1, 9
/* 074AE4 80073EE4 8FA40038 */  lw    $a0, 0x38($sp)
/* 074AE8 80073EE8 24C50004 */  addiu $a1, $a2, 4
/* 074AEC 80073EEC 0C01CC1B */  jal   func_8007306C
/* 074AF0 80073EF0 AFA20028 */   sw    $v0, 0x28($sp)
/* 074AF4 80073EF4 8FB80038 */  lw    $t8, 0x38($sp)
/* 074AF8 80073EF8 8FA30028 */  lw    $v1, 0x28($sp)
/* 074AFC 80073EFC 9319004B */  lbu   $t9, 0x4b($t8)
/* 074B00 80073F00 00000000 */  nop   
/* 074B04 80073F04 13200004 */  beqz  $t9, .L80073F18
/* 074B08 80073F08 8FA4002C */   lw    $a0, 0x2c($sp)
/* 074B0C 80073F0C 10000001 */  b     .L80073F14
/* 074B10 80073F10 24030005 */   li    $v1, 5
.L80073F14:
/* 074B14 80073F14 8FA4002C */  lw    $a0, 0x2c($sp)
.L80073F18:
/* 074B18 80073F18 0C01C450 */  jal   func_80071140
/* 074B1C 80073F1C AFA30028 */   sw    $v1, 0x28($sp)
/* 074B20 80073F20 8FA30028 */  lw    $v1, 0x28($sp)
/* 074B24 80073F24 00000000 */  nop   
.L80073F28:
/* 074B28 80073F28 02002025 */  move  $a0, $s0
/* 074B2C 80073F2C 0C01D6BB */  jal   func_80075AEC
/* 074B30 80073F30 AFA30028 */   sw    $v1, 0x28($sp)
/* 074B34 80073F34 8FA30028 */  lw    $v1, 0x28($sp)
/* 074B38 80073F38 00104780 */  sll   $t0, $s0, 0x1e
/* 074B3C 80073F3C 10600003 */  beqz  $v1, .L80073F4C
/* 074B40 80073F40 00601025 */   move  $v0, $v1
/* 074B44 80073F44 00681825 */  or    $v1, $v1, $t0
/* 074B48 80073F48 00601025 */  move  $v0, $v1
.L80073F4C:
/* 074B4C 80073F4C 8FBF001C */  lw    $ra, 0x1c($sp)
/* 074B50 80073F50 8FB00018 */  lw    $s0, 0x18($sp)
/* 074B54 80073F54 03E00008 */  jr    $ra
/* 074B58 80073F58 27BD0030 */   addiu $sp, $sp, 0x30
