glabel func_80071314
/* 071F14 80071314 27BDFFB0 */  addiu $sp, $sp, -0x50
/* 071F18 80071318 AFBF003C */  sw    $ra, 0x3c($sp)
/* 071F1C 8007131C AFBE0038 */  sw    $fp, 0x38($sp)
/* 071F20 80071320 AFB70034 */  sw    $s7, 0x34($sp)
/* 071F24 80071324 AFB60030 */  sw    $s6, 0x30($sp)
/* 071F28 80071328 AFB5002C */  sw    $s5, 0x2c($sp)
/* 071F2C 8007132C AFB40028 */  sw    $s4, 0x28($sp)
/* 071F30 80071330 AFB30024 */  sw    $s3, 0x24($sp)
/* 071F34 80071334 AFB20020 */  sw    $s2, 0x20($sp)
/* 071F38 80071338 AFB1001C */  sw    $s1, 0x1c($sp)
/* 071F3C 8007133C 0C01BD44 */  jal   func_8006F510
/* 071F40 80071340 AFB00018 */   sw    $s0, 0x18($sp)
/* 071F44 80071344 3C128012 */  lui   $s2, %hi(gNumberOfMemoryPools) # $s2, 0x8012
/* 071F48 80071348 8E5235C0 */  lw    $s2, %lo(gNumberOfMemoryPools)($s2)
/* 071F4C 8007134C 2416FFFF */  li    $s6, -1
/* 071F50 80071350 1256002C */  beq   $s2, $s6, .L80071404
/* 071F54 80071354 AFA20040 */   sw    $v0, 0x40($sp)
/* 071F58 80071358 3C0F8012 */  lui   $t7, %hi(gMemoryPools) # $t7, 0x8012
/* 071F5C 8007135C 25EF3580 */  addiu $t7, %lo(gMemoryPools) # addiu $t7, $t7, 0x3580
/* 071F60 80071360 00127100 */  sll   $t6, $s2, 4
/* 071F64 80071364 01CFA821 */  addu  $s5, $t6, $t7
/* 071F68 80071368 241E0004 */  li    $fp, 4
/* 071F6C 8007136C 24170014 */  li    $s7, 20
/* 071F70 80071370 24130001 */  li    $s3, 1
.L80071374:
/* 071F74 80071374 8EB40008 */  lw    $s4, 8($s5)
/* 071F78 80071378 00008025 */  move  $s0, $zero
.L8007137C:
/* 071F7C 8007137C 02170019 */  multu $s0, $s7
/* 071F80 80071380 02402025 */  move  $a0, $s2
/* 071F84 80071384 0000C012 */  mflo  $t8
/* 071F88 80071388 03148821 */  addu  $s1, $t8, $s4
/* 071F8C 8007138C 86220008 */  lh    $v0, 8($s1)
/* 071F90 80071390 00000000 */  nop   
/* 071F94 80071394 16620005 */  bne   $s3, $v0, .L800713AC
/* 071F98 80071398 00000000 */   nop   
/* 071F9C 8007139C 0C01C593 */  jal   free_memory_pool_slot
/* 071FA0 800713A0 02002825 */   move  $a1, $s0
/* 071FA4 800713A4 86220008 */  lh    $v0, 8($s1)
/* 071FA8 800713A8 00000000 */  nop   
.L800713AC:
/* 071FAC 800713AC 17C2000E */  bne   $fp, $v0, .L800713E8
/* 071FB0 800713B0 02402025 */   move  $a0, $s2
/* 071FB4 800713B4 8EB90004 */  lw    $t9, 4($s5)
/* 071FB8 800713B8 00000000 */  nop   
/* 071FBC 800713BC 16790005 */  bne   $s3, $t9, .L800713D4
/* 071FC0 800713C0 00000000 */   nop   
/* 071FC4 800713C4 0C01C593 */  jal   free_memory_pool_slot
/* 071FC8 800713C8 02002825 */   move  $a1, $s0
/* 071FCC 800713CC 10000007 */  b     .L800713EC
/* 071FD0 800713D0 8630000C */   lh    $s0, 0xc($s1)
.L800713D4:
/* 071FD4 800713D4 8FA40040 */  lw    $a0, 0x40($sp)
/* 071FD8 800713D8 0C01BD4F */  jal   func_8006F53C
/* 071FDC 800713DC 00000000 */   nop   
/* 071FE0 800713E0 1000000C */  b     .L80071414
/* 071FE4 800713E4 8FBF003C */   lw    $ra, 0x3c($sp)
.L800713E8:
/* 071FE8 800713E8 8630000C */  lh    $s0, 0xc($s1)
.L800713EC:
/* 071FEC 800713EC 00000000 */  nop   
/* 071FF0 800713F0 1616FFE2 */  bne   $s0, $s6, .L8007137C
/* 071FF4 800713F4 00000000 */   nop   
/* 071FF8 800713F8 2652FFFF */  addiu $s2, $s2, -1
/* 071FFC 800713FC 1656FFDD */  bne   $s2, $s6, .L80071374
/* 072000 80071400 26B5FFF0 */   addiu $s5, $s5, -0x10
.L80071404:
/* 072004 80071404 8FA40040 */  lw    $a0, 0x40($sp)
/* 072008 80071408 0C01BD4F */  jal   func_8006F53C
/* 07200C 8007140C 00000000 */   nop   
/* 072010 80071410 8FBF003C */  lw    $ra, 0x3c($sp)
.L80071414:
/* 072014 80071414 8FB00018 */  lw    $s0, 0x18($sp)
/* 072018 80071418 8FB1001C */  lw    $s1, 0x1c($sp)
/* 07201C 8007141C 8FB20020 */  lw    $s2, 0x20($sp)
/* 072020 80071420 8FB30024 */  lw    $s3, 0x24($sp)
/* 072024 80071424 8FB40028 */  lw    $s4, 0x28($sp)
/* 072028 80071428 8FB5002C */  lw    $s5, 0x2c($sp)
/* 07202C 8007142C 8FB60030 */  lw    $s6, 0x30($sp)
/* 072030 80071430 8FB70034 */  lw    $s7, 0x34($sp)
/* 072034 80071434 8FBE0038 */  lw    $fp, 0x38($sp)
/* 072038 80071438 03E00008 */  jr    $ra
/* 07203C 8007143C 27BD0050 */   addiu $sp, $sp, 0x50

