glabel func_80045C48
/* 046848 80045C48 3C018012 */  lui   $at, %hi(gCurrentRacerInput) # $at, 0x8012
/* 04684C 80045C4C 27BDFF18 */  addiu $sp, $sp, -0xe8
/* 046850 80045C50 AC20D528 */  sw    $zero, %lo(gCurrentRacerInput)($at)
/* 046854 80045C54 3C018012 */  lui   $at, %hi(gCurrentButtonsPressed) # $at, 0x8012
/* 046858 80045C58 AC20D52C */  sw    $zero, %lo(gCurrentButtonsPressed)($at)
/* 04685C 80045C5C 3C018012 */  lui   $at, %hi(gCurrentButtonsReleased) # $at, 0x8012
/* 046860 80045C60 AC20D530 */  sw    $zero, %lo(gCurrentButtonsReleased)($at)
/* 046864 80045C64 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 046868 80045C68 AC20D534 */  sw    $zero, %lo(gCurrentStickX)($at)
/* 04686C 80045C6C AFBF002C */  sw    $ra, 0x2c($sp)
/* 046870 80045C70 AFB10024 */  sw    $s1, 0x24($sp)
/* 046874 80045C74 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 046878 80045C78 00A08825 */  move  $s1, $a1
/* 04687C 80045C7C AFB20028 */  sw    $s2, 0x28($sp)
/* 046880 80045C80 AFB00020 */  sw    $s0, 0x20($sp)
/* 046884 80045C84 E7B50018 */  swc1  $f21, 0x18($sp)
/* 046888 80045C88 E7B4001C */  swc1  $f20, 0x1c($sp)
/* 04688C 80045C8C AFA400E8 */  sw    $a0, 0xe8($sp)
/* 046890 80045C90 AFA600F0 */  sw    $a2, 0xf0($sp)
/* 046894 80045C94 AFA000C4 */  sw    $zero, 0xc4($sp)
/* 046898 80045C98 0C006E99 */  jal   func_8001BA64
/* 04689C 80045C9C AC20D538 */   sw    $zero, %lo(gCurrentStickY)($at)
/* 0468A0 80045CA0 14400005 */  bnez  $v0, .L80045CB8
/* 0468A4 80045CA4 00409025 */   move  $s2, $v0
/* 0468A8 80045CA8 240E0019 */  li    $t6, 25
/* 0468AC 80045CAC 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 0468B0 80045CB0 10000214 */  b     .L80046504
/* 0468B4 80045CB4 AC2ED534 */   sw    $t6, %lo(gCurrentStickX)($at)
.L80045CB8:
/* 0468B8 80045CB8 C62400A8 */  lwc1  $f4, 0xa8($s1)
/* 0468BC 80045CBC 3C013FF0 */  li    $at, 0x3FF00000 # 1.875000
/* 0468C0 80045CC0 44811800 */  mtc1  $at, $f3
/* 0468C4 80045CC4 44801000 */  mtc1  $zero, $f2
/* 0468C8 80045CC8 460021A1 */  cvt.d.s $f6, $f4
/* 0468CC 80045CCC 46261201 */  sub.d $f8, $f2, $f6
/* 0468D0 80045CD0 44805800 */  mtc1  $zero, $f11
/* 0468D4 80045CD4 46204520 */  cvt.s.d $f20, $f8
/* 0468D8 80045CD8 44805000 */  mtc1  $zero, $f10
/* 0468DC 80045CDC 4600A021 */  cvt.d.s $f0, $f20
/* 0468E0 80045CE0 82300192 */  lb    $s0, 0x192($s1)
/* 0468E4 80045CE4 462A003C */  c.lt.d $f0, $f10
/* 0468E8 80045CE8 00004025 */  move  $t0, $zero
/* 0468EC 80045CEC 45000004 */  bc1f  .L80045D00
/* 0468F0 80045CF0 2610FFFE */   addiu $s0, $s0, -2
/* 0468F4 80045CF4 4480A000 */  mtc1  $zero, $f20
/* 0468F8 80045CF8 00000000 */  nop   
/* 0468FC 80045CFC 4600A021 */  cvt.d.s $f0, $f20
.L80045D00:
/* 046900 80045D00 4620103C */  c.lt.d $f2, $f0
/* 046904 80045D04 27A600B0 */  addiu $a2, $sp, 0xb0
/* 046908 80045D08 45000004 */  bc1f  .L80045D1C
/* 04690C 80045D0C 27A700A0 */   addiu $a3, $sp, 0xa0
/* 046910 80045D10 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 046914 80045D14 4481A000 */  mtc1  $at, $f20
/* 046918 80045D18 00000000 */  nop   
.L80045D1C:
/* 04691C 80045D1C 06010002 */  bgez  $s0, .L80045D28
/* 046920 80045D20 27A30090 */   addiu $v1, $sp, 0x90
/* 046924 80045D24 02028021 */  addu  $s0, $s0, $v0
.L80045D28:
/* 046928 80045D28 0202082A */  slt   $at, $s0, $v0
/* 04692C 80045D2C 14200002 */  bnez  $at, .L80045D38
/* 046930 80045D30 00000000 */   nop   
/* 046934 80045D34 02028023 */  subu  $s0, $s0, $v0
.L80045D38:
/* 046938 80045D38 922501C8 */  lbu   $a1, 0x1c8($s1)
/* 04693C 80045D3C AFA8003C */  sw    $t0, 0x3c($sp)
/* 046940 80045D40 AFA70034 */  sw    $a3, 0x34($sp)
/* 046944 80045D44 AFA60038 */  sw    $a2, 0x38($sp)
/* 046948 80045D48 AFA30030 */  sw    $v1, 0x30($sp)
/* 04694C 80045D4C 0C006E87 */  jal   func_8001BA1C
/* 046950 80045D50 02002025 */   move  $a0, $s0
/* 046954 80045D54 8FA60038 */  lw    $a2, 0x38($sp)
/* 046958 80045D58 C4520010 */  lwc1  $f18, 0x10($v0)
/* 04695C 80045D5C 8FA30030 */  lw    $v1, 0x30($sp)
/* 046960 80045D60 8FA70034 */  lw    $a3, 0x34($sp)
/* 046964 80045D64 8FA8003C */  lw    $t0, 0x3c($sp)
/* 046968 80045D68 E4D20000 */  swc1  $f18, ($a2)
/* 04696C 80045D6C C4440014 */  lwc1  $f4, 0x14($v0)
/* 046970 80045D70 26100001 */  addiu $s0, $s0, 1
/* 046974 80045D74 E4E40000 */  swc1  $f4, ($a3)
/* 046978 80045D78 C4460018 */  lwc1  $f6, 0x18($v0)
/* 04697C 80045D7C 00000000 */  nop   
/* 046980 80045D80 E4660000 */  swc1  $f6, ($v1)
/* 046984 80045D84 922F01C9 */  lbu   $t7, 0x1c9($s1)
/* 046988 80045D88 00000000 */  nop   
/* 04698C 80045D8C 15E00037 */  bnez  $t7, .L80045E6C
/* 046990 80045D90 00000000 */   nop   
/* 046994 80045D94 823801CA */  lb    $t8, 0x1ca($s1)
/* 046998 80045D98 03A85021 */  addu  $t2, $sp, $t0
/* 04699C 80045D9C 0058C821 */  addu  $t9, $v0, $t8
/* 0469A0 80045DA0 8329002E */  lb    $t1, 0x2e($t9)
/* 0469A4 80045DA4 03A87021 */  addu  $t6, $sp, $t0
/* 0469A8 80045DA8 44894000 */  mtc1  $t1, $f8
/* 0469AC 80045DAC 00000000 */  nop   
/* 0469B0 80045DB0 468042A0 */  cvt.s.w $f10, $f8
/* 0469B4 80045DB4 E54A0074 */  swc1  $f10, 0x74($t2)
/* 0469B8 80045DB8 822B01CA */  lb    $t3, 0x1ca($s1)
/* 0469BC 80045DBC 00000000 */  nop   
/* 0469C0 80045DC0 004B6021 */  addu  $t4, $v0, $t3
/* 0469C4 80045DC4 818D0032 */  lb    $t5, 0x32($t4)
/* 0469C8 80045DC8 00000000 */  nop   
/* 0469CC 80045DCC 448D9000 */  mtc1  $t5, $f18
/* 0469D0 80045DD0 00000000 */  nop   
/* 0469D4 80045DD4 46809120 */  cvt.s.w $f4, $f18
/* 0469D8 80045DD8 E5C40064 */  swc1  $f4, 0x64($t6)
/* 0469DC 80045DDC 822F01CA */  lb    $t7, 0x1ca($s1)
/* 0469E0 80045DE0 C446001C */  lwc1  $f6, 0x1c($v0)
/* 0469E4 80045DE4 004FC021 */  addu  $t8, $v0, $t7
/* 0469E8 80045DE8 8319002E */  lb    $t9, 0x2e($t8)
/* 0469EC 80045DEC C4480008 */  lwc1  $f8, 8($v0)
/* 0469F0 80045DF0 44999000 */  mtc1  $t9, $f18
/* 0469F4 80045DF4 46083282 */  mul.s $f10, $f6, $f8
/* 0469F8 80045DF8 C4C80000 */  lwc1  $f8, ($a2)
/* 0469FC 80045DFC 46809120 */  cvt.s.w $f4, $f18
/* 046A00 80045E00 46045182 */  mul.s $f6, $f10, $f4
/* 046A04 80045E04 46064480 */  add.s $f18, $f8, $f6
/* 046A08 80045E08 E4D20000 */  swc1  $f18, ($a2)
/* 046A0C 80045E0C 822901CA */  lb    $t1, 0x1ca($s1)
/* 046A10 80045E10 C44A001C */  lwc1  $f10, 0x1c($v0)
/* 046A14 80045E14 00495021 */  addu  $t2, $v0, $t1
/* 046A18 80045E18 814B0032 */  lb    $t3, 0x32($t2)
/* 046A1C 80045E1C C4F20000 */  lwc1  $f18, ($a3)
/* 046A20 80045E20 448B2000 */  mtc1  $t3, $f4
/* 046A24 80045E24 00000000 */  nop   
/* 046A28 80045E28 46802220 */  cvt.s.w $f8, $f4
/* 046A2C 80045E2C 46085182 */  mul.s $f6, $f10, $f8
/* 046A30 80045E30 46069100 */  add.s $f4, $f18, $f6
/* 046A34 80045E34 E4E40000 */  swc1  $f4, ($a3)
/* 046A38 80045E38 822C01CA */  lb    $t4, 0x1ca($s1)
/* 046A3C 80045E3C C4480000 */  lwc1  $f8, ($v0)
/* 046A40 80045E40 004C6821 */  addu  $t5, $v0, $t4
/* 046A44 80045E44 81AE002E */  lb    $t6, 0x2e($t5)
/* 046A48 80045E48 C44A001C */  lwc1  $f10, 0x1c($v0)
/* 046A4C 80045E4C 448E2000 */  mtc1  $t6, $f4
/* 046A50 80045E50 46004487 */  neg.s $f18, $f8
/* 046A54 80045E54 46125182 */  mul.s $f6, $f10, $f18
/* 046A58 80045E58 C4720000 */  lwc1  $f18, ($v1)
/* 046A5C 80045E5C 46802220 */  cvt.s.w $f8, $f4
/* 046A60 80045E60 46083282 */  mul.s $f10, $f6, $f8
/* 046A64 80045E64 460A9100 */  add.s $f4, $f18, $f10
/* 046A68 80045E68 E4640000 */  swc1  $f4, ($v1)
.L80045E6C:
/* 046A6C 80045E6C 16120002 */  bne   $s0, $s2, .L80045E78
/* 046A70 80045E70 25080004 */   addiu $t0, $t0, 4
/* 046A74 80045E74 00008025 */  move  $s0, $zero
.L80045E78:
/* 046A78 80045E78 24630004 */  addiu $v1, $v1, 4
/* 046A7C 80045E7C 27AF00A0 */  addiu $t7, $sp, 0xa0
/* 046A80 80045E80 006F082B */  sltu  $at, $v1, $t7
/* 046A84 80045E84 24C60004 */  addiu $a2, $a2, 4
/* 046A88 80045E88 1420FFAB */  bnez  $at, .L80045D38
/* 046A8C 80045E8C 24E70004 */   addiu $a3, $a3, 4
/* 046A90 80045E90 922201C9 */  lbu   $v0, 0x1c9($s1)
/* 046A94 80045E94 27A400B0 */  addiu $a0, $sp, 0xb0
/* 046A98 80045E98 14400022 */  bnez  $v0, .L80045F24
/* 046A9C 80045E9C 00000000 */   nop   
/* 046AA0 80045EA0 C7A6007C */  lwc1  $f6, 0x7c($sp)
/* 046AA4 80045EA4 C7A80078 */  lwc1  $f8, 0x78($sp)
/* 046AA8 80045EA8 00000000 */  nop   
/* 046AAC 80045EAC 46083481 */  sub.s $f18, $f6, $f8
/* 046AB0 80045EB0 46149282 */  mul.s $f10, $f18, $f20
/* 046AB4 80045EB4 46085100 */  add.s $f4, $f10, $f8
/* 046AB8 80045EB8 4458F800 */  cfc1  $t8, $31
/* 046ABC 80045EBC 00000000 */  nop   
/* 046AC0 80045EC0 37010003 */  ori   $at, $t8, 3
/* 046AC4 80045EC4 38210002 */  xori  $at, $at, 2
/* 046AC8 80045EC8 44C1F800 */  ctc1  $at, $31
/* 046ACC 80045ECC 00000000 */  nop   
/* 046AD0 80045ED0 460021A4 */  cvt.w.s $f6, $f4
/* 046AD4 80045ED4 44193000 */  mfc1  $t9, $f6
/* 046AD8 80045ED8 44D8F800 */  ctc1  $t8, $31
/* 046ADC 80045EDC A63901BA */  sh    $t9, 0x1ba($s1)
/* 046AE0 80045EE0 C7AA0068 */  lwc1  $f10, 0x68($sp)
/* 046AE4 80045EE4 C7B2006C */  lwc1  $f18, 0x6c($sp)
/* 046AE8 80045EE8 00000000 */  nop   
/* 046AEC 80045EEC 460A9201 */  sub.s $f8, $f18, $f10
/* 046AF0 80045EF0 46144102 */  mul.s $f4, $f8, $f20
/* 046AF4 80045EF4 460A2180 */  add.s $f6, $f4, $f10
/* 046AF8 80045EF8 4449F800 */  cfc1  $t1, $31
/* 046AFC 80045EFC 00000000 */  nop   
/* 046B00 80045F00 35210003 */  ori   $at, $t1, 3
/* 046B04 80045F04 38210002 */  xori  $at, $at, 2
/* 046B08 80045F08 44C1F800 */  ctc1  $at, $31
/* 046B0C 80045F0C 00000000 */  nop   
/* 046B10 80045F10 460034A4 */  cvt.w.s $f18, $f6
/* 046B14 80045F14 440A9000 */  mfc1  $t2, $f18
/* 046B18 80045F18 44C9F800 */  ctc1  $t1, $31
/* 046B1C 80045F1C A62A01BC */  sh    $t2, 0x1bc($s1)
/* 046B20 80045F20 00000000 */  nop   
.L80045F24:
/* 046B24 80045F24 4406A000 */  mfc1  $a2, $f20
/* 046B28 80045F28 0C0089DF */  jal   func_8002277C
/* 046B2C 80045F2C 00002825 */   move  $a1, $zero
/* 046B30 80045F30 4406A000 */  mfc1  $a2, $f20
/* 046B34 80045F34 E7A0005C */  swc1  $f0, 0x5c($sp)
/* 046B38 80045F38 27A400A0 */  addiu $a0, $sp, 0xa0
/* 046B3C 80045F3C 0C0089DF */  jal   func_8002277C
/* 046B40 80045F40 00002825 */   move  $a1, $zero
/* 046B44 80045F44 4406A000 */  mfc1  $a2, $f20
/* 046B48 80045F48 E7A00058 */  swc1  $f0, 0x58($sp)
/* 046B4C 80045F4C 27A40090 */  addiu $a0, $sp, 0x90
/* 046B50 80045F50 0C0089DF */  jal   func_8002277C
/* 046B54 80045F54 00002825 */   move  $a1, $zero
/* 046B58 80045F58 C7A2005C */  lwc1  $f2, 0x5c($sp)
/* 046B5C 80045F5C C7AE0058 */  lwc1  $f14, 0x58($sp)
/* 046B60 80045F60 46021202 */  mul.s $f8, $f2, $f2
/* 046B64 80045F64 E7A00054 */  swc1  $f0, 0x54($sp)
/* 046B68 80045F68 460E7102 */  mul.s $f4, $f14, $f14
/* 046B6C 80045F6C 00000000 */  nop   
/* 046B70 80045F70 46000182 */  mul.s $f6, $f0, $f0
/* 046B74 80045F74 46044280 */  add.s $f10, $f8, $f4
/* 046B78 80045F78 0C0326B4 */  jal   sqrtf
/* 046B7C 80045F7C 46065300 */   add.s $f12, $f10, $f6
/* 046B80 80045F80 44809000 */  mtc1  $zero, $f18
/* 046B84 80045F84 00000000 */  nop   
/* 046B88 80045F88 46120032 */  c.eq.s $f0, $f18
/* 046B8C 80045F8C 00000000 */  nop   
/* 046B90 80045F90 4501000D */  bc1t  .L80045FC8
/* 046B94 80045F94 3C0142C8 */   li    $at, 0x42C80000 # 100.000000
/* 046B98 80045F98 44814000 */  mtc1  $at, $f8
/* 046B9C 80045F9C C7A4005C */  lwc1  $f4, 0x5c($sp)
/* 046BA0 80045FA0 46004503 */  div.s $f20, $f8, $f0
/* 046BA4 80045FA4 C7A60058 */  lwc1  $f6, 0x58($sp)
/* 046BA8 80045FA8 C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 046BAC 80045FAC 46142282 */  mul.s $f10, $f4, $f20
/* 046BB0 80045FB0 00000000 */  nop   
/* 046BB4 80045FB4 46143482 */  mul.s $f18, $f6, $f20
/* 046BB8 80045FB8 E7AA005C */  swc1  $f10, 0x5c($sp)
/* 046BBC 80045FBC 46147382 */  mul.s $f14, $f14, $f20
/* 046BC0 80045FC0 E7B20058 */  swc1  $f18, 0x58($sp)
/* 046BC4 80045FC4 E7AE0054 */  swc1  $f14, 0x54($sp)
.L80045FC8:
/* 046BC8 80045FC8 C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 046BCC 80045FCC C7AC005C */  lwc1  $f12, 0x5c($sp)
/* 046BD0 80045FD0 0C01C1D4 */  jal   arctan2_f
/* 046BD4 80045FD4 00000000 */   nop   
/* 046BD8 80045FD8 24018000 */  li    $at, -32768
/* 046BDC 80045FDC 00418021 */  addu  $s0, $v0, $at
/* 046BE0 80045FE0 3C0142C8 */  li    $at, 0x42C80000 # 100.000000
/* 046BE4 80045FE4 320BFFFF */  andi  $t3, $s0, 0xffff
/* 046BE8 80045FE8 44817000 */  mtc1  $at, $f14
/* 046BEC 80045FEC C7AC0058 */  lwc1  $f12, 0x58($sp)
/* 046BF0 80045FF0 0C01C1D4 */  jal   arctan2_f
/* 046BF4 80045FF4 01608025 */   move  $s0, $t3
/* 046BF8 80045FF8 862C01BE */  lh    $t4, 0x1be($s1)
/* 046BFC 80045FFC 862D01C0 */  lh    $t5, 0x1c0($s1)
/* 046C00 80046000 A62C01C2 */  sh    $t4, 0x1c2($s1)
/* 046C04 80046004 862E01C2 */  lh    $t6, 0x1c2($s1)
/* 046C08 80046008 34088001 */  li    $t0, 32769
/* 046C0C 8004600C 31CFFFFF */  andi  $t7, $t6, 0xffff
/* 046C10 80046010 020F2023 */  subu  $a0, $s0, $t7
/* 046C14 80046014 3045FFFF */  andi  $a1, $v0, 0xffff
/* 046C18 80046018 0088082A */  slt   $at, $a0, $t0
/* 046C1C 8004601C A63001BE */  sh    $s0, 0x1be($s1)
/* 046C20 80046020 A62501C0 */  sh    $a1, 0x1c0($s1)
/* 046C24 80046024 14200004 */  bnez  $at, .L80046038
/* 046C28 80046028 A62D01C4 */   sh    $t5, 0x1c4($s1)
/* 046C2C 8004602C 3C01FFFF */  lui   $at, (0xFFFF0001 >> 16) # lui $at, 0xffff
/* 046C30 80046030 34210001 */  ori   $at, (0xFFFF0001 & 0xFFFF) # ori $at, $at, 1
/* 046C34 80046034 00812021 */  addu  $a0, $a0, $at
.L80046038:
/* 046C38 80046038 28818000 */  slti  $at, $a0, -0x8000
/* 046C3C 8004603C 10200002 */  beqz  $at, .L80046048
/* 046C40 80046040 3401FFFF */   li    $at, 65535
/* 046C44 80046044 00812021 */  addu  $a0, $a0, $at
.L80046048:
/* 046C48 80046048 863801C4 */  lh    $t8, 0x1c4($s1)
/* 046C4C 8004604C 000448C3 */  sra   $t1, $a0, 3
/* 046C50 80046050 3319FFFF */  andi  $t9, $t8, 0xffff
/* 046C54 80046054 00B91823 */  subu  $v1, $a1, $t9
/* 046C58 80046058 0068082A */  slt   $at, $v1, $t0
/* 046C5C 8004605C 14200004 */  bnez  $at, .L80046070
/* 046C60 80046060 00095023 */   negu  $t2, $t1
/* 046C64 80046064 3C01FFFF */  lui   $at, (0xFFFF0001 >> 16) # lui $at, 0xffff
/* 046C68 80046068 34210001 */  ori   $at, (0xFFFF0001 & 0xFFFF) # ori $at, $at, 1
/* 046C6C 8004606C 00611821 */  addu  $v1, $v1, $at
.L80046070:
/* 046C70 80046070 28618000 */  slti  $at, $v1, -0x8000
/* 046C74 80046074 10200002 */  beqz  $at, .L80046080
/* 046C78 80046078 3401FFFF */   li    $at, 65535
/* 046C7C 8004607C 00611821 */  addu  $v1, $v1, $at
.L80046080:
/* 046C80 80046080 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 046C84 80046084 AC2AD534 */  sw    $t2, %lo(gCurrentStickX)($at)
/* 046C88 80046088 000358C3 */  sra   $t3, $v1, 3
/* 046C8C 8004608C 000B6023 */  negu  $t4, $t3
/* 046C90 80046090 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 046C94 80046094 AC2CD538 */  sw    $t4, %lo(gCurrentStickY)($at)
/* 046C98 80046098 862D01A0 */  lh    $t5, 0x1a0($s1)
/* 046C9C 8004609C 44804000 */  mtc1  $zero, $f8
/* 046CA0 800460A0 31AEFFFF */  andi  $t6, $t5, 0xffff
/* 046CA4 800460A4 020E2023 */  subu  $a0, $s0, $t6
/* 046CA8 800460A8 0088082A */  slt   $at, $a0, $t0
/* 046CAC 800460AC 14200003 */  bnez  $at, .L800460BC
/* 046CB0 800460B0 3C01FFFF */   lui   $at, (0xFFFF0001 >> 16) # lui $at, 0xffff
/* 046CB4 800460B4 34210001 */  ori   $at, (0xFFFF0001 & 0xFFFF) # ori $at, $at, 1
/* 046CB8 800460B8 00812021 */  addu  $a0, $a0, $at
.L800460BC:
/* 046CBC 800460BC 28818000 */  slti  $at, $a0, -0x8000
/* 046CC0 800460C0 10200002 */  beqz  $at, .L800460CC
/* 046CC4 800460C4 3401FFFF */   li    $at, 65535
/* 046CC8 800460C8 00812021 */  addu  $a0, $a0, $at
.L800460CC:
/* 046CCC 800460CC 28813001 */  slti  $at, $a0, 0x3001
/* 046CD0 800460D0 10200002 */  beqz  $at, .L800460DC
/* 046CD4 800460D4 2881D000 */   slti  $at, $a0, -0x3000
/* 046CD8 800460D8 10200002 */  beqz  $at, .L800460E4
.L800460DC:
/* 046CDC 800460DC 240F0001 */   li    $t7, 1
/* 046CE0 800460E0 AFAF00C4 */  sw    $t7, 0xc4($sp)
.L800460E4:
/* 046CE4 800460E4 C62400A8 */  lwc1  $f4, 0xa8($s1)
/* 046CE8 800460E8 3C013FF0 */  li    $at, 0x3FF00000 # 1.875000
/* 046CEC 800460EC 44814800 */  mtc1  $at, $f9
/* 046CF0 800460F0 460022A1 */  cvt.d.s $f10, $f4
/* 046CF4 800460F4 462A4181 */  sub.d $f6, $f8, $f10
/* 046CF8 800460F8 44809800 */  mtc1  $zero, $f19
/* 046CFC 800460FC 46203520 */  cvt.s.d $f20, $f6
/* 046D00 80046100 44809000 */  mtc1  $zero, $f18
/* 046D04 80046104 4600A021 */  cvt.d.s $f0, $f20
/* 046D08 80046108 82300192 */  lb    $s0, 0x192($s1)
/* 046D0C 8004610C 4632003C */  c.lt.d $f0, $f18
/* 046D10 80046110 3C013FF0 */  li    $at, 0x3FF00000 # 1.875000
/* 046D14 80046114 45000004 */  bc1f  .L80046128
/* 046D18 80046118 2610FFFE */   addiu $s0, $s0, -2
/* 046D1C 8004611C 4480A000 */  mtc1  $zero, $f20
/* 046D20 80046120 00000000 */  nop   
/* 046D24 80046124 4600A021 */  cvt.d.s $f0, $f20
.L80046128:
/* 046D28 80046128 44812800 */  mtc1  $at, $f5
/* 046D2C 8004612C 44802000 */  mtc1  $zero, $f4
/* 046D30 80046130 8FB800C4 */  lw    $t8, 0xc4($sp)
/* 046D34 80046134 4620203C */  c.lt.d $f4, $f0
/* 046D38 80046138 00000000 */  nop   
/* 046D3C 8004613C 45000003 */  bc1f  .L8004614C
/* 046D40 80046140 3C013F80 */   li    $at, 0x3F800000 # 1.000000
/* 046D44 80046144 4481A000 */  mtc1  $at, $f20
/* 046D48 80046148 00000000 */  nop   
.L8004614C:
/* 046D4C 8004614C 13000003 */  beqz  $t8, .L8004615C
/* 046D50 80046150 3C013F80 */   li    $at, 0x3F800000 # 1.000000
/* 046D54 80046154 4481A000 */  mtc1  $at, $f20
/* 046D58 80046158 00000000 */  nop   
.L8004615C:
/* 046D5C 8004615C 923901C9 */  lbu   $t9, 0x1c9($s1)
/* 046D60 80046160 00000000 */  nop   
/* 046D64 80046164 13200037 */  beqz  $t9, .L80046244
/* 046D68 80046168 00000000 */   nop   
/* 046D6C 8004616C 06010002 */  bgez  $s0, .L80046178
/* 046D70 80046170 27A600B0 */   addiu $a2, $sp, 0xb0
/* 046D74 80046174 02128021 */  addu  $s0, $s0, $s2
.L80046178:
/* 046D78 80046178 0212082A */  slt   $at, $s0, $s2
/* 046D7C 8004617C 14200002 */  bnez  $at, .L80046188
/* 046D80 80046180 27A700A0 */   addiu $a3, $sp, 0xa0
/* 046D84 80046184 02128023 */  subu  $s0, $s0, $s2
.L80046188:
/* 046D88 80046188 27A30090 */  addiu $v1, $sp, 0x90
.L8004618C:
/* 046D8C 8004618C 922501C8 */  lbu   $a1, 0x1c8($s1)
/* 046D90 80046190 AFA70034 */  sw    $a3, 0x34($sp)
/* 046D94 80046194 AFA60038 */  sw    $a2, 0x38($sp)
/* 046D98 80046198 AFA30030 */  sw    $v1, 0x30($sp)
/* 046D9C 8004619C 0C006E87 */  jal   func_8001BA1C
/* 046DA0 800461A0 02002025 */   move  $a0, $s0
/* 046DA4 800461A4 862901BA */  lh    $t1, 0x1ba($s1)
/* 046DA8 800461A8 C448001C */  lwc1  $f8, 0x1c($v0)
/* 046DAC 800461AC C44A0008 */  lwc1  $f10, 8($v0)
/* 046DB0 800461B0 44899000 */  mtc1  $t1, $f18
/* 046DB4 800461B4 460A4182 */  mul.s $f6, $f8, $f10
/* 046DB8 800461B8 C44A0010 */  lwc1  $f10, 0x10($v0)
/* 046DBC 800461BC 8FA60038 */  lw    $a2, 0x38($sp)
/* 046DC0 800461C0 8FA30030 */  lw    $v1, 0x30($sp)
/* 046DC4 800461C4 46809120 */  cvt.s.w $f4, $f18
/* 046DC8 800461C8 8FA70034 */  lw    $a3, 0x34($sp)
/* 046DCC 800461CC 26100001 */  addiu $s0, $s0, 1
/* 046DD0 800461D0 46043202 */  mul.s $f8, $f6, $f4
/* 046DD4 800461D4 27AC00A0 */  addiu $t4, $sp, 0xa0
/* 046DD8 800461D8 24C60004 */  addiu $a2, $a2, 4
/* 046DDC 800461DC 24630004 */  addiu $v1, $v1, 4
/* 046DE0 800461E0 46085480 */  add.s $f18, $f10, $f8
/* 046DE4 800461E4 E4D2FFFC */  swc1  $f18, -4($a2)
/* 046DE8 800461E8 862A01BC */  lh    $t2, 0x1bc($s1)
/* 046DEC 800461EC C446001C */  lwc1  $f6, 0x1c($v0)
/* 046DF0 800461F0 448A2000 */  mtc1  $t2, $f4
/* 046DF4 800461F4 C4520014 */  lwc1  $f18, 0x14($v0)
/* 046DF8 800461F8 468022A0 */  cvt.s.w $f10, $f4
/* 046DFC 800461FC 460A3202 */  mul.s $f8, $f6, $f10
/* 046E00 80046200 46089100 */  add.s $f4, $f18, $f8
/* 046E04 80046204 E4E40000 */  swc1  $f4, ($a3)
/* 046E08 80046208 862B01BA */  lh    $t3, 0x1ba($s1)
/* 046E0C 8004620C C44A0000 */  lwc1  $f10, ($v0)
/* 046E10 80046210 C446001C */  lwc1  $f6, 0x1c($v0)
/* 046E14 80046214 448B2000 */  mtc1  $t3, $f4
/* 046E18 80046218 46005487 */  neg.s $f18, $f10
/* 046E1C 8004621C 46123202 */  mul.s $f8, $f6, $f18
/* 046E20 80046220 C4520018 */  lwc1  $f18, 0x18($v0)
/* 046E24 80046224 468022A0 */  cvt.s.w $f10, $f4
/* 046E28 80046228 460A4182 */  mul.s $f6, $f8, $f10
/* 046E2C 8004622C 46069100 */  add.s $f4, $f18, $f6
/* 046E30 80046230 16120002 */  bne   $s0, $s2, .L8004623C
/* 046E34 80046234 E464FFFC */   swc1  $f4, -4($v1)
/* 046E38 80046238 00008025 */  move  $s0, $zero
.L8004623C:
/* 046E3C 8004623C 146CFFD3 */  bne   $v1, $t4, .L8004618C
/* 046E40 80046240 24E70004 */   addiu $a3, $a3, 4
.L80046244:
/* 046E44 80046244 4406A000 */  mfc1  $a2, $f20
/* 046E48 80046248 27A400B0 */  addiu $a0, $sp, 0xb0
/* 046E4C 8004624C 00002825 */  move  $a1, $zero
/* 046E50 80046250 0C00898F */  jal   func_8002263C
/* 046E54 80046254 27A7005C */   addiu $a3, $sp, 0x5c
/* 046E58 80046258 4406A000 */  mfc1  $a2, $f20
/* 046E5C 8004625C E7A0008C */  swc1  $f0, 0x8c($sp)
/* 046E60 80046260 27A400A0 */  addiu $a0, $sp, 0xa0
/* 046E64 80046264 00002825 */  move  $a1, $zero
/* 046E68 80046268 0C00898F */  jal   func_8002263C
/* 046E6C 8004626C 27A70058 */   addiu $a3, $sp, 0x58
/* 046E70 80046270 4406A000 */  mfc1  $a2, $f20
/* 046E74 80046274 E7A00088 */  swc1  $f0, 0x88($sp)
/* 046E78 80046278 27A40090 */  addiu $a0, $sp, 0x90
/* 046E7C 8004627C 00002825 */  move  $a1, $zero
/* 046E80 80046280 0C00898F */  jal   func_8002263C
/* 046E84 80046284 27A70054 */   addiu $a3, $sp, 0x54
/* 046E88 80046288 C7A2005C */  lwc1  $f2, 0x5c($sp)
/* 046E8C 8004628C C7B00058 */  lwc1  $f16, 0x58($sp)
/* 046E90 80046290 46021202 */  mul.s $f8, $f2, $f2
/* 046E94 80046294 C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 046E98 80046298 E7A00084 */  swc1  $f0, 0x84($sp)
/* 046E9C 8004629C 46108282 */  mul.s $f10, $f16, $f16
/* 046EA0 800462A0 00000000 */  nop   
/* 046EA4 800462A4 460E7182 */  mul.s $f6, $f14, $f14
/* 046EA8 800462A8 460A4480 */  add.s $f18, $f8, $f10
/* 046EAC 800462AC 0C0326B4 */  jal   sqrtf
/* 046EB0 800462B0 46069300 */   add.s $f12, $f18, $f6
/* 046EB4 800462B4 44802000 */  mtc1  $zero, $f4
/* 046EB8 800462B8 3C0143FA */  li    $at, 0x43FA0000 # 500.000000
/* 046EBC 800462BC 46040032 */  c.eq.s $f0, $f4
/* 046EC0 800462C0 00000000 */  nop   
/* 046EC4 800462C4 4501000E */  bc1t  .L80046300
/* 046EC8 800462C8 8FA200E8 */   lw    $v0, 0xe8($sp)
/* 046ECC 800462CC 44814000 */  mtc1  $at, $f8
/* 046ED0 800462D0 C7A2005C */  lwc1  $f2, 0x5c($sp)
/* 046ED4 800462D4 46004503 */  div.s $f20, $f8, $f0
/* 046ED8 800462D8 C7AA0058 */  lwc1  $f10, 0x58($sp)
/* 046EDC 800462DC C7AE0054 */  lwc1  $f14, 0x54($sp)
/* 046EE0 800462E0 46141082 */  mul.s $f2, $f2, $f20
/* 046EE4 800462E4 00000000 */  nop   
/* 046EE8 800462E8 46145482 */  mul.s $f18, $f10, $f20
/* 046EEC 800462EC E7A2005C */  swc1  $f2, 0x5c($sp)
/* 046EF0 800462F0 46147382 */  mul.s $f14, $f14, $f20
/* 046EF4 800462F4 E7B20058 */  swc1  $f18, 0x58($sp)
/* 046EF8 800462F8 E7AE0054 */  swc1  $f14, 0x54($sp)
/* 046EFC 800462FC 8FA200E8 */  lw    $v0, 0xe8($sp)
.L80046300:
/* 046F00 80046300 C7A6008C */  lwc1  $f6, 0x8c($sp)
/* 046F04 80046304 C7A4005C */  lwc1  $f4, 0x5c($sp)
/* 046F08 80046308 C44A000C */  lwc1  $f10, 0xc($v0)
/* 046F0C 8004630C 46043200 */  add.s $f8, $f6, $f4
/* 046F10 80046310 C7A40058 */  lwc1  $f4, 0x58($sp)
/* 046F14 80046314 460A4301 */  sub.s $f12, $f8, $f10
/* 046F18 80046318 C7A60088 */  lwc1  $f6, 0x88($sp)
/* 046F1C 8004631C E7AC005C */  swc1  $f12, 0x5c($sp)
/* 046F20 80046320 C44A0010 */  lwc1  $f10, 0x10($v0)
/* 046F24 80046324 46043200 */  add.s $f8, $f6, $f4
/* 046F28 80046328 C7A40084 */  lwc1  $f4, 0x84($sp)
/* 046F2C 8004632C 460A4181 */  sub.s $f6, $f8, $f10
/* 046F30 80046330 C7A80054 */  lwc1  $f8, 0x54($sp)
/* 046F34 80046334 E7A60058 */  swc1  $f6, 0x58($sp)
/* 046F38 80046338 C4460014 */  lwc1  $f6, 0x14($v0)
/* 046F3C 8004633C 46082280 */  add.s $f10, $f4, $f8
/* 046F40 80046340 46065381 */  sub.s $f14, $f10, $f6
/* 046F44 80046344 0C01C1D4 */  jal   arctan2_f
/* 046F48 80046348 E7AE0054 */   swc1  $f14, 0x54($sp)
/* 046F4C 8004634C 24018000 */  li    $at, -32768
/* 046F50 80046350 00418021 */  addu  $s0, $v0, $at
/* 046F54 80046354 3C0143FA */  li    $at, 0x43FA0000 # 500.000000
/* 046F58 80046358 320DFFFF */  andi  $t5, $s0, 0xffff
/* 046F5C 8004635C 44817000 */  mtc1  $at, $f14
/* 046F60 80046360 C7AC0058 */  lwc1  $f12, 0x58($sp)
/* 046F64 80046364 0C01C1D4 */  jal   arctan2_f
/* 046F68 80046368 01A08025 */   move  $s0, $t5
/* 046F6C 8004636C 862501A0 */  lh    $a1, 0x1a0($s1)
/* 046F70 80046370 34018001 */  li    $at, 32769
/* 046F74 80046374 30AEFFFF */  andi  $t6, $a1, 0xffff
/* 046F78 80046378 020E2023 */  subu  $a0, $s0, $t6
/* 046F7C 8004637C 0081082A */  slt   $at, $a0, $at
/* 046F80 80046380 14200004 */  bnez  $at, .L80046394
/* 046F84 80046384 24070001 */   li    $a3, 1
/* 046F88 80046388 3C01FFFF */  lui   $at, (0xFFFF0001 >> 16) # lui $at, 0xffff
/* 046F8C 8004638C 34210001 */  ori   $at, (0xFFFF0001 & 0xFFFF) # ori $at, $at, 1
/* 046F90 80046390 00812021 */  addu  $a0, $a0, $at
.L80046394:
/* 046F94 80046394 28818000 */  slti  $at, $a0, -0x8000
/* 046F98 80046398 10200002 */  beqz  $at, .L800463A4
/* 046F9C 8004639C 3401FFFF */   li    $at, 65535
/* 046FA0 800463A0 00812021 */  addu  $a0, $a0, $at
.L800463A4:
/* 046FA4 800463A4 8FB800E8 */  lw    $t8, 0xe8($sp)
/* 046FA8 800463A8 304FFFFF */  andi  $t7, $v0, 0xffff
/* 046FAC 800463AC 87190002 */  lh    $t9, 2($t8)
/* 046FB0 800463B0 34018001 */  li    $at, 32769
/* 046FB4 800463B4 3329FFFF */  andi  $t1, $t9, 0xffff
/* 046FB8 800463B8 01E91823 */  subu  $v1, $t7, $t1
/* 046FBC 800463BC 0061082A */  slt   $at, $v1, $at
/* 046FC0 800463C0 14200004 */  bnez  $at, .L800463D4
/* 046FC4 800463C4 00042023 */   negu  $a0, $a0
/* 046FC8 800463C8 3C01FFFF */  lui   $at, (0xFFFF0001 >> 16) # lui $at, 0xffff
/* 046FCC 800463CC 34210001 */  ori   $at, (0xFFFF0001 & 0xFFFF) # ori $at, $at, 1
/* 046FD0 800463D0 00611821 */  addu  $v1, $v1, $at
.L800463D4:
/* 046FD4 800463D4 28618000 */  slti  $at, $v1, -0x8000
/* 046FD8 800463D8 10200003 */  beqz  $at, .L800463E8
/* 046FDC 800463DC 24020005 */   li    $v0, 5
/* 046FE0 800463E0 3401FFFF */  li    $at, 65535
/* 046FE4 800463E4 00611821 */  addu  $v1, $v1, $at
.L800463E8:
/* 046FE8 800463E8 822601D6 */  lb    $a2, 0x1d6($s1)
/* 046FEC 800463EC 24010002 */  li    $at, 2
/* 046FF0 800463F0 14C10002 */  bne   $a2, $at, .L800463FC
/* 046FF4 800463F4 00031823 */   negu  $v1, $v1
/* 046FF8 800463F8 24020006 */  li    $v0, 6
.L800463FC:
/* 046FFC 800463FC 14E60007 */  bne   $a3, $a2, .L8004641C
/* 047000 80046400 3C018012 */   lui   $at, 0x8012
/* 047004 80046404 822A01D4 */  lb    $t2, 0x1d4($s1)
/* 047008 80046408 00045843 */  sra   $t3, $a0, 1
/* 04700C 8004640C 15400003 */  bnez  $t2, .L8004641C
/* 047010 80046410 00AB6023 */   subu  $t4, $a1, $t3
/* 047014 80046414 822601D6 */  lb    $a2, 0x1d6($s1)
/* 047018 80046418 A62C01A0 */  sh    $t4, 0x1a0($s1)
.L8004641C:
/* 04701C 8004641C 10C70019 */  beq   $a2, $a3, .L80046484
/* 047020 80046420 3C0A8012 */   lui   $t2, %hi(gCurrentStickX) # $t2, 0x8012
/* 047024 80046424 24010004 */  li    $at, 4
/* 047028 80046428 10C10013 */  beq   $a2, $at, .L80046478
/* 04702C 8004642C 24010005 */   li    $at, 5
/* 047030 80046430 10C1001B */  beq   $a2, $at, .L800464A0
/* 047034 80046434 3C0D8012 */   lui   $t5, %hi(gCurrentStickX) # $t5, 0x8012
/* 047038 80046438 24010007 */  li    $at, 7
/* 04703C 8004643C 10C10021 */  beq   $a2, $at, .L800464C4
/* 047040 80046440 3C0F8012 */   lui   $t7, %hi(gCurrentStickX) # $t7, 0x8012
/* 047044 80046444 3C0D8012 */  lui   $t5, %hi(gCurrentStickX) # $t5, 0x8012
/* 047048 80046448 8DADD534 */  lw    $t5, %lo(gCurrentStickX)($t5)
/* 04704C 8004644C 00447007 */  srav  $t6, $a0, $v0
/* 047050 80046450 3C198012 */  lui   $t9, %hi(gCurrentStickY) # $t9, 0x8012
/* 047054 80046454 8F39D538 */  lw    $t9, %lo(gCurrentStickY)($t9)
/* 047058 80046458 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 04705C 8004645C 01AEC021 */  addu  $t8, $t5, $t6
/* 047060 80046460 AC38D534 */  sw    $t8, %lo(gCurrentStickX)($at)
/* 047064 80046464 00437807 */  srav  $t7, $v1, $v0
/* 047068 80046468 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 04706C 8004646C 032F4821 */  addu  $t1, $t9, $t7
/* 047070 80046470 10000020 */  b     .L800464F4
/* 047074 80046474 AC29D538 */   sw    $t1, %lo(gCurrentStickY)($at)
.L80046478:
/* 047078 80046478 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 04707C 8004647C 1000001D */  b     .L800464F4
/* 047080 80046480 AC20D538 */   sw    $zero, %lo(gCurrentStickY)($at)
.L80046484:
/* 047084 80046484 8D4AD534 */  lw    $t2, %lo(gCurrentStickX)($t2)
/* 047088 80046488 AC20D538 */  sw    $zero, %lo(gCurrentStickY)($at)
/* 04708C 8004648C 00445807 */  srav  $t3, $a0, $v0
/* 047090 80046490 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 047094 80046494 014B6021 */  addu  $t4, $t2, $t3
/* 047098 80046498 10000016 */  b     .L800464F4
/* 04709C 8004649C AC2CD534 */   sw    $t4, %lo(gCurrentStickX)($at)
.L800464A0:
/* 0470A0 800464A0 8DADD534 */  lw    $t5, %lo(gCurrentStickX)($t5)
/* 0470A4 800464A4 244E001F */  addiu $t6, $v0, 0x1f
/* 0470A8 800464A8 01C4C007 */  srav  $t8, $a0, $t6
/* 0470AC 800464AC 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 0470B0 800464B0 01B8C821 */  addu  $t9, $t5, $t8
/* 0470B4 800464B4 AC39D534 */  sw    $t9, %lo(gCurrentStickX)($at)
/* 0470B8 800464B8 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 0470BC 800464BC 1000000D */  b     .L800464F4
/* 0470C0 800464C0 AC20D538 */   sw    $zero, %lo(gCurrentStickY)($at)
.L800464C4:
/* 0470C4 800464C4 8DEFD534 */  lw    $t7, %lo(gCurrentStickX)($t7)
/* 0470C8 800464C8 2442FFFF */  addiu $v0, $v0, -1
/* 0470CC 800464CC 00444807 */  srav  $t1, $a0, $v0
/* 0470D0 800464D0 3C0B8012 */  lui   $t3, %hi(gCurrentStickY) # $t3, 0x8012
/* 0470D4 800464D4 8D6BD538 */  lw    $t3, %lo(gCurrentStickY)($t3)
/* 0470D8 800464D8 3C018012 */  lui   $at, %hi(gCurrentStickX) # $at, 0x8012
/* 0470DC 800464DC 01E95021 */  addu  $t2, $t7, $t1
/* 0470E0 800464E0 AC2AD534 */  sw    $t2, %lo(gCurrentStickX)($at)
/* 0470E4 800464E4 00436007 */  srav  $t4, $v1, $v0
/* 0470E8 800464E8 3C018012 */  lui   $at, %hi(gCurrentStickY) # $at, 0x8012
/* 0470EC 800464EC 016C7021 */  addu  $t6, $t3, $t4
/* 0470F0 800464F0 AC2ED538 */  sw    $t6, %lo(gCurrentStickY)($at)
.L800464F4:
/* 0470F4 800464F4 8FA400E8 */  lw    $a0, 0xe8($sp)
/* 0470F8 800464F8 8FA600F0 */  lw    $a2, 0xf0($sp)
/* 0470FC 800464FC 0C010B48 */  jal   func_80042D20
/* 047100 80046500 02202825 */   move  $a1, $s1
.L80046504:
/* 047104 80046504 8FBF002C */  lw    $ra, 0x2c($sp)
/* 047108 80046508 C7B50018 */  lwc1  $f21, 0x18($sp)
/* 04710C 8004650C C7B4001C */  lwc1  $f20, 0x1c($sp)
/* 047110 80046510 8FB00020 */  lw    $s0, 0x20($sp)
/* 047114 80046514 8FB10024 */  lw    $s1, 0x24($sp)
/* 047118 80046518 8FB20028 */  lw    $s2, 0x28($sp)
/* 04711C 8004651C 03E00008 */  jr    $ra
/* 047120 80046520 27BD00E8 */   addiu $sp, $sp, 0xe8

