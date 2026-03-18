require_extension(EXT_MATRIX);
sreg_t rs = sext_xlen(insn.uimm10());
P.MU.set_tile_k(rs);