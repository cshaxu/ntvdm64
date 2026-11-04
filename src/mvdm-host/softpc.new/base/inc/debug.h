/*
 * SccsID = @(#)debug.h	1.5 01/23/95 Copyright Insignia Solutions Ltd.
 *
 * The following values are used to indicate which ega setting decided that
 * the display should be disabled. The global variable display_disabled
 * should be 1 (or more) of these values.
 *
 */

#define	ASYNC_RESET		1
#define	SYNC_RESET		2
#define	VIDEO_DRIVERS_DISABLED	4

#ifndef	YES
#define	YES	1
#define	NO	0
#endif

#ifdef	PROD
#define	NON_PROD(x)
#define	PROD_ONLY(x)	x
#else
#line 23
#error DIVERGENCE MVDM-HOST-DIV-617: unsupported alternate host profile
#endif
#line 26

#ifndef	PROD
#line 28
#error DIVERGENCE MVDM-HOST-DIV-504: non-PROD header branch is not selected; see mirror register.
#else /* PROD */
#line 30

#ifdef HUNTER
#line 297
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif /* HUNTER */
#line 300

#define	init_debugging()
#define	note_entrance0(str)
#define	note_entrance1(str,p1)
#define	note_entrance2(str,p1,p2)
#define	note_entrance3(str,p1,p2,p3)
#define	note_entrance4(str,p1,p2,p3,p4)
#define note_write_state0(str)
#define note_write_state1(str,p1)
#define note_write_state2(str,p1,p2)
#define note_write_state3(str,p1,p2,p3)
#define	note_display_state0(str)
#define	note_display_state1(str,p1)
#define	note_display_state2(str,p1,p2)
#define	note_display_state3(str,p1,p2,p3)
#define	do_display_trace(str,thing_to_do)
#define	note_trace0(trace_bit,str)
#define	note_trace1(trace_bit,str,p0)
#define	note_trace2(trace_bit,str,p0,p1)
#define	note_trace3(trace_bit,str,p0,p1,p2)
#define	note_trace4(trace_bit,str,p0,p1,p2,p3)
#define	note_trace5(trace_bit,str,p0,p1,p2,p3,p4)
#define	note_trace6(trace_bit,str,p0,p1,p2,p3,p4,p5)
#define	note_trace7(trace_bit,str,p0,p1,p2,p3,p4,p5,p6)
#define	note_trace8(trace_bit,str,p0,p1,p2,p3,p4,p5,p6,p7)
#define	always_trace0(str)
#define	always_trace1(str,p0)
#define	always_trace2(str,p0,p1)
#define	always_trace3(str,p0,p1,p2)
#define	always_trace4(str,p0,p1,p2,p3)
#define	always_trace5(str,p0,p1,p2,p3,p4)
#define	always_trace6(str,p0,p1,p2,p3,p4,p5)
#define	always_trace7(str,p0,p1,p2,p3,p4,p5,p6)
#define	always_trace8(str,p0,p1,p2,p3,p4,p5,p6,p7)
#define	note_trace0_no_nl(trace_bit,str)
#define	note_trace1_no_nl(trace_bit,str,p0)
#define	note_trace2_no_nl(trace_bit,str,p0,p1)
#define	sure_note_trace0(trace_bit,str)
#define	sure_note_trace1(trace_bit,str,p0)
#define	sure_note_trace2(trace_bit,str,p0,p1)
#define	sure_note_trace3(trace_bit,str,p0,p1,p2)
#define	sure_note_trace4(trace_bit,str,p0,p1,p2,p3)
#define	sure_note_trace5(trace_bit,str,p0,p1,p2,p3,p4)
#define	sure_note_trace6(trace_bit,str,p0,p1,p2,p3,p4,p5)
#define	sure_note_trace7(trace_bit,str,p0,p1,p2,p3,p4,p5,p6)
#define	sure_note_trace8(trace_bit,str,p0,p1,p2,p3,p4,p5,p6,p7)
#define	sub_note_trace0(trace_bit,str)
#define	sub_note_trace1(trace_bit,str,p0)
#define	sub_note_trace2(trace_bit,str,p0,p1)
#define	sub_note_trace3(trace_bit,str,p0,p1,p2)
#define	sub_note_trace4(trace_bit,str,p0,p1,p2,p3)
#define	sub_note_trace5(trace_bit,str,p0,p1,p2,p3,p4)
#define	sub_note_trace6(trace_bit,str,p0,p1,p2,p3,p4,p5)
#define	sub_note_trace7(trace_bit,str,p0,p1,p2,p3,p4,p5,p6)
#define	sub_note_trace8(trace_bit,str,p0,p1,p2,p3,p4,p5,p6,p7)
#define	sure_sub_note_trace0(trace_bit,str)
#define	sure_sub_note_trace1(trace_bit,str,p0)
#define	sure_sub_note_trace2(trace_bit,str,p0,p1)
#define	sure_sub_note_trace3(trace_bit,str,p0,p1,p2)
#define	sure_sub_note_trace4(trace_bit,str,p0,p1,p2,p3)
#define	sure_sub_note_trace5(trace_bit,str,p0,p1,p2,p3,p4)
#define	sure_sub_note_trace6(trace_bit,str,p0,p1,p2,p3,p4,p5)
#define	sure_sub_note_trace7(trace_bit,str,p0,p1,p2,p3,p4,p5,p6)
#define	sure_sub_note_trace8(trace_bit,str,p0,p1,p2,p3,p4,p5,p6,p7)

#define	assert0(test,str)
#define	assert1(test,str,p1)
#define	assert2(test,str,p1,p2)
#define	assert3(test,str,p1,p2,p3)
#define	assert4(test,str,p1,p2,p3,p4)
#define	assert5(test,str,p1,p2,p3,p4,p5)
#define	assert6(test,str,p1,p2,p3,p4,p5,p6)
#define	assert7(test,str,p1,p2,p3,p4,p5,p6,p7)
#define	assert8(test,str,p1,p2,p3,p4,p5,p6,p7,p8)

#define	assert0_do(test,str,do)
#define	assert1_do(test,str,p1,do)
#define	assert2_do(test,str,p1,p2,do)
#define	assert3_do(test,str,p1,p2,p3,do)
#define	assert4_do(test,str,p1,p2,p3,p4,do)
#define	assert5_do(test,str,p1,p2,p3,p4,p5,do)
#define	assert6_do(test,str,p1,p2,p3,p4,p5,p6,do)
#define	assert7_do(test,str,p1,p2,p3,p4,p5,p6,p7,do)
#define	assert8_do(test,str,p1,p2,p3,p4,p5,p6,p7,p8,do)

#endif /* PROD */
#line 386
