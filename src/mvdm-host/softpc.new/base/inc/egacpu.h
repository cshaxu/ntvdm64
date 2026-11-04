
/*
 * SccsID = @(#)egacpu.h	1.22 10/24/94 Copyright Insignia Solutions Ltd.
 */

typedef enum
{
	WRITE_MODE,
	PLANES_ENABLED,
	ROTATION,
	CHAINED,
	BIT_PROT,
	FUNCTION,
	SET_RESET,
	ENABLE_SET_RESET,
	RAM_ENABLED,
	RAM_MOVED
} CHANGE_TYPE;

typedef union
{
#ifdef BIT_ORDER1
#line 23
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#else
	struct
	{
		unsigned pe:1;
		unsigned func:2;
		unsigned bp:1;
		unsigned sr:1;
		unsigned unused:27;
	} state;
	struct
	{
		unsigned lookup:5;
		unsigned unused:27;
	} mode_0;
	struct
	{
		unsigned lookup:1;
		unsigned unused:31;
	} mode_1;
	struct
	{
		unsigned lookup:4;
		unsigned unused:28;
	} mode_2;
	struct
	{
		unsigned lookup:4;
		unsigned unused:28;
	} mode_3;
#endif
} MAGIC;

typedef struct
{
	ULONG	latches;
	UTINY	*VGA_rplane;
	UTINY	*VGA_wplane;
	UTINY	*scratch;
	ULONG	sr_masked_val;
	ULONG	sr_nmask;
	ULONG	data_and_mask;
	ULONG	data_xor_mask;
	ULONG	latch_xor_mask;
	ULONG	bit_prot_mask;
	ULONG	plane_enable;
	ULONG	plane_enable_mask;
	ULONG	*sr_lookup;
	VOID	(*fwd_str_read_addr)();
	VOID	(*bwd_str_read_addr)();
	ULONG	dirty_flag;
	LONG	dirty_low;
	LONG	dirty_high;
	IU8	*video_copy;
	VOID	(*mark_byte)();
	VOID	(*mark_word)();
	VOID	(*mark_string)();
	ULONG	read_shift_count;
	ULONG	read_mapped_plane;
	ULONG	colour_comp;
	ULONG	dont_care;
	ULONG	v7_vid_copy_off;
	ULONG	copy_func_pbp;
	UTINY	*route_reg1;
	UTINY	*route_reg2;
	UTINY	*screen_ptr;
	ULONG	rotate;
} VGA_GLOBALS;

#ifdef C_VID
struct VGAGLOBALSETTINGS;
#endif

extern struct EGA_CPU_GLOBALS
{
#ifndef	HOST_VGA_GLOBALS
	/* DIVERGENCE(MVDM-HOST-DIV-062): the original C-video profile stores its
	 * generated VGAGLOBALSETTINGS carrier here.  NT4's permissive C accepted
	 * assignment to the sibling VGA_GLOBALS view; retain that view for other
	 * profiles, but give C_VID its actual generated source type. */
#ifdef C_VID
	struct VGAGLOBALSETTINGS	*globals;
#else
	VGA_GLOBALS	*globals;
#endif
#endif
	ULONG		saved_state;		/* Last value of EGA_CPU.ega_state.state */
	ULONG		saved_mode_chain;		/* Last value of mode/chain combined */
	MAGIC		ega_state;
	ULONG		fun_or_protection;   /* true means write function is 1-3 and/or there
						is bit protection, so latches must be used */
	ULONG		calc_data_xor;	/* Used to recalculate data_xor_mask when the
									bit prot reg changes */
	ULONG		calc_latch_xor;	/* Used to recalculate latch_xor_mask when the
									bit prot reg changes */
	ULONG		set_reset;
	ULONG		sr_enable;
	ULONG		sr_value;
	ULONG		ram_enabled;

	UTINY		write_mode;
	UTINY		chain;
	UTINY		doubleword;
	UTINY		*plane_offset;
	UTINY		*read_mapped_plane_ch2;
#ifdef V7VGA
	UTINY		seq_chain4;
	UTINY		seq_chain;
#endif
} EGA_CPU;

#ifndef CPU_40_STYLE	/* Vglobs done via access fns */
#line 150
#error DIVERGENCE MVDM-HOST-DIV-514: non-CPU40 branch is not selected for these consumers; see mirror register.
#else	/* CPU_40_STYLE */
#line 137

extern void setVideorplane IPT1(IU8 *, value);
extern IU8 * getVideorplane IPT0();
extern void setVideowplane IPT1(IU8 *, value);
extern IU8 * getVideowplane IPT0();
extern void setVideoscratch IPT1(IU8 *, value);
extern IU8 * getVideoscratch IPT0();
extern void setVideosr_masked_val IPT1(IU32, value);
extern IU32 getVideosr_masked_val IPT0();
extern void setVideosr_nmask IPT1(IU32, value);
extern IU32 getVideosr_nmask IPT0();
extern void setVideodata_and_mask IPT1(IU32, value);
extern IU32 getVideodata_and_mask IPT0();
extern void setVideodata_xor_mask IPT1(IU32, value);
extern IU32 getVideodata_xor_mask IPT0();
extern void setVideolatch_xor_mask IPT1(IU32, value);
extern IU32 getVideolatch_xor_mask IPT0();
extern void setVideobit_prot_mask IPT1(IU32, value);
extern IU32 getVideobit_prot_mask IPT0();
extern void setVideoplane_enable IPT1(IU32, value);
extern IU32 getVideoplane_enable IPT0();
extern void setVideoplane_enable_mask IPT1(IU32, value);
extern IU32 getVideoplane_enable_mask IPT0();
extern void setVideosr_lookup IPT1(IUH *, value);
extern IUH * getVideosr_lookup IPT0();
extern void setVideofwd_str_read_addr IPT1(IUH *, value);
extern IUH * getVideofwd_str_read_addr IPT0();
extern void setVideobwd_str_read_addr IPT1(IUH *, value);
extern IUH * getVideobwd_str_read_addr IPT0();
extern void setVideodirty_total IPT1(IU32, value);
extern IU32 getVideodirty_total IPT0();
extern void setVideodirty_low IPT1(IS32, value);
extern IS32 getVideodirty_low IPT0();
extern void setVideodirty_high IPT1(IS32, value);
extern IS32 getVideodirty_high IPT0();
extern void setVideovideo_copy IPT1(IU8 *, value);
extern IU8 * getVideovideo_copy IPT0();
extern void setVideomark_byte IPT1(IUH *, value);
extern IUH * getVideomark_byte IPT0();
extern void setVideomark_word IPT1(IUH *, value);
extern IUH * getVideomark_word IPT0();
extern void setVideomark_string IPT1(IUH *, value);
extern IUH * getVideomark_string IPT0();
extern void setVideoread_shift_count IPT1(IU32, value);
extern IU32 getVideoread_shift_count IPT0();
extern void setVideoread_mapped_plane IPT1(IU32, value);
extern IU32 getVideoread_mapped_plane IPT0();
extern void setVideocolour_comp IPT1(IU32, value);
extern IU32 getVideocolour_comp IPT0();
extern void setVideodont_care IPT1(IU32, value);
extern IU32 getVideodont_care IPT0();
extern void setVideov7_bank_vid_copy_off IPT1(IU32, value);
extern IU32 getVideov7_bank_vid_copy_off IPT0();
extern void setVideoscreen_ptr IPT1(IU8 *, value);
extern IU8 * getVideoscreen_ptr IPT0();
extern void setVideorotate IPT1(IU32, value);
extern IU32 getVideorotate IPT0();
extern void setVideocalc_data_xor IPT1(IU32, value);
extern IU32 getVideocalc_data_xor IPT0();
extern void setVideocalc_latch_xor IPT1(IU32, value);
extern IU32 getVideocalc_latch_xor IPT0();
extern void setVideoread_byte_addr IPT1(IUH *, value);
extern IUH * getVideoread_byte_addr IPT0();
extern void setVideov7_fg_latches IPT1(IU32, value);
extern IU32 getVideov7_fg_latches IPT0();
extern void setVideoGC_regs IPT1(IUH **, value);
extern IUH ** getVideoGC_regs IPT0();
extern void setVideolast_GC_index IPT1(IU8, value);
extern IU8 getVideolast_GC_index IPT0();
extern void setVideodither IPT1(IU8, value);
extern IU8 getVideodither IPT0();
extern void setVideowrmode IPT1(IU8, value);
extern IU8 getVideowrmode IPT0();
extern void setVideochain IPT1(IU8, value);
extern IU8 getVideochain IPT0();
extern void setVideowrstate IPT1(IU8, value);
extern IU8 getVideowrstate IPT0();

#include "evidgen.h"	/* generated by MkCpuInt */

#ifdef C_VID
extern struct VideoVector C_Video;	/* in (generated) vglfunc.c */

#undef  getVideolatches
#define getVideolatches()	((*(C_Video.GetVideolatches))())
#undef	setVideolatches
#define	setVideolatches(value)	( (*(C_Video.SetVideolatches))(value))
#undef	SetWritePointers
#define SetWritePointers	setWritePointers
#undef	SetReadPointers
#define SetReadPointers	setReadPointers
#undef	SetMarkPointers
#define SetMarkPointers	setMarkPointers
#endif	/* C_VID */
#endif	/* CPU_40_STYLE */
#line 232

#define write_state EGA_CPU.ega_state.state

#define UNCHAINED	0
#define CHAIN2		1
#define CHAIN4		2

IMPORT ULONG sr_lookup[16];

#define N_WRITE_TYPES 24

#ifdef CPU_40_STYLE
extern IU32 latchval;	/* used for following latch macros */
#define get_latch(n) \
	(latchval = getVideolatches(),\
	(* ((UTINY *) (&latchval) + n)))

#define get_latch0 get_latch(0)
#define get_latch1 get_latch(1)
#define get_latch2 get_latch(2)
#define get_latch3 get_latch(3)
#define get_latch01 \
	(latchval = getVideolatches(),  \
	(* (USHORT *) (&latchval)))

#define get_latch23 \
	(latchval = getVideolatches(),  \
	(* (USHORT *) (&latchval + 2)))

#define put_latch(n, value) \
	(* ((UTINY *) (&latchval) + n) = (value), \
	setVideolatches(latchval))

#define put_latch0(value) put_latch(0, value)
#define put_latch1(value) put_latch(1, value)
#define put_latch2(value) put_latch(2, value)
#define put_latch3(value) put_latch(3, value)

#else	/* CPU_40_STYLE */
#line 372
#error DIVERGENCE MVDM-HOST-DIV-514: non-CPU40 branch is not selected for these consumers; see mirror register.
#endif	/* CPU_40_STYLE */

/*
 * macro to do the logical operations on cpu data and the latch values
 */

#define do_logicals(val,latch)  (((latch) & ((val & getVideodata_and_mask()) \
		^ getVideodata_xor_mask())) | (val & ((latch) ^ getVideolatch_xor_mask())))


/* Routines */

extern void ega_mode0_gen_chn_b_write IPT1(byte *,arg1);
extern void ega_mode0_gen_chn_w_write IPT1(byte *,arg1);
extern void ega_mode0_gen_chn_b_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode0_gen_chn_w_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode0_gen_chn_b_move IPT2(byte *,arg1, byte *,arg2);

extern void ega_mode1_gen_chn_b_write IPT1(byte *,arg1);
extern void ega_mode1_gen_chn_w_write IPT1(byte *,arg1);
extern void ega_mode1_gen_chn_b_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode1_gen_chn_w_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode1_gen_chn_b_move IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode1_gen_chn_w_move IPT2(byte *,arg1, byte *,arg2);

extern void ega_mode2_gen_chn_b_write IPT1(byte *,arg1);
extern void ega_mode2_gen_chn_w_write IPT1(byte *,arg1);
extern void ega_mode2_gen_chn_b_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode2_gen_chn_w_fill IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode2_gen_chn_b_move IPT2(byte *,arg1, byte *,arg2);
extern void ega_mode2_gen_chn_w_move IPT2(byte *,arg1, byte *,arg2);

/* 1 plane masked write */
extern void ega_mode0_copy1_mask_b_write0 IPT1(byte *,arg1);
extern void ega_mode0_copy1_mask_b_write1 IPT1(byte *,arg1);
extern void ega_mode0_copy1_mask_b_write2 IPT1(byte *,arg1);
extern void ega_mode0_copy1_mask_b_write3 IPT1(byte *,arg1);

#ifdef VGG
extern boolean vga_mode0_gen_chn4_b_write IPT0();
extern boolean vga_mode0_gen_chn4_w_write IPT0();
extern boolean vga_mode0_gen_chn4_b_fill IPT0();
extern boolean vga_mode0_gen_chn4_w_fill IPT0();
extern boolean vga_mode0_gen_chn4_b_move IPT0();
/* wr 1 */
extern boolean vga_mode1_gen_chn4_b_write IPT0();
extern boolean vga_mode1_gen_chn4_w_write IPT0();
extern boolean vga_mode1_gen_chn4_b_fill IPT0();
extern boolean vga_mode1_gen_chn4_w_fill IPT0();
extern boolean vga_mode1_gen_chn4_b_move IPT0();
extern boolean vga_mode1_gen_chn4_w_move IPT0();
/* wr 2 */
extern boolean vga_mode2_gen_chn4_b_write IPT0();
extern boolean vga_mode2_gen_chn4_w_write IPT0();
extern boolean vga_mode2_gen_chn4_b_fill IPT0();
extern boolean vga_mode2_gen_chn4_w_fill IPT0();
extern boolean vga_mode2_gen_chn4_b_move IPT0();
extern boolean vga_mode2_gen_chn4_w_move IPT0();
/* wr 3 */
extern boolean vga_mode3_gen_chn4_b_write IPT0();
extern boolean vga_mode3_gen_chn4_w_write IPT0();
extern boolean vga_mode3_gen_chn4_b_fill IPT0();
extern boolean vga_mode3_gen_chn4_w_fill IPT0();
extern boolean vga_mode3_gen_chn4_b_move IPT0();
extern boolean vga_mode3_gen_chn4_w_move IPT0();

extern boolean vga_mode3_gen_chn_b_write IPT0();
extern boolean vga_mode3_gen_chn_w_write IPT0();
extern boolean vga_mode3_gen_chn_b_fill IPT0();
extern boolean vga_mode3_gen_chn_w_fill IPT0();
extern boolean vga_mode3_gen_chn_b_move IPT0();
extern boolean vga_mode3_gen_chn_w_move IPT0();

extern boolean vga_mode3_gen_b_write IPT0();
extern boolean vga_mode3_gen_w_write IPT0();
extern boolean vga_mode3_gen_b_fill IPT0();
extern boolean vga_mode3_gen_w_fill IPT0();
extern boolean vga_mode3_gen_b_move IPT0();
extern boolean vga_mode3_gen_w_move IPT0();
#endif /* VGG */
