.rdata
glabel D_800E77CC
.asciz "DKRACING-GHOSTS"
glabel D_800E77DC
.word 0

.text
glabel func_800756D4
/* 0762D4 800756D4 27BDFF90 */  addiu $sp, $sp, -0x70
/* 0762D8 800756D8 AFBF003C */  sw    $ra, 0x3c($sp)
/* 0762DC 800756DC AFBE0038 */  sw    $fp, 0x38($sp)
/* 0762E0 800756E0 AFB70034 */  sw    $s7, 0x34($sp)
/* 0762E4 800756E4 AFB60030 */  sw    $s6, 0x30($sp)
/* 0762E8 800756E8 00A0B025 */  move  $s6, $a1
/* 0762EC 800756EC 00E0B825 */  move  $s7, $a3
/* 0762F0 800756F0 00C0F025 */  move  $fp, $a2
/* 0762F4 800756F4 AFB5002C */  sw    $s5, 0x2c($sp)
/* 0762F8 800756F8 AFB40028 */  sw    $s4, 0x28($sp)
/* 0762FC 800756FC AFB30024 */  sw    $s3, 0x24($sp)
/* 076300 80075700 AFB20020 */  sw    $s2, 0x20($sp)
/* 076304 80075704 AFB1001C */  sw    $s1, 0x1c($sp)
/* 076308 80075708 AFB00018 */  sw    $s0, 0x18($sp)
/* 07630C 8007570C 0C01D637 */  jal   get_si_device_status
/* 076310 80075710 AFA40070 */   sw    $a0, 0x70($sp)
/* 076314 80075714 10400007 */  beqz  $v0, .L80075734
/* 076318 80075718 AFA20064 */   sw    $v0, 0x64($sp)
/* 07631C 8007571C 8FA40070 */  lw    $a0, 0x70($sp)
/* 076320 80075720 0C01D6BB */  jal   start_reading_controller_data
/* 076324 80075724 00000000 */   nop   
/* 076328 80075728 8FA20064 */  lw    $v0, 0x64($sp)
/* 07632C 8007572C 10000060 */  b     .L800758B0
/* 076330 80075730 8FBF003C */   lw    $ra, 0x3c($sp)
.L80075734:
/* 076334 80075734 8FB50080 */  lw    $s5, 0x80($sp)
/* 076338 80075738 00008025 */  move  $s0, $zero
/* 07633C 8007573C 02C02025 */  move  $a0, $s6
/* 076340 80075740 02E02825 */  move  $a1, $s7
/* 076344 80075744 03C03025 */  move  $a2, $fp
/* 076348 80075748 240700FF */  li    $a3, 255
/* 07634C 8007574C 02A01825 */  move  $v1, $s5
.L80075750:
/* 076350 80075750 A0870000 */  sb    $a3, ($a0)
/* 076354 80075754 A4600000 */  sh    $zero, ($v1)
/* 076358 80075758 90620001 */  lbu   $v0, 1($v1)
/* 07635C 8007575C 26100001 */  addiu $s0, $s0, 1
/* 076360 80075760 2A010006 */  slti  $at, $s0, 6
/* 076364 80075764 A0A20000 */  sb    $v0, ($a1)
/* 076368 80075768 24840001 */  addiu $a0, $a0, 1
/* 07636C 8007576C 24630002 */  addiu $v1, $v1, 2
/* 076370 80075770 24A50001 */  addiu $a1, $a1, 1
/* 076374 80075774 24C60001 */  addiu $a2, $a2, 1
/* 076378 80075778 1420FFF5 */  bnez  $at, .L80075750
/* 07637C 8007577C A0C2FFFF */   sb    $v0, -1($a2)
/* 076380 80075780 8FA40070 */  lw    $a0, 0x70($sp)
/* 076384 80075784 3C05800E */  lui   $a1, %hi(D_800E77CC) # $a1, 0x800e
/* 076388 80075788 3C06800E */  lui   $a2, %hi(D_800E77DC) # $a2, 0x800e
/* 07638C 8007578C 24C677DC */  addiu $a2, %lo(D_800E77DC) # addiu $a2, $a2, 0x77dc
/* 076390 80075790 24A577CC */  addiu $a1, %lo(D_800E77CC) # addiu $a1, $a1, 0x77cc
/* 076394 80075794 0C01D93A */  jal   get_file_number
/* 076398 80075798 27A7005C */   addiu $a3, $sp, 0x5c
/* 07639C 8007579C 1440003E */  bnez  $v0, .L80075898
/* 0763A0 800757A0 AFA20064 */   sw    $v0, 0x64($sp)
/* 0763A4 800757A4 8FA40070 */  lw    $a0, 0x70($sp)
/* 0763A8 800757A8 8FA5005C */  lw    $a1, 0x5c($sp)
/* 0763AC 800757AC 0C01DA49 */  jal   get_file_size
/* 0763B0 800757B0 27A60058 */   addiu $a2, $sp, 0x58
/* 0763B4 800757B4 14400038 */  bnez  $v0, .L80075898
/* 0763B8 800757B8 AFA20064 */   sw    $v0, 0x64($sp)
/* 0763BC 800757BC 8FA40058 */  lw    $a0, 0x58($sp)
/* 0763C0 800757C0 240500FF */  li    $a1, 255
/* 0763C4 800757C4 0C01C327 */  jal   allocate_from_main_pool_safe
/* 0763C8 800757C8 24840100 */   addiu $a0, $a0, 0x100
/* 0763CC 800757CC 8FA40070 */  lw    $a0, 0x70($sp)
/* 0763D0 800757D0 8FA5005C */  lw    $a1, 0x5c($sp)
/* 0763D4 800757D4 8FA70058 */  lw    $a3, 0x58($sp)
/* 0763D8 800757D8 00409025 */  move  $s2, $v0
/* 0763DC 800757DC 0C01D984 */  jal   read_data_from_controller_pak
/* 0763E0 800757E0 00403025 */   move  $a2, $v0
/* 0763E4 800757E4 1440002A */  bnez  $v0, .L80075890
/* 0763E8 800757E8 AFA20064 */   sw    $v0, 0x64($sp)
/* 0763EC 800757EC 00008025 */  move  $s0, $zero
/* 0763F0 800757F0 26510004 */  addiu $s1, $s2, 4
/* 0763F4 800757F4 24140006 */  li    $s4, 6
/* 0763F8 800757F8 241300FF */  li    $s3, 255
.L800757FC:
/* 0763FC 800757FC 922E0000 */  lbu   $t6, ($s1)
/* 076400 80075800 00000000 */  nop   
/* 076404 80075804 126E001F */  beq   $s3, $t6, .L80075884
/* 076408 80075808 00000000 */   nop   
/* 07640C 8007580C 862F0002 */  lh    $t7, 2($s1)
/* 076410 80075810 0C01D293 */  jal   calculate_ghost_header_checksum
/* 076414 80075814 01F22021 */   addu  $a0, $t7, $s2
/* 076418 80075818 86380002 */  lh    $t8, 2($s1)
/* 07641C 8007581C 00000000 */  nop   
/* 076420 80075820 0312C821 */  addu  $t9, $t8, $s2
/* 076424 80075824 87280000 */  lh    $t0, ($t9)
/* 076428 80075828 00000000 */  nop   
/* 07642C 8007582C 10480003 */  beq   $v0, $t0, .L8007583C
/* 076430 80075830 24090009 */   li    $t1, 9
/* 076434 80075834 10000016 */  b     .L80075890
/* 076438 80075838 AFA90064 */   sw    $t1, 0x64($sp)
.L8007583C:
/* 07643C 8007583C 922A0000 */  lbu   $t2, ($s1)
/* 076440 80075840 02D05821 */  addu  $t3, $s6, $s0
/* 076444 80075844 A16A0000 */  sb    $t2, ($t3)
/* 076448 80075848 922C0001 */  lbu   $t4, 1($s1)
/* 07644C 8007584C 03D06821 */  addu  $t5, $fp, $s0
/* 076450 80075850 A1AC0000 */  sb    $t4, ($t5)
/* 076454 80075854 862E0002 */  lh    $t6, 2($s1)
/* 076458 80075858 02F0C821 */  addu  $t9, $s7, $s0
/* 07645C 8007585C 01D27821 */  addu  $t7, $t6, $s2
/* 076460 80075860 91F80002 */  lbu   $t8, 2($t7)
/* 076464 80075864 00105840 */  sll   $t3, $s0, 1
/* 076468 80075868 A3380000 */  sb    $t8, ($t9)
/* 07646C 8007586C 86280002 */  lh    $t0, 2($s1)
/* 076470 80075870 02AB6021 */  addu  $t4, $s5, $t3
/* 076474 80075874 01124821 */  addu  $t1, $t0, $s2
/* 076478 80075878 852A0004 */  lh    $t2, 4($t1)
/* 07647C 8007587C 00000000 */  nop   
/* 076480 80075880 A58A0000 */  sh    $t2, ($t4)
.L80075884:
/* 076484 80075884 26100001 */  addiu $s0, $s0, 1
/* 076488 80075888 1614FFDC */  bne   $s0, $s4, .L800757FC
/* 07648C 8007588C 26310004 */   addiu $s1, $s1, 4
.L80075890:
/* 076490 80075890 0C01C450 */  jal   free_from_memory_pool
/* 076494 80075894 02402025 */   move  $a0, $s2
.L80075898:
/* 076498 80075898 8FA40070 */  lw    $a0, 0x70($sp)
/* 07649C 8007589C 0C01D6BB */  jal   start_reading_controller_data
/* 0764A0 800758A0 00000000 */   nop   
/* 0764A4 800758A4 8FA20064 */  lw    $v0, 0x64($sp)
/* 0764A8 800758A8 00000000 */  nop   
/* 0764AC 800758AC 8FBF003C */  lw    $ra, 0x3c($sp)
.L800758B0:
/* 0764B0 800758B0 8FB00018 */  lw    $s0, 0x18($sp)
/* 0764B4 800758B4 8FB1001C */  lw    $s1, 0x1c($sp)
/* 0764B8 800758B8 8FB20020 */  lw    $s2, 0x20($sp)
/* 0764BC 800758BC 8FB30024 */  lw    $s3, 0x24($sp)
/* 0764C0 800758C0 8FB40028 */  lw    $s4, 0x28($sp)
/* 0764C4 800758C4 8FB5002C */  lw    $s5, 0x2c($sp)
/* 0764C8 800758C8 8FB60030 */  lw    $s6, 0x30($sp)
/* 0764CC 800758CC 8FB70034 */  lw    $s7, 0x34($sp)
/* 0764D0 800758D0 8FBE0038 */  lw    $fp, 0x38($sp)
/* 0764D4 800758D4 03E00008 */  jr    $ra
/* 0764D8 800758D8 27BD0070 */   addiu $sp, $sp, 0x70

