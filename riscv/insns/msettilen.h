require_extension(EXT_MATRIX);
sreg_t rs = sext_xlen(READ_REG(insn.rs1()));
P.MU.set_tile_n(rs);