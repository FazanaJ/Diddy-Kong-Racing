glabel func_8000488C
/* 00548C 8000488C 27BDFFD8 */  addiu $sp, $sp, -0x28
/* 005490 80004890 240E0400 */  li    $t6, 1024
/* 005494 80004894 AFBF0014 */  sw    $ra, 0x14($sp)
/* 005498 80004898 A7AE0018 */  sh    $t6, 0x18($sp)
/* 00549C 8000489C 1080000A */  beqz  $a0, .L800048C8
/* 0054A0 800048A0 AFA4001C */   sw    $a0, 0x1c($sp)
/* 0054A4 800048A4 908F003E */  lbu   $t7, 0x3e($a0)
/* 0054A8 800048A8 27A50018 */  addiu $a1, $sp, 0x18
/* 0054AC 800048AC 31F8FFEF */  andi  $t8, $t7, 0xffef
/* 0054B0 800048B0 A098003E */  sb    $t8, 0x3e($a0)
/* 0054B4 800048B4 3C04800E */  lui   $a0, %hi(D_800DC6BC) # $a0, 0x800e
/* 0054B8 800048B8 8C84C6BC */  lw    $a0, %lo(D_800DC6BC)($a0)
/* 0054BC 800048BC 00003025 */  move  $a2, $zero
/* 0054C0 800048C0 0C03246B */  jal   alEvtqPostEvent
/* 0054C4 800048C4 24840014 */   addiu $a0, $a0, 0x14
.L800048C8:
/* 0054C8 800048C8 8FBF0014 */  lw    $ra, 0x14($sp)
/* 0054CC 800048CC 27BD0028 */  addiu $sp, $sp, 0x28
/* 0054D0 800048D0 03E00008 */  jr    $ra
/* 0054D4 800048D4 00000000 */   nop   

