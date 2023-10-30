glabel set_intmask_protection
/* 070164 8006F564 3C01800E */  lui   $at, %hi(gDisableIntMask) # $at, 0x800e
/* 070168 8006F568 03E00008 */  jr    $ra
/* 07016C 8006F56C A024D430 */   sb    $a0, %lo(gDisableIntMask)($at)
