glabel func_80098774
/* 099374 80098774 27BDFFC8 */  addiu $sp, $sp, -0x38
/* 099378 80098778 AFBF0034 */  sw    $ra, 0x34($sp)
/* 09937C 8009877C AFB60030 */  sw    $s6, 0x30($sp)
/* 099380 80098780 0080B025 */  move  $s6, $a0
/* 099384 80098784 AFB5002C */  sw    $s5, 0x2c($sp)
/* 099388 80098788 AFB40028 */  sw    $s4, 0x28($sp)
/* 09938C 8009878C AFB30024 */  sw    $s3, 0x24($sp)
/* 099390 80098790 AFB20020 */  sw    $s2, 0x20($sp)
/* 099394 80098794 AFB1001C */  sw    $s1, 0x1c($sp)
/* 099398 80098798 0C01BAA4 */  jal   get_settings
/* 09939C 8009879C AFB00018 */   sw    $s0, 0x18($sp)
/* 0993A0 800987A0 12C00007 */  beqz  $s6, .L800987C0
/* 0993A4 800987A4 3C05800E */   lui   $a1, %hi(D_800E1048) # $a1, 0x800e
/* 0993A8 800987A8 3C0E800E */  lui   $t6, %hi(gMenuText) # $t6, 0x800e
/* 0993AC 800987AC 8DCEF4A0 */  lw    $t6, %lo(gMenuText)($t6)
/* 0993B0 800987B0 00000000 */  nop   
/* 0993B4 800987B4 8DC30084 */  lw    $v1, 0x84($t6)
/* 0993B8 800987B8 10000006 */  b     .L800987D4
/* 0993BC 800987BC 00000000 */   nop   
.L800987C0:
/* 0993C0 800987C0 3C0F800E */  lui   $t7, %hi(gMenuText) # $t7, 0x800e
/* 0993C4 800987C4 8DEFF4A0 */  lw    $t7, %lo(gMenuText)($t7)
/* 0993C8 800987C8 00000000 */  nop   
/* 0993CC 800987CC 8DE3007C */  lw    $v1, 0x7c($t7)
/* 0993D0 800987D0 00000000 */  nop   
.L800987D4:
/* 0993D4 800987D4 24A51048 */  addiu $a1, %lo(D_800E1048) # addiu $a1, $a1, 0x1048
/* 0993D8 800987D8 ACA30014 */  sw    $v1, 0x14($a1)
/* 0993DC 800987DC ACA30034 */  sw    $v1, 0x34($a1)
/* 0993E0 800987E0 3C09800E */  lui   $t1, %hi(gRankingPlayerCount) # $t1, 0x800e
/* 0993E4 800987E4 8D290FE4 */  lw    $t1, %lo(gRankingPlayerCount)($t1)
/* 0993E8 800987E8 3C198000 */  lui   $t9, %hi(osTvType) # $t9, 0x8000
/* 0993EC 800987EC 8F390300 */  lw    $t9, %lo(osTvType)($t9)
/* 0993F0 800987F0 2528FFFF */  addiu $t0, $t1, -1
/* 0993F4 800987F4 0008C040 */  sll   $t8, $t0, 1
/* 0993F8 800987F8 03004025 */  move  $t0, $t8
/* 0993FC 800987FC 17200003 */  bnez  $t9, .L8009880C
/* 099400 80098800 240700F0 */   li    $a3, 240
/* 099404 80098804 27080010 */  addiu $t0, $t8, 0x10
/* 099408 80098808 24070108 */  li    $a3, 264
.L8009880C:
/* 09940C 8009880C 3C03800E */  lui   $v1, %hi(D_800E14BC) # $v1, 0x800e
/* 099410 80098810 00087080 */  sll   $t6, $t0, 2
/* 099414 80098814 246314BC */  addiu $v1, %lo(D_800E14BC) # addiu $v1, $v1, 0x14bc
/* 099418 80098818 25080001 */  addiu $t0, $t0, 1
/* 09941C 8009881C 006E7821 */  addu  $t7, $v1, $t6
/* 099420 80098820 0008C080 */  sll   $t8, $t0, 2
/* 099424 80098824 8DE60000 */  lw    $a2, ($t7)
/* 099428 80098828 0078C821 */  addu  $t9, $v1, $t8
/* 09942C 8009882C 8F240000 */  lw    $a0, ($t9)
/* 099430 80098830 10C0006F */  beqz  $a2, .L800989F0
/* 099434 80098834 24100002 */   li    $s0, 2
/* 099438 80098838 1080006E */  beqz  $a0, .L800989F4
/* 09943C 8009883C 00107140 */   sll   $t6, $s0, 5
/* 099440 80098840 1920006B */  blez  $t1, .L800989F0
/* 099444 80098844 00004025 */   move  $t0, $zero
/* 099448 80098848 00107140 */  sll   $t6, $s0, 5
/* 09944C 8009884C 00107940 */  sll   $t7, $s0, 5
/* 099450 80098850 3C15800E */  lui   $s5, %hi(gTrophyRacePointsArray) # $s5, 0x800e
/* 099454 80098854 3C148012 */  lui   $s4, %hi(D_80126428) # $s4, 0x8012
/* 099458 80098858 3C138012 */  lui   $s3, %hi(D_80126430) # $s3, 0x8012
/* 09945C 8009885C 3C0D800E */  lui   $t5, %hi(gRacerPortraits) # $t5, 0x800e
/* 099460 80098860 25AD0AF0 */  addiu $t5, %lo(gRacerPortraits) # addiu $t5, $t5, 0xaf0
/* 099464 80098864 26736430 */  addiu $s3, %lo(D_80126430) # addiu $s3, $s3, 0x6430
/* 099468 80098868 26946428 */  addiu $s4, %lo(D_80126428) # addiu $s4, $s4, 0x6428
/* 09946C 8009886C 26B51004 */  addiu $s5, %lo(gTrophyRacePointsArray) # addiu $s5, $s5, 0x1004
/* 099470 80098870 00AF5821 */  addu  $t3, $a1, $t7
/* 099474 80098874 00AE6021 */  addu  $t4, $a1, $t6
/* 099478 80098878 31310003 */  andi  $s1, $t1, 3
/* 09947C 8009887C 241200FF */  li    $s2, 255
/* 099480 80098880 241F0018 */  li    $ra, 24
.L80098884:
/* 099484 80098884 00002825 */  move  $a1, $zero
/* 099488 80098888 01801825 */  move  $v1, $t4
.L8009888C:
/* 09948C 8009888C 84D80000 */  lh    $t8, ($a2)
/* 099490 80098890 29210005 */  slti  $at, $t1, 5
/* 099494 80098894 A4780000 */  sh    $t8, ($v1)
/* 099498 80098898 84D90000 */  lh    $t9, ($a2)
/* 09949C 8009889C 24C60002 */  addiu $a2, $a2, 2
/* 0994A0 800988A0 A4790004 */  sh    $t9, 4($v1)
/* 0994A4 800988A4 84CEFFFE */  lh    $t6, -2($a2)
/* 0994A8 800988A8 14200011 */  bnez  $at, .L800988F0
/* 0994AC 800988AC A46E0008 */   sh    $t6, 8($v1)
/* 0994B0 800988B0 00097843 */  sra   $t7, $t1, 1
/* 0994B4 800988B4 010F082A */  slt   $at, $t0, $t7
/* 0994B8 800988B8 1420000D */  bnez  $at, .L800988F0
/* 0994BC 800988BC 00000000 */   nop   
/* 0994C0 800988C0 84980000 */  lh    $t8, ($a0)
/* 0994C4 800988C4 24840002 */  addiu $a0, $a0, 2
/* 0994C8 800988C8 0307C823 */  subu  $t9, $t8, $a3
/* 0994CC 800988CC A4790002 */  sh    $t9, 2($v1)
/* 0994D0 800988D0 848EFFFE */  lh    $t6, -2($a0)
/* 0994D4 800988D4 00000000 */  nop   
/* 0994D8 800988D8 A46E0006 */  sh    $t6, 6($v1)
/* 0994DC 800988DC 848FFFFE */  lh    $t7, -2($a0)
/* 0994E0 800988E0 00000000 */  nop   
/* 0994E4 800988E4 01E7C021 */  addu  $t8, $t7, $a3
/* 0994E8 800988E8 1000000C */  b     .L8009891C
/* 0994EC 800988EC A478000A */   sh    $t8, 0xa($v1)
.L800988F0:
/* 0994F0 800988F0 84990000 */  lh    $t9, ($a0)
/* 0994F4 800988F4 24840002 */  addiu $a0, $a0, 2
/* 0994F8 800988F8 03277021 */  addu  $t6, $t9, $a3
/* 0994FC 800988FC A46E0002 */  sh    $t6, 2($v1)
/* 099500 80098900 848FFFFE */  lh    $t7, -2($a0)
/* 099504 80098904 00000000 */  nop   
/* 099508 80098908 A46F0006 */  sh    $t7, 6($v1)
/* 09950C 8009890C 8498FFFE */  lh    $t8, -2($a0)
/* 099510 80098910 00000000 */  nop   
/* 099514 80098914 0307C823 */  subu  $t9, $t8, $a3
/* 099518 80098918 A479000A */  sh    $t9, 0xa($v1)
.L8009891C:
/* 09951C 8009891C 24A50020 */  addiu $a1, $a1, 0x20
/* 099520 80098920 28A10060 */  slti  $at, $a1, 0x60
/* 099524 80098924 1420FFD9 */  bnez  $at, .L8009888C
/* 099528 80098928 24630020 */   addiu $v1, $v1, 0x20
/* 09952C 8009892C 12200006 */  beqz  $s1, .L80098948
/* 099530 80098930 26100003 */   addiu $s0, $s0, 3
/* 099534 80098934 29010003 */  slti  $at, $t0, 3
/* 099538 80098938 14200004 */  bnez  $at, .L8009894C
/* 09953C 8009893C 01005025 */   move  $t2, $t0
/* 099540 80098940 10000002 */  b     .L8009894C
/* 099544 80098944 250AFFFD */   addiu $t2, $t0, -3
.L80098948:
/* 099548 80098948 310A0003 */  andi  $t2, $t0, 3
.L8009894C:
/* 09954C 8009894C 000A7180 */  sll   $t6, $t2, 6
/* 099550 80098950 024E7823 */  subu  $t7, $s2, $t6
/* 099554 80098954 12C00010 */  beqz  $s6, .L80098998
/* 099558 80098958 A16F004D */   sb    $t7, 0x4d($t3)
/* 09955C 8009895C 0268C021 */  addu  $t8, $s3, $t0
/* 099560 80098960 93190000 */  lbu   $t9, ($t8)
/* 099564 80098964 00000000 */  nop   
/* 099568 80098968 033F0019 */  multu $t9, $ra
/* 09956C 8009896C 00007012 */  mflo  $t6
/* 099570 80098970 004E1821 */  addu  $v1, $v0, $t6
/* 099574 80098974 806F0059 */  lb    $t7, 0x59($v1)
/* 099578 80098978 00000000 */  nop   
/* 09957C 8009897C 000FC080 */  sll   $t8, $t7, 2
/* 099580 80098980 01B8C821 */  addu  $t9, $t5, $t8
/* 099584 80098984 8F2E0000 */  lw    $t6, ($t9)
/* 099588 80098988 246F0054 */  addiu $t7, $v1, 0x54
/* 09958C 8009898C AD6F0054 */  sw    $t7, 0x54($t3)
/* 099590 80098990 10000010 */  b     .L800989D4
/* 099594 80098994 AD6E0014 */   sw    $t6, 0x14($t3)
.L80098998:
/* 099598 80098998 0288C021 */  addu  $t8, $s4, $t0
/* 09959C 8009899C 93190000 */  lbu   $t9, ($t8)
/* 0995A0 800989A0 00000000 */  nop   
/* 0995A4 800989A4 033F0019 */  multu $t9, $ra
/* 0995A8 800989A8 00007012 */  mflo  $t6
/* 0995AC 800989AC 004E7821 */  addu  $t7, $v0, $t6
/* 0995B0 800989B0 81F80059 */  lb    $t8, 0x59($t7)
/* 0995B4 800989B4 00000000 */  nop   
/* 0995B8 800989B8 0018C880 */  sll   $t9, $t8, 2
/* 0995BC 800989BC 01B97021 */  addu  $t6, $t5, $t9
/* 0995C0 800989C0 8DCF0000 */  lw    $t7, ($t6)
/* 0995C4 800989C4 0008C080 */  sll   $t8, $t0, 2
/* 0995C8 800989C8 02B8C821 */  addu  $t9, $s5, $t8
/* 0995CC 800989CC AD790054 */  sw    $t9, 0x54($t3)
/* 0995D0 800989D0 AD6F0014 */  sw    $t7, 0x14($t3)
.L800989D4:
/* 0995D4 800989D4 25080001 */  addiu $t0, $t0, 1
/* 0995D8 800989D8 0109082A */  slt   $at, $t0, $t1
/* 0995DC 800989DC 258C0060 */  addiu $t4, $t4, 0x60
/* 0995E0 800989E0 1420FFA8 */  bnez  $at, .L80098884
/* 0995E4 800989E4 256B0060 */   addiu $t3, $t3, 0x60
/* 0995E8 800989E8 3C05800E */  lui   $a1, %hi(D_800E1048) # $a1, 0x800e
/* 0995EC 800989EC 24A51048 */  addiu $a1, %lo(D_800E1048) # addiu $a1, $a1, 0x1048
.L800989F0:
/* 0995F0 800989F0 00107140 */  sll   $t6, $s0, 5
.L800989F4:
/* 0995F4 800989F4 00AE7821 */  addu  $t7, $a1, $t6
/* 0995F8 800989F8 ADE00014 */  sw    $zero, 0x14($t7)
/* 0995FC 800989FC 8FBF0034 */  lw    $ra, 0x34($sp)
/* 099600 80098A00 8FB60030 */  lw    $s6, 0x30($sp)
/* 099604 80098A04 8FB5002C */  lw    $s5, 0x2c($sp)
/* 099608 80098A08 8FB40028 */  lw    $s4, 0x28($sp)
/* 09960C 80098A0C 8FB30024 */  lw    $s3, 0x24($sp)
/* 099610 80098A10 8FB20020 */  lw    $s2, 0x20($sp)
/* 099614 80098A14 8FB1001C */  lw    $s1, 0x1c($sp)
/* 099618 80098A18 8FB00018 */  lw    $s0, 0x18($sp)
/* 09961C 80098A1C 03E00008 */  jr    $ra
/* 099620 80098A20 27BD0038 */   addiu $sp, $sp, 0x38

