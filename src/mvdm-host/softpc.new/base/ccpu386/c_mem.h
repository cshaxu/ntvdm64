/*[

c_mem.h

LOCAL CHAR SccsID[]="@(#)c_mem.h	1.8 02/28/95";

CPU to Memory interface.
------------------------

]*/

/* >>>>>>>>>>>>>>>> NON KOSHER FORM <<<<<<<<<<<<<<<< */

/* Use only for decoding the Intel Opcode Stream. */
/* DIRECT ACCESS to memory! */

IMPORT UTINY ccpu_new_code_page IPT1(UTINY **, q );

IMPORT UTINY *pg_end;	/* point up to which host may safely read
			   instruction stream bytes */

/* To return difference between two points in the inst. stream.
   n = new posn, o = old posn. */

#ifdef	PIG
#line 26
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#else	/* !PIG */
#line 29
#ifdef BACK_M
#error DIVERGENCE MVDM-HOST-DIV-525: BACK_M platform branch is not selected; see mirror register.
#else
#define DIFF_INST_BYTE(n, o) ((n) - (o))
#endif /* BACK_M */
#endif	/* PIG */
#line 35

/* To get next inst. byte and move pointer to next inst. byte. */
#ifdef	PIG
#line 38
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#else	/* !PIG */
#line 41
#ifdef BACK_M
#error DIVERGENCE MVDM-HOST-DIV-525: BACK_M platform branch is not selected; see mirror register.
#else
#define GET_INST_BYTE(x) \
   ( (x) >= pg_end ? ccpu_new_code_page(&(x)) : *(x)++ )
#endif /* BACK_M */
#endif	/* PIG */
#line 49


/* >>>>>>>>>>>>>>>> KOSHER FORM <<<<<<<<<<<<<<<< */

#ifdef PIG
#line 54
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#else
#line 66

#define phy_read_byte(x)	((IU8)(phy_r8((PHY_ADDR)x)))
#define phy_read_word(x)	((IU16)(phy_r16((PHY_ADDR)x)))
#define phy_read_dword(x)	((IU32)(phy_r32((PHY_ADDR)x)))

#define phy_write_byte(x, y)	phy_w8((PHY_ADDR)x, (IU8)y)
#define phy_write_word(x, y)	phy_w16((PHY_ADDR)x, (IU16)y)
#define phy_write_dword(x, y)	phy_w32((PHY_ADDR)x, (IU32)y)

#endif /* PIG */
#line 76
