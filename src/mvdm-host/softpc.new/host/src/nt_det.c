#include <windows.h>
#include "host_def.h"
#include "insignia.h"
#include "vdm.h"

/*
 * ==========================================================================
 *	Name:		nt_det.c
 *	Author:		Jerry Sexton
 *	Derived From:
 *	Created On:	6th August 1992
 *	Purpose:	This module contains the code for the thread which
 *			detects transitions between windowed and full-screen.
 *
 *	(c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.
 * ==========================================================================
 *
 * Modifications:
 *
 * Tim August 92. Full-screen and windowed transitions now switch between
 *     SoftPC video BIOS and host PC video BIOS.
 *
 */

/*
 * ==========================================================================
 * Other includes
 * ==========================================================================
 */
#include <stdlib.h>
#include <ntddvdeo.h>
#include "xt.h"
#include CpuH
#include "gmi.h"
#include "gvi.h"
#include "ios.h"
#include "sas.h"
#include "gfx_upd.h"
#include "egacpu.h"
#include "egaports.h"
#include "egamode.h"
#include "egagraph.h"
#include "video.h"
#include "conapi.h"
#include "host_rrr.h"
#include "debug.h"
#include "error.h"
#include "config.h"
#include "idetect.h"
#include "nt_uis.h"
#include "nt_fulsc.h"
#include "nt_graph.h"
#include "nt_mouse.h"
#include "nt_thred.h"
#include "nt_reset.h"
#include "nt_eoi.h"
#include "nt_event.h"

/*
 * ==========================================================================
 * Macros
 * ==========================================================================
 */
#define SUSP_FAILURE            0xffffffff


//
// A bunch of imports
//
extern DISPLAY_MODE choose_mode[];



/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

/* Size of video save block. */
GLOBAL DWORD stateLength;

/* Video save block pointer. */
GLOBAL PVIDEO_HARDWARE_STATE_HEADER videoState;
GLOBAL PVOID textState; // Tim Oct 92.

/* Name of the shared video block. */
GLOBAL WCHAR_STRING videoSection;
GLOBAL WCHAR_STRING textSection; // Tim Oct 92

#ifdef X86GFX
#line 92
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 96

/* Flag to stop host_simulates on this thread executing timer tick code. */
GLOBAL BOOL NoTicks;

/*
** Tim Oct 92.
** New strategy for windowed graphics updates. A shared buffer with Console
** will remove need to copy the new data over, just pass a rectangle co-ord
** instead. But we still need to copy into the buffer.
*/
GLOBAL PBYTE *textBuffer;
GLOBAL COORD  textBufferSize;      // Dimensions of the shared buffer

GLOBAL BOOL Frozen256Packed = FALSE;  // use packed 256 mode paint routine




/*
 * ==========================================================================
 * Local data
 * ==========================================================================
 */

/* Variable that indicates if we are in a non-standard VGA mode. */
LOCAL BOOL inAFunnyMode = FALSE;
LOCAL BOOL ModeSetBatch = FALSE;

/* Storage for the frozen-window thread handle. */
LOCAL HANDLE freezeHandle = (HANDLE)0;



/*
 * ==========================================================================
 * Local function declarations
 * ==========================================================================
 */

#undef LOCAL
#define LOCAL

LOCAL VOID getCursorInfo(word *, half_word *, half_word *, half_word *);
LOCAL VOID setCursorInfo(word, half_word, half_word, half_word);
LOCAL VOID windowedToFullScreen(SHORT, BOOL);
LOCAL VOID fullScreenToWindowed(VOID);
LOCAL VOID syncHardwareToVGAEmulation(SHORT);
LOCAL VOID syncVGAEmulationToHardware(VOID);
LOCAL BOOL funnyMode(VOID);
LOCAL VOID freezeWindow(VOID);
#line 146
#ifndef PROD
#line 147
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 150

/*
 * ==========================================================================
 * Global functions
 * ==========================================================================
 */

/*
** Tim Oct 92
** Centralised Console funx.
*/

GLOBAL VOID doNullRegister()
{
	DWORD dummylen;
        PVOID dummyptr;
        COORD dummycoord;

        if( !RegisterConsoleVDM( CONSOLE_UNREGISTER_VDM,
                                 NULL,
	                         NULL,
                                 NULL,
                                 0,
	                         &dummylen,
                                 &dummyptr,
	                         NULL,
	                         0,
	                         dummycoord,
                                 &dummyptr
	                       )
	  )
		ErrorExit();
#ifdef X86GFX
#line 183
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 185

}

/*
*******************************************************************
** initTextSection()
*******************************************************************
*/
GLOBAL VOID initTextSection(VOID)
{
    DWORD flags;

    //
    // VideoSection size is determined by nt video driver
    // TextSectionSize is 80 * 50 * BytesPerCharacter
    //     on risc BytesPerCharacter is 4 (interleaved vga planes)
    //     on x86  BytesPerCharacter is 2 (only char\attr)
    //
    textBufferSize.X = 80;
    textBufferSize.Y = 50;

#ifdef X86GFX
#line 207
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 223
    sc.ScreenState = WINDOWED;
#endif
#line 225

    if( !RegisterConsoleVDM( VDMForWOW ?
                                CONSOLE_REGISTER_WOW : CONSOLE_REGISTER_VDM,
#ifdef X86GFX
#line 229
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 232
			     NULL,
			     NULL,
#endif
#line 235
                             NULL,            // sectionname no longer used
                             0,               // sectionnamelen no longer used
	                     &stateLength,
                             (PVOID *) &videoState,
                             NULL,            // sectionname no longer used
                             0,               // sectionnamelen no longer used
	                     textBufferSize,
                             (PVOID *) &textBuffer
	                   )
    )
	ErrorExit();

#ifdef X86GFX
#line 248
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 253

} /* end initTextSection() */

#ifdef X86GFX
#line 257
#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 1559

#line 1560
#ifdef PLANEDUMPER
#line 1561
#error Unsupported excluded original configuration
#endif
#line 1612
