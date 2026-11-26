#include <windows.h>
#include "host_def.h"
#include "insignia.h"
#include "vdm.h"

/*
 * ==========================================================================
 *      Name:           nt_det.c
 *      Author:         Jerry Sexton
 *      Derived From:
 *      Created On:     6th August 1992
 *      Purpose:        This module contains the code for the thread which
 *                      detects transitions between windowed and full-screen.
 *
 *      (c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.
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
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 64

/*
 * ==========================================================================
 * Macros
 * ==========================================================================
 */
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 73
#define SUSP_FAILURE            0xffffffff


//
// A bunch of imports
//
extern DISPLAY_MODE choose_mode[];
#ifdef JAPAN
extern BOOL VDMForWOW;         // 32bit IME disable and enable for RAID #1085
#endif // JAPAN



/*
 * ==========================================================================
 * Global data
 * ==========================================================================
 */

/* Size of video save block. */
GLOBAL DWORD stateLength;

/* Video save block pointer. */
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 99
GLOBAL PVIDEO_HARDWARE_STATE_HEADER videoState;
#endif // NEC_98
GLOBAL PVOID textState; // Tim Oct 92.

/* Name of the shared video block. */
GLOBAL WCHAR_STRING videoSection;
GLOBAL WCHAR_STRING textSection; // Tim Oct 92

#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 119

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
LOCAL BOOL HandShakeInProgress = FALSE;

/* Storage for the frozen-window thread handle. */
LOCAL HANDLE freezeHandle = (HANDLE)0;

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 166


/*
 * ==========================================================================
 * Local function declarations
 * ==========================================================================
 */

#undef LOCAL
#define LOCAL

LOCAL VOID getCursorInfo(word *, half_word *, half_word *, half_word *);
LOCAL VOID setCursorInfo(word, half_word, half_word, half_word);
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 182
LOCAL VOID windowedToFullScreen(SHORT, BOOL);
#endif // !NEC_98
LOCAL VOID fullScreenToWindowed(VOID);
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 188
LOCAL VOID syncHardwareToVGAEmulation(SHORT);
#endif // !NEC_98
LOCAL VOID syncVGAEmulationToHardware(VOID);
LOCAL BOOL funnyMode(VOID);
LOCAL VOID freezeWindow(VOID);
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 197

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 202

#define ScreenSwitchErrorExit()  {               \
    if (HandShakeInProgress) {         \
        SetEvent(hErrorHardwareEvent); \
        HandShakeInProgress = FALSE;   \
    }                                  \
    ErrorExit();                       \
}
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
    COORD dummycoord = {0};

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 243
    if (!RegisterConsoleVDM( CONSOLE_UNREGISTER_VDM,
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
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 260

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 268
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
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 305
    sc.ScreenState = WINDOWED;
#endif

    if (!RegisterConsoleVDM( VDMForWOW ?
                             CONSOLE_REGISTER_WOW : CONSOLE_REGISTER_VDM,
#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#else
#line 315
                             NULL,
                             NULL,
                             NULL,
#endif
                             0,
                             &stateLength,
                             (PVOID *) &videoState,
                             NULL,            // sectionname no longer used
                             0,               // sectionnamelen no longer used
                             textBufferSize,
                             (PVOID *) &textBuffer
                           )
       )
        ErrorExit();

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 333
#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif
#line 343

} /* end initTextSection() */

#ifdef X86GFX

#error Unsupported excluded original configuration
#endif /* X86GFX */
#line 2045

#ifdef PLANEDUMPER
#error Unsupported excluded original configuration
#endif
#line 2095
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2618
