require_extension(EXT_MATRIX);
reg_t md = sext_xlen(insn.md() - P.MU.NUM_MR);
reg_t ms1 = sext_xlen(insn.ms1() - P.MU.NUM_MR);
reg_t ms2 = sext_xlen(insn.ms2() - P.MU.NUM_MR);
reg_t tile_m = P.MU.tile_m;
reg_t tile_n = P.MU.tile_n;
for (reg_t m = 0; m < tile_m; m++) {
  for (reg_t n = 0; n < tile_n; n++) {
    int32_t val = P.MU.acc_reg[ms1][m][n] + P.MU.acc_reg[ms2][m][n];
    P.MU.acc_reg[md][m][n] = val;
  }
}
P.MU.print_acc(md);