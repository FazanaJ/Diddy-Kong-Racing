glabel func_80084854
/* 085454 80084854 27BDFFA8 */  addiu $sp, $sp, -0x58
/* 085458 80084858 3C058012 */  lui   $a1, %hi(gMusicTestString) # $a1, 0x8012
/* 08545C 8008485C 24A569E0 */  addiu $a1, %lo(gMusicTestString) # addiu $a1, $a1, 0x69e0
/* 085460 80084860 8CA30000 */  lw    $v1, ($a1)
/* 085464 80084864 AFBF003C */  sw    $ra, 0x3c($sp)
/* 085468 80084868 AFB30038 */  sw    $s3, 0x38($sp)
/* 08546C 8008486C AFB20034 */  sw    $s2, 0x34($sp)
/* 085470 80084870 AFB10030 */  sw    $s1, 0x30($sp)
/* 085474 80084874 AFB0002C */  sw    $s0, 0x2c($sp)
/* 085478 80084878 AFA40058 */  sw    $a0, 0x58($sp)
/* 08547C 8008487C 906E0000 */  lbu   $t6, ($v1)
/* 085480 80084880 3C13800E */  lui   $s3, %hi(D_800DFABC) # $s3, 0x800e
/* 085484 80084884 8E73FABC */  lw    $s3, %lo(D_800DFABC)($s3)
/* 085488 80084888 29C10030 */  slti  $at, $t6, 0x30
/* 08548C 8008488C 14200006 */  bnez  $at, .L800848A8
/* 085490 80084890 00008825 */   move  $s1, $zero
/* 085494 80084894 906F0000 */  lbu   $t7, ($v1)
/* 085498 80084898 00000000 */  nop   
/* 08549C 8008489C 29E1003A */  slti  $at, $t7, 0x3a
/* 0854A0 800848A0 1420000C */  bnez  $at, .L800848D4
/* 0854A4 800848A4 2404000A */   li    $a0, 10
.L800848A8:
/* 0854A8 800848A8 26310001 */  addiu $s1, $s1, 1
/* 0854AC 800848AC 0071C021 */  addu  $t8, $v1, $s1
/* 0854B0 800848B0 93020000 */  lbu   $v0, ($t8)
/* 0854B4 800848B4 00000000 */  nop   
/* 0854B8 800848B8 28410030 */  slti  $at, $v0, 0x30
/* 0854BC 800848BC 1420FFFA */  bnez  $at, .L800848A8
/* 0854C0 800848C0 00000000 */   nop   
/* 0854C4 800848C4 2841003A */  slti  $at, $v0, 0x3a
/* 0854C8 800848C8 1020FFF7 */  beqz  $at, .L800848A8
/* 0854CC 800848CC 00000000 */   nop   
/* 0854D0 800848D0 2404000A */  li    $a0, 10
.L800848D4:
/* 0854D4 800848D4 0264001A */  div   $zero, $s3, $a0
/* 0854D8 800848D8 00714021 */  addu  $t0, $v1, $s1
/* 0854DC 800848DC 14800002 */  bnez  $a0, .L800848E8
/* 0854E0 800848E0 00000000 */   nop   
/* 0854E4 800848E4 0007000D */  break 7
.L800848E8:
/* 0854E8 800848E8 2401FFFF */  li    $at, -1
/* 0854EC 800848EC 14810004 */  bne   $a0, $at, .L80084900
/* 0854F0 800848F0 3C018000 */   lui   $at, 0x8000
/* 0854F4 800848F4 16610002 */  bne   $s3, $at, .L80084900
/* 0854F8 800848F8 00000000 */   nop   
/* 0854FC 800848FC 0006000D */  break 6
.L80084900:
/* 085500 80084900 26310001 */  addiu $s1, $s1, 1
/* 085504 80084904 3C128012 */  lui   $s2, %hi(sMenuCurrDisplayList) # $s2, 0x8012
/* 085508 80084908 265263A0 */  addiu $s2, %lo(sMenuCurrDisplayList) # addiu $s2, $s2, 0x63a0
/* 08550C 8008490C 3C0F800E */  lui   $t7, %hi(D_800DFAC8) # $t7, 0x800e
/* 085510 80084910 00001012 */  mflo  $v0
/* 085514 80084914 24590030 */  addiu $t9, $v0, 0x30
/* 085518 80084918 A1190000 */  sb    $t9, ($t0)
/* 08551C 8008491C 00440019 */  multu $v0, $a0
/* 085520 80084920 8CAB0000 */  lw    $t3, ($a1)
/* 085524 80084924 3C058012 */  lui   $a1, %hi(sMenuCurrHudMat) # $a1, 0x8012
/* 085528 80084928 01716021 */  addu  $t4, $t3, $s1
/* 08552C 8008492C 3C198012 */  lui   $t9, %hi(gAudioOutputStrings) # $t9, 0x8012
/* 085530 80084930 24A563A8 */  addiu $a1, %lo(sMenuCurrHudMat) # addiu $a1, $a1, 0x63a8
/* 085534 80084934 02402025 */  move  $a0, $s2
/* 085538 80084938 00004812 */  mflo  $t1
/* 08553C 8008493C 02699823 */  subu  $s3, $s3, $t1
/* 085540 80084940 266A0030 */  addiu $t2, $s3, 0x30
/* 085544 80084944 A18A0000 */  sb    $t2, ($t4)
/* 085548 80084948 3C138012 */  lui   $s3, %hi(gOptionBlinkTimer) # $s3, 0x8012
/* 08554C 8008494C 8E7363BC */  lw    $s3, %lo(gOptionBlinkTimer)($s3)
/* 085550 80084950 00000000 */  nop   
/* 085554 80084954 001368C0 */  sll   $t5, $s3, 3
/* 085558 80084958 29A10100 */  slti  $at, $t5, 0x100
/* 08555C 8008495C 14200003 */  bnez  $at, .L8008496C
/* 085560 80084960 01A09825 */   move  $s3, $t5
/* 085564 80084964 240E01FF */  li    $t6, 511
/* 085568 80084968 01CD9823 */  subu  $s3, $t6, $t5
.L8008496C:
/* 08556C 8008496C 8DEFFAC8 */  lw    $t7, %lo(D_800DFAC8)($t7)
/* 085570 80084970 3C01800E */  lui   $at, %hi(gAudioMenuStrings+12) # $at, 0x800e
/* 085574 80084974 000FC080 */  sll   $t8, $t7, 2
/* 085578 80084978 0338C821 */  addu  $t9, $t9, $t8
/* 08557C 8008497C 8F3969D0 */  lw    $t9, %lo(gAudioOutputStrings)($t9)
/* 085580 80084980 0C019FCB */  jal   set_ortho_matrix_view
/* 085584 80084984 AC39FA48 */   sw    $t9, %lo(gAudioMenuStrings+12)($at)
/* 085588 80084988 3C088000 */  lui   $t0, %hi(osTvType) # $t0, 0x8000
/* 08558C 8008498C 8D080300 */  lw    $t0, %lo(osTvType)($t0)
/* 085590 80084990 3C11FFC0 */  lui   $s1, (0xFFC040FF >> 16) # lui $s1, 0xffc0
/* 085594 80084994 15000003 */  bnez  $t0, .L800849A4
/* 085598 80084998 363140FF */   ori   $s1, (0xFFC040FF & 0xFFFF) # ori $s1, $s1, 0x40ff
/* 08559C 8008499C 10000002 */  b     .L800849A8
/* 0855A0 800849A0 24100065 */   li    $s0, 101
.L800849A4:
/* 0855A4 800849A4 24100071 */  li    $s0, 113
.L800849A8:
/* 0855A8 800849A8 3C0D8012 */  lui   $t5, %hi(D_80126550) # $t5, 0x8012
/* 0855AC 800849AC 8DAD6660 */  lw    $t5, %lo(D_80126550 + 0x110)($t5)
/* 0855B0 800849B0 24090078 */  li    $t1, 120
/* 0855B4 800849B4 240B000E */  li    $t3, 14
/* 0855B8 800849B8 240A0006 */  li    $t2, 6
/* 0855BC 800849BC 240C0004 */  li    $t4, 4
/* 0855C0 800849C0 AFAC0018 */  sw    $t4, 0x18($sp)
/* 0855C4 800849C4 AFAA0014 */  sw    $t2, 0x14($sp)
/* 0855C8 800849C8 AFAB0010 */  sw    $t3, 0x10($sp)
/* 0855CC 800849CC 01303023 */  subu  $a2, $t1, $s0
/* 0855D0 800849D0 00002025 */  move  $a0, $zero
/* 0855D4 800849D4 2405FFB8 */  li    $a1, -72
/* 0855D8 800849D8 24070090 */  li    $a3, 144
/* 0855DC 800849DC AFB1001C */  sw    $s1, 0x1c($sp)
/* 0855E0 800849E0 0C020160 */  jal   func_80080580
/* 0855E4 800849E4 AFAD0020 */   sw    $t5, 0x20($sp)
/* 0855E8 800849E8 3C088012 */  lui   $t0, %hi(D_80126550) # $t0, 0x8012
/* 0855EC 800849EC 8D086660 */  lw    $t0, %lo(D_80126550 + 0x110)($t0)
/* 0855F0 800849F0 240E0050 */  li    $t6, 80
/* 0855F4 800849F4 240F000E */  li    $t7, 14
/* 0855F8 800849F8 24180006 */  li    $t8, 6
/* 0855FC 800849FC 24190004 */  li    $t9, 4
/* 085600 80084A00 AFB90018 */  sw    $t9, 0x18($sp)
/* 085604 80084A04 AFB80014 */  sw    $t8, 0x14($sp)
/* 085608 80084A08 AFAF0010 */  sw    $t7, 0x10($sp)
/* 08560C 80084A0C 01D03023 */  subu  $a2, $t6, $s0
/* 085610 80084A10 00002025 */  move  $a0, $zero
/* 085614 80084A14 2405FFB8 */  li    $a1, -72
/* 085618 80084A18 24070090 */  li    $a3, 144
/* 08561C 80084A1C AFB1001C */  sw    $s1, 0x1c($sp)
/* 085620 80084A20 0C020160 */  jal   func_80080580
/* 085624 80084A24 AFA80020 */   sw    $t0, 0x20($sp)
/* 085628 80084A28 0C0202F2 */  jal   func_80080BC8
/* 08562C 80084A2C 02402025 */   move  $a0, $s2
/* 085630 80084A30 0C031525 */  jal   assign_dialogue_box_id
/* 085634 80084A34 24040007 */   li    $a0, 7
/* 085638 80084A38 240900FF */  li    $t1, 255
/* 08563C 80084A3C AFA90010 */  sw    $t1, 0x10($sp)
/* 085640 80084A40 24040007 */  li    $a0, 7
/* 085644 80084A44 00002825 */  move  $a1, $zero
/* 085648 80084A48 00003025 */  move  $a2, $zero
/* 08564C 80084A4C 0C0313EF */  jal   set_current_dialogue_background_colour
/* 085650 80084A50 00003825 */   move  $a3, $zero
/* 085654 80084A54 240B007B */  li    $t3, 123
/* 085658 80084A58 AFAB0010 */  sw    $t3, 0x10($sp)
/* 08565C 80084A5C 24040007 */  li    $a0, 7
/* 085660 80084A60 2405005E */  li    $a1, 94
/* 085664 80084A64 24060075 */  li    $a2, 117
/* 085668 80084A68 0C0313B7 */  jal   set_current_dialogue_box_coords
/* 08566C 80084A6C 240700E2 */   li    $a3, 226
/* 085670 80084A70 02402025 */  move  $a0, $s2
/* 085674 80084A74 00002825 */  move  $a1, $zero
/* 085678 80084A78 00003025 */  move  $a2, $zero
/* 08567C 80084A7C 0C0316D6 */  jal   render_dialogue_box
/* 085680 80084A80 24070007 */   li    $a3, 7
/* 085684 80084A84 240A00A3 */  li    $t2, 163
/* 085688 80084A88 AFAA0010 */  sw    $t2, 0x10($sp)
/* 08568C 80084A8C 24040007 */  li    $a0, 7
/* 085690 80084A90 2405005E */  li    $a1, 94
/* 085694 80084A94 2406009D */  li    $a2, 157
/* 085698 80084A98 0C0313B7 */  jal   set_current_dialogue_box_coords
/* 08569C 80084A9C 240700E2 */   li    $a3, 226
/* 0856A0 80084AA0 02402025 */  move  $a0, $s2
/* 0856A4 80084AA4 00002825 */  move  $a1, $zero
/* 0856A8 80084AA8 00003025 */  move  $a2, $zero
/* 0856AC 80084AAC 0C0316D6 */  jal   render_dialogue_box
/* 0856B0 80084AB0 24070007 */   li    $a3, 7
/* 0856B4 80084AB4 3C06800E */  lui   $a2, %hi(gSfxVolumeSliderValue) # $a2, 0x800e
/* 0856B8 80084AB8 8CC6FAC0 */  lw    $a2, %lo(gSfxVolumeSliderValue)($a2)
/* 0856BC 80084ABC 3C10800E */  lui   $s0, %hi(gMenuSelectionArrowLeft) # $s0, 0x800e
/* 0856C0 80084AC0 2610042C */  addiu $s0, %lo(gMenuSelectionArrowLeft) # addiu $s0, $s0, 0x42c
/* 0856C4 80084AC4 240D00FF */  li    $t5, 255
/* 0856C8 80084AC8 240E00FF */  li    $t6, 255
/* 0856CC 80084ACC 240F00FF */  li    $t7, 255
/* 0856D0 80084AD0 241800FF */  li    $t8, 255
/* 0856D4 80084AD4 00066043 */  sra   $t4, $a2, 1
/* 0856D8 80084AD8 25860060 */  addiu $a2, $t4, 0x60
/* 0856DC 80084ADC AFB8001C */  sw    $t8, 0x1c($sp)
/* 0856E0 80084AE0 AFAF0018 */  sw    $t7, 0x18($sp)
/* 0856E4 80084AE4 AFAE0014 */  sw    $t6, 0x14($sp)
/* 0856E8 80084AE8 AFAD0010 */  sw    $t5, 0x10($sp)
/* 0856EC 80084AEC 02002825 */  move  $a1, $s0
/* 0856F0 80084AF0 02402025 */  move  $a0, $s2
/* 0856F4 80084AF4 0C01E2AE */  jal   render_textured_rectangle
/* 0856F8 80084AF8 24070078 */   li    $a3, 120
/* 0856FC 80084AFC 3C06800E */  lui   $a2, %hi(gMusicVolumeSliderValue) # $a2, 0x800e
/* 085700 80084B00 8CC6FAC4 */  lw    $a2, %lo(gMusicVolumeSliderValue)($a2)
/* 085704 80084B04 240800FF */  li    $t0, 255
/* 085708 80084B08 240900FF */  li    $t1, 255
/* 08570C 80084B0C 240B00FF */  li    $t3, 255
/* 085710 80084B10 240A00FF */  li    $t2, 255
/* 085714 80084B14 0006C843 */  sra   $t9, $a2, 1
/* 085718 80084B18 27260060 */  addiu $a2, $t9, 0x60
/* 08571C 80084B1C AFAA001C */  sw    $t2, 0x1c($sp)
/* 085720 80084B20 AFAB0018 */  sw    $t3, 0x18($sp)
/* 085724 80084B24 AFA90014 */  sw    $t1, 0x14($sp)
/* 085728 80084B28 AFA80010 */  sw    $t0, 0x10($sp)
/* 08572C 80084B2C 02402025 */  move  $a0, $s2
/* 085730 80084B30 02002825 */  move  $a1, $s0
/* 085734 80084B34 0C01E2AE */  jal   render_textured_rectangle
/* 085738 80084B38 240700A0 */   li    $a3, 160
/* 08573C 80084B3C 0C01ECF4 */  jal   reset_render_settings
/* 085740 80084B40 02402025 */   move  $a0, $s2
/* 085744 80084B44 00002025 */  move  $a0, $zero
/* 085748 80084B48 00002825 */  move  $a1, $zero
/* 08574C 80084B4C 00003025 */  move  $a2, $zero
/* 085750 80084B50 0C0310F3 */  jal   set_text_background_colour
/* 085754 80084B54 00003825 */   move  $a3, $zero
/* 085758 80084B58 3C0C8012 */  lui   $t4, %hi(gMenuOptionCount) # $t4, 0x8012
/* 08575C 80084B5C 8D8C63E0 */  lw    $t4, %lo(gMenuOptionCount)($t4)
/* 085760 80084B60 00001825 */  move  $v1, $zero
/* 085764 80084B64 29810005 */  slti  $at, $t4, 5
/* 085768 80084B68 10200005 */  beqz  $at, .L80084B80
/* 08576C 80084B6C 3C0D800E */   lui   $t5, %hi(gAudioMenuStrings+12) # $t5, 0x800e
/* 085770 80084B70 3C118012 */  lui   $s1, %hi(gOptionsMenuItemIndex) # $s1, 0x8012
/* 085774 80084B74 86316C46 */  lh    $s1, %lo(gOptionsMenuItemIndex)($s1)
/* 085778 80084B78 1000000E */  b     .L80084BB4
/* 08577C 80084B7C 00000000 */   nop   
.L80084B80:
/* 085780 80084B80 3C028012 */  lui   $v0, %hi(gOptionsMenuItemIndex) # $v0, 0x8012
/* 085784 80084B84 84426C46 */  lh    $v0, %lo(gOptionsMenuItemIndex)($v0)
/* 085788 80084B88 00000000 */  nop   
/* 08578C 80084B8C 28410003 */  slti  $at, $v0, 3
/* 085790 80084B90 10200004 */  beqz  $at, .L80084BA4
/* 085794 80084B94 24010003 */   li    $at, 3
/* 085798 80084B98 10000006 */  b     .L80084BB4
/* 08579C 80084B9C 00408825 */   move  $s1, $v0
/* 0857A0 80084BA0 24010003 */  li    $at, 3
.L80084BA4:
/* 0857A4 80084BA4 14410003 */  bne   $v0, $at, .L80084BB4
/* 0857A8 80084BA8 24110003 */   li    $s1, 3
/* 0857AC 80084BAC 10000001 */  b     .L80084BB4
/* 0857B0 80084BB0 24110006 */   li    $s1, 6
.L80084BB4:
/* 0857B4 80084BB4 8DADFA48 */  lw    $t5, %lo(gAudioMenuStrings+12)($t5)
/* 0857B8 80084BB8 00037100 */  sll   $t6, $v1, 4
/* 0857BC 80084BBC 11A00026 */  beqz  $t5, .L80084C58
/* 0857C0 80084BC0 3C0F800E */   lui   $t7, %hi(gAudioMenuStrings) # $t7, 0x800e
/* 0857C4 80084BC4 25EFFA3C */  addiu $t7, %lo(gAudioMenuStrings) # addiu $t7, $t7, -0x5c4
/* 0857C8 80084BC8 0011C100 */  sll   $t8, $s1, 4
/* 0857CC 80084BCC 030FC821 */  addu  $t9, $t8, $t7
/* 0857D0 80084BD0 AFB90040 */  sw    $t9, 0x40($sp)
/* 0857D4 80084BD4 01CF8021 */  addu  $s0, $t6, $t7
.L80084BD8:
/* 0857D8 80084BD8 92040009 */  lbu   $a0, 9($s0)
/* 0857DC 80084BDC 0C0310BB */  jal   set_text_font
/* 0857E0 80084BE0 00000000 */   nop   
/* 0857E4 80084BE4 8FA80040 */  lw    $t0, 0x40($sp)
/* 0857E8 80084BE8 240400FF */  li    $a0, 255
/* 0857EC 80084BEC 16080008 */  bne   $s0, $t0, .L80084C10
/* 0857F0 80084BF0 240500FF */   li    $a1, 255
/* 0857F4 80084BF4 240900FF */  li    $t1, 255
/* 0857F8 80084BF8 AFA90010 */  sw    $t1, 0x10($sp)
/* 0857FC 80084BFC 240600FF */  li    $a2, 255
/* 085800 80084C00 0C0310E1 */  jal   set_text_colour
/* 085804 80084C04 02603825 */   move  $a3, $s3
/* 085808 80084C08 10000009 */  b     .L80084C30
/* 08580C 80084C0C 860A000A */   lh    $t2, 0xa($s0)
.L80084C10:
/* 085810 80084C10 920B0008 */  lbu   $t3, 8($s0)
/* 085814 80084C14 92040004 */  lbu   $a0, 4($s0)
/* 085818 80084C18 92050005 */  lbu   $a1, 5($s0)
/* 08581C 80084C1C 92060006 */  lbu   $a2, 6($s0)
/* 085820 80084C20 92070007 */  lbu   $a3, 7($s0)
/* 085824 80084C24 0C0310E1 */  jal   set_text_colour
/* 085828 80084C28 AFAB0010 */   sw    $t3, 0x10($sp)
/* 08582C 80084C2C 860A000A */  lh    $t2, 0xa($s0)
.L80084C30:
/* 085830 80084C30 86050000 */  lh    $a1, ($s0)
/* 085834 80084C34 86060002 */  lh    $a2, 2($s0)
/* 085838 80084C38 8E07000C */  lw    $a3, 0xc($s0)
/* 08583C 80084C3C 02402025 */  move  $a0, $s2
/* 085840 80084C40 0C031110 */  jal   draw_text
/* 085844 80084C44 AFAA0010 */   sw    $t2, 0x10($sp)
/* 085848 80084C48 8E0C001C */  lw    $t4, 0x1c($s0)
/* 08584C 80084C4C 26100010 */  addiu $s0, $s0, 0x10
/* 085850 80084C50 1580FFE1 */  bnez  $t4, .L80084BD8
/* 085854 80084C54 00000000 */   nop   
.L80084C58:
/* 085858 80084C58 8FBF003C */  lw    $ra, 0x3c($sp)
/* 08585C 80084C5C 8FB0002C */  lw    $s0, 0x2c($sp)
/* 085860 80084C60 8FB10030 */  lw    $s1, 0x30($sp)
/* 085864 80084C64 8FB20034 */  lw    $s2, 0x34($sp)
/* 085868 80084C68 8FB30038 */  lw    $s3, 0x38($sp)
/* 08586C 80084C6C 03E00008 */  jr    $ra
/* 085870 80084C70 27BD0058 */   addiu $sp, $sp, 0x58

