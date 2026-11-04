#include <windows.h>
#include "host_def.h"
#include "insignia.h"

/*
 * ==========================================================================
 *	Name:		nt_fulsc.c
 *	Author:		Jerry Sexton
 *	Derived From:
 *	Created On:	27th January 1992
 *	Purpose:	This module contains the code required to handle
 *			transitions between graphics and text modes, and
 *			windowed and full-screen displays for SoftPC running
 *			under the x86 monitor.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.
 * ==========================================================================
 */

/*
 * ==========================================================================
 * Other Includes
 * ==========================================================================
 */
#ifdef X86GFX
#line 26
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 28
#include <vdm.h>
#include <stdlib.h>
#include <string.h>
#include "mvdm_softpc_firmware.h"
#line 31
#include "conapi.h"

#include "xt.h"
#include CpuH
#include "gvi.h"
#include "gmi.h"
#include "gfx_upd.h"
#include "video.h"
#include "egacpu.h"
#include "egavideo.h"
#include "egagraph.h"
#include "egaports.h"
#include "egamode.h"
#include "ckmalloc.h"
#include "sas.h"
#include "ica.h"
#include "ios.h"
#include "config.h"
#include "idetect.h"
#include "debug.h"

#include "nt_thred.h"
#include "nt_fulsc.h"
#include "nt_graph.h"
#include "nt_uis.h"
#include "host_rrr.h"
#include "nt_det.h"
#include "nt_mouse.h"
#include "nt_event.h"
#include "ntcheese.h"
#include "nt_eoi.h"
#include "nt_reset.h"

/*
 * ==========================================================================
 * Global Data
 * ==========================================================================
 */
GLOBAL BOOL	ConsoleInitialised = FALSE;
GLOBAL BOOL	ConsoleNoUpdates = FALSE;
#ifdef X86GFX
#line 72
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 76
GLOBAL BOOL blocked_in_gfx_mode = FALSE;  /* need to force text mode? */
#line 77
#ifndef PROD
#line 78
#error DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 80

/* We have to prevent bad values from oddball video cards (eg Prodesigner II
 * EISA) from blatting us before we can load our private baby mode table in
 * ntio.sys. We have to keep another copy to be copied into memory to prevent
 * this. We should only need modes 3 & b.
 */
GLOBAL UTINY tempbabymode[] =
/* 80x25 stuff */
 {
   0x50, 0x18, 0x10, 0x00, 0x10, 0x00, 0x03, 0x00, 0x02, 0x67,
   0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f, 0x00, 0x4f,
   0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x8e, 0x8f, 0x28,
   0x1f, 0x96, 0xb9, 0xa3, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 
   0x05, 0x14, 0x07, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
   0x3f, 0x0c, 0x00, 0x0f, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x10, 0x0e, 0x00, 0xff,
/* mode b stuff */
   0x5e, 0x32, 0x08, 0x00, 0x97, 0x01, 0x0f, 0x00, 0x06, 0xe7,
   0x6d, 0x5d, 0x5e, 0x90, 0x61, 0x8f, 0xbf, 0x1f, 0x00, 0x40,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0x8e, 0x99, 0x2f,
   0x00, 0xa1, 0xb9, 0xe3, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04,
   0x05, 0x14, 0x07, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
   0x3f, 0x01, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x05, 0x0f, 0xff
};

/*
 * ==========================================================================
 * Local Data
 * ==========================================================================
 */

/* The resolution and font-size at start-up. */
LOCAL COORD startUpResolution;
LOCAL COORD startUpFontSize;

/* General purpose console buffer. */
LOCAL CHAR_INFO consoleBuffer[MAX_CONSOLE_SIZE];

LOCAL BOOL WinFrozen = FALSE;

/* Console info from startup which is needed for synchronisation */
LOCAL int ConVGAHeight;
LOCAL int ConTopLine;

/* saved information for console re-integration */
LOCAL CONSOLE_SCREEN_BUFFER_INFO	 ConsBufferInfo;
LOCAL StartupCharHeight;

LOCAL half_word saved_text_lines; /* No of lines for last SelectMouseBuffer. */


/* Variable to check for changes in screen state. */
GLOBAL DWORD savedScreenState;
BOOL nt_init_called = 0;

IMPORT CONSOLE_CURSOR_INFO StartupCursor;

IMPORT void low_set_mode(int);
IMPORT VOID recalc_text(int);
IMPORT VOID enable_gfx_update_routines(VOID);
IMPORT VOID disable_gfx_update_routines(VOID);
#ifdef X86GFX
#line 143
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 145


/*
 * ==========================================================================
 * Local Function Declarations
 * ==========================================================================
 */
VOID enableUpdates(VOID);
VOID disableUpdates(VOID);
VOID copyConsoleToRegen(SHORT, SHORT, SHORT, SHORT);
VOID getVDMCursorPosition(VOID);
VOID setVDMCursorPosition(UTINY, PCOORD);
VOID waitForInputFocus(VOID);
GLOBAL int getModeType(VOID);
#ifdef X86GFX
#line 160
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 163

/*
 * ==========================================================================
 * Global Functions
 * ==========================================================================
 */

GLOBAL VOID nt_init_event_thread(VOID)
{
    note_entrance0("nt_init_event_thread");

    /*
     * May be called more than once, if event thread enters
     * resume\block code before normally intialized
     */
    if (nt_init_called)
	return;
    else
        nt_init_called++;


    if (sc.ScreenState != STREAM_IO) {
	/*
	** Copy the console buffer to the regen buffer.
	** Don't want to adjust the copy from top of console window, console
	** does it itself if we resize the window. Tim September 92.
	*/
	copyConsoleToRegen(0, 0, VGA_WIDTH, (SHORT)ConVGAHeight);

	/*
	** Tim September 92, adjust cursor position if console window size is
	** adjusted.
	*/
	ConsBufferInfo.dwCursorPosition.Y -= ConTopLine;

	/* Set up SoftPC's cursor. */
	setVDMCursorPosition((UTINY)StartupCharHeight,
					&ConsBufferInfo.dwCursorPosition);
                      
#ifdef X86GFX
#line 202
#error Unsupported excluded original configuration
#endif
#line 203
            enableUpdates();
    }
    else
	enableUpdates();

    // set kbd state flags in biosdata area, according to the real kbd Leds
    if (!VDMForWOW) {
        SyncBiosKbdLedToKbdDevice();
	// we have sync up the BIOS led states with the system, we now let the
	// event thread go
	ResumeThread(ThreadInfo.EventMgr.Handle);
        }

    KbdResume(); // JonLe Mod
}


#line 220
#ifdef X86GFX
#line 221
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 511

/*
***************************************************************************
** calcScreenParams(), setup our screen screen parameters as determined
** by current Console state.
** Called from ConsoleInit() and DoFullScreenResume().
** Returns current character height (8,14,16) and lines (22-50).
** Tim Jan 93, extracted common code from init and resume funx.
***************************************************************************
*/
GLOBAL VOID calcScreenParams IFN2( USHORT *, pCharHeight, USHORT *, pVgaHeight )
{
    USHORT   consoleWidth,
	     consoleHeight,
	     vgaHeight,
	     charHeight,
	     scanLines;
    half_word temp;

    /* Get console information. */
    if (!GetConsoleScreenBufferInfo(sc.OutputHandle, &ConsBufferInfo))
	ErrorExit();

    /* Now sync the SoftPC screen to the console. */
#ifdef X86GFX
#line 535
#error Unsupported excluded original configuration
#endif
#line 536
    {
	consoleWidth = ConsBufferInfo.srWindow.Right -
		       ConsBufferInfo.srWindow.Left + 1;
	consoleHeight = ConsBufferInfo.srWindow.Bottom -
			ConsBufferInfo.srWindow.Top + 1;
    }
#ifdef X86GFX
#line 543
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 553

    /*
     * Set the display to the nearest VGA text mode size, which is one of
     * 80x22, 80x25, 80x28, 80x43 or 80x50.
     */
    if (consoleHeight <= MID_VAL(VGA_HEIGHT_0, VGA_HEIGHT_1))
    {
	/* 22 lines */
	vgaHeight = VGA_HEIGHT_0;
	scanLines = 351;
	charHeight = 16;
    }
    else if (consoleHeight <= MID_VAL(VGA_HEIGHT_1, VGA_HEIGHT_2))
    {
	/* 25 lines */
	vgaHeight = VGA_HEIGHT_1;
	scanLines = 399;
	charHeight = 16;
    }
    else if (consoleHeight <= MID_VAL(VGA_HEIGHT_2, VGA_HEIGHT_3))
    {
	/* 28 lines */
	vgaHeight = VGA_HEIGHT_2;
	scanLines = 391;
	charHeight = 14;
    }
    else if (consoleHeight <= MID_VAL(VGA_HEIGHT_3, VGA_HEIGHT_4))
    {
	/* 43 lines */
	vgaHeight = VGA_HEIGHT_3;
	scanLines = 349;
	charHeight = 8;
    }
    else
    {
	/* 50 lines */
	vgaHeight = VGA_HEIGHT_4;
	scanLines = 399;
	charHeight = 8;
    }

#ifdef X86GFX
#line 594
#error Unsupported excluded original configuration
#endif
#line 595
    {
        /* The app may have shutdown in a gfx mode - force text mode back */
        if (blocked_in_gfx_mode)
        {
            low_set_mode(3);
            inb(EGA_IPSTAT1_REG,&temp);
            outb(EGA_AC_INDEX_DATA, EGA_PALETTE_ENABLE);   /* re-enable video */
            (*choose_display_mode)();
            blocked_in_gfx_mode = FALSE;
        }
	/*
	 * Set screen height appropriately for current window size.
	 * Now call video routine to set the character height, updating the
	 * BIOS RAM as it does so.
	 */	
	set_screen_height_recal( scanLines ); /* Tim Oct 92 */
	recalc_text(charHeight);

        /* badly written apps assume 25 line mode page len is 4096 */
        if (vgaHeight == 25)
            sas_storew_no_check(VID_LEN, 0x1000);
#ifdef X86GFX
#line 617
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif	/* X86GFX */
#line 619

    }
#ifdef X86GFX
#line 622
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 647
    sas_storew_no_check(VID_COLS, 80);   // fixup from 40 char shutdown
    *pCharHeight = charHeight;
    *pVgaHeight  = vgaHeight;

} /* end of calcScreenParams() */

/***************************************************************************
 * Function:								   *
 *	ConsoleInit 							   *
 *									   *
 * Description: 							   *
 *	Does all the graphics work required on SoftPC start-up. 	   *
 *      Will split or modify later to accomodate the SCS initialisation    *
 *      that loses the config.sys etc output.                              *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
GLOBAL VOID ConsoleInit(VOID)
{
    USHORT   charHeight, vgaHeight, cursorLine, topLine;

    note_entrance0("ConsoleInit");

#line 675
#ifdef X86GFX
#line 676
#error Unsupported excluded original configuration
#endif
#line 687
    initTextSection();

    /*
     * Set up input focus details (we do it here as the fullscreen stuff
     * is what is really interested in it).
     */
    sc.Focus = TRUE;
    sc.FocusEvent = CreateEvent((LPSECURITY_ATTRIBUTES) NULL,
				FALSE,
				FALSE,
				NULL);

#ifdef X86GFX
#line 700
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 705

    /*
     * We don't want to call paint routines until config.sys processed or if
     * the monitor is writing directly to the frame buffer (fullscreen), so...
     */
    disableUpdates();

    /*
    ** Get console window size and set up our stuff accordingly.
    */
    calcScreenParams( &charHeight, &vgaHeight );

    StartupCharHeight = charHeight;
#ifdef X86GFX
#line 719
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 751

    /*
     * Work out the top line to be displayed in the VGA window, which is line
     * zero of the console unless the cursor would not be displayed, in which
     * case the window is moved down until the cursor is on the bottom line.
     */
    cursorLine = ConsBufferInfo.dwCursorPosition.Y;
    if (cursorLine < vgaHeight)
	topLine = 0;
    else
	topLine = cursorLine - vgaHeight + (SHORT) 1;

    ConVGAHeight = vgaHeight;
    ConTopLine = topLine;

    ConsoleInitialised = TRUE;
}


/***************************************************************************
 * Function:								   *
 *	GfxReset                                                           *
 *									   *
 * Description: 							   *
 *	Called from ConsoleInit to program up the vga hardware into some   *
 *	known state. This compensates on the X86 for not initialising via  *
 *      our bios.    Essential for windowed running, but probably needed   *
 *	for the what-mode-am-i-in stuff as well.			   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
GLOBAL VOID GfxReset(VOID)
{
#ifdef X86GFX
#line 790
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 816
}

/***************************************************************************
 * Function:								   *
 *	ResetConsoleState						   *
 *									   *
 * Description: 							   *
 *	Attempts to put the console window back to the state in which      *
 *      it started up.							   *
 *									   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
GLOBAL VOID ResetConsoleState(VOID)
{
#ifdef X86GFX
#line 837
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 875
    SMALL_RECT	newWin;
    BOOL itfailed = FALSE;

#ifdef X86GFX
#line 879
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 882

	closeGraphicsBuffer();

	if (StreamIoSwitchOn && !host_stream_io_enabled) {
	    /* restore screen buffer and window size */
	    SetConsoleScreenBufferSize(sc.OutputHandle, sc.ConsoleBuffInfo.dwSize);
	    newWin.Top = newWin.Left = 0;
	    newWin.Bottom = sc.ConsoleBuffInfo.srWindow.Bottom -
			    sc.ConsoleBuffInfo.srWindow.Top;
	    newWin.Right = sc.ConsoleBuffInfo.srWindow.Right -
			   sc.ConsoleBuffInfo.srWindow.Left;
	    SetConsoleWindowInfo(sc.OutputHandle, TRUE, &newWin);
	}
	/*                                                                      
	** Tim September 92, don't resize window on way out of DOS app cos      
	** MS (Sudeep) said so. Don't want to do the associated recalc_text()   
	** either.                                                              
	** This keeps the window re-sizing issue nice and simple, but there'll  
	** be people who don't like having a DOS window size forced upon them.  
	*/                                                                      
#line 902
#if 0                                                                           
#line 903
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif	//Zero
#line 925

#if 0
#line 927
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 964
	/*
	** Tim, September 92. Put the Console cursor to the same place as the
	** SoftPC cursor. We already do this in full-screen text mode below.
	** Specifcally to fix weird cursor position problem with 16-bit nmake,
	** but seems like a good safety idea anyway.
	*/
	getVDMCursorPosition();

        doNullRegister();   /* revert console back to ordinary window */

#line 974
#ifdef X86GFX
#line 975
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 1205

    /*Put console's cursor back to the shape it was on startup.*/
    SetConsoleCursorInfo(sc.OutputHandle, &StartupCursor);

    /* reset the current_* variables in nt_graph.c */
    resetWindowParams();
}


#ifdef X86GFX
#line 1215
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 1404

/*
***************************************************************************
** getNtScreenState() - return 0 for windowed, 1 for full-screen.
***************************************************************************
** Tim July 92.
**
** DIVERGENCE(MVDM-NTIO-WINDOWED-STATE): this original, state-only query is
** required by the original NTIO.SYS BOP-5F vector handoff in the selected
** CPU40 worker.  Its original surrounding X86GFX block also owns the retired
** private-console fullscreen transition path.  Keep the original function
** body and publish it outside that unavailable product shell.
*/
#line 1410
GLOBAL UTINY getNtScreenState IFN0()
{
	return( (UTINY) sc.ScreenState );
}

#ifdef X86GFX
#line 1415
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 1465

/***************************************************************************
 * Function:								   *
 *	DoFullScreenResume						   *
 *									   *
 * Description: 							   *
 *	Called by SCS to restart SoftPC when a DOS application restarts    *
 *	after being suspended or starts up for the first time after SoftPC *
 *	has been booted by another application which has since terminated. *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/

GLOBAL VOID DoFullScreenResume(VOID)
{
    USHORT vgaHeight, height;
#ifndef X86GFX
    PVOID pDummy;
#endif

#ifdef X86GFX
#line 1491
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 1554

    // re-register with console for fullscreen switching.
    if( !RegisterConsoleVDM( VDMForWOW ?
                                CONSOLE_REGISTER_WOW : CONSOLE_REGISTER_VDM,
#ifdef X86GFX
#line 1559
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 1562
			     NULL,
			     NULL,
#endif
#line 1565

                             NULL,            // sectionname no longer used
                             0,               // sectionname no longer used
			     &stateLength,
#line 1569
#ifndef X86GFX
#line 1570
			     &pDummy,
#else
#line 1572
#error DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register.
#endif
#line 1574
                             NULL,            // sectionname no longer used
                             0,               // sectionname no longer used
			     textBufferSize,
			     (PVOID *) &textBuffer
			   )
    )
	ErrorExit();

#ifdef X86GFX
#line 1583
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 1588
	/*
	** Create graphics buffer if we need one. Tim Oct 92.
	*/
	if( sc.ModeType==GRAPHICS )
		graphicsResize();
#endif
#line 1594
	/*
	** Tim September 92.
	** If window size is not suitable for a DOS app, snap-to-fit
	** appropriately. Put cursor in correct place.
        ** Do the ConsoleInit() and nt_init_event_thread() type of things.
	** Leave full-screen as it was.
	*/
	if (sc.ScreenState != WINDOWED) {
	   /* Get console info, including the current cursor position. */
	   if (!GetConsoleScreenBufferInfo(sc.OutputHandle, &ConsBufferInfo))
		ErrorExit();
	   /* Hard-wired for f-s resume - needs to be set properly. */
	   height = 8;
	   /* Set up BIOS variables etc. */
	   setVDMCursorPosition( (UTINY)height,
	                         &ConsBufferInfo.dwCursorPosition);
	   /* Copy the console buffer to the regen buffer. */
	   copyConsoleToRegen(0, 0, VGA_WIDTH, (SHORT)ConVGAHeight);
	}

	/*
	** Get console window size and set up our stuff accordingly.
	*/
	calcScreenParams( &height, &vgaHeight );

	/*
	** Window resize code copied out of nt_graph.c:textResize().
	*/
	{
	resizeWindow( 80, vgaHeight );
	}

	/* Copy the console buffer to the regen buffer. */
	copyConsoleToRegen(0, 0, VGA_WIDTH, vgaHeight);

	/*
	** Make sure cursor is not below bottom line.
	*/
	if( ConsBufferInfo.dwCursorPosition.Y >= vgaHeight ){
		ConsBufferInfo.dwCursorPosition.Y = vgaHeight-1;
	}
	setVDMCursorPosition(( UTINY)height, &ConsBufferInfo.dwCursorPosition);

} /* end of DoFullScreenResume() */

/***************************************************************************
 * Function:								   *
 *	GfxCloseDown        						   *
 *									   *
 * Description: 							   *
 *	Hook from host_terminate to ensure section closed so can then start*
 *      more VDMs.							   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID (Errors handled internally in CloseSection)		   *
 *									   *
 ***************************************************************************/
GLOBAL VOID GfxCloseDown(VOID)
{
    /* Text and Video sections previously closed here... */
}

#ifdef X86GFX
#line 1660
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 1765


/*
 * ==========================================================================
 * Local Functions
 * ==========================================================================
 */

/***************************************************************************
 * Function:								   *
 *	enableUpdates							   *
 *									   *
 * Description: 							   *
 *	Restarts the reflection of regen buffer updates to paint routines. *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
VOID enableUpdates(VOID)
{
    enable_gfx_update_routines();
    ConsoleNoUpdates = FALSE;
}

/***************************************************************************
 * Function:								   *
 *	disableUpdates							   *
 *									   *
 * Description: 							   *
 *	Stops changes to the regen buffer being reflected to paint	   *
 *	routines.							   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
VOID disableUpdates(VOID)
{
    disable_gfx_update_routines();
    ConsoleNoUpdates = TRUE;
}

/***************************************************************************
 * Function:								   *
 *	copyConsoleToRegen						   *
 *									   *
 * Description: 							   *
 *	Copies the contents of the console buffer to the video regen	   *
 *	buffer.								   *
 *									   *
 * Parameters:								   *
 *	startCol - start column of console buffer			   *
 *	startLine - start line of console buffer			   *
 *	width - width of console buffer 				   *
 *	height - height of console buffer				   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
VOID copyConsoleToRegen(SHORT startCol, SHORT startLine, SHORT width,
			      SHORT height)
{
    CHAR_INFO	*from;
    COORD	 bufSize,
		 bufCoord;
    LONG	 nChars;
    SMALL_RECT	 readRegion;

    register half_word	*to;
#ifdef X86GFX
#line 1843
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 1846


    /* Allocate the buffer to get the console data into */
    nChars = width * height;
    assert0(nChars <= MAX_CONSOLE_SIZE, "Console buffer overflow");

    /* Get the console data. */
    bufSize.X = width;
    bufSize.Y = height;
    bufCoord.X = 0;
    bufCoord.Y = 0;
    readRegion.Left = startCol;
    readRegion.Top = startLine;
    readRegion.Right = startCol + width - (SHORT) 1;
    readRegion.Bottom = startLine + height - (SHORT) 1;
    if (!ReadConsoleOutput(sc.OutputHandle,
			   consoleBuffer,
			   bufSize,
			   bufCoord,
			   &readRegion))
       ErrorExit();

    /* Copy the console data to the regen buffer. */
    from = consoleBuffer;

#line 1871
#ifndef X86GFX	// on MIPS we actually want to write to the VGA bitplanes.
#line 1872
    to = EGA_planes;
    while(nChars--)
    {
	*to++ = from->Char.AsciiChar;
	*to = (half_word) from->Attributes;
	from++;
	to += 3;	// skipping interleaved font planes.
    }
    host_mark_screen_refresh();
#else
#line 1882
#error DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register.
#endif
#line 1902
}

/***************************************************************************
 * Function:								   *
 *	getVDMCursorPosition						   *
 *									   *
 * Description: 							   *
 *	Gets the cursor position from BIOS variables and tells the console *
 *	where to place its cursor.					   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
VOID getVDMCursorPosition(VOID)
{
    half_word currentPage;
    word cursorWord;
    COORD cursorPos;
    BOOL setok;

    /* Get the current video page. */
    currentPage = sas_hw_at_no_check(vd_current_page);

    /* Store cursor position in BIOS variables. */
    cursorWord = sas_w_at_no_check(VID_CURPOS + (currentPage * 2));

    /* Set the console cursor. */
    cursorPos.X = (SHORT) (cursorWord & 0xff);
    cursorPos.Y = (cursorWord >> 8) & (SHORT) 0xff;

    if ((sc.CharHeight * cursorPos.Y) > sc.PC_W_Height)
	cursorPos.Y = (sc.PC_W_Height / sc.CharHeight) - 1;

    if( !stdoutRedirected ) {
	setok = SetConsoleCursorPosition(sc.OutputHandle, cursorPos);
	if (!setok) {

	    if (GetLastError() != ERROR_INVALID_HANDLE)	// ie. output redirected
		ErrorExit();
	}
    }
}

/***************************************************************************
 * Function:								   *
 *	setVDMCursorPosition						   *
 *									   *
 * Description: 							   *
 *	Positions SoftPC's cursor, setting the relevant BIOS variables.	   *
 *									   *
 * Parameters:								   *
 *	height		- the current character height			   *
 *	cursorPos	- the coordinates of the cursor			   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
VOID setVDMCursorPosition(UTINY height, PCOORD cursorPos)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    ULONG port6845,
    	  cursorStart,
	  cursorEnd,
	  colsOnScreen,
	  videoLen,
	  pageOffset,
	  cursorWord;
    UTINY currentPage;

    /* Get cursor size. */
    if(!GetConsoleCursorInfo(sc.OutputHandle, &cursorInfo))
	ErrorExit();

    /* Work out cursor start and end pixels. */
    cursorStart = height - (height * cursorInfo.dwSize / 100);
    if (cursorStart == height)
	cursorStart--;
    cursorEnd = height - 1;

#ifdef X86GFX
#line 1986
#error Unsupported excluded original configuration
#endif
#line 1987
    {

	/* Pass cursor size to video ports. */
	port6845 = sas_w_at_no_check(VID_INDEX);
	outb((io_addr) port6845, R10_CURS_START);
	outb((io_addr) (port6845 + 1), (half_word) cursorStart);
	outb((io_addr) port6845, R11_CURS_END);
	outb((io_addr) (port6845 + 1), (half_word) cursorEnd);
    }

    /* Get the current video page. */
    currentPage = sas_hw_at_no_check(vd_current_page);

    /* Set BIOS variables. */
    sas_storew_no_check(VID_CURMOD,
			(word) ((cursorStart << 8) | (cursorEnd & 0xff)));

    /* Work out cursor position. */
    colsOnScreen = sas_w_at_no_check(VID_COLS);
    videoLen = sas_w_at_no_check(VID_LEN);
    pageOffset = cursorPos->Y * colsOnScreen * 2 + (cursorPos->X << 1);
    cursorWord = (currentPage * videoLen + pageOffset) / 2;

#ifdef X86GFX
#line 2010
#error Unsupported excluded original configuration
#endif
#line 2011
    {

	/* Send cursor position to video ports. */
	outb((io_addr) port6845, R14_CURS_ADDRH);
	outb((io_addr) (port6845 + 1), (half_word) (cursorWord >> 8));
	outb((io_addr) port6845, R15_CURS_ADDRL);
	outb((io_addr) (port6845 + 1), (half_word) (cursorWord & 0xff));
    }

    /* Store cursor position in BIOS variables. */
    sas_storew_no_check(VID_CURPOS + (currentPage * 2),
			(word) ((cursorPos->Y << 8) | (cursorPos->X & 0xff)));

#ifdef X86GFX
#line 2024
#error Unsupported excluded original configuration
#endif
#line 2025
    {
#line 2026
#ifdef MONITOR
#line 2027
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif
#line 2029
        do_new_cursor();      /* make sure the emulation knows about it */
    }
}

VOID waitForInputFocus()
{
    if (WaitForSingleObject(sc.FocusEvent, INFINITE))
	ErrorExit();
#line 2048
}

/***************************************************************************
 * Function:								   *
 *	getModeType							   *
 *									   *
 * Description: 							   *
 *      Look up video mode to determine whether the VGA current mode is    *
 *      graphics or text.				   		   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	int - TEXT or GRAPHICS.						   *
 *									   *
 ***************************************************************************/
int getModeType(VOID)
{
    half_word mode;
    int modeType;

    mode = sas_hw_at_no_check(vd_video_mode);
    switch(mode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
    case 0x20:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
	modeType = TEXT;
	break;
    default:
	modeType = GRAPHICS;
	break;
    }
    return(modeType);
}

#ifdef X86GFX
#line 2095
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 2362

void host_enable_stream_io(void)
{
    sc.ScreenState = STREAM_IO;
    host_stream_io_enabled = TRUE;

}
void host_disable_stream_io(void)
{
    DWORD mode;

    if(!GetConsoleMode(sc.InputHandle, &mode))
	DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);

    mode |= (ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    if(!SetConsoleMode(sc.InputHandle,mode))
	  DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(), __FILE__,__LINE__);


    if(!GetConsoleMode(sc.OutputHandle, &mode))
	DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
    if(!stdoutRedirected)
    {
	mode &= ~(ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_PROCESSED_OUTPUT);

       if(!SetConsoleMode(sc.OutputHandle,mode))
	  DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(), __FILE__,__LINE__);
    }

    ConsoleInit();
    (void)(*choose_display_mode)();
    /*
    ** Copy the console buffer to the regen buffer.
    ** Don't want to adjust the copy from top of console window, console
    ** does it itself if we resize the window. Tim September 92.
    */
    copyConsoleToRegen(0, 0, VGA_WIDTH, (SHORT)ConVGAHeight);

    /*
    ** Tim September 92, adjust cursor position if console window size is
    ** adjusted.
    */
    ConsBufferInfo.dwCursorPosition.Y -= ConTopLine;

    /* Set up SoftPC's cursor. */
    setVDMCursorPosition((UTINY)StartupCharHeight,
				&ConsBufferInfo.dwCursorPosition);

#ifdef X86GFX
#line 2410
#error Unsupported excluded original configuration
#endif
#line 2411
        enableUpdates();

    MouseAttachMenuItem(sc.ActiveOutputBufferHandle);
    host_stream_io_enabled = FALSE;
}
