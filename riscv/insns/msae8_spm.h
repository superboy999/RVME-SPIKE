require_extension(EXT_MATRIX);
const reg_t spmAddr = RS1;
const reg_t stride  = RS2;
const reg_t md      = insn.md();
// 进入存储循环
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
  for (reg_t k = 0; k < P.MU.MAX_K; k++) {
    int8_t val = P.MU.tile_reg[md][m][k];
    P.MU.spmstore_A(spmAddr + m * stride + k * sizeof(int8_t), val);
  }
}