glabel func_80043ECC
/* 044ACC 80043ECC 27BDFFE0 */  addiu $sp, $sp, -0x20
/* 044AD0 80043ED0 AFBF0014 */  sw    $ra, 0x14($sp)
/* 044AD4 80043ED4 14800008 */  bnez  $a0, .L80043EF8
/* 044AD8 80043ED8 AFA60028 */   sw    $a2, 0x28($sp)
/* 044ADC 80043EDC 3C018012 */  lui   $at, %hi(D_8011D5BA) # $at, 0x8012
/* 044AE0 80043EE0 A020D5BA */  sb    $zero, %lo(D_8011D5BA)($at)
/* 044AE4 80043EE4 3C018012 */  lui   $at, %hi(D_8011D5BB) # $at, 0x8012
/* 044AE8 80043EE8 A020D5BB */  sb    $zero, %lo(D_8011D5BB)($at)
/* 044AEC 80043EEC 3C018012 */  lui   $at, %hi(D_8011D5BC) # $at, 0x8012
/* 044AF0 80043EF0 1000009B */  b     .L80044160
/* 044AF4 80043EF4 A020D5BC */   sb    $zero, %lo(D_8011D5BC)($at)
.L80043EF8:
/* 044AF8 80043EF8 0C01B063 */  jal   func_8006C18C
/* 044AFC 80043EFC AFA50024 */   sw    $a1, 0x24($sp)
/* 044B00 80043F00 8FA50024 */  lw    $a1, 0x24($sp)
/* 044B04 80043F04 00403025 */  move  $a2, $v0
/* 044B08 80043F08 80AE01D3 */  lb    $t6, 0x1d3($a1)
/* 044B0C 80043F0C 3C018012 */  lui   $at, %hi(D_8011D5BB) # $at, 0x8012
/* 044B10 80043F10 11C0001A */  beqz  $t6, .L80043F7C
/* 044B14 80043F14 2404000C */   li    $a0, 12
/* 044B18 80043F18 3C0F8012 */  lui   $t7, %hi(D_8011D5BB) # $t7, 0x8012
/* 044B1C 80043F1C 81EFD5BB */  lb    $t7, %lo(D_8011D5BB)($t7)
/* 044B20 80043F20 240C0001 */  li    $t4, 1
/* 044B24 80043F24 15E0000B */  bnez  $t7, .L80043F54
/* 044B28 80043F28 3C0D8012 */   lui   $t5, %hi(gCurrentCarInput) # $t5, 0x8012
/* 044B2C 80043F2C 80580014 */  lb    $t8, 0x14($v0)
/* 044B30 80043F30 80590016 */  lb    $t9, 0x16($v0)
/* 044B34 80043F34 80490015 */  lb    $t1, 0x15($v0)
/* 044B38 80043F38 804A0017 */  lb    $t2, 0x17($v0)
/* 044B3C 80043F3C 03194021 */  addu  $t0, $t8, $t9
/* 044B40 80043F40 012A5821 */  addu  $t3, $t1, $t2
/* 044B44 80043F44 A0480014 */  sb    $t0, 0x14($v0)
/* 044B48 80043F48 A04B0015 */  sb    $t3, 0x15($v0)
/* 044B4C 80043F4C 3C018012 */  lui   $at, %hi(D_8011D5BB) # $at, 0x8012
/* 044B50 80043F50 A02CD5BB */  sb    $t4, %lo(D_8011D5BB)($at)
.L80043F54:
/* 044B54 80043F54 8DADD528 */  lw    $t5, %lo(gCurrentCarInput)($t5)
/* 044B58 80043F58 8FB80028 */  lw    $t8, 0x28($sp)
/* 044B5C 80043F5C 31AE8000 */  andi  $t6, $t5, 0x8000
/* 044B60 80043F60 15C00016 */  bnez  $t6, .L80043FBC
/* 044B64 80043F64 3C0F8012 */   lui   $t7, %hi(D_8011D5BA) # $t7, 0x8012
/* 044B68 80043F68 81EFD5BA */  lb    $t7, %lo(D_8011D5BA)($t7)
/* 044B6C 80043F6C 3C018012 */  lui   $at, %hi(D_8011D5BA) # $at, 0x8012
/* 044B70 80043F70 01F8C821 */  addu  $t9, $t7, $t8
/* 044B74 80043F74 10000011 */  b     .L80043FBC
/* 044B78 80043F78 A039D5BA */   sb    $t9, %lo(D_8011D5BA)($at)
.L80043F7C:
/* 044B7C 80043F7C 3C088012 */  lui   $t0, %hi(D_8011D5BA) # $t0, 0x8012
/* 044B80 80043F80 8108D5BA */  lb    $t0, %lo(D_8011D5BA)($t0)
/* 044B84 80043F84 A020D5BB */  sb    $zero, %lo(D_8011D5BB)($at)
/* 044B88 80043F88 29010015 */  slti  $at, $t0, 0x15
/* 044B8C 80043F8C 14200009 */  bnez  $at, .L80043FB4
/* 044B90 80043F90 00000000 */   nop   
/* 044B94 80043F94 80490008 */  lb    $t1, 8($v0)
/* 044B98 80043F98 804A000A */  lb    $t2, 0xa($v0)
/* 044B9C 80043F9C 804C0009 */  lb    $t4, 9($v0)
/* 044BA0 80043FA0 804D000B */  lb    $t5, 0xb($v0)
/* 044BA4 80043FA4 012A5821 */  addu  $t3, $t1, $t2
/* 044BA8 80043FA8 018D7021 */  addu  $t6, $t4, $t5
/* 044BAC 80043FAC A04B0008 */  sb    $t3, 8($v0)
/* 044BB0 80043FB0 A04E0009 */  sb    $t6, 9($v0)
.L80043FB4:
/* 044BB4 80043FB4 3C018012 */  lui   $at, %hi(D_8011D5BA) # $at, 0x8012
/* 044BB8 80043FB8 A020D5BA */  sb    $zero, %lo(D_8011D5BA)($at)
.L80043FBC:
/* 044BBC 80043FBC 80AF0173 */  lb    $t7, 0x173($a1)
/* 044BC0 80043FC0 3C188012 */  lui   $t8, %hi(D_8011D5BC) # $t8, 0x8012
/* 044BC4 80043FC4 11E00014 */  beqz  $t7, .L80044018
/* 044BC8 80043FC8 3C018012 */   lui   $at, %hi(D_8011D5BC) # $at, 0x8012
/* 044BCC 80043FCC 80A30174 */  lb    $v1, 0x174($a1)
/* 044BD0 80043FD0 8318D5BC */  lb    $t8, %lo(D_8011D5BC)($t8)
/* 044BD4 80043FD4 00000000 */  nop   
/* 044BD8 80043FD8 0303082A */  slt   $at, $t8, $v1
/* 044BDC 80043FDC 1020000B */  beqz  $at, .L8004400C
/* 044BE0 80043FE0 00000000 */   nop   
/* 044BE4 80043FE4 8059000C */  lb    $t9, 0xc($v0)
/* 044BE8 80043FE8 8048000E */  lb    $t0, 0xe($v0)
/* 044BEC 80043FEC 804A000D */  lb    $t2, 0xd($v0)
/* 044BF0 80043FF0 804B000F */  lb    $t3, 0xf($v0)
/* 044BF4 80043FF4 03284821 */  addu  $t1, $t9, $t0
/* 044BF8 80043FF8 014B6021 */  addu  $t4, $t2, $t3
/* 044BFC 80043FFC A049000C */  sb    $t1, 0xc($v0)
/* 044C00 80044000 A04C000D */  sb    $t4, 0xd($v0)
/* 044C04 80044004 80A30174 */  lb    $v1, 0x174($a1)
/* 044C08 80044008 00000000 */  nop   
.L8004400C:
/* 044C0C 8004400C 3C018012 */  lui   $at, %hi(D_8011D5BC) # $at, 0x8012
/* 044C10 80044010 10000002 */  b     .L8004401C
/* 044C14 80044014 A023D5BC */   sb    $v1, %lo(D_8011D5BC)($at)
.L80044018:
/* 044C18 80044018 A020D5BC */  sb    $zero, %lo(D_8011D5BC)($at)
.L8004401C:
/* 044C1C 8004401C AFA50024 */  sw    $a1, 0x24($sp)
/* 044C20 80044020 0C0078A7 */  jal   get_misc_asset
/* 044C24 80044024 AFA6001C */   sw    $a2, 0x1c($sp)
/* 044C28 80044028 3C0D8012 */  lui   $t5, %hi(D_8011D530) # $t5, 0x8012
/* 044C2C 8004402C 8DADD530 */  lw    $t5, %lo(D_8011D530)($t5)
/* 044C30 80044030 8FA50024 */  lw    $a1, 0x24($sp)
/* 044C34 80044034 8FA6001C */  lw    $a2, 0x1c($sp)
/* 044C38 80044038 31AE2000 */  andi  $t6, $t5, 0x2000
/* 044C3C 8004403C 11C00022 */  beqz  $t6, .L800440C8
/* 044C40 80044040 00000000 */   nop   
/* 044C44 80044044 80AF0173 */  lb    $t7, 0x173($a1)
/* 044C48 80044048 3C0A800E */  lui   $t2, %hi(D_800DCD90) # $t2, 0x800e
/* 044C4C 8004404C 11E0001E */  beqz  $t7, .L800440C8
/* 044C50 80044050 00000000 */   nop   
/* 044C54 80044054 80A30174 */  lb    $v1, 0x174($a1)
/* 044C58 80044058 00000000 */  nop   
/* 044C5C 8004405C 28610003 */  slti  $at, $v1, 3
/* 044C60 80044060 1020000A */  beqz  $at, .L8004408C
/* 044C64 80044064 00000000 */   nop   
/* 044C68 80044068 80B80172 */  lb    $t8, 0x172($a1)
/* 044C6C 8004406C 00000000 */  nop   
/* 044C70 80044070 0018C880 */  sll   $t9, $t8, 2
/* 044C74 80044074 0338C823 */  subu  $t9, $t9, $t8
/* 044C78 80044078 03234021 */  addu  $t0, $t9, $v1
/* 044C7C 8004407C 01024821 */  addu  $t1, $t0, $v0
/* 044C80 80044080 81240000 */  lb    $a0, ($t1)
/* 044C84 80044084 10000004 */  b     .L80044098
/* 044C88 80044088 01445021 */   addu  $t2, $t2, $a0
.L8004408C:
/* 044C8C 8004408C 80A40172 */  lb    $a0, 0x172($a1)
/* 044C90 80044090 00000000 */  nop   
/* 044C94 80044094 01445021 */  addu  $t2, $t2, $a0
.L80044098:
/* 044C98 80044098 814ACD90 */  lb    $t2, %lo(D_800DCD90)($t2)
/* 044C9C 8004409C 24010001 */  li    $at, 1
/* 044CA0 800440A0 1541000A */  bne   $t2, $at, .L800440CC
/* 044CA4 800440A4 00C01025 */   move  $v0, $a2
/* 044CA8 800440A8 80CB0010 */  lb    $t3, 0x10($a2)
/* 044CAC 800440AC 80CC0012 */  lb    $t4, 0x12($a2)
/* 044CB0 800440B0 80CE0011 */  lb    $t6, 0x11($a2)
/* 044CB4 800440B4 80CF0013 */  lb    $t7, 0x13($a2)
/* 044CB8 800440B8 016C6821 */  addu  $t5, $t3, $t4
/* 044CBC 800440BC 01CFC021 */  addu  $t8, $t6, $t7
/* 044CC0 800440C0 A0CD0010 */  sb    $t5, 0x10($a2)
/* 044CC4 800440C4 A0D80011 */  sb    $t8, 0x11($a2)
.L800440C8:
/* 044CC8 800440C8 00C01025 */  move  $v0, $a2
.L800440CC:
/* 044CCC 800440CC 24060002 */  li    $a2, 2
/* 044CD0 800440D0 00002825 */  move  $a1, $zero
/* 044CD4 800440D4 24040064 */  li    $a0, 100
.L800440D8:
/* 044CD8 800440D8 8043000C */  lb    $v1, 0xc($v0)
/* 044CDC 800440DC 24A50001 */  addiu $a1, $a1, 1
/* 044CE0 800440E0 28610065 */  slti  $at, $v1, 0x65
/* 044CE4 800440E4 10200003 */  beqz  $at, .L800440F4
/* 044CE8 800440E8 00000000 */   nop   
/* 044CEC 800440EC 04610002 */  bgez  $v1, .L800440F8
/* 044CF0 800440F0 00000000 */   nop   
.L800440F4:
/* 044CF4 800440F4 A044000C */  sb    $a0, 0xc($v0)
.L800440F8:
/* 044CF8 800440F8 80430008 */  lb    $v1, 8($v0)
/* 044CFC 800440FC 00000000 */  nop   
/* 044D00 80044100 28610065 */  slti  $at, $v1, 0x65
/* 044D04 80044104 10200003 */  beqz  $at, .L80044114
/* 044D08 80044108 00000000 */   nop   
/* 044D0C 8004410C 04610002 */  bgez  $v1, .L80044118
/* 044D10 80044110 00000000 */   nop   
.L80044114:
/* 044D14 80044114 A0440008 */  sb    $a0, 8($v0)
.L80044118:
/* 044D18 80044118 80430014 */  lb    $v1, 0x14($v0)
/* 044D1C 8004411C 00000000 */  nop   
/* 044D20 80044120 28610065 */  slti  $at, $v1, 0x65
/* 044D24 80044124 10200003 */  beqz  $at, .L80044134
/* 044D28 80044128 00000000 */   nop   
/* 044D2C 8004412C 04610002 */  bgez  $v1, .L80044138
/* 044D30 80044130 00000000 */   nop   
.L80044134:
/* 044D34 80044134 A0440014 */  sb    $a0, 0x14($v0)
.L80044138:
/* 044D38 80044138 80430010 */  lb    $v1, 0x10($v0)
/* 044D3C 8004413C 00000000 */  nop   
/* 044D40 80044140 28610065 */  slti  $at, $v1, 0x65
/* 044D44 80044144 10200003 */  beqz  $at, .L80044154
/* 044D48 80044148 00000000 */   nop   
/* 044D4C 8004414C 04610002 */  bgez  $v1, .L80044158
/* 044D50 80044150 00000000 */   nop   
.L80044154:
/* 044D54 80044154 A0440010 */  sb    $a0, 0x10($v0)
.L80044158:
/* 044D58 80044158 14A6FFDF */  bne   $a1, $a2, .L800440D8
/* 044D5C 8004415C 24420001 */   addiu $v0, $v0, 1
.L80044160:
/* 044D60 80044160 8FBF0014 */  lw    $ra, 0x14($sp)
/* 044D64 80044164 27BD0020 */  addiu $sp, $sp, 0x20
/* 044D68 80044168 03E00008 */  jr    $ra
/* 044D6C 8004416C 00000000 */   nop   

