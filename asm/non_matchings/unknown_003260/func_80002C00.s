glabel func_80002C00
/* 003800 80002C00 27BDFFD8 */  addiu $sp, $sp, -0x28
/* 003804 80002C04 AFBF001C */  sw    $ra, 0x1c($sp)
/* 003808 80002C08 AFB00018 */  sw    $s0, 0x18($sp)
/* 00380C 80002C0C 00808025 */  move  $s0, $a0
/* 003810 80002C10 0C000C10 */  jal   func_80003040
/* 003814 80002C14 AFA5002C */   sw    $a1, 0x2c($sp)
/* 003818 80002C18 8E040000 */  lw    $a0, ($s0)
/* 00381C 80002C1C 0C03233C */  jal   osVirtualToPhysical
/* 003820 80002C20 00000000 */   nop   
/* 003824 80002C24 8FA3002C */  lw    $v1, 0x2c($sp)
/* 003828 80002C28 AFA20024 */  sw    $v0, 0x24($sp)
/* 00382C 80002C2C 10600013 */  beqz  $v1, .L80002C7C
/* 003830 80002C30 00000000 */   nop   
/* 003834 80002C34 8C640000 */  lw    $a0, ($v1)
/* 003838 80002C38 3C01800E */  lui   $at, %hi(D_800DC698) # $at, 0x800e
/* 00383C 80002C3C AC24C698 */  sw    $a0, %lo(D_800DC698)($at)
/* 003840 80002C40 84650004 */  lh    $a1, 4($v1)
/* 003844 80002C44 3C01800E */  lui   $at, %hi(D_800DC69C) # $at, 0x800e
/* 003848 80002C48 00057080 */  sll   $t6, $a1, 2
/* 00384C 80002C4C 01C02825 */  move  $a1, $t6
/* 003850 80002C50 0C03235C */  jal   osAiSetNextBuffer
/* 003854 80002C54 AC2EC69C */   sw    $t6, %lo(D_800DC69C)($at)
/* 003858 80002C58 3C0F800E */  lui   $t7, %hi(D_800DC68C) # $t7, 0x800e
/* 00385C 80002C5C 81EFC68C */  lb    $t7, %lo(D_800DC68C)($t7)
/* 003860 80002C60 00002025 */  move  $a0, $zero
/* 003864 80002C64 11E00005 */  beqz  $t7, .L80002C7C
/* 003868 80002C68 00000000 */   nop   
/* 00386C 80002C6C 0C01BE53 */  jal   get_random_number_from_range
/* 003870 80002C70 24052710 */   li    $a1, 10000
/* 003874 80002C74 0C032180 */  jal   osAiSetFrequency
/* 003878 80002C78 24445622 */   addiu $a0, $v0, 0x5622
.L80002C7C:
/* 00387C 80002C7C 0C032388 */  jal   osAiGetLength
/* 003880 80002C80 00000000 */   nop   
/* 003884 80002C84 3C188012 */  lui   $t8, %hi(framesize) # $t8, 0x8012
/* 003888 80002C88 8F18962C */  lw    $t8, %lo(framesize)($t8)
/* 00388C 80002C8C 0002C882 */  srl   $t9, $v0, 2
/* 003890 80002C90 03194023 */  subu  $t0, $t8, $t9
/* 003894 80002C94 25090070 */  addiu $t1, $t0, 0x70
/* 003898 80002C98 312AFFF0 */  andi  $t2, $t1, 0xfff0
/* 00389C 80002C9C A60A0004 */  sh    $t2, 4($s0)
/* 0038A0 80002CA0 3C038012 */  lui   $v1, %hi(minFrameSize) # $v1, 0x8012
/* 0038A4 80002CA4 8C639628 */  lw    $v1, %lo(minFrameSize)($v1)
/* 0038A8 80002CA8 86070004 */  lh    $a3, 4($s0)
/* 0038AC 80002CAC 3C0B800E */  lui   $t3, %hi(D_800DC688) # $t3, 0x800e
/* 0038B0 80002CB0 00E3082B */  sltu  $at, $a3, $v1
/* 0038B4 80002CB4 10200004 */  beqz  $at, .L80002CC8
/* 0038B8 80002CB8 3C048011 */   lui   $a0, %hi(D_80115F98) # $a0, 0x8011
/* 0038BC 80002CBC A6030004 */  sh    $v1, 4($s0)
/* 0038C0 80002CC0 86070004 */  lh    $a3, 4($s0)
/* 0038C4 80002CC4 00000000 */  nop   
.L80002CC8:
/* 0038C8 80002CC8 8D6BC688 */  lw    $t3, %lo(D_800DC688)($t3)
/* 0038CC 80002CCC 3C058012 */  lui   $a1, %hi(nextDMA) # $a1, 0x8012
/* 0038D0 80002CD0 000B6080 */  sll   $t4, $t3, 2
/* 0038D4 80002CD4 008C2021 */  addu  $a0, $a0, $t4
/* 0038D8 80002CD8 8C845F98 */  lw    $a0, %lo(D_80115F98)($a0)
/* 0038DC 80002CDC 8FA60024 */  lw    $a2, 0x24($sp)
/* 0038E0 80002CE0 0C019525 */  jal   alAudioFrame
/* 0038E4 80002CE4 24A59634 */   addiu $a1, %lo(nextDMA) # addiu $a1, $a1, -0x69cc
/* 0038E8 80002CE8 3C0D8011 */  lui   $t5, %hi(D_80116198) # $t5, 0x8011
/* 0038EC 80002CEC 24030002 */  li    $v1, 2
/* 0038F0 80002CF0 3C06800E */  lui   $a2, %hi(D_800DC688) # $a2, 0x800e
/* 0038F4 80002CF4 25AD6198 */  addiu $t5, %lo(D_80116198) # addiu $t5, $t5, 0x6198
/* 0038F8 80002CF8 240EFFFF */  li    $t6, -1
/* 0038FC 80002CFC 240F00FF */  li    $t7, 255
/* 003900 80002D00 24C6C688 */  addiu $a2, %lo(D_800DC688) # addiu $a2, $a2, -0x3978
/* 003904 80002D04 AE000008 */  sw    $zero, 8($s0)
/* 003908 80002D08 AE0D0058 */  sw    $t5, 0x58($s0)
/* 00390C 80002D0C AE10005C */  sw    $s0, 0x5c($s0)
/* 003910 80002D10 AE030010 */  sw    $v1, 0x10($s0)
/* 003914 80002D14 AE0E0060 */  sw    $t6, 0x60($s0)
/* 003918 80002D18 AE0F0068 */  sw    $t7, 0x68($s0)
/* 00391C 80002D1C AE000064 */  sw    $zero, 0x64($s0)
/* 003920 80002D20 AE00006C */  sw    $zero, 0x6c($s0)
/* 003924 80002D24 8CD80000 */  lw    $t8, ($a2)
/* 003928 80002D28 3C078011 */  lui   $a3, %hi(D_80115F98) # $a3, 0x8011
/* 00392C 80002D2C 24E75F98 */  addiu $a3, %lo(D_80115F98) # addiu $a3, $a3, 0x5f98
/* 003930 80002D30 0018C880 */  sll   $t9, $t8, 2
/* 003934 80002D34 00F94021 */  addu  $t0, $a3, $t9
/* 003938 80002D38 8D090000 */  lw    $t1, ($t0)
/* 00393C 80002D3C 3C05800E */  lui   $a1, %hi(rspF3DDKRBootStart) # $a1, 0x800e
/* 003940 80002D40 AE090048 */  sw    $t1, 0x48($s0)
/* 003944 80002D44 8CCA0000 */  lw    $t2, ($a2)
/* 003948 80002D48 3C19800E */  lui   $t9, %hi(rspF3DDKRDramStart) # $t9, 0x800e
/* 00394C 80002D4C 000A5880 */  sll   $t3, $t2, 2
/* 003950 80002D50 00EB6021 */  addu  $t4, $a3, $t3
/* 003954 80002D54 8D8D0000 */  lw    $t5, ($t4)
/* 003958 80002D58 24A584C0 */  addiu $a1, %lo(rspF3DDKRBootStart) # addiu $a1, $a1, -0x7b40
/* 00395C 80002D5C 004D7023 */  subu  $t6, $v0, $t5
/* 003960 80002D60 000E78C3 */  sra   $t7, $t6, 3
/* 003964 80002D64 27398590 */  addiu $t9, %lo(rspF3DDKRDramStart) # addiu $t9, $t9, -0x7a70
/* 003968 80002D68 3C09800D */  lui   $t1, %hi(rspUnknownStart) # $t1, 0x800d
/* 00396C 80002D6C 3C0A800F */  lui   $t2, %hi(rspUnknownDataStart) # $t2, 0x800f
/* 003970 80002D70 000FC0C0 */  sll   $t8, $t7, 3
/* 003974 80002D74 03254023 */  subu  $t0, $t9, $a1
/* 003978 80002D78 25297600 */  addiu $t1, %lo(rspUnknownStart) # addiu $t1, $t1, 0x7600
/* 00397C 80002D7C 254A98D0 */  addiu $t2, %lo(rspUnknownDataStart) # addiu $t2, $t2, -0x6730
/* 003980 80002D80 240B0800 */  li    $t3, 2048
/* 003984 80002D84 240C0001 */  li    $t4, 1
/* 003988 80002D88 AE18004C */  sw    $t8, 0x4c($s0)
/* 00398C 80002D8C AE030018 */  sw    $v1, 0x18($s0)
/* 003990 80002D90 AE050020 */  sw    $a1, 0x20($s0)
/* 003994 80002D94 AE080024 */  sw    $t0, 0x24($s0)
/* 003998 80002D98 AE03001C */  sw    $v1, 0x1c($s0)
/* 00399C 80002D9C AE090028 */  sw    $t1, 0x28($s0)
/* 0039A0 80002DA0 AE0A0030 */  sw    $t2, 0x30($s0)
/* 0039A4 80002DA4 AE0B0034 */  sw    $t3, 0x34($s0)
/* 0039A8 80002DA8 AE000050 */  sw    $zero, 0x50($s0)
/* 0039AC 80002DAC AE000054 */  sw    $zero, 0x54($s0)
/* 0039B0 80002DB0 AE0C0074 */  sw    $t4, 0x74($s0)
/* 0039B4 80002DB4 3C048011 */  lui   $a0, %hi(gAudioSched) # $a0, 0x8011
/* 0039B8 80002DB8 8C845F90 */  lw    $a0, %lo(gAudioSched)($a0)
/* 0039BC 80002DBC 0C01E55D */  jal   osScGetCmdQ
/* 0039C0 80002DC0 00000000 */   nop   
/* 0039C4 80002DC4 00402025 */  move  $a0, $v0
/* 0039C8 80002DC8 26050008 */  addiu $a1, $s0, 8
/* 0039CC 80002DCC 0C03238C */  jal   osSendMesg
/* 0039D0 80002DD0 00003025 */   move  $a2, $zero
/* 0039D4 80002DD4 3C03800E */  lui   $v1, %hi(D_800DC688) # $v1, 0x800e
/* 0039D8 80002DD8 2463C688 */  addiu $v1, %lo(D_800DC688) # addiu $v1, $v1, -0x3978
/* 0039DC 80002DDC 8C6D0000 */  lw    $t5, ($v1)
/* 0039E0 80002DE0 8FBF001C */  lw    $ra, 0x1c($sp)
/* 0039E4 80002DE4 8FB00018 */  lw    $s0, 0x18($sp)
/* 0039E8 80002DE8 39AE0001 */  xori  $t6, $t5, 1
/* 0039EC 80002DEC AC6E0000 */  sw    $t6, ($v1)
/* 0039F0 80002DF0 03E00008 */  jr    $ra
/* 0039F4 80002DF4 27BD0028 */   addiu $sp, $sp, 0x28

