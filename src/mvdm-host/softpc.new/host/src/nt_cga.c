/*
 * SoftPC Revision 3.0
 *
 * Title        : Win32 CGA Graphics Module
 *
 * Description  :
 *
 *              This modules contain the Win32 specific functions required
 *              to support CGA emulations.
 *
 * Author       : Jerry Sexton (based on module by John Shanly)
 *
 * Notes        :
 *
 */

#include <windows.h>
#include <string.h>

#include "insignia.h"
#include "host_def.h"

#include "xt.h"
#include "gvi.h"
#include "gmi.h"
#include "sas.h"
#include "gfx_upd.h"

#include "error.h"
#include <stdio.h>
#include "trace.h"
#include "debug.h"
#include "config.h"
#include "host_rrr.h"
#include "conapi.h"

#include "nt_graph.h"
#include "nt_cga.h"
#include "nt_cgalt.h"
#include "nt_det.h"

#ifdef MONITOR
#line 43
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif /* MONITOR */
#line 46

/* Externs */


extern char *image_buffer;

/* Statics */

static unsigned int cga_med_graph_hi_nyb[256];
static unsigned int cga_med_graph_lo_nyb[256];
#ifdef BIGWIN
static unsigned int cga_med_graph_hi_lut_big[256];
static unsigned int cga_med_graph_mid_lut_big[256];
static unsigned int cga_med_graph_lo_lut_big[256];

static unsigned int cga_med_graph_lut4_huge[256];
static unsigned int cga_med_graph_lut3_huge[256];
static unsigned int cga_med_graph_lut2_huge[256];
static unsigned int cga_med_graph_lut1_huge[256];
#endif

/*
 *  cga_graph_inc_val depends on whether data is interleaved ( EGA/VGA )
 *  or not ( CGA ). Currently always interleaved.
 */
#define CGA_GRAPH_INCVAL 4

// likewise for TEXT_INCVAL
// for x86 we have 2 bytes per character (char and attr)
// for risc we have 4 bytes per character because of vga interleaving
//
#ifdef MONITOR
#line 78
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#else
#line 80
#define TEXT_INCVAL 4
#endif
#line 82





/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::: Initialise CGA text output ::::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_init_text()
{
    half_word misc;
    IMPORT void vga_misc_inb(io_addr, half_word *);

    /*::::::::::::::::::::::::::::::::::::: Tell trace program were we are */

    sub_note_trace0(HERC_HOST_VERBOSE, "nt_init_text");

#ifdef X86GFX
#line 101
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif	//X86GFX
#line 103
    closeGraphicsBuffer(); /* Tim Oct 92 */

#ifdef MONITOR
#line 106
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif  //MONITOR
#line 112
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::: Init CGA mono graph ::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_init_cga_mono_graph()
{
    sub_note_trace0(CGA_HOST_VERBOSE,"nt_init_cga_mono_graph - NOT SUPPORTED");
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::: Init CGA colour med graphics:::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


void nt_init_cga_colour_med_graph()
{
        static boolean cga_colour_med_deja_vu = FALSE;
        unsigned int i,
                     byte1,
                     byte2,
                     byte3,
                     byte4;

        sub_note_trace0(CGA_HOST_VERBOSE, "nt_init_cga_colour_med_graph");

        /* Set up bits-per-pixel for current mode. */
        sc.BitsPerPixel = CGA_BITS_PER_PIXEL;

        /* Initialise look-up table for first call. */
        if( !cga_colour_med_deja_vu )
        {
                for (i = 0; i < 256; i++)
                {
                        byte1 = i & 0x03;
                        byte2 = ( i & 0x0C ) >> 2;
                        byte3 = ( i & 0x30 ) >> 4;
                        byte4 = ( i & 0xC0 ) >> 6;

#line 152
#ifdef BIGEND
#line 153
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#endif /* BIGEND */
#line 188

#ifdef LITTLEND
#line 190
                        cga_med_graph_hi_nyb[i]
                                = ( byte3 << 24 ) | ( byte3 << 16)
                                        | ( byte4 << 8 ) | byte4;
                        cga_med_graph_lo_nyb[i]
                                = ( byte1 << 24 ) | ( byte1 << 16)
                                        | ( byte2 << 8 ) | byte2;

#ifdef BIGWIN
                        cga_med_graph_hi_lut_big[i]
                                = ( byte3 << 24 ) | ( byte4 << 16)
                                        | ( byte4 << 8 ) | byte4;
                        cga_med_graph_mid_lut_big[i]
                                = ( byte2 << 24) | ( byte2 << 16 )
                                        | ( byte3 << 8 ) | byte3;
                        cga_med_graph_lo_lut_big[i]
                                = ( byte1 << 24 ) | ( byte1 << 16)
                                        | ( byte1 << 8 ) | byte2;

                        cga_med_graph_lut4_huge[i]
                                = ( byte4 << 24 ) | ( byte4 << 16)
                                        | ( byte4 << 8 ) | byte4;

                        cga_med_graph_lut3_huge[i]
                                = ( byte3 << 24 ) | ( byte3 << 16)
                                        | ( byte3 << 8 ) | byte3;

                        cga_med_graph_lut2_huge[i]
                                = ( byte2 << 24 ) | ( byte2 << 16)
                                        | ( byte2 << 8 ) | byte2;

                        cga_med_graph_lut1_huge[i]
                                = ( byte1 << 24 ) | ( byte1 << 16)
                                        | ( byte1 << 8 ) | byte1;
#endif /* BIGWIN */
#endif /* LITTLEND */

                }

                cga_colour_med_deja_vu = TRUE;
        }
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::: Init CGA colour hi graphics:::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


void nt_init_cga_colour_hi_graph()
{
        sub_note_trace0(CGA_HOST_VERBOSE,"nt_init_cga_colour_hi_graph");

        /* Set up bits-per-pixel for current mode. */
        sc.BitsPerPixel = MONO_BITS_PER_PIXEL;
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::: Output CGA text :::::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

IMPORT int now_height, now_width;

void nt_text(int ScreenOffset, int ScreenX, int ScreenY,
                 int len, int height)
{
    int org_clen, org_height;
    int clen=len/2;
    int lines;
    SMALL_RECT WriteRegion;
    PBYTE   to;
    PBYTE   pScreenText = get_chain2_screen_ptr(ScreenOffset);

    /*:::::::::::::::::::::::::::::::::::::::::::: Output trace information */

    sub_note_trace6( CGA_HOST_VERBOSE,
		     "nt_cga_text off=%d x=%d y=%d len=%d h=%d o=%#x",
                     ScreenOffset, ScreenX, ScreenY, len, height, pScreenText );

    /*:::::::::::::: Adjust re-paint start location from pixels to characters */

#ifndef MONITOR
    /* Adjust for RISC parameters being in pixels */
    ScreenX = ScreenX / get_pix_char_width();
    ScreenY = ScreenY / get_host_char_height();
#endif

    /*:: Clip requested re-paint region to currently selected console buffer */

    //Clip width
    if(ScreenX + clen > now_width)
    {
	/* Is it possible to adjust the repaint regions width */
	if(ScreenX+1 >= now_width)
	{
	    assert4(NO,"VDM: nt_text() repaint region out of ranged x:%d y:%d w:%d h:%d\n",
		    ScreenX, ScreenY, clen, height);
	    return;
	}

	//Calculate maximum width
	org_clen = clen;
	clen = now_width - ScreenX;

	assert2(NO,"VDM: nt_text() repaint region width clipped from %d to %d\n",
		org_clen,clen);
    }

    //Clip height
    if(ScreenY + height > now_height)
    {
	/* Is it possible to adjust the repaint regions height */
	if(ScreenY+1 >= now_height)
	{
	    assert4(NO,"VDM: nt_text() repaint region out of ranged x:%d y:%d w:%d h:%d\n",
		    ScreenX, ScreenY, clen, height);
	    return;
	}

	//Calculate maximum height
	org_height = height;
	height = now_height - ScreenY;

	assert2(NO,"VDM: nt_text() repaint region height clipped from %d to %d\n",
		org_height,clen);
    }

	// note that the sharing buffer has different format on x86 and RISC
	// platforms. On x86, a cell is defined as:
	//	typedef _x86cell {
	//		byte	char;
	//		byte	attributes;
	//		}
	// on RISC, a cell is defined as:
	//	typedef _RISCcell {
	//		byte	char;
	//		byte	attributes;
	//		byte	reserved_1;
	//		byte	reserved_2;
	//		}
	// the size of each cell was defined by TEXT_INCVAL
	//

    to = &textBuffer[(ScreenY*now_width + ScreenX) * TEXT_INCVAL];
    for (lines = height; lines; lines--)
    {
        int cell;
        for (cell = 0; cell < clen; cell++)
            RtlCopyMemory(to + cell * TEXT_INCVAL,
                pScreenText + cell * 8, TEXT_INCVAL);
        pScreenText += get_offset_per_line() * 4;
        to += now_width * TEXT_INCVAL;
#line 362
    }

#line 365 "nt_cga.c"
    /*:::::::::::::::::::::::::::::::::::::::::::::: Calculate write region */

    WriteRegion.Left = ScreenX;
    WriteRegion.Top = ScreenY;

    WriteRegion.Bottom = WriteRegion.Top + height - 1;
    WriteRegion.Right = WriteRegion.Left + clen - 1;

    /*:::::::::::::::::::::::::::::::::::::::::::::::::: Display characters */

    sub_note_trace4( CGA_HOST_VERBOSE, "t=%d l=%d b=%d r=%d",
                      WriteRegion.Top, WriteRegion.Left,
                      WriteRegion.Bottom, WriteRegion.Right,
		    );

    if(!InvalidateConsoleDIBits(sc.OutputHandle, &WriteRegion)){
	/*
	** We get a rare failure here due to the WriteRegion
	** rectangle being bigger than the screen.
	** Dump out some values and see if it tells us anything.
	** Have also attempted to fix it by putting a delay between
	** the start of a register level mode change and window resize.
	*/
        assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
                 GetLastError() );
        assert4( NO, "VDM: rectangle t:%d l:%d b:%d r:%d", 
                 WriteRegion.Top, WriteRegion.Left,
                 WriteRegion.Bottom, WriteRegion.Right
               );
        assert2( NO, "VDM: bpl=%d sl=%d",
                 get_bytes_per_line(), get_screen_length() );
    }

}   /* end of nt_text() */


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::: Paints CGA graphics for a mono monitor, in a standard window :::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_mono_graph_std(int offset, int screen_x, int screen_y,
                           int len, int height )
{
sub_note_trace5(CGA_HOST_VERBOSE,
    "nt_cga_mono_graph_std off=%d x=%d y=%d len=%d height=%d - NOT SUPPORTED\n",
    offset, screen_x, screen_y, len, height);
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::: Paints CGA graphics for a mono monitor, in a big window :::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_mono_graph_big(int offset, int screen_x, int screen_y,
                           int len, int height)
{
sub_note_trace5(CGA_HOST_VERBOSE,
    "nt_cga_mono_graph_big off=%d x=%d y=%d len=%d height=%d - NOT SUPPORTED\n",
     offset, screen_x, screen_y, len, height);
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:::::::: Paints CGA graphics for a mono monitor, in a huge window ::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_mono_graph_huge(int offset, int screen_x, int screen_y,
                           int len, int height)
{
sub_note_trace5( CGA_HOST_VERBOSE,
    "nt_cga_mono_graph_huge off=%d x=%d y=%d len=%d height=%d - NOT SUPPORTED\n",
    offset, screen_x, screen_y, len, height );
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/* Paints CGA medium res graphics for a colour monitor,in a standard window */
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_med_graph_std(int offset, int screen_x, int screen_y,
                                 int len, int height)
{
    UTINY       *intelmem_ptr;
    ULONG       *graph_ptr;
    LONG         local_len,
                 bytes_per_scanline,
                 longs_per_scanline;
    ULONG	 inc;
    SMALL_RECT   rect;
    static int   rejections=0; /* Stop floods of rejected messages */

    sub_note_trace5(CGA_HOST_VERBOSE,
              "nt_cga_colour_med_graph_std off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height );

    /*
    ** Tim Jan 93, rapid mode changes cause mismatch between update and
    ** paint rountines. Ignore paint request when invalid parameter 
    ** causes crash.
    */
    if( screen_y > 400 ){
	assert1( NO, "VDM: med gfx std rejected y=%d\n", screen_y );
	return;
    }

    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	if( rejections==0 ){
		assert0( NO, "VDM: rejected paint request due to NULL handle" );
		rejections = 1;
	}
	return;
    }else{
	rejections = 0;
    }

    /* Clip image to screen */
    if(height > 1 || len > 80)
       height = 1;
    if (len>80)
	len = 80;

    /* Work out the width of a line (ie 640 pixels) in chars and ints. */
    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    longs_per_scanline = LONGS_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);


    /* Build up DIB. */
    inc = offset & 1 ? 7 : 1;
    intelmem_ptr = get_chain2_screen_ptr(offset);
    graph_ptr = (ULONG *) ((UTINY *) sc.ConsoleBufInfo.lpBitMap +
                           (screen_y * bytes_per_scanline + screen_x));
    local_len = len;
    do
    {
        *(graph_ptr + longs_per_scanline) = *graph_ptr =
            cga_med_graph_hi_nyb[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + longs_per_scanline) = *graph_ptr =
            cga_med_graph_lo_nyb[*intelmem_ptr];
        graph_ptr++;

        intelmem_ptr += inc;
        inc ^= 6;
    }
    while( --local_len );

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = screen_x;
    rect.Top = screen_y;
    rect.Right = rect.Left + (len << 3) - 1;
    rect.Bottom = rect.Top + (height << 1) - 1;

    if( sc.ScreenBufHandle )
    {
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
    }
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*:: Paints CGA medium res graphics for a colour monitor, in a big window ::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_med_graph_big(int offset, int screen_x, int screen_y,
                                 int len, int height)
{
#ifdef BIGWIN
    UTINY       *intelmem_ptr;
    ULONG       *graph_ptr;
    LONG         local_len,
                 bytes_per_scanline,
                 longs_per_scanline;
    ULONG	 inc;
    SMALL_RECT   rect;

    sub_note_trace5(CGA_HOST_VERBOSE,
              "nt_cga_colour_med_graph_big off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height);

    /*
    ** Tim Jan 93, rapid mode changes cause mismatch between update and
    ** paint rountines. Ignore paint request when invalid parameter 
    ** causes crash.
    */
    if( screen_y > 400 ){
	assert1( NO, "VDM: med gfx big rejected y=%d\n", screen_y );
	return;
    }

    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	assert0( NO, "VDM: rejected paint request due to NULL handle" );
	return;
    }

    /* Clip to window */
    height = 1;
    if (len > 80)
	len = 80;

    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    longs_per_scanline = LONGS_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    inc = offset & 1 ? 3 : 1;
    intelmem_ptr = get_screen_ptr(offset);
    graph_ptr = (ULONG *) ((UTINY *) sc.ConsoleBufInfo.lpBitMap +
                           SCALE(screen_y * bytes_per_scanline + screen_x));
    local_len = len;

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);

    do
    {
        *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr =
            cga_med_graph_hi_lut_big[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr =
            cga_med_graph_mid_lut_big[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr =
            cga_med_graph_lo_lut_big[*intelmem_ptr];
        graph_ptr++;

        intelmem_ptr += inc;
        inc ^= 2;
    }
    while( --local_len );

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = SCALE(screen_x);
    rect.Top = SCALE(screen_y);
    rect.Right = rect.Left + SCALE(len << 3) - 1;
    rect.Bottom = rect.Top + SCALE(height << 1) - 1;

    if( sc.ScreenBufHandle )
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
#endif /* BIGWIN */
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*: Paints CGA medium res graphics for a colour monitor, in a huge window. :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_med_graph_huge(int offset, int screen_x, int screen_y,
                                  int len, int height)
{
#ifdef BIGWIN
    UTINY       *intelmem_ptr;
    ULONG       *graph_ptr;
    LONG         local_len,
                 bytes_per_scanline,
                 longs_per_scanline;
    ULONG	 inc;
    SMALL_RECT   rect;

    sub_note_trace5(CGA_HOST_VERBOSE,
             "nt_cga_colour_med_graph_huge off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height );

    /*
    ** Tim Jan 93, rapid mode changes cause mismatch between update and
    ** paint rountines. Ignore paint request when invalid parameter 
    ** causes crash.
    */
    if( screen_y > 400 ){
	assert1( NO, "VDM: med gfx huge rejected y=%d\n", screen_y );
	return;
    }

    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	assert0( NO, "VDM: rejected paint request due to NULL handle" );
	return;
    }

    /* Clip to window */
    height = 1;
    if (len > 80)
	len = 80;

    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    longs_per_scanline = LONGS_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    inc = offset & 1 ? 3 : 1;
    intelmem_ptr = get_screen_ptr(offset);
    graph_ptr = (ULONG *) ((UTINY *) sc.ConsoleBufInfo.lpBitMap +
                           SCALE(screen_y * bytes_per_scanline + screen_x));
    local_len = len;

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);

    do
    {
        *(graph_ptr + 3 * longs_per_scanline) =
            *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr = cga_med_graph_lut4_huge[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + 3 * longs_per_scanline) =
            *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr = cga_med_graph_lut3_huge[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + 3 * longs_per_scanline) =
            *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr = cga_med_graph_lut2_huge[*intelmem_ptr];
        graph_ptr++;

        *(graph_ptr + 3 * longs_per_scanline) =
            *(graph_ptr + 2 * longs_per_scanline) =
            *(graph_ptr + longs_per_scanline) =
            *graph_ptr = cga_med_graph_lut1_huge[*intelmem_ptr];
        graph_ptr++;

        intelmem_ptr += inc;
        inc ^= 2;
    }
    while(--local_len);

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = SCALE(screen_x);
    rect.Top = SCALE(screen_y);
    rect.Right = rect.Left + SCALE(len << 3) - 1;
    rect.Bottom = rect.Top + SCALE(height << 1) - 1;

    if( sc.ScreenBufHandle )
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
#endif /* BIGWIN */
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*: Paints CGA high res graphics for a colour monitor, in a standard window */
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_hi_graph_std(int offset, int screen_x, int screen_y,
                                int len, int height)
{
    register char   *intelmem,
                    *bufptr;
    register int     i;
    int              bytes_per_scanline;
    SMALL_RECT       rect;
    static int       rejections=0; /* Stop floods of rejected messages */

    sub_note_trace5(CGA_HOST_VERBOSE,
               "nt_cga_colour_hi_graph_std off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height );

    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	if( rejections == 0 ){
		assert0( NO, "VDM: rejected paint request due to NULL handle" );
		rejections = 1;
	}
	return;
    }else
	rejections=0;

    /* Clip to window */
    height = 1;
    if (len > 80)
	len = 80;

    /* Work out offset, in bytes, of pixel directly below current pixel. */
    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);

    /*
     * Build up DIB data. In 200-line CGA mode, pixels are double height so
     * one line of PC pixels is equivalent to two lines of host pixels.
     * Note: `height' parameter is always 1 when this function is called so
     * only 1 line at a time is updated.
     */
    intelmem = (char *) get_screen_ptr(offset);

    bufptr =  (char *) sc.ConsoleBufInfo.lpBitMap +
              screen_y * bytes_per_scanline +
              (screen_x >> 3);
    for( i = len; i > 0; i-- )
    {
        *(bufptr + bytes_per_scanline) = *bufptr = *intelmem;
        intelmem += CGA_GRAPH_INCVAL;
        bufptr++;
    }

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = screen_x;
    rect.Top = screen_y;
    rect.Right = rect.Left + (len << 3) - 1;
    rect.Bottom = rect.Top + (height << 1) - 1;

    if( sc.ScreenBufHandle )
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::: Paints CGA high res graphics for a colour monitor, in a big window :::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_hi_graph_big(int offset, int screen_x, int screen_y,
                                int len, int height)
{
#ifdef BIGWIN
    register char   *intelmem,
                    *bufptr;
    register int    i;
    char            *buffer;
    int             bytes_per_scanline;
    SMALL_RECT      rect;

    sub_note_trace5(CGA_HOST_VERBOSE,
              "nt_cga_colour_hi_graph_big off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height );
    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	assert0( NO, "VDM: rejected paint request due to NULL handle" );
	return;
    }

    /* Clip to window */
    height = 1;
    if (len > 80)
	len = 80;

    /*
     * In this mode each byte becomes 12 bits (1.5 screen size) so if screen_x
     * is on an odd byte boundary the resulting bitmap starts on a half-byte
     * boundary. To avoid this set screen_x to the previous even byte.
     */
    if (screen_x & 8)
    {
        screen_x -= 8;
        offset -= CGA_GRAPH_INCVAL;
        len++;
    }

    /* `len' must be even for `high_stretch3' to work. */
    if (len & 1)
        len++;

    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    bufptr = buffer = (char *) sc.ConsoleBufInfo.lpBitMap +
                      SCALE(screen_y * bytes_per_scanline + (screen_x >> 3));
    intelmem = (char *) get_screen_ptr(offset);

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);

    for(i = len; i > 0; i--)
    {
        *bufptr = *intelmem;
        intelmem += CGA_GRAPH_INCVAL;
        bufptr++;
    }

    high_stretch3((unsigned char *) buffer, len);

    memcpy(buffer + bytes_per_scanline, buffer, SCALE(len));
    memcpy(buffer + 2 * bytes_per_scanline, buffer, SCALE(len));

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = SCALE(screen_x);
    rect.Top = SCALE(screen_y);
    rect.Right = rect.Left + SCALE(len << 3) - 1;
    rect.Bottom = rect.Top + SCALE(height << 1) - 1;

    if( sc.ScreenBufHandle )
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
#endif /* BIGWIN */
}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::: Paints CGA high res graphics for a colour monitor, in a huge window ::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void nt_cga_colour_hi_graph_huge(int offset, int screen_x, int screen_y,
                                 int len, int height )
{
#ifdef BIGWIN
    register char   *intelmem,
                    *bufptr;
    char            *buffer;
    register int    i;
    int             bytes_per_scanline;
    SMALL_RECT      rect;

    sub_note_trace5(CGA_HOST_VERBOSE,
              "nt_cga_colour_hi_graph_huge off=%d x=%d y=%d len=%d height=%d\n",
                    offset, screen_x, screen_y, len, height );
    /*
    ** Tim September 92, bounce call if handle to screen buffer is null.
    ** This can happen when VDM session is about to suspend, buffer has
    ** been closed, but still get a paint request.
    */
    if( sc.ScreenBufHandle == (HANDLE)NULL ){
	assert0( NO, "VDM: rejected paint request due to NULL handle" );
	return;
    }

    /* Clip to window */
    height = 1;
    if (len > 80)
	len = 80;

    bytes_per_scanline = BYTES_PER_SCANLINE(sc.ConsoleBufInfo.lpBitMapInfo);
    intelmem = (char *) get_screen_ptr(offset);
    bufptr = buffer = (char *) sc.ConsoleBufInfo.lpBitMap +
                      SCALE(screen_y * bytes_per_scanline + (screen_x >> 3));

    /* Grab the mutex. */
    GrabMutex(sc.ConsoleBufInfo.hMutex);

    for( i = len; i > 0; i-- )
    {
        *bufptr = *intelmem;
        intelmem += CGA_GRAPH_INCVAL;
        bufptr++;
    }

    high_stretch4((unsigned char *) buffer, len);

    memcpy(buffer + bytes_per_scanline, buffer, SCALE(len));
    memcpy(buffer + 2 * bytes_per_scanline, buffer, SCALE(len));
    memcpy(buffer + 3 * bytes_per_scanline, buffer, SCALE(len));

    /* Release the mutex. */
    RelMutex(sc.ConsoleBufInfo.hMutex);

    /* Display the new image. */
    rect.Left = SCALE(screen_x);
    rect.Top = SCALE(screen_y);
    rect.Right = rect.Left + SCALE(len << 3) - 1;
    rect.Bottom = rect.Top + SCALE(height << 1) - 1;

    if( sc.ScreenBufHandle )
	if (!InvalidateConsoleDIBits(sc.ScreenBufHandle, &rect))
		assert1( NO, "VDM: InvalidateConsoleDIBits() error:%#x",
			 GetLastError() );
        //DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
#endif /* BIGWIN */
}

#ifdef MONITOR
#line 972
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif /* MONITOR */
#line 1194
