glabel audioNewThread
/* 003260 80002660 27BDFFB8 */  addiu $sp, $sp, -0x48
/* 003264 80002664 3C018011 */  lui   $at, %hi(D_80115F90) # $at, 0x8011
/* 003268 80002668 AFBF0034 */  sw    $ra, 0x34($sp)
/* 00326C 8000266C AFB40030 */  sw    $s4, 0x30($sp)
/* 003270 80002670 AFB3002C */  sw    $s3, 0x2c($sp)
/* 003274 80002674 AFB20028 */  sw    $s2, 0x28($sp)
/* 003278 80002678 AFB10024 */  sw    $s1, 0x24($sp)
/* 00327C 8000267C AFB00020 */  sw    $s0, 0x20($sp)
/* 003280 80002680 AFA5004C */  sw    $a1, 0x4c($sp)
/* 003284 80002684 AC265F90 */  sw    $a2, %lo(D_80115F90)($at)
/* 003288 80002688 8C8E0014 */  lw    $t6, 0x14($a0)
/* 00328C 8000268C 3C018011 */  lui   $at, %hi(D_80115F94) # $at, 0x8011
/* 003290 80002690 3C0F8000 */  lui   $t7, %hi(func_80003008) # $t7, 0x8000
/* 003294 80002694 25EF3008 */  addiu $t7, %lo(func_80003008) # addiu $t7, $t7, 0x3008
/* 003298 80002698 AC2E5F94 */  sw    $t6, %lo(D_80115F94)($at)
/* 00329C 8000269C 00809825 */  move  $s3, $a0
/* 0032A0 800026A0 AC8F0010 */  sw    $t7, 0x10($a0)
/* 0032A4 800026A4 0C032180 */  jal   func_800C8600
/* 0032A8 800026A8 24045622 */   li    $a0, 22050
/* 0032AC 800026AC 44822000 */  mtc1  $v0, $f4
/* 0032B0 800026B0 AE620018 */  sw    $v0, 0x18($s3)
/* 0032B4 800026B4 3C188012 */  lui   $t8, %hi(D_80126170) # $t8, 0x8012
/* 0032B8 800026B8 468021A0 */  cvt.s.w $f6, $f4
/* 0032BC 800026BC 8F186170 */  lw    $t8, %lo(D_80126170)($t8)
/* 0032C0 800026C0 3C014000 */  li    $at, 0x40000000 # 2.000000
/* 0032C4 800026C4 44814000 */  mtc1  $at, $f8
/* 0032C8 800026C8 44988000 */  mtc1  $t8, $f16
/* 0032CC 800026CC 46083282 */  mul.s $f10, $f6, $f8
/* 0032D0 800026D0 3C048012 */  lui   $a0, %hi(D_8011962C) # $a0, 0x8012
/* 0032D4 800026D4 2484962C */  addiu $a0, %lo(D_8011962C) # addiu $a0, $a0, -0x69d4
/* 0032D8 800026D8 3C148012 */  lui   $s4, %hi(D_80119630) # $s4, 0x8012
/* 0032DC 800026DC 468084A0 */  cvt.s.w $f18, $f16
/* 0032E0 800026E0 26949630 */  addiu $s4, %lo(D_80119630) # addiu $s4, $s4, -0x69d0
/* 0032E4 800026E4 46125003 */  div.s $f0, $f10, $f18
/* 0032E8 800026E8 4459F800 */  cfc1  $t9, $31
/* 0032EC 800026EC 00000000 */  nop   
/* 0032F0 800026F0 37210003 */  ori   $at, $t9, 3
/* 0032F4 800026F4 38210002 */  xori  $at, $at, 2
/* 0032F8 800026F8 44C1F800 */  ctc1  $at, $31
/* 0032FC 800026FC 00000000 */  nop   
/* 003300 80002700 46000124 */  cvt.w.s $f4, $f0
/* 003304 80002704 44032000 */  mfc1  $v1, $f4
/* 003308 80002708 44D9F800 */  ctc1  $t9, $31
/* 00330C 8000270C 44833000 */  mtc1  $v1, $f6
/* 003310 80002710 AC830000 */  sw    $v1, ($a0)
/* 003314 80002714 04610005 */  bgez  $v1, .L8000272C
/* 003318 80002718 46803220 */   cvt.s.w $f8, $f6
/* 00331C 8000271C 3C014F80 */  li    $at, 0x4F800000 # 4294967296.000000
/* 003320 80002720 44818000 */  mtc1  $at, $f16
/* 003324 80002724 00000000 */  nop   
/* 003328 80002728 46104200 */  add.s $f8, $f8, $f16
.L8000272C:
/* 00332C 8000272C 4600403C */  c.lt.s $f8, $f0
/* 003330 80002730 00000000 */  nop   
/* 003334 80002734 45000003 */  bc1f  .L80002744
/* 003338 80002738 246A0001 */   addiu $t2, $v1, 1
/* 00333C 8000273C AC8A0000 */  sw    $t2, ($a0)
/* 003340 80002740 01401825 */  move  $v1, $t2
.L80002744:
/* 003344 80002744 306B000F */  andi  $t3, $v1, 0xf
/* 003348 80002748 11600004 */  beqz  $t3, .L8000275C
/* 00334C 8000274C 2401FFF0 */   li    $at, -16
/* 003350 80002750 00616024 */  and   $t4, $v1, $at
/* 003354 80002754 25830010 */  addiu $v1, $t4, 0x10
/* 003358 80002758 AC830000 */  sw    $v1, ($a0)
.L8000275C:
/* 00335C 8000275C 246EFFF0 */  addiu $t6, $v1, -0x10
/* 003360 80002760 3C018012 */  lui   $at, %hi(D_80119628) # $at, 0x8012
/* 003364 80002764 AC2E9628 */  sw    $t6, %lo(D_80119628)($at)
/* 003368 80002768 246F0070 */  addiu $t7, $v1, 0x70
/* 00336C 8000276C AE8F0000 */  sw    $t7, ($s4)
/* 003370 80002770 9278001C */  lbu   $t8, 0x1c($s3)
/* 003374 80002774 24010006 */  li    $at, 6
/* 003378 80002778 1701001B */  bne   $t8, $at, .L800027E8
/* 00337C 8000277C 00000000 */   nop   
/* 003380 80002780 0C01DB16 */  jal   load_asset_section_from_rom
/* 003384 80002784 24040026 */   li    $a0, 38
/* 003388 80002788 8C590024 */  lw    $t9, 0x24($v0)
/* 00338C 8000278C 8C490020 */  lw    $t1, 0x20($v0)
/* 003390 80002790 3C0500FF */  lui   $a1, (0x00FFFFFF >> 16) # lui $a1, 0xff
/* 003394 80002794 03298023 */  subu  $s0, $t9, $t1
/* 003398 80002798 00409025 */  move  $s2, $v0
/* 00339C 8000279C 02002025 */  move  $a0, $s0
/* 0033A0 800027A0 0C01C327 */  jal   allocate_from_main_pool_safe
/* 0033A4 800027A4 34A5FFFF */   ori   $a1, (0x00FFFFFF & 0xFFFF) # ori $a1, $a1, 0xffff
/* 0033A8 800027A8 8E460020 */  lw    $a2, 0x20($s2)
/* 0033AC 800027AC 00408825 */  move  $s1, $v0
/* 0033B0 800027B0 24040027 */  li    $a0, 39
/* 0033B4 800027B4 00402825 */  move  $a1, $v0
/* 0033B8 800027B8 0C01DB9A */  jal   load_asset_to_address
/* 0033BC 800027BC 02003825 */   move  $a3, $s0
/* 0033C0 800027C0 3C048011 */  lui   $a0, %hi(ALGlobals_801161D0) # $a0, 0x8011
/* 0033C4 800027C4 AE710020 */  sw    $s1, 0x20($s3)
/* 0033C8 800027C8 AE600024 */  sw    $zero, 0x24($s3)
/* 0033CC 800027CC 248461D0 */  addiu $a0, %lo(ALGlobals_801161D0) # addiu $a0, $a0, 0x61d0
/* 0033D0 800027D0 0C0321FB */  jal   alInit
/* 0033D4 800027D4 02602825 */   move  $a1, $s3
/* 0033D8 800027D8 0C01C450 */  jal   free_from_memory_pool
/* 0033DC 800027DC 02202025 */   move  $a0, $s1
/* 0033E0 800027E0 10000005 */  b     .L800027F8
/* 0033E4 800027E4 00000000 */   nop   
.L800027E8:
/* 0033E8 800027E8 3C048011 */  lui   $a0, %hi(ALGlobals_801161D0) # $a0, 0x8011
/* 0033EC 800027EC 248461D0 */  addiu $a0, %lo(ALGlobals_801161D0) # addiu $a0, $a0, 0x61d0
/* 0033F0 800027F0 0C0321FB */  jal   alInit
/* 0033F4 800027F4 02602825 */   move  $a1, $s3
.L800027F8:
/* 0033F8 800027F8 3C028012 */  lui   $v0, %hi(D_80119240) # $v0, 0x8012
/* 0033FC 800027FC 24429240 */  addiu $v0, %lo(D_80119240) # addiu $v0, $v0, -0x6dc0
/* 003400 80002800 3C128012 */  lui   $s2, %hi(D_80119240) # $s2, 0x8012
/* 003404 80002804 3C108012 */  lui   $s0, %hi(D_80119240+20) # $s0, 0x8012
/* 003408 80002808 AC400004 */  sw    $zero, 4($v0)
/* 00340C 8000280C AC400000 */  sw    $zero, ($v0)
/* 003410 80002810 26109254 */  addiu $s0, %lo(D_80119240+20) # addiu $s0, $s0, -0x6dac
/* 003414 80002814 26529240 */  addiu $s2, %lo(D_80119240) # addiu $s2, $s2, -0x6dc0
/* 003418 80002818 00008825 */  move  $s1, $zero
.L8000281C:
/* 00341C 8000281C 02002025 */  move  $a0, $s0
/* 003420 80002820 0C0321E4 */  jal   alLink
/* 003424 80002824 02402825 */   move  $a1, $s2
/* 003428 80002828 8E660014 */  lw    $a2, 0x14($s3)
/* 00342C 8000282C 240A0400 */  li    $t2, 1024
/* 003430 80002830 AFAA0010 */  sw    $t2, 0x10($sp)
/* 003434 80002834 00002025 */  move  $a0, $zero
/* 003438 80002838 00002825 */  move  $a1, $zero
/* 00343C 8000283C 0C031DFC */  jal   alHeapDBAlloc
/* 003440 80002840 24070001 */   li    $a3, 1
/* 003444 80002844 26310001 */  addiu $s1, $s1, 1
/* 003448 80002848 2A210031 */  slti  $at, $s1, 0x31
/* 00344C 8000284C 26520014 */  addiu $s2, $s2, 0x14
/* 003450 80002850 26100014 */  addiu $s0, $s0, 0x14
/* 003454 80002854 1420FFF1 */  bnez  $at, .L8000281C
/* 003458 80002858 AE42FFFC */   sw    $v0, -4($s2)
/* 00345C 8000285C 8E660014 */  lw    $a2, 0x14($s3)
/* 003460 80002860 240B0400 */  li    $t3, 1024
/* 003464 80002864 AFAB0010 */  sw    $t3, 0x10($sp)
/* 003468 80002868 00002025 */  move  $a0, $zero
/* 00346C 8000286C 00002825 */  move  $a1, $zero
/* 003470 80002870 0C031DFC */  jal   alHeapDBAlloc
/* 003474 80002874 24070001 */   li    $a3, 1
/* 003478 80002878 3C048012 */  lui   $a0, %hi(D_80119638) # $a0, 0x8012
/* 00347C 8000287C 24849638 */  addiu $a0, %lo(D_80119638) # addiu $a0, $a0, -0x69c8
/* 003480 80002880 240C0400 */  li    $t4, 1024
/* 003484 80002884 3C08800E */  lui   $t0, %hi(D_800DC68C) # $t0, 0x800e
/* 003488 80002888 A48C0000 */  sh    $t4, ($a0)
/* 00348C 8000288C 84860000 */  lh    $a2, ($a0)
/* 003490 80002890 2508C68C */  addiu $t0, %lo(D_800DC68C) # addiu $t0, $t0, -0x3974
/* 003494 80002894 3C0D8002 */  lui   $t5, %hi(func_80019808) # $t5, 0x8002
/* 003498 80002898 AE420010 */  sw    $v0, 0x10($s2)
/* 00349C 8000289C 25AD9808 */  addiu $t5, %lo(func_80019808) # addiu $t5, $t5, -0x67f8
/* 0034A0 800028A0 A1000000 */  sb    $zero, ($t0)
/* 0034A4 800028A4 3C05800E */  lui   $a1, %hi(D_800DC694) # $a1, 0x800e
/* 0034A8 800028A8 8CA5C694 */  lw    $a1, %lo(D_800DC694)($a1)
/* 0034AC 800028AC 01A61823 */  subu  $v1, $t5, $a2
/* 0034B0 800028B0 00C33821 */  addu  $a3, $a2, $v1
/* 0034B4 800028B4 00008825 */  move  $s1, $zero
/* 0034B8 800028B8 18A0000B */  blez  $a1, .L800028E8
/* 0034BC 800028BC 00001825 */   move  $v1, $zero
/* 0034C0 800028C0 00E01025 */  move  $v0, $a3
.L800028C4:
/* 0034C4 800028C4 848F0000 */  lh    $t7, ($a0)
/* 0034C8 800028C8 904E0000 */  lbu   $t6, ($v0)
/* 0034CC 800028CC 26310001 */  addiu $s1, $s1, 1
/* 0034D0 800028D0 0225082A */  slt   $at, $s1, $a1
/* 0034D4 800028D4 25F80001 */  addiu $t8, $t7, 1
/* 0034D8 800028D8 24420001 */  addiu $v0, $v0, 1
/* 0034DC 800028DC A4980000 */  sh    $t8, ($a0)
/* 0034E0 800028E0 1420FFF8 */  bnez  $at, .L800028C4
/* 0034E4 800028E4 006E1821 */   addu  $v1, $v1, $t6
.L800028E8:
/* 0034E8 800028E8 3C19800E */  lui   $t9, %hi(D_800DC690) # $t9, 0x800e
/* 0034EC 800028EC 8F39C690 */  lw    $t9, %lo(D_800DC690)($t9)
/* 0034F0 800028F0 3C108011 */  lui   $s0, %hi(D_80115F98) # $s0, 0x8011
/* 0034F4 800028F4 10790003 */  beq   $v1, $t9, .L80002904
/* 0034F8 800028F8 26105F98 */   addiu $s0, %lo(D_80115F98) # addiu $s0, $s0, 0x5f98
/* 0034FC 800028FC 24090001 */  li    $t1, 1
/* 003500 80002900 A1090000 */  sb    $t1, ($t0)
.L80002904:
/* 003504 80002904 3C118011 */  lui   $s1, %hi(D_80115FA0) # $s1, 0x8011
/* 003508 80002908 26315FA0 */  addiu $s1, %lo(D_80115FA0) # addiu $s1, $s1, 0x5fa0
.L8000290C:
/* 00350C 8000290C 8E660014 */  lw    $a2, 0x14($s3)
/* 003510 80002910 340AA000 */  li    $t2, 40960
/* 003514 80002914 AFAA0010 */  sw    $t2, 0x10($sp)
/* 003518 80002918 00002025 */  move  $a0, $zero
/* 00351C 8000291C 00002825 */  move  $a1, $zero
/* 003520 80002920 0C031DFC */  jal   alHeapDBAlloc
/* 003524 80002924 24070001 */   li    $a3, 1
/* 003528 80002928 26100004 */  addiu $s0, $s0, 4
/* 00352C 8000292C 0211082B */  sltu  $at, $s0, $s1
/* 003530 80002930 1420FFF6 */  bnez  $at, .L8000290C
/* 003534 80002934 AE02FFFC */   sw    $v0, -4($s0)
/* 003538 80002938 8E820000 */  lw    $v0, ($s4)
/* 00353C 8000293C 3C0C803F */  lui   $t4, (0x803FFE00 >> 16) # lui $t4, 0x803f
/* 003540 80002940 00025880 */  sll   $t3, $v0, 2
/* 003544 80002944 01625823 */  subu  $t3, $t3, $v0
/* 003548 80002948 000B2080 */  sll   $a0, $t3, 2
/* 00354C 8000294C 358CFE00 */  ori   $t4, (0x803FFE00 & 0xFFFF) # ori $t4, $t4, 0xfe00
/* 003550 80002950 3C0600FF */  lui   $a2, (0x00FFFFFF >> 16) # lui $a2, 0xff
/* 003554 80002954 34C6FFFF */  ori   $a2, (0x00FFFFFF & 0xFFFF) # ori $a2, $a2, 0xffff
/* 003558 80002958 01842823 */  subu  $a1, $t4, $a0
/* 00355C 8000295C 0C01C3BE */  jal   allocate_at_address_in_main_pool
/* 003560 80002960 00801025 */   move  $v0, $a0
/* 003564 80002964 3C108011 */  lui   $s0, %hi(D_80115F98) # $s0, 0x8011
/* 003568 80002968 3C118011 */  lui   $s1, %hi(D_80115FA0+4) # $s1, 0x8011
/* 00356C 8000296C 00401825 */  move  $v1, $v0
/* 003570 80002970 26315FA4 */  addiu $s1, %lo(D_80115FA0+4) # addiu $s1, $s1, 0x5fa4
/* 003574 80002974 26105F98 */  addiu $s0, %lo(D_80115F98) # addiu $s0, $s0, 0x5f98
.L80002978:
/* 003578 80002978 8E660014 */  lw    $a2, 0x14($s3)
/* 00357C 8000297C 240D0078 */  li    $t5, 120
/* 003580 80002980 AFAD0010 */  sw    $t5, 0x10($sp)
/* 003584 80002984 AFA3003C */  sw    $v1, 0x3c($sp)
/* 003588 80002988 00002025 */  move  $a0, $zero
/* 00358C 8000298C 00002825 */  move  $a1, $zero
/* 003590 80002990 0C031DFC */  jal   alHeapDBAlloc
/* 003594 80002994 24070001 */   li    $a3, 1
/* 003598 80002998 8FA3003C */  lw    $v1, 0x3c($sp)
/* 00359C 8000299C AE020008 */  sw    $v0, 8($s0)
/* 0035A0 800029A0 AC430000 */  sw    $v1, ($v0)
/* 0035A4 800029A4 8E8E0000 */  lw    $t6, ($s4)
/* 0035A8 800029A8 26100004 */  addiu $s0, $s0, 4
/* 0035AC 800029AC 000E7880 */  sll   $t7, $t6, 2
/* 0035B0 800029B0 1611FFF1 */  bne   $s0, $s1, .L80002978
/* 0035B4 800029B4 006F1821 */   addu  $v1, $v1, $t7
/* 0035B8 800029B8 3C048011 */  lui   $a0, %hi(D_80116198) # $a0, 0x8011
/* 0035BC 800029BC 3C058011 */  lui   $a1, %hi(D_801161B0) # $a1, 0x8011
/* 0035C0 800029C0 24A561B0 */  addiu $a1, %lo(D_801161B0) # addiu $a1, $a1, 0x61b0
/* 0035C4 800029C4 24846198 */  addiu $a0, %lo(D_80116198) # addiu $a0, $a0, 0x6198
/* 0035C8 800029C8 0C032208 */  jal   osCreateMesgQueue
/* 0035CC 800029CC 24060008 */   li    $a2, 8
/* 0035D0 800029D0 3C048011 */  lui   $a0, %hi(OSMesgQueue_80116160) # $a0, 0x8011
/* 0035D4 800029D4 3C058011 */  lui   $a1, %hi(D_80116178) # $a1, 0x8011
/* 0035D8 800029D8 24A56178 */  addiu $a1, %lo(D_80116178) # addiu $a1, $a1, 0x6178
/* 0035DC 800029DC 24846160 */  addiu $a0, %lo(OSMesgQueue_80116160) # addiu $a0, $a0, 0x6160
/* 0035E0 800029E0 0C032208 */  jal   osCreateMesgQueue
/* 0035E4 800029E4 24060008 */   li    $a2, 8
/* 0035E8 800029E8 3C048012 */  lui   $a0, %hi(D_80119AF0) # $a0, 0x8012
/* 0035EC 800029EC 3C058012 */  lui   $a1, %hi(D_80119B08) # $a1, 0x8012
/* 0035F0 800029F0 24A59B08 */  addiu $a1, %lo(D_80119B08) # addiu $a1, $a1, -0x64f8
/* 0035F4 800029F4 24849AF0 */  addiu $a0, %lo(D_80119AF0) # addiu $a0, $a0, -0x6510
/* 0035F8 800029F8 0C032208 */  jal   osCreateMesgQueue
/* 0035FC 800029FC 24060032 */   li    $a2, 50
/* 003600 80002A00 8FB9004C */  lw    $t9, 0x4c($sp)
/* 003604 80002A04 3C188012 */  lui   $t8, %hi(D_80119230) # $t8, 0x8012
/* 003608 80002A08 27189230 */  addiu $t8, %lo(D_80119230) # addiu $t8, $t8, -0x6dd0
/* 00360C 80002A0C 3C048011 */  lui   $a0, %hi(audioThread) # $a0, 0x8011
/* 003610 80002A10 3C068000 */  lui   $a2, %hi(func_80002A98) # $a2, 0x8000
/* 003614 80002A14 24C62A98 */  addiu $a2, %lo(func_80002A98) # addiu $a2, $a2, 0x2a98
/* 003618 80002A18 24845FB0 */  addiu $a0, %lo(audioThread) # addiu $a0, $a0, 0x5fb0
/* 00361C 80002A1C AFB80010 */  sw    $t8, 0x10($sp)
/* 003620 80002A20 24050004 */  li    $a1, 4
/* 003624 80002A24 00003825 */  move  $a3, $zero
/* 003628 80002A28 0C032214 */  jal   osCreateThread
/* 00362C 80002A2C AFB90014 */   sw    $t9, 0x14($sp)
/* 003630 80002A30 8FBF0034 */  lw    $ra, 0x34($sp)
/* 003634 80002A34 8FB00020 */  lw    $s0, 0x20($sp)
/* 003638 80002A38 8FB10024 */  lw    $s1, 0x24($sp)
/* 00363C 80002A3C 8FB20028 */  lw    $s2, 0x28($sp)
/* 003640 80002A40 8FB3002C */  lw    $s3, 0x2c($sp)
/* 003644 80002A44 8FB40030 */  lw    $s4, 0x30($sp)
/* 003648 80002A48 03E00008 */  jr    $ra
/* 00364C 80002A4C 27BD0048 */   addiu $sp, $sp, 0x48

