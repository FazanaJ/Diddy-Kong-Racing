glabel func_800BB2F4
/* 0BBEF4 800BB2F4 27BDFF58 */  addiu $sp, $sp, -0xa8
/* 0BBEF8 800BB2F8 AFBF0014 */  sw    $ra, 0x14($sp)
/* 0BBEFC 800BB2FC AFA400A8 */  sw    $a0, 0xa8($sp)
/* 0BBF00 800BB300 AFA500AC */  sw    $a1, 0xac($sp)
/* 0BBF04 800BB304 AFA600B0 */  sw    $a2, 0xb0($sp)
/* 0BBF08 800BB308 04800007 */  bltz  $a0, .L800BB328
/* 0BBF0C 800BB30C AFA700B4 */   sw    $a3, 0xb4($sp)
/* 0BBF10 800BB310 3C0F8013 */  lui   $t7, %hi(D_8012A0E0) # $t7, 0x8013
/* 0BBF14 800BB314 8DEFA0E0 */  lw    $t7, %lo(D_8012A0E0)($t7)
/* 0BBF18 800BB318 00000000 */  nop   
/* 0BBF1C 800BB31C 008F082A */  slt   $at, $a0, $t7
/* 0BBF20 800BB320 14200003 */  bnez  $at, .L800BB330
/* 0BBF24 800BB324 8FB800A8 */   lw    $t8, 0xa8($sp)
.L800BB328:
/* 0BBF28 800BB328 AFA000A8 */  sw    $zero, 0xa8($sp)
/* 0BBF2C 800BB32C 8FB800A8 */  lw    $t8, 0xa8($sp)
.L800BB330:
/* 0BBF30 800BB330 3C19800E */  lui   $t9, %hi(D_800E30D8) # $t9, 0x800e
/* 0BBF34 800BB334 001870C0 */  sll   $t6, $t8, 3
/* 0BBF38 800BB338 8F3930D8 */  lw    $t9, %lo(D_800E30D8)($t9)
/* 0BBF3C 800BB33C 01D87023 */  subu  $t6, $t6, $t8
/* 0BBF40 800BB340 000E7080 */  sll   $t6, $t6, 2
/* 0BBF44 800BB344 032E2821 */  addu  $a1, $t9, $t6
/* 0BBF48 800BB348 3C19800E */  lui   $t9, %hi(D_800E317C) # $t9, 0x800e
/* 0BBF4C 800BB34C 8F39317C */  lw    $t9, %lo(D_800E317C)($t9)
/* 0BBF50 800BB350 84AF0006 */  lh    $t7, 6($a1)
/* 0BBF54 800BB354 03190019 */  multu $t8, $t9
/* 0BBF58 800BB358 448F5000 */  mtc1  $t7, $f10
/* 0BBF5C 800BB35C 3C018013 */  lui   $at, %hi(D_8012A0BC) # $at, 0x8013
/* 0BBF60 800BB360 46805120 */  cvt.s.w $f4, $f10
/* 0BBF64 800BB364 C426A0BC */  lwc1  $f6, %lo(D_8012A0BC)($at)
/* 0BBF68 800BB368 3C018013 */  lui   $at, %hi(D_8012A0B8) # $at, 0x8013
/* 0BBF6C 800BB36C C428A0B8 */  lwc1  $f8, %lo(D_8012A0B8)($at)
/* 0BBF70 800BB370 3C098013 */  lui   $t1, %hi(D_80129FC8) # $t1, 0x8013
/* 0BBF74 800BB374 25299FC8 */  addiu $t1, %lo(D_80129FC8) # addiu $t1, $t1, -0x6038
/* 0BBF78 800BB378 E7A40078 */  swc1  $f4, 0x78($sp)
/* 0BBF7C 800BB37C 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0BBF80 800BB380 44815000 */  mtc1  $at, $f10
/* 0BBF84 800BB384 C5240044 */  lwc1  $f4, 0x44($t1)
/* 0BBF88 800BB388 3C0142FE */  li    $at, 0x42FE0000 # 127.000000
/* 0BBF8C 800BB38C 46045281 */  sub.s $f10, $f10, $f4
/* 0BBF90 800BB390 44812000 */  mtc1  $at, $f4
/* 0BBF94 800BB394 8D2F0028 */  lw    $t7, 0x28($t1)
/* 0BBF98 800BB398 00007012 */  mflo  $t6
/* 0BBF9C 800BB39C AFAE0058 */  sw    $t6, 0x58($sp)
/* 0BBFA0 800BB3A0 E7A60090 */  swc1  $f6, 0x90($sp)
/* 0BBFA4 800BB3A4 E7A80094 */  swc1  $f8, 0x94($sp)
/* 0BBFA8 800BB3A8 11E0000D */  beqz  $t7, .L800BB3E0
/* 0BBFAC 800BB3AC 46045403 */   div.s $f16, $f10, $f4
/* 0BBFB0 800BB3B0 3C013F00 */  li    $at, 0x3F000000 # 0.500000
/* 0BBFB4 800BB3B4 44815000 */  mtc1  $at, $f10
/* 0BBFB8 800BB3B8 8D380000 */  lw    $t8, ($t1)
/* 0BBFBC 800BB3BC 460A3102 */  mul.s $f4, $f6, $f10
/* 0BBFC0 800BB3C0 44813000 */  mtc1  $at, $f6
/* 0BBFC4 800BB3C4 0018C840 */  sll   $t9, $t8, 1
/* 0BBFC8 800BB3C8 272E0001 */  addiu $t6, $t9, 1
/* 0BBFCC 800BB3CC 46064282 */  mul.s $f10, $f8, $f6
/* 0BBFD0 800BB3D0 E7A40090 */  swc1  $f4, 0x90($sp)
/* 0BBFD4 800BB3D4 AFAE0060 */  sw    $t6, 0x60($sp)
/* 0BBFD8 800BB3D8 10000005 */  b     .L800BB3F0
/* 0BBFDC 800BB3DC E7AA0094 */   swc1  $f10, 0x94($sp)
.L800BB3E0:
/* 0BBFE0 800BB3E0 8D2F0000 */  lw    $t7, ($t1)
/* 0BBFE4 800BB3E4 00000000 */  nop   
/* 0BBFE8 800BB3E8 25F80001 */  addiu $t8, $t7, 1
/* 0BBFEC 800BB3EC AFB80060 */  sw    $t8, 0x60($sp)
.L800BB3F0:
/* 0BBFF0 800BB3F0 84B90004 */  lh    $t9, 4($a1)
/* 0BBFF4 800BB3F4 C7A400AC */  lwc1  $f4, 0xac($sp)
/* 0BBFF8 800BB3F8 44994000 */  mtc1  $t9, $f8
/* 0BBFFC 800BB3FC 3C018013 */  lui   $at, %hi(D_8012A0A0) # $at, 0x8013
/* 0BC000 800BB400 468041A0 */  cvt.s.w $f6, $f8
/* 0BC004 800BB404 44804000 */  mtc1  $zero, $f8
/* 0BC008 800BB408 46062281 */  sub.s $f10, $f4, $f6
/* 0BC00C 800BB40C 4608503C */  c.lt.s $f10, $f8
/* 0BC010 800BB410 E7AA00AC */  swc1  $f10, 0xac($sp)
/* 0BC014 800BB414 45000004 */  bc1f  .L800BB428
/* 0BC018 800BB418 00000000 */   nop   
/* 0BC01C 800BB41C 44802000 */  mtc1  $zero, $f4
/* 0BC020 800BB420 1000000C */  b     .L800BB454
/* 0BC024 800BB424 E7A400AC */   swc1  $f4, 0xac($sp)
.L800BB428:
/* 0BC028 800BB428 C420A0A0 */  lwc1  $f0, %lo(D_8012A0A0)($at)
/* 0BC02C 800BB42C C7A600AC */  lwc1  $f6, 0xac($sp)
/* 0BC030 800BB430 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0BC034 800BB434 4606003E */  c.le.s $f0, $f6
/* 0BC038 800BB438 00000000 */  nop   
/* 0BC03C 800BB43C 45000005 */  bc1f  .L800BB454
/* 0BC040 800BB440 00000000 */   nop   
/* 0BC044 800BB444 44815000 */  mtc1  $at, $f10
/* 0BC048 800BB448 00000000 */  nop   
/* 0BC04C 800BB44C 460A0201 */  sub.s $f8, $f0, $f10
/* 0BC050 800BB450 E7A800AC */  swc1  $f8, 0xac($sp)
.L800BB454:
/* 0BC054 800BB454 84AE0008 */  lh    $t6, 8($a1)
/* 0BC058 800BB458 C7A400B0 */  lwc1  $f4, 0xb0($sp)
/* 0BC05C 800BB45C 448E3000 */  mtc1  $t6, $f6
/* 0BC060 800BB460 3C018013 */  lui   $at, %hi(D_8012A0A4) # $at, 0x8013
/* 0BC064 800BB464 468032A0 */  cvt.s.w $f10, $f6
/* 0BC068 800BB468 44803000 */  mtc1  $zero, $f6
/* 0BC06C 800BB46C 460A2201 */  sub.s $f8, $f4, $f10
/* 0BC070 800BB470 4606403C */  c.lt.s $f8, $f6
/* 0BC074 800BB474 E7A800B0 */  swc1  $f8, 0xb0($sp)
/* 0BC078 800BB478 45000004 */  bc1f  .L800BB48C
/* 0BC07C 800BB47C 00000000 */   nop   
/* 0BC080 800BB480 44802000 */  mtc1  $zero, $f4
/* 0BC084 800BB484 1000000C */  b     .L800BB4B8
/* 0BC088 800BB488 E7A400B0 */   swc1  $f4, 0xb0($sp)
.L800BB48C:
/* 0BC08C 800BB48C C420A0A4 */  lwc1  $f0, %lo(D_8012A0A4)($at)
/* 0BC090 800BB490 C7AA00B0 */  lwc1  $f10, 0xb0($sp)
/* 0BC094 800BB494 3C013F80 */  li    $at, 0x3F800000 # 1.000000
/* 0BC098 800BB498 460A003E */  c.le.s $f0, $f10
/* 0BC09C 800BB49C 00000000 */  nop   
/* 0BC0A0 800BB4A0 45000005 */  bc1f  .L800BB4B8
/* 0BC0A4 800BB4A4 00000000 */   nop   
/* 0BC0A8 800BB4A8 44814000 */  mtc1  $at, $f8
/* 0BC0AC 800BB4AC 00000000 */  nop   
/* 0BC0B0 800BB4B0 46080181 */  sub.s $f6, $f0, $f8
/* 0BC0B4 800BB4B4 E7A600B0 */  swc1  $f6, 0xb0($sp)
.L800BB4B8:
/* 0BC0B8 800BB4B8 C7A400AC */  lwc1  $f4, 0xac($sp)
/* 0BC0BC 800BB4BC C7AA0094 */  lwc1  $f10, 0x94($sp)
/* 0BC0C0 800BB4C0 E7A40018 */  swc1  $f4, 0x18($sp)
/* 0BC0C4 800BB4C4 460A2203 */  div.s $f8, $f4, $f10
/* 0BC0C8 800BB4C8 8D230004 */  lw    $v1, 4($t1)
/* 0BC0CC 800BB4CC 3C0A800E */  lui   $t2, 0x800e
/* 0BC0D0 800BB4D0 444FF800 */  cfc1  $t7, $31
/* 0BC0D4 800BB4D4 00000000 */  nop   
/* 0BC0D8 800BB4D8 35E10003 */  ori   $at, $t7, 3
/* 0BC0DC 800BB4DC 38210002 */  xori  $at, $at, 2
/* 0BC0E0 800BB4E0 44C1F800 */  ctc1  $at, $31
/* 0BC0E4 800BB4E4 00000000 */  nop   
/* 0BC0E8 800BB4E8 460041A4 */  cvt.w.s $f6, $f8
/* 0BC0EC 800BB4EC C7A800B0 */  lwc1  $f8, 0xb0($sp)
/* 0BC0F0 800BB4F0 44023000 */  mfc1  $v0, $f6
/* 0BC0F4 800BB4F4 44CFF800 */  ctc1  $t7, $31
/* 0BC0F8 800BB4F8 C7A60090 */  lwc1  $f6, 0x90($sp)
/* 0BC0FC 800BB4FC AFA20070 */  sw    $v0, 0x70($sp)
/* 0BC100 800BB500 46064103 */  div.s $f4, $f8, $f6
/* 0BC104 800BB504 4458F800 */  cfc1  $t8, $31
/* 0BC108 800BB508 00000000 */  nop   
/* 0BC10C 800BB50C 37010003 */  ori   $at, $t8, 3
/* 0BC110 800BB510 38210002 */  xori  $at, $at, 2
/* 0BC114 800BB514 44C1F800 */  ctc1  $at, $31
/* 0BC118 800BB518 00000000 */  nop   
/* 0BC11C 800BB51C 46002124 */  cvt.w.s $f4, $f4
/* 0BC120 800BB520 44042000 */  mfc1  $a0, $f4
/* 0BC124 800BB524 44822000 */  mtc1  $v0, $f4
/* 0BC128 800BB528 44D8F800 */  ctc1  $t8, $31
/* 0BC12C 800BB52C AFA4006C */  sw    $a0, 0x6c($sp)
/* 0BC130 800BB530 46802120 */  cvt.s.w $f4, $f4
/* 0BC134 800BB534 8FB8006C */  lw    $t8, 0x6c($sp)
/* 0BC138 800BB538 460A2102 */  mul.s $f4, $f4, $f10
/* 0BC13C 800BB53C C7AA0018 */  lwc1  $f10, 0x18($sp)
/* 0BC140 800BB540 00000000 */  nop   
/* 0BC144 800BB544 46045281 */  sub.s $f10, $f10, $f4
/* 0BC148 800BB548 44842000 */  mtc1  $a0, $f4
/* 0BC14C 800BB54C E7AA00AC */  swc1  $f10, 0xac($sp)
/* 0BC150 800BB550 468022A0 */  cvt.s.w $f10, $f4
/* 0BC154 800BB554 46065102 */  mul.s $f4, $f10, $f6
/* 0BC158 800BB558 46044281 */  sub.s $f10, $f8, $f4
/* 0BC15C 800BB55C C7A80090 */  lwc1  $f8, 0x90($sp)
/* 0BC160 800BB560 E7AA00B0 */  swc1  $f10, 0xb0($sp)
/* 0BC164 800BB564 84B90012 */  lh    $t9, 0x12($a1)
/* 0BC168 800BB568 C7A600B0 */  lwc1  $f6, 0xb0($sp)
/* 0BC16C 800BB56C 03224021 */  addu  $t0, $t9, $v0
/* 0BC170 800BB570 0103082A */  slt   $at, $t0, $v1
/* 0BC174 800BB574 14200005 */  bnez  $at, .L800BB58C
/* 0BC178 800BB578 00000000 */   nop   
.L800BB57C:
/* 0BC17C 800BB57C 01034023 */  subu  $t0, $t0, $v1
/* 0BC180 800BB580 0103082A */  slt   $at, $t0, $v1
/* 0BC184 800BB584 1020FFFD */  beqz  $at, .L800BB57C
/* 0BC188 800BB588 00000000 */   nop   
.L800BB58C:
/* 0BC18C 800BB58C 84AF0010 */  lh    $t7, 0x10($a1)
/* 0BC190 800BB590 251F0001 */  addiu $ra, $t0, 1
/* 0BC194 800BB594 01F83821 */  addu  $a3, $t7, $t8
/* 0BC198 800BB598 00E3082A */  slt   $at, $a3, $v1
/* 0BC19C 800BB59C 14200005 */  bnez  $at, .L800BB5B4
/* 0BC1A0 800BB5A0 3C05800E */   lui   $a1, 0x800e
.L800BB5A4:
/* 0BC1A4 800BB5A4 00E33823 */  subu  $a3, $a3, $v1
/* 0BC1A8 800BB5A8 00E3082A */  slt   $at, $a3, $v1
/* 0BC1AC 800BB5AC 1020FFFD */  beqz  $at, .L800BB5A4
/* 0BC1B0 800BB5B0 00000000 */   nop   
.L800BB5B4:
/* 0BC1B4 800BB5B4 46083032 */  c.eq.s $f6, $f8
/* 0BC1B8 800BB5B8 00001025 */  move  $v0, $zero
/* 0BC1BC 800BB5BC 4501000C */  bc1t  .L800BB5F0
/* 0BC1C0 800BB5C0 03E3082A */   slt   $at, $ra, $v1
/* 0BC1C4 800BB5C4 46064281 */  sub.s $f10, $f8, $f6
/* 0BC1C8 800BB5C8 C7A400AC */  lwc1  $f4, 0xac($sp)
/* 0BC1CC 800BB5CC 46085183 */  div.s $f6, $f10, $f8
/* 0BC1D0 800BB5D0 C7AA0094 */  lwc1  $f10, 0x94($sp)
/* 0BC1D4 800BB5D4 00000000 */  nop   
/* 0BC1D8 800BB5D8 460A3202 */  mul.s $f8, $f6, $f10
/* 0BC1DC 800BB5DC 4608203C */  c.lt.s $f4, $f8
/* 0BC1E0 800BB5E0 00000000 */  nop   
/* 0BC1E4 800BB5E4 45000002 */  bc1f  .L800BB5F0
/* 0BC1E8 800BB5E8 00000000 */   nop   
/* 0BC1EC 800BB5EC 24020001 */  li    $v0, 1
.L800BB5F0:
/* 0BC1F0 800BB5F0 104000DF */  beqz  $v0, .L800BB970
/* 0BC1F4 800BB5F4 00000000 */   nop   
/* 0BC1F8 800BB5F8 00E30019 */  multu $a3, $v1
/* 0BC1FC 800BB5FC 3C0A800E */  lui   $t2, %hi(D_800E3044) # $t2, 0x800e
/* 0BC200 800BB600 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC204 800BB604 3C05800E */  lui   $a1, %hi(D_800E3040) # $a1, 0x800e
/* 0BC208 800BB608 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC20C 800BB60C C52E0040 */  lwc1  $f14, 0x40($t1)
/* 0BC210 800BB610 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC214 800BB614 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC218 800BB618 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC21C 800BB61C 0000C812 */  mflo  $t9
/* 0BC220 800BB620 03282021 */  addu  $a0, $t9, $t0
/* 0BC224 800BB624 00047080 */  sll   $t6, $a0, 2
/* 0BC228 800BB628 014E1021 */  addu  $v0, $t2, $t6
/* 0BC22C 800BB62C 844F0002 */  lh    $t7, 2($v0)
/* 0BC230 800BB630 844E0000 */  lh    $t6, ($v0)
/* 0BC234 800BB634 000FC080 */  sll   $t8, $t7, 2
/* 0BC238 800BB638 00B8C821 */  addu  $t9, $a1, $t8
/* 0BC23C 800BB63C 000E7880 */  sll   $t7, $t6, 2
/* 0BC240 800BB640 00AFC021 */  addu  $t8, $a1, $t7
/* 0BC244 800BB644 C70A0000 */  lwc1  $f10, ($t8)
/* 0BC248 800BB648 C7260000 */  lwc1  $f6, ($t9)
/* 0BC24C 800BB64C 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC250 800BB650 460A3100 */  add.s $f4, $f6, $f10
/* 0BC254 800BB654 460E2202 */  mul.s $f8, $f4, $f14
/* 0BC258 800BB658 19600016 */  blez  $t3, .L800BB6B4
/* 0BC25C 800BB65C E7A800A0 */   swc1  $f8, 0xa0($sp)
/* 0BC260 800BB660 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC264 800BB664 AFA70064 */  sw    $a3, 0x64($sp)
/* 0BC268 800BB668 AFA80068 */  sw    $t0, 0x68($sp)
/* 0BC26C 800BB66C 0C02FBF1 */  jal   func_800BEFC4
/* 0BC270 800BB670 E7B0008C */   swc1  $f16, 0x8c($sp)
/* 0BC274 800BB674 C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 0BC278 800BB678 3C038013 */  lui   $v1, %hi(D_80129FC8 + 4) # $v1, 0x8013
/* 0BC27C 800BB67C 46003280 */  add.s $f10, $f6, $f0
/* 0BC280 800BB680 3C05800E */  lui   $a1, %hi(D_800E3040) # $a1, 0x800e
/* 0BC284 800BB684 3C0A800E */  lui   $t2, %hi(D_800E3044) # $t2, 0x800e
/* 0BC288 800BB688 3C018013 */  lui   $at, %hi(D_80129FC8 + 64) # $at, 0x8013
/* 0BC28C 800BB68C 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC290 800BB690 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC294 800BB694 C42EA008 */  lwc1  $f14, %lo(D_80129FC8 + 64)($at)
/* 0BC298 800BB698 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC29C 800BB69C 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC2A0 800BB6A0 8C639FCC */  lw    $v1, %lo(D_80129FC8 + 4)($v1)
/* 0BC2A4 800BB6A4 8FA70064 */  lw    $a3, 0x64($sp)
/* 0BC2A8 800BB6A8 8FA80068 */  lw    $t0, 0x68($sp)
/* 0BC2AC 800BB6AC C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC2B0 800BB6B0 E7AA00A0 */  swc1  $f10, 0xa0($sp)
.L800BB6B4:
/* 0BC2B4 800BB6B4 8FAF006C */  lw    $t7, 0x6c($sp)
/* 0BC2B8 800BB6B8 8FB80060 */  lw    $t8, 0x60($sp)
/* 0BC2BC 800BB6BC 8FB90058 */  lw    $t9, 0x58($sp)
/* 0BC2C0 800BB6C0 01F80019 */  multu $t7, $t8
/* 0BC2C4 800BB6C4 8FAE0070 */  lw    $t6, 0x70($sp)
/* 0BC2C8 800BB6C8 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC2CC 800BB6CC 032E6821 */  addu  $t5, $t9, $t6
/* 0BC2D0 800BB6D0 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC2D4 800BB6D4 24E90001 */  addiu $t1, $a3, 1
/* 0BC2D8 800BB6D8 0000C812 */  mflo  $t9
/* 0BC2DC 800BB6DC 01B9F821 */  addu  $ra, $t5, $t9
/* 0BC2E0 800BB6E0 019F7021 */  addu  $t6, $t4, $ra
/* 0BC2E4 800BB6E4 91C20000 */  lbu   $v0, ($t6)
/* 0BC2E8 800BB6E8 00000000 */  nop   
/* 0BC2EC 800BB6EC 2841007F */  slti  $at, $v0, 0x7f
/* 0BC2F0 800BB6F0 1020000B */  beqz  $at, .L800BB720
/* 0BC2F4 800BB6F4 0123082A */   slt   $at, $t1, $v1
/* 0BC2F8 800BB6F8 44824000 */  mtc1  $v0, $f8
/* 0BC2FC 800BB6FC 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC300 800BB700 468041A0 */  cvt.s.w $f6, $f8
/* 0BC304 800BB704 C424A00C */  lwc1  $f4, %lo(D_80129FC8 + 68)($at)
/* 0BC308 800BB708 46103282 */  mul.s $f10, $f6, $f16
/* 0BC30C 800BB70C C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 0BC310 800BB710 460A2200 */  add.s $f8, $f4, $f10
/* 0BC314 800BB714 46083102 */  mul.s $f4, $f6, $f8
/* 0BC318 800BB718 E7A400A0 */  swc1  $f4, 0xa0($sp)
/* 0BC31C 800BB71C 0123082A */  slt   $at, $t1, $v1
.L800BB720:
/* 0BC320 800BB720 14200003 */  bnez  $at, .L800BB730
/* 0BC324 800BB724 00000000 */   nop   
/* 0BC328 800BB728 10000005 */  b     .L800BB740
/* 0BC32C 800BB72C 01002025 */   move  $a0, $t0
.L800BB730:
/* 0BC330 800BB730 01230019 */  multu $t1, $v1
/* 0BC334 800BB734 00007812 */  mflo  $t7
/* 0BC338 800BB738 01E82021 */  addu  $a0, $t7, $t0
/* 0BC33C 800BB73C 00000000 */  nop   
.L800BB740:
/* 0BC340 800BB740 0004C080 */  sll   $t8, $a0, 2
/* 0BC344 800BB744 01581021 */  addu  $v0, $t2, $t8
/* 0BC348 800BB748 84590002 */  lh    $t9, 2($v0)
/* 0BC34C 800BB74C 84580000 */  lh    $t8, ($v0)
/* 0BC350 800BB750 00197080 */  sll   $t6, $t9, 2
/* 0BC354 800BB754 00AE7821 */  addu  $t7, $a1, $t6
/* 0BC358 800BB758 0018C880 */  sll   $t9, $t8, 2
/* 0BC35C 800BB75C 00B97021 */  addu  $t6, $a1, $t9
/* 0BC360 800BB760 C5C60000 */  lwc1  $f6, ($t6)
/* 0BC364 800BB764 C5EA0000 */  lwc1  $f10, ($t7)
/* 0BC368 800BB768 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC36C 800BB76C 46065200 */  add.s $f8, $f10, $f6
/* 0BC370 800BB770 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC374 800BB774 460E4302 */  mul.s $f12, $f8, $f14
/* 0BC378 800BB778 1960001C */  blez  $t3, .L800BB7EC
/* 0BC37C 800BB77C 24C60001 */   addiu $a2, $a2, 1
/* 0BC380 800BB780 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC384 800BB784 AFBF0034 */  sw    $ra, 0x34($sp)
/* 0BC388 800BB788 AFA70064 */  sw    $a3, 0x64($sp)
/* 0BC38C 800BB78C AFA80068 */  sw    $t0, 0x68($sp)
/* 0BC390 800BB790 AFAD0038 */  sw    $t5, 0x38($sp)
/* 0BC394 800BB794 E7AC009C */  swc1  $f12, 0x9c($sp)
/* 0BC398 800BB798 0C02FBF1 */  jal   func_800BEFC4
/* 0BC39C 800BB79C E7B0008C */   swc1  $f16, 0x8c($sp)
/* 0BC3A0 800BB7A0 C7AC009C */  lwc1  $f12, 0x9c($sp)
/* 0BC3A4 800BB7A4 3C038013 */  lui   $v1, %hi(D_80129FC8 + 4) # $v1, 0x8013
/* 0BC3A8 800BB7A8 3C05800E */  lui   $a1, %hi(D_800E3040) # $a1, 0x800e
/* 0BC3AC 800BB7AC 3C0A800E */  lui   $t2, %hi(D_800E3044) # $t2, 0x800e
/* 0BC3B0 800BB7B0 3C018013 */  lui   $at, %hi(D_80129FC8 + 64) # $at, 0x8013
/* 0BC3B4 800BB7B4 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC3B8 800BB7B8 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC3BC 800BB7BC 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC3C0 800BB7C0 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC3C4 800BB7C4 C42EA008 */  lwc1  $f14, %lo(D_80129FC8 + 64)($at)
/* 0BC3C8 800BB7C8 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC3CC 800BB7CC 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC3D0 800BB7D0 8C639FCC */  lw    $v1, %lo(D_80129FC8 + 4)($v1)
/* 0BC3D4 800BB7D4 8FA70064 */  lw    $a3, 0x64($sp)
/* 0BC3D8 800BB7D8 8FA80068 */  lw    $t0, 0x68($sp)
/* 0BC3DC 800BB7DC 8FAD0038 */  lw    $t5, 0x38($sp)
/* 0BC3E0 800BB7E0 8FBF0034 */  lw    $ra, 0x34($sp)
/* 0BC3E4 800BB7E4 C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC3E8 800BB7E8 46006300 */  add.s $f12, $f12, $f0
.L800BB7EC:
/* 0BC3EC 800BB7EC 8FAF006C */  lw    $t7, 0x6c($sp)
/* 0BC3F0 800BB7F0 8FB90060 */  lw    $t9, 0x60($sp)
/* 0BC3F4 800BB7F4 25F80001 */  addiu $t8, $t7, 1
/* 0BC3F8 800BB7F8 03190019 */  multu $t8, $t9
/* 0BC3FC 800BB7FC 25190001 */  addiu $t9, $t0, 1
/* 0BC400 800BB800 00007012 */  mflo  $t6
/* 0BC404 800BB804 01AE7821 */  addu  $t7, $t5, $t6
/* 0BC408 800BB808 018FC021 */  addu  $t8, $t4, $t7
/* 0BC40C 800BB80C 00E30019 */  multu $a3, $v1
/* 0BC410 800BB810 93020000 */  lbu   $v0, ($t8)
/* 0BC414 800BB814 00000000 */  nop   
/* 0BC418 800BB818 2841007F */  slti  $at, $v0, 0x7f
/* 0BC41C 800BB81C 00003012 */  mflo  $a2
/* 0BC420 800BB820 01062021 */  addu  $a0, $t0, $a2
/* 0BC424 800BB824 10200009 */  beqz  $at, .L800BB84C
/* 0BC428 800BB828 24840001 */   addiu $a0, $a0, 1
/* 0BC42C 800BB82C 44825000 */  mtc1  $v0, $f10
/* 0BC430 800BB830 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC434 800BB834 468051A0 */  cvt.s.w $f6, $f10
/* 0BC438 800BB838 C424A00C */  lwc1  $f4, %lo(D_80129FC8 + 68)($at)
/* 0BC43C 800BB83C 46103202 */  mul.s $f8, $f6, $f16
/* 0BC440 800BB840 46082280 */  add.s $f10, $f4, $f8
/* 0BC444 800BB844 460A6302 */  mul.s $f12, $f12, $f10
/* 0BC448 800BB848 00000000 */  nop   
.L800BB84C:
/* 0BC44C 800BB84C 0323082A */  slt   $at, $t9, $v1
/* 0BC450 800BB850 14200003 */  bnez  $at, .L800BB860
/* 0BC454 800BB854 00000000 */   nop   
/* 0BC458 800BB858 10000001 */  b     .L800BB860
/* 0BC45C 800BB85C 00C02025 */   move  $a0, $a2
.L800BB860:
/* 0BC460 800BB860 00047080 */  sll   $t6, $a0, 2
/* 0BC464 800BB864 014E1021 */  addu  $v0, $t2, $t6
/* 0BC468 800BB868 844F0002 */  lh    $t7, 2($v0)
/* 0BC46C 800BB86C 844E0000 */  lh    $t6, ($v0)
/* 0BC470 800BB870 000FC080 */  sll   $t8, $t7, 2
/* 0BC474 800BB874 00B8C821 */  addu  $t9, $a1, $t8
/* 0BC478 800BB878 000E7880 */  sll   $t7, $t6, 2
/* 0BC47C 800BB87C 00AFC021 */  addu  $t8, $a1, $t7
/* 0BC480 800BB880 C7040000 */  lwc1  $f4, ($t8)
/* 0BC484 800BB884 C7260000 */  lwc1  $f6, ($t9)
/* 0BC488 800BB888 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC48C 800BB88C 46043200 */  add.s $f8, $f6, $f4
/* 0BC490 800BB890 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC494 800BB894 460E4082 */  mul.s $f2, $f8, $f14
/* 0BC498 800BB898 1960000E */  blez  $t3, .L800BB8D4
/* 0BC49C 800BB89C 24A50001 */   addiu $a1, $a1, 1
/* 0BC4A0 800BB8A0 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC4A4 800BB8A4 AFBF0034 */  sw    $ra, 0x34($sp)
/* 0BC4A8 800BB8A8 E7A20098 */  swc1  $f2, 0x98($sp)
/* 0BC4AC 800BB8AC E7AC009C */  swc1  $f12, 0x9c($sp)
/* 0BC4B0 800BB8B0 0C02FBF1 */  jal   func_800BEFC4
/* 0BC4B4 800BB8B4 E7B0008C */   swc1  $f16, 0x8c($sp)
/* 0BC4B8 800BB8B8 C7A20098 */  lwc1  $f2, 0x98($sp)
/* 0BC4BC 800BB8BC 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC4C0 800BB8C0 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC4C4 800BB8C4 8FBF0034 */  lw    $ra, 0x34($sp)
/* 0BC4C8 800BB8C8 C7AC009C */  lwc1  $f12, 0x9c($sp)
/* 0BC4CC 800BB8CC C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC4D0 800BB8D0 46001080 */  add.s $f2, $f2, $f0
.L800BB8D4:
/* 0BC4D4 800BB8D4 019FC821 */  addu  $t9, $t4, $ra
/* 0BC4D8 800BB8D8 93220001 */  lbu   $v0, 1($t9)
/* 0BC4DC 800BB8DC 3C038013 */  lui   $v1, %hi(D_80129FC8 + 40) # $v1, 0x8013
/* 0BC4E0 800BB8E0 8C639FF0 */  lw    $v1, %lo(D_80129FC8 + 40)($v1)
/* 0BC4E4 800BB8E4 2841007F */  slti  $at, $v0, 0x7f
/* 0BC4E8 800BB8E8 10200009 */  beqz  $at, .L800BB910
/* 0BC4EC 800BB8EC 00000000 */   nop   
/* 0BC4F0 800BB8F0 44823000 */  mtc1  $v0, $f6
/* 0BC4F4 800BB8F4 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC4F8 800BB8F8 46803120 */  cvt.s.w $f4, $f6
/* 0BC4FC 800BB8FC C42AA00C */  lwc1  $f10, %lo(D_80129FC8 + 68)($at)
/* 0BC500 800BB900 46102202 */  mul.s $f8, $f4, $f16
/* 0BC504 800BB904 46085180 */  add.s $f6, $f10, $f8
/* 0BC508 800BB908 46061082 */  mul.s $f2, $f2, $f6
/* 0BC50C 800BB90C 00000000 */  nop   
.L800BB910:
/* 0BC510 800BB910 1060000A */  beqz  $v1, .L800BB93C
/* 0BC514 800BB914 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 0BC518 800BB918 C7A400A0 */  lwc1  $f4, 0xa0($sp)
/* 0BC51C 800BB91C 44815000 */  mtc1  $at, $f10
/* 0BC520 800BB920 44813000 */  mtc1  $at, $f6
/* 0BC524 800BB924 460A2202 */  mul.s $f8, $f4, $f10
/* 0BC528 800BB928 44812000 */  mtc1  $at, $f4
/* 0BC52C 800BB92C 46066302 */  mul.s $f12, $f12, $f6
/* 0BC530 800BB930 E7A800A0 */  swc1  $f8, 0xa0($sp)
/* 0BC534 800BB934 46041082 */  mul.s $f2, $f2, $f4
/* 0BC538 800BB938 00000000 */  nop   
.L800BB93C:
/* 0BC53C 800BB93C C7A800A0 */  lwc1  $f8, 0xa0($sp)
/* 0BC540 800BB940 44805000 */  mtc1  $zero, $f10
/* 0BC544 800BB944 C7A40090 */  lwc1  $f4, 0x90($sp)
/* 0BC548 800BB948 46024181 */  sub.s $f6, $f8, $f2
/* 0BC54C 800BB94C E7AA00A4 */  swc1  $f10, 0xa4($sp)
/* 0BC550 800BB950 46043382 */  mul.s $f14, $f6, $f4
/* 0BC554 800BB954 C7AA0094 */  lwc1  $f10, 0x94($sp)
/* 0BC558 800BB958 00000000 */  nop   
/* 0BC55C 800BB95C 460A2402 */  mul.s $f16, $f4, $f10
/* 0BC560 800BB960 460C4181 */  sub.s $f6, $f8, $f12
/* 0BC564 800BB964 460A3482 */  mul.s $f18, $f6, $f10
/* 0BC568 800BB968 100000E5 */  b     .L800BBD00
/* 0BC56C 800BB96C E7B00084 */   swc1  $f16, 0x84($sp)
.L800BB970:
/* 0BC570 800BB970 14200005 */  bnez  $at, .L800BB988
/* 0BC574 800BB974 00000000 */   nop   
/* 0BC578 800BB978 00E30019 */  multu $a3, $v1
/* 0BC57C 800BB97C 00002012 */  mflo  $a0
/* 0BC580 800BB980 10000005 */  b     .L800BB998
/* 0BC584 800BB984 00000000 */   nop   
.L800BB988:
/* 0BC588 800BB988 00E30019 */  multu $a3, $v1
/* 0BC58C 800BB98C 00007012 */  mflo  $t6
/* 0BC590 800BB990 010E2021 */  addu  $a0, $t0, $t6
/* 0BC594 800BB994 24840001 */  addiu $a0, $a0, 1
.L800BB998:
/* 0BC598 800BB998 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC59C 800BB99C 00047880 */  sll   $t7, $a0, 2
/* 0BC5A0 800BB9A0 014F1021 */  addu  $v0, $t2, $t7
/* 0BC5A4 800BB9A4 84580002 */  lh    $t8, 2($v0)
/* 0BC5A8 800BB9A8 844F0000 */  lh    $t7, ($v0)
/* 0BC5AC 800BB9AC 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC5B0 800BB9B0 0018C880 */  sll   $t9, $t8, 2
/* 0BC5B4 800BB9B4 000FC080 */  sll   $t8, $t7, 2
/* 0BC5B8 800BB9B8 00B97021 */  addu  $t6, $a1, $t9
/* 0BC5BC 800BB9BC 00B8C821 */  addu  $t9, $a1, $t8
/* 0BC5C0 800BB9C0 C7280000 */  lwc1  $f8, ($t9)
/* 0BC5C4 800BB9C4 C5C40000 */  lwc1  $f4, ($t6)
/* 0BC5C8 800BB9C8 C52E0040 */  lwc1  $f14, 0x40($t1)
/* 0BC5CC 800BB9CC 46082180 */  add.s $f6, $f4, $f8
/* 0BC5D0 800BB9D0 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC5D4 800BB9D4 460E3282 */  mul.s $f10, $f6, $f14
/* 0BC5D8 800BB9D8 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC5DC 800BB9DC 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC5E0 800BB9E0 1960001A */  blez  $t3, .L800BBA4C
/* 0BC5E4 800BB9E4 E7AA00A0 */   swc1  $f10, 0xa0($sp)
/* 0BC5E8 800BB9E8 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC5EC 800BB9EC 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC5F0 800BB9F0 AFBF0024 */  sw    $ra, 0x24($sp)
/* 0BC5F4 800BB9F4 AFA70064 */  sw    $a3, 0x64($sp)
/* 0BC5F8 800BB9F8 AFA80068 */  sw    $t0, 0x68($sp)
/* 0BC5FC 800BB9FC E7B0008C */  swc1  $f16, 0x8c($sp)
/* 0BC600 800BBA00 0C02FBF1 */  jal   func_800BEFC4
/* 0BC604 800BBA04 24A50001 */   addiu $a1, $a1, 1
/* 0BC608 800BBA08 C7A400A0 */  lwc1  $f4, 0xa0($sp)
/* 0BC60C 800BBA0C 3C038013 */  lui   $v1, %hi(D_80129FC8 + 4) # $v1, 0x8013
/* 0BC610 800BBA10 46002200 */  add.s $f8, $f4, $f0
/* 0BC614 800BBA14 3C05800E */  lui   $a1, %hi(D_800E3040) # $a1, 0x800e
/* 0BC618 800BBA18 3C0A800E */  lui   $t2, %hi(D_800E3044) # $t2, 0x800e
/* 0BC61C 800BBA1C 3C018013 */  lui   $at, %hi(D_80129FC8 + 64) # $at, 0x8013
/* 0BC620 800BBA20 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC624 800BBA24 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC628 800BBA28 C42EA008 */  lwc1  $f14, %lo(D_80129FC8 + 64)($at)
/* 0BC62C 800BBA2C 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC630 800BBA30 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC634 800BBA34 8C639FCC */  lw    $v1, %lo(D_80129FC8 + 4)($v1)
/* 0BC638 800BBA38 8FA70064 */  lw    $a3, 0x64($sp)
/* 0BC63C 800BBA3C 8FA80068 */  lw    $t0, 0x68($sp)
/* 0BC640 800BBA40 8FBF0024 */  lw    $ra, 0x24($sp)
/* 0BC644 800BBA44 C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC648 800BBA48 E7A800A0 */  swc1  $f8, 0xa0($sp)
.L800BBA4C:
/* 0BC64C 800BBA4C 8FB8006C */  lw    $t8, 0x6c($sp)
/* 0BC650 800BBA50 8FB90060 */  lw    $t9, 0x60($sp)
/* 0BC654 800BBA54 8FAE0058 */  lw    $t6, 0x58($sp)
/* 0BC658 800BBA58 03190019 */  multu $t8, $t9
/* 0BC65C 800BBA5C 8FAF0070 */  lw    $t7, 0x70($sp)
/* 0BC660 800BBA60 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC664 800BBA64 01CF6821 */  addu  $t5, $t6, $t7
/* 0BC668 800BBA68 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC66C 800BBA6C 24E90001 */  addiu $t1, $a3, 1
/* 0BC670 800BBA70 00007012 */  mflo  $t6
/* 0BC674 800BBA74 01AE7821 */  addu  $t7, $t5, $t6
/* 0BC678 800BBA78 018FC021 */  addu  $t8, $t4, $t7
/* 0BC67C 800BBA7C 93020001 */  lbu   $v0, 1($t8)
/* 0BC680 800BBA80 00000000 */  nop   
/* 0BC684 800BBA84 2841007F */  slti  $at, $v0, 0x7f
/* 0BC688 800BBA88 1020000B */  beqz  $at, .L800BBAB8
/* 0BC68C 800BBA8C 0123082A */   slt   $at, $t1, $v1
/* 0BC690 800BBA90 44825000 */  mtc1  $v0, $f10
/* 0BC694 800BBA94 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC698 800BBA98 46805120 */  cvt.s.w $f4, $f10
/* 0BC69C 800BBA9C C426A00C */  lwc1  $f6, %lo(D_80129FC8 + 68)($at)
/* 0BC6A0 800BBAA0 46102202 */  mul.s $f8, $f4, $f16
/* 0BC6A4 800BBAA4 C7A400A0 */  lwc1  $f4, 0xa0($sp)
/* 0BC6A8 800BBAA8 46083280 */  add.s $f10, $f6, $f8
/* 0BC6AC 800BBAAC 460A2182 */  mul.s $f6, $f4, $f10
/* 0BC6B0 800BBAB0 E7A600A0 */  swc1  $f6, 0xa0($sp)
/* 0BC6B4 800BBAB4 0123082A */  slt   $at, $t1, $v1
.L800BBAB8:
/* 0BC6B8 800BBAB8 14200003 */  bnez  $at, .L800BBAC8
/* 0BC6BC 800BBABC 00000000 */   nop   
/* 0BC6C0 800BBAC0 10000005 */  b     .L800BBAD8
/* 0BC6C4 800BBAC4 01002025 */   move  $a0, $t0
.L800BBAC8:
/* 0BC6C8 800BBAC8 01230019 */  multu $t1, $v1
/* 0BC6CC 800BBACC 0000C812 */  mflo  $t9
/* 0BC6D0 800BBAD0 03282021 */  addu  $a0, $t9, $t0
/* 0BC6D4 800BBAD4 00000000 */  nop   
.L800BBAD8:
/* 0BC6D8 800BBAD8 00047080 */  sll   $t6, $a0, 2
/* 0BC6DC 800BBADC 014E1021 */  addu  $v0, $t2, $t6
/* 0BC6E0 800BBAE0 844F0002 */  lh    $t7, 2($v0)
/* 0BC6E4 800BBAE4 844E0000 */  lh    $t6, ($v0)
/* 0BC6E8 800BBAE8 000FC080 */  sll   $t8, $t7, 2
/* 0BC6EC 800BBAEC 00B8C821 */  addu  $t9, $a1, $t8
/* 0BC6F0 800BBAF0 000E7880 */  sll   $t7, $t6, 2
/* 0BC6F4 800BBAF4 00AFC021 */  addu  $t8, $a1, $t7
/* 0BC6F8 800BBAF8 C7040000 */  lwc1  $f4, ($t8)
/* 0BC6FC 800BBAFC C7280000 */  lwc1  $f8, ($t9)
/* 0BC700 800BBB00 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC704 800BBB04 46044280 */  add.s $f10, $f8, $f4
/* 0BC708 800BBB08 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC70C 800BBB0C 460E5302 */  mul.s $f12, $f10, $f14
/* 0BC710 800BBB10 1960001A */  blez  $t3, .L800BBB7C
/* 0BC714 800BBB14 24C60001 */   addiu $a2, $a2, 1
/* 0BC718 800BBB18 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC71C 800BBB1C AFBF0024 */  sw    $ra, 0x24($sp)
/* 0BC720 800BBB20 AFA9004C */  sw    $t1, 0x4c($sp)
/* 0BC724 800BBB24 AFAD0038 */  sw    $t5, 0x38($sp)
/* 0BC728 800BBB28 E7AC009C */  swc1  $f12, 0x9c($sp)
/* 0BC72C 800BBB2C 0C02FBF1 */  jal   func_800BEFC4
/* 0BC730 800BBB30 E7B0008C */   swc1  $f16, 0x8c($sp)
/* 0BC734 800BBB34 C7AC009C */  lwc1  $f12, 0x9c($sp)
/* 0BC738 800BBB38 3C038013 */  lui   $v1, %hi(D_80129FC8 + 4) # $v1, 0x8013
/* 0BC73C 800BBB3C 3C05800E */  lui   $a1, %hi(D_800E3040) # $a1, 0x800e
/* 0BC740 800BBB40 3C0A800E */  lui   $t2, %hi(D_800E3044) # $t2, 0x800e
/* 0BC744 800BBB44 3C018013 */  lui   $at, %hi(D_80129FC8 + 64) # $at, 0x8013
/* 0BC748 800BBB48 3C0B800E */  lui   $t3, %hi(D_800E3188) # $t3, 0x800e
/* 0BC74C 800BBB4C 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC750 800BBB50 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC754 800BBB54 8D6B3188 */  lw    $t3, %lo(D_800E3188)($t3)
/* 0BC758 800BBB58 C42EA008 */  lwc1  $f14, %lo(D_80129FC8 + 64)($at)
/* 0BC75C 800BBB5C 8D4A3044 */  lw    $t2, %lo(D_800E3044)($t2)
/* 0BC760 800BBB60 8CA53040 */  lw    $a1, %lo(D_800E3040)($a1)
/* 0BC764 800BBB64 8C639FCC */  lw    $v1, %lo(D_80129FC8 + 4)($v1)
/* 0BC768 800BBB68 8FA9004C */  lw    $t1, 0x4c($sp)
/* 0BC76C 800BBB6C 8FAD0038 */  lw    $t5, 0x38($sp)
/* 0BC770 800BBB70 8FBF0024 */  lw    $ra, 0x24($sp)
/* 0BC774 800BBB74 C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC778 800BBB78 46006300 */  add.s $f12, $f12, $f0
.L800BBB7C:
/* 0BC77C 800BBB7C 8FA6006C */  lw    $a2, 0x6c($sp)
/* 0BC780 800BBB80 8FB90060 */  lw    $t9, 0x60($sp)
/* 0BC784 800BBB84 24C60001 */  addiu $a2, $a2, 1
/* 0BC788 800BBB88 00D90019 */  multu $a2, $t9
/* 0BC78C 800BBB8C 03E02025 */  move  $a0, $ra
/* 0BC790 800BBB90 00007012 */  mflo  $t6
/* 0BC794 800BBB94 01AE3821 */  addu  $a3, $t5, $t6
/* 0BC798 800BBB98 01877821 */  addu  $t7, $t4, $a3
/* 0BC79C 800BBB9C 91E20000 */  lbu   $v0, ($t7)
/* 0BC7A0 800BBBA0 00000000 */  nop   
/* 0BC7A4 800BBBA4 2841007F */  slti  $at, $v0, 0x7f
/* 0BC7A8 800BBBA8 1020000A */  beqz  $at, .L800BBBD4
/* 0BC7AC 800BBBAC 03E3082A */   slt   $at, $ra, $v1
/* 0BC7B0 800BBBB0 44824000 */  mtc1  $v0, $f8
/* 0BC7B4 800BBBB4 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC7B8 800BBBB8 46804120 */  cvt.s.w $f4, $f8
/* 0BC7BC 800BBBBC C426A00C */  lwc1  $f6, %lo(D_80129FC8 + 68)($at)
/* 0BC7C0 800BBBC0 46102282 */  mul.s $f10, $f4, $f16
/* 0BC7C4 800BBBC4 460A3200 */  add.s $f8, $f6, $f10
/* 0BC7C8 800BBBC8 46086302 */  mul.s $f12, $f12, $f8
/* 0BC7CC 800BBBCC 00000000 */  nop   
/* 0BC7D0 800BBBD0 03E3082A */  slt   $at, $ra, $v1
.L800BBBD4:
/* 0BC7D4 800BBBD4 14200003 */  bnez  $at, .L800BBBE4
/* 0BC7D8 800BBBD8 00000000 */   nop   
/* 0BC7DC 800BBBDC 10000001 */  b     .L800BBBE4
/* 0BC7E0 800BBBE0 00002025 */   move  $a0, $zero
.L800BBBE4:
/* 0BC7E4 800BBBE4 0123082A */  slt   $at, $t1, $v1
/* 0BC7E8 800BBBE8 10200006 */  beqz  $at, .L800BBC04
/* 0BC7EC 800BBBEC 0004C880 */   sll   $t9, $a0, 2
/* 0BC7F0 800BBBF0 01230019 */  multu $t1, $v1
/* 0BC7F4 800BBBF4 0000C012 */  mflo  $t8
/* 0BC7F8 800BBBF8 00982021 */  addu  $a0, $a0, $t8
/* 0BC7FC 800BBBFC 00000000 */  nop   
/* 0BC800 800BBC00 0004C880 */  sll   $t9, $a0, 2
.L800BBC04:
/* 0BC804 800BBC04 01591021 */  addu  $v0, $t2, $t9
/* 0BC808 800BBC08 844E0002 */  lh    $t6, 2($v0)
/* 0BC80C 800BBC0C 84590000 */  lh    $t9, ($v0)
/* 0BC810 800BBC10 000E7880 */  sll   $t7, $t6, 2
/* 0BC814 800BBC14 00AFC021 */  addu  $t8, $a1, $t7
/* 0BC818 800BBC18 00197080 */  sll   $t6, $t9, 2
/* 0BC81C 800BBC1C 00AE7821 */  addu  $t7, $a1, $t6
/* 0BC820 800BBC20 C5E60000 */  lwc1  $f6, ($t7)
/* 0BC824 800BBC24 C7040000 */  lwc1  $f4, ($t8)
/* 0BC828 800BBC28 8FA50070 */  lw    $a1, 0x70($sp)
/* 0BC82C 800BBC2C 46062280 */  add.s $f10, $f4, $f6
/* 0BC830 800BBC30 8FA400A8 */  lw    $a0, 0xa8($sp)
/* 0BC834 800BBC34 460E5082 */  mul.s $f2, $f10, $f14
/* 0BC838 800BBC38 1960000D */  blez  $t3, .L800BBC70
/* 0BC83C 800BBC3C 24A50001 */   addiu $a1, $a1, 1
/* 0BC840 800BBC40 AFA70028 */  sw    $a3, 0x28($sp)
/* 0BC844 800BBC44 E7A20098 */  swc1  $f2, 0x98($sp)
/* 0BC848 800BBC48 E7AC009C */  swc1  $f12, 0x9c($sp)
/* 0BC84C 800BBC4C 0C02FBF1 */  jal   func_800BEFC4
/* 0BC850 800BBC50 E7B0008C */   swc1  $f16, 0x8c($sp)
/* 0BC854 800BBC54 C7A20098 */  lwc1  $f2, 0x98($sp)
/* 0BC858 800BBC58 3C0C800E */  lui   $t4, %hi(D_800E3178) # $t4, 0x800e
/* 0BC85C 800BBC5C 8D8C3178 */  lw    $t4, %lo(D_800E3178)($t4)
/* 0BC860 800BBC60 8FA70028 */  lw    $a3, 0x28($sp)
/* 0BC864 800BBC64 C7AC009C */  lwc1  $f12, 0x9c($sp)
/* 0BC868 800BBC68 C7B0008C */  lwc1  $f16, 0x8c($sp)
/* 0BC86C 800BBC6C 46001080 */  add.s $f2, $f2, $f0
.L800BBC70:
/* 0BC870 800BBC70 0187C021 */  addu  $t8, $t4, $a3
/* 0BC874 800BBC74 93020001 */  lbu   $v0, 1($t8)
/* 0BC878 800BBC78 3C038013 */  lui   $v1, %hi(D_80129FC8 + 40) # $v1, 0x8013
/* 0BC87C 800BBC7C 8C639FF0 */  lw    $v1, %lo(D_80129FC8 + 40)($v1)
/* 0BC880 800BBC80 2841007F */  slti  $at, $v0, 0x7f
/* 0BC884 800BBC84 10200009 */  beqz  $at, .L800BBCAC
/* 0BC888 800BBC88 00000000 */   nop   
/* 0BC88C 800BBC8C 44822000 */  mtc1  $v0, $f4
/* 0BC890 800BBC90 3C018013 */  lui   $at, %hi(D_80129FC8 + 68) # $at, 0x8013
/* 0BC894 800BBC94 468021A0 */  cvt.s.w $f6, $f4
/* 0BC898 800BBC98 C428A00C */  lwc1  $f8, %lo(D_80129FC8 + 68)($at)
/* 0BC89C 800BBC9C 46103282 */  mul.s $f10, $f6, $f16
/* 0BC8A0 800BBCA0 460A4100 */  add.s $f4, $f8, $f10
/* 0BC8A4 800BBCA4 46041082 */  mul.s $f2, $f2, $f4
/* 0BC8A8 800BBCA8 00000000 */  nop   
.L800BBCAC:
/* 0BC8AC 800BBCAC 1060000A */  beqz  $v1, .L800BBCD8
/* 0BC8B0 800BBCB0 3C013F00 */   li    $at, 0x3F000000 # 0.500000
/* 0BC8B4 800BBCB4 C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 0BC8B8 800BBCB8 44814000 */  mtc1  $at, $f8
/* 0BC8BC 800BBCBC 44812000 */  mtc1  $at, $f4
/* 0BC8C0 800BBCC0 46083282 */  mul.s $f10, $f6, $f8
/* 0BC8C4 800BBCC4 44813000 */  mtc1  $at, $f6
/* 0BC8C8 800BBCC8 46046302 */  mul.s $f12, $f12, $f4
/* 0BC8CC 800BBCCC E7AA00A0 */  swc1  $f10, 0xa0($sp)
/* 0BC8D0 800BBCD0 46061082 */  mul.s $f2, $f2, $f6
/* 0BC8D4 800BBCD4 00000000 */  nop   
.L800BBCD8:
/* 0BC8D8 800BBCD8 46026281 */  sub.s $f10, $f12, $f2
/* 0BC8DC 800BBCDC C7A40090 */  lwc1  $f4, 0x90($sp)
/* 0BC8E0 800BBCE0 C7A80094 */  lwc1  $f8, 0x94($sp)
/* 0BC8E4 800BBCE4 46045382 */  mul.s $f14, $f10, $f4
/* 0BC8E8 800BBCE8 C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 0BC8EC 800BBCEC E7A800A4 */  swc1  $f8, 0xa4($sp)
/* 0BC8F0 800BBCF0 46082402 */  mul.s $f16, $f4, $f8
/* 0BC8F4 800BBCF4 46023281 */  sub.s $f10, $f6, $f2
/* 0BC8F8 800BBCF8 46085482 */  mul.s $f18, $f10, $f8
/* 0BC8FC 800BBCFC E7B00084 */  swc1  $f16, 0x84($sp)
.L800BBD00:
/* 0BC900 800BBD00 460E7102 */  mul.s $f4, $f14, $f14
/* 0BC904 800BBD04 E7AE0088 */  swc1  $f14, 0x88($sp)
/* 0BC908 800BBD08 E7B0004C */  swc1  $f16, 0x4c($sp)
/* 0BC90C 800BBD0C E7B20080 */  swc1  $f18, 0x80($sp)
/* 0BC910 800BBD10 46108182 */  mul.s $f6, $f16, $f16
/* 0BC914 800BBD14 00000000 */  nop   
/* 0BC918 800BBD18 46129202 */  mul.s $f8, $f18, $f18
/* 0BC91C 800BBD1C 46062280 */  add.s $f10, $f4, $f6
/* 0BC920 800BBD20 0C0326B4 */  jal   sqrtf
/* 0BC924 800BBD24 46085300 */   add.s $f12, $f10, $f8
/* 0BC928 800BBD28 44801800 */  mtc1  $zero, $f3
/* 0BC92C 800BBD2C 44801000 */  mtc1  $zero, $f2
/* 0BC930 800BBD30 46000121 */  cvt.d.s $f4, $f0
/* 0BC934 800BBD34 46241032 */  c.eq.d $f2, $f4
/* 0BC938 800BBD38 C7AE0088 */  lwc1  $f14, 0x88($sp)
/* 0BC93C 800BBD3C C7B0004C */  lwc1  $f16, 0x4c($sp)
/* 0BC940 800BBD40 C7B20080 */  lwc1  $f18, 0x80($sp)
/* 0BC944 800BBD44 4501001B */  bc1t  .L800BBDB4
/* 0BC948 800BBD48 8FB900B4 */   lw    $t9, 0xb4($sp)
/* 0BC94C 800BBD4C 460081A1 */  cvt.d.s $f6, $f16
/* 0BC950 800BBD50 46261032 */  c.eq.d $f2, $f6
/* 0BC954 800BBD54 00000000 */  nop   
/* 0BC958 800BBD58 45010015 */  bc1t  .L800BBDB0
/* 0BC95C 800BBD5C 00000000 */   nop   
/* 0BC960 800BBD60 46007383 */  div.s $f14, $f14, $f0
/* 0BC964 800BBD64 C7AA00AC */  lwc1  $f10, 0xac($sp)
/* 0BC968 800BBD68 C7A400B0 */  lwc1  $f4, 0xb0($sp)
/* 0BC96C 800BBD6C 46009483 */  div.s $f18, $f18, $f0
/* 0BC970 800BBD70 460A7202 */  mul.s $f8, $f14, $f10
/* 0BC974 800BBD74 00000000 */  nop   
/* 0BC978 800BBD78 46008083 */  div.s $f2, $f16, $f0
/* 0BC97C 800BBD7C 46049182 */  mul.s $f6, $f18, $f4
/* 0BC980 800BBD80 C7A400A4 */  lwc1  $f4, 0xa4($sp)
/* 0BC984 800BBD84 46064280 */  add.s $f10, $f8, $f6
/* 0BC988 800BBD88 C7A600A0 */  lwc1  $f6, 0xa0($sp)
/* 0BC98C 800BBD8C 460E2202 */  mul.s $f8, $f4, $f14
/* 0BC990 800BBD90 E7A20084 */  swc1  $f2, 0x84($sp)
/* 0BC994 800BBD94 46023102 */  mul.s $f4, $f6, $f2
/* 0BC998 800BBD98 46044180 */  add.s $f6, $f8, $f4
/* 0BC99C 800BBD9C 46065201 */  sub.s $f8, $f10, $f6
/* 0BC9A0 800BBDA0 C7AA0078 */  lwc1  $f10, 0x78($sp)
/* 0BC9A4 800BBDA4 46024103 */  div.s $f4, $f8, $f2
/* 0BC9A8 800BBDA8 46045181 */  sub.s $f6, $f10, $f4
/* 0BC9AC 800BBDAC E7A60078 */  swc1  $f6, 0x78($sp)
.L800BBDB0:
/* 0BC9B0 800BBDB0 8FB900B4 */  lw    $t9, 0xb4($sp)
.L800BBDB4:
/* 0BC9B4 800BBDB4 C7A20084 */  lwc1  $f2, 0x84($sp)
/* 0BC9B8 800BBDB8 13200005 */  beqz  $t9, .L800BBDD0
/* 0BC9BC 800BBDBC 8FBF0014 */   lw    $ra, 0x14($sp)
/* 0BC9C0 800BBDC0 E72E0000 */  swc1  $f14, ($t9)
/* 0BC9C4 800BBDC4 E7220004 */  swc1  $f2, 4($t9)
/* 0BC9C8 800BBDC8 E7320008 */  swc1  $f18, 8($t9)
/* 0BC9CC 800BBDCC 8FBF0014 */  lw    $ra, 0x14($sp)
.L800BBDD0:
/* 0BC9D0 800BBDD0 C7A00078 */  lwc1  $f0, 0x78($sp)
/* 0BC9D4 800BBDD4 03E00008 */  jr    $ra
/* 0BC9D8 800BBDD8 27BD00A8 */   addiu $sp, $sp, 0xa8

