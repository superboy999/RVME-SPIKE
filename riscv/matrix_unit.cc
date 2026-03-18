#include "/cluster/home/geyh/RV/xuantie_gnu_toolchain/test/rvme/include/config_v2_3.h"
#include "config.h"
#include "matrix_unit.h"
#include "processor.h"
#include "arith.h"
#include "math.h"

void matrixUnit_t::reset() {
    for (int t = 0; t < NUM_MR; t++) {
        for (int i = 0; i < MAX_M; i++) {
            for (int k = 0; k < MAX_K; k++) {
                tile_reg[t][i][k] = 0;
            }
        }
    }
    for (int t = 0; t < NUM_MR; t++) {
        for (int i = 0; i < MAX_M; i++) {
            for (int j = 0; j < MAX_N; j++) {
                acc_reg[t][i][j] = 0;
            }
        }
    }
    for (int i = 0; i < SPM_A_ROWS; i++) {
        for (int j = 0; j < SPM_WIDTH/sizeof(elem_t); j++) {
            SPM_A[i][j] = 0;
        }
    }
    for (int i = 0; i < SPM_B_ROWS; i++) {
        for (int j = 0; j < SPM_WIDTH/sizeof(elem_t); j++) {
            SPM_B[i][j] = 0;
        }
    }
    for (int i = 0; i < SPM_C_ROWS; i++) {
        for (int j = 0; j < SPM_WIDTH/sizeof(acc_t); j++) {
            SPM_C[i][j] = 0;
        }
    }
    // Reset CSR registers to default values
    misa = 0;
    tile_m = 8;
    tile_k = 32;
    tile_n = 8;
    mcsr = 0;
    xmxrm = 0;
    xmsat = 0;
    xmfflags = 0;
    xmfrm = 0;
    xmsaten = 0;
    numLoaded = 0;
    numStored = 0;
    fprintf(stderr, "Matrix Engine Reset CSR Registers: misa = %u, tile_m = %u, tile_k = %u, tile_n = %u, "
        "mcsr = %u, xmxrm = %u, xmsat = %u, xmfflags = %u, xmfrm = %u, xmsaten = %u, numLoaded = %u, numStored = %u\n",
        misa, tile_m, tile_k, tile_n, mcsr, xmxrm, xmsat, xmfflags, xmfrm, xmsaten, numLoaded, numStored);
    for (int t = 0; t < NUM_MR; t++) {
        print_tile(t);
    }
    for (int t = 0; t < NUM_MR; t++) {
        print_acc(t);
    }  
}

void matrixUnit_t::set_tile_m(int m_value) {
    tile_m = m_value > MAX_M ? MAX_M : m_value;
    fprintf(stderr, "Set tile_m to %u\n", tile_m);
}

void matrixUnit_t::set_tile_k(int k_value) {
    tile_k = k_value > MAX_K ? MAX_K : k_value;
    fprintf(stderr, "Set tile_k to %u\n", tile_k);
}

void matrixUnit_t::set_tile_n(int n_value) {
    tile_n = n_value > MAX_N ? MAX_N : n_value;
    fprintf(stderr, "Set tile_n to %u\n", tile_n);
}

void matrixUnit_t::print_tile(int reg_idx) {
    if (reg_idx < 0 || reg_idx >= NUM_MR) {
        fprintf(stderr, "Invalid reg index %d. Valid range is [0, %d].\n", reg_idx, NUM_MR - 1);
        return;
    }
    fprintf(stderr, "Tile Register %d:\n", reg_idx);
    for (int i = 0; i < MAX_M; i++) {
        for (int k = 0; k < MAX_K; k++) {
            fprintf(stderr, "%4d ", tile_reg[reg_idx][i][k]);
        }
        fprintf(stderr, "\n");
    }
}

void matrixUnit_t::print_acc(int reg_idx) {
    if (reg_idx < 0 || reg_idx >= NUM_MR) {
        fprintf(stderr, "Invalid reg index %d. Valid range is [0, %d].\n", reg_idx, NUM_MR - 1);
        return;
    }
    fprintf(stderr, "Accumulator Register %d:\n", reg_idx);
    for (int i = 0; i < MAX_M; i++) {
        for (int j = 0; j < MAX_N; j++) {
            fprintf(stderr, "%4d ", acc_reg[reg_idx][i][j]);
        }
        fprintf(stderr, "\n");
    }
}

uint8_t matrixUnit_t::spmload_A(uint64_t addr) {
    uint64_t maxaddr_A = SPM_A_ROWS * SPM_WIDTH;
    assert(addr < maxaddr_A && "SPM address out of range");
    // Load from SPM_A
    uint64_t row = (addr / SPM_WIDTH) % SPM_A_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(elem_t);
    fprintf(stderr, "Loading from SPM_A 0x%lx: SPM_A[%lu][%lu]=%d\n", addr, row, col, SPM_A[row][col]);
    return SPM_A[row][col];
}

uint8_t matrixUnit_t::spmload_B(uint64_t addr) {
    uint64_t minaddr_B = SPM_A_ROWS * SPM_WIDTH;
    uint64_t maxaddr_B = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH;
    assert(addr >= minaddr_B && addr < maxaddr_B && "SPM address out of range");
    // Load from SPM_B
    addr -= minaddr_B;
    uint64_t row = (addr / SPM_WIDTH) % SPM_B_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(elem_t);
    fprintf(stderr, "Loading from SPM_B 0x%lx: SPM_B[%lu][%lu]=%d\n", addr + minaddr_B, row, col, SPM_B[row][col]);
    return SPM_B[row][col];
}

uint32_t matrixUnit_t::spmload_C(uint64_t addr) {
    uint64_t minaddr_C = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH;
    uint64_t maxaddr_C = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH + SPM_C_ROWS * SPM_WIDTH + max_temp_size;
    assert(addr >= minaddr_C && addr < maxaddr_C && "SPM_C address out of range");
    addr -= minaddr_C;
    uint64_t row = (addr / SPM_WIDTH) % SPM_C_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(acc_t);
    fprintf(stderr, "Loading from SPM_C 0x%lx: SPM_C[%lu][%lu]=%d\n", addr + minaddr_C, row, col, SPM_C[row][col]);
    return SPM_C[row][col];
}

void matrixUnit_t::spmstore_A(uint64_t addr, int8_t value) {
    uint64_t maxaddr_A = SPM_A_ROWS * SPM_WIDTH;
    assert(addr < maxaddr_A && "SPM_A address out of range");
    // Store to SPM_A
    uint64_t row = (addr / SPM_WIDTH) % SPM_A_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(elem_t);
    SPM_A[row][col] = value;
    fprintf(stderr, "Storing to SPM_A 0x%lx: SPM_A[%lu][%lu]=%d\n", addr, row, col, value);
}

void matrixUnit_t::spmstore_B(uint64_t addr, int8_t value) {
    uint64_t minaddr_B = SPM_A_ROWS * SPM_WIDTH;
    uint64_t maxaddr_B = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH;
    assert(addr >= minaddr_B && addr < maxaddr_B && "SPM_B address out of range");
    // Store to SPM_B
    addr -= minaddr_B;
    uint64_t row = (addr / SPM_WIDTH) % SPM_B_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(elem_t);
    SPM_B[row][col] = value;
    fprintf(stderr, "Storing to SPM_B 0x%lx: SPM_B[%lu][%lu]=%d\n", addr + minaddr_B, row, col, value);
}

void matrixUnit_t::spmstore_C(uint64_t addr, int32_t value) {
    uint64_t minaddr_C = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH;
    uint64_t maxaddr_C = SPM_A_ROWS * SPM_WIDTH + SPM_B_ROWS * SPM_WIDTH + SPM_C_ROWS * SPM_WIDTH + max_temp_size;
    assert(addr >= minaddr_C && addr < maxaddr_C && "SPM_C address out of range");
    // Store to SPM_C
    addr -= minaddr_C;
    uint64_t row = (addr / SPM_WIDTH) % SPM_C_ROWS;
    uint64_t col = (addr % SPM_WIDTH) / sizeof(acc_t);
    SPM_C[row][col] = value;
    fprintf(stderr, "Storing to SPM_C 0x%lx: SPM_C[%lu][%lu]=%d\n", addr + minaddr_C, row, col, value);
}