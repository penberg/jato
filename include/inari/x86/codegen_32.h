#ifndef INARI_X86_CODEGEN_32_H
#define INARI_X86_CODEGEN_32_H

/*
 * Registers:
 */
enum inari_x86_reg {
	/*
	 * 32-bit:
	 */
	INARI_X86_EAX			= 0x00,
	INARI_X86_ECX			= 0x01,
	INARI_X86_EDX			= 0x02,
	INARI_X86_EBX			= 0x03,
	INARI_X86_ESP			= 0x04,
	INARI_X86_EBP			= 0x05,
	INARI_X86_ESI			= 0x06,
	INARI_X86_EDI			= 0x07,

	/*
	 * 16-bit:
	 */
	INARI_X86_AX			= 0x00,
	INARI_X86_CX			= 0x01,
	INARI_X86_DX			= 0x02,
	INARI_X86_BX			= 0x03,
	INARI_X86_SP			= 0x04,
	INARI_X86_BP			= 0x05,
	INARI_X86_SI			= 0x06,
	INARI_X86_DI			= 0x07,

	/*
	 * 8-bit:
	 */
	INARI_X86_AL			= 0x00,
	INARI_X86_CL			= 0x01,
	INARI_X86_DL			= 0x02,
	INARI_X86_BL			= 0x03,
	INARI_X86_AH			= 0x04,
	INARI_X86_CH			= 0x05,
	INARI_X86_BH			= 0x06,
	INARI_X86_DH			= 0x07,
};

/*
 * Instruction prefixes:
 */
enum inari_x86_prefix {
	/*
	 * Lock and repeat:
	 */
	INARI_X86_LOCK_PREFIX		= 0xf0,
	INARI_X86_REPNE_PREFIX		= 0xf2,
	INARI_X86_REP_PREFIX		= 0xf3,

	/*
	 * Segment override:
	 */
	INARI_X86_CS_PREFIX		= 0x2e,
	INARI_X86_SS_PREFIX		= 0x36,
	INARI_X86_DS_PREFIX		= 0x3e,
	INARI_X86_ES_PREFIX		= 0x26,
	INARI_X86_FS_PREFIX		= 0x64,
	INARI_X86_GS_PREFIX		= 0x65,

	/*
	 * Branch hints:
	 */
	INARI_X86_UNLIKELY_PREFIX	= 0x2e,
	INARI_X86_LIKELY_PREFIX		= 0x3e,

	/*
	 * Operand-size override:
	 */
	INARI_X86_OPERAND_PREFIX	= 0x66,

	/*
	 * Address-size override:
	 */
	INARI_X86_ADDRESS_PREFIX	= 0x67,
};

static inline unsigned char
inari_x86_modrm(unsigned char mod, unsigned char reg_opc, unsigned char rm)
{
	return ((mod & 0x3) << 6) | ((reg_opc & 0x7) << 3) | (rm & 0x7);
}

#define inari_x86_emit8(code, imm)						\
	do {									\
		*(code++) = imm;						\
	} while (0)

#define inari_x86_emit32(code, imm)						\
	do {									\
		inari_x86_emit8(code, ((imm) & 0x000000ffUL) >> 0 );		\
		inari_x86_emit8(code, ((imm) & 0x0000ff00UL) >> 8 );		\
		inari_x86_emit8(code, ((imm) & 0x00ff0000UL) >> 16);		\
		inari_x86_emit8(code, ((imm) & 0xff000000UL) >> 24);		\
	} while (0)

#define inari_x86_prefix(code, prefix)						\
	do {									\
		inari_x86_emit8(code, (prefix));				\
	} while (0)

#define inari_x86_add_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x01);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_or_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x09);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_alu_membase_reg(code, opc, base, disp, reg_opc)		\
	do {									\
		unsigned char mod, rm;						\
		bool need_sib;							\
										\
		inari_x86_emit8(code, opc);					\
										\
		need_sib = (base == INARI_X86_ESP);				\
										\
		if (need_sib)							\
			rm = 0x04;						\
		else								\
			rm = base;						\
										\
		if ((disp) == 0 && (base) != INARI_X86_EBP)			\
			mod = 0;						\
		else if (is_imm_8(disp))					\
			mod = 1;						\
		else								\
			mod = 2;						\
										\
		inari_x86_emit8(code, inari_x86_modrm(mod, reg_opc, rm));	\
										\
		if (need_sib)							\
			inari_x86_emit8(code, x86_encode_sib(0, 4, base));	\
										\
		if (!disp)							\
			break;							\
										\
		if (is_imm_8(disp))						\
			inari_x86_emit8(code, disp);				\
		else 								\
			inari_x86_emit32(code, disp);				\
	} while (0)

#define inari_x86_add_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x03, base, disp, dst);		\
	} while (0)

#define inari_x86_or_membase_reg(code, base, disp, dst)				\
	do {									\
		inari_x86_alu_membase_reg(code, 0x0b, base, disp, dst);		\
	} while (0)

#define inari_x86_adc_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x13, base, disp, dst);		\
	} while (0)

#define inari_x86_sbb_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x1b, base, disp, dst);		\
	} while (0)

#define inari_x86_and_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x23, base, disp, dst);		\
	} while (0)

#define inari_x86_sub_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x3b, base, disp, dst);		\
	} while (0)

#define inari_x86_xor_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x33, base, disp, dst);		\
	} while (0)

#define inari_x86_cmp_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x3b, base, disp, dst);		\
	} while (0)

#define inari_x86_adc_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x11);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_and_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x21);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_sub_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x29);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_xor_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x31);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_cmp_reg_reg(code, src, dst)					\
	do {									\
		inari_x86_emit8(code, 0x39);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));		\
	} while (0)

#define inari_x86_push_reg(code, reg)						\
	do {									\
		inari_x86_emit8(code, 0x50 + (reg));				\
	} while (0)

#define inari_x86_pop_reg(code, reg)						\
	do {									\
		inari_x86_emit8(code, 0x58 + (reg));				\
	} while (0)

#define inari_x86_push_imm(code, imm)						\
	do {									\
		if (is_imm_8(imm)) {						\
			inari_x86_emit8(code, 0x6a);				\
			inari_x86_emit8(code, (unsigned char) (imm));		\
		} else {							\
			inari_x86_emit8(code, 0x68);				\
			inari_x86_emit32(code, imm);				\
		}								\
	} while (0)

#define inari_x86_alu_imm_reg(code, opc, imm, reg)				\
	do {									\
		if (is_imm_8(imm)) {						\
			inari_x86_emit8(code, 0x83);				\
			inari_x86_emit8(code, inari_x86_modrm(0x03, opc, reg));	\
			inari_x86_emit8(code, (unsigned char) (imm));		\
		} else {							\
			inari_x86_emit8(code, 0x81);				\
			inari_x86_emit8(code, inari_x86_modrm(0x03, opc, reg));	\
			inari_x86_emit32(code, imm);				\
		}								\
	} while (0)

#define inari_x86_add_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x00, imm, reg);			\
	} while (0)

#define inari_x86_or_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x01, imm, reg);			\
	} while (0)

#define inari_x86_adc_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x02, imm, reg);			\
	} while (0)

#define inari_x86_sbb_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x03, imm, reg);			\
	} while (0)

#define inari_x86_and_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x04, imm, reg);			\
	} while (0)

#define inari_x86_sub_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x05, imm, reg);			\
	} while (0)

#define inari_x86_xor_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x06, imm, reg);			\
	} while (0)

#define inari_x86_cmp_imm_reg(code, imm, reg)					\
	do {									\
		inari_x86_alu_imm_reg(code, 0x07, imm, reg);			\
	} while (0)

#define inari_x86_test_membase_reg(code, base, disp, dst)			\
	do {									\
		inari_x86_alu_membase_reg(code, 0x85, base, disp, dst);		\
	} while (0)

#define inari_x86_mov_reg_reg(code, src, dst, size)				\
	do {									\
		switch (size) {							\
		case 1:								\
			inari_x86_emit8(code, 0x88);				\
			inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));	\
			break;							\
		case 2:								\
			inari_x86_emit8(code, INARI_X86_OPERAND_PREFIX);	\
			/* fall-through */					\
		case 4:								\
			inari_x86_emit8(code, 0x89);				\
			inari_x86_emit8(code, inari_x86_modrm(0x03, src, dst));	\
			break;							\
		default:							\
			assert(0);						\
		}								\
	} while (0)

#define inari_x86_nop(code)							\
	do {									\
		inari_x86_emit8(code, 0x90);					\
	} while (0)

#define inari_x86_cdq(code)							\
	do {									\
		inari_x86_emit8(code, 0x99);					\
	} while (0)

#define inari_x86_mov_imm_reg(code, imm, reg)					\
	do {									\
		unsigned long _imm = (unsigned long) imm;			\
		inari_x86_emit8(code, 0xb8 + reg);				\
		inari_x86_emit32(code, _imm);					\
	} while (0)

#define inari_x86_ret(code)							\
	do {									\
		inari_x86_emit8(code, 0xc3);					\
	} while (0)

#define inari_x86_leave(code)							\
	do {									\
		inari_x86_emit8(code, 0xc9);					\
	} while (0)

#define inari_x86_call(code, target)						\
	do {									\
		unsigned long disp = (void *) (target) - (void *) (code) - 5;	\
										\
		inari_x86_emit8(code, 0xe8);					\
		inari_x86_emit32(code, disp);					\
	} while (0)

#define inari_x86_jmp(code, target)						\
	do {									\
		unsigned long disp = (void *) (target) - (void *) (code) - 5;	\
										\
		inari_x86_emit8(code, 0xe9);					\
		inari_x86_emit32(code, disp);					\
	} while (0)

#define inari_x86_jne(code, target)						\
	do {									\
		unsigned long disp = (void *) (target) - (void *) (code) - 5;	\
										\
		inari_x86_emit8(code, 0x0f);					\
		inari_x86_emit8(code, 0x85);					\
		inari_x86_emit32(code, disp);					\
	} while (0)

#define inari_x86_call_regp(code, reg)						\
	do {									\
		inari_x86_emit8(code, 0xff);					\
		inari_x86_emit8(code, inari_x86_modrm(0x00, 2, reg));		\
	} while (0)

#define inari_x86_push_membase(code, base, disp)				\
	do {									\
		inari_x86_alu_membase_reg(code, 0xff, base, disp, 6);		\
	} while (0)

/*
 * SSE:
 */

#define inari_x86_sse_reg_reg(code, opc1, opc2, opc3, src, dst)			\
	do {									\
		inari_x86_emit8(code, opc1);					\
		inari_x86_emit8(code, opc2);					\
		inari_x86_emit8(code, opc3);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, dst, src));		\
	} while (0)

#define inari_x86_sse_reg_reg_opc2(code, opc1, opc2, src, dst)			\
	do {									\
		inari_x86_emit8(code, opc1);					\
		inari_x86_emit8(code, opc2);					\
		inari_x86_emit8(code, inari_x86_modrm(0x03, dst, src));		\
	} while (0)

#define inari_x86_sse_addsd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf2, 0x0f, 0x58, src, dst)
#define inari_x86_sse_addss_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf3, 0x0f, 0x58, src, dst)
#define inari_x86_sse_divsd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf2, 0x0f, 0x5e, src, dst)
#define inari_x86_sse_divss_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf3, 0x0f, 0x5e, src, dst)
#define inari_x86_sse_movsd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf2, 0x0f, 0x10, src, dst)
#define inari_x86_sse_movss_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf3, 0x0f, 0x10, src, dst)
#define inari_x86_sse_mulsd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf2, 0x0f, 0x59, src, dst)
#define inari_x86_sse_mulss_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf3, 0x0f, 0x59, src, dst)
#define inari_x86_sse_subsd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf2, 0x0f, 0x5c, src, dst)
#define inari_x86_sse_subss_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0xf3, 0x0f, 0x5c, src, dst)
#define inari_x86_sse_xorpd_reg_reg(code, src, dst) inari_x86_sse_reg_reg(code, 0x66, 0x0f, 0x57, src, dst)
#define inari_x86_sse_xorps_reg_reg(code, src, dst) inari_x86_sse_reg_reg_opc2(code, 0x0f, 0x57, src, dst)

#define inari_x86_sse_membase_reg(code, opc1, opc2, opc3, base, disp, dst) \
	do {									\
		inari_x86_emit8(code, opc1);					\
		inari_x86_emit8(code, opc2);					\
		inari_x86_alu_membase_reg(code, opc3, base, disp, dst);		\
	} while (0)

#define inari_x86_sse_addsd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf2, 0x0f, 0x58, base, disp, dst)
#define inari_x86_sse_addss_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf3, 0x0f, 0x58, base, disp, dst)
#define inari_x86_sse_divsd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf2, 0x0f, 0x5e, base, disp, dst)
#define inari_x86_sse_divss_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf3, 0x0f, 0x5e, base, disp, dst)
#define inari_x86_sse_movsd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf2, 0x0f, 0x10, base, disp, dst)
#define inari_x86_sse_movss_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf3, 0x0f, 0x10, base, disp, dst)
#define inari_x86_sse_mulsd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf2, 0x0f, 0x59, base, disp, dst)
#define inari_x86_sse_mulss_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf3, 0x0f, 0x59, base, disp, dst)
#define inari_x86_sse_subsd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf2, 0x0f, 0x5c, base, disp, dst)
#define inari_x86_sse_subss_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0xf3, 0x0f, 0x5c, base, disp, dst)
#define inari_x86_sse_xorpd_membase_reg(code, base, disp, dst) inari_x86_sse_membase_reg(code, 0x66, 0x0f, 0x57, base, disp, dst)

#endif
