.rdata
glabel D_800E5FB4
.asciz "Illegal door no!!!\n"

.text
glabel func_8003B7CC
/* 03C3CC 8003B7CC 27BDFFD8 */  addiu $sp, $sp, -0x28
/* 03C3D0 8003B7D0 AFBF001C */  sw    $ra, 0x1c($sp)
/* 03C3D4 8003B7D4 AFB00018 */  sw    $s0, 0x18($sp)
/* 03C3D8 8003B7D8 80A7000C */  lb    $a3, 0xc($a1)
/* 03C3DC 8003B7DC 8C830064 */  lw    $v1, 0x64($a0)
/* 03C3E0 8003B7E0 2401FFFF */  li    $at, -1
/* 03C3E4 8003B7E4 00808025 */  move  $s0, $a0
/* 03C3E8 8003B7E8 14E10008 */  bne   $a3, $at, .L8003B80C
/* 03C3EC 8003B7EC 00A03025 */   move  $a2, $a1
/* 03C3F0 8003B7F0 AFA30024 */  sw    $v1, 0x24($sp)
/* 03C3F4 8003B7F4 0C003308 */  jal   func_8000CC20
/* 03C3F8 8003B7F8 AFA5002C */   sw    $a1, 0x2c($sp)
/* 03C3FC 8003B7FC 8FA6002C */  lw    $a2, 0x2c($sp)
/* 03C400 8003B800 8FA30024 */  lw    $v1, 0x24($sp)
/* 03C404 8003B804 10000009 */  b     .L8003B82C
/* 03C408 8003B808 A0C2000C */   sb    $v0, 0xc($a2)
.L8003B80C:
/* 03C40C 8003B80C 02002025 */  move  $a0, $s0
/* 03C410 8003B810 00E02825 */  move  $a1, $a3
/* 03C414 8003B814 AFA30024 */  sw    $v1, 0x24($sp)
/* 03C418 8003B818 0C0032FC */  jal   func_8000CBF0
/* 03C41C 8003B81C AFA6002C */   sw    $a2, 0x2c($sp)
/* 03C420 8003B820 8FA30024 */  lw    $v1, 0x24($sp)
/* 03C424 8003B824 8FA6002C */  lw    $a2, 0x2c($sp)
/* 03C428 8003B828 00000000 */  nop   
.L8003B82C:
/* 03C42C 8003B82C 80CE000C */  lb    $t6, 0xc($a2)
/* 03C430 8003B830 2401FFFF */  li    $at, -1
/* 03C434 8003B834 A06E000E */  sb    $t6, 0xe($v1)
/* 03C438 8003B838 90CF000E */  lbu   $t7, 0xe($a2)
/* 03C43C 8003B83C 8069000E */  lb    $t1, 0xe($v1)
/* 03C440 8003B840 A06F000F */  sb    $t7, 0xf($v1)
/* 03C444 8003B844 90D8000D */  lbu   $t8, 0xd($a2)
/* 03C448 8003B848 3C04800E */  lui   $a0, %hi(D_800E5FB4) # $a0, 0x800e
/* 03C44C 8003B84C A0780011 */  sb    $t8, 0x11($v1)
/* 03C450 8003B850 90D9000D */  lbu   $t9, 0xd($a2)
/* 03C454 8003B854 24845FB4 */  addiu $a0, %lo(D_800E5FB4) # addiu $a0, $a0, 0x5fb4
/* 03C458 8003B858 A0790010 */  sb    $t9, 0x10($v1)
/* 03C45C 8003B85C 90C8000B */  lbu   $t0, 0xb($a2)
/* 03C460 8003B860 15210007 */  bne   $t1, $at, .L8003B880
/* 03C464 8003B864 A0680012 */   sb    $t0, 0x12($v1)
/* 03C468 8003B868 AFA30024 */  sw    $v1, 0x24($sp)
/* 03C46C 8003B86C 0C032755 */  jal   xprintf
/* 03C470 8003B870 AFA6002C */   sw    $a2, 0x2c($sp)
/* 03C474 8003B874 8FA30024 */  lw    $v1, 0x24($sp)
/* 03C478 8003B878 8FA6002C */  lw    $a2, 0x2c($sp)
/* 03C47C 8003B87C 00000000 */  nop   
.L8003B880:
/* 03C480 8003B880 90CA000A */  lbu   $t2, 0xa($a2)
/* 03C484 8003B884 C6040010 */  lwc1  $f4, 0x10($s0)
/* 03C488 8003B888 A20A003A */  sb    $t2, 0x3a($s0)
/* 03C48C 8003B88C 90CC0008 */  lbu   $t4, 8($a2)
/* 03C490 8003B890 3C014120 */  li    $at, 0x41200000 # 10.000000
/* 03C494 8003B894 000C6A80 */  sll   $t5, $t4, 0xa
/* 03C498 8003B898 A60D0000 */  sh    $t5, ($s0)
/* 03C49C 8003B89C AC600008 */  sw    $zero, 8($v1)
/* 03C4A0 8003B8A0 E4640000 */  swc1  $f4, ($v1)
/* 03C4A4 8003B8A4 860E0000 */  lh    $t6, ($s0)
/* 03C4A8 8003B8A8 44811000 */  mtc1  $at, $f2
/* 03C4AC 8003B8AC AE0E0078 */  sw    $t6, 0x78($s0)
/* 03C4B0 8003B8B0 90CF0009 */  lbu   $t7, 9($a2)
/* 03C4B4 8003B8B4 3C014280 */  li    $at, 0x42800000 # 64.000000
/* 03C4B8 8003B8B8 31F8003F */  andi  $t8, $t7, 0x3f
/* 03C4BC 8003B8BC 0018CA80 */  sll   $t9, $t8, 0xa
/* 03C4C0 8003B8C0 AE19007C */  sw    $t9, 0x7c($s0)
/* 03C4C4 8003B8C4 90C90012 */  lbu   $t1, 0x12($a2)
/* 03C4C8 8003B8C8 44814000 */  mtc1  $at, $f8
/* 03C4CC 8003B8CC 44893000 */  mtc1  $t1, $f6
/* 03C4D0 8003B8D0 00000000 */  nop   
/* 03C4D4 8003B8D4 46803020 */  cvt.s.w $f0, $f6
/* 03C4D8 8003B8D8 4602003C */  c.lt.s $f0, $f2
/* 03C4DC 8003B8DC 00000000 */  nop   
/* 03C4E0 8003B8E0 45000003 */  bc1f  .L8003B8F0
/* 03C4E4 8003B8E4 00000000 */   nop   
/* 03C4E8 8003B8E8 46001006 */  mov.s $f0, $f2
/* 03C4EC 8003B8EC 00000000 */  nop   
.L8003B8F0:
/* 03C4F0 8003B8F0 46080003 */  div.s $f0, $f0, $f8
/* 03C4F4 8003B8F4 8E0A0040 */  lw    $t2, 0x40($s0)
/* 03C4F8 8003B8F8 240D0021 */  li    $t5, 33
/* 03C4FC 8003B8FC C54A000C */  lwc1  $f10, 0xc($t2)
/* 03C500 8003B900 240F0002 */  li    $t7, 2
/* 03C504 8003B904 24190014 */  li    $t9, 20
/* 03C508 8003B908 46005402 */  mul.s $f16, $f10, $f0
/* 03C50C 8003B90C E6100008 */  swc1  $f16, 8($s0)
/* 03C510 8003B910 90CB000F */  lbu   $t3, 0xf($a2)
/* 03C514 8003B914 00000000 */  nop   
/* 03C518 8003B918 A06B0013 */  sb    $t3, 0x13($v1)
/* 03C51C 8003B91C 80CC0011 */  lb    $t4, 0x11($a2)
/* 03C520 8003B920 00000000 */  nop   
/* 03C524 8003B924 A06C0014 */  sb    $t4, 0x14($v1)
/* 03C528 8003B928 8E0E004C */  lw    $t6, 0x4c($s0)
/* 03C52C 8003B92C 00000000 */  nop   
/* 03C530 8003B930 A5CD0014 */  sh    $t5, 0x14($t6)
/* 03C534 8003B934 8E18004C */  lw    $t8, 0x4c($s0)
/* 03C538 8003B938 00000000 */  nop   
/* 03C53C 8003B93C A30F0011 */  sb    $t7, 0x11($t8)
/* 03C540 8003B940 8E08004C */  lw    $t0, 0x4c($s0)
/* 03C544 8003B944 00000000 */  nop   
/* 03C548 8003B948 A1190010 */  sb    $t9, 0x10($t0)
/* 03C54C 8003B94C 8E09004C */  lw    $t1, 0x4c($s0)
/* 03C550 8003B950 00000000 */  nop   
/* 03C554 8003B954 A1200012 */  sb    $zero, 0x12($t1)
/* 03C558 8003B958 8E0B0040 */  lw    $t3, 0x40($s0)
/* 03C55C 8003B95C 820A003A */  lb    $t2, 0x3a($s0)
/* 03C560 8003B960 816C0055 */  lb    $t4, 0x55($t3)
/* 03C564 8003B964 00000000 */  nop   
/* 03C568 8003B968 014C082A */  slt   $at, $t2, $t4
/* 03C56C 8003B96C 14200003 */  bnez  $at, .L8003B97C
/* 03C570 8003B970 8FBF001C */   lw    $ra, 0x1c($sp)
/* 03C574 8003B974 A200003A */  sb    $zero, 0x3a($s0)
/* 03C578 8003B978 8FBF001C */  lw    $ra, 0x1c($sp)
.L8003B97C:
/* 03C57C 8003B97C 8FB00018 */  lw    $s0, 0x18($sp)
/* 03C580 8003B980 03E00008 */  jr    $ra
/* 03C584 8003B984 27BD0028 */   addiu $sp, $sp, 0x28

