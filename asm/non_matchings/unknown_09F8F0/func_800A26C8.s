glabel func_800A26C8
/* 0A32C8 800A26C8 27BDFFE0 */  addiu $sp, $sp, -0x20
/* 0A32CC 800A26CC AFBF0014 */  sw    $ra, 0x14($sp)
/* 0A32D0 800A26D0 AFA40020 */  sw    $a0, 0x20($sp)
/* 0A32D4 800A26D4 0C019884 */  jal   func_80066210
/* 0A32D8 800A26D8 AFA50024 */   sw    $a1, 0x24($sp)
/* 0A32DC 800A26DC 14400024 */  bnez  $v0, .L800A2770
/* 0A32E0 800A26E0 8FBF0014 */   lw    $ra, 0x14($sp)
/* 0A32E4 800A26E4 8FAE0020 */  lw    $t6, 0x20($sp)
/* 0A32E8 800A26E8 24040001 */  li    $a0, 1
/* 0A32EC 800A26EC 8DCF0064 */  lw    $t7, 0x64($t6)
/* 0A32F0 800A26F0 0C01A142 */  jal   func_80068508
/* 0A32F4 800A26F4 AFAF001C */   sw    $t7, 0x1c($sp)
/* 0A32F8 800A26F8 8FA4001C */  lw    $a0, 0x1c($sp)
/* 0A32FC 800A26FC 0C029C63 */  jal   func_800A718C
/* 0A3300 800A2700 00000000 */   nop   
/* 0A3304 800A2704 8FA40020 */  lw    $a0, 0x20($sp)
/* 0A3308 800A2708 8FA50024 */  lw    $a1, 0x24($sp)
/* 0A330C 800A270C 0C028E21 */  jal   func_800A3884
/* 0A3310 800A2710 00000000 */   nop   
/* 0A3314 800A2714 0C027B20 */  jal   func_8009EC80
/* 0A3318 800A2718 00000000 */   nop   
/* 0A331C 800A271C 10400011 */  beqz  $v0, .L800A2764
/* 0A3320 800A2720 3C078012 */   lui   $a3, %hi(D_80126CDC) # $a3, 0x8012
/* 0A3324 800A2724 8CE76CDC */  lw    $a3, %lo(D_80126CDC)($a3)
/* 0A3328 800A2728 00000000 */  nop   
/* 0A332C 800A272C 24E70720 */  addiu $a3, $a3, 0x720
/* 0A3330 800A2730 0C01BAA4 */  jal   func_8006EA90
/* 0A3334 800A2734 AFA70018 */   sw    $a3, 0x18($sp)
/* 0A3338 800A2738 80580071 */  lb    $t8, 0x71($v0)
/* 0A333C 800A273C 8FA70018 */  lw    $a3, 0x18($sp)
/* 0A3340 800A2740 3C048012 */  lui   $a0, %hi(D_80126CFC) # $a0, 0x8012
/* 0A3344 800A2744 3C058012 */  lui   $a1, %hi(D_80126D00) # $a1, 0x8012
/* 0A3348 800A2748 3C068012 */  lui   $a2, %hi(D_80126D04) # $a2, 0x8012
/* 0A334C 800A274C 27190038 */  addiu $t9, $t8, 0x38
/* 0A3350 800A2750 24C66D04 */  addiu $a2, %lo(D_80126D04) # addiu $a2, $a2, 0x6d04
/* 0A3354 800A2754 24A56D00 */  addiu $a1, %lo(D_80126D00) # addiu $a1, $a1, 0x6d00
/* 0A3358 800A2758 24846CFC */  addiu $a0, %lo(D_80126CFC) # addiu $a0, $a0, 0x6cfc
/* 0A335C 800A275C 0C02A980 */  jal   func_800AA600
/* 0A3360 800A2760 A4F90006 */   sh    $t9, 6($a3)
.L800A2764:
/* 0A3364 800A2764 0C01A142 */  jal   func_80068508
/* 0A3368 800A2768 00002025 */   move  $a0, $zero
/* 0A336C 800A276C 8FBF0014 */  lw    $ra, 0x14($sp)
.L800A2770:
/* 0A3370 800A2770 27BD0020 */  addiu $sp, $sp, 0x20
/* 0A3374 800A2774 03E00008 */  jr    $ra
/* 0A3378 800A2778 00000000 */   nop   
