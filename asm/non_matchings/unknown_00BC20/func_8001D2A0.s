glabel func_8001D2A0
/* 01DEA0 8001D2A0 27BDFFD8 */  addiu $sp, $sp, -0x28
/* 01DEA4 8001D2A4 AFBF0024 */  sw    $ra, 0x24($sp)
/* 01DEA8 8001D2A8 AFB10020 */  sw    $s1, 0x20($sp)
/* 01DEAC 8001D2AC AFB0001C */  sw    $s0, 0x1c($sp)
/* 01DEB0 8001D2B0 AFA70034 */  sw    $a3, 0x34($sp)
/* 01DEB4 8001D2B4 8C900054 */  lw    $s0, 0x54($a0)
/* 01DEB8 8001D2B8 44856000 */  mtc1  $a1, $f12
/* 01DEBC 8001D2BC 44867000 */  mtc1  $a2, $f14
/* 01DEC0 8001D2C0 12000077 */  beqz  $s0, .L8001D4A0
/* 01DEC4 8001D2C4 00808825 */   move  $s1, $a0
/* 01DEC8 8001D2C8 C6040028 */  lwc1  $f4, 0x28($s0)
/* 01DECC 8001D2CC 44801000 */  mtc1  $zero, $f2
/* 01DED0 8001D2D0 460C2180 */  add.s $f6, $f4, $f12
/* 01DED4 8001D2D4 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 01DED8 8001D2D8 E6060028 */  swc1  $f6, 0x28($s0)
/* 01DEDC 8001D2DC 8C900054 */  lw    $s0, 0x54($a0)
/* 01DEE0 8001D2E0 00000000 */  nop   
/* 01DEE4 8001D2E4 C6000028 */  lwc1  $f0, 0x28($s0)
/* 01DEE8 8001D2E8 00000000 */  nop   
/* 01DEEC 8001D2EC 4602003C */  c.lt.s $f0, $f2
/* 01DEF0 8001D2F0 00000000 */  nop   
/* 01DEF4 8001D2F4 45000005 */  bc1f  .L8001D30C
/* 01DEF8 8001D2F8 00000000 */   nop   
/* 01DEFC 8001D2FC E6020028 */  swc1  $f2, 0x28($s0)
/* 01DF00 8001D300 8C900054 */  lw    $s0, 0x54($a0)
/* 01DF04 8001D304 1000000B */  b     .L8001D334
/* 01DF08 8001D308 C608002C */   lwc1  $f8, 0x2c($s0)
.L8001D30C:
/* 01DF0C 8001D30C 44816000 */  mtc1  $at, $f12
/* 01DF10 8001D310 00000000 */  nop   
/* 01DF14 8001D314 4600603C */  c.lt.s $f12, $f0
/* 01DF18 8001D318 00000000 */  nop   
/* 01DF1C 8001D31C 45000004 */  bc1f  .L8001D330
/* 01DF20 8001D320 00000000 */   nop   
/* 01DF24 8001D324 E60C0028 */  swc1  $f12, 0x28($s0)
/* 01DF28 8001D328 8E300054 */  lw    $s0, 0x54($s1)
/* 01DF2C 8001D32C 00000000 */  nop   
.L8001D330:
/* 01DF30 8001D330 C608002C */  lwc1  $f8, 0x2c($s0)
.L8001D334:
/* 01DF34 8001D334 3C014000 */  li    $at, 0x40000000 # 2.000000
/* 01DF38 8001D338 460E4280 */  add.s $f10, $f8, $f14
/* 01DF3C 8001D33C 44818000 */  mtc1  $at, $f16
/* 01DF40 8001D340 E60A002C */  swc1  $f10, 0x2c($s0)
/* 01DF44 8001D344 8E300054 */  lw    $s0, 0x54($s1)
/* 01DF48 8001D348 3C01800E */  lui   $at, %hi(D_800E5660) # $at, 0x800e
/* 01DF4C 8001D34C C600002C */  lwc1  $f0, 0x2c($s0)
/* 01DF50 8001D350 00000000 */  nop   
/* 01DF54 8001D354 4602003C */  c.lt.s $f0, $f2
/* 01DF58 8001D358 00000000 */  nop   
/* 01DF5C 8001D35C 45000006 */  bc1f  .L8001D378
/* 01DF60 8001D360 00000000 */   nop   
/* 01DF64 8001D364 E602002C */  swc1  $f2, 0x2c($s0)
/* 01DF68 8001D368 8E300054 */  lw    $s0, 0x54($s1)
/* 01DF6C 8001D36C 00000000 */  nop   
/* 01DF70 8001D370 C600002C */  lwc1  $f0, 0x2c($s0)
/* 01DF74 8001D374 00000000 */  nop   
.L8001D378:
/* 01DF78 8001D378 4600803E */  c.le.s $f16, $f0
/* 01DF7C 8001D37C 00000000 */  nop   
/* 01DF80 8001D380 45000008 */  bc1f  .L8001D3A4
/* 01DF84 8001D384 00000000 */   nop   
/* 01DF88 8001D388 C4325660 */  lwc1  $f18, %lo(D_800E5660)($at)
/* 01DF8C 8001D38C 00000000 */  nop   
/* 01DF90 8001D390 E612002C */  swc1  $f18, 0x2c($s0)
/* 01DF94 8001D394 8E300054 */  lw    $s0, 0x54($s1)
/* 01DF98 8001D398 00000000 */  nop   
/* 01DF9C 8001D39C C600002C */  lwc1  $f0, 0x2c($s0)
/* 01DFA0 8001D3A0 00000000 */  nop   
.L8001D3A4:
/* 01DFA4 8001D3A4 86080024 */  lh    $t0, 0x24($s0)
/* 01DFA8 8001D3A8 87A9003A */  lh    $t1, 0x3a($sp)
/* 01DFAC 8001D3AC 860E0022 */  lh    $t6, 0x22($s0)
/* 01DFB0 8001D3B0 8E050028 */  lw    $a1, 0x28($s0)
/* 01DFB4 8001D3B4 01095021 */  addu  $t2, $t0, $t1
/* 01DFB8 8001D3B8 87AF0036 */  lh    $t7, 0x36($sp)
/* 01DFBC 8001D3BC AFAA0010 */  sw    $t2, 0x10($sp)
/* 01DFC0 8001D3C0 860B0026 */  lh    $t3, 0x26($s0)
/* 01DFC4 8001D3C4 87AC003E */  lh    $t4, 0x3e($sp)
/* 01DFC8 8001D3C8 01CF3821 */  addu  $a3, $t6, $t7
/* 01DFCC 8001D3CC 0007C400 */  sll   $t8, $a3, 0x10
/* 01DFD0 8001D3D0 44060000 */  mfc1  $a2, $f0
/* 01DFD4 8001D3D4 016C6821 */  addu  $t5, $t3, $t4
/* 01DFD8 8001D3D8 AFAD0014 */  sw    $t5, 0x14($sp)
/* 01DFDC 8001D3DC 00183C03 */  sra   $a3, $t8, 0x10
/* 01DFE0 8001D3E0 0C00752D */  jal   func_8001D4B4
/* 01DFE4 8001D3E4 02002025 */   move  $a0, $s0
/* 01DFE8 8001D3E8 8E220040 */  lw    $v0, 0x40($s1)
/* 01DFEC 8001D3EC 00000000 */  nop   
/* 01DFF0 8001D3F0 904E003D */  lbu   $t6, 0x3d($v0)
/* 01DFF4 8001D3F4 00000000 */  nop   
/* 01DFF8 8001D3F8 11C0002A */  beqz  $t6, .L8001D4A4
/* 01DFFC 8001D3FC 8FBF0024 */   lw    $ra, 0x24($sp)
/* 01E000 8001D400 904F003A */  lbu   $t7, 0x3a($v0)
/* 01E004 8001D404 8E380054 */  lw    $t8, 0x54($s1)
/* 01E008 8001D408 00000000 */  nop   
/* 01E00C 8001D40C A30F0004 */  sb    $t7, 4($t8)
/* 01E010 8001D410 8E390040 */  lw    $t9, 0x40($s1)
/* 01E014 8001D414 8E290054 */  lw    $t1, 0x54($s1)
/* 01E018 8001D418 9328003B */  lbu   $t0, 0x3b($t9)
/* 01E01C 8001D41C 00000000 */  nop   
/* 01E020 8001D420 A1280005 */  sb    $t0, 5($t1)
/* 01E024 8001D424 8E2A0040 */  lw    $t2, 0x40($s1)
/* 01E028 8001D428 8E2C0054 */  lw    $t4, 0x54($s1)
/* 01E02C 8001D42C 914B003C */  lbu   $t3, 0x3c($t2)
/* 01E030 8001D430 00000000 */  nop   
/* 01E034 8001D434 A18B0006 */  sb    $t3, 6($t4)
/* 01E038 8001D438 8E2D0040 */  lw    $t5, 0x40($s1)
/* 01E03C 8001D43C 8E2F0054 */  lw    $t7, 0x54($s1)
/* 01E040 8001D440 91AE003D */  lbu   $t6, 0x3d($t5)
/* 01E044 8001D444 00000000 */  nop   
/* 01E048 8001D448 A1EE0007 */  sb    $t6, 7($t7)
/* 01E04C 8001D44C 8E300054 */  lw    $s0, 0x54($s1)
/* 01E050 8001D450 00000000 */  nop   
/* 01E054 8001D454 8618001C */  lh    $t8, 0x1c($s0)
/* 01E058 8001D458 00000000 */  nop   
/* 01E05C 8001D45C 0018C843 */  sra   $t9, $t8, 1
/* 01E060 8001D460 00194023 */  negu  $t0, $t9
/* 01E064 8001D464 A6080008 */  sh    $t0, 8($s0)
/* 01E068 8001D468 8E300054 */  lw    $s0, 0x54($s1)
/* 01E06C 8001D46C 00000000 */  nop   
/* 01E070 8001D470 8609001E */  lh    $t1, 0x1e($s0)
/* 01E074 8001D474 00000000 */  nop   
/* 01E078 8001D478 00095043 */  sra   $t2, $t1, 1
/* 01E07C 8001D47C 000A5823 */  negu  $t3, $t2
/* 01E080 8001D480 A60B000A */  sh    $t3, 0xa($s0)
/* 01E084 8001D484 8E300054 */  lw    $s0, 0x54($s1)
/* 01E088 8001D488 00000000 */  nop   
/* 01E08C 8001D48C 860C0020 */  lh    $t4, 0x20($s0)
/* 01E090 8001D490 00000000 */  nop   
/* 01E094 8001D494 000C6843 */  sra   $t5, $t4, 1
/* 01E098 8001D498 000D7023 */  negu  $t6, $t5
/* 01E09C 8001D49C A60E000C */  sh    $t6, 0xc($s0)
.L8001D4A0:
/* 01E0A0 8001D4A0 8FBF0024 */  lw    $ra, 0x24($sp)
.L8001D4A4:
/* 01E0A4 8001D4A4 8FB0001C */  lw    $s0, 0x1c($sp)
/* 01E0A8 8001D4A8 8FB10020 */  lw    $s1, 0x20($sp)
/* 01E0AC 8001D4AC 03E00008 */  jr    $ra
/* 01E0B0 8001D4B0 27BD0028 */   addiu $sp, $sp, 0x28