require_extension(EXT_MATRIX);
const reg_t spmAddr = RS1;
const reg_t stride  = RS2;
const reg_t md      = insn.md();
// 清空 acc register
reg_t acc_idx = md - P.MU.NUM_MR;
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
for (reg_t n = 0; n < P.MU.MAX_N; n++) {
    P.MU.acc_reg[acc_idx][m][n] = 0;
}
}
// 然后进入加载循环
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
for (reg_t n = 0; n < P.MU.MAX_N; n++) {
    int32_t val = P.MU.spmload_C(spmAddr + m * stride + n * sizeof(int32_t));
    P.MU.acc_reg[acc_idx][n][m] = val;
}
}
P.MU.print_acc(acc_idx);