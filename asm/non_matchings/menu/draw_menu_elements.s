.late_rodata
glabel jpt_800E836C
.word L800823E8, L80082454, L800824B8, L80082524, L8008256C, L80082608, L80082760, L800827C0

.text
glabel draw_menu_elements
/* 082DEC 800821EC 27BDFF90 */  addiu $sp, $sp, -0x70
/* 082DF0 800821F0 E7B4003C */  swc1  $f20, 0x3c($sp)
/* 082DF4 800821F4 4486A000 */  mtc1  $a2, $f20
/* 082DF8 800821F8 AFB5005C */  sw    $s5, 0x5c($sp)
/* 082DFC 800821FC AFB00048 */  sw    $s0, 0x48($sp)
/* 082E00 80082200 24010004 */  li    $at, 4
/* 082E04 80082204 00A08025 */  move  $s0, $a1
/* 082E08 80082208 AFBF006C */  sw    $ra, 0x6c($sp)
/* 082E0C 8008220C AFBE0068 */  sw    $fp, 0x68($sp)
/* 082E10 80082210 AFB70064 */  sw    $s7, 0x64($sp)
/* 082E14 80082214 AFB60060 */  sw    $s6, 0x60($sp)
/* 082E18 80082218 AFB40058 */  sw    $s4, 0x58($sp)
/* 082E1C 8008221C AFB30054 */  sw    $s3, 0x54($sp)
/* 082E20 80082220 AFB20050 */  sw    $s2, 0x50($sp)
/* 082E24 80082224 AFB1004C */  sw    $s1, 0x4c($sp)
/* 082E28 80082228 E7B70040 */  swc1  $f23, 0x40($sp)
/* 082E2C 8008222C E7B60044 */  swc1  $f22, 0x44($sp)
/* 082E30 80082230 E7B50038 */  swc1  $f21, 0x38($sp)
/* 082E34 80082234 AFA40070 */  sw    $a0, 0x70($sp)
/* 082E38 80082238 1081018F */  beq   $a0, $at, .L80082878
/* 082E3C 8008223C 0000A825 */   move  $s5, $zero
/* 082E40 80082240 3C168012 */  lui   $s6, %hi(sMenuCurrDisplayList) # $s6, 0x8012
/* 082E44 80082244 26D663A0 */  addiu $s6, %lo(sMenuCurrDisplayList) # addiu $s6, $s6, 0x63a0
/* 082E48 80082248 3C058012 */  lui   $a1, %hi(sMenuCurrHudMat) # $a1, 0x8012
/* 082E4C 8008224C 24A563A8 */  addiu $a1, %lo(sMenuCurrHudMat) # addiu $a1, $a1, 0x63a8
/* 082E50 80082250 0C019FCB */  jal   set_ortho_matrix_view
/* 082E54 80082254 02C02025 */   move  $a0, $s6
/* 082E58 80082258 8E110014 */  lw    $s1, 0x14($s0)
/* 082E5C 8008225C 3C013B80 */  li    $at, 0x3B800000 # 0.003906
/* 082E60 80082260 12200175 */  beqz  $s1, .L80082838
/* 082E64 80082264 3C17800E */   lui   $s7, %hi(D_800DF7A0) # $s7, 0x800e
/* 082E68 80082268 3C1E800E */  lui   $fp, %hi(D_800DF79C) # $fp, 0x800e
/* 082E6C 8008226C 3C14800E */  lui   $s4, %hi(gMenuImageStack) # $s4, 0x800e
/* 082E70 80082270 4481B000 */  mtc1  $at, $f22
/* 082E74 80082274 8FA40070 */  lw    $a0, 0x70($sp)
/* 082E78 80082278 2694F75C */  addiu $s4, %lo(gMenuImageStack) # addiu $s4, $s4, -0x8a4
/* 082E7C 8008227C 27DEF79C */  addiu $fp, %lo(D_800DF79C) # addiu $fp, $fp, -0x864
/* 082E80 80082280 26F7F7A0 */  addiu $s7, %lo(D_800DF7A0) # addiu $s7, $s7, -0x860
.L80082284:
/* 082E84 80082284 3C0F8012 */  lui   $t7, %hi(D_80126850) # $t7, 0x8012
/* 082E88 80082288 25EF6850 */  addiu $t7, %lo(D_80126850) # addiu $t7, $t7, 0x6850
/* 082E8C 8008228C 11F10166 */  beq   $t7, $s1, .L80082828
/* 082E90 80082290 00000000 */   nop   
/* 082E94 80082294 14800022 */  bnez  $a0, .L80082320
/* 082E98 80082298 24010001 */   li    $at, 1
/* 082E9C 8008229C 86020000 */  lh    $v0, ($s0)
/* 082EA0 800822A0 86180004 */  lh    $t8, 4($s0)
/* 082EA4 800822A4 86030002 */  lh    $v1, 2($s0)
/* 082EA8 800822A8 0302C823 */  subu  $t9, $t8, $v0
/* 082EAC 800822AC 44992000 */  mtc1  $t9, $f4
/* 082EB0 800822B0 860A0006 */  lh    $t2, 6($s0)
/* 082EB4 800822B4 468021A0 */  cvt.s.w $f6, $f4
/* 082EB8 800822B8 01435823 */  subu  $t3, $t2, $v1
/* 082EBC 800822BC 448B8000 */  mtc1  $t3, $f16
/* 082EC0 800822C0 46143202 */  mul.s $f8, $f6, $f20
/* 082EC4 800822C4 4448F800 */  cfc1  $t0, $31
/* 082EC8 800822C8 00000000 */  nop   
/* 082ECC 800822CC 35010003 */  ori   $at, $t0, 3
/* 082ED0 800822D0 38210002 */  xori  $at, $at, 2
/* 082ED4 800822D4 44C1F800 */  ctc1  $at, $31
/* 082ED8 800822D8 00000000 */  nop   
/* 082EDC 800822DC 460042A4 */  cvt.w.s $f10, $f8
/* 082EE0 800822E0 44C8F800 */  ctc1  $t0, $31
/* 082EE4 800822E4 44095000 */  mfc1  $t1, $f10
/* 082EE8 800822E8 468084A0 */  cvt.s.w $f18, $f16
/* 082EEC 800822EC 01229021 */  addu  $s2, $t1, $v0
/* 082EF0 800822F0 46149102 */  mul.s $f4, $f18, $f20
/* 082EF4 800822F4 444CF800 */  cfc1  $t4, $31
/* 082EF8 800822F8 00000000 */  nop   
/* 082EFC 800822FC 35810003 */  ori   $at, $t4, 3
/* 082F00 80082300 38210002 */  xori  $at, $at, 2
/* 082F04 80082304 44C1F800 */  ctc1  $at, $31
/* 082F08 80082308 00000000 */  nop   
/* 082F0C 8008230C 460021A4 */  cvt.w.s $f6, $f4
/* 082F10 80082310 440D3000 */  mfc1  $t5, $f6
/* 082F14 80082314 44CCF800 */  ctc1  $t4, $31
/* 082F18 80082318 10000028 */  b     .L800823BC
/* 082F1C 8008231C 01A39821 */   addu  $s3, $t5, $v1
.L80082320:
/* 082F20 80082320 14810005 */  bne   $a0, $at, .L80082338
/* 082F24 80082324 00000000 */   nop   
/* 082F28 80082328 86120004 */  lh    $s2, 4($s0)
/* 082F2C 8008232C 86130006 */  lh    $s3, 6($s0)
/* 082F30 80082330 10000023 */  b     .L800823C0
/* 082F34 80082334 920C0013 */   lbu   $t4, 0x13($s0)
.L80082338:
/* 082F38 80082338 86020004 */  lh    $v0, 4($s0)
/* 082F3C 8008233C 860E0008 */  lh    $t6, 8($s0)
/* 082F40 80082340 86030006 */  lh    $v1, 6($s0)
/* 082F44 80082344 01C27823 */  subu  $t7, $t6, $v0
/* 082F48 80082348 448F4000 */  mtc1  $t7, $f8
/* 082F4C 8008234C 8608000A */  lh    $t0, 0xa($s0)
/* 082F50 80082350 468042A0 */  cvt.s.w $f10, $f8
/* 082F54 80082354 01034823 */  subu  $t1, $t0, $v1
/* 082F58 80082358 44892000 */  mtc1  $t1, $f4
/* 082F5C 8008235C 46145402 */  mul.s $f16, $f10, $f20
/* 082F60 80082360 4458F800 */  cfc1  $t8, $31
/* 082F64 80082364 00000000 */  nop   
/* 082F68 80082368 37010003 */  ori   $at, $t8, 3
/* 082F6C 8008236C 38210002 */  xori  $at, $at, 2
/* 082F70 80082370 44C1F800 */  ctc1  $at, $31
/* 082F74 80082374 00000000 */  nop   
/* 082F78 80082378 460084A4 */  cvt.w.s $f18, $f16
/* 082F7C 8008237C 44D8F800 */  ctc1  $t8, $31
/* 082F80 80082380 44199000 */  mfc1  $t9, $f18
/* 082F84 80082384 468021A0 */  cvt.s.w $f6, $f4
/* 082F88 80082388 03229021 */  addu  $s2, $t9, $v0
/* 082F8C 8008238C 46143202 */  mul.s $f8, $f6, $f20
/* 082F90 80082390 444AF800 */  cfc1  $t2, $31
/* 082F94 80082394 00000000 */  nop   
/* 082F98 80082398 35410003 */  ori   $at, $t2, 3
/* 082F9C 8008239C 38210002 */  xori  $at, $at, 2
/* 082FA0 800823A0 44C1F800 */  ctc1  $at, $31
/* 082FA4 800823A4 00000000 */  nop   
/* 082FA8 800823A8 460042A4 */  cvt.w.s $f10, $f8
/* 082FAC 800823AC 440B5000 */  mfc1  $t3, $f10
/* 082FB0 800823B0 44CAF800 */  ctc1  $t2, $31
/* 082FB4 800823B4 01639821 */  addu  $s3, $t3, $v1
/* 082FB8 800823B8 00000000 */  nop   
.L800823BC:
/* 082FBC 800823BC 920C0013 */  lbu   $t4, 0x13($s0)
.L800823C0:
/* 082FC0 800823C0 00000000 */  nop   
/* 082FC4 800823C4 2D810008 */  sltiu $at, $t4, 8
/* 082FC8 800823C8 10200117 */  beqz  $at, .L80082828
/* 082FCC 800823CC 000C6080 */   sll   $t4, $t4, 2
/* 082FD0 800823D0 3C01800F */  lui   $at, %hi(jpt_800E836C) # $at, 0x800f
/* 082FD4 800823D4 002C0821 */  addu  $at, $at, $t4
/* 082FD8 800823D8 8C2C836C */  lw    $t4, %lo(jpt_800E836C)($at)
/* 082FDC 800823DC 00000000 */  nop   
/* 082FE0 800823E0 01800008 */  jr    $t4
/* 082FE4 800823E4 00000000 */   nop   
glabel L800823E8
/* 082FE8 800823E8 86040018 */  lh    $a0, 0x18($s0)
/* 082FEC 800823EC 8605001A */  lh    $a1, 0x1a($s0)
/* 082FF0 800823F0 8606001C */  lh    $a2, 0x1c($s0)
/* 082FF4 800823F4 8607001E */  lh    $a3, 0x1e($s0)
/* 082FF8 800823F8 0C0310F3 */  jal   set_text_background_colour
/* 082FFC 800823FC 00000000 */   nop   
/* 083000 80082400 920D0010 */  lbu   $t5, 0x10($s0)
/* 083004 80082404 9204000C */  lbu   $a0, 0xc($s0)
/* 083008 80082408 9205000D */  lbu   $a1, 0xd($s0)
/* 08300C 8008240C 9206000E */  lbu   $a2, 0xe($s0)
/* 083010 80082410 9207000F */  lbu   $a3, 0xf($s0)
/* 083014 80082414 0C0310E1 */  jal   set_text_colour
/* 083018 80082418 AFAD0010 */   sw    $t5, 0x10($sp)
/* 08301C 8008241C 92040011 */  lbu   $a0, 0x11($s0)
/* 083020 80082420 0C0310BB */  jal   set_text_font
/* 083024 80082424 00000000 */   nop   
/* 083028 80082428 8FCE0000 */  lw    $t6, ($fp)
/* 08302C 8008242C 920F0012 */  lbu   $t7, 0x12($s0)
/* 083030 80082430 8E070014 */  lw    $a3, 0x14($s0)
/* 083034 80082434 02C02025 */  move  $a0, $s6
/* 083038 80082438 02402825 */  move  $a1, $s2
/* 08303C 8008243C 026E3021 */  addu  $a2, $s3, $t6
/* 083040 80082440 0C031110 */  jal   draw_text
/* 083044 80082444 AFAF0010 */   sw    $t7, 0x10($sp)
/* 083048 80082448 8FA40070 */  lw    $a0, 0x70($sp)
/* 08304C 8008244C 100000F7 */  b     .L8008282C
/* 083050 80082450 8E110034 */   lw    $s1, 0x34($s0)
glabel L80082454
/* 083054 80082454 12A00003 */  beqz  $s5, .L80082464
/* 083058 80082458 02C02025 */   move  $a0, $s6
/* 08305C 8008245C 0C01ECF4 */  jal   reset_render_settings
/* 083060 80082460 0000A825 */   move  $s5, $zero
.L80082464:
/* 083064 80082464 92180010 */  lbu   $t8, 0x10($s0)
/* 083068 80082468 3C01800E */  lui   $at, %hi(sMenuGuiOpacity) # $at, 0x800e
/* 08306C 8008246C AC38F764 */  sw    $t8, %lo(sMenuGuiOpacity)($at)
/* 083070 80082470 8E190014 */  lw    $t9, 0x14($s0)
/* 083074 80082474 920A000D */  lbu   $t2, 0xd($s0)
/* 083078 80082478 9207000C */  lbu   $a3, 0xc($s0)
/* 08307C 8008247C 97240000 */  lhu   $a0, ($t9)
/* 083080 80082480 AFAA0010 */  sw    $t2, 0x10($sp)
/* 083084 80082484 920B000E */  lbu   $t3, 0xe($s0)
/* 083088 80082488 8EE90000 */  lw    $t1, ($s7)
/* 08308C 8008248C AFAB0014 */  sw    $t3, 0x14($sp)
/* 083090 80082490 920C0011 */  lbu   $t4, 0x11($s0)
/* 083094 80082494 00134023 */  negu  $t0, $s3
/* 083098 80082498 01093023 */  subu  $a2, $t0, $t1
/* 08309C 8008249C 24C60078 */  addiu $a2, $a2, 0x78
/* 0830A0 800824A0 2645FF60 */  addiu $a1, $s2, -0xa0
/* 0830A4 800824A4 0C020600 */  jal   show_timestamp
/* 0830A8 800824A8 AFAC0018 */   sw    $t4, 0x18($sp)
/* 0830AC 800824AC 8FA40070 */  lw    $a0, 0x70($sp)
/* 0830B0 800824B0 100000DE */  b     .L8008282C
/* 0830B4 800824B4 8E110034 */   lw    $s1, 0x34($s0)
glabel L800824B8
/* 0830B8 800824B8 12A00005 */  beqz  $s5, .L800824D0
/* 0830BC 800824BC 02C02025 */   move  $a0, $s6
/* 0830C0 800824C0 0C01ECF4 */  jal   reset_render_settings
/* 0830C4 800824C4 0000A825 */   move  $s5, $zero
/* 0830C8 800824C8 8E110014 */  lw    $s1, 0x14($s0)
/* 0830CC 800824CC 00000000 */  nop   
.L800824D0:
/* 0830D0 800824D0 920F000D */  lbu   $t7, 0xd($s0)
/* 0830D4 800824D4 8E240000 */  lw    $a0, ($s1)
/* 0830D8 800824D8 9207000C */  lbu   $a3, 0xc($s0)
/* 0830DC 800824DC AFAF0010 */  sw    $t7, 0x10($sp)
/* 0830E0 800824E0 9218000E */  lbu   $t8, 0xe($s0)
/* 0830E4 800824E4 8EEE0000 */  lw    $t6, ($s7)
/* 0830E8 800824E8 AFB80014 */  sw    $t8, 0x14($sp)
/* 0830EC 800824EC 92190010 */  lbu   $t9, 0x10($s0)
/* 0830F0 800824F0 00136823 */  negu  $t5, $s3
/* 0830F4 800824F4 AFB90018 */  sw    $t9, 0x18($sp)
/* 0830F8 800824F8 92080011 */  lbu   $t0, 0x11($s0)
/* 0830FC 800824FC 01AE3023 */  subu  $a2, $t5, $t6
/* 083100 80082500 AFA8001C */  sw    $t0, 0x1c($sp)
/* 083104 80082504 92090012 */  lbu   $t1, 0x12($s0)
/* 083108 80082508 24C60078 */  addiu $a2, $a2, 0x78
/* 08310C 8008250C 2645FF60 */  addiu $a1, $s2, -0xa0
/* 083110 80082510 0C020701 */  jal   func_80081C04
/* 083114 80082514 AFA90020 */   sw    $t1, 0x20($sp)
/* 083118 80082518 8FA40070 */  lw    $a0, 0x70($sp)
/* 08311C 8008251C 100000C3 */  b     .L8008282C
/* 083120 80082520 8E110034 */   lw    $s1, 0x34($s0)
glabel L80082524
/* 083124 80082524 920B000C */  lbu   $t3, 0xc($s0)
/* 083128 80082528 8FCA0000 */  lw    $t2, ($fp)
/* 08312C 8008252C AFAB0010 */  sw    $t3, 0x10($sp)
/* 083130 80082530 920C000D */  lbu   $t4, 0xd($s0)
/* 083134 80082534 02C02025 */  move  $a0, $s6
/* 083138 80082538 AFAC0014 */  sw    $t4, 0x14($sp)
/* 08313C 8008253C 920D000E */  lbu   $t5, 0xe($s0)
/* 083140 80082540 02202825 */  move  $a1, $s1
/* 083144 80082544 AFAD0018 */  sw    $t5, 0x18($sp)
/* 083148 80082548 920E0010 */  lbu   $t6, 0x10($s0)
/* 08314C 8008254C 02403025 */  move  $a2, $s2
/* 083150 80082550 24150001 */  li    $s5, 1
/* 083154 80082554 026A3821 */  addu  $a3, $s3, $t2
/* 083158 80082558 0C01E2AE */  jal   render_textured_rectangle
/* 08315C 8008255C AFAE001C */   sw    $t6, 0x1c($sp)
/* 083160 80082560 8FA40070 */  lw    $a0, 0x70($sp)
/* 083164 80082564 100000B1 */  b     .L8008282C
/* 083168 80082568 8E110034 */   lw    $s1, 0x34($s0)
glabel L8008256C
/* 08316C 8008256C 86190018 */  lh    $t9, 0x18($s0)
/* 083170 80082570 44928000 */  mtc1  $s2, $f16
/* 083174 80082574 44992000 */  mtc1  $t9, $f4
/* 083178 80082578 8FCF0000 */  lw    $t7, ($fp)
/* 08317C 8008257C 468021A0 */  cvt.s.w $f6, $f4
/* 083180 80082580 026FC021 */  addu  $t8, $s3, $t7
/* 083184 80082584 44989000 */  mtc1  $t8, $f18
/* 083188 80082588 46163202 */  mul.s $f8, $f6, $f22
/* 08318C 8008258C 02C02025 */  move  $a0, $s6
/* 083190 80082590 02202825 */  move  $a1, $s1
/* 083194 80082594 24150001 */  li    $s5, 1
/* 083198 80082598 E7A80010 */  swc1  $f8, 0x10($sp)
/* 08319C 8008259C 46808420 */  cvt.s.w $f16, $f16
/* 0831A0 800825A0 8608001A */  lh    $t0, 0x1a($s0)
/* 0831A4 800825A4 00000000 */  nop   
/* 0831A8 800825A8 44885000 */  mtc1  $t0, $f10
/* 0831AC 800825AC 468094A0 */  cvt.s.w $f18, $f18
/* 0831B0 800825B0 44068000 */  mfc1  $a2, $f16
/* 0831B4 800825B4 46805420 */  cvt.s.w $f16, $f10
/* 0831B8 800825B8 44079000 */  mfc1  $a3, $f18
/* 0831BC 800825BC 46168482 */  mul.s $f18, $f16, $f22
/* 0831C0 800825C0 E7B20014 */  swc1  $f18, 0x14($sp)
/* 0831C4 800825C4 920B000D */  lbu   $t3, 0xd($s0)
/* 0831C8 800825C8 9209000C */  lbu   $t1, 0xc($s0)
/* 0831CC 800825CC 920E000E */  lbu   $t6, 0xe($s0)
/* 0831D0 800825D0 000B6400 */  sll   $t4, $t3, 0x10
/* 0831D4 800825D4 00095600 */  sll   $t2, $t1, 0x18
/* 0831D8 800825D8 92190010 */  lbu   $t9, 0x10($s0)
/* 0831DC 800825DC 014C6825 */  or    $t5, $t2, $t4
/* 0831E0 800825E0 000E7A00 */  sll   $t7, $t6, 8
/* 0831E4 800825E4 01AFC025 */  or    $t8, $t5, $t7
/* 0831E8 800825E8 03194025 */  or    $t0, $t8, $t9
/* 0831EC 800825EC AFA80018 */  sw    $t0, 0x18($sp)
/* 0831F0 800825F0 92090012 */  lbu   $t1, 0x12($s0)
/* 0831F4 800825F4 0C01E340 */  jal   render_texture_rectangle_scaled
/* 0831F8 800825F8 AFA9001C */   sw    $t1, 0x1c($sp)
/* 0831FC 800825FC 8FA40070 */  lw    $a0, 0x70($sp)
/* 083200 80082600 1000008A */  b     .L8008282C
/* 083204 80082604 8E110034 */   lw    $s1, 0x34($s0)
glabel L80082608
/* 083208 80082608 12A00003 */  beqz  $s5, .L80082618
/* 08320C 8008260C 02C02025 */   move  $a0, $s6
/* 083210 80082610 0C01ECF4 */  jal   reset_render_settings
/* 083214 80082614 0000A825 */   move  $s5, $zero
.L80082618:
/* 083218 80082618 0C01A142 */  jal   func_80068508
/* 08321C 8008261C 24040001 */   li    $a0, 1
/* 083220 80082620 0C01EFC7 */  jal   sprite_opaque
/* 083224 80082624 00002025 */   move  $a0, $zero
/* 083228 80082628 264BFF60 */  addiu $t3, $s2, -0xa0
/* 08322C 8008262C 448B2000 */  mtc1  $t3, $f4
/* 083230 80082630 8E0C0014 */  lw    $t4, 0x14($s0)
/* 083234 80082634 468021A0 */  cvt.s.w $f6, $f4
/* 083238 80082638 8E8A0000 */  lw    $t2, ($s4)
/* 08323C 8008263C 000C7140 */  sll   $t6, $t4, 5
/* 083240 80082640 014E6821 */  addu  $t5, $t2, $t6
/* 083244 80082644 E5A6000C */  swc1  $f6, 0xc($t5)
/* 083248 80082648 8EF80000 */  lw    $t8, ($s7)
/* 08324C 8008264C 00137823 */  negu  $t7, $s3
/* 083250 80082650 01F8C823 */  subu  $t9, $t7, $t8
/* 083254 80082654 27280078 */  addiu $t0, $t9, 0x78
/* 083258 80082658 44884000 */  mtc1  $t0, $f8
/* 08325C 8008265C 8E0B0014 */  lw    $t3, 0x14($s0)
/* 083260 80082660 468042A0 */  cvt.s.w $f10, $f8
/* 083264 80082664 8E890000 */  lw    $t1, ($s4)
/* 083268 80082668 000B6140 */  sll   $t4, $t3, 5
/* 08326C 8008266C 012C5021 */  addu  $t2, $t1, $t4
/* 083270 80082670 E54A0010 */  swc1  $f10, 0x10($t2)
/* 083274 80082674 8E0F0014 */  lw    $t7, 0x14($s0)
/* 083278 80082678 8E8D0000 */  lw    $t5, ($s4)
/* 08327C 8008267C 920E0011 */  lbu   $t6, 0x11($s0)
/* 083280 80082680 000FC140 */  sll   $t8, $t7, 5
/* 083284 80082684 01B8C821 */  addu  $t9, $t5, $t8
/* 083288 80082688 A72E0018 */  sh    $t6, 0x18($t9)
/* 08328C 8008268C 8E090014 */  lw    $t1, 0x14($s0)
/* 083290 80082690 8E8B0000 */  lw    $t3, ($s4)
/* 083294 80082694 86080018 */  lh    $t0, 0x18($s0)
/* 083298 80082698 00096140 */  sll   $t4, $t1, 5
/* 08329C 8008269C 016C5021 */  addu  $t2, $t3, $t4
/* 0832A0 800826A0 A5480004 */  sh    $t0, 4($t2)
/* 0832A4 800826A4 8E180014 */  lw    $t8, 0x14($s0)
/* 0832A8 800826A8 8E8D0000 */  lw    $t5, ($s4)
/* 0832AC 800826AC 860F001A */  lh    $t7, 0x1a($s0)
/* 0832B0 800826B0 00187140 */  sll   $t6, $t8, 5
/* 0832B4 800826B4 01AEC821 */  addu  $t9, $t5, $t6
/* 0832B8 800826B8 A72F0002 */  sh    $t7, 2($t9)
/* 0832BC 800826BC 8E0C0014 */  lw    $t4, 0x14($s0)
/* 0832C0 800826C0 8E8B0000 */  lw    $t3, ($s4)
/* 0832C4 800826C4 8609001C */  lh    $t1, 0x1c($s0)
/* 0832C8 800826C8 000C4140 */  sll   $t0, $t4, 5
/* 0832CC 800826CC 01685021 */  addu  $t2, $t3, $t0
/* 0832D0 800826D0 A5490000 */  sh    $t1, ($t2)
/* 0832D4 800826D4 8618001E */  lh    $t8, 0x1e($s0)
/* 0832D8 800826D8 8E0E0014 */  lw    $t6, 0x14($s0)
/* 0832DC 800826DC 44988000 */  mtc1  $t8, $f16
/* 0832E0 800826E0 8E8D0000 */  lw    $t5, ($s4)
/* 0832E4 800826E4 468084A0 */  cvt.s.w $f18, $f16
/* 0832E8 800826E8 000E7940 */  sll   $t7, $t6, 5
/* 0832EC 800826EC 01AFC821 */  addu  $t9, $t5, $t7
/* 0832F0 800826F0 46169102 */  mul.s $f4, $f18, $f22
/* 0832F4 800826F4 3C01800E */  lui   $at, %hi(sMenuGuiColourR) # $at, 0x800e
/* 0832F8 800826F8 E7240008 */  swc1  $f4, 8($t9)
/* 0832FC 800826FC 920C000C */  lbu   $t4, 0xc($s0)
/* 083300 80082700 00000000 */  nop   
/* 083304 80082704 A02CF4A4 */  sb    $t4, %lo(sMenuGuiColourR)($at)
/* 083308 80082708 920B000D */  lbu   $t3, 0xd($s0)
/* 08330C 8008270C 3C01800E */  lui   $at, %hi(sMenuGuiColourG) # $at, 0x800e
/* 083310 80082710 A02BF4A8 */  sb    $t3, %lo(sMenuGuiColourG)($at)
/* 083314 80082714 9208000E */  lbu   $t0, 0xe($s0)
/* 083318 80082718 3C01800E */  lui   $at, %hi(sMenuGuiColourB) # $at, 0x800e
/* 08331C 8008271C A028F4AC */  sb    $t0, %lo(sMenuGuiColourB)($at)
/* 083320 80082720 9209000F */  lbu   $t1, 0xf($s0)
/* 083324 80082724 3C01800E */  lui   $at, %hi(D_800DF4B0) # $at, 0x800e
/* 083328 80082728 A029F4B0 */  sb    $t1, %lo(D_800DF4B0)($at)
/* 08332C 8008272C 920A0010 */  lbu   $t2, 0x10($s0)
/* 083330 80082730 3C01800E */  lui   $at, %hi(sMenuGuiOpacity) # $at, 0x800e
/* 083334 80082734 AC2AF764 */  sw    $t2, %lo(sMenuGuiOpacity)($at)
/* 083338 80082738 8E040014 */  lw    $a0, 0x14($s0)
/* 08333C 8008273C 0C027298 */  jal   func_8009CA60
/* 083340 80082740 00000000 */   nop   
/* 083344 80082744 0C01A142 */  jal   func_80068508
/* 083348 80082748 00002025 */   move  $a0, $zero
/* 08334C 8008274C 0C01EFC7 */  jal   sprite_opaque
/* 083350 80082750 24040001 */   li    $a0, 1
/* 083354 80082754 8FA40070 */  lw    $a0, 0x70($sp)
/* 083358 80082758 10000034 */  b     .L8008282C
/* 08335C 8008275C 8E110034 */   lw    $s1, 0x34($s0)
glabel L80082760
/* 083360 80082760 860E001A */  lh    $t6, 0x1a($s0)
/* 083364 80082764 86070018 */  lh    $a3, 0x18($s0)
/* 083368 80082768 AFAE0010 */  sw    $t6, 0x10($sp)
/* 08336C 8008276C 860D001C */  lh    $t5, 0x1c($s0)
/* 083370 80082770 8EF80000 */  lw    $t8, ($s7)
/* 083374 80082774 AFAD0014 */  sw    $t5, 0x14($sp)
/* 083378 80082778 860F001E */  lh    $t7, 0x1e($s0)
/* 08337C 8008277C 02C02025 */  move  $a0, $s6
/* 083380 80082780 AFAF0018 */  sw    $t7, 0x18($sp)
/* 083384 80082784 9219000C */  lbu   $t9, 0xc($s0)
/* 083388 80082788 02402825 */  move  $a1, $s2
/* 08338C 8008278C AFB9001C */  sw    $t9, 0x1c($sp)
/* 083390 80082790 920C000D */  lbu   $t4, 0xd($s0)
/* 083394 80082794 02783021 */  addu  $a2, $s3, $t8
/* 083398 80082798 AFAC0020 */  sw    $t4, 0x20($sp)
/* 08339C 8008279C 920B000E */  lbu   $t3, 0xe($s0)
/* 0833A0 800827A0 00000000 */  nop   
/* 0833A4 800827A4 AFAB0024 */  sw    $t3, 0x24($sp)
/* 0833A8 800827A8 92080010 */  lbu   $t0, 0x10($s0)
/* 0833AC 800827AC 0C0203A4 */  jal   func_80080E90
/* 0833B0 800827B0 AFA80028 */   sw    $t0, 0x28($sp)
/* 0833B4 800827B4 8FA40070 */  lw    $a0, 0x70($sp)
/* 0833B8 800827B8 1000001C */  b     .L8008282C
/* 0833BC 800827BC 8E110034 */   lw    $s1, 0x34($s0)
glabel L800827C0
/* 0833C0 800827C0 860A001A */  lh    $t2, 0x1a($s0)
/* 0833C4 800827C4 86070018 */  lh    $a3, 0x18($s0)
/* 0833C8 800827C8 AFAA0010 */  sw    $t2, 0x10($sp)
/* 0833CC 800827CC 8618001C */  lh    $t8, 0x1c($s0)
/* 0833D0 800827D0 8EE90000 */  lw    $t1, ($s7)
/* 0833D4 800827D4 AFB80014 */  sw    $t8, 0x14($sp)
/* 0833D8 800827D8 860E001E */  lh    $t6, 0x1e($s0)
/* 0833DC 800827DC 02693021 */  addu  $a2, $s3, $t1
/* 0833E0 800827E0 AFAE0018 */  sw    $t6, 0x18($sp)
/* 0833E4 800827E4 9219000D */  lbu   $t9, 0xd($s0)
/* 0833E8 800827E8 920D000C */  lbu   $t5, 0xc($s0)
/* 0833EC 800827EC 9208000E */  lbu   $t0, 0xe($s0)
/* 0833F0 800827F0 00196400 */  sll   $t4, $t9, 0x10
/* 0833F4 800827F4 000D7E00 */  sll   $t7, $t5, 0x18
/* 0833F8 800827F8 92180010 */  lbu   $t8, 0x10($s0)
/* 0833FC 800827FC 01EC5825 */  or    $t3, $t7, $t4
/* 083400 80082800 00084A00 */  sll   $t1, $t0, 8
/* 083404 80082804 01695025 */  or    $t2, $t3, $t1
/* 083408 80082808 01587025 */  or    $t6, $t2, $t8
/* 08340C 8008280C AFAE001C */  sw    $t6, 0x1c($sp)
/* 083410 80082810 AFB10020 */  sw    $s1, 0x20($sp)
/* 083414 80082814 02C02025 */  move  $a0, $s6
/* 083418 80082818 0C020160 */  jal   func_80080580
/* 08341C 8008281C 02402825 */   move  $a1, $s2
/* 083420 80082820 8FA40070 */  lw    $a0, 0x70($sp)
/* 083424 80082824 00000000 */  nop   
.L80082828:
/* 083428 80082828 8E110034 */  lw    $s1, 0x34($s0)
.L8008282C:
/* 08342C 8008282C 26100020 */  addiu $s0, $s0, 0x20
/* 083430 80082830 1620FE94 */  bnez  $s1, .L80082284
/* 083434 80082834 00000000 */   nop   
.L80082838:
/* 083438 80082838 12A00004 */  beqz  $s5, .L8008284C
/* 08343C 8008283C 240200FF */   li    $v0, 255
/* 083440 80082840 0C01ECF4 */  jal   reset_render_settings
/* 083444 80082844 02C02025 */   move  $a0, $s6
/* 083448 80082848 240200FF */  li    $v0, 255
.L8008284C:
/* 08344C 8008284C 3C01800E */  lui   $at, %hi(sMenuGuiColourR) # $at, 0x800e
/* 083450 80082850 A022F4A4 */  sb    $v0, %lo(sMenuGuiColourR)($at)
/* 083454 80082854 3C01800E */  lui   $at, %hi(sMenuGuiColourG) # $at, 0x800e
/* 083458 80082858 A022F4A8 */  sb    $v0, %lo(sMenuGuiColourG)($at)
/* 08345C 8008285C 3C01800E */  lui   $at, %hi(sMenuGuiColourB) # $at, 0x800e
/* 083460 80082860 A022F4AC */  sb    $v0, %lo(sMenuGuiColourB)($at)
/* 083464 80082864 3C01800E */  lui   $at, %hi(D_800DF4B0) # $at, 0x800e
/* 083468 80082868 A020F4B0 */  sb    $zero, %lo(D_800DF4B0)($at)
/* 08346C 8008286C 3C01800E */  lui   $at, %hi(sMenuGuiOpacity) # $at, 0x800e
/* 083470 80082870 240D00FF */  li    $t5, 255
/* 083474 80082874 AC2DF764 */  sw    $t5, %lo(sMenuGuiOpacity)($at)
.L80082878:
/* 083478 80082878 8FBF006C */  lw    $ra, 0x6c($sp)
/* 08347C 8008287C C7B50038 */  lwc1  $f21, 0x38($sp)
/* 083480 80082880 C7B4003C */  lwc1  $f20, 0x3c($sp)
/* 083484 80082884 C7B70040 */  lwc1  $f23, 0x40($sp)
/* 083488 80082888 C7B60044 */  lwc1  $f22, 0x44($sp)
/* 08348C 8008288C 8FB00048 */  lw    $s0, 0x48($sp)
/* 083490 80082890 8FB1004C */  lw    $s1, 0x4c($sp)
/* 083494 80082894 8FB20050 */  lw    $s2, 0x50($sp)
/* 083498 80082898 8FB30054 */  lw    $s3, 0x54($sp)
/* 08349C 8008289C 8FB40058 */  lw    $s4, 0x58($sp)
/* 0834A0 800828A0 8FB5005C */  lw    $s5, 0x5c($sp)
/* 0834A4 800828A4 8FB60060 */  lw    $s6, 0x60($sp)
/* 0834A8 800828A8 8FB70064 */  lw    $s7, 0x64($sp)
/* 0834AC 800828AC 8FBE0068 */  lw    $fp, 0x68($sp)
/* 0834B0 800828B0 03E00008 */  jr    $ra
/* 0834B4 800828B4 27BD0070 */   addiu $sp, $sp, 0x70

