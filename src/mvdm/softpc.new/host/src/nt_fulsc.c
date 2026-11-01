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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif
#include <vdm.h>
#include <stdlib.h>
#include <string.h>
#include "mvdm_softpc_firmware.h"
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 77 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
GLOBAL BOOL blocked_in_gfx_mode = FALSE;  /* need to force text mode? */
#ifndef PROD
/* DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register. */
#endif /* PROD */

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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */


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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */

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
        /* DIVERGENCE(MVDM-HOST-DIV-322): CCPU updates in both routes. */
        if (sc.ScreenState == WINDOWED)
#endif
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


#ifdef X86GFX
/* 
* Find the address of the ROM font, load it up into the correct
* portion of the regen area and set Int 43 to point to it.
*
* Size of font we are loading is known, so don't listen to what
* the native BIOS returns to us in CX. BIOS might be returning
* character height we set in recalc_text() above. Tim Oct 92.
*/

NativeFontAddr nativeFontAddresses[6]; /* pointers to native BIOS ROM fonts */
                               /* 8x14, 8x8 pt1, 8x8 pt2, 9x14, 8x16 and 9x16 */


#define GET_BIOS_FONT_ADDRESS(FontIndex)			    \
	(((sys_addr)nativeFontAddresses[FontIndex].seg << 4) +	    \
	(sys_addr)nativeFontAddresses[FontIndex].off)

/*
*****************************************************************************
** locateNativeBIOSfonts() X86 only.
*****************************************************************************
** Get the addresses of the BIOS ROM fonts. (Insignia video ROM not loaded)
** ntdetect.com runs the INT 10 to look the addresses up on system boot and
** stores them in page 0 at 700.
** This function is called once on startup X86 only. It gets the addresses of
** the native ROM fonts and stores them in the nativeFontAddresses[] array.
*/
VOID locateNativeBIOSfonts IFN0()
{
    int i;
    sys_addr font_off = 0x700;	/* Magic place used by ntdetect.com */

    for(i = 0; i < 6; i++)
    {
	sas_loadw(font_off, &nativeFontAddresses[i].off);
	sas_loadw(font_off+2, &nativeFontAddresses[i].seg);
	font_off += 4;
    }
} /* end of locateNativeBIOSfonts() */

/*
****************************************************************************
** loadNativeBIOSfont() X86 only.
****************************************************************************
** Loads the appropriate font, specified by current window size, into the
** font area in video RAM.
** This function is called on every windowed startup and resume. *Never* on
** a full-screen startup or resume. The font is loaded so that it will be
** available for full-screen text mode, but easier to load when windowed.
** Remember a mode change will load the corect font.
*/
VOID loadNativeBIOSfont IFN1( int, vgaHeight )
{
    sys_addr fontadd;	// location of font
    UTINY *regenptr; 	// destination in video
    int cellsize;	// individual character size
    int skip;		// gap between characters
    int loop, pool;
    UINT OutputCP;


    /*
    ** ordered this way as 80x50 console is default
    **	VGA_HEIGHT_4 = 50
    **	VGA_HEIGHT_3 = 43
    **	VGA_HEIGHT_2 = 28
    **	VGA_HEIGHT_1 = 25
    **	VGA_HEIGHT_0 = 22
    */
    if (vgaHeight == VGA_HEIGHT_4 || vgaHeight == VGA_HEIGHT_3){
	cellsize = 8;
	fontadd = GET_BIOS_FONT_ADDRESS(F8x8pt1);
    }else
	if (vgaHeight == VGA_HEIGHT_2){
	    cellsize = 14;
	    fontadd = GET_BIOS_FONT_ADDRESS(F8x14);
	}else{
	    cellsize = 16;
	    fontadd = GET_BIOS_FONT_ADDRESS(F8x16);
	}

    // set Int 43 to point to font
    sas_storew(0x43 * 4, (word)(fontadd & 0xffff));
    sas_storew(0x43 * 4 + 2, (word)(fontadd >> 4 & 0xf000));

/* BUGBUG, williamh
   We should have set int43 to the new font read from the CPI font.
   This would require at least 4KB buffer in real mode address space.
   The question is who is going to use this vector? So far, we haven't found
   any applications use the vector(ROM BIOS is okay because the set video mode
   function will reset the font and our new font will be lost anyway).

*/

    if (!sc.Registered || (OutputCP = GetConsoleOutputCP()) == 437 ||
	!LoadCPIFont(OutputCP, (WORD)8, (WORD)cellsize)) {
	// now load it into the regen memory. We load it in at a0000 where
	// an app will have to get to it. Luckily, this means we don't 
	// conflict with the text on the screen

	skip = 32 - cellsize;

	regenptr = (half_word *)0xa0000;

	if (cellsize == 8)	/* 8x8 font comes in two halves */
        {
	    for (loop = 0; loop < 128; loop++)
	    {
	        for (pool = 0; pool < cellsize; pool++)
		    *regenptr++ = *(UTINY *)fontadd++;
	        regenptr += skip;
	    }
	    fontadd = GET_BIOS_FONT_ADDRESS(F8x8pt2);
	    for (loop = 0; loop < 128; loop++)
	    {
	        for (pool = 0; pool < cellsize; pool++)
		    *regenptr++ = *(UTINY *)fontadd++;
	        regenptr += skip;
	    }
        }
        else
        {
	    for (loop = 0; loop < 256; loop++)
	    {
	        for (pool = 0; pool < cellsize; pool++)
		    *regenptr++ = *(UTINY *)fontadd++;
	        regenptr += skip;
	    }
	}
    }
} /* end of loadNativeBIOSfont() */

/* this function loads font data from EGA.CPI file located at %systemroot%\system32.
   It is the same file console server used to load ROM font when the video
   is in full screen. This function covers code page 437(ROM default). However,
   the caller should make its best decision to call this function if the
   output code page is not 437. This function doesn't care what code page
   was provided.
   The font size are limitted to(an assumption made by nt video driver and
   the console server):
   ** width must be 8 pixels.
   ** Height must less or equal to 16 pixels.

*/



BOOL LoadCPIFont(UINT CodePageID, WORD FontWidth, WORD FontHeight)
{
    BYTE Buffer[16 * 256];
    DWORD dw, BytesRead, FilePtr;
    BYTE *VramAddr, *pSrc;
    DWORD nChars;
    PCPIFILEHEADER pCPIFileHeader = (PCPIFILEHEADER)Buffer;
    PCPICODEPAGEHEADER pCPICodePageHeader = (PCPICODEPAGEHEADER) Buffer;
    PCPICODEPAGEENTRY pCPICodePageEntry = (PCPICODEPAGEENTRY) Buffer;
    PCPIFONTHEADER pCPIFontHeader = (PCPIFONTHEADER) Buffer;
    PCPIFONTDATA   pCPIFontData   = (PCPIFONTDATA) Buffer;
    BOOL    bDOSCPI = FALSE;
    HANDLE hCPIFile;

    /* max font height is 16 pixels and font width must be 8 pixels */
    if (FontHeight > 16 || FontWidth != 8)
	return FALSE;
    dw = NtvdmGetSystemDirectoryA((CHAR *)Buffer, sizeof(Buffer));
    if (dw == 0 || dw + CPI_FILENAME_LENGTH > sizeof(Buffer))
	return FALSE;
    RtlMoveMemory(&Buffer[dw], CPI_FILENAME, CPI_FILENAME_LENGTH);
    // the file must be opened in READONLY mode or the CreateFileA will fail
    // because the console sevrer always keeps an opened handle to the file
    // and the file is opened READONLY.

    hCPIFile = CreateFileA(Buffer, GENERIC_READ, FILE_SHARE_READ,
			   NULL, OPEN_EXISTING, 0, NULL);
    if (hCPIFile == INVALID_HANDLE_VALUE)
	return FALSE;

    if (!ReadFile(hCPIFile, Buffer, sizeof(CPIFILEHEADER), &BytesRead, NULL) ||
	BytesRead != sizeof(CPIFILEHEADER)) {
	CloseHandle(hCPIFile);
	return FALSE;
    }
    if (memcmp(pCPIFileHeader->Signature, CPI_SIGNATURE_NT, CPI_SIGNATURE_LENGTH))
	{
	if (memcmp(pCPIFileHeader->Signature, CPI_SIGNATURE_DOS,CPI_SIGNATURE_LENGTH))
	    {
	    CloseHandle(hCPIFile);
	    return FALSE;
	}
	else
	    bDOSCPI = TRUE;
    }

    // move the file pointer to the code page table header
    FilePtr = pCPIFileHeader->OffsetToCodePageHeader;
    if (SetFilePointer(hCPIFile, FilePtr, NULL, FILE_BEGIN) == (DWORD) -1){
	CloseHandle(hCPIFile);
	return FALSE;
    }

    if (!ReadFile(hCPIFile, Buffer, sizeof(CPICODEPAGEHEADER), &BytesRead, NULL) ||
	BytesRead != sizeof(CPICODEPAGEHEADER)) {
	    CloseHandle(hCPIFile);
	    return FALSE;
    }
    // how many code page entries in the file
    dw = pCPICodePageHeader->NumberOfCodePages;
    FilePtr += BytesRead;

    // serach for the specific code page
    while (dw > 0 &&
	   ReadFile(hCPIFile, Buffer, sizeof(CPICODEPAGEENTRY), &BytesRead, NULL) &&
	   BytesRead == sizeof(CPICODEPAGEENTRY)) {
	    if (pCPICodePageEntry->CodePageID == CodePageID)
		break;
	    if (dw > 1) {
		if (!bDOSCPI)
		    FilePtr += pCPICodePageEntry->OffsetToNextCodePageEntry;
		else
		    FilePtr = pCPICodePageEntry->OffsetToNextCodePageEntry;

		if (SetFilePointer(hCPIFile, FilePtr, NULL, FILE_BEGIN) == (DWORD) -1) {
		    CloseHandle(hCPIFile);
		    return FALSE;
		}
	    }
	    dw--;
    }
    if (dw == 0) {
	CloseHandle(hCPIFile);
	return FALSE;
    }
    // seek to the font header for the code page
    if (!bDOSCPI)
	FilePtr += pCPICodePageEntry->OffsetToFontHeader;
    else
	FilePtr = pCPICodePageEntry->OffsetToFontHeader;
    if (SetFilePointer(hCPIFile, FilePtr, NULL, FILE_BEGIN) == (DWORD) -1) {
	CloseHandle(hCPIFile);
	return FALSE;
    }
    if (!ReadFile(hCPIFile, Buffer, sizeof(CPIFONTHEADER), &BytesRead, NULL) ||
	BytesRead != sizeof(CPIFONTHEADER)){
	CloseHandle(hCPIFile);
	return FALSE;
    }
    // number of fonts with the specific code page
    dw = pCPIFontHeader->NumberOfFonts;

    while(dw != 0 &&
	  ReadFile(hCPIFile, Buffer, sizeof(CPIFONTDATA), &BytesRead, NULL) &&
	  BytesRead == sizeof(CPIFONTDATA))
	{
	if (pCPIFontData->FontHeight == FontHeight &&
	    pCPIFontData->FontWidth == FontWidth)
	    {
	    nChars = pCPIFontData->NumberOfCharacters;
	    if (ReadFile(hCPIFile, Buffer, nChars * FontHeight, &BytesRead, NULL) &&
	       BytesRead == nChars * FontHeight)
		break;
	    else  {
		CloseHandle(hCPIFile);
		return FALSE;
	    }
	}
	else {
	    if (SetFilePointer(hCPIFile,
			       (DWORD)pCPIFontData->NumberOfCharacters * (DWORD)pCPIFontData->FontHeight,
			       NULL,
			       FILE_CURRENT) == (DWORD) -1) {
		CloseHandle(hCPIFile);
		return FALSE;
	    }
	    dw--;
	}
    }
    if (dw != 0) {
	VramAddr = (BYTE *)0xa0000;
	pSrc = Buffer;
	for(dw = nChars; dw > 0; dw--) {
	    RtlMoveMemory(VramAddr, pSrc, FontHeight);
	    pSrc += FontHeight;
	    // font in VRAM is always 32 bytes
	    VramAddr += 32;
	}
	return TRUE;
    }
    return FALSE;
}
#endif /* X86GFX */

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
    /* DIVERGENCE(MVDM-HOST-DIV-322): software fullscreen uses this VGA body. */
    if (sc.ScreenState == WINDOWED)
#endif
    {
	consoleWidth = ConsBufferInfo.srWindow.Right -
		       ConsBufferInfo.srWindow.Left + 1;
	consoleHeight = ConsBufferInfo.srWindow.Bottom -
			ConsBufferInfo.srWindow.Top + 1;
    }
#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif
#line 559 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
    /* DIVERGENCE(MVDM-HOST-DIV-322): retain CCPU scan/font/BIOS configuration. */
    if (sc.ScreenState == WINDOWED)
#endif
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif	/* X86GFX */

    }
#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 656 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
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

#ifdef X86GFX

    /* Software video initialization is performed after sas_init. */

    /*
     * Set emulation to a known state when starting up windowed. This has to
     * be done after the ROMS are mapped but before we start to look at
     * things like BIOS variables.
     */
    GfxReset();

#endif
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 714 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif //X86GFX
#line 760 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif
#line 825 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 884 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
    SMALL_RECT	newWin;
    BOOL itfailed = FALSE;

#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 891 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
#if 0                                                                           
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif	//Zero
#line 934 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */

#if 0
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif
#line 973 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */
	/*
	** Tim, September 92. Put the Console cursor to the same place as the
	** SoftPC cursor. We already do this in full-screen text mode below.
	** Specifcally to fix weird cursor position problem with 16-bit nmake,
	** but seems like a good safety idea anyway.
	*/
	getVDMCursorPosition();

        doNullRegister();   /* revert console back to ordinary window */

#ifdef X86GFX
    }
    else /* FULLSCREEN */
    {
	/*
	 * If SoftPC blocks in a text mode, sync console srceen buffer to regen
	 * area.
	 */
	if (getModeType() == TEXT)
	{
	    /* Get the current screen buffer info. */
	    if (!GetConsoleScreenBufferInfo(sc.OutputHandle, &bufferInfo))
		ErrorExit();

	    /* Get nearest screen size SetConsoleHardwareState will allow. */
	    tableLen = sizeof(validStates) / sizeof(HARDWARE_STATE);
	    for (mode = 0; mode < tableLen; mode++)
		if (validStates[mode].LinesOnScreen ==
		    bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1)
		    break;

	    /* Set it to 50 line mode if we had a funny number of lines. */
	    if (mode == tableLen)
	    {
		assert0(FALSE,
			"Non standard lines on blocking - setting 50 lines");
		mode = MODE_50_INDEX;
	    }

	/*
	** Tim September 92, if the console hardware state is the same as
	** we are about to set it, do not bother setting it.
	** This should stop the screen from flashing.
	*/
	if( !GetConsoleHardwareState(sc.OutputHandle,
					&screenRes,
					&fontSize) )
		assert1( NO,"VDM: GetConsHwState() failed:%#x",GetLastError() );

	    /* Sync the console to the regen buffer. */
	    currentPage = sas_hw_at_no_check(vd_current_page);
	    vga_misc_inb(0x3cc, &misc);
	    if (misc & 1)			// may be mono mode
	        videoLine = (half_word *) CGA_REGEN_START +
			(VIDEO_PAGE_SIZE * currentPage);
	    else
	        videoLine = (half_word *) MDA_REGEN_START +
			(VIDEO_PAGE_SIZE * currentPage);
	    to = consoleBuffer;
	    videoWidth   = sas_w_at_no_check(VID_COLS);
	    videoHeight  = (SHORT) (sas_hw_at_no_check(vd_rows_on_screen) + 1);
	    bufferSize.X = MAX_CONSOLE_WIDTH;
	    bufferSize.Y = MAX_CONSOLE_HEIGHT;
	    if (bufferSize.X * bufferSize.Y > MAX_CONSOLE_SIZE)
	    {
		assert1(FALSE, "Buffer size, %d, too large",
			bufferSize.X * bufferSize.Y);
		ErrorExit();
	    }
	    for (i = 0; i < videoHeight; i++)
	    {
		from = videoLine;
		for (j = 0; j < videoWidth; j++)
		{
		    to->Char.AsciiChar = *from++;
		    to->Attributes = *from++;
		    to++;
		}
		for (; j < (ULONG)bufferSize.X; j++)
		{
		    to->Char.AsciiChar = 0x20;
		    to->Attributes = 7;
		    to++;
		}
		videoLine += videoWidth * 2;
	    }
	    for (; i < (ULONG)bufferSize.Y; i++)
		for (j = 0; j < (ULONG)bufferSize.X; j++)
		{
		    to->Char.AsciiChar = 0x20;
		    to->Attributes = 7;
		    to++;
		}
	    bufferCoord.X = bufferCoord.Y = 0;
	    writeRegion.Left = writeRegion.Top = 0;
	    writeRegion.Right = bufferSize.X - 1;
	    writeRegion.Bottom = bufferSize.Y - 1;

            doNullRegister();   /* revert back to normal console */

	if( screenRes.X != validStates[mode].Resolution.X ||
	    screenRes.Y != validStates[mode].Resolution.Y ||
	    fontSize.X  != validStates[mode].FontSize.X   ||
	    fontSize.Y  != validStates[mode].FontSize.Y   ||
            sas_hw_at_no_check(VID_COLS) == 40 ||
            fontSize.Y  != sas_hw_at_no_check(ega_char_height))
	{
		/* Set up the screen. */                                      
		if( !SetConsoleHardwareState( sc.OutputHandle,                
			validStates[mode].Resolution,
			validStates[mode].FontSize) ){
			/*                                                    
			** Tim Sept 92, attempt a recovery.                   
			*/                                                    
			assert1( NO, "VDM: SetConsoleHwState() failed:%#x",
					GetLastError() );
		}                                                             
	}


            /* put VDM screen onto console screen */
	    if (!WriteConsoleOutput(sc.OutputHandle,
				    consoleBuffer,
				    bufferSize,
				    bufferCoord,
				    &writeRegion))
		ErrorExit();

#if 0  //STF removed with new mouse stuff??
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif //STF
#line 1112 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */

	    /*
	     * Get the cursor position from the BIOS RAM and tell the
	     * console.
	     * Set up variables getVDMCursorPosition() needs. Tim Jan 93.
	     */
	    sc.PC_W_Height = screenRes.Y;
	    sc.CharHeight  = fontSize.Y;
	    getVDMCursorPosition();
	}
	else /* GRAPHICS */
	{
	    /*
	    ** A tricky issue. If we were just in a full-screen graphics
	    ** mode, we are just about to lose the VGA state and can't get
	    ** it back very easily. So do we pretend we are still in the
	    ** graphics mode or pretend we are in a standard text mode?
	    ** Seems like standard text mode is more sensible. Tim Feb 93.
	    */
	    sas_store_no_check( vd_video_mode, 0x3 );
            blocked_in_gfx_mode = TRUE;

#if 0  //STF removed with new mouse stuff??
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif //STF
#line 1142 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */

	    /* Get the current screen buffer info. */
	    if (!GetConsoleScreenBufferInfo(sc.OutputHandle, &bufferInfo))
		ErrorExit();

	    /* Get nearest screen size SetConsoleHardwareState will allow. */
	    tableLen = sizeof(validStates) / sizeof(HARDWARE_STATE);
	    for (mode = 0; mode < tableLen; mode++)
		if (validStates[mode].LinesOnScreen ==
		    bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1)
		    break;

	    /* Set it to 50 line mode if we had a funny number of lines. */
	    if (mode == tableLen)
	    {
		assert0(FALSE,
			"Non standard lines on blocking - setting 50 lines");
		mode = MODE_50_INDEX;
	    }

	    /* Clear the console buffer. */
	    bufferSize.X = MAX_CONSOLE_WIDTH;
	    bufferSize.Y = MAX_CONSOLE_HEIGHT;
	    nChars = bufferSize.X * bufferSize.Y;
	    if (nChars > MAX_CONSOLE_SIZE)
	    {
		assert1(FALSE, "Buffer size, %d, too large", nChars);
		ErrorExit();
	    }
	    to = consoleBuffer;
	    while (nChars--)
	    {
		to->Char.AsciiChar = 0x20;
		to->Attributes = 7;
		to++;
	    }
	    bufferCoord.X = bufferCoord.Y = 0;
	    writeRegion.Left = writeRegion.Top = 0;
	    writeRegion.Right = MAX_CONSOLE_WIDTH-1;
	    writeRegion.Bottom = bufferSize.Y-1;

            doNullRegister();   /* revert back to normal console */

	    if (!WriteConsoleOutput(sc.OutputHandle,
				    consoleBuffer,
				    bufferSize,
				    bufferCoord,
				    &writeRegion))
		ErrorExit();

	    /* Set the cursor to the top left corner. */
	    cursorPos.X = 0;
	    cursorPos.Y = 0;
	    if (!SetConsoleCursorPosition(sc.OutputHandle, cursorPos))
		ErrorExit();
#ifndef PROD
/* DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register. */
#endif

	    /* Set up the screen. */
	    SetConsoleHardwareState(sc.OutputHandle,
				 	 validStates[mode].Resolution,
					 validStates[mode].FontSize);
	}
    /*
    ** Tim September 92, close graphics screen buffer on way out when in
    ** full-screen.
    */
    closeGraphicsBuffer();
    }
#endif /* X86GFX */

    /*Put console's cursor back to the shape it was on startup.*/
    SetConsoleCursorInfo(sc.OutputHandle, &StartupCursor);

    /* reset the current_* variables in nt_graph.c */
    resetWindowParams();
}


#ifdef X86GFX

/***************************************************************************
 * Function:								   *
 *	TextToGraphics							   *
 *									   *
 * Description: 							   *
 *	Handles transitions from text to graphics modes when running	   *
 *	windowed. Waits until the window has input focus and then requests *
 *	a transition to full-screen operation as graphics modes cannot be  *
 *	run in a window.						   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
GLOBAL VOID TextToGraphics(VOID)
{
    DWORD    flags;
    SAVED COORD    scrSize;

    /* Freeze until the window receives input focus. */
    //if (! sc.Focus )  //awaiting console fix.
    if (GetForegroundWindow() != hWndConsole)
    {

	FreezeWinTitle(); 	/* Add `-FROZEN' to the console title. */

	/* Now wait until input focus is received. */
	waitForInputFocus();

	UnFreezeWinTitle(); /* Remove frozen message */
    }

    /*
     * We are about to go full-screen but there will be a delay while the
     * detection thread does its hand-shaking. So disable screen writes before
     * we switch to prevent inadvertent updates while running full-screen.
     */
    disableUpdates();

    /*
     * Sanity Check for stress problem where forced fullscreen after INT 10
     * mode change has already started so can apparently go fullscreen twice.
     */
    if (!GetConsoleDisplayMode(&flags))
	ErrorExit();

    /* make sure we haven't been taken back fullscreen */
    if ((flags & CONSOLE_FULLSCREEN_HARDWARE) == CONSOLE_FULLSCREEN_HARDWARE)
    {
#ifndef PROD
/* DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register. */
#endif
	return;
    }

    /* check for iconified FS console. Only under extreme conditions (stress) */
    if ((flags & CONSOLE_FULLSCREEN) == CONSOLE_FULLSCREEN)
    {
        SetForegroundWindow(hWndConsole);    /* focus will send us back FS */
    }
    else    /* desktop window */
    {
        /* The window now has input focus so request to go full-screen. */
	BiosModeChange = TRUE;
        if (!SetConsoleDisplayMode(sc.OutputHandle,
			       CONSOLE_FULLSCREEN_MODE,
                               &scrSize))
	{
            if (GetLastError() == ERROR_INVALID_PARAMETER)
	    {
                RcErrorDialogBox(ED_INITFSCREEN, NULL, NULL);
            }
            else
            {
                ErrorExit();
            }
	}
	BiosModeChange = FALSE;
    }
}

/***************************************************************************
 * Function:								   *
 *	CheckForFullscreenSwitch					   *
 *									   *
 * Description: 							   *
 *	Checks to see if there has been a transition between windowed and  *
 *	fullscreen and does any console calls necessary. This is called    *
 *	on a timer tick before the graphics tick code.			   *
 *									   *
 * Parameters:								   *
 *	None.								   *
 *									   *
 * Return value:							   *
 *	VOID								   *
 *									   *
 ***************************************************************************/
GLOBAL VOID CheckForFullscreenSwitch(VOID)
{
    half_word mode,
	      lines;

    if (sc.ScreenState == STREAM_IO)
	return;

    /*
     * Do any console calls relating to screen state changes. They have to be
     * done now as they cannot be done on the same thread as the screen switch
     * hand-shaking.
     */
    if (sc.ScreenState != savedScreenState)
    {
	if (sc.ScreenState == WINDOWED)
	{
	    if (sc.ModeType == TEXT)
	    {

		/* Remove frozen window indicator if necessary. */
		UnFreezeWinTitle();

#if 0  //STF removed with new mouse stuff??
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif //STF
#line 1352 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */

		/* Get Window to correct shape */
		textResize();

		/* Enable screen updates. */
		enableUpdates();

		/*
		 * Now get the image on the screen (timer updates currently
		 * disabled).
		 */
		(void)(*update_alg.calc_update)();
		
	    }
	}
	else /* FULLSCREEN */
	{
	    int cnt = 0; /* Counter to break out of the cursor off loop. */

	    /* Disable screen updates*/
	    disableUpdates();

            /* Disable mouse 'attached'ness if enabled */
             if (bPointerOff)
             {
                 PointerAttachedWindowed = TRUE;
                 MouseDisplay();
             }

#if 0 //STF removed with new mouse stuff
/* DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register. */
#endif
#line 1385 /* MVDM-HOST-DIV-497: preserve original diagnostic line numbers. */

	    /* Do mouse scaling */
	     mode = sas_hw_at_no_check(vd_video_mode);
	     lines = sas_hw_at_no_check(vd_rows_on_screen) + 1;
	     SelectMouseBuffer(mode, lines);

             /* force mouse */
             ica_hw_interrupt(AT_CPU_MOUSE_ADAPTER, AT_CPU_MOUSE_INT, 1);

	    /*
	     * Now turn off console cursor - otherwise can ruin screen trying to
	     * draw system's cursor. The VDM will have to worry about the mouse
	     * image.
	     */
	  //  while(ShowConsoleCursor(sc.OutputHandle, FALSE) >=0 && cnt++ < 200);
	} /* FULLSCREEN */

	/* Update saved variable. */
	savedScreenState = sc.ScreenState;
    }
    /* Delayed Client Rect query */
    if (DelayedReattachMouse)
    {
        DelayedReattachMouse = FALSE;
	MovePointerToWindowCentre();
    }
}

#endif /* X86GFX */
#line 1414 /* MVDM-HOST-DIV-497: preserve line numbers when X86GFX is excluded. */

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
GLOBAL UTINY getNtScreenState IFN0()
{
	return( (UTINY) sc.ScreenState );
}

#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 1484 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 1573 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

    // re-register with console for fullscreen switching.
    if( !RegisterConsoleVDM( VDMForWOW ?
                                CONSOLE_REGISTER_WOW : CONSOLE_REGISTER_VDM,
#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#else
#line 1581 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
			     NULL,
			     NULL,
#endif

                             NULL,            // sectionname no longer used
                             0,               // sectionname no longer used
			     &stateLength,
#ifndef X86GFX
			     &pDummy,
#else
/* DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register. */
#endif
                             NULL,            // sectionname no longer used
                             0,               // sectionname no longer used
			     textBufferSize,
			     (PVOID *) &textBuffer
			   )
    )
	ErrorExit();

#ifdef X86GFX
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#else
#line 1607 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */
	/*
	** Create graphics buffer if we need one. Tim Oct 92.
	*/
	if( sc.ModeType==GRAPHICS )
		graphicsResize();
#endif
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif
#line 1784 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */


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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif
#line 1865 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */


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

#ifndef X86GFX	// on MIPS we actually want to write to the VGA bitplanes.
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
/* DIVERGENCE MVDM-HOST-DIV-511: X86GFX alternative is not selected for this source; see mirror register. */
#endif
#line 1921 /* MVDM-HOST-DIV-511: preserve original diagnostic line numbers. */
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
    /* DIVERGENCE(MVDM-HOST-DIV-322): simulated CRTC still owns the cursor. */
    if (sc.ScreenState == WINDOWED)
#endif
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
    /* DIVERGENCE(MVDM-HOST-DIV-322): software fullscreen has emulated ports. */
    if (sc.ScreenState == WINDOWED)
#endif
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
    /* DIVERGENCE(MVDM-HOST-DIV-322): notify the software cursor owner. */
    if (sc.ScreenState == WINDOWED)
#endif
    {
#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif
        do_new_cursor();      /* make sure the emulation knows about it */
    }
}

VOID waitForInputFocus()
{
    if (WaitForSingleObject(sc.FocusEvent, INFINITE))
	ErrorExit();
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
/* DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register. */
#endif /* X86GFX */
#line 2379 /* MVDM-HOST-DIV-510: preserve original diagnostic line numbers. */

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
    /* DIVERGENCE(MVDM-HOST-DIV-322): stream-to-software-VGA keeps updates. */
    if (sc.ScreenState == WINDOWED)
#endif
        enableUpdates();

    MouseAttachMenuItem(sc.ActiveOutputBufferHandle);
    host_stream_io_enabled = FALSE;
}
