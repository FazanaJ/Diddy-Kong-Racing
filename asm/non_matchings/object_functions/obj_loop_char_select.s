glabel obj_loop_char_select
/* 038F3C 8003833C 27BDFF88 */  addiu $sp, $sp, -0x78
/* 038F40 80038340 AFBF0024 */  sw    $ra, 0x24($sp)
/* 038F44 80038344 AFB2001C */  sw    $s2, 0x1c($sp)
/* 038F48 80038348 AFB00014 */  sw    $s0, 0x14($sp)
/* 038F4C 8003834C 00809025 */  move  $s2, $a0
/* 038F50 80038350 AFB30020 */  sw    $s3, 0x20($sp)
/* 038F54 80038354 AFB10018 */  sw    $s1, 0x18($sp)
/* 038F58 80038358 AFA5007C */  sw    $a1, 0x7c($sp)
/* 038F5C 8003835C 00008025 */  move  $s0, $zero
/* 038F60 80038360 0C007D18 */  jal   func_8001F460
/* 038F64 80038364 00003025 */   move  $a2, $zero
/* 038F68 80038368 8E430064 */  lw    $v1, 0x64($s2)
/* 038F6C 8003836C AE400074 */  sw    $zero, 0x74($s2)
/* 038F70 80038370 106000E1 */  beqz  $v1, .L800386F8
/* 038F74 80038374 8FBF0024 */   lw    $ra, 0x24($sp)
/* 038F78 80038378 824F003A */  lb    $t7, 0x3a($s2)
/* 038F7C 8003837C 8E4E0068 */  lw    $t6, 0x68($s2)
/* 038F80 80038380 000FC080 */  sll   $t8, $t7, 2
/* 038F84 80038384 01D8C821 */  addu  $t9, $t6, $t8
/* 038F88 80038388 8F220000 */  lw    $v0, ($t9)
/* 038F8C 8003838C 00000000 */  nop   
/* 038F90 80038390 104000D9 */  beqz  $v0, .L800386F8
/* 038F94 80038394 8FBF0024 */   lw    $ra, 0x24($sp)
/* 038F98 80038398 8C530000 */  lw    $s3, ($v0)
/* 038F9C 8003839C 0C027B34 */  jal   is_drumstick_unlocked
/* 038FA0 800383A0 AFA30064 */   sw    $v1, 0x64($sp)
/* 038FA4 800383A4 1040000C */  beqz  $v0, .L800383D8
/* 038FA8 800383A8 00000000 */   nop   
/* 038FAC 800383AC 0C027B2E */  jal   is_tt_unlocked
/* 038FB0 800383B0 00000000 */   nop   
/* 038FB4 800383B4 10400005 */  beqz  $v0, .L800383CC
/* 038FB8 800383B8 3C06800E */   lui   $a2, %hi(D_800DCA58) # $a2, 0x800e
/* 038FBC 800383BC 3C06800E */  lui   $a2, %hi(D_800DCA70) # $a2, 0x800e
/* 038FC0 800383C0 24C6CA70 */  addiu $a2, %lo(D_800DCA70) # addiu $a2, $a2, -0x3590
/* 038FC4 800383C4 1000000E */  b     .L80038400
/* 038FC8 800383C8 2404000A */   li    $a0, 10
.L800383CC:
/* 038FCC 800383CC 24C6CA58 */  addiu $a2, %lo(D_800DCA58) # addiu $a2, $a2, -0x35a8
/* 038FD0 800383D0 1000000B */  b     .L80038400
/* 038FD4 800383D4 24040009 */   li    $a0, 9
.L800383D8:
/* 038FD8 800383D8 0C027B2E */  jal   is_tt_unlocked
/* 038FDC 800383DC 00000000 */   nop   
/* 038FE0 800383E0 10400005 */  beqz  $v0, .L800383F8
/* 038FE4 800383E4 3C06800E */   lui   $a2, %hi(D_800DCA50) # $a2, 0x800e
/* 038FE8 800383E8 3C06800E */  lui   $a2, %hi(D_800DCA64) # $a2, 0x800e
/* 038FEC 800383EC 24C6CA64 */  addiu $a2, %lo(D_800DCA64) # addiu $a2, $a2, -0x359c
/* 038FF0 800383F0 10000003 */  b     .L80038400
/* 038FF4 800383F4 24040009 */   li    $a0, 9
.L800383F8:
/* 038FF8 800383F8 24C6CA50 */  addiu $a2, %lo(D_800DCA50) # addiu $a2, $a2, -0x35b0
/* 038FFC 800383FC 24040008 */  li    $a0, 8
.L80038400:
/* 039000 80038400 1880000E */  blez  $a0, .L8003843C
/* 039004 80038404 00002825 */   move  $a1, $zero
/* 039008 80038408 8FA90064 */  lw    $t1, 0x64($sp)
/* 03900C 8003840C 00C01021 */  addu  $v0, $a2, $zero
/* 039010 80038410 85230028 */  lh    $v1, 0x28($t1)
/* 039014 80038414 00000000 */  nop   
.L80038418:
/* 039018 80038418 904A0000 */  lbu   $t2, ($v0)
/* 03901C 8003841C 24A50001 */  addiu $a1, $a1, 1
/* 039020 80038420 146A0002 */  bne   $v1, $t2, .L8003842C
/* 039024 80038424 00A4082A */   slt   $at, $a1, $a0
/* 039028 80038428 24100001 */  li    $s0, 1
.L8003842C:
/* 03902C 8003842C 10200003 */  beqz  $at, .L8003843C
/* 039030 80038430 24420001 */   addiu $v0, $v0, 1
/* 039034 80038434 1200FFF8 */  beqz  $s0, .L80038418
/* 039038 80038438 00000000 */   nop   
.L8003843C:
/* 03903C 8003843C 1200006E */  beqz  $s0, .L800385F8
/* 039040 80038440 24A5FFFF */   addiu $a1, $a1, -1
/* 039044 80038444 240B0001 */  li    $t3, 1
/* 039048 80038448 A24B003B */  sb    $t3, 0x3b($s2)
/* 03904C 8003844C 00008025 */  move  $s0, $zero
/* 039050 80038450 00001825 */  move  $v1, $zero
/* 039054 80038454 27B10050 */  addiu $s1, $sp, 0x50
/* 039058 80038458 02002025 */  move  $a0, $s0
.L8003845C:
/* 03905C 8003845C A3A3004F */  sb    $v1, 0x4f($sp)
/* 039060 80038460 0C0270A0 */  jal   get_player_character
/* 039064 80038464 AFA50070 */   sw    $a1, 0x70($sp)
/* 039068 80038468 8FA50070 */  lw    $a1, 0x70($sp)
/* 03906C 8003846C 93A3004F */  lbu   $v1, 0x4f($sp)
/* 039070 80038470 14450006 */  bne   $v0, $a1, .L8003848C
/* 039074 80038474 24010004 */   li    $at, 4
/* 039078 80038478 02236021 */  addu  $t4, $s1, $v1
/* 03907C 8003847C 24630001 */  addiu $v1, $v1, 1
/* 039080 80038480 306D00FF */  andi  $t5, $v1, 0xff
/* 039084 80038484 01A01825 */  move  $v1, $t5
/* 039088 80038488 A1900000 */  sb    $s0, ($t4)
.L8003848C:
/* 03908C 8003848C 26100001 */  addiu $s0, $s0, 1
/* 039090 80038490 1601FFF2 */  bne   $s0, $at, .L8003845C
/* 039094 80038494 02002025 */   move  $a0, $s0
/* 039098 80038498 3C0F800E */  lui   $t7, %hi(D_800DCA7C) # $t7, 0x800e
/* 03909C 8003849C 25EFCA7C */  addiu $t7, %lo(D_800DCA7C) # addiu $t7, $t7, -0x3584
/* 0390A0 800384A0 00AF1021 */  addu  $v0, $a1, $t7
/* 0390A4 800384A4 904E0000 */  lbu   $t6, ($v0)
/* 0390A8 800384A8 8FB8007C */  lw    $t8, 0x7c($sp)
/* 0390AC 800384AC 3C0D800E */  lui   $t5, %hi(D_800DCA88) # $t5, 0x800e
/* 0390B0 800384B0 01D8C821 */  addu  $t9, $t6, $t8
/* 0390B4 800384B4 332400FF */  andi  $a0, $t9, 0xff
/* 0390B8 800384B8 28810010 */  slti  $at, $a0, 0x10
/* 0390BC 800384BC 14200009 */  bnez  $at, .L800384E4
/* 0390C0 800384C0 A0590000 */   sb    $t9, ($v0)
/* 0390C4 800384C4 3C0A800E */  lui   $t2, %hi(D_800DCA88) # $t2, 0x800e
/* 0390C8 800384C8 254ACA88 */  addiu $t2, %lo(D_800DCA88) # addiu $t2, $t2, -0x3578
/* 0390CC 800384CC 00AA4021 */  addu  $t0, $a1, $t2
/* 0390D0 800384D0 910B0000 */  lbu   $t3, ($t0)
/* 0390D4 800384D4 3089000F */  andi  $t1, $a0, 0xf
/* 0390D8 800384D8 256C0001 */  addiu $t4, $t3, 1
/* 0390DC 800384DC A0490000 */  sb    $t1, ($v0)
/* 0390E0 800384E0 A10C0000 */  sb    $t4, ($t0)
.L800384E4:
/* 0390E4 800384E4 25ADCA88 */  addiu $t5, %lo(D_800DCA88) # addiu $t5, $t5, -0x3578
/* 0390E8 800384E8 00AD4021 */  addu  $t0, $a1, $t5
/* 0390EC 800384EC 910F0000 */  lbu   $t7, ($t0)
/* 0390F0 800384F0 00603825 */  move  $a3, $v1
/* 0390F4 800384F4 01E3082A */  slt   $at, $t7, $v1
/* 0390F8 800384F8 14200002 */  bnez  $at, .L80038504
/* 0390FC 800384FC 00000000 */   nop   
/* 039100 80038500 A1000000 */  sb    $zero, ($t0)
.L80038504:
/* 039104 80038504 18E0003C */  blez  $a3, .L800385F8
/* 039108 80038508 00000000 */   nop   
/* 03910C 8003850C AFA70040 */  sw    $a3, 0x40($sp)
/* 039110 80038510 0C02709D */  jal   func_8009C274
/* 039114 80038514 AFA80044 */   sw    $t0, 0x44($sp)
/* 039118 80038518 8FA70040 */  lw    $a3, 0x40($sp)
/* 03911C 8003851C 8FA80044 */  lw    $t0, 0x44($sp)
/* 039120 80038520 00403025 */  move  $a2, $v0
/* 039124 80038524 18E0001B */  blez  $a3, .L80038594
/* 039128 80038528 00001825 */   move  $v1, $zero
/* 03912C 8003852C 27A20050 */  addiu $v0, $sp, 0x50
/* 039130 80038530 24100001 */  li    $s0, 1
.L80038534:
/* 039134 80038534 904E0000 */  lbu   $t6, ($v0)
/* 039138 80038538 24010001 */  li    $at, 1
/* 03913C 8003853C 01C6C021 */  addu  $t8, $t6, $a2
/* 039140 80038540 83190000 */  lb    $t9, ($t8)
/* 039144 80038544 02402025 */  move  $a0, $s2
/* 039148 80038548 1721000E */  bne   $t9, $at, .L80038584
/* 03914C 8003854C 24050002 */   li    $a1, 2
/* 039150 80038550 AE500074 */  sw    $s0, 0x74($s2)
/* 039154 80038554 AFA80044 */  sw    $t0, 0x44($sp)
/* 039158 80038558 AFA70040 */  sw    $a3, 0x40($sp)
/* 03915C 8003855C AFA60058 */  sw    $a2, 0x58($sp)
/* 039160 80038560 AFA30074 */  sw    $v1, 0x74($sp)
/* 039164 80038564 0C02BF0F */  jal   func_800AFC3C
/* 039168 80038568 AFA2003C */   sw    $v0, 0x3c($sp)
/* 03916C 8003856C 8FA2003C */  lw    $v0, 0x3c($sp)
/* 039170 80038570 8FA30074 */  lw    $v1, 0x74($sp)
/* 039174 80038574 8FA60058 */  lw    $a2, 0x58($sp)
/* 039178 80038578 8FA70040 */  lw    $a3, 0x40($sp)
/* 03917C 8003857C 8FA80044 */  lw    $t0, 0x44($sp)
/* 039180 80038580 00000000 */  nop   
.L80038584:
/* 039184 80038584 24630001 */  addiu $v1, $v1, 1
/* 039188 80038588 1467FFEA */  bne   $v1, $a3, .L80038534
/* 03918C 8003858C 24420001 */   addiu $v0, $v0, 1
/* 039190 80038590 00001825 */  move  $v1, $zero
.L80038594:
/* 039194 80038594 86660028 */  lh    $a2, 0x28($s3)
/* 039198 80038598 00001025 */  move  $v0, $zero
/* 03919C 8003859C 18C00015 */  blez  $a2, .L800385F4
/* 0391A0 800385A0 00000000 */   nop   
.L800385A4:
/* 0391A4 800385A4 8E690038 */  lw    $t1, 0x38($s3)
/* 0391A8 800385A8 24630001 */  addiu $v1, $v1, 1
/* 0391AC 800385AC 01222021 */  addu  $a0, $t1, $v0
/* 0391B0 800385B0 90850000 */  lbu   $a1, ($a0)
/* 0391B4 800385B4 00000000 */  nop   
/* 0391B8 800385B8 04A0000B */  bltz  $a1, .L800385E8
/* 0391BC 800385BC 28A10004 */   slti  $at, $a1, 4
/* 0391C0 800385C0 1020000A */  beqz  $at, .L800385EC
/* 0391C4 800385C4 0066082A */   slt   $at, $v1, $a2
/* 0391C8 800385C8 910A0000 */  lbu   $t2, ($t0)
/* 0391CC 800385CC 00000000 */  nop   
/* 0391D0 800385D0 022A5821 */  addu  $t3, $s1, $t2
/* 0391D4 800385D4 916C0000 */  lbu   $t4, ($t3)
/* 0391D8 800385D8 00000000 */  nop   
/* 0391DC 800385DC A08C0000 */  sb    $t4, ($a0)
/* 0391E0 800385E0 86660028 */  lh    $a2, 0x28($s3)
/* 0391E4 800385E4 00000000 */  nop   
.L800385E8:
/* 0391E8 800385E8 0066082A */  slt   $at, $v1, $a2
.L800385EC:
/* 0391EC 800385EC 1420FFED */  bnez  $at, .L800385A4
/* 0391F0 800385F0 2442000C */   addiu $v0, $v0, 0xc
.L800385F4:
/* 0391F4 800385F4 A240003B */  sb    $zero, 0x3b($s2)
.L800385F8:
/* 0391F8 800385F8 8242003B */  lb    $v0, 0x3b($s2)
/* 0391FC 800385FC 00000000 */  nop   
/* 039200 80038600 0440003D */  bltz  $v0, .L800386F8
/* 039204 80038604 8FBF0024 */   lw    $ra, 0x24($sp)
/* 039208 80038608 866D0048 */  lh    $t5, 0x48($s3)
/* 03920C 8003860C 8FA90064 */  lw    $t1, 0x64($sp)
/* 039210 80038610 004D082A */  slt   $at, $v0, $t5
/* 039214 80038614 10200037 */  beqz  $at, .L800386F4
/* 039218 80038618 000270C0 */   sll   $t6, $v0, 3
/* 03921C 8003861C 8E6F0044 */  lw    $t7, 0x44($s3)
/* 039220 80038620 912A002C */  lbu   $t2, 0x2c($t1)
/* 039224 80038624 01EEC021 */  addu  $t8, $t7, $t6
/* 039228 80038628 8F050004 */  lw    $a1, 4($t8)
/* 03922C 8003862C 24010001 */  li    $at, 1
/* 039230 80038630 24A5FFFF */  addiu $a1, $a1, -1
/* 039234 80038634 1541002F */  bne   $t2, $at, .L800386F4
/* 039238 80038638 0005C900 */   sll   $t9, $a1, 4
/* 03923C 8003863C 0C00057E */  jal   music_animation_fraction
/* 039240 80038640 AFB90070 */   sw    $t9, 0x70($sp)
/* 039244 80038644 3C013FE0 */  li    $at, 0x3FE00000 # 1.750000
/* 039248 80038648 44819800 */  mtc1  $at, $f19
/* 03924C 8003864C 44809000 */  mtc1  $zero, $f18
/* 039250 80038650 460003A1 */  cvt.d.s $f14, $f0
/* 039254 80038654 462E903C */  c.lt.d $f18, $f14
/* 039258 80038658 8FA50070 */  lw    $a1, 0x70($sp)
/* 03925C 8003865C 45000015 */  bc1f  .L800386B4
/* 039260 80038660 3C014000 */   li    $at, 0x40000000 # 2.000000
/* 039264 80038664 46327101 */  sub.d $f4, $f14, $f18
/* 039268 80038668 44854000 */  mtc1  $a1, $f8
/* 03926C 8003866C 46202320 */  cvt.s.d $f12, $f4
/* 039270 80038670 460060A1 */  cvt.d.s $f2, $f12
/* 039274 80038674 46221180 */  add.d $f6, $f2, $f2
/* 039278 80038678 46804420 */  cvt.s.w $f16, $f8
/* 03927C 8003867C 46203320 */  cvt.s.d $f12, $f6
/* 039280 80038680 46106282 */  mul.s $f10, $f12, $f16
/* 039284 80038684 460A8101 */  sub.s $f4, $f16, $f10
/* 039288 80038688 444BF800 */  cfc1  $t3, $31
/* 03928C 8003868C 00000000 */  nop   
/* 039290 80038690 35610003 */  ori   $at, $t3, 3
/* 039294 80038694 38210002 */  xori  $at, $at, 2
/* 039298 80038698 44C1F800 */  ctc1  $at, $31
/* 03929C 8003869C 00000000 */  nop   
/* 0392A0 800386A0 460021A4 */  cvt.w.s $f6, $f4
/* 0392A4 800386A4 440C3000 */  mfc1  $t4, $f6
/* 0392A8 800386A8 44CBF800 */  ctc1  $t3, $31
/* 0392AC 800386AC 10000011 */  b     .L800386F4
/* 0392B0 800386B0 A64C0018 */   sh    $t4, 0x18($s2)
.L800386B4:
/* 0392B4 800386B4 44814000 */  mtc1  $at, $f8
/* 0392B8 800386B8 44855000 */  mtc1  $a1, $f10
/* 0392BC 800386BC 46080302 */  mul.s $f12, $f0, $f8
/* 0392C0 800386C0 46805120 */  cvt.s.w $f4, $f10
/* 0392C4 800386C4 460C2182 */  mul.s $f6, $f4, $f12
/* 0392C8 800386C8 444DF800 */  cfc1  $t5, $31
/* 0392CC 800386CC 00000000 */  nop   
/* 0392D0 800386D0 35A10003 */  ori   $at, $t5, 3
/* 0392D4 800386D4 38210002 */  xori  $at, $at, 2
/* 0392D8 800386D8 44C1F800 */  ctc1  $at, $31
/* 0392DC 800386DC 00000000 */  nop   
/* 0392E0 800386E0 46003224 */  cvt.w.s $f8, $f6
/* 0392E4 800386E4 440F4000 */  mfc1  $t7, $f8
/* 0392E8 800386E8 44CDF800 */  ctc1  $t5, $31
/* 0392EC 800386EC A64F0018 */  sh    $t7, 0x18($s2)
/* 0392F0 800386F0 00000000 */  nop   
.L800386F4:
/* 0392F4 800386F4 8FBF0024 */  lw    $ra, 0x24($sp)
.L800386F8:
/* 0392F8 800386F8 8FB00014 */  lw    $s0, 0x14($sp)
/* 0392FC 800386FC 8FB10018 */  lw    $s1, 0x18($sp)
/* 039300 80038700 8FB2001C */  lw    $s2, 0x1c($sp)
/* 039304 80038704 8FB30020 */  lw    $s3, 0x20($sp)
/* 039308 80038708 03E00008 */  jr    $ra
/* 03930C 8003870C 27BD0078 */   addiu $sp, $sp, 0x78

