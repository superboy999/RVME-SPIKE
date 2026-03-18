require_extension(EXT_MATRIX);
const reg_t memAddr = RS1;
const reg_t spmAddr = RS2;
const reg_t size    = insn.uimm5();
uint32_t totalsize = size == 0? 64*64 : size*64; // 总字节数
uint64_t BANK_A_START_ADDR = 0x0; // SPM_A 起始地址
uint64_t BANK_B_START_ADDR = (P.MU.SPM_A_ROWS * P.MU.SPM_WIDTH); // SPM_B 起始地址
uint64_t BANK_C_START_ADDR = (P.MU.SPM_A_ROWS * P.MU.SPM_WIDTH + P.MU.SPM_B_ROWS * P.MU.SPM_WIDTH); // SPM_C 起始地址
if (spmAddr < BANK_B_START_ADDR) {
    // store data from SPM_A to mem
    assert(spmAddr + totalsize <= BANK_B_START_ADDR && "SPM_A address out of range");
    for (uint32_t i = 0; i < totalsize; i++) {
        int8_t val = P.MU.spmload_A(spmAddr + i * sizeof(int8_t));
        MMU.store<int8_t>(memAddr + i * sizeof(int8_t), val);
    }
} else if (spmAddr >= BANK_B_START_ADDR && spmAddr < BANK_C_START_ADDR) {   
    // store data from SPM_B to mem
    assert(spmAddr + totalsize <= BANK_C_START_ADDR && "SPM_B address out of range");
    for (uint32_t i = 0; i < totalsize; i++) {
        int8_t val = P.MU.spmload_B(spmAddr + i * sizeof(int8_t));
        MMU.store<int8_t>(memAddr + i * sizeof(int8_t), val);
    }
} else if (spmAddr >= BANK_C_START_ADDR) {
    // store data from SPM_C to mem
    uint32_t acc_totalsize = totalsize / 4; // acc 是 4 字节
    assert(spmAddr + acc_totalsize <= BANK_C_START_ADDR + P.MU.SPM_C_ROWS * P.MU.SPM_WIDTH && "SPM_C address out of range");
    for (uint32_t i = 0; i < acc_totalsize; i++) {
        int32_t val = P.MU.spmload_C(spmAddr + i * sizeof(int32_t));
        MMU.store<int32_t>(memAddr + i * sizeof(int32_t), val);
    }
} else {
    assert(0 && "Invalid SPM address");
}

P.MU.numStored += size;
printf("[INFO] NUM OF DATA STORED UPDATED: %u\n", P.MU.numStored);