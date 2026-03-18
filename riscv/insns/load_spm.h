require_extension(EXT_MATRIX);
const reg_t memAddr = RS1;
const reg_t spmAddr = RS2;
const reg_t size    = insn.uimm5();
uint32_t totalsize = size == 0? 64*64 : size*64; // 总字节数
uint64_t BANK_A_START_ADDR = 0x0; // SPM_A 起始地址
uint64_t BANK_B_START_ADDR = (P.MU.SPM_A_ROWS * P.MU.SPM_WIDTH); // SPM_B 起始地址
uint64_t BANK_C_START_ADDR = (P.MU.SPM_A_ROWS * P.MU.SPM_WIDTH + P.MU.SPM_B_ROWS * P.MU.SPM_WIDTH); // SPM_C 起始地址
if (spmAddr < BANK_B_START_ADDR) {
    // Load data from mem to SPM_A
    assert(spmAddr + totalsize <= BANK_B_START_ADDR && "SPM_A address out of range");
    for (uint32_t i = 0; i < totalsize; i++) {
        int8_t val = MMU.load<int8_t>(memAddr + i * sizeof(int8_t));
        P.MU.spmstore_A(spmAddr + i * sizeof(int8_t), val);
    }
} else if (spmAddr >= BANK_B_START_ADDR && spmAddr < BANK_C_START_ADDR) {
    // Load data from mem to SPM_B
    assert(spmAddr + totalsize <= BANK_C_START_ADDR && "SPM_B address out of range");
    for (uint32_t i = 0; i < totalsize; i++) {
        int8_t val = MMU.load<int8_t>(memAddr + i * sizeof(int8_t));
        P.MU.spmstore_B(spmAddr + i * sizeof(int8_t), val);
    }
} else if (spmAddr >= BANK_C_START_ADDR) {
    // Load data from mem to SPM_C
    uint32_t acc_totalsize = totalsize / 4; // acc 是 4 字节
    assert(spmAddr + totalsize <= BANK_C_START_ADDR + P.MU.SPM_C_ROWS * P.MU.SPM_WIDTH && "SPM_C address out of range");
    for (uint32_t i = 0; i < acc_totalsize; i++) {
        // printf("[SPM] [DEBUG] loading from memAddr: 0x%lx to spmAddr: 0x%lx\n", memAddr + i * sizeof(int32_t), spmAddr + i * sizeof(int32_t));
        // printf("[SPM] [DEBUG] memAddr base = 0x%lx\n", (unsigned long)memAddr);
        // uintptr_t addr = (uintptr_t)memAddr;
        // uint64_t val1;

        // printf("addr=0x%" PRIxPTR "\n", addr);

        // // 尽量避免未对齐导致的 trap
        // memcpy(&val1, (void*)addr, sizeof(val1));   // 如果地址不可读，这里仍会 segfault

        // printf("val1=0x%016" PRIx64 " (%" PRIu64 ")\n", val1, val1);
        int32_t val = MMU.load<int32_t>(memAddr + i * sizeof(int32_t));
        P.MU.spmstore_C(spmAddr + i * sizeof(int32_t), val);

        // printf("[SPM] [DEBUG] value = %d", val);
        // printf("[SPM] [DEBUG] value1 = %d", val1);



    }
} else {
    assert(0 && "Invalid SPM address");
}

P.MU.numLoaded += size;
// printf("[INFO] NUM OF DATA LOADED UPDATED: %u\n", P.MU.numLoaded);