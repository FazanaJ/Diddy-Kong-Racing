glabel func_80004384
/* 004F84 80004384 27BDFFC8 */  addiu $sp, $sp, -0x38
/* 004F88 80004388 3C03800E */  lui   $v1, %hi(D_800DC6B0) # $v1, 0x800e
/* 004F8C 8000438C 2463C6B0 */  addiu $v1, %lo(D_800DC6B0) # addiu $v1, $v1, -0x3950
/* 004F90 80004390 AFB00018 */  sw    $s0, 0x18($sp)
/* 004F94 80004394 8C700008 */  lw    $s0, 8($v1)
/* 004F98 80004398 AFBF001C */  sw    $ra, 0x1c($sp)
/* 004F9C 8000439C AFA40038 */  sw    $a0, 0x38($sp)
/* 004FA0 800043A0 AFA5003C */  sw    $a1, 0x3c($sp)
/* 004FA4 800043A4 8CA60004 */  lw    $a2, 4($a1)
/* 004FA8 800043A8 12000058 */  beqz  $s0, .L8000450C
/* 004FAC 800043AC 24040001 */   li    $a0, 1
/* 004FB0 800043B0 0C03268C */  jal   osSetIntMask
/* 004FB4 800043B4 AFA60030 */   sw    $a2, 0x30($sp)
/* 004FB8 800043B8 AFA20028 */  sw    $v0, 0x28($sp)
/* 004FBC 800043BC 8E0F0000 */  lw    $t7, ($s0)
/* 004FC0 800043C0 3C03800E */  lui   $v1, %hi(D_800DC6B0) # $v1, 0x800e
/* 004FC4 800043C4 2463C6B0 */  addiu $v1, %lo(D_800DC6B0) # addiu $v1, $v1, -0x3950
/* 004FC8 800043C8 02002025 */  move  $a0, $s0
/* 004FCC 800043CC 0C0321D8 */  jal   alUnlink
/* 004FD0 800043D0 AC6F0008 */   sw    $t7, 8($v1)
/* 004FD4 800043D4 3C03800E */  lui   $v1, %hi(D_800DC6B0) # $v1, 0x800e
/* 004FD8 800043D8 2463C6B0 */  addiu $v1, %lo(D_800DC6B0) # addiu $v1, $v1, -0x3950
/* 004FDC 800043DC 8C620000 */  lw    $v0, ($v1)
/* 004FE0 800043E0 8FA60030 */  lw    $a2, 0x30($sp)
/* 004FE4 800043E4 10400008 */  beqz  $v0, .L80004408
/* 004FE8 800043E8 00000000 */   nop   
/* 004FEC 800043EC AE020000 */  sw    $v0, ($s0)
/* 004FF0 800043F0 AE000004 */  sw    $zero, 4($s0)
/* 004FF4 800043F4 8C780000 */  lw    $t8, ($v1)
/* 004FF8 800043F8 00000000 */  nop   
/* 004FFC 800043FC AF100004 */  sw    $s0, 4($t8)
/* 005000 80004400 10000005 */  b     .L80004418
/* 005004 80004404 AC700000 */   sw    $s0, ($v1)
.L80004408:
/* 005008 80004408 AE000004 */  sw    $zero, 4($s0)
/* 00500C 8000440C AE000000 */  sw    $zero, ($s0)
/* 005010 80004410 AC700000 */  sw    $s0, ($v1)
/* 005014 80004414 AC700004 */  sw    $s0, 4($v1)
.L80004418:
/* 005018 80004418 8FA40028 */  lw    $a0, 0x28($sp)
/* 00501C 8000441C 0C03268C */  jal   osSetIntMask
/* 005020 80004420 AFA60030 */   sw    $a2, 0x30($sp)
/* 005024 80004424 8FA4003C */  lw    $a0, 0x3c($sp)
/* 005028 80004428 8FA60030 */  lw    $a2, 0x30($sp)
/* 00502C 8000442C 8C990000 */  lw    $t9, ($a0)
/* 005030 80004430 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 005034 80004434 8F230004 */  lw    $v1, 4($t9)
/* 005038 80004438 44812000 */  mtc1  $at, $f4
/* 00503C 8000443C 24680001 */  addiu $t0, $v1, 1
/* 005040 80004440 2D080001 */  sltiu $t0, $t0, 1
/* 005044 80004444 25050040 */  addiu $a1, $t0, 0x40
/* 005048 80004448 24090005 */  li    $t1, 5
/* 00504C 8000444C 240A0002 */  li    $t2, 2
/* 005050 80004450 A2050036 */  sb    $a1, 0x36($s0)
/* 005054 80004454 A209003F */  sb    $t1, 0x3f($s0)
/* 005058 80004458 AE0A0038 */  sw    $t2, 0x38($s0)
/* 00505C 8000445C AE040008 */  sw    $a0, 8($s0)
/* 005060 80004460 E604002C */  swc1  $f4, 0x2c($s0)
/* 005064 80004464 90CB0003 */  lbu   $t3, 3($a2)
/* 005068 80004468 AE000030 */  sw    $zero, 0x30($s0)
/* 00506C 8000446C 316D00F0 */  andi  $t5, $t3, 0xf0
/* 005070 80004470 31AE0020 */  andi  $t6, $t5, 0x20
/* 005074 80004474 11C0000D */  beqz  $t6, .L800044AC
/* 005078 80004478 A20D003E */   sb    $t5, 0x3e($s0)
/* 00507C 8000447C 90C40004 */  lbu   $a0, 4($a2)
/* 005080 80004480 AFA50024 */  sw    $a1, 0x24($sp)
/* 005084 80004484 00047880 */  sll   $t7, $a0, 2
/* 005088 80004488 01E47823 */  subu  $t7, $t7, $a0
/* 00508C 8000448C 000F78C0 */  sll   $t7, $t7, 3
/* 005090 80004490 01E47821 */  addu  $t7, $t7, $a0
/* 005094 80004494 000F7880 */  sll   $t7, $t7, 2
/* 005098 80004498 0C032678 */  jal   func_800C99E0
/* 00509C 8000449C 25E4E890 */   addiu $a0, $t7, -0x1770
/* 0050A0 800044A0 8FA50024 */  lw    $a1, 0x24($sp)
/* 0050A4 800044A4 1000000E */  b     .L800044E0
/* 0050A8 800044A8 E6000028 */   swc1  $f0, 0x28($s0)
.L800044AC:
/* 0050AC 800044AC 90D80004 */  lbu   $t8, 4($a2)
/* 0050B0 800044B0 80C80005 */  lb    $t0, 5($a2)
/* 0050B4 800044B4 0018C880 */  sll   $t9, $t8, 2
/* 0050B8 800044B8 0338C823 */  subu  $t9, $t9, $t8
/* 0050BC 800044BC 0019C8C0 */  sll   $t9, $t9, 3
/* 0050C0 800044C0 0338C821 */  addu  $t9, $t9, $t8
/* 0050C4 800044C4 0019C880 */  sll   $t9, $t9, 2
/* 0050C8 800044C8 03282021 */  addu  $a0, $t9, $t0
/* 0050CC 800044CC 2484E890 */  addiu $a0, $a0, -0x1770
/* 0050D0 800044D0 0C032678 */  jal   func_800C99E0
/* 0050D4 800044D4 AFA50024 */   sw    $a1, 0x24($sp)
/* 0050D8 800044D8 8FA50024 */  lw    $a1, 0x24($sp)
/* 0050DC 800044DC E6000028 */  swc1  $f0, 0x28($s0)
.L800044E0:
/* 0050E0 800044E0 24010040 */  li    $at, 64
/* 0050E4 800044E4 10A10005 */  beq   $a1, $at, .L800044FC
/* 0050E8 800044E8 240B0040 */   li    $t3, 64
/* 0050EC 800044EC 9209003E */  lbu   $t1, 0x3e($s0)
/* 0050F0 800044F0 00000000 */  nop   
/* 0050F4 800044F4 352A0002 */  ori   $t2, $t1, 2
/* 0050F8 800044F8 A20A003E */  sb    $t2, 0x3e($s0)
.L800044FC:
/* 0050FC 800044FC 240C7FFF */  li    $t4, 32767
/* 005100 80004500 A200003D */  sb    $zero, 0x3d($s0)
/* 005104 80004504 A20B003C */  sb    $t3, 0x3c($s0)
/* 005108 80004508 A60C0034 */  sh    $t4, 0x34($s0)
.L8000450C:
/* 00510C 8000450C 8FBF001C */  lw    $ra, 0x1c($sp)
/* 005110 80004510 02001025 */  move  $v0, $s0
/* 005114 80004514 8FB00018 */  lw    $s0, 0x18($sp)
/* 005118 80004518 03E00008 */  jr    $ra
/* 00511C 8000451C 27BD0038 */   addiu $sp, $sp, 0x38

