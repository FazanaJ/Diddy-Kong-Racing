glabel func_80062A3C
/* 06363C 80062A3C 27BDFFC8 */  addiu $sp, $sp, -0x38
/* 063640 80062A40 AFBF001C */  sw    $ra, 0x1c($sp)
/* 063644 80062A44 AFB00018 */  sw    $s0, 0x18($sp)
/* 063648 80062A48 8C860018 */  lw    $a2, 0x18($a0)
/* 06364C 80062A4C 00808025 */  move  $s0, $a0
/* 063650 80062A50 10C00027 */  beqz  $a2, .L80062AF0
/* 063654 80062A54 27A50028 */   addiu $a1, $sp, 0x28
/* 063658 80062A58 0C031F41 */  jal   func_800C7D04
/* 06365C 80062A5C 00C02025 */   move  $a0, $a2
/* 063660 80062A60 87AE0028 */  lh    $t6, 0x28($sp)
/* 063664 80062A64 27A50028 */  addiu $a1, $sp, 0x28
/* 063668 80062A68 25CFFFFF */  addiu $t7, $t6, -1
/* 06366C 80062A6C 2DE10014 */  sltiu $at, $t7, 0x14
/* 063670 80062A70 1020001F */  beqz  $at, .L80062AF0
/* 063674 80062A74 000F7880 */   sll   $t7, $t7, 2
/* 063678 80062A78 3C01800E */  lui   $at, 0x800e
/* 06367C 80062A7C 002F0821 */  addu  $at, $at, $t7
/* 063680 80062A80 8C2F6C90 */  lw    $t7, 0x6c90($at)
/* 063684 80062A84 00000000 */  nop   
/* 063688 80062A88 01E00008 */  jr    $t7
/* 06368C 80062A8C 00000000 */   nop   
/* 063690 80062A90 0C018AC0 */  jal   func_80062B00
/* 063694 80062A94 02002025 */   move  $a0, $s0
/* 063698 80062A98 0C018A73 */  jal   func_800629CC
/* 06369C 80062A9C 02002025 */   move  $a0, $s0
/* 0636A0 80062AA0 10000014 */  b     .L80062AF4
/* 0636A4 80062AA4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0636A8 80062AA8 0C018DFB */  jal   func_800637EC
/* 0636AC 80062AAC 02002025 */   move  $a0, $s0
/* 0636B0 80062AB0 0C018A73 */  jal   func_800629CC
/* 0636B4 80062AB4 02002025 */   move  $a0, $s0
/* 0636B8 80062AB8 1000000E */  b     .L80062AF4
/* 0636BC 80062ABC 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0636C0 80062AC0 24180002 */  li    $t8, 2
/* 0636C4 80062AC4 AE18002C */  sw    $t8, 0x2c($s0)
/* 0636C8 80062AC8 24190010 */  li    $t9, 16
/* 0636CC 80062ACC 3C067FFF */  lui   $a2, (0x7FFFFFFF >> 16) # lui $a2, 0x7fff
/* 0636D0 80062AD0 A7B90028 */  sh    $t9, 0x28($sp)
/* 0636D4 80062AD4 34C6FFFF */  ori   $a2, (0x7FFFFFFF & 0xFFFF) # ori $a2, $a2, 0xffff
/* 0636D8 80062AD8 0C03246B */  jal   alEvtqPostEvent
/* 0636DC 80062ADC 26040048 */   addiu $a0, $s0, 0x48
/* 0636E0 80062AE0 10000004 */  b     .L80062AF4
/* 0636E4 80062AE4 8FBF001C */   lw    $ra, 0x1c($sp)
/* 0636E8 80062AE8 0C018A73 */  jal   func_800629CC
/* 0636EC 80062AEC 02002025 */   move  $a0, $s0
.L80062AF0:
/* 0636F0 80062AF0 8FBF001C */  lw    $ra, 0x1c($sp)
.L80062AF4:
/* 0636F4 80062AF4 8FB00018 */  lw    $s0, 0x18($sp)
/* 0636F8 80062AF8 03E00008 */  jr    $ra
/* 0636FC 80062AFC 27BD0038 */   addiu $sp, $sp, 0x38

