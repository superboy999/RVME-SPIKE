require_extension(EXT_MATRIX);
reg_t md = sext_xlen(insn.md() - P.MU.NUM_MR);
reg_t ms1 = sext_xlen(insn.ms1());
reg_t ms2 = sext_xlen(insn.ms2());
reg_t tile_m = P.MU.tile_m;
reg_t tile_k = P.MU.tile_k;
reg_t tile_n = P.MU.tile_n;
for(uint32_t m=0; m<tile_m; m++)
  for(uint32_t n=0; n<tile_n; n++)
    for(uint32_t k=0; k<tile_k; k++)
    {
      int8_t ts1 = P.MU.tile_reg[ms1][m][k];
      int8_t ts2 = P.MU.tile_reg[ms2][n][k];
      int32_t &accd = P.MU.acc_reg[md][m][n];
      accd += ts2 * ts1;
    }
P.MU.print_acc(md);