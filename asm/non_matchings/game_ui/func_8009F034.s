.late_rodata
glabel D_800E86A8 
.double 0.6
glabel D_800E86B0 
.double 1.2
glabel D_800E86B8 
.float 263.0
glabel D_800E86BC 
.float 259.0
glabel D_800E86C0 
.float 265.0
.word 0 # padding
glabel D_800E86C8 
.double 1.2
glabel D_800E86D0 
.double 0.6
glabel D_800E86D8 
.double 1.2
glabel D_800E86E0
.double 0.6
glabel D_800E86E8
.double 1.2
glabel D_800E86F0
.double 0.6

.text
glabel func_8009F034
/* 09FC34 8009F034 27BDFF68 */  addiu $sp, $sp, -0x98
/* 09FC38 8009F038 AFBF0064 */  sw    $ra, 0x64($sp)
/* 09FC3C 8009F03C AFBE0060 */  sw    $fp, 0x60($sp)
/* 09FC40 8009F040 AFB7005C */  sw    $s7, 0x5c($sp)
/* 09FC44 8009F044 AFB60058 */  sw    $s6, 0x58($sp)
/* 09FC48 8009F048 AFB50054 */  sw    $s5, 0x54($sp)
/* 09FC4C 8009F04C AFB40050 */  sw    $s4, 0x50($sp)
/* 09FC50 8009F050 AFB3004C */  sw    $s3, 0x4c($sp)
/* 09FC54 8009F054 AFB20048 */  sw    $s2, 0x48($sp)
/* 09FC58 8009F058 AFB10044 */  sw    $s1, 0x44($sp)
/* 09FC5C 8009F05C AFB00040 */  sw    $s0, 0x40($sp)
/* 09FC60 8009F060 E7BD0038 */  swc1  $f29, 0x38($sp)
/* 09FC64 8009F064 E7BC003C */  swc1  $f28, 0x3c($sp)
/* 09FC68 8009F068 E7BB0030 */  swc1  $f27, 0x30($sp)
/* 09FC6C 8009F06C E7BA0034 */  swc1  $f26, 0x34($sp)
/* 09FC70 8009F070 E7B90028 */  swc1  $f25, 0x28($sp)
/* 09FC74 8009F074 E7B8002C */  swc1  $f24, 0x2c($sp)
/* 09FC78 8009F078 E7B70020 */  swc1  $f23, 0x20($sp)
/* 09FC7C 8009F07C E7B60024 */  swc1  $f22, 0x24($sp)
/* 09FC80 8009F080 E7B50018 */  swc1  $f21, 0x18($sp)
/* 09FC84 8009F084 0C01AF66 */  jal   get_current_level_race_type
/* 09FC88 8009F088 E7B4001C */   swc1  $f20, 0x1c($sp)
/* 09FC8C 8009F08C 240C0001 */  li    $t4, 1
/* 09FC90 8009F090 11820004 */  beq   $t4, $v0, .L8009F0A4
/* 09FC94 8009F094 3C108000 */   lui   $s0, %hi(osTvType) # $s0, 0x8000
/* 09FC98 8009F098 24010005 */  li    $at, 5
/* 09FC9C 8009F09C 14410007 */  bne   $v0, $at, .L8009F0BC
/* 09FCA0 8009F0A0 3C0F8012 */   lui   $t7, %hi(gHUDNumPlayers) # $t7, 0x8012
.L8009F0A4:
/* 09FCA4 8009F0A4 3C018012 */  lui   $at, %hi(gHudOffsetX) # $at, 0x8012
/* 09FCA8 8009F0A8 AC206D24 */  sw    $zero, %lo(gHudOffsetX)($at)
/* 09FCAC 8009F0AC 3C018012 */  lui   $at, %hi(gHudRaceStart) # $at, 0x8012
/* 09FCB0 8009F0B0 240E0001 */  li    $t6, 1
/* 09FCB4 8009F0B4 10000015 */  b     .L8009F10C
/* 09FCB8 8009F0B8 A02E6D34 */   sb    $t6, %lo(gHudRaceStart)($at)
.L8009F0BC:
/* 09FCBC 8009F0BC 8DEF6D0C */  lw    $t7, %lo(gHUDNumPlayers)($t7)
/* 09FCC0 8009F0C0 24180140 */  li    $t8, 320
/* 09FCC4 8009F0C4 29E10002 */  slti  $at, $t7, 2
/* 09FCC8 8009F0C8 10200008 */  beqz  $at, .L8009F0EC
/* 09FCCC 8009F0CC 241900C8 */   li    $t9, 200
/* 09FCD0 8009F0D0 3C018012 */  lui   $at, %hi(gHudOffsetX) # $at, 0x8012
/* 09FCD4 8009F0D4 AC386D24 */  sw    $t8, %lo(gHudOffsetX)($at)
/* 09FCD8 8009F0D8 3C014000 */  li    $at, 0x40000000 # 2.000000
/* 09FCDC 8009F0DC 44812000 */  mtc1  $at, $f4
/* 09FCE0 8009F0E0 3C018012 */  lui   $at, %hi(gHudBounceMag) # $at, 0x8012
/* 09FCE4 8009F0E4 10000007 */  b     .L8009F104
/* 09FCE8 8009F0E8 E4246D30 */   swc1  $f4, %lo(gHudBounceMag)($at)
.L8009F0EC:
/* 09FCEC 8009F0EC 3C018012 */  lui   $at, %hi(gHudOffsetX) # $at, 0x8012
/* 09FCF0 8009F0F0 AC396D24 */  sw    $t9, %lo(gHudOffsetX)($at)
/* 09FCF4 8009F0F4 3C014000 */  li    $at, 0x40000000 # 2.000000
/* 09FCF8 8009F0F8 44813000 */  mtc1  $at, $f6
/* 09FCFC 8009F0FC 3C018012 */  lui   $at, %hi(gHudBounceMag) # $at, 0x8012
/* 09FD00 8009F100 E4266D30 */  swc1  $f6, %lo(gHudBounceMag)($at)
.L8009F104:
/* 09FD04 8009F104 3C018012 */  lui   $at, %hi(gHudRaceStart) # $at, 0x8012
/* 09FD08 8009F108 A0206D34 */  sb    $zero, %lo(gHudRaceStart)($at)
.L8009F10C:
/* 09FD0C 8009F10C 3C018012 */  lui   $at, %hi(gHudBounceTimer) # $at, 0x8012
/* 09FD10 8009F110 A4206D2C */  sh    $zero, %lo(gHudBounceTimer)($at)
/* 09FD14 8009F114 3C018012 */  lui   $at, %hi(gRaceStartShowHudStep) # $at, 0x8012
/* 09FD18 8009F118 A0206CD4 */  sb    $zero, %lo(gRaceStartShowHudStep)($at)
/* 09FD1C 8009F11C 3C018012 */  lui   $at, %hi(gHideRaceTimer) # $at, 0x8012
/* 09FD20 8009F120 A0206D36 */  sb    $zero, %lo(gHideRaceTimer)($at)
/* 09FD24 8009F124 3C018012 */  lui   $at, %hi(gWrongWayNagPrefix) # $at, 0x8012
/* 09FD28 8009F128 A0206D38 */  sb    $zero, %lo(gWrongWayNagPrefix)($at)
/* 09FD2C 8009F12C 3C018012 */  lui   $at, %hi(D_80126D35) # $at, 0x8012
/* 09FD30 8009F130 A0206D35 */  sb    $zero, %lo(D_80126D35)($at)
/* 09FD34 8009F134 3C018012 */  lui   $at, %hi(gMinimapOpacity) # $at, 0x8012
/* 09FD38 8009F138 26100300 */  addiu $s0, %lo(osTvType) # addiu $s0, $s0, 0x300
/* 09FD3C 8009F13C A0206CD0 */  sb    $zero, %lo(gMinimapOpacity)($at)
/* 09FD40 8009F140 8E0E0000 */  lw    $t6, ($s0)
/* 09FD44 8009F144 2419FF9C */  li    $t9, -100
/* 09FD48 8009F148 15C00005 */  bnez  $t6, .L8009F160
/* 09FD4C 8009F14C 24040078 */   li    $a0, 120
/* 09FD50 8009F150 240F003C */  li    $t7, 60
/* 09FD54 8009F154 3C018012 */  lui   $at, %hi(gMinimapOpacityTarget) # $at, 0x8012
/* 09FD58 8009F158 10000004 */  b     .L8009F16C
/* 09FD5C 8009F15C A02F718B */   sb    $t7, %lo(gMinimapOpacityTarget)($at)
.L8009F160:
/* 09FD60 8009F160 24180032 */  li    $t8, 50
/* 09FD64 8009F164 3C018012 */  lui   $at, %hi(gMinimapOpacityTarget) # $at, 0x8012
/* 09FD68 8009F168 A038718B */  sb    $t8, %lo(gMinimapOpacityTarget)($at)
.L8009F16C:
/* 09FD6C 8009F16C 3C018012 */  lui   $at, %hi(gMinimapFade) # $at, 0x8012
/* 09FD70 8009F170 A0206CD1 */  sb    $zero, %lo(gMinimapFade)($at)
/* 09FD74 8009F174 3C018012 */  lui   $at, %hi(gShowHUD) # $at, 0x8012
/* 09FD78 8009F178 A0206CD2 */  sb    $zero, %lo(gShowHUD)($at)
/* 09FD7C 8009F17C 3C018012 */  lui   $at, %hi(D_80126CD5) # $at, 0x8012
/* 09FD80 8009F180 A0206CD5 */  sb    $zero, %lo(D_80126CD5)($at)
/* 09FD84 8009F184 3C018012 */  lui   $at, %hi(D_80127189) # $at, 0x8012
/* 09FD88 8009F188 A0207189 */  sb    $zero, %lo(D_80127189)($at)
/* 09FD8C 8009F18C 3C018012 */  lui   $at, %hi(D_80126D4C) # $at, 0x8012
/* 09FD90 8009F190 AC396D4C */  sw    $t9, %lo(D_80126D4C)($at)
/* 09FD94 8009F194 0C01BE53 */  jal   get_random_number_from_range
/* 09FD98 8009F198 24050168 */   li    $a1, 360
/* 09FD9C 8009F19C 3C018012 */  lui   $at, %hi(D_80126D50) # $at, 0x8012
/* 09FDA0 8009F1A0 0C003856 */  jal   func_8000E158
/* 09FDA4 8009F1A4 AC226D50 */   sw    $v0, %lo(D_80126D50)($at)
/* 09FDA8 8009F1A8 3C018012 */  lui   $at, %hi(D_8012718A) # $at, 0x8012
/* 09FDAC 8009F1AC A022718A */  sb    $v0, %lo(D_8012718A)($at)
/* 09FDB0 8009F1B0 8E180000 */  lw    $t8, ($s0)
/* 09FDB4 8009F1B4 3C038012 */  lui   $v1, %hi(gStopwatchErrorX) # $v1, 0x8012
/* 09FDB8 8009F1B8 3C048012 */  lui   $a0, %hi(gStopwatchErrorY) # $a0, 0x8012
/* 09FDBC 8009F1BC 24847190 */  addiu $a0, %lo(gStopwatchErrorY) # addiu $a0, $a0, 0x7190
/* 09FDC0 8009F1C0 2463718C */  addiu $v1, %lo(gStopwatchErrorX) # addiu $v1, $v1, 0x718c
/* 09FDC4 8009F1C4 240E0037 */  li    $t6, 55
/* 09FDC8 8009F1C8 240F00B3 */  li    $t7, 179
/* 09FDCC 8009F1CC AC6E0000 */  sw    $t6, ($v1)
/* 09FDD0 8009F1D0 17000006 */  bnez  $t8, .L8009F1EC
/* 09FDD4 8009F1D4 AC8F0000 */   sw    $t7, ($a0)
/* 09FDD8 8009F1D8 25EE001E */  addiu $t6, $t7, 0x1e
/* 09FDDC 8009F1DC 8C6F0000 */  lw    $t7, ($v1)
/* 09FDE0 8009F1E0 AC8E0000 */  sw    $t6, ($a0)
/* 09FDE4 8009F1E4 25F8FFFC */  addiu $t8, $t7, -4
/* 09FDE8 8009F1E8 AC780000 */  sw    $t8, ($v1)
.L8009F1EC:
/* 09FDEC 8009F1EC 0C006D90 */  jal   get_time_trial_ghost
/* 09FDF0 8009F1F0 00000000 */   nop   
/* 09FDF4 8009F1F4 14400004 */  bnez  $v0, .L8009F208
/* 09FDF8 8009F1F8 240C0001 */   li    $t4, 1
/* 09FDFC 8009F1FC 3C018012 */  lui   $at, %hi(gHudTimeTrialGhost) # $at, 0x8012
/* 09FE00 8009F200 10000004 */  b     .L8009F214
/* 09FE04 8009F204 A0206D71 */   sb    $zero, %lo(gHudTimeTrialGhost)($at)
.L8009F208:
/* 09FE08 8009F208 24190001 */  li    $t9, 1
/* 09FE0C 8009F20C 3C018012 */  lui   $at, %hi(gHudTimeTrialGhost) # $at, 0x8012
/* 09FE10 8009F210 A0396D71 */  sb    $t9, %lo(gHudTimeTrialGhost)($at)
.L8009F214:
/* 09FE14 8009F214 3C028012 */  lui   $v0, %hi(gNumActivePlayers) # $v0, 0x8012
/* 09FE18 8009F218 90426D37 */  lbu   $v0, %lo(gNumActivePlayers)($v0)
/* 09FE1C 8009F21C 24010003 */  li    $at, 3
/* 09FE20 8009F220 10410003 */  beq   $v0, $at, .L8009F230
/* 09FE24 8009F224 240E0004 */   li    $t6, 4
/* 09FE28 8009F228 10000002 */  b     .L8009F234
/* 09FE2C 8009F22C AFA2008C */   sw    $v0, 0x8c($sp)
.L8009F230:
/* 09FE30 8009F230 AFAE008C */  sw    $t6, 0x8c($sp)
.L8009F234:
/* 09FE34 8009F234 8FAF008C */  lw    $t7, 0x8c($sp)
/* 09FE38 8009F238 00003825 */  move  $a3, $zero
/* 09FE3C 8009F23C 19E00359 */  blez  $t7, .L8009FFA4
/* 09FE40 8009F240 3C188012 */   lui   $t8, %hi(gPlayerHud) # $t8, 0x8012
/* 09FE44 8009F244 3C01C268 */  li    $at, 0xC2680000 # -58.000000
/* 09FE48 8009F248 4481E000 */  mtc1  $at, $f28
/* 09FE4C 8009F24C 3C01C2F0 */  li    $at, 0xC2F00000 # -120.000000
/* 09FE50 8009F250 4481D000 */  mtc1  $at, $f26
/* 09FE54 8009F254 3C01C1C8 */  li    $at, 0xC1C80000 # -25.000000
/* 09FE58 8009F258 4481C000 */  mtc1  $at, $f24
/* 09FE5C 8009F25C 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 09FE60 8009F260 4481B000 */  mtc1  $at, $f22
/* 09FE64 8009F264 3C0141E0 */  li    $at, 0x41E00000 # 28.000000
/* 09FE68 8009F268 4481A000 */  mtc1  $at, $f20
/* 09FE6C 8009F26C 3C013FE8 */  li    $at, 0x3FE80000 # 1.812500
/* 09FE70 8009F270 44818800 */  mtc1  $at, $f17
/* 09FE74 8009F274 3C01800F */  lui   $at, %hi(D_800E86A8 + 4) # $at, 0x800f
/* 09FE78 8009F278 27186CE0 */  addiu $t8, %lo(gPlayerHud) # addiu $t8, $t8, 0x6ce0
/* 09FE7C 8009F27C 3C10800E */  lui   $s0, %hi(D_800E25C4) # $s0, 0x800e
/* 09FE80 8009F280 3C098012 */  lui   $t1, %hi(gAssetHudElementIds) # $t1, 0x8012
/* 09FE84 8009F284 3C058012 */  lui   $a1, %hi(gCurrentHud) # $a1, 0x8012
/* 09FE88 8009F288 C42F86A8 */  lwc1  $f15, %lo(D_800E86A8)($at)
/* 09FE8C 8009F28C C42E86AC */  lwc1  $f14, %lo(D_800E86A8 + 4)($at)
/* 09FE90 8009F290 44808000 */  mtc1  $zero, $f16
/* 09FE94 8009F294 24A56CDC */  addiu $a1, %lo(gCurrentHud) # addiu $a1, $a1, 0x6cdc
/* 09FE98 8009F298 25296CF0 */  addiu $t1, %lo(gAssetHudElementIds) # addiu $t1, $t1, 0x6cf0
/* 09FE9C 8009F29C 261025C4 */  addiu $s0, %lo(D_800E25C4) # addiu $s0, $s0, 0x25c4
/* 09FEA0 8009F2A0 AFB8006C */  sw    $t8, 0x6c($sp)
/* 09FEA4 8009F2A4 241E0036 */  li    $fp, 54
/* 09FEA8 8009F2A8 2417001C */  li    $s7, 28
/* 09FEAC 8009F2AC 2416000C */  li    $s6, 12
/* 09FEB0 8009F2B0 2415000B */  li    $s5, 11
/* 09FEB4 8009F2B4 2414000F */  li    $s4, 15
/* 09FEB8 8009F2B8 24130004 */  li    $s3, 4
/* 09FEBC 8009F2BC 24120760 */  li    $s2, 1888
/* 09FEC0 8009F2C0 2411000A */  li    $s1, 10
/* 09FEC4 8009F2C4 241F0012 */  li    $ra, 18
/* 09FEC8 8009F2C8 240D0009 */  li    $t5, 9
/* 09FECC 8009F2CC 240B0002 */  li    $t3, 2
/* 09FED0 8009F2D0 240AFFFF */  li    $t2, -1
/* 09FED4 8009F2D4 3408C000 */  li    $t0, 49152
.L8009F2D8:
/* 09FED8 8009F2D8 8FB9006C */  lw    $t9, 0x6c($sp)
/* 09FEDC 8009F2DC 3C02800E */  lui   $v0, %hi(gHudElementBase) # $v0, 0x800e
/* 09FEE0 8009F2E0 8F2E0000 */  lw    $t6, ($t9)
/* 09FEE4 8009F2E4 24421E64 */  addiu $v0, %lo(gHudElementBase) # addiu $v0, $v0, 0x1e64
/* 09FEE8 8009F2E8 00001825 */  move  $v1, $zero
/* 09FEEC 8009F2EC ACAE0000 */  sw    $t6, ($a1)
.L8009F2F0:
/* 09FEF0 8009F2F0 8CAF0000 */  lw    $t7, ($a1)
/* 09FEF4 8009F2F4 C448000C */  lwc1  $f8, 0xc($v0)
/* 09FEF8 8009F2F8 01E3C021 */  addu  $t8, $t7, $v1
/* 09FEFC 8009F2FC E708000C */  swc1  $f8, 0xc($t8)
/* 09FF00 8009F300 8CB90000 */  lw    $t9, ($a1)
/* 09FF04 8009F304 C44A0010 */  lwc1  $f10, 0x10($v0)
/* 09FF08 8009F308 03237021 */  addu  $t6, $t9, $v1
/* 09FF0C 8009F30C E5CA0010 */  swc1  $f10, 0x10($t6)
/* 09FF10 8009F310 8CAF0000 */  lw    $t7, ($a1)
/* 09FF14 8009F314 C4440014 */  lwc1  $f4, 0x14($v0)
/* 09FF18 8009F318 01E3C021 */  addu  $t8, $t7, $v1
/* 09FF1C 8009F31C E7040014 */  swc1  $f4, 0x14($t8)
/* 09FF20 8009F320 8CB90000 */  lw    $t9, ($a1)
/* 09FF24 8009F324 C4460008 */  lwc1  $f6, 8($v0)
/* 09FF28 8009F328 03237021 */  addu  $t6, $t9, $v1
/* 09FF2C 8009F32C E5C60008 */  swc1  $f6, 8($t6)
/* 09FF30 8009F330 8CB80000 */  lw    $t8, ($a1)
/* 09FF34 8009F334 844F0004 */  lh    $t7, 4($v0)
/* 09FF38 8009F338 0303C821 */  addu  $t9, $t8, $v1
/* 09FF3C 8009F33C A72F0004 */  sh    $t7, 4($t9)
/* 09FF40 8009F340 8CB80000 */  lw    $t8, ($a1)
/* 09FF44 8009F344 844E0002 */  lh    $t6, 2($v0)
/* 09FF48 8009F348 03037821 */  addu  $t7, $t8, $v1
/* 09FF4C 8009F34C A5EE0002 */  sh    $t6, 2($t7)
/* 09FF50 8009F350 8CB80000 */  lw    $t8, ($a1)
/* 09FF54 8009F354 84590000 */  lh    $t9, ($v0)
/* 09FF58 8009F358 03037021 */  addu  $t6, $t8, $v1
/* 09FF5C 8009F35C A5D90000 */  sh    $t9, ($t6)
/* 09FF60 8009F360 8CB80000 */  lw    $t8, ($a1)
/* 09FF64 8009F364 844F0006 */  lh    $t7, 6($v0)
/* 09FF68 8009F368 0303C821 */  addu  $t9, $t8, $v1
/* 09FF6C 8009F36C A72F0006 */  sh    $t7, 6($t9)
/* 09FF70 8009F370 8CB80000 */  lw    $t8, ($a1)
/* 09FF74 8009F374 844E0018 */  lh    $t6, 0x18($v0)
/* 09FF78 8009F378 03037821 */  addu  $t7, $t8, $v1
/* 09FF7C 8009F37C A5EE0018 */  sh    $t6, 0x18($t7)
/* 09FF80 8009F380 8CB80000 */  lw    $t8, ($a1)
/* 09FF84 8009F384 8059001A */  lb    $t9, 0x1a($v0)
/* 09FF88 8009F388 03037021 */  addu  $t6, $t8, $v1
/* 09FF8C 8009F38C A1D9001A */  sb    $t9, 0x1a($t6)
/* 09FF90 8009F390 8CB80000 */  lw    $t8, ($a1)
/* 09FF94 8009F394 804F001B */  lb    $t7, 0x1b($v0)
/* 09FF98 8009F398 0303C821 */  addu  $t9, $t8, $v1
/* 09FF9C 8009F39C A32F001B */  sb    $t7, 0x1b($t9)
/* 09FFA0 8009F3A0 8CB80000 */  lw    $t8, ($a1)
/* 09FFA4 8009F3A4 804E001C */  lb    $t6, 0x1c($v0)
/* 09FFA8 8009F3A8 03037821 */  addu  $t7, $t8, $v1
/* 09FFAC 8009F3AC A1EE001C */  sb    $t6, 0x1c($t7)
/* 09FFB0 8009F3B0 8CB80000 */  lw    $t8, ($a1)
/* 09FFB4 8009F3B4 8059001D */  lb    $t9, 0x1d($v0)
/* 09FFB8 8009F3B8 24420020 */  addiu $v0, $v0, 0x20
/* 09FFBC 8009F3BC 03037021 */  addu  $t6, $t8, $v1
/* 09FFC0 8009F3C0 24630020 */  addiu $v1, $v1, 0x20
/* 09FFC4 8009F3C4 1450FFCA */  bne   $v0, $s0, .L8009F2F0
/* 09FFC8 8009F3C8 A1D9001D */   sb    $t9, 0x1d($t6)
/* 09FFCC 8009F3CC 8CB80000 */  lw    $t8, ($a1)
/* 09FFD0 8009F3D0 240F0005 */  li    $t7, 5
/* 09FFD4 8009F3D4 3C038012 */  lui   $v1, %hi(gHUDNumPlayers) # $v1, 0x8012
/* 09FFD8 8009F3D8 A30F05DC */  sb    $t7, 0x5dc($t8)
/* 09FFDC 8009F3DC 8C636D0C */  lw    $v1, %lo(gHUDNumPlayers)($v1)
/* 09FFE0 8009F3E0 3C014188 */  li    $at, 0x41880000 # 17.000000
/* 09FFE4 8009F3E4 15830084 */  bne   $t4, $v1, .L8009F5F8
/* 09FFE8 8009F3E8 00000000 */   nop   
/* 09FFEC 8009F3EC 3C03800E */  lui   $v1, %hi(D_800E25C4) # $v1, 0x800e
/* 09FFF0 8009F3F0 246225C4 */  addiu $v0, $v1, %lo(D_800E25C4)
/* 09FFF4 8009F3F4 84590000 */  lh    $t9, ($v0)
/* 09FFF8 8009F3F8 44810000 */  mtc1  $at, $f0
/* 09FFFC 8009F3FC 1159003F */  beq   $t2, $t9, .L8009F4FC
/* 0A0000 8009F400 3C01800F */   lui   $at, %hi(D_800E86B8) # $at, 0x800f
/* 0A0004 8009F404 3C0E800E */  lui   $t6, %hi(D_800E25C4) # $t6, 0x800e
/* 0A0008 8009F408 25CE25C4 */  addiu $t6, %lo(D_800E25C4) # addiu $t6, $t6, 0x25c4
/* 0A000C 8009F40C 85C40000 */  lh    $a0, ($t6)
/* 0A0010 8009F410 00000000 */  nop   
.L8009F414:
/* 0A0014 8009F414 844F0002 */  lh    $t7, 2($v0)
/* 0A0018 8009F418 8CB80000 */  lw    $t8, ($a1)
/* 0A001C 8009F41C 448F4000 */  mtc1  $t7, $f8
/* 0A0020 8009F420 0004C940 */  sll   $t9, $a0, 5
/* 0A0024 8009F424 468042A0 */  cvt.s.w $f10, $f8
/* 0A0028 8009F428 03197021 */  addu  $t6, $t8, $t9
/* 0A002C 8009F42C E5CA000C */  swc1  $f10, 0xc($t6)
/* 0A0030 8009F430 84580000 */  lh    $t8, ($v0)
/* 0A0034 8009F434 8CAF0000 */  lw    $t7, ($a1)
/* 0A0038 8009F438 0018C940 */  sll   $t9, $t8, 5
/* 0A003C 8009F43C 01F91821 */  addu  $v1, $t7, $t9
/* 0A0040 8009F440 84780006 */  lh    $t8, 6($v1)
/* 0A0044 8009F444 8D2E0000 */  lw    $t6, ($t1)
/* 0A0048 8009F448 00187840 */  sll   $t7, $t8, 1
/* 0A004C 8009F44C 01CFC821 */  addu  $t9, $t6, $t7
/* 0A0050 8009F450 87380000 */  lh    $t8, ($t9)
/* 0A0054 8009F454 00000000 */  nop   
/* 0A0058 8009F458 330EC000 */  andi  $t6, $t8, 0xc000
/* 0A005C 8009F45C 150E0012 */  bne   $t0, $t6, .L8009F4A8
/* 0A0060 8009F460 00000000 */   nop   
/* 0A0064 8009F464 14E00008 */  bnez  $a3, .L8009F488
/* 0A0068 8009F468 00000000 */   nop   
/* 0A006C 8009F46C 844F0004 */  lh    $t7, 4($v0)
/* 0A0070 8009F470 00000000 */  nop   
/* 0A0074 8009F474 448F2000 */  mtc1  $t7, $f4
/* 0A0078 8009F478 00000000 */  nop   
/* 0A007C 8009F47C 468021A0 */  cvt.s.w $f6, $f4
/* 0A0080 8009F480 1000001A */  b     .L8009F4EC
/* 0A0084 8009F484 E4660010 */   swc1  $f6, 0x10($v1)
.L8009F488:
/* 0A0088 8009F488 84590004 */  lh    $t9, 4($v0)
/* 0A008C 8009F48C 00000000 */  nop   
/* 0A0090 8009F490 2738006C */  addiu $t8, $t9, 0x6c
/* 0A0094 8009F494 44984000 */  mtc1  $t8, $f8
/* 0A0098 8009F498 00000000 */  nop   
/* 0A009C 8009F49C 468042A0 */  cvt.s.w $f10, $f8
/* 0A00A0 8009F4A0 10000012 */  b     .L8009F4EC
/* 0A00A4 8009F4A4 E46A0010 */   swc1  $f10, 0x10($v1)
.L8009F4A8:
/* 0A00A8 8009F4A8 14E00009 */  bnez  $a3, .L8009F4D0
/* 0A00AC 8009F4AC 00000000 */   nop   
/* 0A00B0 8009F4B0 844E0004 */  lh    $t6, 4($v0)
/* 0A00B4 8009F4B4 00000000 */  nop   
/* 0A00B8 8009F4B8 25CF003C */  addiu $t7, $t6, 0x3c
/* 0A00BC 8009F4BC 448F2000 */  mtc1  $t7, $f4
/* 0A00C0 8009F4C0 00000000 */  nop   
/* 0A00C4 8009F4C4 468021A0 */  cvt.s.w $f6, $f4
/* 0A00C8 8009F4C8 10000008 */  b     .L8009F4EC
/* 0A00CC 8009F4CC E4660010 */   swc1  $f6, 0x10($v1)
.L8009F4D0:
/* 0A00D0 8009F4D0 84590004 */  lh    $t9, 4($v0)
/* 0A00D4 8009F4D4 00000000 */  nop   
/* 0A00D8 8009F4D8 2738FFD0 */  addiu $t8, $t9, -0x30
/* 0A00DC 8009F4DC 44984000 */  mtc1  $t8, $f8
/* 0A00E0 8009F4E0 00000000 */  nop   
/* 0A00E4 8009F4E4 468042A0 */  cvt.s.w $f10, $f8
/* 0A00E8 8009F4E8 E46A0010 */  swc1  $f10, 0x10($v1)
.L8009F4EC:
/* 0A00EC 8009F4EC 84440006 */  lh    $a0, 6($v0)
/* 0A00F0 8009F4F0 24420006 */  addiu $v0, $v0, 6
/* 0A00F4 8009F4F4 1544FFC7 */  bne   $t2, $a0, .L8009F414
/* 0A00F8 8009F4F8 00000000 */   nop   
.L8009F4FC:
/* 0A00FC 8009F4FC C42486B8 */  lwc1  $f4, %lo(D_800E86B8)($at)
/* 0A0100 8009F500 8CAE0000 */  lw    $t6, ($a1)
/* 0A0104 8009F504 3C014377 */  li    $at, 0x43770000 # 247.000000
/* 0A0108 8009F508 E5C4064C */  swc1  $f4, 0x64c($t6)
/* 0A010C 8009F50C 8CAF0000 */  lw    $t7, ($a1)
/* 0A0110 8009F510 44813000 */  mtc1  $at, $f6
/* 0A0114 8009F514 E5E00650 */  swc1  $f0, 0x650($t7)
/* 0A0118 8009F518 8CB90000 */  lw    $t9, ($a1)
/* 0A011C 8009F51C 3C014234 */  li    $at, 0x42340000 # 45.000000
/* 0A0120 8009F520 E726040C */  swc1  $f6, 0x40c($t9)
/* 0A0124 8009F524 8CB80000 */  lw    $t8, ($a1)
/* 0A0128 8009F528 44814000 */  mtc1  $at, $f8
/* 0A012C 8009F52C 3C01800F */  lui   $at, %hi(D_800E86BC) # $at, 0x800f
/* 0A0130 8009F530 E7080410 */  swc1  $f8, 0x410($t8)
/* 0A0134 8009F534 8CAE0000 */  lw    $t6, ($a1)
/* 0A0138 8009F538 C42A86BC */  lwc1  $f10, %lo(D_800E86BC)($at)
/* 0A013C 8009F53C 3C014244 */  li    $at, 0x42440000 # 49.000000
/* 0A0140 8009F540 E5CA066C */  swc1  $f10, 0x66c($t6)
/* 0A0144 8009F544 8CAF0000 */  lw    $t7, ($a1)
/* 0A0148 8009F548 44812000 */  mtc1  $at, $f4
/* 0A014C 8009F54C 3C0142CE */  li    $at, 0x42CE0000 # 103.000000
/* 0A0150 8009F550 E5E40670 */  swc1  $f4, 0x670($t7)
/* 0A0154 8009F554 8CB90000 */  lw    $t9, ($a1)
/* 0A0158 8009F558 44813000 */  mtc1  $at, $f6
/* 0A015C 8009F55C 3C014244 */  li    $at, 0x42440000 # 49.000000
/* 0A0160 8009F560 E726068C */  swc1  $f6, 0x68c($t9)
/* 0A0164 8009F564 8CB80000 */  lw    $t8, ($a1)
/* 0A0168 8009F568 44814000 */  mtc1  $at, $f8
/* 0A016C 8009F56C 3C014384 */  li    $at, 0x43840000 # 264.000000
/* 0A0170 8009F570 E7080690 */  swc1  $f8, 0x690($t8)
/* 0A0174 8009F574 8CAE0000 */  lw    $t6, ($a1)
/* 0A0178 8009F578 44815000 */  mtc1  $at, $f10
/* 0A017C 8009F57C 3C014248 */  li    $at, 0x42480000 # 50.000000
/* 0A0180 8009F580 E5CA06AC */  swc1  $f10, 0x6ac($t6)
/* 0A0184 8009F584 8CAF0000 */  lw    $t7, ($a1)
/* 0A0188 8009F588 44812000 */  mtc1  $at, $f4
/* 0A018C 8009F58C 3C01800F */  lui   $at, %hi(D_800E86C0) # $at, 0x800f
/* 0A0190 8009F590 E5E406B0 */  swc1  $f4, 0x6b0($t7)
/* 0A0194 8009F594 8CB90000 */  lw    $t9, ($a1)
/* 0A0198 8009F598 C42686C0 */  lwc1  $f6, %lo(D_800E86C0)($at)
/* 0A019C 8009F59C 3C01424C */  li    $at, 0x424C0000 # 51.000000
/* 0A01A0 8009F5A0 E72606CC */  swc1  $f6, 0x6cc($t9)
/* 0A01A4 8009F5A4 8CB80000 */  lw    $t8, ($a1)
/* 0A01A8 8009F5A8 44814000 */  mtc1  $at, $f8
/* 0A01AC 8009F5AC 3C014389 */  li    $at, 0x43890000 # 274.000000
/* 0A01B0 8009F5B0 E70806D0 */  swc1  $f8, 0x6d0($t8)
/* 0A01B4 8009F5B4 8CAE0000 */  lw    $t6, ($a1)
/* 0A01B8 8009F5B8 44815000 */  mtc1  $at, $f10
/* 0A01BC 8009F5BC 3C01424C */  li    $at, 0x424C0000 # 51.000000
/* 0A01C0 8009F5C0 E5CA06EC */  swc1  $f10, 0x6ec($t6)
/* 0A01C4 8009F5C4 8CAF0000 */  lw    $t7, ($a1)
/* 0A01C8 8009F5C8 44812000 */  mtc1  $at, $f4
/* 0A01CC 8009F5CC 24180003 */  li    $t8, 3
/* 0A01D0 8009F5D0 E5E406F0 */  swc1  $f4, 0x6f0($t7)
/* 0A01D4 8009F5D4 8CB90000 */  lw    $t9, ($a1)
/* 0A01D8 8009F5D8 3C038012 */  lui   $v1, %hi(gHUDNumPlayers) # $v1, 0x8012
/* 0A01DC 8009F5DC E7200650 */  swc1  $f0, 0x650($t9)
/* 0A01E0 8009F5E0 8CAE0000 */  lw    $t6, ($a1)
/* 0A01E4 8009F5E4 00000000 */  nop   
/* 0A01E8 8009F5E8 A1D805DC */  sb    $t8, 0x5dc($t6)
/* 0A01EC 8009F5EC 8C636D0C */  lw    $v1, %lo(gHUDNumPlayers)($v1)
/* 0A01F0 8009F5F0 10000072 */  b     .L8009F7BC
/* 0A01F4 8009F5F4 00000000 */   nop   
.L8009F5F8:
/* 0A01F8 8009F5F8 11630002 */  beq   $t3, $v1, .L8009F604
/* 0A01FC 8009F5FC 24010003 */   li    $at, 3
/* 0A0200 8009F600 1461006E */  bne   $v1, $at, .L8009F7BC
.L8009F604:
/* 0A0204 8009F604 3C04800E */   lui   $a0, %hi(D_800E2684) # $a0, 0x800e
/* 0A0208 8009F608 24822684 */  addiu $v0, $a0, %lo(D_800E2684)
/* 0A020C 8009F60C 844F0000 */  lh    $t7, ($v0)
/* 0A0210 8009F610 3C19800E */  lui   $t9, %hi(D_800E2684) # $t9, 0x800e
/* 0A0214 8009F614 114F0069 */  beq   $t2, $t7, .L8009F7BC
/* 0A0218 8009F618 27392684 */   addiu $t9, %lo(D_800E2684) # addiu $t9, $t9, 0x2684
/* 0A021C 8009F61C 87230000 */  lh    $v1, ($t9)
/* 0A0220 8009F620 00000000 */  nop   
.L8009F624:
/* 0A0224 8009F624 8CB80000 */  lw    $t8, ($a1)
/* 0A0228 8009F628 00037140 */  sll   $t6, $v1, 5
/* 0A022C 8009F62C 030E2021 */  addu  $a0, $t8, $t6
/* 0A0230 8009F630 84990006 */  lh    $t9, 6($a0)
/* 0A0234 8009F634 8D2F0000 */  lw    $t7, ($t1)
/* 0A0238 8009F638 0019C040 */  sll   $t8, $t9, 1
/* 0A023C 8009F63C 01F87021 */  addu  $t6, $t7, $t8
/* 0A0240 8009F640 85D90000 */  lh    $t9, ($t6)
/* 0A0244 8009F644 00000000 */  nop   
/* 0A0248 8009F648 332FC000 */  andi  $t7, $t9, 0xc000
/* 0A024C 8009F64C 150F002A */  bne   $t0, $t7, .L8009F6F8
/* 0A0250 8009F650 00000000 */   nop   
/* 0A0254 8009F654 10E00003 */  beqz  $a3, .L8009F664
/* 0A0258 8009F658 00000000 */   nop   
/* 0A025C 8009F65C 14EB0008 */  bne   $a3, $t3, .L8009F680
/* 0A0260 8009F660 00000000 */   nop   
.L8009F664:
/* 0A0264 8009F664 84580002 */  lh    $t8, 2($v0)
/* 0A0268 8009F668 00000000 */  nop   
/* 0A026C 8009F66C 44983000 */  mtc1  $t8, $f6
/* 0A0270 8009F670 00000000 */  nop   
/* 0A0274 8009F674 46803220 */  cvt.s.w $f8, $f6
/* 0A0278 8009F678 10000008 */  b     .L8009F69C
/* 0A027C 8009F67C E488000C */   swc1  $f8, 0xc($a0)
.L8009F680:
/* 0A0280 8009F680 844E0006 */  lh    $t6, 6($v0)
/* 0A0284 8009F684 00000000 */  nop   
/* 0A0288 8009F688 25D900A0 */  addiu $t9, $t6, 0xa0
/* 0A028C 8009F68C 44995000 */  mtc1  $t9, $f10
/* 0A0290 8009F690 00000000 */  nop   
/* 0A0294 8009F694 46805120 */  cvt.s.w $f4, $f10
/* 0A0298 8009F698 E484000C */  swc1  $f4, 0xc($a0)
.L8009F69C:
/* 0A029C 8009F69C 10E00003 */  beqz  $a3, .L8009F6AC
/* 0A02A0 8009F6A0 00000000 */   nop   
/* 0A02A4 8009F6A4 14EC000A */  bne   $a3, $t4, .L8009F6D0
/* 0A02A8 8009F6A8 00000000 */   nop   
.L8009F6AC:
/* 0A02AC 8009F6AC 844F0004 */  lh    $t7, 4($v0)
/* 0A02B0 8009F6B0 844E0000 */  lh    $t6, ($v0)
/* 0A02B4 8009F6B4 448F3000 */  mtc1  $t7, $f6
/* 0A02B8 8009F6B8 8CB80000 */  lw    $t8, ($a1)
/* 0A02BC 8009F6BC 46803220 */  cvt.s.w $f8, $f6
/* 0A02C0 8009F6C0 000EC940 */  sll   $t9, $t6, 5
/* 0A02C4 8009F6C4 03197821 */  addu  $t7, $t8, $t9
/* 0A02C8 8009F6C8 10000035 */  b     .L8009F7A0
/* 0A02CC 8009F6CC E5E80010 */   swc1  $f8, 0x10($t7)
.L8009F6D0:
/* 0A02D0 8009F6D0 844E0004 */  lh    $t6, 4($v0)
/* 0A02D4 8009F6D4 844F0000 */  lh    $t7, ($v0)
/* 0A02D8 8009F6D8 25D8006C */  addiu $t8, $t6, 0x6c
/* 0A02DC 8009F6DC 44985000 */  mtc1  $t8, $f10
/* 0A02E0 8009F6E0 8CB90000 */  lw    $t9, ($a1)
/* 0A02E4 8009F6E4 46805120 */  cvt.s.w $f4, $f10
/* 0A02E8 8009F6E8 000F7140 */  sll   $t6, $t7, 5
/* 0A02EC 8009F6EC 032EC021 */  addu  $t8, $t9, $t6
/* 0A02F0 8009F6F0 1000002B */  b     .L8009F7A0
/* 0A02F4 8009F6F4 E7040010 */   swc1  $f4, 0x10($t8)
.L8009F6F8:
/* 0A02F8 8009F6F8 10E00003 */  beqz  $a3, .L8009F708
/* 0A02FC 8009F6FC 00000000 */   nop   
/* 0A0300 8009F700 14EB0009 */  bne   $a3, $t3, .L8009F728
/* 0A0304 8009F704 00000000 */   nop   
.L8009F708:
/* 0A0308 8009F708 844F0002 */  lh    $t7, 2($v0)
/* 0A030C 8009F70C 00000000 */  nop   
/* 0A0310 8009F710 25F9FFB0 */  addiu $t9, $t7, -0x50
/* 0A0314 8009F714 44993000 */  mtc1  $t9, $f6
/* 0A0318 8009F718 00000000 */  nop   
/* 0A031C 8009F71C 46803220 */  cvt.s.w $f8, $f6
/* 0A0320 8009F720 10000008 */  b     .L8009F744
/* 0A0324 8009F724 E488000C */   swc1  $f8, 0xc($a0)
.L8009F728:
/* 0A0328 8009F728 844E0006 */  lh    $t6, 6($v0)
/* 0A032C 8009F72C 00000000 */  nop   
/* 0A0330 8009F730 25D80050 */  addiu $t8, $t6, 0x50
/* 0A0334 8009F734 44985000 */  mtc1  $t8, $f10
/* 0A0338 8009F738 00000000 */  nop   
/* 0A033C 8009F73C 46805120 */  cvt.s.w $f4, $f10
/* 0A0340 8009F740 E484000C */  swc1  $f4, 0xc($a0)
.L8009F744:
/* 0A0344 8009F744 10E00003 */  beqz  $a3, .L8009F754
/* 0A0348 8009F748 00000000 */   nop   
/* 0A034C 8009F74C 14EC000B */  bne   $a3, $t4, .L8009F77C
/* 0A0350 8009F750 00000000 */   nop   
.L8009F754:
/* 0A0354 8009F754 844F0004 */  lh    $t7, 4($v0)
/* 0A0358 8009F758 84580000 */  lh    $t8, ($v0)
/* 0A035C 8009F75C 25F9003C */  addiu $t9, $t7, 0x3c
/* 0A0360 8009F760 44993000 */  mtc1  $t9, $f6
/* 0A0364 8009F764 8CAE0000 */  lw    $t6, ($a1)
/* 0A0368 8009F768 46803220 */  cvt.s.w $f8, $f6
/* 0A036C 8009F76C 00187940 */  sll   $t7, $t8, 5
/* 0A0370 8009F770 01CFC821 */  addu  $t9, $t6, $t7
/* 0A0374 8009F774 1000000A */  b     .L8009F7A0
/* 0A0378 8009F778 E7280010 */   swc1  $f8, 0x10($t9)
.L8009F77C:
/* 0A037C 8009F77C 84580004 */  lh    $t8, 4($v0)
/* 0A0380 8009F780 84590000 */  lh    $t9, ($v0)
/* 0A0384 8009F784 270EFFD0 */  addiu $t6, $t8, -0x30
/* 0A0388 8009F788 448E5000 */  mtc1  $t6, $f10
/* 0A038C 8009F78C 8CAF0000 */  lw    $t7, ($a1)
/* 0A0390 8009F790 46805120 */  cvt.s.w $f4, $f10
/* 0A0394 8009F794 0019C140 */  sll   $t8, $t9, 5
/* 0A0398 8009F798 01F87021 */  addu  $t6, $t7, $t8
/* 0A039C 8009F79C E5C40010 */  swc1  $f4, 0x10($t6)
.L8009F7A0:
/* 0A03A0 8009F7A0 84430008 */  lh    $v1, 8($v0)
/* 0A03A4 8009F7A4 24420008 */  addiu $v0, $v0, 8
/* 0A03A8 8009F7A8 1543FF9E */  bne   $t2, $v1, .L8009F624
/* 0A03AC 8009F7AC 00000000 */   nop   
/* 0A03B0 8009F7B0 3C038012 */  lui   $v1, %hi(gHUDNumPlayers) # $v1, 0x8012
/* 0A03B4 8009F7B4 8C636D0C */  lw    $v1, %lo(gHUDNumPlayers)($v1)
/* 0A03B8 8009F7B8 00000000 */  nop   
.L8009F7BC:
/* 0A03BC 8009F7BC 10600046 */  beqz  $v1, .L8009F8D8
/* 0A03C0 8009F7C0 00000000 */   nop   
/* 0A03C4 8009F7C4 00001825 */  move  $v1, $zero
.L8009F7C8:
/* 0A03C8 8009F7C8 8CB90000 */  lw    $t9, ($a1)
/* 0A03CC 8009F7CC 00000000 */  nop   
/* 0A03D0 8009F7D0 03232021 */  addu  $a0, $t9, $v1
/* 0A03D4 8009F7D4 84820006 */  lh    $v0, 6($a0)
/* 0A03D8 8009F7D8 00000000 */  nop   
/* 0A03DC 8009F7DC 15A20003 */  bne   $t5, $v0, .L8009F7EC
/* 0A03E0 8009F7E0 00000000 */   nop   
/* 0A03E4 8009F7E4 10000036 */  b     .L8009F8C0
/* 0A03E8 8009F7E8 A4910006 */   sh    $s1, 6($a0)
.L8009F7EC:
/* 0A03EC 8009F7EC 16620003 */  bne   $s3, $v0, .L8009F7FC
/* 0A03F0 8009F7F0 00000000 */   nop   
/* 0A03F4 8009F7F4 10000032 */  b     .L8009F8C0
/* 0A03F8 8009F7F8 A4940006 */   sh    $s4, 6($a0)
.L8009F7FC:
/* 0A03FC 8009F7FC 16A20003 */  bne   $s5, $v0, .L8009F80C
/* 0A0400 8009F800 00000000 */   nop   
/* 0A0404 8009F804 1000002E */  b     .L8009F8C0
/* 0A0408 8009F808 A4960006 */   sh    $s6, 6($a0)
.L8009F80C:
/* 0A040C 8009F80C 17E20003 */  bne   $ra, $v0, .L8009F81C
/* 0A0410 8009F810 00000000 */   nop   
/* 0A0414 8009F814 1000002A */  b     .L8009F8C0
/* 0A0418 8009F818 A4970006 */   sh    $s7, 6($a0)
.L8009F81C:
/* 0A041C 8009F81C 17C20008 */  bne   $fp, $v0, .L8009F840
/* 0A0420 8009F820 00000000 */   nop   
/* 0A0424 8009F824 C4860008 */  lwc1  $f6, 8($a0)
/* 0A0428 8009F828 00000000 */  nop   
/* 0A042C 8009F82C 46003221 */  cvt.d.s $f8, $f6
/* 0A0430 8009F830 462E4282 */  mul.d $f10, $f8, $f14
/* 0A0434 8009F834 46205120 */  cvt.s.d $f4, $f10
/* 0A0438 8009F838 10000021 */  b     .L8009F8C0
/* 0A043C 8009F83C E4840008 */   swc1  $f4, 8($a0)
.L8009F840:
/* 0A0440 8009F840 1060001F */  beqz  $v1, .L8009F8C0
/* 0A0444 8009F844 24010020 */   li    $at, 32
/* 0A0448 8009F848 1061001D */  beq   $v1, $at, .L8009F8C0
/* 0A044C 8009F84C 24010040 */   li    $at, 64
/* 0A0450 8009F850 1061001B */  beq   $v1, $at, .L8009F8C0
/* 0A0454 8009F854 240104A0 */   li    $at, 1184
/* 0A0458 8009F858 10610019 */  beq   $v1, $at, .L8009F8C0
/* 0A045C 8009F85C 240105E0 */   li    $at, 1504
/* 0A0460 8009F860 10610017 */  beq   $v1, $at, .L8009F8C0
/* 0A0464 8009F864 24010600 */   li    $at, 1536
/* 0A0468 8009F868 10610015 */  beq   $v1, $at, .L8009F8C0
/* 0A046C 8009F86C 24010060 */   li    $at, 96
/* 0A0470 8009F870 10610013 */  beq   $v1, $at, .L8009F8C0
/* 0A0474 8009F874 24010640 */   li    $at, 1600
/* 0A0478 8009F878 10610011 */  beq   $v1, $at, .L8009F8C0
/* 0A047C 8009F87C 24010660 */   li    $at, 1632
/* 0A0480 8009F880 1061000F */  beq   $v1, $at, .L8009F8C0
/* 0A0484 8009F884 28610680 */   slti  $at, $v1, 0x680
/* 0A0488 8009F888 14200007 */  bnez  $at, .L8009F8A8
/* 0A048C 8009F88C 28610700 */   slti  $at, $v1, 0x700
/* 0A0490 8009F890 10200005 */  beqz  $at, .L8009F8A8
/* 0A0494 8009F894 24010380 */   li    $at, 896
/* 0A0498 8009F898 10610003 */  beq   $v1, $at, .L8009F8A8
/* 0A049C 8009F89C 24010120 */   li    $at, 288
/* 0A04A0 8009F8A0 14610007 */  bne   $v1, $at, .L8009F8C0
/* 0A04A4 8009F8A4 00000000 */   nop   
.L8009F8A8:
/* 0A04A8 8009F8A8 C4860008 */  lwc1  $f6, 8($a0)
/* 0A04AC 8009F8AC 00000000 */  nop   
/* 0A04B0 8009F8B0 46003221 */  cvt.d.s $f8, $f6
/* 0A04B4 8009F8B4 46304282 */  mul.d $f10, $f8, $f16
/* 0A04B8 8009F8B8 46205120 */  cvt.s.d $f4, $f10
/* 0A04BC 8009F8BC E4840008 */  swc1  $f4, 8($a0)
.L8009F8C0:
/* 0A04C0 8009F8C0 24630020 */  addiu $v1, $v1, 0x20
/* 0A04C4 8009F8C4 1472FFC0 */  bne   $v1, $s2, .L8009F7C8
/* 0A04C8 8009F8C8 00000000 */   nop   
/* 0A04CC 8009F8CC 3C038012 */  lui   $v1, %hi(gHUDNumPlayers) # $v1, 0x8012
/* 0A04D0 8009F8D0 8C636D0C */  lw    $v1, %lo(gHUDNumPlayers)($v1)
/* 0A04D4 8009F8D4 00000000 */  nop   
.L8009F8D8:
/* 0A04D8 8009F8D8 8CA40000 */  lw    $a0, ($a1)
/* 0A04DC 8009F8DC 28610002 */  slti  $at, $v1, 2
/* 0A04E0 8009F8E0 1420002F */  bnez  $at, .L8009F9A0
/* 0A04E4 8009F8E4 00000000 */   nop   
/* 0A04E8 8009F8E8 E49603C8 */  swc1  $f22, 0x3c8($a0)
/* 0A04EC 8009F8EC 8CAF0000 */  lw    $t7, ($a1)
/* 0A04F0 8009F8F0 00000000 */  nop   
/* 0A04F4 8009F8F4 E5F603E8 */  swc1  $f22, 0x3e8($t7)
/* 0A04F8 8009F8F8 8CB80000 */  lw    $t8, ($a1)
/* 0A04FC 8009F8FC 00000000 */  nop   
/* 0A0500 8009F900 E71603A8 */  swc1  $f22, 0x3a8($t8)
/* 0A0504 8009F904 8CAE0000 */  lw    $t6, ($a1)
/* 0A0508 8009F908 00000000 */  nop   
/* 0A050C 8009F90C E5D60188 */  swc1  $f22, 0x188($t6)
/* 0A0510 8009F910 8CB90000 */  lw    $t9, ($a1)
/* 0A0514 8009F914 00000000 */  nop   
/* 0A0518 8009F918 E73601A8 */  swc1  $f22, 0x1a8($t9)
/* 0A051C 8009F91C 8CAF0000 */  lw    $t7, ($a1)
/* 0A0520 8009F920 24190026 */  li    $t9, 38
/* 0A0524 8009F924 E5F60468 */  swc1  $f22, 0x468($t7)
/* 0A0528 8009F928 8CB80000 */  lw    $t8, ($a1)
/* 0A052C 8009F92C 00000000 */  nop   
/* 0A0530 8009F930 E7160488 */  swc1  $f22, 0x488($t8)
/* 0A0534 8009F934 8CAE0000 */  lw    $t6, ($a1)
/* 0A0538 8009F938 2418002B */  li    $t8, 43
/* 0A053C 8009F93C E5D601C8 */  swc1  $f22, 0x1c8($t6)
/* 0A0540 8009F940 8CAF0000 */  lw    $t7, ($a1)
/* 0A0544 8009F944 00000000 */  nop   
/* 0A0548 8009F948 A5F903C6 */  sh    $t9, 0x3c6($t7)
/* 0A054C 8009F94C 8CAE0000 */  lw    $t6, ($a1)
/* 0A0550 8009F950 24190029 */  li    $t9, 41
/* 0A0554 8009F954 A5D803E6 */  sh    $t8, 0x3e6($t6)
/* 0A0558 8009F958 8CAF0000 */  lw    $t7, ($a1)
/* 0A055C 8009F95C 24180025 */  li    $t8, 37
/* 0A0560 8009F960 A5F903A6 */  sh    $t9, 0x3a6($t7)
/* 0A0564 8009F964 8CAE0000 */  lw    $t6, ($a1)
/* 0A0568 8009F968 24190027 */  li    $t9, 39
/* 0A056C 8009F96C A5D80186 */  sh    $t8, 0x186($t6)
/* 0A0570 8009F970 8CAF0000 */  lw    $t7, ($a1)
/* 0A0574 8009F974 2418002D */  li    $t8, 45
/* 0A0578 8009F978 A5F901A6 */  sh    $t9, 0x1a6($t7)
/* 0A057C 8009F97C 8CAE0000 */  lw    $t6, ($a1)
/* 0A0580 8009F980 2419002C */  li    $t9, 44
/* 0A0584 8009F984 A5D80466 */  sh    $t8, 0x466($t6)
/* 0A0588 8009F988 8CAF0000 */  lw    $t7, ($a1)
/* 0A058C 8009F98C 2418002A */  li    $t8, 42
/* 0A0590 8009F990 A5F90486 */  sh    $t9, 0x486($t7)
/* 0A0594 8009F994 8CAE0000 */  lw    $t6, ($a1)
/* 0A0598 8009F998 10000005 */  b     .L8009F9B0
/* 0A059C 8009F99C A5D801C6 */   sh    $t8, 0x1c6($t6)
.L8009F9A0:
/* 0A05A0 8009F9A0 A08C005D */  sb    $t4, 0x5d($a0)
/* 0A05A4 8009F9A4 8CB90000 */  lw    $t9, ($a1)
/* 0A05A8 8009F9A8 00000000 */  nop   
/* 0A05AC 8009F9AC A32C037D */  sb    $t4, 0x37d($t9)
.L8009F9B0:
/* 0A05B0 8009F9B0 0C003936 */  jal   is_in_time_trial
/* 0A05B4 8009F9B4 AFA70090 */   sw    $a3, 0x90($sp)
/* 0A05B8 8009F9B8 3C058012 */  lui   $a1, %hi(gCurrentHud) # $a1, 0x8012
/* 0A05BC 8009F9BC 8FA70090 */  lw    $a3, 0x90($sp)
/* 0A05C0 8009F9C0 10400043 */  beqz  $v0, .L8009FAD0
/* 0A05C4 8009F9C4 24A56CDC */   addiu $a1, %lo(gCurrentHud) # addiu $a1, $a1, 0x6cdc
/* 0A05C8 8009F9C8 8CA40000 */  lw    $a0, ($a1)
/* 0A05CC 8009F9CC 00000000 */  nop   
/* 0A05D0 8009F9D0 C48600EC */  lwc1  $f6, 0xec($a0)
/* 0A05D4 8009F9D4 00000000 */  nop   
/* 0A05D8 8009F9D8 46183200 */  add.s $f8, $f6, $f24
/* 0A05DC 8009F9DC E48800EC */  swc1  $f8, 0xec($a0)
/* 0A05E0 8009F9E0 8CA40000 */  lw    $a0, ($a1)
/* 0A05E4 8009F9E4 00000000 */  nop   
/* 0A05E8 8009F9E8 C48A010C */  lwc1  $f10, 0x10c($a0)
/* 0A05EC 8009F9EC 00000000 */  nop   
/* 0A05F0 8009F9F0 46185100 */  add.s $f4, $f10, $f24
/* 0A05F4 8009F9F4 E484010C */  swc1  $f4, 0x10c($a0)
/* 0A05F8 8009F9F8 8CA40000 */  lw    $a0, ($a1)
/* 0A05FC 8009F9FC 00000000 */  nop   
/* 0A0600 8009FA00 C486012C */  lwc1  $f6, 0x12c($a0)
/* 0A0604 8009FA04 00000000 */  nop   
/* 0A0608 8009FA08 46183200 */  add.s $f8, $f6, $f24
/* 0A060C 8009FA0C E488012C */  swc1  $f8, 0x12c($a0)
/* 0A0610 8009FA10 8CA40000 */  lw    $a0, ($a1)
/* 0A0614 8009FA14 00000000 */  nop   
/* 0A0618 8009FA18 C48A038C */  lwc1  $f10, 0x38c($a0)
/* 0A061C 8009FA1C 00000000 */  nop   
/* 0A0620 8009FA20 46185100 */  add.s $f4, $f10, $f24
/* 0A0624 8009FA24 E484038C */  swc1  $f4, 0x38c($a0)
/* 0A0628 8009FA28 8CA40000 */  lw    $a0, ($a1)
/* 0A062C 8009FA2C 00000000 */  nop   
/* 0A0630 8009FA30 C486036C */  lwc1  $f6, 0x36c($a0)
/* 0A0634 8009FA34 00000000 */  nop   
/* 0A0638 8009FA38 46183200 */  add.s $f8, $f6, $f24
/* 0A063C 8009FA3C E488036C */  swc1  $f8, 0x36c($a0)
/* 0A0640 8009FA40 8CA40000 */  lw    $a0, ($a1)
/* 0A0644 8009FA44 00000000 */  nop   
/* 0A0648 8009FA48 C48A024C */  lwc1  $f10, 0x24c($a0)
/* 0A064C 8009FA4C 00000000 */  nop   
/* 0A0650 8009FA50 46185100 */  add.s $f4, $f10, $f24
/* 0A0654 8009FA54 E484024C */  swc1  $f4, 0x24c($a0)
/* 0A0658 8009FA58 8CA40000 */  lw    $a0, ($a1)
/* 0A065C 8009FA5C 00000000 */  nop   
/* 0A0660 8009FA60 C486008C */  lwc1  $f6, 0x8c($a0)
/* 0A0664 8009FA64 00000000 */  nop   
/* 0A0668 8009FA68 461C3200 */  add.s $f8, $f6, $f28
/* 0A066C 8009FA6C E488008C */  swc1  $f8, 0x8c($a0)
/* 0A0670 8009FA70 8CA40000 */  lw    $a0, ($a1)
/* 0A0674 8009FA74 00000000 */  nop   
/* 0A0678 8009FA78 C48A00AC */  lwc1  $f10, 0xac($a0)
/* 0A067C 8009FA7C 00000000 */  nop   
/* 0A0680 8009FA80 461C5100 */  add.s $f4, $f10, $f28
/* 0A0684 8009FA84 E48400AC */  swc1  $f4, 0xac($a0)
/* 0A0688 8009FA88 8CA40000 */  lw    $a0, ($a1)
/* 0A068C 8009FA8C 00000000 */  nop   
/* 0A0690 8009FA90 C48600CC */  lwc1  $f6, 0xcc($a0)
/* 0A0694 8009FA94 00000000 */  nop   
/* 0A0698 8009FA98 461C3200 */  add.s $f8, $f6, $f28
/* 0A069C 8009FA9C E48800CC */  swc1  $f8, 0xcc($a0)
/* 0A06A0 8009FAA0 8CA40000 */  lw    $a0, ($a1)
/* 0A06A4 8009FAA4 00000000 */  nop   
/* 0A06A8 8009FAA8 C48A006C */  lwc1  $f10, 0x6c($a0)
/* 0A06AC 8009FAAC 00000000 */  nop   
/* 0A06B0 8009FAB0 461C5100 */  add.s $f4, $f10, $f28
/* 0A06B4 8009FAB4 E484006C */  swc1  $f4, 0x6c($a0)
/* 0A06B8 8009FAB8 8CA40000 */  lw    $a0, ($a1)
/* 0A06BC 8009FABC 00000000 */  nop   
/* 0A06C0 8009FAC0 C486020C */  lwc1  $f6, 0x20c($a0)
/* 0A06C4 8009FAC4 00000000 */  nop   
/* 0A06C8 8009FAC8 461C3200 */  add.s $f8, $f6, $f28
/* 0A06CC 8009FACC E488020C */  swc1  $f8, 0x20c($a0)
.L8009FAD0:
/* 0A06D0 8009FAD0 0C01AF66 */  jal   get_current_level_race_type
/* 0A06D4 8009FAD4 AFA70090 */   sw    $a3, 0x90($sp)
/* 0A06D8 8009FAD8 3C014080 */  li    $at, 0x40800000 # 4.000000
/* 0A06DC 8009FADC 44819000 */  mtc1  $at, $f18
/* 0A06E0 8009FAE0 24010008 */  li    $at, 8
/* 0A06E4 8009FAE4 3C058012 */  lui   $a1, %hi(gCurrentHud) # $a1, 0x8012
/* 0A06E8 8009FAE8 8FA70090 */  lw    $a3, 0x90($sp)
/* 0A06EC 8009FAEC 24A56CDC */  addiu $a1, %lo(gCurrentHud) # addiu $a1, $a1, 0x6cdc
/* 0A06F0 8009FAF0 240B0002 */  li    $t3, 2
/* 0A06F4 8009FAF4 10410006 */  beq   $v0, $at, .L8009FB10
/* 0A06F8 8009FAF8 240C0001 */   li    $t4, 1
/* 0A06FC 8009FAFC 24010041 */  li    $at, 65
/* 0A0700 8009FB00 10410046 */  beq   $v0, $at, .L8009FC1C
/* 0A0704 8009FB04 3C0F8000 */   lui   $t7, %hi(osTvType) # $t7, 0x8000
/* 0A0708 8009FB08 10000085 */  b     .L8009FD20
/* 0A070C 8009FB0C 00000000 */   nop   
.L8009FB10:
/* 0A0710 8009FB10 8CA40000 */  lw    $a0, ($a1)
/* 0A0714 8009FB14 00000000 */  nop   
/* 0A0718 8009FB18 C48A00EC */  lwc1  $f10, 0xec($a0)
/* 0A071C 8009FB1C 00000000 */  nop   
/* 0A0720 8009FB20 461A5100 */  add.s $f4, $f10, $f26
/* 0A0724 8009FB24 E48400EC */  swc1  $f4, 0xec($a0)
/* 0A0728 8009FB28 8CA40000 */  lw    $a0, ($a1)
/* 0A072C 8009FB2C 00000000 */  nop   
/* 0A0730 8009FB30 C486010C */  lwc1  $f6, 0x10c($a0)
/* 0A0734 8009FB34 00000000 */  nop   
/* 0A0738 8009FB38 461A3200 */  add.s $f8, $f6, $f26
/* 0A073C 8009FB3C E488010C */  swc1  $f8, 0x10c($a0)
/* 0A0740 8009FB40 8CA40000 */  lw    $a0, ($a1)
/* 0A0744 8009FB44 00000000 */  nop   
/* 0A0748 8009FB48 C48A012C */  lwc1  $f10, 0x12c($a0)
/* 0A074C 8009FB4C 00000000 */  nop   
/* 0A0750 8009FB50 461A5100 */  add.s $f4, $f10, $f26
/* 0A0754 8009FB54 E484012C */  swc1  $f4, 0x12c($a0)
/* 0A0758 8009FB58 8CA40000 */  lw    $a0, ($a1)
/* 0A075C 8009FB5C 00000000 */  nop   
/* 0A0760 8009FB60 C486038C */  lwc1  $f6, 0x38c($a0)
/* 0A0764 8009FB64 00000000 */  nop   
/* 0A0768 8009FB68 461A3200 */  add.s $f8, $f6, $f26
/* 0A076C 8009FB6C E488038C */  swc1  $f8, 0x38c($a0)
/* 0A0770 8009FB70 8CA40000 */  lw    $a0, ($a1)
/* 0A0774 8009FB74 00000000 */  nop   
/* 0A0778 8009FB78 C48A036C */  lwc1  $f10, 0x36c($a0)
/* 0A077C 8009FB7C 00000000 */  nop   
/* 0A0780 8009FB80 461A5100 */  add.s $f4, $f10, $f26
/* 0A0784 8009FB84 E484036C */  swc1  $f4, 0x36c($a0)
/* 0A0788 8009FB88 8CA40000 */  lw    $a0, ($a1)
/* 0A078C 8009FB8C 00000000 */  nop   
/* 0A0790 8009FB90 C486024C */  lwc1  $f6, 0x24c($a0)
/* 0A0794 8009FB94 00000000 */  nop   
/* 0A0798 8009FB98 461A3200 */  add.s $f8, $f6, $f26
/* 0A079C 8009FB9C E488024C */  swc1  $f8, 0x24c($a0)
/* 0A07A0 8009FBA0 8CA40000 */  lw    $a0, ($a1)
/* 0A07A4 8009FBA4 00000000 */  nop   
/* 0A07A8 8009FBA8 C48A006C */  lwc1  $f10, 0x6c($a0)
/* 0A07AC 8009FBAC 00000000 */  nop   
/* 0A07B0 8009FBB0 46145100 */  add.s $f4, $f10, $f20
/* 0A07B4 8009FBB4 E484006C */  swc1  $f4, 0x6c($a0)
/* 0A07B8 8009FBB8 8CA40000 */  lw    $a0, ($a1)
/* 0A07BC 8009FBBC 00000000 */  nop   
/* 0A07C0 8009FBC0 C486008C */  lwc1  $f6, 0x8c($a0)
/* 0A07C4 8009FBC4 00000000 */  nop   
/* 0A07C8 8009FBC8 46143200 */  add.s $f8, $f6, $f20
/* 0A07CC 8009FBCC E488008C */  swc1  $f8, 0x8c($a0)
/* 0A07D0 8009FBD0 8CA40000 */  lw    $a0, ($a1)
/* 0A07D4 8009FBD4 00000000 */  nop   
/* 0A07D8 8009FBD8 C48A00AC */  lwc1  $f10, 0xac($a0)
/* 0A07DC 8009FBDC 00000000 */  nop   
/* 0A07E0 8009FBE0 46145100 */  add.s $f4, $f10, $f20
/* 0A07E4 8009FBE4 E48400AC */  swc1  $f4, 0xac($a0)
/* 0A07E8 8009FBE8 8CA40000 */  lw    $a0, ($a1)
/* 0A07EC 8009FBEC 00000000 */  nop   
/* 0A07F0 8009FBF0 C48600CC */  lwc1  $f6, 0xcc($a0)
/* 0A07F4 8009FBF4 00000000 */  nop   
/* 0A07F8 8009FBF8 46143200 */  add.s $f8, $f6, $f20
/* 0A07FC 8009FBFC E48800CC */  swc1  $f8, 0xcc($a0)
/* 0A0800 8009FC00 8CA40000 */  lw    $a0, ($a1)
/* 0A0804 8009FC04 00000000 */  nop   
/* 0A0808 8009FC08 C48A020C */  lwc1  $f10, 0x20c($a0)
/* 0A080C 8009FC0C 00000000 */  nop   
/* 0A0810 8009FC10 46145100 */  add.s $f4, $f10, $f20
/* 0A0814 8009FC14 10000042 */  b     .L8009FD20
/* 0A0818 8009FC18 E484020C */   swc1  $f4, 0x20c($a0)
.L8009FC1C:
/* 0A081C 8009FC1C 8DEF0300 */  lw    $t7, %lo(osTvType)($t7)
/* 0A0820 8009FC20 3C028012 */  lui   $v0, %hi(gNumActivePlayers) # $v0, 0x8012
/* 0A0824 8009FC24 15E00007 */  bnez  $t7, .L8009FC44
/* 0A0828 8009FC28 00000000 */   nop   
/* 0A082C 8009FC2C 8CA40000 */  lw    $a0, ($a1)
/* 0A0830 8009FC30 00000000 */  nop   
/* 0A0834 8009FC34 C4860410 */  lwc1  $f6, 0x410($a0)
/* 0A0838 8009FC38 00000000 */  nop   
/* 0A083C 8009FC3C 46123201 */  sub.s $f8, $f6, $f18
/* 0A0840 8009FC40 E4880410 */  swc1  $f8, 0x410($a0)
.L8009FC44:
/* 0A0844 8009FC44 90426D37 */  lbu   $v0, %lo(gNumActivePlayers)($v0)
/* 0A0848 8009FC48 00000000 */  nop   
/* 0A084C 8009FC4C 28410003 */  slti  $at, $v0, 3
/* 0A0850 8009FC50 14200009 */  bnez  $at, .L8009FC78
/* 0A0854 8009FC54 00000000 */   nop   
/* 0A0858 8009FC58 10E00003 */  beqz  $a3, .L8009FC68
/* 0A085C 8009FC5C 00000000 */   nop   
/* 0A0860 8009FC60 14EB0003 */  bne   $a3, $t3, .L8009FC70
/* 0A0864 8009FC64 00000000 */   nop   
.L8009FC68:
/* 0A0868 8009FC68 10000008 */  b     .L8009FC8C
/* 0A086C 8009FC6C 02801025 */   move  $v0, $s4
.L8009FC70:
/* 0A0870 8009FC70 10000006 */  b     .L8009FC8C
/* 0A0874 8009FC74 2402FFF6 */   li    $v0, -10
.L8009FC78:
/* 0A0878 8009FC78 15820004 */  bne   $t4, $v0, .L8009FC8C
/* 0A087C 8009FC7C 00001025 */   move  $v0, $zero
/* 0A0880 8009FC80 10000002 */  b     .L8009FC8C
/* 0A0884 8009FC84 2402FFF6 */   li    $v0, -10
/* 0A0888 8009FC88 00001025 */  move  $v0, $zero
.L8009FC8C:
/* 0A088C 8009FC8C 44825000 */  mtc1  $v0, $f10
/* 0A0890 8009FC90 8CA40000 */  lw    $a0, ($a1)
/* 0A0894 8009FC94 46805020 */  cvt.s.w $f0, $f10
/* 0A0898 8009FC98 C484036C */  lwc1  $f4, 0x36c($a0)
/* 0A089C 8009FC9C 00000000 */  nop   
/* 0A08A0 8009FCA0 46002180 */  add.s $f6, $f4, $f0
/* 0A08A4 8009FCA4 E486036C */  swc1  $f6, 0x36c($a0)
/* 0A08A8 8009FCA8 8CA40000 */  lw    $a0, ($a1)
/* 0A08AC 8009FCAC 00000000 */  nop   
/* 0A08B0 8009FCB0 C48800EC */  lwc1  $f8, 0xec($a0)
/* 0A08B4 8009FCB4 00000000 */  nop   
/* 0A08B8 8009FCB8 46004280 */  add.s $f10, $f8, $f0
/* 0A08BC 8009FCBC E48A00EC */  swc1  $f10, 0xec($a0)
/* 0A08C0 8009FCC0 8CA40000 */  lw    $a0, ($a1)
/* 0A08C4 8009FCC4 00000000 */  nop   
/* 0A08C8 8009FCC8 C484010C */  lwc1  $f4, 0x10c($a0)
/* 0A08CC 8009FCCC 00000000 */  nop   
/* 0A08D0 8009FCD0 46002180 */  add.s $f6, $f4, $f0
/* 0A08D4 8009FCD4 E486010C */  swc1  $f6, 0x10c($a0)
/* 0A08D8 8009FCD8 8CA40000 */  lw    $a0, ($a1)
/* 0A08DC 8009FCDC 00000000 */  nop   
/* 0A08E0 8009FCE0 C488012C */  lwc1  $f8, 0x12c($a0)
/* 0A08E4 8009FCE4 00000000 */  nop   
/* 0A08E8 8009FCE8 46004280 */  add.s $f10, $f8, $f0
/* 0A08EC 8009FCEC E48A012C */  swc1  $f10, 0x12c($a0)
/* 0A08F0 8009FCF0 8CA40000 */  lw    $a0, ($a1)
/* 0A08F4 8009FCF4 00000000 */  nop   
/* 0A08F8 8009FCF8 C484038C */  lwc1  $f4, 0x38c($a0)
/* 0A08FC 8009FCFC 00000000 */  nop   
/* 0A0900 8009FD00 46002180 */  add.s $f6, $f4, $f0
/* 0A0904 8009FD04 E486038C */  swc1  $f6, 0x38c($a0)
/* 0A0908 8009FD08 8CA40000 */  lw    $a0, ($a1)
/* 0A090C 8009FD0C 00000000 */  nop   
/* 0A0910 8009FD10 C488024C */  lwc1  $f8, 0x24c($a0)
/* 0A0914 8009FD14 00000000 */  nop   
/* 0A0918 8009FD18 46004280 */  add.s $f10, $f8, $f0
/* 0A091C 8009FD1C E48A024C */  swc1  $f10, 0x24c($a0)
.L8009FD20:
/* 0A0920 8009FD20 0C008D07 */  jal   is_taj_challenge
/* 0A0924 8009FD24 AFA70090 */   sw    $a3, 0x90($sp)
/* 0A0928 8009FD28 3C014120 */  li    $at, 0x41200000 # 10.000000
/* 0A092C 8009FD2C 44811000 */  mtc1  $at, $f2
/* 0A0930 8009FD30 3C01800F */  lui   $at, %hi(D_800E86E8 + 4) # $at, 0x800f
/* 0A0934 8009FD34 C42D86E8 */  lwc1  $f13, %lo(D_800E86E8)($at)
/* 0A0938 8009FD38 C42C86EC */  lwc1  $f12, %lo(D_800E86E8 + 4)($at)
/* 0A093C 8009FD3C 3C01800F */  lui   $at, %hi(D_800E86F0 + 4) # $at, 0x800f
/* 0A0940 8009FD40 C42F86F0 */  lwc1  $f15, %lo(D_800E86F0)($at)
/* 0A0944 8009FD44 C42E86F4 */  lwc1  $f14, %lo(D_800E86F0 + 4)($at)
/* 0A0948 8009FD48 3C013FE8 */  li    $at, 0x3FE80000 # 1.812500
/* 0A094C 8009FD4C 44818800 */  mtc1  $at, $f17
/* 0A0950 8009FD50 3C014080 */  li    $at, 0x40800000 # 4.000000
/* 0A0954 8009FD54 44819000 */  mtc1  $at, $f18
/* 0A0958 8009FD58 3C058012 */  lui   $a1, %hi(gCurrentHud) # $a1, 0x8012
/* 0A095C 8009FD5C 8FA70090 */  lw    $a3, 0x90($sp)
/* 0A0960 8009FD60 3C098012 */  lui   $t1, %hi(gAssetHudElementIds) # $t1, 0x8012
/* 0A0964 8009FD64 44808000 */  mtc1  $zero, $f16
/* 0A0968 8009FD68 25296CF0 */  addiu $t1, %lo(gAssetHudElementIds) # addiu $t1, $t1, 0x6cf0
/* 0A096C 8009FD6C 24A56CDC */  addiu $a1, %lo(gCurrentHud) # addiu $a1, $a1, 0x6cdc
/* 0A0970 8009FD70 3408C000 */  li    $t0, 49152
/* 0A0974 8009FD74 240AFFFF */  li    $t2, -1
/* 0A0978 8009FD78 240B0002 */  li    $t3, 2
/* 0A097C 8009FD7C 240C0001 */  li    $t4, 1
/* 0A0980 8009FD80 240D0009 */  li    $t5, 9
/* 0A0984 8009FD84 1040001F */  beqz  $v0, .L8009FE04
/* 0A0988 8009FD88 241F0012 */   li    $ra, 18
/* 0A098C 8009FD8C 8CA40000 */  lw    $a0, ($a1)
/* 0A0990 8009FD90 00000000 */  nop   
/* 0A0994 8009FD94 C484006C */  lwc1  $f4, 0x6c($a0)
/* 0A0998 8009FD98 00000000 */  nop   
/* 0A099C 8009FD9C 46142180 */  add.s $f6, $f4, $f20
/* 0A09A0 8009FDA0 E486006C */  swc1  $f6, 0x6c($a0)
/* 0A09A4 8009FDA4 8CA40000 */  lw    $a0, ($a1)
/* 0A09A8 8009FDA8 00000000 */  nop   
/* 0A09AC 8009FDAC C488008C */  lwc1  $f8, 0x8c($a0)
/* 0A09B0 8009FDB0 00000000 */  nop   
/* 0A09B4 8009FDB4 46144280 */  add.s $f10, $f8, $f20
/* 0A09B8 8009FDB8 E48A008C */  swc1  $f10, 0x8c($a0)
/* 0A09BC 8009FDBC 8CA40000 */  lw    $a0, ($a1)
/* 0A09C0 8009FDC0 00000000 */  nop   
/* 0A09C4 8009FDC4 C48400AC */  lwc1  $f4, 0xac($a0)
/* 0A09C8 8009FDC8 00000000 */  nop   
/* 0A09CC 8009FDCC 46142180 */  add.s $f6, $f4, $f20
/* 0A09D0 8009FDD0 E48600AC */  swc1  $f6, 0xac($a0)
/* 0A09D4 8009FDD4 8CA40000 */  lw    $a0, ($a1)
/* 0A09D8 8009FDD8 00000000 */  nop   
/* 0A09DC 8009FDDC C48800CC */  lwc1  $f8, 0xcc($a0)
/* 0A09E0 8009FDE0 00000000 */  nop   
/* 0A09E4 8009FDE4 46144280 */  add.s $f10, $f8, $f20
/* 0A09E8 8009FDE8 E48A00CC */  swc1  $f10, 0xcc($a0)
/* 0A09EC 8009FDEC 8CA40000 */  lw    $a0, ($a1)
/* 0A09F0 8009FDF0 00000000 */  nop   
/* 0A09F4 8009FDF4 C484020C */  lwc1  $f4, 0x20c($a0)
/* 0A09F8 8009FDF8 00000000 */  nop   
/* 0A09FC 8009FDFC 46142180 */  add.s $f6, $f4, $f20
/* 0A0A00 8009FE00 E486020C */  swc1  $f6, 0x20c($a0)
.L8009FE04:
/* 0A0A04 8009FE04 3C188000 */  lui   $t8, %hi(osTvType) # $t8, 0x8000
/* 0A0A08 8009FE08 8F180300 */  lw    $t8, %lo(osTvType)($t8)
/* 0A0A0C 8009FE0C 00000000 */  nop   
/* 0A0A10 8009FE10 17000053 */  bnez  $t8, .L8009FF60
/* 0A0A14 8009FE14 00000000 */   nop   
/* 0A0A18 8009FE18 8CA40000 */  lw    $a0, ($a1)
/* 0A0A1C 8009FE1C 00001825 */  move  $v1, $zero
/* 0A0A20 8009FE20 00831021 */  addu  $v0, $a0, $v1
.L8009FE24:
/* 0A0A24 8009FE24 C4480010 */  lwc1  $f8, 0x10($v0)
/* 0A0A28 8009FE28 00000000 */  nop   
/* 0A0A2C 8009FE2C 460042A1 */  cvt.d.s $f10, $f8
/* 0A0A30 8009FE30 462C5102 */  mul.d $f4, $f10, $f12
/* 0A0A34 8009FE34 462021A0 */  cvt.s.d $f6, $f4
/* 0A0A38 8009FE38 E4460010 */  swc1  $f6, 0x10($v0)
/* 0A0A3C 8009FE3C 8CAE0000 */  lw    $t6, ($a1)
/* 0A0A40 8009FE40 00000000 */  nop   
/* 0A0A44 8009FE44 01C31021 */  addu  $v0, $t6, $v1
/* 0A0A48 8009FE48 C448000C */  lwc1  $f8, 0xc($v0)
/* 0A0A4C 8009FE4C 00000000 */  nop   
/* 0A0A50 8009FE50 46124281 */  sub.s $f10, $f8, $f18
/* 0A0A54 8009FE54 E44A000C */  swc1  $f10, 0xc($v0)
/* 0A0A58 8009FE58 8CA40000 */  lw    $a0, ($a1)
/* 0A0A5C 8009FE5C 8D390000 */  lw    $t9, ($t1)
/* 0A0A60 8009FE60 00831021 */  addu  $v0, $a0, $v1
/* 0A0A64 8009FE64 844F0006 */  lh    $t7, 6($v0)
/* 0A0A68 8009FE68 24630020 */  addiu $v1, $v1, 0x20
/* 0A0A6C 8009FE6C 000FC040 */  sll   $t8, $t7, 1
/* 0A0A70 8009FE70 03387021 */  addu  $t6, $t9, $t8
/* 0A0A74 8009FE74 85CF0000 */  lh    $t7, ($t6)
/* 0A0A78 8009FE78 28610760 */  slti  $at, $v1, 0x760
/* 0A0A7C 8009FE7C 31F9C000 */  andi  $t9, $t7, 0xc000
/* 0A0A80 8009FE80 15190007 */  bne   $t0, $t9, .L8009FEA0
/* 0A0A84 8009FE84 00000000 */   nop   
/* 0A0A88 8009FE88 C4440010 */  lwc1  $f4, 0x10($v0)
/* 0A0A8C 8009FE8C 00000000 */  nop   
/* 0A0A90 8009FE90 46022181 */  sub.s $f6, $f4, $f2
/* 0A0A94 8009FE94 E4460010 */  swc1  $f6, 0x10($v0)
/* 0A0A98 8009FE98 8CA40000 */  lw    $a0, ($a1)
/* 0A0A9C 8009FE9C 00000000 */  nop   
.L8009FEA0:
/* 0A0AA0 8009FEA0 1420FFE0 */  bnez  $at, .L8009FE24
/* 0A0AA4 8009FEA4 00831021 */   addu  $v0, $a0, $v1
/* 0A0AA8 8009FEA8 248204E0 */  addiu $v0, $a0, 0x4e0
/* 0A0AAC 8009FEAC C4480010 */  lwc1  $f8, 0x10($v0)
/* 0A0AB0 8009FEB0 2406002A */  li    $a2, 42
/* 0A0AB4 8009FEB4 46024280 */  add.s $f10, $f8, $f2
/* 0A0AB8 8009FEB8 00061940 */  sll   $v1, $a2, 5
/* 0A0ABC 8009FEBC E44A0010 */  swc1  $f10, 0x10($v0)
/* 0A0AC0 8009FEC0 8CA20000 */  lw    $v0, ($a1)
/* 0A0AC4 8009FEC4 00000000 */  nop   
/* 0A0AC8 8009FEC8 C4440510 */  lwc1  $f4, 0x510($v0)
/* 0A0ACC 8009FECC 244204E0 */  addiu $v0, $v0, 0x4e0
/* 0A0AD0 8009FED0 46022180 */  add.s $f6, $f4, $f2
/* 0A0AD4 8009FED4 E4460030 */  swc1  $f6, 0x30($v0)
/* 0A0AD8 8009FED8 8CA20000 */  lw    $v0, ($a1)
/* 0A0ADC 8009FEDC 00000000 */  nop   
/* 0A0AE0 8009FEE0 C4480530 */  lwc1  $f8, 0x530($v0)
/* 0A0AE4 8009FEE4 244204E0 */  addiu $v0, $v0, 0x4e0
/* 0A0AE8 8009FEE8 46024280 */  add.s $f10, $f8, $f2
/* 0A0AEC 8009FEEC E44A0050 */  swc1  $f10, 0x50($v0)
/* 0A0AF0 8009FEF0 8CB80000 */  lw    $t8, ($a1)
/* 0A0AF4 8009FEF4 00000000 */  nop   
/* 0A0AF8 8009FEF8 03031021 */  addu  $v0, $t8, $v1
/* 0A0AFC 8009FEFC C4440010 */  lwc1  $f4, 0x10($v0)
/* 0A0B00 8009FF00 00000000 */  nop   
/* 0A0B04 8009FF04 46022180 */  add.s $f6, $f4, $f2
/* 0A0B08 8009FF08 E4460010 */  swc1  $f6, 0x10($v0)
/* 0A0B0C 8009FF0C 8CAE0000 */  lw    $t6, ($a1)
/* 0A0B10 8009FF10 00000000 */  nop   
/* 0A0B14 8009FF14 01C31021 */  addu  $v0, $t6, $v1
/* 0A0B18 8009FF18 C4480030 */  lwc1  $f8, 0x30($v0)
/* 0A0B1C 8009FF1C 00000000 */  nop   
/* 0A0B20 8009FF20 46024280 */  add.s $f10, $f8, $f2
/* 0A0B24 8009FF24 E44A0030 */  swc1  $f10, 0x30($v0)
/* 0A0B28 8009FF28 8CAF0000 */  lw    $t7, ($a1)
/* 0A0B2C 8009FF2C 00000000 */  nop   
/* 0A0B30 8009FF30 01E31021 */  addu  $v0, $t7, $v1
/* 0A0B34 8009FF34 C4440050 */  lwc1  $f4, 0x50($v0)
/* 0A0B38 8009FF38 00000000 */  nop   
/* 0A0B3C 8009FF3C 46022180 */  add.s $f6, $f4, $f2
/* 0A0B40 8009FF40 E4460050 */  swc1  $f6, 0x50($v0)
/* 0A0B44 8009FF44 8CB90000 */  lw    $t9, ($a1)
/* 0A0B48 8009FF48 00000000 */  nop   
/* 0A0B4C 8009FF4C 03231021 */  addu  $v0, $t9, $v1
/* 0A0B50 8009FF50 C4480070 */  lwc1  $f8, 0x70($v0)
/* 0A0B54 8009FF54 00000000 */  nop   
/* 0A0B58 8009FF58 46024280 */  add.s $f10, $f8, $f2
/* 0A0B5C 8009FF5C E44A0070 */  swc1  $f10, 0x70($v0)
.L8009FF60:
/* 0A0B60 8009FF60 8CB80000 */  lw    $t8, ($a1)
/* 0A0B64 8009FF64 24E70001 */  addiu $a3, $a3, 1
/* 0A0B68 8009FF68 E7160268 */  swc1  $f22, 0x268($t8)
/* 0A0B6C 8009FF6C 8CAE0000 */  lw    $t6, ($a1)
/* 0A0B70 8009FF70 00000000 */  nop   
/* 0A0B74 8009FF74 A5DF0286 */  sh    $ra, 0x286($t6)
/* 0A0B78 8009FF78 8CAF0000 */  lw    $t7, ($a1)
/* 0A0B7C 8009FF7C 00000000 */  nop   
/* 0A0B80 8009FF80 A5ED02A6 */  sh    $t5, 0x2a6($t7)
/* 0A0B84 8009FF84 8CB90000 */  lw    $t9, ($a1)
/* 0A0B88 8009FF88 00000000 */  nop   
/* 0A0B8C 8009FF8C A72D02C6 */  sh    $t5, 0x2c6($t9)
/* 0A0B90 8009FF90 8FB8006C */  lw    $t8, 0x6c($sp)
/* 0A0B94 8009FF94 8FAF008C */  lw    $t7, 0x8c($sp)
/* 0A0B98 8009FF98 270E0004 */  addiu $t6, $t8, 4
/* 0A0B9C 8009FF9C 14EFFCCE */  bne   $a3, $t7, .L8009F2D8
/* 0A0BA0 8009FFA0 AFAE006C */   sw    $t6, 0x6c($sp)
.L8009FFA4:
/* 0A0BA4 8009FFA4 0C01AF66 */  jal   get_current_level_race_type
/* 0A0BA8 8009FFA8 00000000 */   nop   
/* 0A0BAC 8009FFAC 30590040 */  andi  $t9, $v0, 0x40
/* 0A0BB0 8009FFB0 13200008 */  beqz  $t9, .L8009FFD4
/* 0A0BB4 8009FFB4 3C02800E */   lui   $v0, %hi(gHudToggleSettings) # $v0, 0x800e
/* 0A0BB8 8009FFB8 3C02800E */  lui   $v0, %hi(gHudToggleSettings) # $v0, 0x800e
/* 0A0BBC 8009FFBC 244227A4 */  addiu $v0, %lo(gHudToggleSettings) # addiu $v0, $v0, 0x27a4
/* 0A0BC0 8009FFC0 80580001 */  lb    $t8, 1($v0)
/* 0A0BC4 8009FFC4 3C01800E */  lui   $at, %hi(gPrevToggleSetting) # $at, 0x800e
/* 0A0BC8 8009FFC8 A03827A8 */  sb    $t8, %lo(gPrevToggleSetting)($at)
/* 0A0BCC 8009FFCC 10000005 */  b     .L8009FFE4
/* 0A0BD0 8009FFD0 A0400001 */   sb    $zero, 1($v0)
.L8009FFD4:
/* 0A0BD4 8009FFD4 3C0E800E */  lui   $t6, %hi(gPrevToggleSetting) # $t6, 0x800e
/* 0A0BD8 8009FFD8 81CE27A8 */  lb    $t6, %lo(gPrevToggleSetting)($t6)
/* 0A0BDC 8009FFDC 244227A4 */  addiu $v0, %lo(gHudToggleSettings) # addiu $v0, $v0, 0x27a4
/* 0A0BE0 8009FFE0 A04E0001 */  sb    $t6, 1($v0)
.L8009FFE4:
/* 0A0BE4 8009FFE4 8FBF0064 */  lw    $ra, 0x64($sp)
/* 0A0BE8 8009FFE8 C7B50018 */  lwc1  $f21, 0x18($sp)
/* 0A0BEC 8009FFEC C7B4001C */  lwc1  $f20, 0x1c($sp)
/* 0A0BF0 8009FFF0 C7B70020 */  lwc1  $f23, 0x20($sp)
/* 0A0BF4 8009FFF4 C7B60024 */  lwc1  $f22, 0x24($sp)
/* 0A0BF8 8009FFF8 C7B90028 */  lwc1  $f25, 0x28($sp)
/* 0A0BFC 8009FFFC C7B8002C */  lwc1  $f24, 0x2c($sp)
/* 0A0C00 800A0000 C7BB0030 */  lwc1  $f27, 0x30($sp)
/* 0A0C04 800A0004 C7BA0034 */  lwc1  $f26, 0x34($sp)
/* 0A0C08 800A0008 C7BD0038 */  lwc1  $f29, 0x38($sp)
/* 0A0C0C 800A000C C7BC003C */  lwc1  $f28, 0x3c($sp)
/* 0A0C10 800A0010 8FB00040 */  lw    $s0, 0x40($sp)
/* 0A0C14 800A0014 8FB10044 */  lw    $s1, 0x44($sp)
/* 0A0C18 800A0018 8FB20048 */  lw    $s2, 0x48($sp)
/* 0A0C1C 800A001C 8FB3004C */  lw    $s3, 0x4c($sp)
/* 0A0C20 800A0020 8FB40050 */  lw    $s4, 0x50($sp)
/* 0A0C24 800A0024 8FB50054 */  lw    $s5, 0x54($sp)
/* 0A0C28 800A0028 8FB60058 */  lw    $s6, 0x58($sp)
/* 0A0C2C 800A002C 8FB7005C */  lw    $s7, 0x5c($sp)
/* 0A0C30 800A0030 8FBE0060 */  lw    $fp, 0x60($sp)
/* 0A0C34 800A0034 03E00008 */  jr    $ra
/* 0A0C38 800A0038 27BD0098 */   addiu $sp, $sp, 0x98

