#if (defined(JAPAN) || defined(KOREA)) && !defined(i386)
#line 2
#include <windows.h>
#endif // (JAPAN || KOREA)&& !i386
#line 4
#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: video.c
 *
 * Description	: BIOS video internal routines.
 *
 * Author	: Henry Nash
 *
 * Notes	: The following functions are defined in this module:
 *
 *                video_init()
 *
 *		  vd_set_mode()
 *		  vd_set_cursor_mode()
 *		  vd_set_cursor_position()
 *		  vd_get_cursor_position()
 *		  vd_get_light_pen()
 *		  vd_set_active_page()
 *		  vd_scroll_up()
 *		  vd_scroll_down()
 *		  vd_read_attrib_char()
 *		  vd_write_char_attrib()
 *		  vd_write_char()
 *		  vd_set_colour_palette()
 *		  vd_write_dot()
 *		  vd_read_dot()
 *		  vd_write_teletype()
 *		  vd_get_mode()
 *		  vd_write_string()
 *
 *		  The above vd_ functions are called by the video_io()
 *		  function via a function table.
 *
 */

/*
 * static char SccsID[]="@(#)video.c	1.61 07/03/95 Copyright Insignia Solutions Ltd.";
 */


#ifndef NEC_98
#line 48
#ifdef SEGMENTATION
#line 49
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 56
#endif // !NEC_98
#line 57


/*
 *    O/S include files.
 */
#include <stdio.h>
#include <malloc.h>
#include StringH
#include TypesH
#include FCntlH

/*
 * SoftPC include files
 */
#include "xt.h"
#include "sas.h"
#include CpuH
#include "error.h"
#include "config.h"
#include "bios.h"
#include "ios.h"
#include "gmi.h"
#include "gvi.h"
#include "gfx_upd.h"
#include "host.h"
#include "video.h"
#include "cga.h"
#ifdef	EGG
#line 85
#include "egacpu.h"
#include "egaports.h"
#endif	/* EGG */
#line 88
#include "equip.h"
#include "debug.h"
#include "timer.h"
#ifndef PROD
#line 92
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 94
#include "egavideo.h"
#include "host_gfx.h"
#include "cpu_vid.h"
#include "ga_defs.h"
#if defined(JAPAN) || defined(KOREA)
#line 99
#include <conapi.h>
#include "nt_graph.h" // for SetConsoleEUDC()
#include "egagraph.h" // for SetVram()
#endif // JAPAN || KOREA
#line 103

#if defined(NEC_98)
#line 105
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 107

#ifdef	EGG
#line 109
#define	VD_ROWS_ON_SCREEN	sas_hw_at_no_check(vd_rows_on_screen)
#else
#line 111
#error DIVERGENCE MVDM-HOST-DIV-540: non-EGG branch is not selected; see mirror register.
#endif	/* EGG */
#line 113


#ifndef NEC_98
#line 116
#ifdef NTVDM
#line 117
short		stream_io_dirty_count_32 = 0;
half_word  *	stream_io_buffer = NULL;
boolean 	stream_io_enabled = FALSE;
word		stream_io_buffer_size = 0;
word  * 	stream_io_dirty_count_ptr = NULL;
#ifdef MONITOR
#line 123
sys_addr	stream_io_bios_busy_sysaddr;
#endif
#line 125

#endif
#line 127
#endif // !NEC_98
#line 128



/*
 * ============================================================================
 * Global data
 * ============================================================================
 *
 * These variables are basically the same as the corresponding gvi_.. variables,
 * but reflect where the BIOS thinks the screen is, rather than where it really is.
 * This was done to fix "dots on screen" problem with EGA-PICS, which changes screen
 * mode behind the BIOS's back.
 */
#if defined(NEC_98)
#line 142
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 144
GLOBAL sys_addr video_pc_low_regen,video_pc_high_regen;
#endif // !NEC_98
#line 146

#if defined(JAPAN) || defined(KOREA)
#line 148
GLOBAL byte Int10Flag[80*50];
GLOBAL byte NtInt10Flag[80*50];

GLOBAL word DosvVramSeg;
GLOBAL word DosvVramOff;
GLOBAL word DosvModeSeg;
GLOBAL word DosvModeOff;
GLOBAL word NtConsoleFlagSeg;
GLOBAL word NtConsoleFlagOff;
GLOBAL word DispInitSeg;
GLOBAL word DispInitOff;
GLOBAL word FullScreenResumeOff;
GLOBAL word FullScreenResumeSeg;
GLOBAL sys_addr DosvVramPtr;
GLOBAL sys_addr DosvModePtr;
GLOBAL sys_addr NtConsoleFlagPtr;
GLOBAL sys_addr SetModeFlagPtr;
GLOBAL int PrevCP = 437;                  // default CP
GLOBAL int DosvVramSize;
#define DOSV_VRAM_SIZE 8000               // 8/6/1993 V-KazuyS
GLOBAL word textAttr;                   // for screen attributes
#endif
#line 170

#ifdef JAPAN
#line 172
GLOBAL int  Int10FlagCnt = 0;

// #4183: status line of oakv(DOS/V FEP) doesn't disappear -yasuho
GLOBAL half_word IMEStatusLines;
#endif
#define vd_text_mode() (vd_mode_table[sas_hw_at_no_check(vd_video_mode)])
#line 177

/*
 * ============================================================================
 * Local static data and defines
 * ============================================================================
 */

/* internal function declarations */
#if defined(NEC_98)
#line 186
#error Unsupported NEC PC-98 configuration
#line 263
#else  // !NEC_98
#line 264
LOCAL sys_addr 	extend_addr IPT1(sys_addr,addr);
LOCAL half_word fgcolmask IPT1(word, rawchar);
LOCAL word 	expand_byte IPT1(word, lobyte);
GLOBAL void 	graphics_write_char IPT5(half_word, x, half_word, y, half_word, wchar, half_word, attr, word, how_many);
LOCAL void 	M6845_reg_init IPT2(half_word, mode, word, base);
LOCAL void 	vd_dummy IPT0();

#ifdef HERC
#line 272
#error DIVERGENCE MVDM-HOST-DIV-522: HERC platform branch is not selected; see mirror register.
#line 275
#endif /* HERC */
#line 276

void (*video_func[]) () = {
				vd_set_mode,
				vd_set_cursor_mode,
				vd_set_cursor_position,
				vd_get_cursor_position,
		 		vd_get_light_pen,
				vd_set_active_page,
				vd_scroll_up,
				vd_scroll_down,
				vd_read_attrib_char,
				vd_write_char_attrib,
				vd_write_char,
				vd_set_colour_palette,
				vd_write_dot,
				vd_read_dot,
				vd_write_teletype,
				vd_get_mode,
				vd_dummy,
#ifdef HERC
#line 296
#error DIVERGENCE MVDM-HOST-DIV-522: HERC platform branch is not selected; see mirror register.
#line 298
#else /* !HERC */
#line 299
				vd_dummy,
				vd_dummy,
#endif /* HERC */
#line 302
				vd_write_string,
				vd_dummy,
				vd_dummy,
				vd_dummy,
				vd_dummy,
				vd_dummy,
				vd_dummy,
				vd_dummy,
#ifdef VGG
#line 311
				vga_disp_func,
#else /* !VGG */
#line 313
#error DIVERGENCE MVDM-HOST-DIV-540: non-VGG branch is not selected; see mirror register.
#endif /* VGG */
#line 315
				vd_dummy,
			   };
#endif // !NEC_98
#line 318

#ifndef NEC_98
#line 320
unsigned char   valid_modes[] =
        {
                ALL_MODES,              /* Mode 0. */
                ALL_MODES,              /* Mode 1. */
                ALL_MODES,              /* Mode 2. */
                ALL_MODES,              /* Mode 3. */
                ALL_MODES,              /* Mode 4. */
                ALL_MODES,              /* Mode 5. */
                ALL_MODES,              /* Mode 6. */
                ALL_MODES,              /* Mode 7. */
                NO_MODES,               /* Mode 8. */
                NO_MODES,               /* Mode 9. */
                NO_MODES,               /* Mode 10. */
                EGA_MODE | VGA_MODE,    /* Mode 11. */
                EGA_MODE | VGA_MODE,    /* Mode 12. */
                EGA_MODE | VGA_MODE,    /* Mode 13. */
                EGA_MODE | VGA_MODE,    /* Mode 14. */
                EGA_MODE | VGA_MODE,    /* Mode 15. */
                EGA_MODE | VGA_MODE,    /* Mode 16. */
                VGA_MODE,               /* Mode 17. */
                VGA_MODE,               /* Mode 18. */
                VGA_MODE,               /* Mode 19. */
        };

MODE_ENTRY vd_mode_table[] = {
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,    0x2C,40,16,8,/*Blink|BW*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,    0x28,40,16,8,/*Blink*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,    0x2D,80,16,8,/*Blink|BW|80x25*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,    0x29,80,16,8,/*Blink|80x25*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_GRAPHICS,0x2A,40,4,1,/*Blink|graph*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_GRAPHICS,0x2E,40,4,1,/*Blink|graph|BW*/
	0xB8000L, 0xBFFFFL, VD_CLEAR_GRAPHICS,0x1E,80,2,1,/*640x200|graph|BW*/
	0xB0000L, 0xB7FFFL, VD_CLEAR_TEXT,    0x29,80,0,8,/*MDA:Blink|80x25*/
	0L, 0L, 0,		VD_BAD_MODE,	0,0,0,	/* Never a valid mode */
	0L, 0L ,0,		VD_BAD_MODE,	0,0,0,	/* Never a valid mode */
	0,0,0,			VD_BAD_MODE,	0,0,0,	/* Never a valid mode */
	0xA0000L, 0xAFFFFL, 0,VD_BAD_MODE,0,0,0,/* Mode B - EGA colour font load */
	0xA0000L, 0xAFFFFL, 0,VD_BAD_MODE,0,0,0,/* Mode C - EGA monochrome font load */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,40,16,8,/* 320x200 EGA graphics */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,16,4,/* 640x200 EGA graphics */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,2,2,/* 640x350 EGA 'mono' */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,16,2,/* 640x350 EGA 16 colour */
#ifdef VGG
#line 363
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,2,1,/* 640x480 EGA++ 2 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,16,1,/* 640x480 EGA++ 16 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,40,256,1,/* 320x200 VGA 256 colour */
#endif
#line 367
	};

#ifdef V7VGA
#line 370
MODE_ENTRY vd_ext_text_table[] = {
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,80,16,8,/* 80x43 V7VGA 16 colour */
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,132,16,8,/* 132x25 V7VGA 16 colour */
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,132,16,8,/* 132x43 V7VGA 16 colour */
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,80,16,8,/* 80x60 V7VGA 16 colour */
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,100,16,8,/* 100x60 V7VGA 16 colour */
	0xB8000L, 0xBFFFFL, VD_CLEAR_TEXT,VD_BAD_MODE,132,16,8,/* 132x28 V7VGA 16 colour */
	};

MODE_ENTRY vd_ext_graph_table[] = {
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,94,16,2,/* 752x410 V7VGA 16 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,90,16,2,/* 720x540 V7VGA 16 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,100,16,2,/* 800x600 V7VGA 16 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,128,2,2,/* 1024x768 V7VGA 2 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,128,4,2,/* 1024x768 V7VGA 4 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,128,16,2,/* 1024x768 V7VGA 16 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,256,1,/* 640x400 V7VGA 256 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,80,256,1,/* 640x480 V7VGA 256 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,90,256,1,/* 720x540 V7VGA 256 colour */
	0xA0000L, 0xAFFFFL, VD_CLEAR_GRAPHICS,VD_BAD_MODE,100,256,1,/* 800x600 V7VGA 256 colour */
	};
#endif /* V7VGA */
#line 392

/*
 * Macros to calculate the offset from the start of the screen buffer
 * and start of page for a given row and column.
 */

#define vd_page_offset(col, row)       ( ((row) * vd_cols_on_screen + (col))<<1)

#define vd_regen_offset(page, col, row)					  \
		((page) * sas_w_at_no_check(VID_LEN) + vd_page_offset((col), (row)) )

#define vd_high_offset(col, row)   (((row) * ONELINEOFF)+(col))

#define vd_medium_offset(col, row)   (((row) * ONELINEOFF)+(col<<1))

#define vd_cursor_offset(page)						  \
		( vd_regen_offset(page, sas_hw_at_no_check(VID_CURPOS+2*page), sas_hw_at_no_check(VID_CURPOS+2*page+1)) )

#define GET_CURSOR_POS 3
#define SET_CURSOR_POS 2
#define WRITE_A_CHAR 10

/*
 * Static function declarations.
 */

LOCAL void sensible_text_scroll_down IPT6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr);
LOCAL void sensible_text_scroll_up IPT6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr);
LOCAL void sensible_graph_scroll_up IPT6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr);
LOCAL void sensible_graph_scroll_down IPT6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr);
LOCAL void kinky_scroll_up IPT7(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr, int, vd_cols_on_screen);
LOCAL void kinky_scroll_down IPT7(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr, int, vd_cols_on_screen);
#endif // !NEC_98
#line 425

/*
 * ============================================================================
 * External functions
 * ============================================================================
 */

#ifdef JAPAN
#line 433
GLOBAL int dbcs_first[0x100] = {
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 0x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 1x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 2x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 3x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 4x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 5x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 6x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 7x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // 8x
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // 9x
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // Ax
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // Bx
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // Cx
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // Dx
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
#if defined(NEC_98)
#line 463
#error Unsupported NEC PC-98 configuration
#line 465
#else  // !NEC_98
#line 466
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Ex
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
#endif // !NEC_98
#line 469
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Fx
		TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, FALSE, FALSE
	};
#elif defined(KOREA) // JAPAN
#line 473
GLOBAL int dbcs_first[0x100] = {
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 0x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 1x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 2x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 3x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 4x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 5x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 6x
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 7x
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 8x
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // 9x
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Ax
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Bx
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Cx
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Dx
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Ex
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  // Fx
                TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE
        };
#endif // KOREA
#line 508

#if defined(JAPAN) || defined(KOREA)
#line 510
GLOBAL int BOPFromDispFlag = FALSE;
GLOBAL int BOPFromNtDisp1Flag = FALSE;
GLOBAL sys_addr DBCSVectorAddr = 0;
GLOBAL word SaveDBCSVector[20];
GLOBAL int DBCSVectorLen = 0;

int is_us_mode()
{
#if defined(NEC_98)
#line 519
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 521
    if ( ( BOPFromDispFlag == TRUE ) &&  DBCSVectorAddr != 0 &&
	 ( sas_w_at_no_check(DBCSVectorAddr) != 0x00 ) )
        return FALSE;   // Not US mode
    else
        return TRUE;    // US mode
#endif // !NEC_98
#line 527
}

#ifndef NEC_98
#line 530
void SetDBCSVector( int CP )
{
    int i, j;
    sys_addr ptr;

#ifdef JAPAN_DBG
#line 536
    DbgPrint( " SetDBCSVector(%d) BOPFromDispFlag=%d, BOPFromNtDisp1Flag=%d\n", CP, BOPFromDispFlag, BOPFromNtDisp1Flag );
#endif
#line 538

    if ( !BOPFromDispFlag && !BOPFromNtDisp1Flag )
        return;

    ptr = DBCSVectorAddr;

    if ( CP == 437 ) {
        for ( i = 0; i < DBCSVectorLen; i++ ) {
	    sas_storew_no_check( ptr, 0x0000 );
            ptr += 2;
        }
        for ( i = 0; i < 0x100; i++ ) {
            dbcs_first[i] = FALSE;
        }
        // Set cursor mode
        if ( !SetConsoleCursorMode( sc.OutputHandle,
                                    TRUE,             // Bringing
                                    FALSE             //  No double byte cursor
                                         ) ) {
            DbgPrint( "NTVDM: SetConsoleCursorMode Error\n" );
        }
    }
    else { // CP == 932
        for ( i = 0; i < DBCSVectorLen; i++ ) {
            sas_storew_no_check( ptr, SaveDBCSVector[i] );
            ptr += 2;
        }
        for ( i = 0, j = 0; i < DBCSVectorLen; i++ ) {
            //DbgPrint( "...%02x.", LOBYTE(SaveDBCSVector[i]) );
            for ( ; j < LOBYTE(SaveDBCSVector[i]); j++ ) {
                dbcs_first[j] = FALSE;
            }
            //DbgPrint( "...%02x.", HIBYTE(SaveDBCSVector[i]) );
            for ( ; j <= HIBYTE(SaveDBCSVector[i]); j++ ) {
                dbcs_first[j] = TRUE;
            }
        }
        for ( ; j < 0x100; j++ ) {
            dbcs_first[j] = FALSE;
        }
        // Set cursor mode
        if ( !SetConsoleCursorMode( sc.OutputHandle,
                                    FALSE,            //  No bringing
                                    FALSE             //  No double byte cursor
                                         ) ) {
            DbgPrint( "NTVDM: SetConsoleCursorMode Error\n" );
        }
    }

}

void SetVram()
{

#ifdef i386
#line 593
    if ( !is_us_mode() ) {
        // set_up_screen_ptr() vga_mode.c
        set_screen_ptr( (byte *)DosvVramPtr );
        // low_set_mode() ega_vide.c
        video_pc_low_regen = DosvVramPtr;
        video_pc_high_regen = DosvVramPtr + DosvVramSize - 1;
        // vga_gc_misc() vga_prts.c
        gvi_pc_low_regen = DosvVramPtr;
        gvi_pc_high_regen = DosvVramPtr + DosvVramSize - 1;
        sas_connect_memory(gvi_pc_low_regen,gvi_pc_high_regen,(half_word)SAS_VIDEO);
        // recalc_screen_params() gvi.c
        set_screen_length( DosvVramSize );

#ifdef JAPAN_DBG
#line 607
        DbgPrint( "NTVDM: SetVram() video_pc_low_regen %x, high %x, gvi_pc_low_regen %x, high %x len%d\n", video_pc_low_regen, video_pc_high_regen, gvi_pc_low_regen, gvi_pc_high_regen, get_screen_length() );
#endif
#line 609
    }
    else {
        // set_up_screen_ptr() vga_mode.c
        set_screen_ptr( (IU8*)0xB8000 );
        // low_set_mode() ega_vide.c
        video_pc_low_regen = vd_mode_table[sas_hw_at_no_check(vd_video_mode)].start_addr;
        video_pc_high_regen = vd_mode_table[sas_hw_at_no_check(vd_video_mode)].end_addr;
        // vga_gc_misc() vga_prts.c
        gvi_pc_low_regen = 0xB8000;
        gvi_pc_high_regen = 0xBFFFF;
        sas_connect_memory(gvi_pc_low_regen,gvi_pc_high_regen,(half_word)SAS_VIDEO);
        // recalc_screen_params() gvi.c
        set_screen_length(get_offset_per_line()*get_screen_height()/get_char_height());

#ifdef JAPAN_DBG
#line 624
        DbgPrint( "NTVDM: SetVram() video_pc_low_regen %x, high %x, gvi_pc_low_regen %x, high %x len%d\n", video_pc_low_regen, video_pc_high_regen, gvi_pc_low_regen, gvi_pc_high_regen, get_screen_length() );
#endif
#line 626
    }
#endif // i386
#line 628
}

// IME will change status line with set mode
void SetModeForIME()
{
    word CS_save, IP_save, AX_save;
    half_word video_mode;
    extern UINT ConsoleOutputCP;
#ifdef X86GFX
#line 637
    extern word int10_seg, int10_caller;
#endif // X86GFX
#line 639

    if (!BOPFromDispFlag)
        return;

    video_mode = sas_hw_at_no_check(vd_video_mode);

    if (video_mode == 0x03 && (UINT)PrevCP != ConsoleOutputCP) {

        /* Call int10 handler */
        CS_save = getCS();          /* Save current CS,IP settings */
        IP_save = getIP();
        AX_save = getAX();
        sas_store_no_check( SetModeFlagPtr, 1 );
        setAX((word)video_mode);    /* IME expects setmode */
#ifdef X86GFX
#line 654
        exec_sw_interrupt( int10_seg, int10_caller );
#else // !X86GFX
#line 656
        setCS(VIDEO_IO_SEGMENT);
        setIP(VIDEO_IO_RE_ENTRY);
        host_simulate();
#endif // !X86GFX
#line 660
        sas_store_no_check( SetModeFlagPtr, 0 );
        setCS(CS_save);             /* Restore CS,IP */
        setIP(IP_save);
        setAX(AX_save);
    }
    PrevCP = ConsoleOutputCP;
}
#endif // !NEC_98
#line 668
#endif // JAPAN || KOREA
#line 669

#ifndef NEC_98
#line 671
GLOBAL VOID
simple_bios_byte_wrt IFN2(ULONG, ch, ULONG, ch_addr)
{
	*(IU8 *)(getVideoscreen_ptr() + ch_addr) = (UTINY)ch;
#if !defined(EGG) && !defined(C_VID) && !defined(A_VID)
#line 676
#error Unsupported excluded original configuration
#endif	/* not EGG or C_VID or A_VID */
#line 678
}

GLOBAL VOID
simple_bios_word_wrt IFN2(ULONG, ch_attr, ULONG, ch_addr)
{
	*(IU8 *)(getVideoscreen_ptr() + ch_addr) = (UTINY)ch_attr;
	*(IU8 *)(getVideoscreen_ptr() + ch_addr + 1) = (UTINY)(ch_attr >> 8);
#if !defined(EGG) && !defined(C_VID) && !defined(A_VID)
#line 686
#error Unsupported excluded original configuration
#endif	/* not EGG or C_VID or A_VID */
#line 688
}

/*
 * It is possible for the Hercules to attempt text in graphics mode,
 * relying on our int 10 handler to call itself recursively so a user
 * handler can intercept the write character function.
 */

GLOBAL void vd_set_mode IFN0()
{
    half_word card_mode = 0;
    half_word pag;
    EQUIPMENT_WORD equip_flag;
    word page_size,vd_addr_6845,vd_cols_on_screen;
    UCHAR current_video_mode = getAL();

    if (is_bad_vid_mode(current_video_mode))
    {
	always_trace1("Bad video mode - %d.\n", current_video_mode);
	return;
    }

    /*
     * Set the Video mode to the value in AL
     */
    equip_flag.all = sas_w_at_no_check(EQUIP_FLAG);
    if ((half_word)current_video_mode > VD_MAX_MODE ||
	vd_mode_table[current_video_mode].mode_control_val == VD_BAD_MODE) {
#ifndef PROD
#line 717
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 719
        return;
    }
    if (equip_flag.bits.video_mode == VIDEO_MODE_80X25_BW) {
        vd_addr_6845 = 0x3B4;    /* Index register for B/W M6845 chip */
        sas_store_no_check (vd_video_mode , 7);       /* Force B/W mode */
        card_mode++;
    }
    else {
        vd_addr_6845 = 0x3D4;
	if (current_video_mode == 7) {
	    /*
	     * Someone has tried to set the monochrome mode without
	     * the monochrome card installed - this can be generated by
	     * a 'mode 80' from medium res graphics mode.
	     * Take 'I am very confused' type actions by clearing the
	     * screen and then disabling video - this is v. similar to
	     * the action taken by the PC but with less snow!
	     */

	    /*
	     * Clear the video area
	     */
#ifdef REAL_VGA
#line 742
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 745
#else
#line 746
	    sas_fillsw(video_pc_low_regen,
				vd_mode_table[sas_hw_at_no_check(vd_video_mode)].clear_char,
				 (video_pc_high_regen - video_pc_low_regen)/ 2 + 1);
#endif
#line 750

	    /*
	     * Force a redraw
	     */
	    outb(M6845_MODE_REG, card_mode);
	    outb(M6845_MODE_REG,
		(IU8)(vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val | VIDEO_ENABLE));
	    /*
	     * Turn off the video until another mode command is given
	     */
	    outb(M6845_MODE_REG,
		(IU8)(vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val & ~VIDEO_ENABLE));
	    return;
	}
        sas_store_no_check (vd_video_mode , current_video_mode);
    }

#ifdef EGG
#line 768
    sas_store_no_check(vd_rows_on_screen, 24);
#endif
#line 770
    sas_store_no_check (vd_current_page , 0);

    /*
     * Initialise the Control Register
     */

    outb(M6845_MODE_REG, card_mode);

    /*
     * Set up M6845 registers for this mode
     */

    M6845_reg_init(sas_hw_at_no_check(vd_video_mode), vd_addr_6845);

    /*
     * ... now overwrite the dynamic registers, eg cursor position
     */

    outb(M6845_INDEX_REG, R14_CURS_ADDRH);
    outb(M6845_DATA_REG, 0);
    outb(M6845_INDEX_REG, R15_CURS_ADDRL);
    outb(M6845_DATA_REG, 0);
    /*
     * Clear the video area
     */
#ifdef REAL_VGA
#line 796
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 798
#else
#line 799
    sas_fillsw(video_pc_low_regen, vd_mode_table[sas_hw_at_no_check(vd_video_mode)].clear_char,
				 (video_pc_high_regen - video_pc_low_regen)/ 2 + 1);
#endif
#line 802

    /*
     * re-enable video for this mode
     */
    outb(M6845_MODE_REG, vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val);

    if (sas_hw_at_no_check(vd_video_mode) != 7) {
        if (sas_hw_at_no_check(vd_video_mode) != 6)
            sas_store_no_check (vd_crt_palette , 0x30);
        else
            sas_store_no_check (vd_crt_palette , 0x3F);
        outb(CGA_COLOUR_REG, sas_hw_at_no_check(vd_crt_palette));
    }

    vd_cols_on_screen = vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_screen_cols;


    /*
     * Update BIOS data variables
     */

    sas_storew_no_check((sys_addr)VID_COLS, vd_cols_on_screen);
    sas_storew_no_check((sys_addr)VID_ADDR, 0);
    sas_storew_no_check((sys_addr)VID_INDEX, vd_addr_6845);
    sas_store_no_check (vd_crt_mode , vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val);
    for(pag=0; pag<8; pag++)
	sas_storew_no_check(VID_CURPOS + 2*pag, 0);
    if(sas_hw_at_no_check(vd_video_mode) == 7)
    	page_size = 4096;
    else
	page_size = sas_w_at_no_check(VID_LENS+(sas_hw_at_no_check(vd_video_mode) & 0xE));	/* sneakily divide mode by 2 and use as word address! */
    sas_storew_no_check(VID_LEN,page_size);
}


GLOBAL void vd_set_cursor_mode IFN0()
{
    /*
     * Set cursor mode
     * Parameters:
     *  CX - cursor value (CH - start scanline, CL - stop scanline)
     */
    io_addr vd_addr_6845;

    vd_addr_6845 = sas_w_at_no_check(VID_INDEX);
    outb(M6845_INDEX_REG, R10_CURS_START);
    outb(M6845_DATA_REG, getCH());
    outb(M6845_INDEX_REG, R11_CURS_END);
    outb(M6845_DATA_REG, getCL());

    /*
     * Update BIOS data variables
     */
    sure_sub_note_trace2(CURSOR_VERBOSE,"setting bios cursor vbl to start=%d, end=%d",getCH(),getCL());

    sas_storew_no_check((sys_addr)VID_CURMOD, getCX());
    setAH(0);
}


GLOBAL void vd_set_cursor_position IFN0()
{
    /*
     * Set cursor variables to new values and update the display
     * adaptor registers.
     * The parameters are held in the following registers:
     *
     * DX - row/column of new cursor position
     * BH - page number
     *
     */

    word cur_pos,vd_addr_6845,vd_cols_on_screen;

    /* Load internal variables with the values stored in BIOS
     * data area.
     */

    vd_addr_6845 = sas_w_at_no_check(VID_INDEX);
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);

#if defined(JAPAN) || defined(KOREA)
#line 884
    // for assist calc installer
    // assist calc installer doesn't set BH register to 0
    // In MS-DOS/V it was only support page number == 0
    // I think $DISP.SYS check to see if BH == 0 or to set BH=0
    // in all Int10 function.
    // Therefore you must check BH register value.

    if ( !is_us_mode() )
        sas_storew_no_check(VID_CURPOS, getDX()); // same as BH == 0
    else
        sas_storew_no_check(VID_CURPOS+(getBH()*2), getDX());
#else // !JAPAN && !KOREA
#line 896
    sas_storew_no_check(VID_CURPOS+(getBH()*2), getDX());
#endif // !JAPAN && !KOREA
#line 898

    if (sas_hw_at_no_check(vd_current_page) == getBH()) {           /* display if this page */

        /*
         * Calculate position in regen buffer, ignoring attribute bytes
         */

        cur_pos = vd_regen_offset(getBH(), getDL(), getDH());
        cur_pos /= 2;		/* not interested in attributes */

        /*
         * tell the 6845 all about the change
         */
        outb(M6845_INDEX_REG, R14_CURS_ADDRH);
        outb(M6845_DATA_REG,  (IU8)(cur_pos >> 8));
        outb(M6845_INDEX_REG, R15_CURS_ADDRL);
        outb(M6845_DATA_REG,  (IU8)(cur_pos & 0xff));
    }
}


GLOBAL void vd_get_cursor_position IFN0()
{
    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    word vd_cursor_mode;
    half_word vd_cursor_col, vd_cursor_row;

    vd_cursor_mode = sas_w_at_no_check(VID_CURMOD);
    vd_cursor_col = sas_hw_at_no_check(VID_CURPOS + getBH()*2);
    vd_cursor_row = sas_hw_at_no_check(VID_CURPOS + getBH()*2 + 1);

    /*
     * Return the cursor coordinates and mode
     */
    sure_sub_note_trace4(CURSOR_VERBOSE,"returning bios cursor info; start=%d, end=%d, row=%#x, col=%#x",(vd_cursor_mode>>8) & 0xff,vd_cursor_mode & 0xff, vd_cursor_row, vd_cursor_col);

    setDH(vd_cursor_row);
    setDL(vd_cursor_col);
    setCX(vd_cursor_mode);
    setAH(0);
}


GLOBAL void vd_get_light_pen IFN0()
{
    /*
     * Read the current position  of the light pen. Tests light pen switch
     * & trigger & returns AH == 0 if not triggered. (This should always be
     * true in this version) If set (AH == 1) then returns:
     *  DH, DL - row, column of char lp posn.
     *  CH  -  raster line (0-199)
     *  BX  -  pixel column (0-319,639)
     */

    half_word status;

    if (sas_hw_at_no_check(vd_video_mode) == 7) {
        setAX(0x00F0);    /* Returned by real MDA */
        return;           /* MDA doesn't support a light pen */
    }

    inb(CGA_STATUS_REG, &status);
    if ((status & 0x6) == 0) {	/* Switch & trigger */
	setAH(0);		/* fail */
	return;
    }
    else {		        /* not supported */
#ifndef PROD
#line 968
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 970
    }
}


GLOBAL void vd_set_active_page IFN0()
{
    /*
     * Set active display page from the 8 (4) available from the adaptor.
     * Parameters:
     * 	AL - New active page #
     */

    word cur_pos,vd_addr_6845,vd_crt_start,vd_cols_on_screen;
    half_word vd_cursor_col, vd_cursor_row;
#ifdef V7VGA
#line 985
    UTINY bank;
#endif
#line 987

    /* Load internal variables with the values stored in BIOS
     * data area.
     */

    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
    vd_addr_6845 = sas_w_at_no_check(VID_INDEX);

    /*	redundancy check against BIOS page number variable removed as it
	was redundant (more checks in the outbs) and caused a bug in
	"image.exe", a 3D drawing package for EGA which itself sets the
	BIOS variable before using this routine to set the active page */

    if (sas_hw_at_no_check(vd_video_mode) >3 && sas_hw_at_no_check(vd_video_mode)<8)return;	/* Only one page for MDA * CGA graphics */
    sas_store_no_check (vd_current_page , getAL());

#ifdef V7VGA
#line 1004
	/*
	 *	This function is used by the Video 7 to set the bank for the
	 *	hi-res V7 graphics modes.
	 *	For this case, the setting of vd_crt_start etc. seems to be
	 *	inappropriate.
	 */

	if (sas_hw_at_no_check(vd_video_mode) >= 0x14)
	{
		bank = sas_hw_at_no_check(vd_current_page);
		set_banking( bank, bank );

		return;
	}
#endif /* V7VGA */
#line 1019

    /* start of screen */
    vd_crt_start = sas_w_at_no_check(VID_LEN) * sas_hw_at_no_check(vd_current_page);
    /*
     * Update BIOS data variables
     */
    sas_storew_no_check((sys_addr)VID_ADDR, vd_crt_start);

    if(alpha_num_mode())vd_crt_start /= 2; /* WORD address for text modes */

    /*
     * set the start address into the colour adaptor
     */

    outb(CGA_INDEX_REG, CGA_R12_START_ADDRH);
    outb(CGA_DATA_REG, (IU8)(vd_crt_start >> 8));
    outb(CGA_INDEX_REG, CGA_R13_START_ADDRL);
    outb(CGA_DATA_REG, (IU8)(vd_crt_start  & 0xff));

    /*
     * Swap to cursor for this page
     */

    vd_cursor_col = sas_hw_at_no_check(VID_CURPOS + sas_hw_at_no_check(vd_current_page)*2);
    vd_cursor_row = sas_hw_at_no_check(VID_CURPOS + sas_hw_at_no_check(vd_current_page)*2 + 1);

   /*
    * Calculate position in regen buffer, ignoring attribute bytes
    */

    cur_pos = (sas_w_at_no_check(VID_ADDR)+vd_page_offset( vd_cursor_col, vd_cursor_row)) / 2;

    outb(M6845_INDEX_REG, R14_CURS_ADDRH);
    outb(M6845_DATA_REG,  (IU8)(cur_pos >> 8));
    outb(M6845_INDEX_REG, R15_CURS_ADDRL);
    outb(M6845_DATA_REG,  (IU8)(cur_pos & 0xff));

}

GLOBAL void vd_scroll_up IFN0()
{
    /*
     * Scroll up a block of text.  The parameters are held in the following
     * registers:
     *
     * AL - Number of rows to scroll. NB. if AL == 0 then the whole region
     *      is cleared.
     * CX - Row/col of upper left corner
     * DX - row/col of lower right corner
     * BH - attribute to be used on blanked line(s)
     *
     * IMPORTANT MESSAGE TO ALL VIDEO HACKERS:
     * vd_scroll_up() and vd_scroll_down() are functionally identical
     * except for the sense of the scroll - if you find and fix a bug
     * in one, then please do the same for the other
     */
    word vd_cols_on_screen;
    int t_row,b_row,l_col,r_col,lines,attr;
    int rowsdiff,colsdiff;
#ifdef JAPAN
#line 1079
    int text_flag = 0;
#endif // JAPAN
#line 1081

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);

    t_row = getCH();
    b_row = getDH();
    l_col = getCL();
    r_col = getDL();
    lines = getAL();
    attr = getBH();

#ifdef	JAPAN
#line 1095
    // #4183: status line of oakv(DOS/V FEP) doesn't disappear -yasuho
    // $disp.sys is also be able to scroll in IME status lines.
    if (!is_us_mode()) {
	if(b_row > VD_ROWS_ON_SCREEN + IMEStatusLines)
	    b_row = VD_ROWS_ON_SCREEN + IMEStatusLines;
	if(t_row > VD_ROWS_ON_SCREEN + IMEStatusLines)
	    t_row = VD_ROWS_ON_SCREEN + IMEStatusLines;
    } else {
	if(b_row > VD_ROWS_ON_SCREEN)
	    b_row = VD_ROWS_ON_SCREEN;
	if(t_row > VD_ROWS_ON_SCREEN)
	    t_row = VD_ROWS_ON_SCREEN;
    }
#else // !JAPAN
#line 1109
    if(b_row > VD_ROWS_ON_SCREEN)
		b_row = VD_ROWS_ON_SCREEN; /* trim to screen size */

    if(t_row > VD_ROWS_ON_SCREEN)
		t_row = VD_ROWS_ON_SCREEN; /* trim to screen size */
#endif // !JAPAN
#line 1115

    if (r_col < l_col)		/* some dipstick has got their left & right mixed up */
    {
	colsdiff = l_col;	/* use colsdiff as temp */
	l_col = r_col;
	r_col = colsdiff;
    }

#ifdef JAPAN
#line 1124
    // for HANAKO v2 installer, it sets DL to 0x80 ( >=vd_cols_on_screen)
    // And when app set DL=0x4f, text_scroll should be run
    if ( r_col == 0x80 || r_col == 0x4f)
        text_flag = 1;
#endif // JAPAN
#line 1129
    if ( r_col >= vd_cols_on_screen )
    	r_col = vd_cols_on_screen-1;

    colsdiff = r_col-l_col+1;
    rowsdiff = b_row-t_row+1;

    if (lines == 0)	/* clear region */
    {
	lines = rowsdiff;
    }
#ifdef JAPAN
#line 1140
    // for HANAKO v2 installer, it sets DL to 0x80 ( >=vd_cols_on_screen )
    if(r_col == vd_cols_on_screen-1 && !text_flag)
#else // !JAPAN
#line 1143
    if(r_col == vd_cols_on_screen-1)
#endif // !JAPAN
#line 1145
    {
#ifdef EGG
#line 1147
	if(ega_mode())
    		ega_sensible_graph_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  ifdef VGG
#line 1150
	else if(vga_256_mode())
    		vga_sensible_graph_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  endif
#line 1153
	else
#endif
#line 1155
    	kinky_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr,vd_cols_on_screen);
    }
    else
    {
	if(alpha_num_mode())
    		sensible_text_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#ifdef EGG
#line 1162
#  ifdef VGG
#line 1163
	else if(vga_256_mode())
    		vga_sensible_graph_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  endif
#line 1166
	else if(ega_mode())
    		ega_sensible_graph_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#endif
#line 1169
	else
    		sensible_graph_scroll_up(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#ifdef EGA_DUMP
#line 1172
#error DIVERGENCE MVDM-HOST-DIV-527: EGA_DUMP branch is not selected; see mirror register.
#line 1174
#endif
#line 1175
    /*
     * re-enable video for this mode, if on a CGA adaptor (fixes ROUND42 bug).
     */
	if(video_adapter == CGA)
    	outb(CGA_CONTROL_REG, vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val);
    }
#ifdef	JAPAN
#line 1182
    // mskkbug#2757 works2.5 garbage remains after exiting install -yasuho
    // We need flush screen when scroll the screen.
    Int10FlagCnt++;
#endif // JAPAN
#line 1186
}

/*
 * Functions to scroll sensible areas of the screen. This routine will try to use
 * host scrolling and clearing.
 */
LOCAL void sensible_text_scroll_up IFN6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr)
{
    register sys_addr	source, dest;
#if !defined(i386) && defined(JAPAN)
#line 1196
    register sys_addr sourceV, destV;
    int                 linesdiff;
    word attrW = (word)((attr << 8)|' ');
#endif // !i386 && JAPAN
#line 1200
    register int	col_incr,i;
    boolean 		screen_updated = FALSE;
    int			vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
#ifdef JAPAN
#line 1204
    // for mskkbug #875
    byte *p = &Int10Flag[t_row * vd_cols_on_screen + l_col];
#endif // JAPAN
#line 1207


	/* Set origin of data movement for calculating screen refresh */

#if defined(JAPAN) && defined(i386)
#line 1212
    // mode73h support
    if ( !is_us_mode() && ( sas_hw_at_no_check (DosvModePtr) == 0x73 ) ) {
       source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row)*2 + video_pc_low_regen;

       col_incr = sas_w_at_no_check(VID_COLS) * 4;	/* offset to next line */
    }
    else {
       source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + video_pc_low_regen;
       col_incr = sas_w_at_no_check(VID_COLS) * 2;	/* offset to next line */
    }
#else // !JAPAN || !i386
    source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + vd_text_mode().start_addr;
#line 1224
#if !defined(i386) && defined(JAPAN)
#line 1225
    sourceV = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + DosvVramPtr;
#endif // !i386 && JAPAN
#line 1227
    col_incr = sas_w_at_no_check(VID_COLS) * 2;	/* offset to next line */
#endif // !JAPAN || !i386
#line 1229

	/* Try to scroll the adaptor memory & host screen. */

	if( source >= get_screen_base() )
	{
#if defined(JAPAN) && defined(i386)
#line 1235
	    // mode73h support
	    if ( !is_us_mode() && ( sas_hw_at_no_check (DosvModePtr) == 0x73 ) ) {
                screen_updated = (*update_alg.scroll_up)(source,4*colsdiff,rowsdiff,attr,lines,0);
            }
            else {
                screen_updated = (*update_alg.scroll_up)(source,2*colsdiff,rowsdiff,attr,lines,0);
            }
#else // !JAPAN || !i386
#line 1243
	    screen_updated = (*update_alg.scroll_up)(source,2*colsdiff,rowsdiff,attr,lines,0);
#endif // !JAPAN || i386
#line 1245
	}

    dest = source;
#if !defined(i386) && defined(JAPAN)
#line 1249
    destV = sourceV;
#endif // !i386 && JAPAN
#line 1251
/*
 * We dont need to move data which would be scrolled off the
 * window. So point source at the first line which needs to
 * be retained.
 *
 * NB if we are just doing a clear, the scroll for loop will
 * terminate immediately.
 */
    source += lines*col_incr;	
#if !defined(i386) && defined(JAPAN)
#line 1261
    sourceV += lines*col_incr;
    linesdiff = vd_cols_on_screen * lines;
#endif // !i386 && JAPAN
#line 1264
    for(i = 0; i < rowsdiff-lines; i++)
    {
#ifdef REAL_VGA
#line 1267
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1269
#ifdef JAPAN
#line 1270
		// for RAID #875
		if( !screen_updated ) {
		    // mode73h support
#ifdef i386
#line 1274
		    if ( !is_us_mode() && (sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
	  	        sas_move_bytes_forward (source, dest, colsdiff*4);
                    }
                    else {
	  	        sas_move_bytes_forward (source, dest, colsdiff*2);
                    }

#else // !i386
#line 1282
	    sas_move_bytes_forward (source, dest, colsdiff*2);
	    if ( !is_us_mode() )
	      sas_move_bytes_forward (sourceV, destV, colsdiff*2);
#endif // !i386
#line 1286
                    {
                        register int i;

                        for ( i = 0; i < colsdiff; i++ ) {
#ifdef i386
#line 1291
                            p[i] = ( p[i+vd_cols_on_screen] | INT10_CHANGED );
#else // !i386
#line 1293
//I think this is correct!!
                            p[i] = ( p[i+linesdiff] | INT10_CHANGED );
#endif // !i386
#line 1296
                        }
                    }
                }
#else // !JAPAN
#line 1300
		if( !screen_updated )
			sas_move_bytes_forward (source, dest, colsdiff*2);
#endif // !JAPAN
#line 1303
#endif
#line 1304

		/* next line */
		source += col_incr;
		dest += col_incr;
#ifdef JAPAN
#line 1309
#if !defined(i386)
#line 1310
		sourceV += col_incr;
		destV += col_incr;
#endif // !i386
#line 1313
                p += vd_cols_on_screen;
#endif // JAPAN
#line 1315
    }

/* moved all the data we were going to move - blank the cleared region */

#if !defined(i386) && defined(JAPAN)
#line 1320
    if( sas_hw_at_no_check(DosvModePtr) == 0x73 )
      attrW = 0;
#endif // !i386 && JAPAN
#line 1323

    while(lines--)
    {
#ifdef REAL_VGA
#line 1327
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1329
#ifdef JAPAN
#line 1330
		// for mskkbug #875
		if( !screen_updated ) {
		// mode73h support
#ifdef i386
#line 1334
		    if ( !is_us_mode() && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
                        unsigned long *destptr = (unsigned long *)dest;
                        int i;

                        for ( i = 0; i < colsdiff; i++ ) {
                            *destptr++ = (attr << 8)|' ';
                        }
                    }
                    else {
		        sas_fillsw(dest, (attr << 8)|' ', colsdiff);
                    }
#else // !i386
#line 1346
                    sas_fillsw(dest, (attr << 8)|' ', colsdiff);
		    //add Apr. 18 1994 DosvVram holds extended attributes
		    if ( !is_us_mode() )
		      sas_fillsw(destV, attrW, colsdiff);
#endif // !i386
#line 1351
#ifdef i386
#line 1352
// "p" (Int10Flag) is on 32 bit address space, so we must no use "sas" function
// to access 32 bit address.
                    sas_fills( (sys_addr)p, ( INT10_SBCS | INT10_CHANGED ), colsdiff );
#else // !i386
#line 1356
                    {
		      register int i = colsdiff;
		      register byte *pp = p;
		      while( i-- ){
			*pp++ = ( INT10_SBCS | INT10_CHANGED );
		      }
		    }
#endif // !i386
#line 1364
                }
#else // !JAPAN
#line 1366
		if( !screen_updated )
			sas_fillsw(dest, (IU16)((attr << 8)|' '), colsdiff);
#endif // !JAPAN
#line 1369
#endif
#line 1370
		dest += col_incr;
#ifdef JAPAN
#line 1372
#if !defined(i386)
#line 1373
		destV += col_incr;
#endif // !i386
#line 1375
                p += vd_cols_on_screen;
#endif // JAPAN
#line 1377
    }
}

LOCAL void sensible_graph_scroll_up IFN6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr)
{
    sys_addr	source, dest;
    int		i,colour;
    boolean 	screen_updated;

	rowsdiff *= 4;		/* 8 scans per char - 4 per bank */
	lines *= 4;		/* scan lines */

    /* Set origin of data movement for calculating screen refresh */

	if( sas_hw_at_no_check(vd_video_mode) != 6)
	{
		colour = attr & 0x3;
		colsdiff *= 2;		/* 4 pixels/byte */

		source = vd_medium_offset(l_col, t_row) + video_pc_low_regen;
	}
	else
	{
		colour = attr & 0x1;
		source = vd_high_offset(l_col, t_row) + video_pc_low_regen;
	}

	/* Try to scroll the adaptor memory & host screen */

    screen_updated = (*update_alg.scroll_up)(source,colsdiff,rowsdiff,attr,lines,colour);

    if( screen_updated && (video_adapter != CGA ))
		return;

    dest = source;

	/*
	 * We dont need to move data which would be scrolled off the
	 * window. So point source at the first line which needs to
	 * be retained.
	 *
	 * NB if we are just doing a clear, the scroll for loop will
	 * terminate immediately.
	 */

	source += lines*SCAN_LINE_LENGTH;	

	for(i = 0; i < rowsdiff-lines; i++)
	{
#ifdef REAL_VGA
#line 1427
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1429
		sas_move_bytes_forward (source,dest, colsdiff);
#endif
#line 1431
		/*
		 * graphics mode has to cope with odd bank as well
		 */
#ifdef REAL_VGA
#line 1435
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1437
		sas_move_bytes_forward (source+ODD_OFF,dest+ODD_OFF, colsdiff);
#endif
#line 1439
		source += SCAN_LINE_LENGTH;
		dest += SCAN_LINE_LENGTH;
	}

    /* Moved all the data we were going to move - blank the cleared region */

	while( lines-- )
	{
#ifdef REAL_VGA
#line 1448
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 1450
#else
#line 1451
		sas_fills(dest, (IU8)attr, colsdiff);
		sas_fills(dest+ODD_OFF, (IU8)attr, colsdiff);
#endif
#line 1454
		dest += SCAN_LINE_LENGTH;
	}
}

/*
 * Handle silly case where the wally programmer is scrolling a daft window.
 * We must be careful not to scribble off the end of the video page, to avoid
 * nasty things like dead MacIIs.
 */
LOCAL void kinky_scroll_up IFN7(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr, int, vd_cols_on_screen)
{
    register sys_addr	source, dest;
    register int	col_incr;
    register int	i;
    half_word is_alpha;

    switch (sas_hw_at_no_check(vd_video_mode)) {

    case 0: case 1: case 2:		/* text */
    case 3: case 7:
	is_alpha = TRUE;
	/* set origin of data movement for calculating screen refresh */
	source = sas_w_at_no_check(VID_ADDR)+vd_page_offset(l_col, t_row) + vd_text_mode().start_addr;
	col_incr = vd_cols_on_screen * 2;	/* offset to next line */
	break;

    case 6: case 4: case 5:
	is_alpha = FALSE;
	rowsdiff *= 4;		/* 8 scans per char - 4 per bank */
	lines *= 4;		/* scan lines */
#ifdef NTVDM
#line 1485
	/* mode 4 and 5 have 40 rows with 4 pixels/byte while
	 * mode 6 has 80 rows with 8 pixels/byte.
	 * They have the same line increment value
	 */
	col_incr = SCAN_LINE_LENGTH;
#endif
#line 1491
	if (sas_hw_at_no_check(vd_video_mode) != 6) {
	    colsdiff *= 2;		/* 4 pixels/byte */
	    /* set origin of data movement for calculating screen refresh */
	    source = vd_medium_offset(l_col, t_row) + video_pc_low_regen;
	}
	else
	    source = vd_high_offset(l_col, t_row) + video_pc_low_regen;

	break;

    default:
#ifndef PROD
#line 1503
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1505
	;
    }

    dest = source;
/*
 * We dont need to move data which would be scrolled off the
 * window. So point source at the first line which needs to
 * be retained. AL lines ( = lines ) are to be scrolled so
 * add lines*<width> to source pointer - apg
 *
 * NB if we are just doing a clear, the scroll for loop will
 * terminate immediately.
 */
    source += lines*col_incr;	
    if (is_alpha) {
	    for(i = 0; i < rowsdiff-lines; i++) {
#ifdef REAL_VGA
#line 1522
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1524
		sas_move_bytes_forward (source,dest, colsdiff*2);
#endif
#line 1526
	        /* next line */
	        source += col_incr;
	        dest += col_incr;
	    }
     }
     else {
	    for(i = 0; i < rowsdiff-lines; i++) {
#ifdef REAL_VGA
#line 1534
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1536
		sas_move_bytes_forward (source,dest, colsdiff);
#endif
#line 1538
	        /*
	         * graphics mode has to cope with odd bank as well
	         */
#ifdef REAL_VGA
#line 1542
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1544
		sas_move_bytes_forward (source+ODD_OFF,dest+ODD_OFF, colsdiff);
#endif
#line 1546
	        source += SCAN_LINE_LENGTH;
	        dest += SCAN_LINE_LENGTH;
	    }
     }
    /* moved all the data we were going to move - blank the cleared region */
    if (is_alpha) {

	while(lines--) {
	    if((dest + 2*colsdiff) > vd_text_mode().end_addr+1)
	    {
		colsdiff = (int)((vd_text_mode().end_addr+1-dest)/2);
		lines = 0; /* force termination */
	    }
#ifdef REAL_VGA
#line 1560
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1562
	    sas_fillsw(dest, (IU16)((attr << 8)|' '), colsdiff);
#endif
#line 1564
	    dest += col_incr;
        }
    }
    else {

	while( lines-- ) {
#ifdef REAL_VGA
#line 1571
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 1573
#else
#line 1574
	    sas_fills(dest, (IU8)attr, colsdiff);
	    sas_fills(dest+ODD_OFF, (IU8)attr, colsdiff);
#endif
#line 1577
	    dest += SCAN_LINE_LENGTH;
	}
    }

}


GLOBAL void vd_scroll_down IFN0()
{
    /*
     * Scroll down a block of text.  The parameters are held in the following
     * registers:
     *
     * AL - Number of rows to scroll. NB. if AL == 0 then the whole region
     *      is cleared.
     * CX - Row/col of upper left corner
     * DX - row/col of lower right corner
     * BH - attribute to be used on blanked line(s)
     *
     * IMPORTANT MESSAGE TO ALL VIDEO HACKERS:
     * vd_scroll_up() and vd_scroll_down() are functionally identical
     * except for the sense of the scroll - if you find and fix a bug
     * in one, then please do the same for the other
     */
    word vd_cols_on_screen;
    int t_row,b_row,l_col,r_col,lines,attr;
    int rowsdiff,colsdiff;
#ifdef JAPAN
#line 1605
    int text_flag = 0;
#endif // JAPAN
#line 1607

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);

    t_row = getCH();
    b_row = getDH();
    l_col = getCL();
    r_col = getDL();
    lines = getAL();
    attr = getBH();

#ifdef	JAPAN
#line 1621
    // #4183: status line of oakv(DOS/V FEP) doesn't disappear 12/11/93 yasuho
    // $disp.sys is also be able to scroll in IME status lines.
    if (!is_us_mode()) {
	if(b_row > VD_ROWS_ON_SCREEN + IMEStatusLines)
	    b_row = VD_ROWS_ON_SCREEN + IMEStatusLines;
	if(t_row > VD_ROWS_ON_SCREEN + IMEStatusLines)
	    t_row = VD_ROWS_ON_SCREEN + IMEStatusLines;
    } else {
	if(b_row > VD_ROWS_ON_SCREEN)
	    b_row = VD_ROWS_ON_SCREEN;
	if(t_row > VD_ROWS_ON_SCREEN)
	    t_row = VD_ROWS_ON_SCREEN;
    }
#else // !JAPAN
#line 1635
    if(b_row > VD_ROWS_ON_SCREEN)
		b_row = VD_ROWS_ON_SCREEN; /* trim to screen size */

    if(t_row > VD_ROWS_ON_SCREEN)
		t_row = VD_ROWS_ON_SCREEN; /* trim to screen size */
#endif // !JAPAN
#line 1641

    if (r_col < l_col)		/* some dipstick has got their left & right mixed up */
    {
	colsdiff = l_col;	/* use colsdiff as temp */
	l_col = r_col;
	r_col = colsdiff;
    }

#ifdef JAPAN
#line 1650
    // for HANAKO v2 installer, it sets DL to 0x80 ( >=vd_cols_on_screen)
    // And when app set DL=0x4f, text_scroll should be run
    if ( r_col == 0x80 || r_col == 0x4f)
        text_flag = 1;
#endif // JAPAN
#line 1655
    if ( r_col >= vd_cols_on_screen )
    	r_col = vd_cols_on_screen-1;

    colsdiff = r_col-l_col+1;
    rowsdiff = b_row-t_row+1;

    if (lines == 0)	/* clear region */
    {
	lines = rowsdiff;
    }
#ifdef JAPAN
#line 1666
    // for HANAKO v2 installer
    if(r_col == vd_cols_on_screen-1 && !text_flag)
#else // !JAPAN
#line 1669
    if(r_col == vd_cols_on_screen-1)
#endif // !JAPAN
#line 1671
#ifdef EGG
#line 1672
	if(ega_mode())
    		ega_sensible_graph_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  ifdef VGG
#line 1675
	else if(vga_256_mode())
    		vga_sensible_graph_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  endif
#line 1678
	else
#endif
#line 1680
    	kinky_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr,vd_cols_on_screen);
    else
    {
	if(alpha_num_mode())
    		sensible_text_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#ifdef EGG
#line 1686
	else if(ega_mode())
    		ega_sensible_graph_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  ifdef VGG
#line 1689
	else if(vga_256_mode())
    		vga_sensible_graph_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#  endif
#line 1692
#endif
#line 1693
	else
    		sensible_graph_scroll_down(t_row,l_col,rowsdiff,colsdiff,lines,attr);
#ifdef EGA_DUMP
#line 1696
#error DIVERGENCE MVDM-HOST-DIV-527: EGA_DUMP branch is not selected; see mirror register.
#line 1698
#endif
#line 1699
    /*
     * re-enable video for this mode, if on a CGA adaptor (fixes ROUND42 bug).
     */
	if(video_adapter == CGA)
    	outb(CGA_CONTROL_REG, vd_mode_table[sas_hw_at_no_check(vd_video_mode)].mode_control_val);
    }
#ifdef	JAPAN
#line 1706
    // mskkbug#2757: works2.5: garbage remains after exiting install -yasuho
    // We need flush screen when scroll the screen.
    Int10FlagCnt++;
#endif // JAPAN
#line 1710
}

LOCAL void sensible_text_scroll_down IFN6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr)
{
    register sys_addr	source, dest;
#if !defined(i386) && defined(JAPAN)
#line 1716
    register sys_addr	sourceV, destV;
    int                 linesdiff;
    word attrW = (word)((attr << 8)|' ');
#endif // !i386 && JAPAN
#line 1720
    register int	col_incr;
    register int	i;
    boolean		screen_updated = FALSE;
    int			vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
#ifdef JAPAN
#line 1725
    // for mskkbug #875
    byte *p = &Int10Flag[ (t_row+rowsdiff-1) * vd_cols_on_screen + l_col];
#endif // JAPAN
#line 1728

#if defined(JAPAN) && defined(i386)
#line 1730
    // mode73h support
    if ( !is_us_mode() && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
       source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row)*2 + video_pc_low_regen;

       col_incr = sas_w_at_no_check(VID_COLS) * 4;	/* offset to next line */
    }
    else {
       source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + video_pc_low_regen;
       col_incr = sas_w_at_no_check(VID_COLS) * 2;	/* offset to next line */
    }
#else // !JAPAN || !i386
	source = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + vd_text_mode().start_addr;
#line 1742
	col_incr = sas_w_at_no_check(VID_COLS) * 2;
#if !defined(i386) && defined(JAPAN)
#line 1744
	sourceV = sas_w_at_no_check(VID_ADDR) + vd_page_offset(l_col, t_row) + DosvVramPtr;
#endif // !i386 && JAPAN
#line 1746
#endif // !386 || !JAPAN
#line 1747

	/* Try to scroll the adaptor memory & host screen. */

	if( source >= get_screen_base() )
	{
#if defined(JAPAN) && defined(i386)
#line 1753
            // mode73h support
	    if ( !is_us_mode() && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
                screen_updated = (*update_alg.scroll_up)(source,4*colsdiff,rowsdiff,attr,lines,0);
            }
            else {
                screen_updated = (*update_alg.scroll_up)(source,2*colsdiff,rowsdiff,attr,lines,0);
            }
#else // !JAPAN || !i386
#line 1761
	    screen_updated = (*update_alg.scroll_down)(source,2*colsdiff,rowsdiff,attr,lines,0);
#endif // !JAPAN || !i386
#line 1763
	}

    dest = source + (rowsdiff-1)*col_incr;
    source = dest - lines*col_incr;
#if !defined(i386) && defined(JAPAN)
#line 1768
    destV = sourceV + (rowsdiff-1)*col_incr;
    sourceV = destV - lines*col_incr;
    linesdiff = vd_cols_on_screen * lines;
#endif // !i386 && JAPAN
#line 1772
/*
 * NB if we are just doing a clear area, the scrolling 'for' loop will terminate immediately
 */

	for(i = 0; i < rowsdiff-lines; i++)
	{
#ifdef REAL_VGA
#line 1779
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1781
#ifdef JAPAN
#line 1782
		// for mskkbug #875
		if( !screen_updated ) {
		// mode73h support
#ifdef i386
#line 1786
		    if ( !is_us_mode() && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
	  	        sas_move_bytes_forward (source, dest, colsdiff*4);
                    }
                    else {
		        sas_move_bytes_forward (source, dest, colsdiff*2);
                    }

#else // !i386
#line 1794
		    sas_move_bytes_forward (source, dest, colsdiff*2);
		    if ( !is_us_mode())
		      sas_move_bytes_forward( sourceV, destV, colsdiff*2);
#endif // !i386
#line 1798
                    {
                        register int i;

                        for ( i = 0; i < colsdiff; i++ ) {
#ifdef i386
#line 1803
                            p[i] = ( p[i - vd_cols_on_screen] | INT10_CHANGED );
#else // !i386
#line 1805
			    //I think this is correct!!
                            p[i] = ( p[i - linesdiff] | INT10_CHANGED );
#endif // !i386
#line 1808
                        }
                    }
                }
#else // !JAPAN
#line 1812
		if( !screen_updated )
			sas_move_bytes_forward (source, dest, colsdiff*2);
#endif // !JAPAN
#line 1815
#endif
#line 1816
		source -= col_incr;
		dest -= col_incr;
#ifdef JAPAN
#line 1819
#if !defined(i386)
#line 1820
		sourceV -= col_incr;
		destV -= col_incr;
#endif // !i386
#line 1823
                p -= vd_cols_on_screen;
#endif // JAPAN
#line 1825
	}

    /* moved all the data we were going to move - blank the cleared region */

#if !defined(i386) && defined(JAPAN)
#line 1830
    if( sas_hw_at_no_check(DosvModePtr) == 0x73 )
      attrW = 0;
#endif // !i386 && JAPAN
#line 1833

	while(lines--)
	{
#ifdef REAL_VGA
#line 1837
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 1839
#ifdef JAPAN
#line 1840
		// for mskkbug #875
		if( !screen_updated ) {
		// mode73h support
#ifdef i386
#line 1844
//"dest" is on DOS address space, so we must use "sas" function to access it.
		    if ( !is_us_mode() && (sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
                        unsigned long *destptr = (unsigned long*)dest;
                        int i;

                        for ( i = 0; i < colsdiff; i++ ) {
                            *destptr++ = (attr << 8)|' ';
                        }
                    }
                    else {
 		        sas_fillsw(dest, (attr << 8)|' ', colsdiff);
                    }
#else // !i386
#line 1857
		        sas_fillsw(dest, (attr << 8)|' ', colsdiff);
                    if ( !is_us_mode() )
		        sas_fillsw(destV, attrW, colsdiff);
#endif // !i386
#line 1861
#ifdef i386
#line 1862
// "p" (Int10Flag) is on 32 bit address space, so we must no use "sas" function
// to access 32 bit address.
                    sas_fills( (sys_addr)p, ( INT10_SBCS | INT10_CHANGED ), colsdiff );
#else // !i386
#line 1866
                    {	
		      register int i = colsdiff;
		      register byte *pp = p;
		      while( i-- ){
			*pp++ = ( INT10_SBCS | INT10_CHANGED );
		      }
		    }
#endif // !i386
#line 1874
                }
#else // !JAPAN
#line 1876
		if( !screen_updated )
			sas_fillsw(dest, (IU16)((attr << 8)|' '), colsdiff);
#endif // !JAPAN
#line 1879
#endif
#line 1880
		dest -= col_incr;
#ifdef JAPAN
#line 1882
#if !defined(i386)
#line 1883
		destV -= col_incr;
#endif // !i386
#line 1885
                p -= vd_cols_on_screen;
#endif // JAPAN
#line 1887
	}
}

LOCAL void sensible_graph_scroll_down IFN6(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr)
{
    sys_addr	source, dest;
    int		i,colour;
    boolean		screen_updated;

	rowsdiff *= 4;		/* 8 scans per char, 4 per bank */
	lines *= 4;

	if( sas_hw_at_no_check(vd_video_mode) != 6 )
	{
		colour = attr & 0x3;
		colsdiff *= 2;		/* 4 pixels/byte */

		source = vd_medium_offset(l_col, t_row)+video_pc_low_regen;
	}
	else
	{
		colour = attr & 0x1;
		source = vd_high_offset(l_col, t_row)+video_pc_low_regen;
	}

	/* Try to scroll the host screen */

    screen_updated = (*update_alg.scroll_down)(source,colsdiff,rowsdiff,attr,lines,colour);

	if( screen_updated && ( video_adapter != CGA ))
		return;

    dest = source + (rowsdiff-1)*SCAN_LINE_LENGTH;
    source = dest - lines*SCAN_LINE_LENGTH;

	/*
	 * NB if we are just doing a clear area, the scrolling 'for' loop
	 * will terminate immediately
	 */

	for( i = 0; i < rowsdiff-lines; i++ )
	{
		/*
		 * graphics mode has to do odd & even banks
		 */

#ifdef REAL_VGA
#line 1934
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 1936
#else
#line 1937
		sas_move_bytes_forward (source, dest, colsdiff);
		sas_move_bytes_forward (source+ODD_OFF, dest+ODD_OFF, colsdiff);
#endif
#line 1940
		source -= SCAN_LINE_LENGTH;
		dest -= SCAN_LINE_LENGTH;
	}

	/* moved all the data we were going to move - blank the cleared region */

	while( lines-- )
	{
#ifdef REAL_VGA
#line 1949
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 1951
#else
#line 1952
		sas_fills(dest, (IU8)attr, colsdiff);
		sas_fills(dest+ODD_OFF, (IU8)attr, colsdiff);
#endif
#line 1955
		dest -= SCAN_LINE_LENGTH;
	}
}

LOCAL void kinky_scroll_down IFN7(int, t_row, int, l_col, int, rowsdiff, int, colsdiff, int, lines, int, attr, int, vd_cols_on_screen)
{
    register sys_addr	source, dest;
    register int	col_incr;
    register int	i;
    half_word is_alpha;

    switch (sas_hw_at_no_check(vd_video_mode)) {

    case 0: case 1: case 2:
    case 3: case 7:
	is_alpha = TRUE;
	col_incr = vd_cols_on_screen * 2;
	source = sas_w_at_no_check(VID_ADDR)+vd_page_offset(l_col, t_row)+vd_text_mode().start_addr;	/* top left */
	break;

    case 4: case 5: case 6:
	is_alpha = FALSE;
	rowsdiff *= 4;		/* 8 scans per char, 4 per bank */
	lines *= 4;
	col_incr = SCAN_LINE_LENGTH;
	if(sas_hw_at_no_check(vd_video_mode) != 6) {
	    colsdiff *= 2;		/* 4 pixels/byte */
	    source = vd_medium_offset(l_col, t_row)+video_pc_low_regen;
	}
	else
	    source = vd_high_offset(l_col, t_row)+video_pc_low_regen;
	break;

    default:
#ifndef PROD
#line 1990
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1992
	;
    }

    /* set origin of data movement for calculating screen refresh */
    dest = source + (rowsdiff-1)*col_incr;
    source = dest -lines*col_incr;

	/*
	 * NB if we are just doing a clear area, the scrolling 'for' loop
	 * will terminate immediately
	 */

	if (is_alpha) {
	    for(i = 0; i < rowsdiff-lines; i++) {
#ifdef REAL_VGA
#line 2007
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 2009
		sas_move_bytes_forward (source, dest, colsdiff*2);
#endif
#line 2011
	        source -= col_incr;
	        dest -= col_incr;
	    }
	}
	else {
	    for(i = 0; i < rowsdiff-lines; i++) {
#ifdef REAL_VGA
#line 2018
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 2020
		sas_move_bytes_forward (source, dest, colsdiff);
#endif
#line 2022
	        /*
	         * graphics mode has to do odd & even banks
	         */
#ifdef REAL_VGA
#line 2026
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 2028
		sas_move_bytes_forward (source+ODD_OFF, dest+ODD_OFF, colsdiff);
#endif
#line 2030
	        source -= col_incr;
	        dest -= col_incr;
	    }
	}

    /* moved all the data we were going to move - blank the cleared region */

    if (is_alpha) {		/* alpha blank */
	while(lines--) {
#ifdef REAL_VGA
#line 2040
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 2042
	    sas_fillsw(dest, (IU16)((attr << 8)|' '), colsdiff);
#endif
#line 2044
	    dest -= col_incr;
        }
    }
    else {			/* graphics blank */

	while(lines--) {
#ifdef REAL_VGA
#line 2051
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 2053
#else
#line 2054
	    sas_fills(dest, (IU8)attr, colsdiff);
	    sas_fills(dest+ODD_OFF, (IU8)attr, colsdiff);
#endif
#line 2057
	    dest -= col_incr;
	}
    }
}


GLOBAL void vd_read_attrib_char IFN0()
{
    /*
     * Routine to read character and attribute from the current cursor
     * position.
     * Parameters:
     *  AH - current video mode
     *  BH - display page (alpha modes)
     * Returns:
     *  AL - character read
     *  AH - attribute read
     */

    register sys_addr   cpos, cgen;
    register half_word	i, ext_no;
    word	        chattr;      /* unfortunately want to take addr */
    word		vd_cols_on_screen;
    half_word	        match[CHAR_MAP_SIZE], tmp[CHAR_MAP_SIZE];
    half_word vd_cursor_col, vd_cursor_row;

    /* Load internal variables with the values stored in BIOS
     * data area.
     */

    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
    vd_cursor_col = sas_hw_at_no_check(VID_CURPOS + getBH()*2);
    vd_cursor_row = sas_hw_at_no_check(VID_CURPOS + getBH()*2 + 1);

   if (alpha_num_mode()) {		/* alpha */
#if defined(JAPAN) && defined(i386)
#line 2093
      // mode73h support
      if ( ( !is_us_mode() ) && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) )
        cpos = video_pc_low_regen + vd_cursor_offset(getBH()) * 2;
      else
#endif // JAPAN && i386
	cpos = vd_text_mode().start_addr + vd_cursor_offset(getBH());
#line 2099
#ifdef A2CPU
#line 2100
#error DIVERGENCE MVDM-HOST-DIV-520: A2CPU platform branch is not selected; see mirror register.
#line 2103
#else
#line 2104
	sas_loadw(cpos,&chattr);
#endif /* A2CPU */
#line 2106
	setAX(chattr);			/* hmm that was easy */
    }
#ifdef EGG
#line 2109
    else if(ega_mode())
	ega_read_attrib_char(vd_cursor_col,vd_cursor_row,getBH());
#  ifdef VGG
#line 2112
    else if(vga_256_mode())
	vga_read_attrib_char(vd_cursor_col,vd_cursor_row,getBH());
#  endif
#line 2115
#endif
#line 2116
    else {
	/*
	 * graphics not so easy - have to build 8 byte string with all
	 * colour attributes masked out then match that in the character
	 * generator table (and extended character set if necessary)
	 */
	if (sas_hw_at_no_check(vd_video_mode) != 6)
	    cpos = video_pc_low_regen
            + 2 * (((vd_cursor_row * vd_cols_on_screen) << 2) + vd_cursor_col);
	else
	    cpos = video_pc_low_regen
              + vd_high_offset(vd_cursor_col,vd_cursor_row);
	if (sas_hw_at_no_check(vd_video_mode) == 6) {	/* high res */
	    for(i = 0; i < 4; i++) {	/* build 8 byte char string */
		sas_load(cpos, &match[i*2]);
		sas_load(cpos+ODD_OFF, &match[i*2+1]);
		cpos += 80;
	    }
	}
        else {				/* med res */
            /*
             * Note that in the following, the attribute byte must end
             * up in the LOW byte. That's why the bytes are swapped after the
             * sas_loadw().
             */
	    for(i = 0; i < 4; i++) {		/* to build char string, must */
		sas_loadw(cpos,&chattr);
		chattr = ((chattr>>8) | (chattr<<8)) & 0xffff;

		/* mask out foreground colour */
		match[i*2] = fgcolmask(chattr);

		sas_loadw(cpos+ODD_OFF,&chattr);
		chattr = ((chattr>>8) | (chattr<<8)) & 0xffff;

		/* mask out foreground colour */
		match[i*2+1] = fgcolmask(chattr);
		cpos += 80;
	    }
	}
#ifdef EGG
#line 2157
	if(video_adapter == EGA || video_adapter == VGA)
	    cgen = extend_addr(EGA_FONT_INT*4);
	else
	    cgen = CHAR_GEN_ADDR;			/* match in char generator */
#else
#line 2162
#error DIVERGENCE MVDM-HOST-DIV-540: non-EGG branch is not selected; see mirror register.
#endif
#line 2164
	if (cgen != 0)
		for(i = 0; i < CHARS_IN_GEN; i++) {
			sas_loads (cgen, tmp, sizeof(tmp));
		    if (memcmp(tmp, match, sizeof(match)) == 0)	/* matched */
				break;
		    cgen += CHAR_MAP_SIZE;	/* next char string */
		}
	else
		i = CHARS_IN_GEN;

	if (i < CHARS_IN_GEN)				/* char found */
	    setAL(i);
	else {
	    /*
	     * look for char in extended character set
	     */
	    if ((cgen = extend_addr(BIOS_EXTEND_CHAR*4)) != 0)
	    	for(ext_no = 0; ext_no < CHARS_IN_GEN; ext_no++) {
			sas_loads (cgen, tmp, sizeof(tmp));
		    if (memcmp(tmp, match, sizeof(match)) == 0)	/* matched */
		    		break;
			cgen += CHAR_MAP_SIZE;	/* still valid char len */
	    	}
	    else
		ext_no = CHARS_IN_GEN;

	    if (ext_no < CHARS_IN_GEN)		/* match found... */
		setAL((UCHAR)(ext_no + CHARS_IN_GEN));
	    else
		setAL(0);			/* no match, return 0 */
	}
    }
}


GLOBAL void vd_write_char_attrib IFN0()
{
/*
* Routine to write character and attribute from the current cursor
* position.
* Parameters:
*  AH - current video mode
*  BH - display page (alpha & EGA modes)
*  CX - # of characters to write
*  AL - Character to write
*  BL - attribute of character to write. If in graphics mode then
*       attribute is foreground colour. In that case if bit 7 of BL
*       is set then char is XOR'ed into buffer.
*/

    register word i, cpos;
    word vd_cols_on_screen;
    half_word vd_cursor_col, vd_cursor_row;
#ifdef JAPAN
#line 2218
    word vram_addr;
    static int DBCSState = FALSE;
#endif // JAPAN
#line 2221

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
    vd_cursor_col = sas_hw_at_no_check(VID_CURPOS + getBH()*2);
    vd_cursor_row = sas_hw_at_no_check(VID_CURPOS + getBH()*2 + 1);

    if (alpha_num_mode())
    {
#ifdef JAPAN
#line 2232
	    // stress test sets cursor over 25line
            if ( !is_us_mode() ) {
                if ( vd_cursor_row > 25 )
                    return; // we can't write VRAM!!
            }
#ifdef i386
#line 2238
	    // mode73h support
	    if ( ( !is_us_mode() ) && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) )
                cpos = vd_cursor_offset(getBH()) * 2;
            else
#endif // !i386
#line 2243
#endif // JAPAN
#line 2244
		cpos = vd_cursor_offset(getBH());

#ifdef JAPAN
#line 2247
		// Int10Flag set
                Int10FlagCnt++;
                vram_addr = vd_page_offset(vd_cursor_col,vd_cursor_row)/2;
#endif // JAPAN
#line 2251
		/* place in memory */

#ifdef REAL_VGA
#line 2254
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#else
#line 2256
		for(i = 0; i < getCX(); i++)
		{
#if ( defined(NTVDM) && defined(MONITOR) && !defined(MVDM_STANDALONE_SAS_VIDEO) ) || defined(GISP_SVGA)/* No Ega planes... */
#line 2259
#error Excluded unsupported original monitor or GISP video pointer
#else
#line 2261
#ifdef	EGG
#line 2262
			if ( ( (video_adapter != CGA) && (EGA_CPU.chain != CHAIN2) )
#ifdef CPU_40_STYLE
#line 2264
				|| (getVM())	/* if we are in V86 mode, the memory may be mapped... */
#endif
#line 2266
				)
				sas_storew(vd_text_mode().start_addr + cpos, (getBL() << 8) | getAL());
			else
#endif	/* EGG */
#line 2270
				(*bios_ch2_word_wrt_fn)( (getBL() << 8) | getAL(), cpos );
#endif	/* NTVDM & MONITOR */
#line 2272
#if defined(JAPAN) && defined(i386)
#line 2273
		    // mode73h support
		    if ( ( !is_us_mode() ) && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) )
                        cpos += 4;
                    else
#endif // JAPAN && i386
#line 2278
			cpos += 2;
#ifdef JAPAN
#line 2280
		        // Int10Flag set
#if 0
#line 2282
                        DbgPrint( "vd_write_char_attrib: Int10Flag Offset=%04X\n", vram_addr );
#endif
#line 2284
                        if ( DBCSState ) {
                            Int10Flag[vram_addr] = INT10_DBCS_TRAILING | INT10_CHANGED;
                            DBCSState = FALSE;
                        }
                        else if ( DBCSState = is_dbcs_first( getAL() ) ) {
                            Int10Flag[vram_addr] = INT10_DBCS_LEADING | INT10_CHANGED;
                        }
                        else {
                            Int10Flag[vram_addr] = INT10_SBCS | INT10_CHANGED;
                        }
                        vram_addr++;
#endif // JAPAN
#line 2296
		}
#endif
#line 2298
    }
#ifdef EGG
#line 2300
    else if(ega_mode())
	ega_graphics_write_char(vd_cursor_col,vd_cursor_row,getAL(),getBL(),getBH(),getCX());
#  ifdef VGG
#line 2303
    else if(vga_256_mode())
	vga_graphics_write_char(vd_cursor_col,vd_cursor_row,getAL(),getBL(),getBH(),getCX());
#  endif
#line 2306
#endif
#line 2307
    else
	/* rather more long winded - call common routine as vd_write_char() */
	graphics_write_char(vd_cursor_col, vd_cursor_row, getAL(), getBL(), getCX());
}


GLOBAL void vd_write_char IFN0()
{
    /*
     * Write a character a number of times starting from the current cursor
     * position.  Parameters are held in the following registers.
     *
     * AH - Crt Mode
     * AL - Character to write
     * CX - Number of characters
     * BH - display page
     *
     */

    register word i, cpos;
    word vd_cols_on_screen;
    half_word vd_cursor_col, vd_cursor_row;
#ifdef JAPAN
#line 2330
    static int DBCSState = FALSE;
    word vram_addr;
#endif // JAPAN
#line 2333

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
    vd_cursor_col = sas_hw_at_no_check(VID_CURPOS + getBH()*2);
    vd_cursor_row = sas_hw_at_no_check(VID_CURPOS + getBH()*2 + 1);

    /*
     * handle alphanumeric here:
     */

	if (alpha_num_mode())
	{
#ifdef JAPAN
#line 2348
	    // stress test sets cursor over 25line
            if ( !is_us_mode() ) {
                if ( vd_cursor_row > 25 )
                    return; // we can't write VRAM!!
            }
#ifdef i386
#line 2354
	    // mode73h support
	    if ( ( !is_us_mode() ) && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) )
                cpos = vd_cursor_offset(getBH()) * 2;
            else
#endif // i386
#line 2359
#endif // JAPAN
#line 2360
		cpos = vd_cursor_offset(getBH());
#ifdef JAPAN
#line 2362
	        // Int10Flag set
                Int10FlagCnt++;
                //vram_addr = vd_cursor_offset(getBH()) >> 1;
                vram_addr = vd_page_offset(vd_cursor_col,vd_cursor_row)/2; //7/23/1993 V-KazuyS
#endif // JAPAN
#line 2367

		/* store in memory, skipping attribute bytes */

		for(i = 0; i < getCX(); i++)
		{
#if ( defined(NTVDM) && defined(MONITOR) && !defined(MVDM_STANDALONE_SAS_VIDEO) ) || defined( GISP_SVGA )
#line 2373
#error Excluded unsupported original monitor or GISP video pointer
#else
#line 2375
#ifdef	EGG
#line 2376
			if ( ( (video_adapter != CGA) && (EGA_CPU.chain != CHAIN2) )
#ifdef CPU_40_STYLE
#line 2378
				|| (getVM())	/* if we are in V86 mode, the memory may be mapped... */
#endif
#line 2380
				)
				sas_store(vd_text_mode().start_addr + cpos, getAL());
			else
#endif	/* EGG */
#line 2384
				(*bios_ch2_byte_wrt_fn)( getAL(), cpos );
#endif	/* NTVDM & MONITOR */
#line 2386
#if defined(JAPAN) && defined(i386)
#line 2387
		    // mode73h support
		    if ( ( !is_us_mode() ) && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) )
                        cpos += 4;
                    else
#endif // JAPAN && i386
#line 2392
			cpos += 2;
#ifdef JAPAN
#line 2394
			// Int10Flag set
#if 0
#line 2396
                        DbgPrint( "vd_write_char(%d,%d): Int10Flag Offset=%04X\n", vd_cursor_row, vd_cursor_col, vram_addr );
#endif
#line 2398
                        if ( DBCSState ) {
                            Int10Flag[vram_addr] = INT10_DBCS_TRAILING | INT10_CHANGED;
                            DBCSState = FALSE;
                        }
                        else if ( DBCSState = is_dbcs_first( getAL() ) ) {
                            Int10Flag[vram_addr] = INT10_DBCS_LEADING | INT10_CHANGED;
                        }
                        else {
                            Int10Flag[vram_addr] = INT10_SBCS | INT10_CHANGED;
                        }
                        vram_addr++;
#endif // JAPAN
#line 2410
		}
	}

    /*
     * handle graphics seperately - I know what you're thinking - why pass
     * BL as the attribute when this routine is meant to leave the attribute
     * well alone. Well this is the way it's done in the bios! If it causes
     * problems then we'll need to do a vd_read_char_attr here and then pass the
     * attribute gleaned from that.
     */
#ifdef EGG
#line 2421
    else if(ega_mode())
	ega_graphics_write_char(vd_cursor_col,vd_cursor_row,getAL(),getBL(),getBH(),getCX());
#  ifdef VGG
#line 2424
    else if(vga_256_mode())
	vga_graphics_write_char(vd_cursor_col,vd_cursor_row,getAL(),getBL(),getBH(),getCX());
#  endif
#line 2427
#endif
#line 2428
    else
	graphics_write_char(vd_cursor_col, vd_cursor_row, getAL(), getBL(), getCX());
}


GLOBAL void vd_set_colour_palette IFN0()
{
    /*
     * Set Colo[u]r Palette. Established background, foreground & overscan
     * colours.
     * Parameters:
     *   BH - Colour Id
     *   BL - Colour to set
     *      if BH == 0 background colour set from low bits of BL
     *      if BH == 1 selection made based on low bit of BL
     */

    /* Load internal variables with the values stored in BIOS
     * data area.
     */

    if (getBH() == 1) {		/* use low bit of BL */
	sas_store_no_check (vd_crt_palette, (IU8)(sas_hw_at_no_check(vd_crt_palette) & 0xDF));
	if (getBL() & 1)
	    sas_store_no_check (vd_crt_palette, (IU8)(sas_hw_at_no_check(vd_crt_palette) | 0x20));
    }
    else
	sas_store_no_check (vd_crt_palette, (IU8)((sas_hw_at_no_check(vd_crt_palette) & 0xE0) | (getBL() & 0x1F)));

    /* now tell the 6845 */
    outb(CGA_COLOUR_REG, sas_hw_at_no_check(vd_crt_palette));

}


GLOBAL void vd_write_dot IFN0()
{
    /*
     * Write dot
     * Parameters:
     *  DX - row (0-349)
     *  CX - column (0-639)
     *  BH - page
     *  AL - dot value; right justified 1,2 or 4 bits mode dependant
     *       if bit 7 of AL == 1 then XOR the value into mem.
     */

    half_word	dotval, data;
    int	dotpos, lsb;			/* dot posn in memory */
    half_word  right_just, bitmask;

#ifdef EGG
#line 2480
    if(ega_mode())
    {
	ega_write_dot(getAL(),getBH(),getCX(),getDX());
	return;
    }
#  ifdef VGG
#line 2486
    else if(vga_256_mode())
    {
	vga_write_dot(getAL(),getBH(),getCX(),getDX());
	return;
    }
#  endif
#line 2492
#endif
#line 2493
    dotpos = getDL();			/* row */

    if (dotpos & 1)			/* set up for odd or even banks */
		dotpos = ODD_OFF-40 + 40 * dotpos;
    else
		dotpos *= 40;

    /*
     * different pixel memory sizes for different graphics modes. Mode 6
     * is high res, mode 4,5 medium res
     */

    dotval = getAL();

    if (sas_hw_at_no_check(vd_video_mode) < 6)
    {
		/*
		 * Modes 4 & 5 (medium res)
		 */
		dotpos += getCX() >> 2;		/* column offset */
		right_just = (getCL() & 3) << 1;/* displacement in byte */
		dotval = (dotval & 3) << (6-right_just);
		bitmask = (0xC0 >> right_just); /* bits of interest */

#ifdef EGG
#line 2518
		/*
		 * EGA & VGA can be told which byte has changed, CGA is
		 * only told that screen has changed.
		 */
		if ( video_adapter != CGA )
    			(*update_alg.mark_byte) ( dotpos );
		else
#endif
#line 2526
			setVideodirty_total(getVideodirty_total() + 2);

		/*
		 * if the top bit of the value to write is set then value is xor'ed
		 * onto the screen, otherwise it is or'ed on.
		 */

		if( getAL() & 0x80 )
		{
#ifdef	EGG
#line 2536
			if( video_adapter != CGA )
			{
				lsb = dotpos & 1;
				dotpos = (dotpos >> 1) << 2;
				dotpos |= lsb;

				data = EGA_planes[dotpos];
				EGA_planes[dotpos] =  data ^ dotval;
			}
			else
#endif	/* EGG */
#line 2547
			{
				data = *(UTINY *) get_screen_ptr( dotpos );	
				*(UTINY *) get_screen_ptr( dotpos ) =
					data ^ dotval;
			}
		}
		else
		{
#ifdef	EGG
#line 2556
			if( video_adapter != CGA )
			{
				lsb = dotpos & 1;
				dotpos = (dotpos >> 1) << 2;
				dotpos |= lsb;

				data = EGA_planes[dotpos];
				EGA_planes[dotpos] = (data & ~bitmask) |
					dotval;
			}
			else
#endif	/* EGG */
#line 2568
			{
				data = *(UTINY *) get_screen_ptr( dotpos );	
				*(UTINY *) get_screen_ptr( dotpos ) =
					(data & ~bitmask) | dotval;
			}
		}
    }
    else
    {
		/*
		 * Mode 6 (hi res)
		 */
		dotpos += getCX() >> 3;
		right_just = getCL() & 7;
		dotval = (dotval & 1) << (7-right_just);
		bitmask = (0x80 >> right_just);

#ifdef EGG
#line 2586
		/*
		 * EGA & VGA can be told which byte has changed, CGA is
		 * only told that screen has changed.
		 */
		if ( video_adapter != CGA )
    			(*update_alg.mark_byte) ( dotpos );
		else
#endif
#line 2594
			setVideodirty_total(getVideodirty_total() + 2);

		/*
		 * if the top bit of the value to write is set then value is xor'ed
		 * onto the screen, otherwise it is or'ed on.
		 */

		if( getAL() & 0x80 )
		{
#ifdef	EGG
#line 2604
			if( video_adapter != CGA )
			{
				data = EGA_planes[dotpos << 2];
				EGA_planes[dotpos << 2] =  data ^ dotval;
			}
			else
#endif	/* EGG */
#line 2611
			{
				data = *(UTINY *) get_screen_ptr( dotpos );	
				*(UTINY *) get_screen_ptr( dotpos ) =
					data ^ dotval;
			}
		}
		else
		{
#ifdef	EGG
#line 2620
			if( video_adapter != CGA )
			{
				data = EGA_planes[dotpos << 2];
				EGA_planes[dotpos << 2] = (data & ~bitmask) |
					dotval;
			}
			else
#endif	/* EGG */
#line 2628
			{
				data = *(UTINY *) get_screen_ptr( dotpos );	
				*(UTINY *) get_screen_ptr( dotpos ) =
					(data & ~bitmask) | dotval;
			}
		}
    }
}

extern void ega_read_dot (int, int, int);

GLOBAL void vd_read_dot IFN0()
{
    /*
     * Read dot
     * Parameters:
     *  DX - row (0-349)
     *  CX - column (0-639)
     * Returns
     *  AL - dot value read, right justified, read only
     */

    int	dotpos;			/* dot posn in memory */
    half_word  right_just, bitmask, data;

#ifdef EGG
#line 2654
    if(ega_mode())
    {
	ega_read_dot(getBH(),getCX(),getDX());
	return;
    }
#  ifdef VGG
#line 2660
    else if(vga_256_mode())
    {
	vga_read_dot(getBH(),getCX(),getDX());
	return;
    }
#  endif
#line 2666
#endif
#line 2667
    dotpos = getDL();			/* row */
    if (dotpos & 1)			/* set up for odd or even banks */
	dotpos = ODD_OFF-40 + 40 * dotpos;
    else
	dotpos *= 40;
    /*
     * different pixel memory sizes for different graphics modes. Mode 6
     * is high res, mode 4,5 medium res
     */

    if (sas_hw_at_no_check(vd_video_mode) < 6) {
	dotpos += getCX() >> 2;		/* column offset */
	right_just = (3 - (getCL() & 3)) << 1;/* displacement in byte */
	bitmask = 3; 			/* bits of interest */
    }
    else {
	dotpos += getCX() >> 3;
	right_just = 7 - (getCL() & 7);
	bitmask = 1;
    }
    /*
     * get value of memory at that position, shifted down to bottom of byte
     * Result returned in AL.
     */

	sas_load(video_pc_low_regen+dotpos, &data);	
    setAL((UCHAR)(( data >> right_just) & bitmask));
}


#ifdef CPU_40_STYLE
#line 2698

/* Optimisations are not possible, IO virtualisation may be active. */
#define OUTB(port, val) outb(port, val)

#else
#line 2703
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#line 2711
#endif /* CPU_40_STYLE */
#line 2712

GLOBAL void vd_write_teletype IFN0()
{
    /*
     * Provide a teletype interface.  Put a character to the screen
     * allowing for scrolling etc.  The parameters are
     *
     * AL - Character to write
     * BL - Foreground colour in graphics mode
     */

    register char	ch;
    register sys_addr	ch_addr;
    int			cur_pos;
    word vd_addr_6845 = sas_w_at_no_check(VID_INDEX);
    half_word		scroll_required = FALSE;
    half_word		attrib;
    register half_word	vd_cursor_row,vd_cursor_col;
    word 		vd_cols_on_screen;
#ifdef ANSI
#line 2732
     IMPORT VOID (**get_outb_ptr(io_addr))(io_addr address, half_word value);
#else
#line 2734
#error Unsupported excluded original configuration
#endif
#line 2736
#ifdef NTVDM
#line 2737
     void (** hack)(io_addr address, half_word value);
#endif
#line 2739

    unsigned short savedAX, savedBX, savedCX, savedIP, savedCS, savedDX;
    unsigned short re_entrant = FALSE;
#ifdef JAPAN
#line 2743
    static short dbcs_status = FALSE;
    half_word	 move_cursor_lines = 0;
#endif // JAPAN
#line 2746

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    ch = getAL();
    if (stream_io_enabled) {
	if (*stream_io_dirty_count_ptr >= stream_io_buffer_size)
	    stream_io_update();
	stream_io_buffer[(*stream_io_dirty_count_ptr)++] = ch;
	return;
    }

    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);

#if defined(NTVDM) && defined(MONITOR)
#line 2761
        /*
        ** Tim August 92, Microsoft. Need to change this test, cos INT 10
        ** vector now points into the NTIO.SYS driver on X86.
        */
        {
                extern word int10_seg;

                re_entrant = (sas_w_at_no_check(0x42) != int10_seg);
        }
#else
#line 2771
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#line 2782
#endif
#line 2783

    vd_cursor_col = sas_hw_at_no_check(current_cursor_col);
    vd_cursor_row = sas_hw_at_no_check(current_cursor_row);

#ifdef JAPAN
#line 2788
	if ( dbcs_status == FALSE ) {
	    if ( is_dbcs_first(ch) ) {
		if ( vd_cursor_col + 1 == vd_cols_on_screen ) {
		    savedAX = getAX();
		    savedBX = getBX();
		    setAL( 0x20 );            /* space */
		    vd_write_teletype();
		    setBX( savedBX );
		    setAX( savedAX );
		    // get new col and row
		    vd_cursor_col = sas_hw_at_no_check(current_cursor_col);
		    vd_cursor_row = sas_hw_at_no_check(current_cursor_row);
	        }
		dbcs_status = TRUE;
		goto write_char;
	    }
	}
	else {			/* if kanji second byte then write */
	    dbcs_status = FALSE;
	    goto write_char;
	}

#endif // JAPAN
#line 2811
    /*
     * First check to see if it is a control character and if so action
     * it here rather than call the write char function.
     */

    switch (ch)
    {
    case VD_BS:  			/* Backspace	*/
	if (vd_cursor_col != 0) {
	    vd_cursor_col--;
	}
	break;

    case VD_CR:			/* Return	*/
        vd_cursor_col = 0;
	break;

    case VD_LF:			/* Line feed	*/
	/* Row only should be checked for == (25-1), so in principle
	 * it ignores LF off the top of the screen.
	 */
#ifdef JAPAN
#line 2833
	// scroll problem when start VDM 24line with $IAS.SYS
#ifdef JAPAN_DBG
#line 2835
// DbgPrint("LF---vd_row=%d, VD_ROWS=%d\n", vd_cursor_row, VD_ROWS_ON_SCREEN );
#endif
#line 2837
	if (vd_cursor_row > VD_ROWS_ON_SCREEN) {
                move_cursor_lines = vd_cursor_row - VD_ROWS_ON_SCREEN;
                if ( move_cursor_lines >= VD_ROWS_ON_SCREEN )   // 8/28/1993
                    move_cursor_lines =  0; //VD_ROWS_ON_SCREEN - 1; // Stress test
                vd_cursor_row = VD_ROWS_ON_SCREEN;
		scroll_required = TRUE;
        }
        else
#endif // JAPAN
#line 2846
	if (vd_cursor_row == VD_ROWS_ON_SCREEN)
		scroll_required = TRUE;
	else
		vd_cursor_row++;
	break;

    case VD_BEL:			/* Bell		*/
        host_ring_bell(BEEP_LENGTH);
        return;			/* after all, shouldn't cause a scroll */

    default:
#ifdef JAPAN
#line 2858
write_char:
#endif // JAPAN
#line 2860
        /*
         * It's a real character, place it in the regen buffer.
         */
        if(alpha_num_mode())
	{
	    if(re_entrant)
            {
                 savedAX = getAX();
                 savedBX = getBX();
                 savedCX = getCX();
                 savedIP = getIP();
                 savedCS = getCS();

                 setAH(WRITE_A_CHAR);
                 setBH(sas_hw_at_no_check(vd_current_page));
                 setCX(1);

#if defined(NTVDM) && defined(X86GFX)
#line 2878
                /*
                ** Tim August 92 Microsoft. INT 10 caller code is now
                ** in NTIO.SYS
                */
                {
                        extern word int10_seg, int10_caller;

                        exec_sw_interrupt( int10_seg, int10_caller );
                }
#else
#line 2888
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#line 2892
#endif	/* NTVDM & MONITOR */
#line 2893

		/*
		 * Note: Always make sure CS comes before IP
		 */
                 setCS(savedCS);
                 setIP(savedIP);
                 setCX(savedCX);
                 setBX(savedBX);
                 setAX(savedAX);
            }
            else
	    {
#if defined(JAPAN) && defined(i386)
#line 2906
	// alpha always uses EGA plane.
	// mode73h support
	if( !is_us_mode() && ( sas_hw_at_no_check(DosvModePtr) == 0x73 ) ) {
#ifdef JAPAN_DBG
#line 2910
                 DbgPrint( "@" );
#endif
#line 2912
	         ch_addr = sas_w_at_no_check(VID_ADDR) +
                              vd_page_offset(vd_cursor_col,vd_cursor_row) * 2;
        }
        else {
	         ch_addr = sas_w_at_no_check(VID_ADDR) +
                              vd_page_offset(vd_cursor_col,vd_cursor_row);
        }
#else // !JAPAN || !i386
#line 2920
	         ch_addr = sas_w_at_no_check(VID_ADDR) +
                              vd_page_offset(vd_cursor_col,vd_cursor_row);
#endif // !JAPAN && !i386
#line 2923

		/*
		 *	Call the C code to do the biz rather than brothel
		 *	around in SAS.
		 */

#if ( defined(NTVDM) && defined(MONITOR) && !defined(MVDM_STANDALONE_SAS_VIDEO) ) || defined( GISP_SVGA )
#line 2930
#error Excluded unsupported original monitor or GISP video pointer
#else
#line 2932
#ifdef	EGG
#line 2933
			if ( ( (video_adapter != CGA) && (EGA_CPU.chain != CHAIN2) )
#ifdef CPU_40_STYLE
#line 2935
				|| (getVM())	/* if we are in V86 mode, the memory may be mapped... */
#endif
#line 2937
				)
				sas_store(vd_text_mode().start_addr + ch_addr, ch);
			else
#endif	/* EGG */
#line 2941
				(*bios_ch2_byte_wrt_fn)( ch, ch_addr );
#endif	/* ( NTVDM & MONITOR ) | GISP_SVGA */
#line 2943
	    }
	}
#ifdef EGG
#line 2946
	else if(ega_mode())
            ega_graphics_write_char(vd_cursor_col, vd_cursor_row, ch, getBL(),sas_hw_at_no_check(vd_current_page), 1);
#  ifdef VGG
#line 2949
	else if(vga_256_mode())
            vga_graphics_write_char(vd_cursor_col, vd_cursor_row, ch, getBL(),sas_hw_at_no_check(vd_current_page), 1);
#  endif
#line 2952
#endif
#line 2953
        else
            graphics_write_char(vd_cursor_col, vd_cursor_row, ch, getBL(), 1);

        vd_cursor_col++;
        /*
         * Now see if we have gone off the edge of the screen
         */

        if (vd_cursor_col == vd_cols_on_screen)
        {
            vd_cursor_col = 0;

	    /* Row only should be checked for == (25-1) and
	     * only if there was a line wrap.
	     */
#ifdef JAPAN
#line 2969
	    // scroll problem when start VDM 24line with $IAS.SYS
#ifdef JAPAN_DBG
#line 2971
// DbgPrint("col==80 --vd_row=%d, VD_ROWS=%d\n", vd_cursor_row, VD_ROWS_ON_SCREEN );
#endif
#line 2973
	    if (vd_cursor_row > VD_ROWS_ON_SCREEN) {
                move_cursor_lines = vd_cursor_row - VD_ROWS_ON_SCREEN;
                if ( move_cursor_lines >= VD_ROWS_ON_SCREEN )   // 8/28/1993
                    move_cursor_lines =  0; //VD_ROWS_ON_SCREEN - 1; // Stress test
                vd_cursor_row = VD_ROWS_ON_SCREEN;
		scroll_required = TRUE;
            }
            else
#endif // JAPAN
#line 2982
	    if (vd_cursor_row == VD_ROWS_ON_SCREEN)
		scroll_required = TRUE;
	    else
          	vd_cursor_row++;
        }

        /* cursor_row validity actually never checked unless processing a
         * Line Feed or a wrapping at the end of line.
	 *
	 * The BYTE "text" benchmark program contains an off-by-one error
	 * which causes it to set the cursor position off the end of the
	 * screen: SoftPC was incorrectly deciding to scroll, with consequent
	 * horrendous time penalties...
	 */
    }

    /*
     * By this point we have calculated the new cursor position
     * so output the cursor position and the character
     */

    if(alpha_num_mode())
    {
#ifdef REAL_VGA
#line 3006
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 3020
#else
#line 3021
	if(re_entrant)
        {
             savedAX = getAX();
             savedBX = getBX();
             savedDX = getDX();
             savedIP = getIP();
             savedCS = getCS();

             setAH(SET_CURSOR_POS);
             setBH(sas_hw_at_no_check(vd_current_page));
             setDH(vd_cursor_row);
             setDL(vd_cursor_col);

#if defined(NTVDM) && defined(X86GFX)
#line 3035
                /*
                ** Tim August 92 Microsoft. INT 10 caller code is now
                ** in NTIO.SYS
                */
                {
                        extern word int10_seg, int10_caller;

                        exec_sw_interrupt( int10_seg, int10_caller );
                }
#else
#line 3045
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#line 3049
#endif	/* NTVDM & MONITOR */
#line 3050

		/*
		 * Note: Always make sure CS comes before IP
		 */

             setCS(savedCS);
             setIP(savedIP);
             setDX(savedDX);
             setBX(savedBX);
             setAX(savedAX);
        }
        else
        {
        /*
		** tell the 6845 all about the change
		*/

		/* Set the current position - word address, not byte */
		cur_pos = (sas_w_at_no_check(VID_ADDR) +
			vd_page_offset(vd_cursor_col, vd_cursor_row)) >> 1;

		OUTB(M6845_INDEX_REG, R14_CURS_ADDRH);
		OUTB(M6845_DATA_REG,  (IU8)(cur_pos >> 8));
		OUTB(M6845_INDEX_REG, R15_CURS_ADDRL);
		OUTB(M6845_DATA_REG,  (IU8)(cur_pos & 0xff));

		/*
		* store the new cursor position in the
		* bios vars (this should be done by the re-entrant
		* code called above)
		*/
		sas_store_no_check (current_cursor_col , vd_cursor_col);
		sas_store_no_check (current_cursor_row , vd_cursor_row);

        }
#endif
#line 3086
    }
    else {
		/*
		* store the new cursor position in the
		* bios vars for graphics mode
		*/
		sas_store_no_check (current_cursor_col , vd_cursor_col);
		sas_store_no_check (current_cursor_row , vd_cursor_row);
    }

    if (scroll_required)
    {
	/*
	 * Update the memory to be scrolled
	 */
	if (alpha_num_mode()) {
#ifdef A2CPU
#line 3103
#error DIVERGENCE MVDM-HOST-DIV-520: A2CPU platform branch is not selected; see mirror register.
#line 3106
#else
		sas_load( vd_text_mode().start_addr + sas_w_at_no_check(VID_ADDR)+vd_page_offset(vd_cursor_col,vd_cursor_row) + 1, &attrib);
#line 3108
#endif /* A2CPU */
#line 3109

#ifdef JAPAN
#line 3111
#ifdef JAPAN_DBG
#line 3112
//               DbgPrint("Scroll_required!!!\n" );
#endif
#line 3114
		sensible_text_scroll_up( 0, 0,
                                    VD_ROWS_ON_SCREEN + 1 + move_cursor_lines,
                                    vd_cols_on_screen,
                                    1 + move_cursor_lines,
                                    attrib);
		// #3920: CR+LFs are needed when using 32bit cmd in command.com
		// 12/9/93 yasuho
		Int10FlagCnt++;
#else // !JAPAN
#line 3123
		sensible_text_scroll_up(0,0, VD_ROWS_ON_SCREEN+1,vd_cols_on_screen,1,attrib);
#endif // !JAPAN
#line 3125

	}
#ifdef EGG
#line 3128
	else if(ega_mode())
		ega_sensible_graph_scroll_up(0,0, VD_ROWS_ON_SCREEN+1,vd_cols_on_screen,1,0);
#  ifdef VGG
#line 3131
	else if(vga_256_mode())
		vga_sensible_graph_scroll_up(0,0, VD_ROWS_ON_SCREEN+1,vd_cols_on_screen,1,0);
#  endif
#line 3134
#endif
#line 3135
	else 	/* graphics mode */

		sensible_graph_scroll_up(0,0, VD_ROWS_ON_SCREEN+1,vd_cols_on_screen,1,0);

#ifdef EGA_DUMP
#line 3140
#error DIVERGENCE MVDM-HOST-DIV-527: EGA_DUMP branch is not selected; see mirror register.
#line 3143
#endif
#line 3144

    }
}

GLOBAL void vd_write_string IFN0()
{
    /*
     * AL = write mode (0-3)
     *      Specical for NT: if AL = 0xff then write character string
     *      with existing attributes.
     * BH = page
     * BL = attribute (if AL=0 or 1)
     * CX = length
     * DH = Y coord
     * DL = x coord
     * ES:BP = pointer to string.
     *
     *  NB. This routine behaves very strangely wrt line feeds etc -
     *  These ALWAYS affect the current page!!!!!
     */
	int i,op;
    UCHAR col,row;
    USHORT len;
	UCHAR save_col,save_row;
	sys_addr ptr;
	boolean ctl;
#ifdef NTVDM
#line 3171
	word	count, avail;
#endif
#line 3173
#ifdef JAPAN
#line 3174
	// Big fix for multiplan
	// ntraid:mskkbug#2784: Title of VJE-PEN is strange
	// ntraid:mskkbug#3014: VJE-PEN: function keys don't work on windowed
	// 11/5/93 yasuho
	// Don't broken AX, BX, DX register !!
        IU16 saveAX, saveBX, saveDX;
#endif // JAPAN
#line 3181

	op = getAL();

#ifdef NTVDM
#line 3185
        if (op == 0xff)                 /* Special for MS */
        {

	    if (stream_io_enabled){
		count = getCX();
		avail = stream_io_buffer_size - *stream_io_dirty_count_ptr;
		ptr = effective_addr(getES(), getDI());
		if (count <= avail) {
		    sas_loads(ptr, stream_io_buffer + *stream_io_dirty_count_ptr, count);
		    *stream_io_dirty_count_ptr += count;
		}
		else {	/* buffer overflow */
		    if (*stream_io_dirty_count_ptr) {
			stream_io_update();
		    }
		    while (count) {
			if (count >= stream_io_buffer_size) {
			    sas_loads(ptr, stream_io_buffer, stream_io_buffer_size);
			    *stream_io_dirty_count_ptr = stream_io_buffer_size;
			    stream_io_update();
			    count -= stream_io_buffer_size;
			    ptr += stream_io_buffer_size;
			}
			else {
			    sas_loads(ptr, stream_io_buffer, count);
			    *stream_io_dirty_count_ptr = count;
			    break;
			}
		    }
		}

		setAL(1);
		return;
	    }

            if (sas_hw_at_no_check(vd_video_mode) < 4)  /* text mode */
            {
		ptr = effective_addr(getES(), getDI());
		/* sudeepb 28-Sep-1992 taken out for int10h/13ff fix */
		/* vd_set_cursor_position(); */	 /* set to start from DX */
                for(i = getCX(); i > 0; i--)
                {
                    setAL(sas_hw_at_no_check(ptr));
                    vd_write_teletype();
                    ptr++;
                }
                setAL(1);       /* success - string printed */
            }
            else
            {
                setAL(0);       /* failure */
            }
            return;
        }

#ifdef X86GFX
#line 3241
    else if (op == 0xfe) {
	    disable_stream_io();
	    return;
	}
#endif
#line 3246

#endif	/* NTVDM */
#line 3248

#ifdef JAPAN
#line 3250
	// DOS/V function support
	if ( op == 0x10 ) {
	    unsigned short *Offset;
	    int i;
	    unsigned long addr;

	    // DbgPrint( "\nNTVDM: INT 10 AH=13, AL=%02x\n", op );
	    addr = ( getES() << 4 ) + getBP();
	    col = getDL();
	    row = getDH();
	    len = getCX();

            // mode73h support
#ifdef i386
#line 3264
	    if ( !is_us_mode() && (sas_hw_at_no_check(DosvModePtr) == 0x73) )
                Offset = (unsigned short*)get_screen_ptr( row * get_bytes_per_line()*2 + (col * 4) );
            else
                Offset = (unsigned short*)get_screen_ptr( row * get_bytes_per_line() + (col * 2) );
#else // !i386
#line 3269
	    if( is_us_mode() )
	      return;          //In us mode, do nothing

	    Offset = get_screen_ptr( row * get_bytes_per_line()*2 + (col * 4) );
#endif  // !i386
#line 3274

	    //assert1( len >= 0, "vd_write_string len=%x\n", len );
	    //DbgPrint( "\nread_str len=%d ADDR:%04x ", len, addr );
	    for ( i = len; i > 0; i-- ) {
		sas_storew_no_check( addr, *Offset );
		//DbgPrint( " %04x", (*Offset) );
		addr += 2;
#ifdef i386
#line 3282
		if ( !is_us_mode() && (sas_hw_at_no_check(DosvModePtr) == 0x73) )
                    Offset += 2;
                else
		    Offset++;
#else // !i386
#line 3287
                Offset += 2;
#endif  // !i386
#line 3289
	    }
	    return;
	}
	else if ( op == 0x11 ) {
	    // copy from op == 0x10
	    unsigned short *Offset;
	    int i;
	    unsigned long addr;

#ifdef i386
#line 3299
	    if ( sas_hw_at_no_check(DosvModePtr) != 0x73 || is_us_mode() )
#else // !i386
#line 3301
            if ( is_us_mode() )
	        // In us mode, videomode 73h has no meaning.
#endif  // !i386     REAL DOS/V acts like this.
#line 3304
                return;

	    // DbgPrint( "\nNTVDM: INT 10 AH=13, AL=%02x\n", op );
	    addr = ( getES() << 4 ) + getBP();
	    col = getDL();
	    row = getDH();
	    len = getCX();

	    Offset = (unsigned short*) get_screen_ptr( row * (get_bytes_per_line()*2) + (col * 4) );
	    assert1( len >= 0, "vd_write_string len=%x\n", len );
	    //DbgPrint( "\nread_str len=%d ADDR:%04x ", len, addr );
	    for ( i = len; i > 0; i-- ) {
		sas_storew_no_check( addr, *Offset ); // char, attr1
		addr += 2;
		Offset++;
		sas_storew_no_check( addr, *Offset ); // attr2, attr3
		addr += 2;
		Offset++;
	    }
	    return;
	}
	else if ( op == 0x20 ) {
	    // copy from original routine, and delete ctl check
#ifndef i386
#line 3328
	    if( is_us_mode() )
	      return;        // In us mode, do nothing.

#endif // !i386
#line 3332
	    ptr = effective_addr( getES(), getBP() );
	    col = getDL();
	    row = getDH();
	    len = getCX();
            saveAX = getAX();
            saveBX = getBX();
	    saveDX = getDX();
	    vd_get_cursor_position();
	    save_col = getDL(); save_row = getDH();
	    setCX( 1 );
	    setDL( col ); setDH( row );
	    for( i = len; i > 0; i-- ) {
		vd_set_cursor_position();
		setAL( sas_hw_at_no_check( ptr++ ) );
		setBL( sas_hw_at_no_check( ptr++ ) );
		vd_write_char_attrib();

		if( ++col >= sas_w_at_no_check(VID_COLS) ) {
			if(++row > VD_ROWS_ON_SCREEN ) {
				//setAL( 0xa );
				//vd_write_teletype();
				row--;
			}
			col = 0;
		}
		setDL( col ); setDH( row );
	    }
	    // restore cursor position
	    setDL( save_col ); setDH( save_row );
	    vd_set_cursor_position();
            setCX( len );
	    setDX( saveDX );
            setAX( saveAX );
            setBX( saveBX );
	    return;
	}
	else if ( op == 0x21 ) {
	    // 5/27/1993 V-KazuyS
	    // copy from 0x20 routine

	    unsigned short *Offset;

#ifdef i386
#line 3375
	    if ( sas_hw_at_no_check(DosvModePtr) != 0x73 || is_us_mode() )
	        return;
#else // !i386
#line 3378
	    register sys_addr Vptr = DosvVramPtr;
	    if( is_us_mode() ) //In us mode, videomode 73h is meanless.
                return;
#endif // !i386
#line 3382

	    ptr = effective_addr( getES(), getBP() );
	    col = getDL();
	    row = getDH();
	    len = getCX();
            saveAX = getAX();
            saveBX = getBX();
	    saveDX = getDX();
	    Offset = (unsigned short*)get_screen_ptr( row * (get_bytes_per_line()*2) + (col * 4) );
#ifndef i386
#line 3392
	    // we now use DosvVram to hold extended attribute
	    Vptr += row * get_bytes_per_line() + (col *2);
#endif // !i386
#line 3395
	    vd_get_cursor_position();
	    save_col = getDL(); save_row = getDH();
	    setCX( 1 );
	    setDL( col ); setDH( row );
	    for( i = len; i > 0; i-- ) {
		vd_set_cursor_position();
		setAL( sas_hw_at_no_check( ptr++ ) );
		setBL( sas_hw_at_no_check( ptr++ ) );
		vd_write_char_attrib();

                // V-KazuyS copy ext. attrib
                Offset++;
                *Offset = sas_w_at_no_check( ptr );
#ifndef i386
#line 3409
		sas_move_bytes_forward(ptr, Vptr, 2);
		Vptr += 2;
#endif // !i386
#line 3412
                ptr += 2;                      // Attrib 2 byte
                Offset++;

		if( ++col >= sas_w_at_no_check(VID_COLS) ) {
			if(++row > VD_ROWS_ON_SCREEN ) {
				row--;
			}
			col = 0;
		}
		setDL( col ); setDH( row );
	    }
	    // restore cursor position
	    setDL( save_col ); setDH( save_row );
	    vd_set_cursor_position();
            setCX( len );
	    setDX( saveDX );
            setAX( saveAX );
            setBX( saveBX );
            return;
	}
#endif // JAPAN
#line 3433
	ptr =  effective_addr(getES(),getBP()) ;
	col = getDL();
	row = getDH();
	len = getCX();
	vd_get_cursor_position();
	save_col = getDL(); save_row = getDH();
	setCX(1);
	setDL(col); setDH(row);
	vd_set_cursor_position();
	for(i=len;i>0;i--)
	{
		ctl = sas_hw_at_no_check(ptr) == 7 || sas_hw_at_no_check(ptr) == 8 || sas_hw_at_no_check(ptr) == 0xa || sas_hw_at_no_check(ptr) == 0xd;
		setAL(sas_hw_at_no_check(ptr++));
		if(op > 1)setBL(sas_hw_at_no_check(ptr++));
		if(ctl)
		{
			vd_write_teletype();
			vd_get_cursor_position();
			col = getDL(); row = getDH();
			setCX(1);
		}
		else
		{
			vd_write_char_attrib();
			if(++col >= sas_w_at_no_check(VID_COLS))
			{

				if(++row > VD_ROWS_ON_SCREEN)
				{
					setAL(0xa);
					vd_write_teletype();
					row--;
				}
				col = 0;
			}
			setDL(col); setDH(row);
		}
		vd_set_cursor_position();
	}
	if(op==0 || op==2)
	{
		setDL(save_col); setDH(save_row);
		vd_set_cursor_position();
	}
}


GLOBAL void vd_get_mode IFN0()
{
    /*
     * Returns the current video mode.  Registers are set up viz:
     *
     * AL - Video mode
     * AH - Number of columns on screen
     * BH - Current display page
     */

    word vd_cols_on_screen;
	half_word	video_mode;

    /* Load internal variables with the values stored in BIOS
     * data area.
     */
    vd_cols_on_screen = sas_w_at_no_check(VID_COLS);
	video_mode = sas_hw_at_no_check(vd_video_mode);

    setAL(video_mode);
    setAH((UCHAR)vd_cols_on_screen);
    setBH(sas_hw_at_no_check(vd_current_page));
}


/*
 * ============================================================================
 * Internal functions
 * ============================================================================
 */

/*
 * function to return the (host) address stored at Intel address 'addr'
 * or 0 if not present
 */
LOCAL sys_addr extend_addr IFN1(sys_addr,addr)
{
	word	ext_seg, ext_off;	/* for segment & offset addrs */

	/* get vector */
	ext_off = sas_w_at_no_check(addr);
	ext_seg = sas_w_at_no_check(addr+2);
	/* if still defaults then no extended chars */
	if (ext_seg == EXTEND_CHAR_SEGMENT && ext_off == EXTEND_CHAR_OFFSET)
		return(0);	/* no user set char gen table */
	else
		return( effective_addr( ext_seg , ext_off ) );
}


/*
* routine to establish the foreground colour mask for the appropriate
* medium res. word forming part (1/8th) of the char.
* See vd_read_attrib_char() above.
*/
LOCAL half_word fgcolmask IFN1(word, rawchar)
{
    	register word mask, onoff = 0;

	mask = 0xC000;		/* compare with foreground colour */
	onoff = 0;
	do {
	    if ((rawchar & mask) == 0) /* not this bit, shift */
		onoff <<= 1;
	    else
		onoff = (onoff << 1) | 1;	/* set this bit */
	    mask >>= 2;
	} while(mask);		/* 8 times thru loop */
	return((half_word)onoff);
}


/*
* double all bits in lower byte of 'lobyte' into word.
* Have tried to speed this up using ffs() to only look at set bits but
* add overhead while calculating result shifts
*/
LOCAL word expand_byte IFN1(word, lobyte)
{
    register word mask = 1, res = 0;

    while(mask) {
	res |= lobyte & mask;	/* set res bit if masked bit in lobyte set*/
	lobyte <<= 1;
	mask <<= 1;
	res |= lobyte & mask;	/* and duplicate */
	mask <<= 1;		/* next bit */
    }
    return(res);
}


/*
* Routine to do 'how_many' char writes of 'wchar' with attribute 'attr' from
* position (x,y) in graphics mode
*/
GLOBAL void graphics_write_char IFN5(half_word, x, half_word, y, half_word, wchar, half_word, attr, word, how_many)
{
    register sys_addr	gpos;	/* gpos holds character address &...*/
    register sys_addr   cpos;	/*cpos steps through scanlines for char*/
    register word	j, colword,  colmask;
    register sys_addr	iopos, char_addr;
    register half_word	i, xor;
    half_word		current;

    /*
     * if the high bit of the attribute byte is set then xor the char
     * onto the display
     */
    xor = (attr & 0x80) ? 1 : 0;
    if (wchar >= 128)
    {   /* must be in user installed extended char set */
        if ( (char_addr = extend_addr(4*BIOS_EXTEND_CHAR)) == 0)
        {
#ifndef PROD
#line 3595
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 3597
            return;
        }
        else
            char_addr += (wchar - 128) * CHAR_MAP_SIZE;
    }
#ifdef EGG
#line 3603
    else if(video_adapter == EGA || video_adapter == VGA)
	char_addr = extend_addr(EGA_FONT_INT*4)+ CHAR_MAP_SIZE *wchar;
#endif
#line 3606
    else
        char_addr = CHAR_GEN_ADDR+ CHAR_MAP_SIZE *wchar;	/* point to entry in std set */

    if (sas_hw_at_no_check(vd_video_mode) == 6) {			/* high res */

	gpos = vd_high_offset(x, y);	/* sys & host memory offsets */
	gpos += video_pc_low_regen;

        for(j = 0; j < how_many; j++) {		/* number of chars to store */
	    cpos = gpos++;			/* start of this character */
	    for(i = 0; i < 4; i++) {		/* 8 bytes per char */
		if (xor) {		/* XOR in char */
		    sas_load(cpos, &current);	/* even bank */
		    sas_store(cpos, (IU8)(current ^ sas_hw_at_no_check(char_addr + i*2)));
		    sas_load(cpos+ODD_OFF, &current);
		    current ^= sas_hw_at_no_check(char_addr + i*2+1);
		}
		else {				/* just store new char */
		    sas_store(cpos, sas_hw_at_no_check(char_addr + i*2));
		    current = sas_hw_at_no_check(char_addr + i*2+1);
		}
		sas_store(cpos+ODD_OFF, current);	/* odd bank */
		cpos += SCAN_LINE_LENGTH;			/* next scan line */
	    }
	}
    }

    else {				/* medium res */

	gpos = vd_medium_offset(x, y);	/* sys & host memory offsets */
	gpos += video_pc_low_regen;

	/* build colour mask from attribute byte */
	attr &= 3;			/* only interested in low bits */
	colmask = attr;			/* replicate low bits across word */
	for(i = 0; i < 3; i++)
	    colmask = (colmask << 2) | attr;
	colmask = (colmask << 8) | colmask;

	for(j = 0; j < how_many; j++) {
	    cpos = gpos;
	    gpos += 2;
	    for(i = 0; i < 8; i++) {		/* 16 bytes per char */

		if ((i & 1) == 0)		/* setup for odd/even bank */
		    iopos = cpos;
		else {
		    iopos = cpos+ODD_OFF;
		    cpos += SCAN_LINE_LENGTH;	/* next scan line */
		}

		colword = expand_byte(sas_hw_at_no_check(char_addr + i));  /*char in fg colour*/
		colword &= colmask;
		if (xor) {			          /* XOR in char */
		    sas_load(iopos, &current);
		    sas_store(iopos++, (IU8)(current ^ (colword >> 8)));
		    sas_load(iopos, &current);
		    sas_store(iopos, (IU8)(current ^ (colword & 0xFF)));
		}
		else {					  /* just store char */
		    sas_store(iopos++, (IU8)((colword >> 8)));
		    sas_store(iopos, (IU8)((colword & 0xFF)));
		}
	    }
	}
	how_many *= 2;
    }
}


/*
 * Initialise the M6845 registers for the given mode.
 */

LOCAL void M6845_reg_init IFN2(half_word, mode, word, base)
{
    UCHAR i, table_index;

    switch(mode)
    {
    case 0:
    case 1:  table_index = 0;
	     break;
    case 2:
    case 3:  table_index = NO_OF_M6845_REGISTERS;
	     break;
    case 4:
    case 5:
    case 6:  table_index = NO_OF_M6845_REGISTERS * 2;
	     break;
    default: table_index = NO_OF_M6845_REGISTERS * 3;
	     break;
    }

    for (i = 0; i < NO_OF_M6845_REGISTERS; i++)
    {
	/*
	 * Select the register in question via the index register (== base)
	 * and then output the actual value.
	 */

	outb(base, i);
	outb((IU16)(base + 1), (IU8)(sas_hw_at_no_check(VID_PARMS+table_index + i)));
    }
}

LOCAL void vd_dummy IFN0()
{
}

#ifdef REAL_VGA
#line 3717
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#line 3742
#endif
#line 3743

#ifdef SEGMENTATION
#line 3745
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 3752

GLOBAL void video_init IFN0()
{
    UCHAR mode;
    word vd_addr_6845;
    word curmod;
#ifdef HERC
#line 3759
#error DIVERGENCE MVDM-HOST-DIV-522: HERC platform branch is not selected; see mirror register.
#endif /* HERC */
#line 3761


    /*
     * Initialise BIOS data area variables
     */

    curmod = 0x607;	/* default cursor is scans 6-7 */

    switch (video_adapter)
    {
	case MDA:
        	mode = 0x7;
        	vd_addr_6845  = 0x3B4;
		video_pc_low_regen = MDA_REGEN_START;
		video_pc_high_regen = MDA_REGEN_END;
		break;
#ifdef HERC
#line 3778
#error DIVERGENCE MVDM-HOST-DIV-522: HERC platform branch is not selected; see mirror register.
#line 3790
#endif /* HERC */
#line 3791
#ifdef EGG
#line 3792
    	case EGA:
    	case VGA:
        	mode = 0x3;
        	vd_addr_6845  = 0x3D4;
    		sas_storew_no_check(VID_INDEX, vd_addr_6845);
		sure_sub_note_trace0(CURSOR_VERBOSE,"setting bios vbls start=6, end=7");
    		sas_storew_no_check(VID_CURMOD, 0x607);
    		setAL(mode);
		ega_video_init();
		return;
		break;
#endif
#line 3804
	default:	/* Presumably CGA */
		video_pc_low_regen = CGA_REGEN_START;
		video_pc_high_regen = CGA_REGEN_END;
        	mode = 0x3;
        	vd_addr_6845  = 0x3D4;
    }

    sas_storew_no_check(VID_INDEX, vd_addr_6845);
    sure_sub_note_trace2(CURSOR_VERBOSE,"setting bios vbls start=%d, end=%d",
	(curmod>>8)&0xff, curmod&0xff);
    sas_storew_no_check(VID_CURMOD, curmod);

    /* Call vd_set_mode() to set up 6845 chip */
    setAL(mode);
    (video_func[SET_MODE])();
}

#ifdef HERC
#line 3822
#error DIVERGENCE MVDM-HOST-DIV-522: HERC platform branch is not selected; see mirror register.
#line 3938
#endif /* HERC */
#line 3939

#ifdef NTVDM
#line 3941
void enable_stream_io(void)
{
#ifdef MONITOR
#line 3944
/* for non RISC machine the buffer is from 16bits code bop from spckbd.asm */
    host_enable_stream_io();
    stream_io_enabled = TRUE;
#else
#line 3948
#error Excluded unsupported original non-MONITOR stream buffer allocation
#line 3956
#endif
#line 3957

}

void disable_stream_io(void)
{

    stream_io_update();
    stream_io_enabled = FALSE;
    host_disable_stream_io();
#ifndef MONITOR
#line 3967
#error Excluded unsupported original non-MONITOR stream buffer release
#endif
#line 3969
}
#endif
#line 3971

#if defined(JAPAN) || defined(KOREA)
#line 3973

//;;;;;;;;;;;;;;;;;;; MS-DOS/V BOP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef i386
#line 3977
#define CONSOLE_BUFSIZE (80*50*2*2)
GLOBAL byte FromConsoleOutput[CONSOLE_BUFSIZE];

GLOBAL int FromConsoleOutputFlag=FALSE;

GLOBAL byte SaveDosvVram[DOSV_VRAM_SIZE];
#endif // i386
#line 3984

// use GetStringBitmap()
#define BITMAPBUFSIZ 128
extern HDC hdcVDM;
extern HFONT hFont24;
extern HFONT hFont16;

extern BOOL VDMForWOW;

// For GetStringBitmap()
typedef struct tagSTRINGBITMAP
{
    UINT uiWidth;
    UINT uiHeight;
    BYTE ajBits[1];
} STRINGBITMAP, *LPSTRINGBITMAP;

typedef struct
{
    BYTE ajBits[19];
} FONTBITMAP8x19;

typedef struct
{
    BYTE ajBits[16];
} FONTBITMAP8x16;

typedef struct
{
    BYTE ajBits[48];
} FONTBITMAP12x24;

typedef struct
{
    BYTE ajBits[32];
} FONTBITMAP16x16;

typedef struct
{
    BYTE ajBits[72];
} FONTBITMAP24x24;

typedef struct
{
    BYTE is_used[189];
    FONTBITMAP16x16 font16x16[189];
} FONT16CACHE, *PFONT16CACHE;

typedef struct
{
    BYTE is_used[189];
    FONTBITMAP24x24 font24x24[189];
} FONT24CACHE, *PFONT24CACHE;

FONTBITMAP8x19  font8x19[256];
FONTBITMAP8x16  font8x16[256];
FONTBITMAP12x24  font12x24[256];

BYTE            font16table[256];
HLOCAL          hFont16mem[128];
HLOCAL          hFont24mem[128];

#define USED (1)
#define NOT_USED (0)

void GetVerticallineFlag( int *VFlag );

UINT
GetStringBitmapA(
    HDC             hdc,
    LPSTR           pc,
    UINT            cch,
    UINT            cbData,
    LPSTRINGBITMAP  pSB
);

void LoadBitmapFont()
{
    char code[3];
    static char sb[BITMAPBUFSIZ];
    LPSTRINGBITMAP psb;
    int i, j;
    int n;
    int VFlag[256];

    // first 8x16, 8x19 font
#ifdef JAPAN_DBG
#line 4071
    DbgPrint( "NTVDM:Loading NTFONT 8x16,8x19\n" );
#endif
#line 4073
    GetVerticallineFlag( VFlag );
    SelectObject( hdcVDM, hFont16 );
    psb = (LPSTRINGBITMAP)sb;
    code[1] = '\0';
    for ( i = 0; i < 256; i++ ) {
        code[0] = (char)i;

        GetStringBitmapA( hdcVDM, code, 1, BITMAPBUFSIZ, psb );

        RtlCopyMemory( &(font8x16[i].ajBits[0]), &(psb->ajBits[0]), 16 );

        for ( j = 0; j < (19-18) * 1; j++ ) {
            if ( VFlag[i] )
                font8x19[i].ajBits[0] = psb->ajBits[0];
            else
                font8x19[i].ajBits[0] = 0x00;
        }
        RtlCopyMemory( &(font8x19[i].ajBits[1]), &(psb->ajBits[0]), 18 );
    }

    // second 12x24 font
#ifdef JAPAN_DBG
#line 4095
    DbgPrint( "NTVDM:Loading NTFONT 12x24\n" );
#endif
#line 4097
    SelectObject( hdcVDM, hFont24 );
    psb = (LPSTRINGBITMAP)sb;
    code[1] = '\0';
    for ( i = 0; i < 256; i++ ) {
        code[0] = (char)i;
        GetStringBitmapA( hdcVDM, code, 1, BITMAPBUFSIZ, psb );

        // 12x24 dot font is console 12x27 font.
        RtlCopyMemory( &(font12x24[i].ajBits[0]), &(psb->ajBits[2]), 48 );
    }



    // make table
    for ( i = 0, n = 0; i < 256; i++ ) {  // Leading byte
        if ( is_dbcs_first(i) ) {
            font16table[i] = n++;
        }
    }
#ifdef JAPAN_DBG
#line 4117
    DbgPrint( "NTVDM:Loading font... end\n" );
#endif
#line 4119
}

// This is only stub routine
// finally, this information gets from registry
void GetVerticallineFlag( int *VFlag )
{
    int i;

    for ( i = 0; i < 256; i++ ) {
        VFlag[i] = FALSE;
    }
    VFlag[0x03] = TRUE;
    VFlag[0x04] = TRUE;
    VFlag[0x05] = TRUE;
    VFlag[0x10] = TRUE;
    VFlag[0x15] = TRUE;
    VFlag[0x17] = TRUE;
    VFlag[0x19] = TRUE;
    VFlag[0x1d] = TRUE;

    return;
}



void GetBitmap()
{
    sys_addr ptr;
    int i;
    int width, height;
    char code[3];
    static char sb[BITMAPBUFSIZ];
    LPSTRINGBITMAP psb;
    int index;
    PFONT16CACHE pCache16;
    PFONT24CACHE pCache24;

#ifdef JAPAN_DBG
#line 4157
    DbgPrint( "NTFONT BOP 02\n" );
    DbgPrint( "ES:SI=%x:%x\n", getES(), getSI() );
    DbgPrint( "BH,BL=%x,%x\n", getBH(), getBL() );
    DbgPrint( "CH,CL=%x,%x\n", getCH(), getCL() );
#endif
#line 4162

    width = getBH();
    height = getBL();
    ptr =  effective_addr(getES(),getSI());

    if ( getCH() == 0 ) {
        if ( ( width == 8 ) && ( height == 16 ) ) {
	    sas_stores_from_transbuf(ptr,
				     (host_addr)&(font8x16[getCL()].ajBits[0]),
				     (sys_addr)16 );
            setAX(0);
        }
        else if ( ( width == 8 ) && ( height == 19 ) ) {
	    sas_stores_from_transbuf(ptr,
				     (host_addr)&(font8x19[getCL()].ajBits[0]),
				     (sys_addr)19 );
            setAX(0);
        }
        else if ( ( width == 12 ) && ( height == 24 ) ) {
	    sas_stores_from_transbuf(ptr,
				     (host_addr)&(font12x24[getCL()].ajBits[0]),
				     (sys_addr)48 );
            setAX(0);
        }
        else {
            DbgPrint( "Illegal Fontsize %xh, %xh\n", getBH(), getBL() );
            setAH(1);
        }
    }
    else {
        if ( !is_dbcs_first( getCH() ) ) {
            setAH( 5 );
            return;
        }
        if ( getCL() < 0x40 || getCL() > 0xfc || getCL() == 0x7f ) {
            setAH( 5 );
            return;
        }
        if (width == 16 && height == 16) {
            index = font16table[getCH()];

            if (!hFont16mem[index])
                hFont16mem[index] = LocalAlloc(LHND, sizeof(FONT16CACHE));

            pCache16 = LocalLock( hFont16mem[index] );
            if (pCache16->is_used[getCL()-0x40] != USED) {
                 code[0] = getCH();
                 code[1] = getCL();
                 code[2] = '\0';
                 psb = (LPSTRINGBITMAP)sb;
                 SelectObject( hdcVDM, hFont16 );
                 GetStringBitmapA(hdcVDM, code, 2, BITMAPBUFSIZ, psb);

                 RtlCopyMemory(&(pCache16->font16x16[getCL()-0x40].ajBits[0]),
                               &(psb->ajBits[0]),
                               32);
                 pCache16->is_used[getCL()-0x40] = USED;
            }
            sas_stores_from_transbuf(ptr,
                   (host_addr)&(pCache16->font16x16[getCL()-0x40].ajBits[0]),
                                     (sys_addr)32);
            LocalUnlock( hFont16mem[index] );

            setAX(0);
        }
        else if (width == 24 && height == 24) {
            index = font16table[getCH()];
            if (!hFont24mem[index])
                hFont24mem[index] = LocalAlloc(LHND, sizeof(FONT24CACHE));

            pCache24 = LocalLock( hFont24mem[index] );
            if (pCache24->is_used[getCL()-0x40] != USED) {
                code[0] = getCH();
                code[1] = getCL();
                code[2] = '\0';
                psb = (LPSTRINGBITMAP)sb;
                SelectObject( hdcVDM, hFont24 );
                GetStringBitmapA( hdcVDM, code, 2, BITMAPBUFSIZ, psb );

                RtlCopyMemory(&(pCache24->font24x24[getCL()-0x40].ajBits[0]),
                              &(psb->ajBits[0]),
                              72 );
                pCache24->is_used[getCL()-0x40] = USED;
            }
            sas_stores_from_transbuf(ptr,
                     (host_addr)&(pCache24->font24x24[getCL()-0x40].ajBits[0]),
				     (sys_addr)72 );
            LocalUnlock( hFont24mem[index] );

            setAX(0);
        }
        else {
            DbgPrint("Illegal Fontsize %xh, %xh\n", getBH(), getBL());
            setAH(1);
        }
    } // bouble byte case
}

// SetBitmap() save the font image to cache,
// and call SetConsoleLocalEUDC() to display in windowed.
void SetBitmap()
{
    sys_addr ptr;
    int i;
    SHORT width, height;
    int index;
    PFONT16CACHE pCache16;
    PFONT24CACHE pCache24;
    COORD cFontSize;

#ifdef JAPAN_DBG
#line 4273
    DbgPrint( "NTFONT BOP 03\n" );
    DbgPrint( "ES:SI=%x:%x\n", getES(), getSI() );
    DbgPrint( "BH,BL=%x,%x\n", getBH(), getBL() );
    DbgPrint( "CH,CL=%x,%x\n", getCH(), getCL() );
#endif
#line 4278

    width = getBH();
    height = getBL();
    ptr =  effective_addr(getES(),getSI());

    if ( getCH() == 0 ) {
        if ( ( width == 8 ) && ( height == 16 ) ) {
	    sas_loads_to_transbuf(ptr,
				  (host_addr)&(font8x16[getCL()].ajBits[0]),
				  (sys_addr)16 );
            setAL(0);
        }
        else if ( ( width == 8 ) && ( height == 19 ) ) {
	    sas_loads_to_transbuf(ptr,
				  (host_addr)&(font8x19[getCL()].ajBits[0]),
				  (sys_addr)19 );
            setAL(0);
        }
        else if ( ( width == 12 ) && ( height == 24 ) ) {
	    sas_loads_to_transbuf(ptr,
		(host_addr)&(font12x24[getCL()].ajBits[0]),
	        (sys_addr)48 );
            setAL(0);
        }
        else {
            DbgPrint( "Illegal Fontsize %xh, %xh\n", getBH(), getBL() );
            setAL(1);
        }
    }
    else {
        if ( !is_dbcs_first( getCH() ) ) {
            setAL( 5 );
            return;
        }
        if ( getCL() < 0x40 || getCL() > 0xfc || getCL() == 0x7f ) {
            setAL( 5 );
            return;
        }
        if ((width == 16) && (height == 16)) {
            index = font16table[getCH()];
            if (!hFont16mem[index])
                hFont16mem[index] = LocalAlloc(LHND, sizeof(FONT16CACHE));

            pCache16 = LocalLock(hFont16mem[index]);
            sas_loads_to_transbuf(ptr,
                    (host_addr)&(pCache16->font16x16[getCL()-0x40].ajBits[0]),
		    (sys_addr)32);
            pCache16->is_used[getCL()-0x40] = USED;
            LocalUnlock(hFont16mem[index]);
            cFontSize.X = width;
            cFontSize.Y = height;

            if (!SetConsoleLocalEUDC(sc.OutputHandle,
                                     getCX(),
                                     cFontSize,
                      (PCHAR)(pCache16->font16x16[getCL()-0x40].ajBits)))
                DbgPrint("NTVDM: SetConsoleEUDC() Error. CodePoint=%04x\n",
                         getCX());
            setAL(0);
        }
        else if ((width == 24) && (height == 24)) {
            index = font16table[getCH()];
            if (!hFont24mem[index])
                hFont24mem[index] = LocalAlloc(LHND, sizeof(FONT24CACHE));

            pCache24 = LocalLock(hFont24mem[index]);
            sas_loads_to_transbuf(ptr,
                    (host_addr)&(pCache24->font24x24[getCL()-0x40].ajBits[0]),
                    (sys_addr)72 );
            pCache24->is_used[getCL()-0x40] = USED;
            LocalUnlock( hFont24mem[index] );
            cFontSize.X = width;
            cFontSize.Y = height;

            if (!SetConsoleLocalEUDC(sc.OutputHandle,
                                     getCX(),
                                     cFontSize,
                       (PCHAR)(pCache24->font24x24[getCL()-0x40].ajBits)))
                DbgPrint("NTVDM: SetConsoleEUDC() Error. CodePoint=%04x\n",
                         getCX() );
            setAL(0);
        }
        else {
            DbgPrint("Illegal Fontsize %xh, %xh\n", getBH(), getBL());
            setAL(1);
        }
    }
}

// ntraid:mskkbug#3167: works2.5: character corrupted -yasuho
// generate single byte charset
void GenerateBitmap()
{
	sys_addr	ptr;
	int		size, nchars, offset;
	char		mode;

	mode = sas_hw_at_no_check(DosvModePtr);
	if (is_us_mode() || (mode != 0x03 && mode != 0x73))
		return;
	ptr =  effective_addr(getES(), getBP());
	size = getBH();
	nchars = getCX();
	offset = getDX();
	if (nchars + offset > 0x100) {
		setCF(1);
		return;
	}
        if (size == 16) {
                sas_loads_to_transbuf(ptr,
				      (host_addr)&(font8x16[offset].ajBits[0]),
				      (sys_addr)(nchars * size));
		setCF(0);
        } else if (size == 19) {
                sas_loads_to_transbuf(ptr,
				      (host_addr)&(font8x19[offset].ajBits[0]),
				      (sys_addr)(nchars * size));
		setCF(0);
        } else if (size == 24) {
                sas_loads_to_transbuf(ptr,
				      (host_addr)&(font12x24[offset].ajBits[0]),
				      (sys_addr)(nchars * size));
		setCF(0);
        } else {
		DbgPrint("Illegal Fontsize %xh\n", size);
		setCF(1);
        }
}


/*
 * MS_DosV_bop()
 *
 * The type of operation is coded into the AH register.
 *
 *  AH = 00 - ff 	for $NTFONT.SYS
 *     = 10		DBCS vector adress(DS:SI)
 *     = 11 - 1f	reserved
 *     = 20		Window information packet adress(DS:SI)
 *     = 21		Text Vram save & restore
 *     = 22		palette and DAC registers operations
 *     = 23		monitoring IME status lines
 *     = 24 - ff     reserved
 *
 */
#if defined(JAPAN)
#line 4424
void MS_DosV_bop IFN0()
#else // JAPAN
#line 4426
void MS_HDos_bop IFN0()
#endif // KOREA
#line 4428
{
    int op;

    op = getAH();
    switch ( op ) {
	case 0x00:

// inquery font type
//
// input
//   ES:SI pointer of buffer
//   CX    buffersize
// output
//   CX    number of font element
//   AL    if CX < N(number of element) then 1
//   ES:SI --> x0, y0(byte)  -- font size
//             x1, y1
//             ......
//             x(N-1), y(N-1)

	    {
		sys_addr ptr;
                int bufsize;
                int N;
                int i;
#ifdef JAPAN_DBG
#line 4454
                DbgPrint( "NTFONT BOP 00\n" );
                DbgPrint( "ES:SI=%x:%x\n", getES(), getSI() );
                DbgPrint( "CX=%x\n", getCX() );
#endif
#line 4458
                if ( VDMForWOW ) {
                    setCF(1);
                    return;
                }
		ptr =  effective_addr(getES(),getSI());
                bufsize = getCX();

                // now only 8x16, 8,19, 16x16 font -- July 13 V-KazuyS
                N = 5;

                if ( N > bufsize ) {
                    N = bufsize;
                    setAL(1);
                }
                else {
                    setAL(0);
                }
                setCX((IU16)N);

                sas_store_no_check( ptr++,  8 );  // x
                sas_store_no_check( ptr++, 16 );  // y
                sas_store_no_check( ptr++,  8 );  // x
                sas_store_no_check( ptr++, 19 );  // y
                sas_store_no_check( ptr++, 12 );  // x
                sas_store_no_check( ptr++, 24 );  // y
                sas_store_no_check( ptr++, 16 );  // x
                sas_store_no_check( ptr++, 16 );  // y
                sas_store_no_check( ptr++, 24 );  // x
                sas_store_no_check( ptr++, 24 );  // y
            }
            break;

	case 0x01:
	    // Load font image from Gre
#ifdef JAPAN_DBG
#line 4493
            DbgPrint( "NTFONT BOP 01\n" );
#endif
#line 4495
            if ( VDMForWOW ) {
                setCF(1);
                return;
            }
            LoadBitmapFont();
            break;

	case 0x02:
	    // Read font image
            if ( VDMForWOW ) {
                setCF(1);
                return;
            }
            GetBitmap();
            break;

	case 0x03:
#ifdef JAPAN_DBG
#line 4513
            //DbgPrint( "NTFONT BOP 03 CH:CL=%02x:%02x\n", getCH(), getCL() );
#endif
#line 4515
            if ( VDMForWOW ) {
                setCF(1);
                return;
            }
            SetBitmap();
            break;


	case 0x10:
#ifdef JAPAN_DBG
#line 4525
	    DbgPrint( "NTVDM: DBCS vector address=%04x:%04x\n", getDS(), getSI() );
#endif
#line 4527
	    {
		// NOTE: This routine is called ONLY ONE from $NtDisp1.sys !
		sys_addr ptr;
		word vector;
		extern UINT ConsoleInputCP;
		extern UINT ConsoleOutputCP;

   	    	DBCSVectorAddr =  effective_addr(getDS(),getSI());
	    	// Save DBCS vector
		DBCSVectorLen = 0;
	   	for ( ptr = DBCSVectorAddr;
			vector = sas_w_at_no_check( ptr ); ptr += 2 ) {
	            SaveDBCSVector[DBCSVectorLen] = vector;
	            DBCSVectorLen++;
	        }
                assert0( ConsoleInputCP == ConsoleOutputCP, "InputCP != OutputCP" );

	        BOPFromNtDisp1Flag = TRUE;

#ifdef JAPAN_DBG
#line 4547
                DbgPrint( "BOP from $NTDISP1\n" );
#endif
#line 4549
	    }
	    break;

	case 0x20:
#ifdef JAPAN_DBG
#line 4554
	    DbgPrint( "NTVDM: Information packet address=%04x:%04x\n", getDS(), getSI() );
#endif
#line 4556
	    /* information packet is
	     *
	     *   offset	bytes	mode	information
	     *   +0x0	2       r	packet length
	     *   +0x2   4   	r	virtual text VRAM address seg:off
	     *   +0x6   4   	r	MS-DOS/V display mode address seg:off
	     *   +0xa   4	w	Windowed or Fullscreen flag address seg:	     *   +0xe   4	r	NT console mode flag address seg:off
	     *                          when VDM terminate, this flag == 1
	     *   +0x12  4	r	Switch to FullScreen subroutine address seg:	     */
	    {
		sys_addr ptr;
		extern UINT ConsoleInputCP;
		extern UINT ConsoleOutputCP;
#ifdef i386
#line 4570
		extern word useHostInt10;
		extern word int10_seg;
#endif
#line 4573

		ptr =  effective_addr(getDS(),getSI());

                DosvVramOff = sas_w_at_no_check( ptr+0x02 );
                DosvVramSeg = sas_w_at_no_check( ptr+0x04 );
                DosvVramPtr = effective_addr( DosvVramSeg, DosvVramOff );

		// disp_win.sys don't know VramSize.
		// because $disp.sys doesn't have get_vram_size function, now.
                DosvVramSize = DOSV_VRAM_SIZE;
#ifdef JAPAN_DBG
#line 4584
                DbgPrint( "NTVDM:DosvVirtualTextVRAM addr = %04x:%04x\n", DosvVramSeg, DosvVramOff );
#endif
#line 4586

                DosvModeOff = sas_w_at_no_check( ptr+0x06 );
                DosvModeSeg = sas_w_at_no_check( ptr+0x08 );
                DosvModePtr = effective_addr( DosvModeSeg, DosvModeOff );
#ifdef JAPAN_DBG
#line 4591
                DbgPrint( "NTVDM:DosvVideoMode addr = %04x:%04x\n", DosvModeSeg, DosvModeOff );
#endif
#line 4593

#ifndef i386	// !!! remeber to change DISP_WIN.SYS since it relies on this.
#line 4595
		sas_storew_no_check( ptr+0x0a, 0 );
		sas_storew_no_check( ptr+0x0c, 0 );
#else
#line 4598
	        sas_storew_no_check( ptr+0x0a, useHostInt10 );
	        sas_storew_no_check( ptr+0x0c, int10_seg );
#endif
#line 4601


                NtConsoleFlagOff = sas_w_at_no_check( ptr+0x0e );
                NtConsoleFlagSeg = sas_w_at_no_check( ptr+0x10 );
                NtConsoleFlagPtr = effective_addr( NtConsoleFlagSeg, NtConsoleFlagOff );
#ifdef JAPAN_DBG
#line 4607
                DbgPrint( "NTVDM:ConsoleFlagAddr = %04x:%04x\n", NtConsoleFlagSeg, NtConsoleFlagOff );
#endif
#line 4609

#ifdef i386
#line 4611
// RISC never use following value.
// Swtich to fullscreen, call these address..
                DispInitOff = sas_w_at_no_check( ptr+0x12 );
                DispInitSeg = sas_w_at_no_check( ptr+0x14 );
#ifdef JAPAN_DBG
#line 4616
                DbgPrint( "NTVDM:Disp Init Addr = %04x:%04x\n", DispInitSeg, DispInitOff );
#endif
#line 4618

		FullScreenResumeOff = sas_w_at_no_check(ptr + 0x16);
		FullScreenResumeSeg = sas_w_at_no_check(ptr + 0x18);
#endif // !i386
#line 4622
                SetModeFlagPtr = effective_addr(
                                       sas_w_at_no_check( ptr+0x1c ),
                                       sas_w_at_no_check( ptr+0x1a ));
		if ( BOPFromNtDisp1Flag )
		    BOPFromDispFlag = TRUE;

                SetDBCSVector( ConsoleInputCP );
                PrevCP = ConsoleOutputCP;

#ifdef JAPAN_DBG
#line 4632
                DbgPrint( "BOP from $NTDISP2\n" );
#endif
#line 4634
	    }
	    break;

#ifdef i386
#line 4638
        case 0x21:
		// Vram save restore function
		// AL == 00 save
		// AL == 01 restore
		// AL == 02 GetConsoleBuffer
		// AL == 03 Get FromConsoleOutput
            {
		sys_addr ptr;
                int op;
                int count;
                int i;

                op = getAL();
		ptr =  effective_addr(getES(),getDI());
                count = getCX();

                if ( op == 0x02 ) {
                    if ( count > DOSV_VRAM_SIZE )
                        count = DOSV_VRAM_SIZE;
                    //for ( i = 0; i < 2; i++ ) {
                        if ( FromConsoleOutputFlag ) {
#ifdef JAPAN_DBG
#line 4660
                            DbgPrint( "NTVDM: MS-DOS/V BOP 21\n" );
#endif
#line 4662
#ifdef i386
#line 4663
//FromConsoleOutput is on 32bit address space not on DOS address space.
			    sas_move_bytes_forward((sys_addr) FromConsoleOutput,
						   (sys_addr) ptr,
						   (sys_addr) count
						  );
#else
#line 4669
			    sas_stores_from_transbuf(ptr,
					       (host_addr) FromConsoleOutput,
					       (sys_addr) count
						  );
#endif
#line 4674
                            FromConsoleOutputFlag = FALSE;
                            break;
                        }
                        else {
                            DbgPrint( "NTVDM: MS-DOS/V BOP 21 can't get console screen data!! \n" );
                            //Sleep( 1000L );
                        }
                    //}
                    break;
                }

                if ( count > DOSV_VRAM_SIZE )
                    count = DOSV_VRAM_SIZE;

                if ( op == 0x00 ) {       // save function
#ifdef JAPAN_DBG
#line 4690
                    DbgPrint( "NTVDM:MS_DOSV_BOP 0x21, %02x %04x:%04x(%04x)\n", op, getES(), getDI(), count );
#endif
#line 4692
//SaveDosvVram is on 32bit address space not on DOS address space.
		    sas_loads_to_transbuf(ptr,
					  (host_addr)SaveDosvVram,
					  (sys_addr)count
					  );

                }
                else if ( op == 0x01 ) {  // restore function
#ifdef JAPAN_DBG
#line 4701
                    DbgPrint( "NTVDM:MS_DOSV_BOP 0x21, %02x %04x:%04x(%04x)\n", op, getES(), getDI(), count );
#endif
#line 4703
// DEC-J comment
// SaveDosvVram is on 32bit address space not on DOS address space.
// for C7 PWB.
// This is internal bop
// It doesn't need check memory type.
		    RtlCopyMemory( (void*)ptr,
				   (void*)SaveDosvVram,
				   (unsigned long)count
				 );
                }
		// #3086: VDM crash when exit 16bit apps of video mode 11h
		// 12/2/93 yasuho
		else if ( op == 0x03 ) { // Get from FromConsoleOutput
		    RtlCopyMemory( (void*)ptr,
				   (void*)FromConsoleOutput,
					     (unsigned long) count
					     );
                }
            }
            break;
#endif // i386
#line 4724

	case 0x22:
	    // #3176: vz display white letter on white screen
	    // 12/1/93 yasuho (reviewed by williamh)
	    // palette and DAC registers operations
	    // Input	AH = 22H
	    //		AL = 00H : get from simulated value for windowed
	    //		ES:DI = ptr to palette/DAC buffer
	    {
		sys_addr	ptr;
		byte		op;
		static void	get_cur_pal_and_DAC(sys_addr);

		op = getAL();
		ptr = effective_addr(getES(), getDI());
#ifdef	VGG
#line 4740
		if (op == 0x00) { // get from simulated value for windowed
			get_cur_pal_and_DAC(ptr);
		}
#endif	//VGG
#line 4744
	    }
	    break;

#if !defined(KOREA)
#line 4748
	case 0x23:
	    // #4183: status line of oakv(DOS/V FEP) doesn't disappear
	    // 12/11/93 yasuho
	    // monitoring IME status lines
	    // Input	AH = 23H
	    //		AL = number of IME status lines
	    {
		IMEStatusLines = getAL();
	    }
	    break;
#endif
#line 4759

        // kksuzuka #6168 screen attributes for DOS fullscreen
	case 0x24:
	    // set console attributes for fullscreen
            // Input	AH = 23H
	    //		AL = none
            // Output	AL 4-7 bit = back ground color
	    //		AL 0-3 bit = fore ground color
	    {
		setAX(textAttr);
	    }
	    break;

#if !defined(KOREA)
#line 4773
        case 0xff:
            // Int10 Function FF
            // ES: Vram seg, DI: Vram Off, CX:counter
            {
                register int i;
                register int vram_addr;
                int DBCSState = FALSE;
                register  char *p;
#ifndef i386
#line 4782
	        register sys_addr V_vram = effective_addr(getES(),getDI());
#endif // !i386
#line 4784

                if ( is_us_mode() ) {
                    setCF(1);
                    return;
                }
		if ( sas_hw_at_no_check(DosvModePtr) != 0x03 ) {
#ifdef JAPAN_DBG
#line 4791
                    DbgPrint( "NTVDM: mode != 0x03, int10 FF not support\n" );
#endif
#line 4793
                    setCF(1);
                    return;
                }
#if 0
#line 4797
                DbgPrint( "Addr %04x:%04x, CX=%x, ", getES(), getDI(), getCX() );
                DbgPrint( "%d, %d, CX=%d\n", getDI() < 160 ? 0 : getDI()/160,
                      getDI() < 160 ? 0 : (getDI() - (getDI()/160)*160)/2, getCX() );
#endif
#line 4801
                Int10FlagCnt++;
                vram_addr = getDI() >> 1;
#ifdef i386
#line 4804
                p = get_screen_ptr( getDI() );
#else // !i386 uses the EGA plane.
#line 4806
                p = get_screen_ptr( getDI()<<1 );

		// for speed up!!
                i = getCX();
                if( vram_addr + i > DOSV_VRAM_SIZE / 2 ){
                    i = DOSV_VRAM_SIZE / 2 - vram_addr;
#ifdef JAPAN_DBG
#line 4813
                    DbgPrint("NTVDM:Int10 FF over VRAM(DI)=%04x\n", getDI() );
#endif
#line 4815
                }
#endif // !i386
#line 4817

#ifdef i386
#line 4819
                for ( i = 0; i < getCX(); i++ ) {
                        if ( vram_addr >= DOSV_VRAM_SIZE/2 ) {
                            DbgPrint("NTVDM:Int10 FF over VRAM(DI)=%04x\n", getDI() );
                            break;
                        }
                        else if ( DBCSState ) {
#else // !i386
#line 4826
		//for speed up!!
                while( i-- ) {
		  sas_loadw(V_vram++, (word *)p);
		  V_vram++;
		  setVideodirty_total(getVideodirty_total() + 2);
                        if ( DBCSState ) {
#endif // !i386
#line 4833
                            Int10Flag[vram_addr] = INT10_DBCS_TRAILING | INT10_CHANGED;
                            DBCSState = FALSE;
                        }
                        else if ( DBCSState = is_dbcs_first( *p ) ) {
                            Int10Flag[vram_addr] = INT10_DBCS_LEADING | INT10_CHANGED;
                        }
                        else {
                            Int10Flag[vram_addr] = INT10_SBCS | INT10_CHANGED;
                        }
                        vram_addr++;
#ifdef i386
#line 4844
                        p += 2;
#else // !i386 uses the EGA plane.
#line 4846
			p += 4;
#endif  // !i386
#line 4848
                }
                // Last char check! for Vz
                if ( DBCSState && ( vram_addr % 80 != 0 ) )
                    Int10Flag[vram_addr] = INT10_DBCS_TRAILING | INT10_CHANGED;

            }
            break;
#endif
#line 4856

	default:
		DbgPrint("NTVDM: Not support MS-DOS/V BOP:%d\n", op );
	        setCF(1);
		return;
    }
    setCF(0);
}

// mskkbug #3176 vz display white letter on white screen -yasuho
#ifdef	VGG
#line 4867
static void get_cur_pal_and_DAC(ptr)
	sys_addr	ptr;
{
	register	i;
	byte		temp;
	struct _rgb {
		byte	red, green, blue;
	} rgb;

	// get palette and overscan
	for(i = 0; i < 16; i++) {
		outb(EGA_AC_INDEX_DATA, (IU8)i); /* set index */
		inb(EGA_AC_SECRET, &temp);
		sas_store(ptr, temp);
		inb(EGA_IPSTAT1_REG, &temp);
		ptr++;
	}
	outb(EGA_AC_INDEX_DATA, 17); /* overscan index */
	inb(EGA_AC_SECRET, &temp);
	sas_store(ptr++, temp);
	inb(EGA_IPSTAT1_REG, &temp);
	outb(EGA_AC_INDEX_DATA, EGA_PALETTE_ENABLE);
	// get DAC registers
	for(i = 0; i < 256; i++)
	{
		outb(VGA_DAC_RADDR, (IU8)i);
		inb(VGA_DAC_DATA, &rgb.red);
		inb(VGA_DAC_DATA, &rgb.green);
		inb(VGA_DAC_DATA, &rgb.blue);
		sas_store(ptr++, rgb.red);
		sas_store(ptr++, rgb.green);
		sas_store(ptr++, rgb.blue);
	}
}
#endif	//VGG
#line 4902

//;;;;;;;;;;;;;;;;;;; end of MS-DOS/V BOP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#endif // JAPAN
#line 4905

#endif // !NEC_98
#line 4907
#if defined(NEC_98)
#line 4908
#error Unsupported NEC PC-98 configuration
#line 5620
#endif // NEC_98
#line 5621
