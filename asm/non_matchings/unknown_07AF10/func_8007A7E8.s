glabel func_8007A7E8
/* 07B3E8 8007A7E8 27BDFFD0 */  addiu $sp, $sp, -0x30
/* 07B3EC 8007A7EC 3C0E8012 */  lui   $t6, %hi(D_801262C0) # $t6, 0x8012
/* 07B3F0 8007A7F0 AFB00018 */  sw    $s0, 0x18($sp)
/* 07B3F4 8007A7F4 25CE62C0 */  addiu $t6, %lo(D_801262C0) # addiu $t6, $t6, 0x62c0
/* 07B3F8 8007A7F8 00042880 */  sll   $a1, $a0, 2
/* 07B3FC 8007A7FC 00AE8021 */  addu  $s0, $a1, $t6
/* 07B400 8007A800 8E060000 */  lw    $a2, ($s0)
/* 07B404 8007A804 AFBF001C */  sw    $ra, 0x1c($sp)
/* 07B408 8007A808 10C00008 */  beqz  $a2, .L8007A82C
/* 07B40C 8007A80C 00C02025 */   move  $a0, $a2
/* 07B410 8007A810 0C01C54E */  jal   func_80071538
/* 07B414 8007A814 AFA5002C */   sw    $a1, 0x2c($sp)
/* 07B418 8007A818 8E040000 */  lw    $a0, ($s0)
/* 07B41C 8007A81C 0C01C450 */  jal   free_from_memory_pool
/* 07B420 8007A820 00000000 */   nop   
/* 07B424 8007A824 8FA5002C */  lw    $a1, 0x2c($sp)
/* 07B428 8007A828 00000000 */  nop   
.L8007A82C:
/* 07B42C 8007A82C 3C028012 */  lui   $v0, %hi(D_801262CC) # $v0, 0x8012
/* 07B430 8007A830 8C4262CC */  lw    $v0, %lo(D_801262CC)($v0)
/* 07B434 8007A834 3C08800E */  lui   $t0, %hi(D_800DE77C) # $t0, 0x800e
/* 07B438 8007A838 30580007 */  andi  $t8, $v0, 7
/* 07B43C 8007A83C 0018C8C0 */  sll   $t9, $t8, 3
/* 07B440 8007A840 2508E77C */  addiu $t0, %lo(D_800DE77C) # addiu $t0, $t0, -0x1884
/* 07B444 8007A844 03281821 */  addu  $v1, $t9, $t0
/* 07B448 8007A848 3C0F8012 */  lui   $t7, %hi(D_801262B0) # $t7, 0x8012
/* 07B44C 8007A84C 3C0A8012 */  lui   $t2, %hi(D_801262B8) # $t2, 0x8012
/* 07B450 8007A850 8C690000 */  lw    $t1, ($v1)
/* 07B454 8007A854 8C6B0004 */  lw    $t3, 4($v1)
/* 07B458 8007A858 25EF62B0 */  addiu $t7, %lo(D_801262B0) # addiu $t7, $t7, 0x62b0
/* 07B45C 8007A85C 254A62B8 */  addiu $t2, %lo(D_801262B8) # addiu $t2, $t2, 0x62b8
/* 07B460 8007A860 00AF3021 */  addu  $a2, $a1, $t7
/* 07B464 8007A864 00AA3821 */  addu  $a3, $a1, $t2
/* 07B468 8007A868 28410002 */  slti  $at, $v0, 2
/* 07B46C 8007A86C ACC90000 */  sw    $t1, ($a2)
/* 07B470 8007A870 14200019 */  bnez  $at, .L8007A8D8
/* 07B474 8007A874 ACEB0000 */   sw    $t3, ($a3)
/* 07B478 8007A878 3C040009 */  lui   $a0, (0x00096030 >> 16) # lui $a0, 9
/* 07B47C 8007A87C 34846030 */  ori   $a0, (0x00096030 & 0xFFFF) # ori $a0, $a0, 0x6030
/* 07B480 8007A880 0C01C327 */  jal   allocate_from_main_pool_safe
/* 07B484 8007A884 2405FFFF */   li    $a1, -1
/* 07B488 8007A888 3C03800E */  lui   $v1, %hi(D_800DE770) # $v1, 0x800e
/* 07B48C 8007A88C 2463E770 */  addiu $v1, %lo(D_800DE770) # addiu $v1, $v1, -0x1890
/* 07B490 8007A890 8C6E0000 */  lw    $t6, ($v1)
/* 07B494 8007A894 244C003F */  addiu $t4, $v0, 0x3f
/* 07B498 8007A898 2401FFC0 */  li    $at, -64
/* 07B49C 8007A89C 01816824 */  and   $t5, $t4, $at
/* 07B4A0 8007A8A0 15C00030 */  bnez  $t6, .L8007A964
/* 07B4A4 8007A8A4 AE0D0000 */   sw    $t5, ($s0)
/* 07B4A8 8007A8A8 3C040009 */  lui   $a0, (0x00096030 >> 16) # lui $a0, 9
/* 07B4AC 8007A8AC 34846030 */  ori   $a0, (0x00096030 & 0xFFFF) # ori $a0, $a0, 0x6030
/* 07B4B0 8007A8B0 0C01C327 */  jal   allocate_from_main_pool_safe
/* 07B4B4 8007A8B4 2405FFFF */   li    $a1, -1
/* 07B4B8 8007A8B8 3C03800E */  lui   $v1, %hi(D_800DE770) # $v1, 0x800e
/* 07B4BC 8007A8BC 2463E770 */  addiu $v1, %lo(D_800DE770) # addiu $v1, $v1, -0x1890
/* 07B4C0 8007A8C0 2458003F */  addiu $t8, $v0, 0x3f
/* 07B4C4 8007A8C4 2401FFC0 */  li    $at, -64
/* 07B4C8 8007A8C8 AC620000 */  sw    $v0, ($v1)
/* 07B4CC 8007A8CC 0301C824 */  and   $t9, $t8, $at
/* 07B4D0 8007A8D0 10000024 */  b     .L8007A964
/* 07B4D4 8007A8D4 AC790000 */   sw    $t9, ($v1)
.L8007A8D8:
/* 07B4D8 8007A8D8 8CE80000 */  lw    $t0, ($a3)
/* 07B4DC 8007A8DC 8CC90000 */  lw    $t1, ($a2)
/* 07B4E0 8007A8E0 2405FFFF */  li    $a1, -1
/* 07B4E4 8007A8E4 01090019 */  multu $t0, $t1
/* 07B4E8 8007A8E8 AFA60024 */  sw    $a2, 0x24($sp)
/* 07B4EC 8007A8EC AFA70020 */  sw    $a3, 0x20($sp)
/* 07B4F0 8007A8F0 00002012 */  mflo  $a0
/* 07B4F4 8007A8F4 00045040 */  sll   $t2, $a0, 1
/* 07B4F8 8007A8F8 0C01C327 */  jal   allocate_from_main_pool_safe
/* 07B4FC 8007A8FC 25440030 */   addiu $a0, $t2, 0x30
/* 07B500 8007A900 3C03800E */  lui   $v1, %hi(D_800DE770) # $v1, 0x800e
/* 07B504 8007A904 2463E770 */  addiu $v1, %lo(D_800DE770) # addiu $v1, $v1, -0x1890
/* 07B508 8007A908 8C6D0000 */  lw    $t5, ($v1)
/* 07B50C 8007A90C 244B003F */  addiu $t3, $v0, 0x3f
/* 07B510 8007A910 2401FFC0 */  li    $at, -64
/* 07B514 8007A914 8FA60024 */  lw    $a2, 0x24($sp)
/* 07B518 8007A918 8FA70020 */  lw    $a3, 0x20($sp)
/* 07B51C 8007A91C 01616024 */  and   $t4, $t3, $at
/* 07B520 8007A920 15A00010 */  bnez  $t5, .L8007A964
/* 07B524 8007A924 AE0C0000 */   sw    $t4, ($s0)
/* 07B528 8007A928 8CEE0000 */  lw    $t6, ($a3)
/* 07B52C 8007A92C 8CCF0000 */  lw    $t7, ($a2)
/* 07B530 8007A930 2405FFFF */  li    $a1, -1
/* 07B534 8007A934 01CF0019 */  multu $t6, $t7
/* 07B538 8007A938 00002012 */  mflo  $a0
/* 07B53C 8007A93C 0004C040 */  sll   $t8, $a0, 1
/* 07B540 8007A940 0C01C327 */  jal   allocate_from_main_pool_safe
/* 07B544 8007A944 27040030 */   addiu $a0, $t8, 0x30
/* 07B548 8007A948 3C03800E */  lui   $v1, %hi(D_800DE770) # $v1, 0x800e
/* 07B54C 8007A94C 2463E770 */  addiu $v1, %lo(D_800DE770) # addiu $v1, $v1, -0x1890
/* 07B550 8007A950 2448003F */  addiu $t0, $v0, 0x3f
/* 07B554 8007A954 2401FFC0 */  li    $at, -64
/* 07B558 8007A958 AC620000 */  sw    $v0, ($v1)
/* 07B55C 8007A95C 01014824 */  and   $t1, $t0, $at
/* 07B560 8007A960 AC690000 */  sw    $t1, ($v1)
.L8007A964:
/* 07B564 8007A964 8FBF001C */  lw    $ra, 0x1c($sp)
/* 07B568 8007A968 8FB00018 */  lw    $s0, 0x18($sp)
/* 07B56C 8007A96C 03E00008 */  jr    $ra
/* 07B570 8007A970 27BD0030 */   addiu $sp, $sp, 0x30

