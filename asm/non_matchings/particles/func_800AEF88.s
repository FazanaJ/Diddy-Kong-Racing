glabel func_800AEF88
/* 0AFB88 800AEF88 240E0006 */  li    $t6, 6
/* 0AFB8C 800AEF8C 240F0004 */  li    $t7, 4
/* 0AFB90 800AEF90 A48E0004 */  sh    $t6, 4($a0)
/* 0AFB94 800AEF94 A48F0006 */  sh    $t7, 6($a0)
/* 0AFB98 800AEF98 8CB80000 */  lw    $t8, ($a1)
/* 0AFB9C 800AEF9C 240300FF */  li    $v1, 255
/* 0AFBA0 800AEFA0 AC980008 */  sw    $t8, 8($a0)
/* 0AFBA4 800AEFA4 8CD90000 */  lw    $t9, ($a2)
/* 0AFBA8 800AEFA8 00000000 */  nop   
/* 0AFBAC 800AEFAC AC99000C */  sw    $t9, 0xc($a0)
/* 0AFBB0 800AEFB0 8CA20000 */  lw    $v0, ($a1)
/* 0AFBB4 800AEFB4 00000000 */  nop   
/* 0AFBB8 800AEFB8 A0430006 */  sb    $v1, 6($v0)
/* 0AFBBC 800AEFBC A0430007 */  sb    $v1, 7($v0)
/* 0AFBC0 800AEFC0 A0430008 */  sb    $v1, 8($v0)
/* 0AFBC4 800AEFC4 A0430009 */  sb    $v1, 9($v0)
/* 0AFBC8 800AEFC8 A0430010 */  sb    $v1, 0x10($v0)
/* 0AFBCC 800AEFCC A0430011 */  sb    $v1, 0x11($v0)
/* 0AFBD0 800AEFD0 A0430012 */  sb    $v1, 0x12($v0)
/* 0AFBD4 800AEFD4 A0430013 */  sb    $v1, 0x13($v0)
/* 0AFBD8 800AEFD8 A0430024 */  sb    $v1, 0x24($v0)
/* 0AFBDC 800AEFDC A0430025 */  sb    $v1, 0x25($v0)
/* 0AFBE0 800AEFE0 A0430026 */  sb    $v1, 0x26($v0)
/* 0AFBE4 800AEFE4 A0430027 */  sb    $v1, 0x27($v0)
/* 0AFBE8 800AEFE8 A043002E */  sb    $v1, 0x2e($v0)
/* 0AFBEC 800AEFEC A043002F */  sb    $v1, 0x2f($v0)
/* 0AFBF0 800AEFF0 A0430030 */  sb    $v1, 0x30($v0)
/* 0AFBF4 800AEFF4 A0430031 */  sb    $v1, 0x31($v0)
/* 0AFBF8 800AEFF8 A0430038 */  sb    $v1, 0x38($v0)
/* 0AFBFC 800AEFFC A0430039 */  sb    $v1, 0x39($v0)
/* 0AFC00 800AF000 A043003A */  sb    $v1, 0x3a($v0)
/* 0AFC04 800AF004 A043003B */  sb    $v1, 0x3b($v0)
/* 0AFC08 800AF008 2442003C */  addiu $v0, $v0, 0x3c
/* 0AFC0C 800AF00C A043FFDE */  sb    $v1, -0x22($v0)
/* 0AFC10 800AF010 A043FFDF */  sb    $v1, -0x21($v0)
/* 0AFC14 800AF014 A043FFE0 */  sb    $v1, -0x20($v0)
/* 0AFC18 800AF018 A043FFE1 */  sb    $v1, -0x1f($v0)
/* 0AFC1C 800AF01C 03E00008 */  jr    $ra
/* 0AFC20 800AF020 ACA20000 */   sw    $v0, ($a1)
