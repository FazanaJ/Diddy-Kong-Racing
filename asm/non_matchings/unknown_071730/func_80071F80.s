glabel func_80071F80
/* 072B80 80071F80 27BDFFA8 */  addiu $sp, $sp, -0x58
/* 072B84 80071F84 AFB00018 */  sw    $s0, 0x18($sp)
/* 072B88 80071F88 00808025 */  move  $s0, $a0
/* 072B8C 80071F8C AFBF001C */  sw    $ra, 0x1c($sp)
/* 072B90 80071F90 AFA5005C */  sw    $a1, 0x5c($sp)
/* 072B94 80071F94 00E01825 */  move  $v1, $a3
/* 072B98 80071F98 00004025 */  move  $t0, $zero
/* 072B9C 80071F9C 00E01025 */  move  $v0, $a3
.L80071FA0:
/* 072BA0 80071FA0 25080001 */  addiu $t0, $t0, 1
/* 072BA4 80071FA4 2901000F */  slti  $at, $t0, 0xf
/* 072BA8 80071FA8 24420004 */  addiu $v0, $v0, 4
/* 072BAC 80071FAC 1420FFFC */  bnez  $at, .L80071FA0
/* 072BB0 80071FB0 AC40FFFC */   sw    $zero, -4($v0)
/* 072BB4 80071FB4 240E0001 */  li    $t6, 1
/* 072BB8 80071FB8 ACEE003C */  sw    $t6, 0x3c($a3)
/* 072BBC 80071FBC 240F00FF */  li    $t7, 255
/* 072BC0 80071FC0 24180023 */  li    $t8, 35
/* 072BC4 80071FC4 24190001 */  li    $t9, 1
/* 072BC8 80071FC8 24090003 */  li    $t1, 3
/* 072BCC 80071FCC 97A4005E */  lhu   $a0, 0x5e($sp)
/* 072BD0 80071FD0 A3AF002C */  sb    $t7, 0x2c($sp)
/* 072BD4 80071FD4 A3B8002D */  sb    $t8, 0x2d($sp)
/* 072BD8 80071FD8 A3B9002E */  sb    $t9, 0x2e($sp)
/* 072BDC 80071FDC A3A9002F */  sb    $t1, 0x2f($sp)
/* 072BE0 80071FE0 AFA60060 */  sw    $a2, 0x60($sp)
/* 072BE4 80071FE4 0C0335DC */  jal   func_800CD770
/* 072BE8 80071FE8 AFA30054 */   sw    $v1, 0x54($sp)
/* 072BEC 80071FEC 97AA005E */  lhu   $t2, 0x5e($sp)
/* 072BF0 80071FF0 240D00FF */  li    $t5, 255
/* 072BF4 80071FF4 000A5940 */  sll   $t3, $t2, 5
/* 072BF8 80071FF8 004B6025 */  or    $t4, $v0, $t3
/* 072BFC 80071FFC 8FA30054 */  lw    $v1, 0x54($sp)
/* 072C00 80072000 8FA60060 */  lw    $a2, 0x60($sp)
/* 072C04 80072004 A7AC0030 */  sh    $t4, 0x30($sp)
/* 072C08 80072008 A3AD0052 */  sb    $t5, 0x52($sp)
/* 072C0C 8007200C 27A2004C */  addiu $v0, $sp, 0x4c
/* 072C10 80072010 27A4002C */  addiu $a0, $sp, 0x2c
.L80072014:
/* 072C14 80072014 90CE0000 */  lbu   $t6, ($a2)
/* 072C18 80072018 24840001 */  addiu $a0, $a0, 1
/* 072C1C 8007201C 0082082B */  sltu  $at, $a0, $v0
/* 072C20 80072020 24C60001 */  addiu $a2, $a2, 1
/* 072C24 80072024 1420FFFB */  bnez  $at, .L80072014
/* 072C28 80072028 A08E0005 */   sb    $t6, 5($a0)
/* 072C2C 8007202C 12000012 */  beqz  $s0, .L80072078
/* 072C30 80072030 27AF002C */   addiu $t7, $sp, 0x2c
/* 072C34 80072034 1A000010 */  blez  $s0, .L80072078
/* 072C38 80072038 00004025 */   move  $t0, $zero
/* 072C3C 8007203C 32040003 */  andi  $a0, $s0, 3
/* 072C40 80072040 10800006 */  beqz  $a0, .L8007205C
/* 072C44 80072044 00801025 */   move  $v0, $a0
.L80072048:
/* 072C48 80072048 25080001 */  addiu $t0, $t0, 1
/* 072C4C 8007204C A0600000 */  sb    $zero, ($v1)
/* 072C50 80072050 1448FFFD */  bne   $v0, $t0, .L80072048
/* 072C54 80072054 24630001 */   addiu $v1, $v1, 1
/* 072C58 80072058 11100007 */  beq   $t0, $s0, .L80072078
.L8007205C:
/* 072C5C 8007205C 25080004 */   addiu $t0, $t0, 4
/* 072C60 80072060 A0600001 */  sb    $zero, 1($v1)
/* 072C64 80072064 A0600002 */  sb    $zero, 2($v1)
/* 072C68 80072068 A0600003 */  sb    $zero, 3($v1)
/* 072C6C 8007206C 24630004 */  addiu $v1, $v1, 4
/* 072C70 80072070 1510FFFA */  bne   $t0, $s0, .L8007205C
/* 072C74 80072074 A060FFFC */   sb    $zero, -4($v1)
.L80072078:
/* 072C78 80072078 00604825 */  move  $t1, $v1
/* 072C7C 8007207C 25F90024 */  addiu $t9, $t7, 0x24
.L80072080:
/* 072C80 80072080 8DE10000 */  lw    $at, ($t7)
/* 072C84 80072084 25EF000C */  addiu $t7, $t7, 0xc
/* 072C88 80072088 A9210000 */  swl   $at, ($t1)
/* 072C8C 8007208C B9210003 */  swr   $at, 3($t1)
/* 072C90 80072090 8DE1FFF8 */  lw    $at, -8($t7)
/* 072C94 80072094 2529000C */  addiu $t1, $t1, 0xc
/* 072C98 80072098 A921FFF8 */  swl   $at, -8($t1)
/* 072C9C 8007209C B921FFFB */  swr   $at, -5($t1)
/* 072CA0 800720A0 8DE1FFFC */  lw    $at, -4($t7)
/* 072CA4 800720A4 00000000 */  nop   
/* 072CA8 800720A8 A921FFFC */  swl   $at, -4($t1)
/* 072CAC 800720AC 15F9FFF4 */  bne   $t7, $t9, .L80072080
/* 072CB0 800720B0 B921FFFF */   swr   $at, -1($t1)
/* 072CB4 800720B4 8DE10000 */  lw    $at, ($t7)
/* 072CB8 800720B8 240A00FE */  li    $t2, 254
/* 072CBC 800720BC A9210000 */  swl   $at, ($t1)
/* 072CC0 800720C0 B9210003 */  swr   $at, 3($t1)
/* 072CC4 800720C4 A06A0028 */  sb    $t2, 0x28($v1)
/* 072CC8 800720C8 8FBF001C */  lw    $ra, 0x1c($sp)
/* 072CCC 800720CC 8FB00018 */  lw    $s0, 0x18($sp)
/* 072CD0 800720D0 24630028 */  addiu $v1, $v1, 0x28
/* 072CD4 800720D4 03E00008 */  jr    $ra
/* 072CD8 800720D8 27BD0058 */   addiu $sp, $sp, 0x58
