glabel func_8007F414
/* 080014 8007F414 948E000C */  lhu   $t6, 0xc($a0)
/* 080018 8007F418 94830000 */  lhu   $v1, ($a0)
/* 08001C 8007F41C A4800002 */  sh    $zero, 2($a0)
/* 080020 8007F420 A4800004 */  sh    $zero, 4($a0)
/* 080024 8007F424 A4800006 */  sh    $zero, 6($a0)
/* 080028 8007F428 1860000B */  blez  $v1, .L8007F458
/* 08002C 8007F42C AC8E0008 */   sw    $t6, 8($a0)
/* 080030 8007F430 00037880 */  sll   $t7, $v1, 2
/* 080034 8007F434 01E42821 */  addu  $a1, $t7, $a0
/* 080038 8007F438 00801025 */  move  $v0, $a0
.L8007F43C:
/* 08003C 8007F43C 94980006 */  lhu   $t8, 6($a0)
/* 080040 8007F440 9459000E */  lhu   $t9, 0xe($v0)
/* 080044 8007F444 24420004 */  addiu $v0, $v0, 4
/* 080048 8007F448 0045082B */  sltu  $at, $v0, $a1
/* 08004C 8007F44C 03194021 */  addu  $t0, $t8, $t9
/* 080050 8007F450 1420FFFA */  bnez  $at, .L8007F43C
/* 080054 8007F454 A4880006 */   sh    $t0, 6($a0)
.L8007F458:
/* 080058 8007F458 03E00008 */  jr    $ra
/* 08005C 8007F45C 00000000 */   nop   
