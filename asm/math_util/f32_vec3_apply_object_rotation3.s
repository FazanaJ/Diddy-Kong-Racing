/* Official Name: mathOneFloatPY */
glabel f32_vec3_apply_object_rotation3
/* 071090 80070490 27BDFFF8 */  addiu $sp, $sp, -8
/* 071094 80070494 FFBF0000 */  sd    $ra, ($sp)
/* 071098 80070498 00803025 */  move  $a2, $a0
/* 07109C 8007049C C4A80008 */  lwc1  $f8, 8($a1)
/* 0710A0 800704A0 0C01C1F1 */  jal   sins_f
/* 0710A4 800704A4 84C40002 */   lh    $a0, 2($a2)
/* 0710A8 800704A8 46004182 */  mul.s $f6, $f8, $f0
/* 0710AC 800704AC 0C01C1FE */  jal   coss_f
/* 0710B0 800704B0 84C40002 */   lh    $a0, 2($a2)
/* 0710B4 800704B4 46004202 */  mul.s $f8, $f8, $f0
/* 0710B8 800704B8 46003187 */  neg.s $f6, $f6
/* 0710BC 800704BC 0C01C1F1 */  jal   sins_f
/* 0710C0 800704C0 84C40000 */   lh    $a0, ($a2)
/* 0710C4 800704C4 46004102 */  mul.s $f4, $f8, $f0
/* 0710C8 800704C8 0C01C1FE */  jal   coss_f
/* 0710CC 800704CC 84C40000 */   lh    $a0, ($a2)
/* 0710D0 800704D0 46004202 */  mul.s $f8, $f8, $f0
/* 0710D4 800704D4 E4A40000 */  swc1  $f4, ($a1)
/* 0710D8 800704D8 E4A60004 */  swc1  $f6, 4($a1)
/* 0710DC 800704DC E4A80008 */  swc1  $f8, 8($a1)
/* 0710E0 800704E0 DFBF0000 */  ld    $ra, ($sp)
/* 0710E4 800704E4 27BD0008 */  addiu $sp, $sp, 8
/* 0710E8 800704E8 03E00008 */  jr    $ra
/* 0710EC 800704EC 00000000 */   nop   

