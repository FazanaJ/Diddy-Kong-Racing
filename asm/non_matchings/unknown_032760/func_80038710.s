glabel func_80038710
/* 039310 80038710 27BDFFC0 */  addiu $sp, $sp, -0x40
/* 039314 80038714 AFB00028 */  sw    $s0, 0x28($sp)
/* 039318 80038718 00808025 */  move  $s0, $a0
/* 03931C 8003871C AFBF002C */  sw    $ra, 0x2c($sp)
/* 039320 80038720 0C007D18 */  jal   func_8001F460
/* 039324 80038724 02003025 */   move  $a2, $s0
/* 039328 80038728 860E0006 */  lh    $t6, 6($s0)
/* 03932C 8003872C 8E180064 */  lw    $t8, 0x64($s0)
/* 039330 80038730 35CF4000 */  ori   $t7, $t6, 0x4000
/* 039334 80038734 A60F0006 */  sh    $t7, 6($s0)
/* 039338 80038738 1440001D */  bnez  $v0, .L800387B0
/* 03933C 8003873C AFB80034 */   sw    $t8, 0x34($sp)
/* 039340 80038740 0C019884 */  jal   get_viewport_count
/* 039344 80038744 00000000 */   nop   
/* 039348 80038748 14400008 */  bnez  $v0, .L8003876C
/* 03934C 8003874C 24030001 */   li    $v1, 1
/* 039350 80038750 8FB90034 */  lw    $t9, 0x34($sp)
/* 039354 80038754 00000000 */  nop   
/* 039358 80038758 83240044 */  lb    $a0, 0x44($t9)
/* 03935C 8003875C 0C008433 */  jal   func_800210CC
/* 039360 80038760 00000000 */   nop   
/* 039364 80038764 10000001 */  b     .L8003876C
/* 039368 80038768 00401825 */   move  $v1, $v0
.L8003876C:
/* 03936C 8003876C 10600010 */  beqz  $v1, .L800387B0
/* 039370 80038770 340A8000 */   li    $t2, 32768
/* 039374 80038774 8FA80034 */  lw    $t0, 0x34($sp)
/* 039378 80038778 86090000 */  lh    $t1, ($s0)
/* 03937C 8003877C 8E05000C */  lw    $a1, 0xc($s0)
/* 039380 80038780 8E060010 */  lw    $a2, 0x10($s0)
/* 039384 80038784 8E070014 */  lw    $a3, 0x14($s0)
/* 039388 80038788 81040030 */  lb    $a0, 0x30($t0)
/* 03938C 8003878C 01495823 */  subu  $t3, $t2, $t1
/* 039390 80038790 AFAB0010 */  sw    $t3, 0x10($sp)
/* 039394 80038794 860C0002 */  lh    $t4, 2($s0)
/* 039398 80038798 00000000 */  nop   
/* 03939C 8003879C 000C6823 */  negu  $t5, $t4
/* 0393A0 800387A0 AFAD0014 */  sw    $t5, 0x14($sp)
/* 0393A4 800387A4 860E0004 */  lh    $t6, 4($s0)
/* 0393A8 800387A8 0C019922 */  jal   func_80066488
/* 0393AC 800387AC AFAE0018 */   sw    $t6, 0x18($sp)
.L800387B0:
/* 0393B0 800387B0 8FBF002C */  lw    $ra, 0x2c($sp)
/* 0393B4 800387B4 8FB00028 */  lw    $s0, 0x28($sp)
/* 0393B8 800387B8 03E00008 */  jr    $ra
/* 0393BC 800387BC 27BD0040 */   addiu $sp, $sp, 0x40

/* 0393C0 800387C0 AFA40000 */  sw    $a0, ($sp)
/* 0393C4 800387C4 03E00008 */  jr    $ra
/* 0393C8 800387C8 AFA50004 */   sw    $a1, 4($sp)

