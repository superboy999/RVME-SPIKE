#ifndef _RISCV_MATRIX_UNIT_H
#define _RISCV_MATRIX_UNIT_H
#include "vector_unit.h"

typedef int8_t elem_t;
typedef int32_t acc_t;

class matrixUnit_t {
    public:
        processor_t* p;
        static constexpr uint32_t NUM_MR = 4;
        static constexpr uint32_t MAX_M = 8;
        static constexpr uint32_t MAX_K = 32;
        static constexpr uint32_t MAX_N = 8;
        static constexpr uint32_t SPM_A_ROWS = 2048 + 4;
        static constexpr uint32_t SPM_B_ROWS = 2048;
        static constexpr uint32_t SPM_C_ROWS = 4096 + 32;
        static constexpr uint32_t SPM_WIDTH = 32; //in bytes
        elem_t SPM_A[SPM_A_ROWS][SPM_WIDTH/sizeof(elem_t)]; // SPM_A_ROWS x 32 x int8
        elem_t SPM_B[SPM_B_ROWS][SPM_WIDTH/sizeof(elem_t)]; // SPM_B_ROWS x 32 x int8
        acc_t  SPM_C[SPM_C_ROWS][SPM_WIDTH/sizeof(acc_t)];  // SPM_C_ROWS x 8 x int32
        elem_t tile_reg[NUM_MR][MAX_M][MAX_K]; // 4 tiles, each tile is 8x32
        acc_t acc_reg[NUM_MR][MAX_M][MAX_N]; // 4 tiles, each tile is 8x8
        uint32_t misa, tile_m, tile_k, tile_n, mcsr, xmxrm, xmsat, xmfflags, xmfrm, xmsaten, numLoaded, numStored;

        
        void reset();
        void set_tile_m(int m_value);
        void set_tile_k(int k_value);
        void set_tile_n(int n_value);
        void print_tile(int tile_idx);
        void print_acc(int tile_idx);
        uint8_t spmload_A(uint64_t addr);
        uint8_t spmload_B(uint64_t addr);
        uint32_t spmload_C(uint64_t addr);
        void spmstore_A(uint64_t addr, int8_t value);
        void spmstore_B(uint64_t addr, int8_t value);
        void spmstore_C(uint64_t addr, int32_t value);
};


#endif // _RISCV_MATRIX_UNIT_H