#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
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
#line 32
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 34
#include <vdm.h>
#include <stdlib.h>
#include <string.h>
#line 37
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
#if defined(NEC_98)
#line 70
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 76

#if defined(X86GFX) && (defined(JAPAN) || defined(KOREA))
#include "sim32.h"
LOCAL   void CallVDM(word CS, word IP);
#endif // X86GFX && (JAPAN || KOREA)
/*
 * ==========================================================================
 * Global Data
 * ==========================================================================
 */
GLOBAL BOOL	ConsoleInitialised = FALSE;
GLOBAL BOOL	ConsoleNoUpdates = FALSE;
#ifdef X86GFX
#line 89
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 93
GLOBAL BOOL blocked_in_gfx_mode = FALSE;  /* need to force text mode? */
#line 94
#ifndef PROD
#line 95
#error DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 97

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
#if defined(NEC_98)
#line 123
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 125

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

#if defined(JAPAN) || defined(KOREA)
// #3086: VDM crash when exit 16bit apps of video mode 11h -yasuho
LOCAL half_word saved_video_mode = 0xFF; // save previous video mode
#endif  // JAPAN || KOREA

#if defined(NEC_98)
#line 157
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 192
/* Variable to check for changes in screen state. */
GLOBAL DWORD savedScreenState;
BOOL nt_init_called = 0;

IMPORT CONSOLE_CURSOR_INFO StartupCursor;

IMPORT void low_set_mode(int);
IMPORT VOID recalc_text(int);
IMPORT VOID enable_gfx_update_routines(VOID);
IMPORT VOID disable_gfx_update_routines(VOID);
#ifdef X86GFX
#line 203
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#if defined(NEC_98)
#line 206
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 212
#if defined(JAPAN) || defined(KOREA)
#ifdef i386
#define CONSOLE_BUFSIZE (80*50*2*2)
extern byte FromConsoleOutput[];
extern int FromConsoleOutputFlag;
IMPORT word FullScreenResumeSeg;
IMPORT word FullScreenResumeOff;
IMPORT sys_addr mouseCFsysaddr;
#endif // i386

IMPORT BOOL CurNowOff;  // mskkbug #2002: lotus1-2-3 display garbage -yasuho
IMPORT word textAttr;   // Console attributes will be taken over to VDM.
#endif // JAPAN || KOREA
/*
 * ==========================================================================
 * Local Function Declarations
 * ==========================================================================
 */
#if defined(NEC_98)
#line 231
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 234
VOID enableUpdates(VOID);
VOID disableUpdates(VOID);
VOID copyConsoleToRegen(SHORT, SHORT, SHORT, SHORT);
VOID getVDMCursorPosition(VOID);
VOID setVDMCursorPosition(UTINY, PCOORD);
VOID waitForInputFocus(VOID);
GLOBAL int getModeType(VOID);
#ifdef X86GFX
#line 242
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#if defined(NEC_98)
#line 246
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 254
GLOBAL VOID calcScreenParams IFN2( USHORT *, pCharHeight, USHORT *, pVgaHeight );

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

#if !defined(i386) && defined(JAPAN)
  // for $ias.sys to display the status on bottom line.
  if( !is_us_mode() ){
    CHAR_INFO   Buffer[80];
    COORD       bufSize,  bufCoord;
    SMALL_RECT  writeRegion;
    register PCHAR_INFO buf = Buffer;
    register half_word  *plane = get_screen_ptr(80*24*4); //bottom line
    register int nChars = 80;

    while (nChars--)
      {
        buf->Char.AsciiChar = *plane++;
        buf->Attributes = *plane++;
        buf++;
        plane += 2;
      }

    bufSize.X = 80;
    bufSize.Y = 1;
    bufCoord.X = 0;
    bufCoord.Y = 0;
    writeRegion.Left = 0;
    writeRegion.Top = 24;
    writeRegion.Right = 79;
    writeRegion.Bottom = 24;
    WriteConsoleOutput(sc.OutputHandle,
                       Buffer,
                       bufSize,
                       bufCoord,
                       &writeRegion);
  }
#endif // !i386 && JAPAN

#ifndef NEC_98
    if (sc.ScreenState != STREAM_IO) {
#endif // !NEC_98
#line 312
        USHORT dummy1, dummy2;

        //
        // Force native bios fonts to be reloaded.  On ConsoleInit, the native
        // bios fonts were loaded into 0xa0000.  But, after we get here, some
        // program/driver may trash it.  So, we need to reload it again. In case
        // the user switches to fullscreen before nt_resume_event_thread is called.
        //

        calcScreenParams (&dummy1, &dummy2);

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
        ConsBufferInfo.dwCursorPosition.Y -= (SHORT)ConTopLine;


#if defined(JAPAN) && !defined(NEC_98)
    // mskkbug#3704: DOS/V messages are not cleared when command.com starts
    // 11/14/93 yasuho  12/8/93 yasuho
    // Don't set cursor position on startup
    if (!is_us_mode()) {
        ConsBufferInfo.dwCursorPosition.X = sas_hw_at_no_check(VID_CURPOS);
        ConsBufferInfo.dwCursorPosition.Y = sas_hw_at_no_check(VID_CURPOS+1);
    }
#endif  //JAPAN & !NEC_98
#line 346
	/* Set up SoftPC's cursor. */
	setVDMCursorPosition((UTINY)StartupCharHeight,
					&ConsBufferInfo.dwCursorPosition);
                      
#line 351
		enableUpdates();
#ifndef NEC_98
    }
    else
	enableUpdates();
#endif // !NEC_98
#line 357

    // set kbd state flags in biosdata area, according to the real kbd Leds
    if (!VDMForWOW) {
        SyncBiosKbdLedToKbdDevice();
	// we have sync up the BIOS led states with the system, we now let the
	// event thread go
	ResumeThread(ThreadInfo.EventMgr.Handle);
        }

    KbdResume(); // JonLe Mod
}


#line 370
#ifdef X86GFX
#line 371
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 697

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
#line 722
    {
	consoleWidth = ConsBufferInfo.srWindow.Right -
		       ConsBufferInfo.srWindow.Left + 1;
	consoleHeight = ConsBufferInfo.srWindow.Bottom -
			ConsBufferInfo.srWindow.Top + 1;
    }
#ifdef X86GFX
#line 729
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 739

#if defined(NEC_98)
#line 741
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 745
    /*
     * Set the display to the nearest VGA text mode size, which is one of
     * 80x22, 80x25, 80x28, 80x43 or 80x50.
     */
#if defined(JAPAN) || defined(KOREA)
    // Japanese mode is only 25 lines, now.
    if ( is_us_mode() && ( GetConsoleOutputCP() == 437 ) ) {
#endif // JAPAN || KOREA
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

#if defined(JAPAN) || defined(KOREA)
    // Japanese mode is only 25 lines, now.  for RAID #1429
    }
    else {
        /* 25 lines */
        vgaHeight = VGA_HEIGHT_1;
        scanLines = 474; // change from 399
        charHeight = 19; // change from 16
#ifdef JAPAN_DBG
        DbgPrint( "NTVDM: calcScreenParams() Set Japanese 25line mode\n" );
#endif
        // Get Console attributes
        textAttr = ConsBufferInfo.wAttributes;
    }
#endif // JAPAN || KOREA
#endif // !NEC_98
#line 806
    {
#ifndef NEC_98
        /* The app may have shutdown in a gfx mode - force text mode back */
        if (blocked_in_gfx_mode)
        {
            low_set_mode(3);
            inb(EGA_IPSTAT1_REG,&temp);
            outb(EGA_AC_INDEX_DATA, EGA_PALETTE_ENABLE);   /* re-enable video */
            (*choose_display_mode)();
            blocked_in_gfx_mode = FALSE;
        }
#endif // !NEC_98
#line 818
	/*
	 * Set screen height appropriately for current window size.
	 * Now call video routine to set the character height, updating the
	 * BIOS RAM as it does so.
	 */	
	if (charHeight != get_char_height())
	{
	    extern void load_font(sys_addr, int, int, int, int);
	    load_font(charHeight == 8 ? EGA_CGDDOT :
	              charHeight == 14 ? EGA_CGMN : EGA_HIFONT,
	              256, 0, 0, charHeight);
	}
#line 823
	set_screen_height_recal( scanLines ); /* Tim Oct 92 */
	recalc_text(charHeight);

#ifndef NEC_98
        /* badly written apps assume 25 line mode page len is 4096 */
        if (vgaHeight == 25)
            sas_storew_no_check(VID_LEN, 0x1000);
#ifdef X86GFX
#line 831
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif	/* X86GFX */
#endif // !NEC_98
#line 834

    }
#ifdef X86GFX
#line 837
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 873
#ifndef NEC_98
    sas_storew_no_check(VID_COLS, 80);   // fixup from 40 char shutdown
#endif // !NEC_98
#line 876
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
#if defined(NEC_98)
#line 901
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 903

    note_entrance0("ConsoleInit");

#line 906
#ifdef X86GFX
#line 907
#error Unsupported excluded original configuration
#endif
#line 918
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
#line 931
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 936

    /*
     * We don't want to call paint routines until config.sys processed or if
     * the monitor is writing directly to the frame buffer (fullscreen), so...
     */
    disableUpdates();

#if defined(NEC_98)
#line 944
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 952
    /*
    ** Get console window size and set up our stuff accordingly.
    */
    calcScreenParams( &charHeight, &vgaHeight );

    StartupCharHeight = charHeight;
#ifdef X86GFX
#line 959
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif //X86GFX
#line 1010

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
#if defined(NEC_98)
#line 1027
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 1036
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
#line 1059
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 1087
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
#line 1108
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 1187

    PCHAR_INFO to;
    ULONG videoHeight,
	  nChars;
    COORD	bufferCoord,
		bufferSize;
    SMALL_RECT writeRegion;
#if defined(JAPAN) && defined(i386) && !defined(NEC_98)
    int skip = 0; // for mode 73h
#endif // JAPAN & i386 & !NEC_98
#line 1185


    SMALL_RECT	newWin;
    BOOL itfailed = FALSE;

#if (defined(JAPAN) || defined(KOREA)) && !defined(NEC_98)
    // #3086: VDM crash when exit 16bit apps of video mode 11h
    // 12/8/93 yasuho
    saved_video_mode = sas_hw_at_no_check(vd_video_mode);
#endif  // (JAPAN || KOREA) & !NEC_98
#line 1195
#if defined(NEC_98)
#line 1196
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 1201
#ifdef X86GFX
#line 1202
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 1205

	closeGraphicsBuffer();

#if defined(NEC_98)
#line 1209
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 1213
#ifndef NEC_98
#if !defined(JAPAN) && !defined(KOREA)
	if (StreamIoSwitchOn && !host_stream_io_enabled) {
#endif // !JAPAN && !KOREA
	    /* restore screen buffer and window size */
	    SetConsoleScreenBufferSize(sc.OutputHandle, sc.ConsoleBuffInfo.dwSize);
	    newWin.Top = newWin.Left = 0;
	    newWin.Bottom = sc.ConsoleBuffInfo.srWindow.Bottom -
			    sc.ConsoleBuffInfo.srWindow.Top;
	    newWin.Right = sc.ConsoleBuffInfo.srWindow.Right -
			   sc.ConsoleBuffInfo.srWindow.Left;
	    SetConsoleWindowInfo(sc.OutputHandle, TRUE, &newWin);
#if !defined(JAPAN) && !defined(KOREA)
	}
#endif // !JAPAN && !KOREA
#endif // !NEC_98
#line 1229
	/*                                                                      
	** Tim September 92, don't resize window on way out of DOS app cos      
	** MS (Sudeep) said so. Don't want to do the associated recalc_text()   
	** either.                                                              
	** This keeps the window re-sizing issue nice and simple, but there'll  
	** be people who don't like having a DOS window size forced upon them.  
	*/                                                                      
#line 1236
#if 0                                                                           
#line 1237
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif	//Zero
#line 1259

#if 0
#line 1266
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 1316
	/*
	** Tim, September 92. Put the Console cursor to the same place as the
	** SoftPC cursor. We already do this in full-screen text mode below.
	** Specifcally to fix weird cursor position problem with 16-bit nmake,
	** but seems like a good safety idea anyway.
	*/
	getVDMCursorPosition();

        doNullRegister();   /* revert console back to ordinary window */

#line 1326
#ifdef X86GFX
#line 1327
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 1736

    /*Put console's cursor back to the shape it was on startup.*/
    SetConsoleCursorInfo(sc.OutputHandle, &StartupCursor);
#if defined(JAPAN) || defined(KOREA)
    //  mskkbug#2002: lotus1-2-3 display garbage        9/24/93 yasuho
    CurNowOff = !StartupCursor.bVisible;        // adjust cursor state
#endif  // JAPAN

#ifndef NEC_98
    /* reset the current_* variables in nt_graph.c */
    resetWindowParams();
#endif // !NEC_98
#line 1748
}


#ifdef X86GFX
#line 1752
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 2111

/*
***************************************************************************
** getNtScreenState() - return 0 for windowed, 1 for full-screen.
***************************************************************************
** Tim July 92.
*/
#line 2118
GLOBAL UTINY getNtScreenState IFN0()
{
	return( (UTINY) sc.ScreenState );
}

#ifdef X86GFX
#line 2123
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 2176

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
#if defined(NEC_98)
#line 2201
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2207

#ifdef X86GFX
#line 2209
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 2310

    // re-register with console for fullscreen switching.
    if( !RegisterConsoleVDM( VDMForWOW ?
                                CONSOLE_REGISTER_WOW : CONSOLE_REGISTER_VDM,
#ifdef X86GFX
#line 2315
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 2319
                             NULL,
			     NULL,
			     NULL,
#endif
#line 2323

                             0,               // sectionname no longer used
			     &stateLength,
#line 2326
#ifndef X86GFX
#line 2327
			     &pDummy,
#else
#line 2329
#error DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register.
#endif
#line 2331
                             NULL,            // sectionname no longer used
                             0,               // sectionname no longer used
			     textBufferSize,
			     (PVOID *) &textBuffer
			   )
    )
	ErrorExit();

#if defined(NEC_98)
#line 2340
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2355
#if defined(NEC_98)
#line 2356
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2382
#ifdef X86GFX
#line 2383
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 2392
	/*
	** Create graphics buffer if we need one. Tim Oct 92.
	*/
	if( sc.ModeType==GRAPHICS )
		graphicsResize();
#endif
#if !defined(JAPAN) && !defined(KOREA) /* ???? NO REASON TO DO THIS STUFF TWICE ???????
                and if this is REALY necessary, we should count
                in the IME status line(s) (40:84) */
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
#if defined(NEC_98)
#line 2416
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 2419
	   setVDMCursorPosition( (UTINY)height,
	                         &ConsBufferInfo.dwCursorPosition);
#endif // !NEC_98
#line 2422
	   /* Copy the console buffer to the regen buffer. */
	   copyConsoleToRegen(0, 0, VGA_WIDTH, (SHORT)ConVGAHeight);
	}
#endif // !JAPAN && !KOREA

	/*
	** Get console window size and set up our stuff accordingly.
	*/
#if defined(NEC_98)
#line 2431
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 2433
#ifdef JAPAN
        // save BIOS work area 0x484 for $IAS
        {
            byte save;
#ifndef i386
            // for ichitaro
            static byte lines = 24;
#endif // !i386

            if ( !is_us_mode() ) {
                save = sas_hw_at_no_check( 0x484 );
#ifndef i386
                if( save < lines )
                  lines = save;
#endif // !i386
                calcScreenParams( &height, &vgaHeight );
#ifndef i386
                if( lines < sas_hw_at_no_check( 0x484 ))
                  sas_store_no_check( 0x484, lines );
#ifdef JAPAN_DBG
                DbgPrint(" NTVDM: DoFullScreenResume() set %d lines/screen\n",
                         sas_hw_at_no_check( 0x484 ) + 1 );
#endif
#else // i386
                sas_store( 0x484, save );
#endif // i386
            }
            else
                calcScreenParams( &height, &vgaHeight );
        }
#else // !JAPAN
        calcScreenParams( &height, &vgaHeight );
#endif // !JAPAN
#endif // !NEC_98
#line 2467

	/*
	** Window resize code copied out of nt_graph.c:textResize().
	*/
	{
	resizeWindow( 80, vgaHeight );
	}

	/* Copy the console buffer to the regen buffer. */
#if defined(NEC_98)
#line 2477
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 2479
#ifdef JAPAN
        // for $IAS, KKCFUNC
        if ( !is_us_mode() ) {
            SHORT rows;

            rows = sas_hw_at_no_check( 0x484 );

            if (rows+1 != vgaHeight && ConsBufferInfo.dwCursorPosition.Y>= rows+1 )
                copyConsoleToRegen(0, 1, VGA_WIDTH, (SHORT)(rows+1));
            else
                copyConsoleToRegen(0, 0, VGA_WIDTH, (SHORT)(rows+1));
#ifdef JAPAN_DBG
            DbgPrint( "NTVDM: copyConsoleToRegen (All)\n" );
#endif
        }
        else
            copyConsoleToRegen(0, 0, VGA_WIDTH, vgaHeight); // kksuzuka#4009
#else // !JAPAN
        copyConsoleToRegen(0, 0, VGA_WIDTH, vgaHeight);
#endif // !JAPAN
#endif // !NEC_98
#line 2500

	/*
	** Make sure cursor is not below bottom line.
	*/
#ifndef NEC_98
#ifdef JAPAN
        // scroll up if $IAS is loaded.
        if ( !is_us_mode() ) {
            byte rows;

            rows = sas_hw_at_no_check( 0x484 );
            if( ConsBufferInfo.dwCursorPosition.Y >= rows+1 ){
                ConsBufferInfo.dwCursorPosition.Y = rows;
#ifdef JAPAN_DBG
DbgPrint( "NTVDM: CursorPosition reset %d\n", rows );
#endif
            }
#ifdef JAPAN_DBG
DbgPrint(" NTVDM:DoFullScreenResume() set cur pos %d,%d\n", ConsBufferInfo.dwCursorPosition.X, ConsBufferInfo.dwCursorPosition.Y );
#endif
        }
        else
#endif // JAPAN
#endif // !NEC_98
#line 2524
	if( ConsBufferInfo.dwCursorPosition.Y >= vgaHeight ){
		ConsBufferInfo.dwCursorPosition.Y = vgaHeight-1;
	}
#if defined(NEC_98)
#line 2528
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 2531
	setVDMCursorPosition(( UTINY)height, &ConsBufferInfo.dwCursorPosition);
#endif // !NEC_98
#line 2533

#ifndef NEC_98
#if defined(JAPAN) || defined(KOREA)
#ifdef i386
    // #3741: WordStar6.0: Hilight color is changed after running in window
    // 11/27/93 yasuho
    // Also call VDM when in US mode, because we necessary restore the
    // palette and DAC registers
    if (sc.ScreenState == FULLSCREEN && FullScreenResumeSeg) {
        CallVDM(FullScreenResumeSeg, FullScreenResumeOff);
    }
#endif // i386
#endif // JAPAN
#endif // !NEC_98
#line 2547
#if defined(NEC_98)
#line 2548
#error Unsupported NEC PC-98 configuration
#endif // !NEC_98
#line 2556
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
#if 0  // Forzen window is no longer supported a-stzong 5/15/01
#ifdef X86GFX
#line 2579
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 2684

#endif

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

#ifndef NEC_98
    register half_word	*to;
#endif // !NEC_98
#line 2763
#ifdef X86GFX
#line 2764
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 2776
#if defined(NEC_98)
#line 2777
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2782


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

#line 2807
#ifndef X86GFX	// on MIPS we actually want to write to the VGA bitplanes.
#line 2808
    to = EGA_planes;
#if defined(JAPAN)
  // copy from beneath block and modified
  // save Console Output for MS-DOS/V
  // mode73h support
  {
    register sys_addr V_vram;

    // We now use DosvVramPtr to host extended attributes in video mode 73h.
    V_vram = DosvVramPtr;

    if ( !is_us_mode() && saved_video_mode == 0xff)
        goto skip_copy_console;

    if ( !is_us_mode() && sas_hw_at_no_check(DosvModePtr) == 0x73 ) {
        while (nChars--)
        {
            *to++ = from->Char.AsciiChar;
            *to++ = (half_word) from->Attributes;
            *to++ = (half_word)( (from->Attributes
                                           & ( COMMON_LVB_GRID_HORIZONTAL
                                             | COMMON_LVB_GRID_LVERTICAL
                                             | COMMON_LVB_REVERSE_VIDEO
                                             | COMMON_LVB_UNDERSCORE )
                                           ) >> 8 );
            *to++ = 0x00;

//          sas_store_no_check(V_vram++, from->Char.AsciiChar);
//          sas_store_no_check(V_vram++, (half_word) from->Attributes);
            sas_store_no_check(V_vram++, (half_word)( (from->Attributes
                                           & ( COMMON_LVB_GRID_HORIZONTAL
                                             | COMMON_LVB_GRID_LVERTICAL
                                             | COMMON_LVB_REVERSE_VIDEO
                                             | COMMON_LVB_UNDERSCORE )
                                           ) >> 8 ));
            sas_store_no_check(V_vram++, 0x00);

            from++;
        }
    }
    else {
        while (nChars--)
        {
            *to++ = from->Char.AsciiChar;
            *to   = (half_word) from->Attributes;

//          sas_store_no_check(V_vram++, from->Char.AsciiChar);
//          sas_store_no_check(V_vram++, (half_word) from->Attributes);

            from++;
            to += 3;
        }
    }
  }
  skip_copy_console:
#elif defined(KOREA) // !JAPAN
  {
    register sys_addr V_vram;

    V_vram = DosvVramPtr;

    if ( !is_us_mode() && saved_video_mode == 0xff)
        goto skip_copy_console;

        while (nChars--)
        {
            *to++ = from->Char.AsciiChar;
            *to   = (half_word) from->Attributes;

//          sas_store_no_check(V_vram++, from->Char.AsciiChar);
//          sas_store_no_check(V_vram++, (half_word) from->Attributes);

            from++;
            to += 3;
        }
  }
  skip_copy_console:
#else  // !KOREA
    while(nChars--)
    {
	*to++ = from->Char.AsciiChar;
	*to = (half_word) from->Attributes;
	from++;
	to += 7;	// CHAIN2: next character is eight plane bytes away.
    }
#endif // !KOREA
    host_mark_screen_refresh();
#else
#line 2896
#error DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register.
#endif
#line 3013
#if defined(NEC_98)
#line 3014
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 3091
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
#if defined(NEC_98)
#line 3111
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3114
    half_word currentPage;
    word cursorWord;
#endif // !NEC_98
#line 3117
    COORD cursorPos;
    BOOL setok;

#if defined(NEC_98)
#line 3121
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3135
    /* Get the current video page. */
    currentPage = sas_hw_at_no_check(vd_current_page);

    /* Store cursor position in BIOS variables. */
    cursorWord = sas_w_at_no_check(VID_CURPOS + (currentPage * 2));

    /* Set the console cursor. */
    cursorPos.X = (SHORT) (cursorWord & 0xff);
    cursorPos.Y = (cursorWord >> 8) & (SHORT) 0xff;

    if ((sc.CharHeight * cursorPos.Y) > sc.PC_W_Height)
	cursorPos.Y = (sc.PC_W_Height / sc.CharHeight) - 1;
#endif // !NEC_98
#line 3148

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
#ifndef NEC_98
#if defined(JAPAN) || defined(KOREA)
    if ( !is_us_mode() )
        height = 8;             // for dosv cursor
#endif // JAPAN || KOREA
#endif // !NEC_98
#line 3197
    cursorStart = height - (height * cursorInfo.dwSize / 100);
    if (cursorStart == height)
	cursorStart--;
    cursorEnd = height - 1;

#if defined(NEC_98)
#line 3203
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3207
    {

	/* Pass cursor size to video ports. */
	port6845 = sas_w_at_no_check(VID_INDEX);
	outb((io_addr) port6845, R10_CURS_START);
	outb((io_addr) (port6845 + 1), (half_word) cursorStart);
	outb((io_addr) port6845, R11_CURS_END);
	outb((io_addr) (port6845 + 1), (half_word) cursorEnd);
    }
#endif // !NEC_98
#line 3217

    /* Get the current video page. */
#if defined(NEC_98)
#line 3220
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3222
    currentPage = sas_hw_at_no_check(vd_current_page);
#endif // !NEC_98
#line 3224

#if defined(NEC_98)
#line 3226
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3229
    /* Set BIOS variables. */
    sas_storew_no_check(VID_CURMOD,
			(word) ((cursorStart << 8) | (cursorEnd & 0xff)));

    /* Work out cursor position. */
    colsOnScreen = sas_w_at_no_check(VID_COLS);
    videoLen = sas_w_at_no_check(VID_LEN);
#endif // !NEC_98
#line 3237
    pageOffset = cursorPos->Y * colsOnScreen * 2 + (cursorPos->X << 1);
#ifndef NEC_98
    cursorWord = (currentPage * videoLen + pageOffset) / 2;
#endif // !NEC_98
#line 3241

#if defined(NEC_98)
#line 3243
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 3254
    {

	/* Send cursor position to video ports. */
	outb((io_addr) port6845, R14_CURS_ADDRH);
	outb((io_addr) (port6845 + 1), (half_word) (cursorWord >> 8));
	outb((io_addr) port6845, R15_CURS_ADDRL);
	outb((io_addr) (port6845 + 1), (half_word) (cursorWord & 0xff));
    }
#endif // !NEC_98
#line 3263

    /* Store cursor position in BIOS variables. */
#ifndef NEC_98
    sas_storew_no_check(VID_CURPOS + (currentPage * 2),
			(word) ((cursorPos->Y << 8) | (cursorPos->X & 0xff)));
#endif // !NEC_98
#line 3269

#line 3270
    if (sc.ScreenState == WINDOWED)
    {
#line 3272
#ifdef MONITOR
#line 3273
#error DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register.
#endif
#line 3275
        do_new_cursor();      /* make sure the emulation knows about it */
    }
}

VOID waitForInputFocus()
{
    if (WaitForSingleObject(sc.FocusEvent, INFINITE))
	ErrorExit();
#line 3283
}

VOID AddTempIVTFixups()
{
#ifndef NEC_98
		   /* BOP        17,   IRET */
    UTINY code[] = { 0xc4, 0xc4, 0x17, 0xcf };

    //location is random but should be safe until DOS is initialised!!!
    sas_stores(0x40000, code, sizeof(code));	// new Int 17 code
    sas_storew(0x17*4, 0);			// Int 17h offset
    sas_storew((0x17*4) + 2, 0x4000);		// Int 17h segment
#endif // !NEC_98
#line 3296
}

#if defined(JAPAN) || defined(KOREA)
#ifdef X86GFX
/***************************************************************************
 * Function:                                                               *
 *      call 16bits subroutine                                             *
 *                                                                         *
 * Description:                                                            *
 *      This function makes necessary mode transition before calling       *
 *      16bits call.                                                       *
 *                                                                         *
 * Parameters:                                                             *
 *      word CS:IP is the 16bits code to be executed.                      *
 *      It should return with BOP 0xFE                                     *
 * Return value:                                                           *
 *      none                                                               *
 *                                                                         *
 ***************************************************************************/
LOCAL  void CallVDM(word CS, word IP)
{
#ifndef NEC_98

 /*****************************
 - williamh -
What we did here is:
(1). save current VDM context
(2). switch VDM context to real mode
(3). switch VDM stack to DOSX real mode stack
(4). set our real mode target to the VDM context
(5). execute the VDM
(6). switch stack to DOSX protected mode stack
(7). switch VDM context to protected mode
(8). restor VDM context

Don't ask me why. We don't have a generic software
interrupt simulation mechanism like Windows does.
***************************************************/


IMPORT void DpmiSwitchToRealMode(void);
IMPORT void DpmiSwitchToDosxStack(void);
IMPORT void DpmiSwitchFromDosxStack(void);
IMPORT void DpmiSwitchToProtectedMode(void);
WORD    OldAX, OldBX, OldCX, OldDX, OldSI, OldDI;
WORD    OldES, OldDS, OldSS, OldCS, OldGS, OldFS;
WORD    OldSP, OldIP, OldMSW;
    if (getMSW() & MSW_PE) {

        OldAX = getAX(); OldBX = getBX(); OldCX = getCX();
        OldDX = getDX(); OldSI = getSI(); OldDI = getDI();
        OldES = getES(); OldDS = getDS(); OldSS = getSS();
        OldCS = getCS(); OldGS = getGS(); OldFS = getFS();
        OldSP = getSP(); OldIP = getIP();
        OldMSW = getMSW();

        DpmiSwitchToRealMode();
        DpmiSwitchToDosxStack();
        setCS(CS);
        setIP(IP);
        host_simulate();
        DpmiSwitchFromDosxStack();
        DpmiSwitchToProtectedMode();
        setAX(OldAX); setBX(OldBX); setCX(OldCX);
        setDX(OldDX); setSI(OldSI); setDI(OldDI);
        setES(OldES); setDS(OldDS); setSS(OldSS);
        setCS(OldCS); setGS(OldGS); setFS(OldGS);
        setSP(OldSP); setIP(OldIP);
        setMSW(OldMSW);
    }
    else {
        OldCS = getCS();
        OldIP = getIP();
        setCS(CS);
        setIP(IP);
        host_simulate();
        setCS(OldCS);
        setIP(OldIP);
    }

#endif // !NEC_98
#line 3377
}
#endif  /* X86GFX */
#endif  /* JAPAN || KOREA*/

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
#ifndef NEC_98
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
#endif // !NEC_98
#line 3425
#if defined(NEC_98)
#line 3426
#error Unsupported NEC PC-98 configuration
#endif // !NEC_98
#line 3428
}

#ifdef X86GFX
#line 3431
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif /* X86GFX */
#line 3806

#ifndef NEC_98
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
    ConsBufferInfo.dwCursorPosition.Y -= (SHORT)ConTopLine;

    /* Set up SoftPC's cursor. */
    setVDMCursorPosition((UTINY)StartupCharHeight,
				&ConsBufferInfo.dwCursorPosition);

#line 3856
	enableUpdates();

    MouseAttachMenuItem(sc.ActiveOutputBufferHandle);
    host_stream_io_enabled = FALSE;
}
#endif // !NEC_98
#line 3862
#if defined(NEC_98)
#line 3863
#error Unsupported NEC PC-98 configuration
#endif

#if defined(NEC_98)
#line 3975
#error Unsupported NEC PC-98 configuration
#endif  // NEC_98
