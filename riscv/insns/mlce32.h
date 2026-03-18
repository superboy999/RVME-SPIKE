require_extension(EXT_MATRIX);
const reg_t baseAddr = RS1;
const reg_t stride   = RS2;
const reg_t md       = insn.md() - P.MU.NUM_MR;

// 清空 acc register
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
  for (reg_t n = 0; n < P.MU.MAX_N; n++) {
    P.MU.acc_reg[md][m][n] = 0;
  }
}

// 然后进入加载循环
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
  for (reg_t n = 0; n < P.MU.MAX_N; n++) {
    int32_t val = MMU.load<int32_t>(baseAddr + m * stride + n * sizeof(int32_t));
    P.MU.acc_reg[md][m][n] = val;
  }
}

P.MU.print_tile(md);