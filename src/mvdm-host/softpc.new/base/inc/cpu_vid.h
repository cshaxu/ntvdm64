/*[
======================================================================

				 SoftPC Revision 3.0

 Title:
		cpu_vid.h

 Description:

		This module supports the interface between the cpu
		and the video emulation code.

 Author:
		John Shanly

 Date:
		12 April 1991

 SccsID		@(#)cpu_vid.h	1.11 03/09/94

	(c)Copyright Insignia Solutions Ltd., 1991. All rights reserved.

======================================================================
]*/

typedef struct
{
	void	(*b_write) IPT2(ULONG, value, ULONG, offset);
	void	(*w_write) IPT2(ULONG, value, ULONG, offset);

#ifndef	NO_STRING_OPERATIONS

	void	(*b_fill) IPT3(ULONG, value, ULONG, offset, ULONG, count);
	void	(*w_fill) IPT3(ULONG, value, ULONG, offset, ULONG, count);
	void	(*b_fwd_move) IPT4(ULONG, offset, ULONG, eas, ULONG, count,
		ULONG, src_flag);
	void	(*b_bwd_move) IPT4(ULONG, offset, ULONG, eas, ULONG, count,
		ULONG, src_flag);
	void	(*w_fwd_move) IPT4(ULONG, offset, ULONG, eas, ULONG, count,
		ULONG, src_flag);
	void	(*w_bwd_move) IPT4(ULONG, offset, ULONG, eas, ULONG, count,
		ULONG, src_flag);

#endif	/* NO_STRING_OPERATIONS */

} WRT_POINTERS;

typedef struct
{
	void	(*b_write) IPT2(IU8, eaOff, IU32, eaVal);
	void	(*w_write) IPT2(IU16, eaOff, IU32, eaVal);
	void	(*d_write) IPT2(IU32, eaOff, IU32, eaVal);
	void	(*b_fill) IPT3(IU32, eaOff, IU8, eaVal, IU32, count);
	void	(*w_fill) IPT3(IU32, eaOff, IU16, eaVal, IU32, count);
	void	(*d_fill) IPT3(IU32, eaOff, IU32, eaVal, IU32, count);
	void	(*b_fwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
	void	(*b_bwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
	void	(*w_fwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
	void	(*w_bwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
	void	(*d_fwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
	void	(*d_bwd_move) IPT4(IU32, eaVal, IHPE, fromOff, IU32, count, IBOOL, src_flag);
} EVID_WRT_POINTERS;

typedef struct
{
	WRT_POINTERS	mode_0[32];
	WRT_POINTERS	mode_1[2];
	WRT_POINTERS	mode_2[16];
#ifdef VGG
	WRT_POINTERS	mode_3[16];
#endif
} CHN_TABLE;

typedef struct
{
	CHN_TABLE	nch;
#ifdef VGG
	CHN_TABLE	ch4;
#endif
} MODE_TABLE;

extern MODE_TABLE mode_table;

typedef struct 
{
#if defined(CPU_40_STYLE) && defined(C_VID)
	/* DIVERGENCE MVDM-HOST-DIV-089: selected C-video EVID glue uses the
	 * exact IU32 offset/count contract below.  The historical ULONG carrier
	 * only matched it by 32-bit typedef coincidence. */
	IU32 (*b_read) IPT1(IU32, eaOff);
	IU32 (*w_read) IPT1(IU32, eaOff);
#else
#error DIVERGENCE MVDM-HOST-DIV-618: unsupported generic video callback profile
#endif
#line 90
#ifndef	NO_STRING_OPERATIONS
#if defined(CPU_40_STYLE) && defined(C_VID)
#line 95
	void (*str_read) IPT3(IU8 *, dest, IU32, eaOff, IU32, count);
#else
#line 102
	void (*str_read) IPT3(IU8 *, dest, ULONG, offset, ULONG, count);
#endif
#endif	/* NO_STRING_OPERATIONS */
#line 105

} READ_POINTERS; 

typedef struct 
{
	IU32 (*b_read) IPT1(IU32, eaOff);
	IU32 (*w_read) IPT1(IU32, eaOff);
	IU32 (*d_read) IPT1(IU32, eaOff);
	void (*str_fwd_read) IPT3(IU8 *, dest, IU32, eaOff, IU32, count);
	void (*str_bwd_read) IPT3(IU8 *, dest, IU32, eaOff, IU32, count);
} EVID_READ_POINTERS; 

extern READ_POINTERS read_pointers;

#ifndef Cpu_set_vid_wrt_ptrs
extern void Cpu_set_vid_wrt_ptrs IPT1( WRT_POINTERS *, ptrs );
extern void Cpu_set_vid_rd_ptrs IPT1( READ_POINTERS *, ptrs );
#endif /* Cpu_set_vid_wrt_ptrs */

#line 112
#ifndef CPU_40_STYLE
#line 113
#error DIVERGENCE MVDM-HOST-DIV-514: non-CPU40 branch is not selected for these consumers; see mirror register.
#endif /* CPU_40_STYLE */
#line 127
#line 127

extern WRT_POINTERS simple_writes;
extern WRT_POINTERS dth_md0_writes, dth_md1_writes, dth_md2_writes, dth_md3_writes;
extern WRT_POINTERS ch2_md0, ch2_md1, ch2_md2, ch2_md3, ch2_mdcopy;

extern READ_POINTERS Glue_reads;

extern READ_POINTERS simple_reads, pointers_RAM_off;
extern READ_POINTERS pointers_mode0_nch, pointers_mode0_ch4, pointers_mode0_ch2;
extern READ_POINTERS pointers_mode1_nch, pointers_mode1_ch4, pointers_mode1_ch2;
