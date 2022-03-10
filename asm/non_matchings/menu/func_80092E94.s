glabel func_80092E94
/* 093A94 80092E94 27BDFF88 */  addiu $sp, $sp, -0x78
/* 093A98 80092E98 AFBF0034 */  sw    $ra, 0x34($sp)
/* 093A9C 80092E9C AFB1002C */  sw    $s1, 0x2c($sp)
/* 093AA0 80092EA0 00C08825 */  move  $s1, $a2
/* 093AA4 80092EA4 AFB20030 */  sw    $s2, 0x30($sp)
/* 093AA8 80092EA8 AFB00028 */  sw    $s0, 0x28($sp)
/* 093AAC 80092EAC AFA40078 */  sw    $a0, 0x78($sp)
/* 093AB0 80092EB0 AFA5007C */  sw    $a1, 0x7c($sp)
/* 093AB4 80092EB4 0C01BAA4 */  jal   get_settings
/* 093AB8 80092EB8 AFA00054 */   sw    $zero, 0x54($sp)
/* 093ABC 80092EBC 3C0E8000 */  lui   $t6, %hi(osTvType) # $t6, 0x8000
/* 093AC0 80092EC0 8DCE0300 */  lw    $t6, %lo(osTvType)($t6)
/* 093AC4 80092EC4 00402025 */  move  $a0, $v0
/* 093AC8 80092EC8 15C00002 */  bnez  $t6, .L80092ED4
/* 093ACC 80092ECC 00002825 */   move  $a1, $zero
/* 093AD0 80092ED0 2405000C */  li    $a1, 12
.L80092ED4:
/* 093AD4 80092ED4 3C18800E */  lui   $t8, %hi(gTrackIdForPreview) # $t8, 0x800e
/* 093AD8 80092ED8 8F18F4C4 */  lw    $t8, %lo(gTrackIdForPreview)($t8)
/* 093ADC 80092EDC 8C8F004C */  lw    $t7, 0x4c($a0)
/* 093AE0 80092EE0 3C128012 */  lui   $s2, %hi(sMenuCurrDisplayList) # $s2, 0x8012
/* 093AE4 80092EE4 265263A0 */  addiu $s2, %lo(sMenuCurrDisplayList) # addiu $s2, $s2, 0x63a0
/* 093AE8 80092EE8 01F8C821 */  addu  $t9, $t7, $t8
/* 093AEC 80092EEC 83280002 */  lb    $t0, 2($t9)
/* 093AF0 80092EF0 8E430000 */  lw    $v1, ($s2)
/* 093AF4 80092EF4 AFA80058 */  sw    $t0, 0x58($sp)
/* 093AF8 80092EF8 24690008 */  addiu $t1, $v1, 8
/* 093AFC 80092EFC AE490000 */  sw    $t1, ($s2)
/* 093B00 80092F00 3C0AB600 */  lui   $t2, 0xb600
/* 093B04 80092F04 240B1000 */  li    $t3, 4096
/* 093B08 80092F08 AC6B0004 */  sw    $t3, 4($v1)
/* 093B0C 80092F0C AC6A0000 */  sw    $t2, ($v1)
/* 093B10 80092F10 AFA50060 */  sw    $a1, 0x60($sp)
/* 093B14 80092F14 0C026F57 */  jal   func_8009BD5C
/* 093B18 80092F18 AFA40050 */   sw    $a0, 0x50($sp)
/* 093B1C 80092F1C 3C058012 */  lui   $a1, %hi(sMenuCurrHudMat) # $a1, 0x8012
/* 093B20 80092F20 24A563A8 */  addiu $a1, %lo(sMenuCurrHudMat) # addiu $a1, $a1, 0x63a8
/* 093B24 80092F24 0C019FCB */  jal   func_80067F2C
/* 093B28 80092F28 02402025 */   move  $a0, $s2
/* 093B2C 80092F2C 3C02800E */  lui   $v0, %hi(gMenuDelay) # $v0, 0x800e
/* 093B30 80092F30 8C42F47C */  lw    $v0, %lo(gMenuDelay)($v0)
/* 093B34 80092F34 00000000 */  nop   
/* 093B38 80092F38 2841FFEC */  slti  $at, $v0, -0x14
/* 093B3C 80092F3C 142001AA */  bnez  $at, .L800935E8
/* 093B40 80092F40 28410015 */   slti  $at, $v0, 0x15
/* 093B44 80092F44 102001A9 */  beqz  $at, .L800935EC
/* 093B48 80092F48 8FBF0034 */   lw    $ra, 0x34($sp)
/* 093B4C 80092F4C 8FA40058 */  lw    $a0, 0x58($sp)
/* 093B50 80092F50 0C01AC3E */  jal   func_8006B0F8
/* 093B54 80092F54 00000000 */   nop   
/* 093B58 80092F58 8FA40058 */  lw    $a0, 0x58($sp)
/* 093B5C 80092F5C 0C01AF77 */  jal   get_level_name
/* 093B60 80092F60 AFA2005C */   sw    $v0, 0x5c($sp)
/* 093B64 80092F64 00408025 */  move  $s0, $v0
/* 093B68 80092F68 0C0310BB */  jal   set_text_font
/* 093B6C 80092F6C 24040002 */   li    $a0, 2
/* 093B70 80092F70 00002025 */  move  $a0, $zero
/* 093B74 80092F74 00002825 */  move  $a1, $zero
/* 093B78 80092F78 00003025 */  move  $a2, $zero
/* 093B7C 80092F7C 0C0310F3 */  jal   set_text_background_colour
/* 093B80 80092F80 00003825 */   move  $a3, $zero
/* 093B84 80092F84 240C0080 */  li    $t4, 128
/* 093B88 80092F88 AFAC0010 */  sw    $t4, 0x10($sp)
/* 093B8C 80092F8C 00002025 */  move  $a0, $zero
/* 093B90 80092F90 00002825 */  move  $a1, $zero
/* 093B94 80092F94 00003025 */  move  $a2, $zero
/* 093B98 80092F98 0C0310E1 */  jal   set_text_colour
/* 093B9C 80092F9C 240700FF */   li    $a3, 255
/* 093BA0 80092FA0 240D000C */  li    $t5, 12
/* 093BA4 80092FA4 AFAD0010 */  sw    $t5, 0x10($sp)
/* 093BA8 80092FA8 02402025 */  move  $a0, $s2
/* 093BAC 80092FAC 240500A1 */  li    $a1, 161
/* 093BB0 80092FB0 2406002E */  li    $a2, 46
/* 093BB4 80092FB4 0C031110 */  jal   draw_text
/* 093BB8 80092FB8 02003825 */   move  $a3, $s0
/* 093BBC 80092FBC 240E00FF */  li    $t6, 255
/* 093BC0 80092FC0 AFAE0010 */  sw    $t6, 0x10($sp)
/* 093BC4 80092FC4 240400FF */  li    $a0, 255
/* 093BC8 80092FC8 240500FF */  li    $a1, 255
/* 093BCC 80092FCC 240600FF */  li    $a2, 255
/* 093BD0 80092FD0 0C0310E1 */  jal   set_text_colour
/* 093BD4 80092FD4 00003825 */   move  $a3, $zero
/* 093BD8 80092FD8 240F000C */  li    $t7, 12
/* 093BDC 80092FDC AFAF0010 */  sw    $t7, 0x10($sp)
/* 093BE0 80092FE0 02402025 */  move  $a0, $s2
/* 093BE4 80092FE4 240500A0 */  li    $a1, 160
/* 093BE8 80092FE8 2406002B */  li    $a2, 43
/* 093BEC 80092FEC 0C031110 */  jal   draw_text
/* 093BF0 80092FF0 02003825 */   move  $a3, $s0
/* 093BF4 80092FF4 8FA40058 */  lw    $a0, 0x58($sp)
/* 093BF8 80092FF8 0C01AC53 */  jal   func_8006B14C
/* 093BFC 80092FFC 00000000 */   nop   
/* 093C00 80093000 30580040 */  andi  $t8, $v0, 0x40
/* 093C04 80093004 17000179 */  bnez  $t8, .L800935EC
/* 093C08 80093008 8FBF0034 */   lw    $ra, 0x34($sp)
/* 093C0C 8009300C 16200141 */  bnez  $s1, .L80093514
/* 093C10 80093010 00000000 */   nop   
/* 093C14 80093014 0C003932 */  jal   is_time_trial_enabled
/* 093C18 80093018 00000000 */   nop   
/* 093C1C 8009301C 10400091 */  beqz  $v0, .L80093264
/* 093C20 80093020 00000000 */   nop   
/* 093C24 80093024 8FA40058 */  lw    $a0, 0x58($sp)
/* 093C28 80093028 0C024AF8 */  jal   func_80092BE0
/* 093C2C 8009302C 00000000 */   nop   
/* 093C30 80093030 04400010 */  bltz  $v0, .L80093074
/* 093C34 80093034 3C0A800E */   lui   $t2, %hi(sMenuGuiOpacity) # $t2, 0x800e
/* 093C38 80093038 8D4AF764 */  lw    $t2, %lo(sMenuGuiOpacity)($t2)
/* 093C3C 8009303C 8FA70060 */  lw    $a3, 0x60($sp)
/* 093C40 80093040 3C05800E */  lui   $a1, %hi(D_800E0614) # $a1, 0x800e
/* 093C44 80093044 241900FF */  li    $t9, 255
/* 093C48 80093048 240800FF */  li    $t0, 255
/* 093C4C 8009304C 240900FF */  li    $t1, 255
/* 093C50 80093050 AFA90018 */  sw    $t1, 0x18($sp)
/* 093C54 80093054 AFA80014 */  sw    $t0, 0x14($sp)
/* 093C58 80093058 AFB90010 */  sw    $t9, 0x10($sp)
/* 093C5C 8009305C 24A50614 */  addiu $a1, %lo(D_800E0614) # addiu $a1, $a1, 0x614
/* 093C60 80093060 02402025 */  move  $a0, $s2
/* 093C64 80093064 240600CC */  li    $a2, 204
/* 093C68 80093068 AFAA001C */  sw    $t2, 0x1c($sp)
/* 093C6C 8009306C 0C01E2AE */  jal   render_textured_rectangle
/* 093C70 80093070 24E7007A */   addiu $a3, $a3, 0x7a
.L80093074:
/* 093C74 80093074 0C0310BB */  jal   set_text_font
/* 093C78 80093078 00002025 */   move  $a0, $zero
/* 093C7C 8009307C 240B00FF */  li    $t3, 255
/* 093C80 80093080 AFAB0010 */  sw    $t3, 0x10($sp)
/* 093C84 80093084 240400FF */  li    $a0, 255
/* 093C88 80093088 24050040 */  li    $a1, 64
/* 093C8C 8009308C 24060040 */  li    $a2, 64
/* 093C90 80093090 0C0310E1 */  jal   set_text_colour
/* 093C94 80093094 24070060 */   li    $a3, 96
/* 093C98 80093098 3C0C800E */  lui   $t4, %hi(gMenuText) # $t4, 0x800e
/* 093C9C 8009309C 8D8CF4A0 */  lw    $t4, %lo(gMenuText)($t4)
/* 093CA0 800930A0 8FB00060 */  lw    $s0, 0x60($sp)
/* 093CA4 800930A4 240D000C */  li    $t5, 12
/* 093CA8 800930A8 8D870024 */  lw    $a3, 0x24($t4)
/* 093CAC 800930AC 26100048 */  addiu $s0, $s0, 0x48
/* 093CB0 800930B0 02003025 */  move  $a2, $s0
/* 093CB4 800930B4 AFAD0010 */  sw    $t5, 0x10($sp)
/* 093CB8 800930B8 02402025 */  move  $a0, $s2
/* 093CBC 800930BC 0C031110 */  jal   draw_text
/* 093CC0 800930C0 24050058 */   li    $a1, 88
/* 093CC4 800930C4 3C0E800E */  lui   $t6, %hi(gMenuText) # $t6, 0x800e
/* 093CC8 800930C8 8DCEF4A0 */  lw    $t6, %lo(gMenuText)($t6)
/* 093CCC 800930CC 8FA60060 */  lw    $a2, 0x60($sp)
/* 093CD0 800930D0 8DC70028 */  lw    $a3, 0x28($t6)
/* 093CD4 800930D4 240F000C */  li    $t7, 12
/* 093CD8 800930D8 24C6005C */  addiu $a2, $a2, 0x5c
/* 093CDC 800930DC AFA60040 */  sw    $a2, 0x40($sp)
/* 093CE0 800930E0 AFAF0010 */  sw    $t7, 0x10($sp)
/* 093CE4 800930E4 02402025 */  move  $a0, $s2
/* 093CE8 800930E8 0C031110 */  jal   draw_text
/* 093CEC 800930EC 24050058 */   li    $a1, 88
/* 093CF0 800930F0 241800FF */  li    $t8, 255
/* 093CF4 800930F4 AFB80010 */  sw    $t8, 0x10($sp)
/* 093CF8 800930F8 240400FF */  li    $a0, 255
/* 093CFC 800930FC 24050080 */  li    $a1, 128
/* 093D00 80093100 240600FF */  li    $a2, 255
/* 093D04 80093104 0C0310E1 */  jal   set_text_colour
/* 093D08 80093108 24070060 */   li    $a3, 96
/* 093D0C 8009310C 3C098012 */  lui   $t1, %hi(gPlayerSelectVehicle) # $t1, 0x8012
/* 093D10 80093110 812969C0 */  lb    $t1, %lo(gPlayerSelectVehicle)($t1)
/* 093D14 80093114 8FA80050 */  lw    $t0, 0x50($sp)
/* 093D18 80093118 00095080 */  sll   $t2, $t1, 2
/* 093D1C 8009311C 8FB10058 */  lw    $s1, 0x58($sp)
/* 093D20 80093120 010A5821 */  addu  $t3, $t0, $t2
/* 093D24 80093124 8D6C0030 */  lw    $t4, 0x30($t3)
/* 093D28 80093128 0011C840 */  sll   $t9, $s1, 1
/* 093D2C 8009312C 01996821 */  addu  $t5, $t4, $t9
/* 093D30 80093130 95A40000 */  lhu   $a0, ($t5)
/* 093D34 80093134 03208825 */  move  $s1, $t9
/* 093D38 80093138 27A50054 */  addiu $a1, $sp, 0x54
/* 093D3C 8009313C 0C025DBE */  jal   decompress_filename_string
/* 093D40 80093140 24060003 */   li    $a2, 3
/* 093D44 80093144 240E000C */  li    $t6, 12
/* 093D48 80093148 AFAE0010 */  sw    $t6, 0x10($sp)
/* 093D4C 8009314C 02402025 */  move  $a0, $s2
/* 093D50 80093150 24050102 */  li    $a1, 258
/* 093D54 80093154 02003025 */  move  $a2, $s0
/* 093D58 80093158 0C031110 */  jal   draw_text
/* 093D5C 8009315C 27A70054 */   addiu $a3, $sp, 0x54
/* 093D60 80093160 3C188012 */  lui   $t8, %hi(gPlayerSelectVehicle) # $t8, 0x8012
/* 093D64 80093164 831869C0 */  lb    $t8, %lo(gPlayerSelectVehicle)($t8)
/* 093D68 80093168 8FAF0050 */  lw    $t7, 0x50($sp)
/* 093D6C 8009316C 0018C880 */  sll   $t9, $t8, 2
/* 093D70 80093170 01F94821 */  addu  $t1, $t7, $t9
/* 093D74 80093174 8D280018 */  lw    $t0, 0x18($t1)
/* 093D78 80093178 27B00054 */  addiu $s0, $sp, 0x54
/* 093D7C 8009317C 01115021 */  addu  $t2, $t0, $s1
/* 093D80 80093180 95440000 */  lhu   $a0, ($t2)
/* 093D84 80093184 02002825 */  move  $a1, $s0
/* 093D88 80093188 0C025DBE */  jal   decompress_filename_string
/* 093D8C 8009318C 24060003 */   li    $a2, 3
/* 093D90 80093190 8FA60040 */  lw    $a2, 0x40($sp)
/* 093D94 80093194 240B000C */  li    $t3, 12
/* 093D98 80093198 AFAB0010 */  sw    $t3, 0x10($sp)
/* 093D9C 8009319C 02402025 */  move  $a0, $s2
/* 093DA0 800931A0 24050102 */  li    $a1, 258
/* 093DA4 800931A4 0C031110 */  jal   draw_text
/* 093DA8 800931A8 02003825 */   move  $a3, $s0
/* 093DAC 800931AC 8FA20050 */  lw    $v0, 0x50($sp)
/* 093DB0 800931B0 3C0D800E */  lui   $t5, %hi(gTrackIdForPreview) # $t5, 0x800e
/* 093DB4 800931B4 3C198012 */  lui   $t9, %hi(gPlayerSelectVehicle) # $t9, 0x8012
/* 093DB8 800931B8 833969C0 */  lb    $t9, %lo(gPlayerSelectVehicle)($t9)
/* 093DBC 800931BC 8DADF4C4 */  lw    $t5, %lo(gTrackIdForPreview)($t5)
/* 093DC0 800931C0 8C4C004C */  lw    $t4, 0x4c($v0)
/* 093DC4 800931C4 00194880 */  sll   $t1, $t9, 2
/* 093DC8 800931C8 018D7021 */  addu  $t6, $t4, $t5
/* 093DCC 800931CC 81D80002 */  lb    $t8, 2($t6)
/* 093DD0 800931D0 00494021 */  addu  $t0, $v0, $t1
/* 093DD4 800931D4 8D0A003C */  lw    $t2, 0x3c($t0)
/* 093DD8 800931D8 00187840 */  sll   $t7, $t8, 1
/* 093DDC 800931DC 014F5821 */  addu  $t3, $t2, $t7
/* 093DE0 800931E0 95640000 */  lhu   $a0, ($t3)
/* 093DE4 800931E4 240D00FF */  li    $t5, 255
/* 093DE8 800931E8 240C00FF */  li    $t4, 255
/* 093DEC 800931EC AFAC0010 */  sw    $t4, 0x10($sp)
/* 093DF0 800931F0 AFAD0014 */  sw    $t5, 0x14($sp)
/* 093DF4 800931F4 AFA00018 */  sw    $zero, 0x18($sp)
/* 093DF8 800931F8 2405001A */  li    $a1, 26
/* 093DFC 800931FC 24060035 */  li    $a2, 53
/* 093E00 80093200 0C020600 */  jal   show_timestamp
/* 093E04 80093204 24070080 */   li    $a3, 128
/* 093E08 80093208 8FAE0050 */  lw    $t6, 0x50($sp)
/* 093E0C 8009320C 3C188012 */  lui   $t8, %hi(gPlayerSelectVehicle) # $t8, 0x8012
/* 093E10 80093210 3C0F800E */  lui   $t7, %hi(gTrackIdForPreview) # $t7, 0x800e
/* 093E14 80093214 8DEFF4C4 */  lw    $t7, %lo(gTrackIdForPreview)($t7)
/* 093E18 80093218 831869C0 */  lb    $t8, %lo(gPlayerSelectVehicle)($t8)
/* 093E1C 8009321C 8DCA004C */  lw    $t2, 0x4c($t6)
/* 093E20 80093220 0018C880 */  sll   $t9, $t8, 2
/* 093E24 80093224 014F5821 */  addu  $t3, $t2, $t7
/* 093E28 80093228 816C0002 */  lb    $t4, 2($t3)
/* 093E2C 8009322C 01D94821 */  addu  $t1, $t6, $t9
/* 093E30 80093230 8D280024 */  lw    $t0, 0x24($t1)
/* 093E34 80093234 000C6840 */  sll   $t5, $t4, 1
/* 093E38 80093238 010DC021 */  addu  $t8, $t0, $t5
/* 093E3C 8009323C 97040000 */  lhu   $a0, ($t8)
/* 093E40 80093240 240900FF */  li    $t1, 255
/* 093E44 80093244 241900C0 */  li    $t9, 192
/* 093E48 80093248 AFB90010 */  sw    $t9, 0x10($sp)
/* 093E4C 8009324C AFA90014 */  sw    $t1, 0x14($sp)
/* 093E50 80093250 AFA00018 */  sw    $zero, 0x18($sp)
/* 093E54 80093254 2405001A */  li    $a1, 26
/* 093E58 80093258 24060021 */  li    $a2, 33
/* 093E5C 8009325C 0C020600 */  jal   show_timestamp
/* 093E60 80093260 240700FF */   li    $a3, 255
.L80093264:
/* 093E64 80093264 3C068012 */  lui   $a2, %hi(D_801263BC) # $a2, 0x8012
/* 093E68 80093268 8CC663BC */  lw    $a2, %lo(D_801263BC)($a2)
/* 093E6C 8009326C 24040007 */  li    $a0, 7
/* 093E70 80093270 000670C0 */  sll   $t6, $a2, 3
/* 093E74 80093274 29C10100 */  slti  $at, $t6, 0x100
/* 093E78 80093278 14200003 */  bnez  $at, .L80093288
/* 093E7C 8009327C 01C03025 */   move  $a2, $t6
/* 093E80 80093280 240A01FF */  li    $t2, 511
/* 093E84 80093284 014E3023 */  subu  $a2, $t2, $t6
.L80093288:
/* 093E88 80093288 240F00FF */  li    $t7, 255
/* 093E8C 8009328C AFAF0010 */  sw    $t7, 0x10($sp)
/* 093E90 80093290 240500FF */  li    $a1, 255
/* 093E94 80093294 0C0313EF */  jal   set_current_dialogue_background_colour
/* 093E98 80093298 00003825 */   move  $a3, $zero
/* 093E9C 8009329C 8FA20060 */  lw    $v0, 0x60($sp)
/* 093EA0 800932A0 24040007 */  li    $a0, 7
/* 093EA4 800932A4 244B0089 */  addiu $t3, $v0, 0x89
/* 093EA8 800932A8 AFAB0010 */  sw    $t3, 0x10($sp)
/* 093EAC 800932AC 24050086 */  li    $a1, 134
/* 093EB0 800932B0 240700BA */  li    $a3, 186
/* 093EB4 800932B4 0C0313B7 */  jal   set_current_dialogue_box_coords
/* 093EB8 800932B8 24460070 */   addiu $a2, $v0, 0x70
/* 093EBC 800932BC 02402025 */  move  $a0, $s2
/* 093EC0 800932C0 00002825 */  move  $a1, $zero
/* 093EC4 800932C4 00003025 */  move  $a2, $zero
/* 093EC8 800932C8 0C0316D6 */  jal   render_dialogue_box
/* 093ECC 800932CC 24070007 */   li    $a3, 7
/* 093ED0 800932D0 8FA70060 */  lw    $a3, 0x60($sp)
/* 093ED4 800932D4 3C05800E */  lui   $a1, %hi(D_800E05B4) # $a1, 0x800e
/* 093ED8 800932D8 240C00FF */  li    $t4, 255
/* 093EDC 800932DC 240800FF */  li    $t0, 255
/* 093EE0 800932E0 240D00FF */  li    $t5, 255
/* 093EE4 800932E4 241800FF */  li    $t8, 255
/* 093EE8 800932E8 AFB8001C */  sw    $t8, 0x1c($sp)
/* 093EEC 800932EC AFAD0018 */  sw    $t5, 0x18($sp)
/* 093EF0 800932F0 AFA80014 */  sw    $t0, 0x14($sp)
/* 093EF4 800932F4 AFAC0010 */  sw    $t4, 0x10($sp)
/* 093EF8 800932F8 24A505B4 */  addiu $a1, %lo(D_800E05B4) # addiu $a1, $a1, 0x5b4
/* 093EFC 800932FC 02402025 */  move  $a0, $s2
/* 093F00 80093300 24060088 */  li    $a2, 136
/* 093F04 80093304 0C01E2AE */  jal   render_textured_rectangle
/* 093F08 80093308 24E70072 */   addiu $a3, $a3, 0x72
/* 093F0C 8009330C 8FB90060 */  lw    $t9, 0x60($sp)
/* 093F10 80093310 00008025 */  move  $s0, $zero
/* 093F14 80093314 2731008B */  addiu $s1, $t9, 0x8b
/* 093F18 80093318 AFB10040 */  sw    $s1, 0x40($sp)
/* 093F1C 8009331C 8FAE007C */  lw    $t6, 0x7c($sp)
.L80093320:
/* 093F20 80093320 8FA40058 */  lw    $a0, 0x58($sp)
/* 093F24 80093324 29C10002 */  slti  $at, $t6, 2
/* 093F28 80093328 10200008 */  beqz  $at, .L8009334C
/* 093F2C 8009332C 240200FF */   li    $v0, 255
/* 093F30 80093330 0C01AC2B */  jal   func_8006B0AC
/* 093F34 80093334 00000000 */   nop   
/* 093F38 80093338 10500004 */  beq   $v0, $s0, .L8009334C
/* 093F3C 8009333C 240200FF */   li    $v0, 255
/* 093F40 80093340 10000002 */  b     .L8009334C
/* 093F44 80093344 24020080 */   li    $v0, 128
/* 093F48 80093348 240200FF */  li    $v0, 255
.L8009334C:
/* 093F4C 8009334C 8FAB005C */  lw    $t3, 0x5c($sp)
/* 093F50 80093350 240A0001 */  li    $t2, 1
/* 093F54 80093354 020A7804 */  sllv  $t7, $t2, $s0
/* 093F58 80093358 01EB6024 */  and   $t4, $t7, $t3
/* 093F5C 8009335C 11800025 */  beqz  $t4, .L800933F4
/* 093F60 80093360 00104080 */   sll   $t0, $s0, 2
/* 093F64 80093364 3C198012 */  lui   $t9, %hi(gPlayerSelectVehicle) # $t9, 0x8012
/* 093F68 80093368 833969C0 */  lb    $t9, %lo(gPlayerSelectVehicle)($t9)
/* 093F6C 8009336C 01104023 */  subu  $t0, $t0, $s0
/* 093F70 80093370 3C18800E */  lui   $t8, %hi(D_800E0624) # $t8, 0x800e
/* 093F74 80093374 27180624 */  addiu $t8, %lo(D_800E0624) # addiu $t8, $t8, 0x624
/* 093F78 80093378 00086880 */  sll   $t5, $t0, 2
/* 093F7C 8009337C 16190010 */  bne   $s0, $t9, .L800933C0
/* 093F80 80093380 01B81821 */   addu  $v1, $t5, $t8
/* 093F84 80093384 8C650004 */  lw    $a1, 4($v1)
/* 093F88 80093388 240900FF */  li    $t1, 255
/* 093F8C 8009338C 240E00FF */  li    $t6, 255
/* 093F90 80093390 240A00FF */  li    $t2, 255
/* 093F94 80093394 240F00FF */  li    $t7, 255
/* 093F98 80093398 AFAF001C */  sw    $t7, 0x1c($sp)
/* 093F9C 8009339C AFAA0018 */  sw    $t2, 0x18($sp)
/* 093FA0 800933A0 AFAE0014 */  sw    $t6, 0x14($sp)
/* 093FA4 800933A4 AFA90010 */  sw    $t1, 0x10($sp)
/* 093FA8 800933A8 02402025 */  move  $a0, $s2
/* 093FAC 800933AC 24060068 */  li    $a2, 104
/* 093FB0 800933B0 0C01E2AE */  jal   render_textured_rectangle
/* 093FB4 800933B4 02203825 */   move  $a3, $s1
/* 093FB8 800933B8 1000000E */  b     .L800933F4
/* 093FBC 800933BC 26310018 */   addiu $s1, $s1, 0x18
.L800933C0:
/* 093FC0 800933C0 8C650008 */  lw    $a1, 8($v1)
/* 093FC4 800933C4 240B00FF */  li    $t3, 255
/* 093FC8 800933C8 240C00FF */  li    $t4, 255
/* 093FCC 800933CC 240800FF */  li    $t0, 255
/* 093FD0 800933D0 AFA80018 */  sw    $t0, 0x18($sp)
/* 093FD4 800933D4 AFAC0014 */  sw    $t4, 0x14($sp)
/* 093FD8 800933D8 AFAB0010 */  sw    $t3, 0x10($sp)
/* 093FDC 800933DC 02402025 */  move  $a0, $s2
/* 093FE0 800933E0 24060068 */  li    $a2, 104
/* 093FE4 800933E4 02203825 */  move  $a3, $s1
/* 093FE8 800933E8 0C01E2AE */  jal   render_textured_rectangle
/* 093FEC 800933EC AFA2001C */   sw    $v0, 0x1c($sp)
/* 093FF0 800933F0 26310018 */  addiu $s1, $s1, 0x18
.L800933F4:
/* 093FF4 800933F4 26100001 */  addiu $s0, $s0, 1
/* 093FF8 800933F8 24010003 */  li    $at, 3
/* 093FFC 800933FC 1601FFC8 */  bne   $s0, $at, .L80093320
/* 094000 80093400 8FAE007C */   lw    $t6, 0x7c($sp)
/* 094004 80093404 3C028012 */  lui   $v0, %hi(gPlayerSelectVehicle) # $v0, 0x8012
/* 094008 80093408 804269C0 */  lb    $v0, %lo(gPlayerSelectVehicle)($v0)
/* 09400C 8009340C 8FB10040 */  lw    $s1, 0x40($sp)
/* 094010 80093410 24010002 */  li    $at, 2
/* 094014 80093414 14410002 */  bne   $v0, $at, .L80093420
/* 094018 80093418 02402025 */   move  $a0, $s2
/* 09401C 8009341C 26310002 */  addiu $s1, $s1, 2
.L80093420:
/* 094020 80093420 00026880 */  sll   $t5, $v0, 2
/* 094024 80093424 01A26823 */  subu  $t5, $t5, $v0
/* 094028 80093428 000DC080 */  sll   $t8, $t5, 2
/* 09402C 8009342C 3C05800E */  lui   $a1, %hi(D_800E0624) # $a1, 0x800e
/* 094030 80093430 00B82821 */  addu  $a1, $a1, $t8
/* 094034 80093434 8CA50624 */  lw    $a1, %lo(D_800E0624)($a1)
/* 094038 80093438 241900FF */  li    $t9, 255
/* 09403C 8009343C 240900FF */  li    $t1, 255
/* 094040 80093440 240E00FF */  li    $t6, 255
/* 094044 80093444 240A00FF */  li    $t2, 255
/* 094048 80093448 AFAA001C */  sw    $t2, 0x1c($sp)
/* 09404C 8009344C AFAE0018 */  sw    $t6, 0x18($sp)
/* 094050 80093450 AFA90014 */  sw    $t1, 0x14($sp)
/* 094054 80093454 AFB90010 */  sw    $t9, 0x10($sp)
/* 094058 80093458 24060095 */  li    $a2, 149
/* 09405C 8009345C 0C01E2AE */  jal   render_textured_rectangle
/* 094060 80093460 02203825 */   move  $a3, $s1
/* 094064 80093464 0C01ECF4 */  jal   func_8007B3D0
/* 094068 80093468 02402025 */   move  $a0, $s2
/* 09406C 8009346C 3C02800E */  lui   $v0, %hi(gMenuImageStack) # $v0, 0x800e
/* 094070 80093470 2442F75C */  addiu $v0, %lo(gMenuImageStack) # addiu $v0, $v0, -0x8a4
/* 094074 80093474 3C0141A8 */  li    $at, 0x41A80000 # 21.000000
/* 094078 80093478 44812000 */  mtc1  $at, $f4
/* 09407C 8009347C 8C4F0000 */  lw    $t7, ($v0)
/* 094080 80093480 3C01C250 */  li    $at, 0xC2500000 # -52.000000
/* 094084 80093484 E5E400EC */  swc1  $f4, 0xec($t7)
/* 094088 80093488 8C4B0000 */  lw    $t3, ($v0)
/* 09408C 8009348C 44813000 */  mtc1  $at, $f6
/* 094090 80093490 24040007 */  li    $a0, 7
/* 094094 80093494 0C027298 */  jal   func_8009CA60
/* 094098 80093498 E56600F0 */   swc1  $f6, 0xf0($t3)
/* 09409C 8009349C 3C0C8012 */  lui   $t4, %hi(D_801263E0) # $t4, 0x8012
/* 0940A0 800934A0 8D8C63E0 */  lw    $t4, %lo(D_801263E0)($t4)
/* 0940A4 800934A4 00000000 */  nop   
/* 0940A8 800934A8 11800050 */  beqz  $t4, .L800935EC
/* 0940AC 800934AC 8FBF0034 */   lw    $ra, 0x34($sp)
/* 0940B0 800934B0 0C0310BB */  jal   set_text_font
/* 0940B4 800934B4 24040002 */   li    $a0, 2
/* 0940B8 800934B8 00002025 */  move  $a0, $zero
/* 0940BC 800934BC 00002825 */  move  $a1, $zero
/* 0940C0 800934C0 00003025 */  move  $a2, $zero
/* 0940C4 800934C4 0C0310F3 */  jal   set_text_background_colour
/* 0940C8 800934C8 00003825 */   move  $a3, $zero
/* 0940CC 800934CC 240800FF */  li    $t0, 255
/* 0940D0 800934D0 AFA80010 */  sw    $t0, 0x10($sp)
/* 0940D4 800934D4 240400FF */  li    $a0, 255
/* 0940D8 800934D8 240500FF */  li    $a1, 255
/* 0940DC 800934DC 240600FF */  li    $a2, 255
/* 0940E0 800934E0 0C0310E1 */  jal   set_text_colour
/* 0940E4 800934E4 00003825 */   move  $a3, $zero
/* 0940E8 800934E8 8FA60060 */  lw    $a2, 0x60($sp)
/* 0940EC 800934EC 3C07800F */  lui   $a3, %hi(D_800E8240) # $a3, 0x800f
/* 0940F0 800934F0 240D000C */  li    $t5, 12
/* 0940F4 800934F4 AFAD0010 */  sw    $t5, 0x10($sp)
/* 0940F8 800934F8 24E78240 */  addiu $a3, %lo(D_800E8240) # addiu $a3, $a3, -0x7dc0
/* 0940FC 800934FC 02402025 */  move  $a0, $s2
/* 094100 80093500 240500A0 */  li    $a1, 160
/* 094104 80093504 0C031110 */  jal   draw_text
/* 094108 80093508 24C600AC */   addiu $a2, $a2, 0xac
/* 09410C 8009350C 10000037 */  b     .L800935EC
/* 094110 80093510 8FBF0034 */   lw    $ra, 0x34($sp)
.L80093514:
/* 094114 80093514 0C0310BB */  jal   set_text_font
/* 094118 80093518 24040002 */   li    $a0, 2
/* 09411C 8009351C 00002025 */  move  $a0, $zero
/* 094120 80093520 00002825 */  move  $a1, $zero
/* 094124 80093524 00003025 */  move  $a2, $zero
/* 094128 80093528 0C0310F3 */  jal   set_text_background_colour
/* 09412C 8009352C 00003825 */   move  $a3, $zero
/* 094130 80093530 241800FF */  li    $t8, 255
/* 094134 80093534 AFB80010 */  sw    $t8, 0x10($sp)
/* 094138 80093538 240400FF */  li    $a0, 255
/* 09413C 8009353C 240500FF */  li    $a1, 255
/* 094140 80093540 240600FF */  li    $a2, 255
/* 094144 80093544 0C0310E1 */  jal   set_text_colour
/* 094148 80093548 00003825 */   move  $a3, $zero
/* 09414C 8009354C 8FB00060 */  lw    $s0, 0x60($sp)
/* 094150 80093550 00000000 */  nop   
/* 094154 80093554 261000B0 */  addiu $s0, $s0, 0xb0
/* 094158 80093558 0C027AC8 */  jal   get_language
/* 09415C 8009355C 02008825 */   move  $s1, $s0
/* 094160 80093560 24010002 */  li    $at, 2
/* 094164 80093564 1441000A */  bne   $v0, $at, .L80093590
/* 094168 80093568 02402025 */   move  $a0, $s2
/* 09416C 8009356C 3C19800E */  lui   $t9, %hi(gMenuText) # $t9, 0x800e
/* 094170 80093570 8F39F4A0 */  lw    $t9, %lo(gMenuText)($t9)
/* 094174 80093574 2409000C */  li    $t1, 12
/* 094178 80093578 8F270034 */  lw    $a3, 0x34($t9)
/* 09417C 8009357C AFA90010 */  sw    $t1, 0x10($sp)
/* 094180 80093580 240500A0 */  li    $a1, 160
/* 094184 80093584 0C031110 */  jal   draw_text
/* 094188 80093588 02003025 */   move  $a2, $s0
/* 09418C 8009358C 26110020 */  addiu $s1, $s0, 0x20
.L80093590:
/* 094190 80093590 3C0E800E */  lui   $t6, %hi(gMenuText) # $t6, 0x800e
/* 094194 80093594 8DCEF4A0 */  lw    $t6, %lo(gMenuText)($t6)
/* 094198 80093598 240A000C */  li    $t2, 12
/* 09419C 8009359C 8DC7002C */  lw    $a3, 0x2c($t6)
/* 0941A0 800935A0 AFAA0010 */  sw    $t2, 0x10($sp)
/* 0941A4 800935A4 02402025 */  move  $a0, $s2
/* 0941A8 800935A8 240500A0 */  li    $a1, 160
/* 0941AC 800935AC 0C031110 */  jal   draw_text
/* 0941B0 800935B0 02203025 */   move  $a2, $s1
/* 0941B4 800935B4 0C027AC8 */  jal   get_language
/* 0941B8 800935B8 26310020 */   addiu $s1, $s1, 0x20
/* 0941BC 800935BC 24010002 */  li    $at, 2
/* 0941C0 800935C0 10410009 */  beq   $v0, $at, .L800935E8
/* 0941C4 800935C4 02402025 */   move  $a0, $s2
/* 0941C8 800935C8 3C0F800E */  lui   $t7, %hi(gMenuText) # $t7, 0x800e
/* 0941CC 800935CC 8DEFF4A0 */  lw    $t7, %lo(gMenuText)($t7)
/* 0941D0 800935D0 240B000C */  li    $t3, 12
/* 0941D4 800935D4 8DE70034 */  lw    $a3, 0x34($t7)
/* 0941D8 800935D8 AFAB0010 */  sw    $t3, 0x10($sp)
/* 0941DC 800935DC 240500A0 */  li    $a1, 160
/* 0941E0 800935E0 0C031110 */  jal   draw_text
/* 0941E4 800935E4 02203025 */   move  $a2, $s1
.L800935E8:
/* 0941E8 800935E8 8FBF0034 */  lw    $ra, 0x34($sp)
.L800935EC:
/* 0941EC 800935EC 8FB00028 */  lw    $s0, 0x28($sp)
/* 0941F0 800935F0 8FB1002C */  lw    $s1, 0x2c($sp)
/* 0941F4 800935F4 8FB20030 */  lw    $s2, 0x30($sp)
/* 0941F8 800935F8 03E00008 */  jr    $ra
/* 0941FC 800935FC 27BD0078 */   addiu $sp, $sp, 0x78

