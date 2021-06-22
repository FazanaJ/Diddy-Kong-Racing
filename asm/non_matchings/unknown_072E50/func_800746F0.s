glabel func_800746F0
/* 0752F0 800746F0 27BDFFC0 */  addiu $sp, $sp, -0x40
/* 0752F4 800746F4 AFBF0024 */  sw    $ra, 0x24($sp)
/* 0752F8 800746F8 AFB1001C */  sw    $s1, 0x1c($sp)
/* 0752FC 800746FC AFB00018 */  sw    $s0, 0x18($sp)
/* 075300 80074700 00808025 */  move  $s0, $a0
/* 075304 80074704 30B100FF */  andi  $s1, $a1, 0xff
/* 075308 80074708 AFB20020 */  sw    $s2, 0x20($sp)
/* 07530C 8007470C 0C01A840 */  jal   func_8006A100
/* 075310 80074710 AFA50044 */   sw    $a1, 0x44($sp)
/* 075314 80074714 0C033884 */  jal   osEepromProbe
/* 075318 80074718 00402025 */   move  $a0, $v0
/* 07531C 8007471C 14400003 */  bnez  $v0, .L8007472C
/* 075320 80074720 24040200 */   li    $a0, 512
/* 075324 80074724 10000033 */  b     .L800747F4
/* 075328 80074728 2402FFFF */   li    $v0, -1
.L8007472C:
/* 07532C 8007472C 0C01C327 */  jal   allocate_from_main_pool_safe
/* 075330 80074730 2405FFFF */   li    $a1, -1
/* 075334 80074734 00409025 */  move  $s2, $v0
/* 075338 80074738 02002025 */  move  $a0, $s0
/* 07533C 8007473C 0C01CE29 */  jal   func_800738A4
/* 075340 80074740 00402825 */   move  $a1, $v0
/* 075344 80074744 322F0001 */  andi  $t7, $s1, 1
/* 075348 80074748 11E00011 */  beqz  $t7, .L80074790
/* 07534C 8007474C AFB10028 */   sw    $s1, 0x28($sp)
/* 075350 80074750 0C01BAB0 */  jal   func_8006EAC0
/* 075354 80074754 24110018 */   li    $s1, 24
/* 075358 80074758 1440000D */  bnez  $v0, .L80074790
/* 07535C 8007475C 00008025 */   move  $s0, $zero
.L80074760:
/* 075360 80074760 0C01A840 */  jal   func_8006A100
/* 075364 80074764 00000000 */   nop   
/* 075368 80074768 26050010 */  addiu $a1, $s0, 0x10
/* 07536C 8007476C 30B800FF */  andi  $t8, $a1, 0xff
/* 075370 80074770 0010C8C0 */  sll   $t9, $s0, 3
/* 075374 80074774 03323021 */  addu  $a2, $t9, $s2
/* 075378 80074778 03002825 */  move  $a1, $t8
/* 07537C 8007477C 0C033960 */  jal   osEepromWrite
/* 075380 80074780 00402025 */   move  $a0, $v0
/* 075384 80074784 26100001 */  addiu $s0, $s0, 1
/* 075388 80074788 1611FFF5 */  bne   $s0, $s1, .L80074760
/* 07538C 8007478C 00000000 */   nop   
.L80074790:
/* 075390 80074790 8FA80028 */  lw    $t0, 0x28($sp)
/* 075394 80074794 00000000 */  nop   
/* 075398 80074798 31090002 */  andi  $t1, $t0, 2
/* 07539C 8007479C 11200012 */  beqz  $t1, .L800747E8
/* 0753A0 800747A0 00000000 */   nop   
/* 0753A4 800747A4 0C01BAB0 */  jal   func_8006EAC0
/* 0753A8 800747A8 24110018 */   li    $s1, 24
/* 0753AC 800747AC 1440000E */  bnez  $v0, .L800747E8
/* 0753B0 800747B0 00008025 */   move  $s0, $zero
.L800747B4:
/* 0753B4 800747B4 0C01A840 */  jal   func_8006A100
/* 0753B8 800747B8 00000000 */   nop   
/* 0753BC 800747BC 26050028 */  addiu $a1, $s0, 0x28
/* 0753C0 800747C0 001058C0 */  sll   $t3, $s0, 3
/* 0753C4 800747C4 024B3021 */  addu  $a2, $s2, $t3
/* 0753C8 800747C8 30AA00FF */  andi  $t2, $a1, 0xff
/* 0753CC 800747CC 01402825 */  move  $a1, $t2
/* 0753D0 800747D0 24C600C0 */  addiu $a2, $a2, 0xc0
/* 0753D4 800747D4 0C033960 */  jal   osEepromWrite
/* 0753D8 800747D8 00402025 */   move  $a0, $v0
/* 0753DC 800747DC 26100001 */  addiu $s0, $s0, 1
/* 0753E0 800747E0 1611FFF4 */  bne   $s0, $s1, .L800747B4
/* 0753E4 800747E4 00000000 */   nop   
.L800747E8:
/* 0753E8 800747E8 0C01C450 */  jal   free_from_memory_pool
/* 0753EC 800747EC 02402025 */   move  $a0, $s2
/* 0753F0 800747F0 00001025 */  move  $v0, $zero
.L800747F4:
/* 0753F4 800747F4 8FBF0024 */  lw    $ra, 0x24($sp)
/* 0753F8 800747F8 8FB00018 */  lw    $s0, 0x18($sp)
/* 0753FC 800747FC 8FB1001C */  lw    $s1, 0x1c($sp)
/* 075400 80074800 8FB20020 */  lw    $s2, 0x20($sp)
/* 075404 80074804 03E00008 */  jr    $ra
/* 075408 80074808 27BD0040 */   addiu $sp, $sp, 0x40

