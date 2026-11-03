
/*
 * VPC-XT Revision 1.0
 *
 * Title	: Colour Graphics Adaptor declarations
 *
 * Description	: Definitions for users of the CGA
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 */

/* SccsID[]="@(#)cga.h	1.5 05/15/93 Copyright Insignia Solutions Ltd."; */

/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

#ifdef HUNTER
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif


#line 27
#ifdef BIT_ORDER1
#line 28
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 45

#ifdef BIT_ORDER2
typedef union                       /* Template for character attributes     */
{
    half_word all;
    struct 
    {
        HALF_WORD_BIT_FIELD foreground:3;   /* Foreground colour R,G,B               */
        HALF_WORD_BIT_FIELD bold      :1;   /* Intensity bit                         */
        HALF_WORD_BIT_FIELD background:3;   /* Background colour R,G,B               */
        HALF_WORD_BIT_FIELD blinking  :1;   /* Blinking attribute                    */
    } bits;
    struct 
    {
        HALF_WORD_BIT_FIELD foreground_and_bold :4;
        HALF_WORD_BIT_FIELD background_and_blink:4;     
    } plane;
} ATTRIBUTE;
#endif

/*
 * ============================================================================
 * External declarations and macros
 * ============================================================================
 */

#ifdef HUNTER
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif

extern void cga_init	IPT0();
extern void cga_term	IPT0();
extern void cga_inb	IPT2(io_addr, address, half_word *, value);
extern void cga_outb	IPT2(io_addr, address, half_word, value);

typedef	struct {
	int	mode;
	int	resolution;
	int	color_select;
	int	colormask;
} CGA_GLOBS;

extern	CGA_GLOBS	CGA_GLOBALS;

#define	set_cga_mode(val)		CGA_GLOBALS.mode = (val)
#define	set_cga_resolution(val)		CGA_GLOBALS.resolution = (val)
#define	set_cga_color_select(val)	CGA_GLOBALS.color_select = (val)
#define	set_cga_colormask(val)		CGA_GLOBALS.colormask = (val)

#define	get_cga_mode()			(CGA_GLOBALS.mode)
#define	get_cga_resolution()		(CGA_GLOBALS.resolution)
#define	get_cga_color_select()		(CGA_GLOBALS.color_select)
#define	get_cga_colormask()		(CGA_GLOBALS.colormask)

#line 100
#if !defined(EGG) && !defined(A_VID) && !defined(C_VID)
#line 101
#error Unsupported excluded original configuration
#endif	/* !EGG */
#line 113
#line 97
