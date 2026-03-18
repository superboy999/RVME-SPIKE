require_extension(EXT_MATRIX);
const reg_t spmAddr = RS1;
const reg_t stride  = RS2;
const reg_t md      = insn.md();
// 清空 tile register
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
  for (reg_t k = 0; k < P.MU.MAX_K; k++) {
    P.MU.tile_reg[md][m][k] = 0;
  }
}
// 然后进入加载循环
for (reg_t k = 0; k < P.MU.MAX_K; k++) {
  for (reg_t m = 0; m < P.MU.MAX_M; m++){
    int8_t val = P.MU.spmload_B(spmAddr + k * stride + m * sizeof(int8_t));
    P.MU.tile_reg[md][m][k] = val;
  }
}
P.MU.print_tile(md);