require_extension(EXT_MATRIX);
const reg_t spmAddr = RS1;
const reg_t stride  = RS2;
const reg_t md      = insn.md();
// 进入存储循环
reg_t acc_idx = md - P.MU.NUM_MR;
for (reg_t m = 0; m < P.MU.MAX_M; m++) {
    for (reg_t n = 0; n < P.MU.MAX_N; n++) {
        int32_t val = P.MU.acc_reg[acc_idx][n][m];
        P.MU.spmstore_C(spmAddr + m * stride + n * sizeof(int32_t), val);
    }
}