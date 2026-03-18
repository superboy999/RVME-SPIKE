require_extension(EXT_MATRIX);
reg_t md = sext_xlen(insn.md() - P.MU.NUM_MR);
reg_t ms1 = sext_xlen(insn.ms1() - P.MU.NUM_MR);
reg_t uimm3 = insn.uimm3();
reg_t tile_m = P.MU.tile_m;
reg_t tile_n = P.MU.tile_n;
for (reg_t n = 0; n < tile_n; n++) {
  int32_t sum_val = 0;
  for (reg_t m = 0; m < tile_m; m++) {
    int32_t val = (int32_t)P.MU.acc_reg[ms1][m][n];
    sum_val += val;
  }
  P.MU.acc_reg[md][uimm3][n] = sum_val; // 每列和写入md累加器第uimm3行
}
P.MU.print_acc(md);