/*
 * SoftPC Revision 3.0
 *
 * Title                : NT reset functions
 *
 * Description  : This function is called by the standard reset function to
 * set up any specific devices used by the Sun4 implementation.
 *
 * Author               : SoftPC team
 *
 * Notes                :
 */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <vdm.h>
#include <vdmapi.h>
#include "insignia.h"
#include "host_def.h"

#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif

#include <sys\types.h>
#include "xt.h"
#include "sas.h"
#include "bios.h"
#include "keyboard.h"
#include "gmi.h"
#include "gfx_upd.h"
#include "gvi.h"
#include "mouse_io.h"
#include "error.h"
#include "config.h"
#include "host.h"
#include "timer.h"
#include "idetect.h"
#include CpuH
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <conapi.h>
#include "nt_timer.h"
#include "nt_graph.h"
#include "ntcheese.h"
#include "nt_uis.h"
#include "nt_com.h"
#include "nt_reset.h"
extern void ntvdm_worker_lifecycle_authorize_exit(void);
#line 51
#include "nt_event.h"
#include "nt_fulsc.h"
#include "nt_eoi.h"
#include "video.h"
#include "nt_thred.h"
#include "nt_sb.h"

VOID DeleteConfigFiles(VOID);  // from command.lib
void ShowStartGlass (DWORD);   // private user api

extern VIDEOFUNCS nt_video_funcs;
extern KEYBDFUNCS nt_keybd_funcs;
extern HOSTMOUSEFUNCS the_mouse_funcs;

#ifndef MONITOR
extern WORD BWVKey;
extern char achES[];
#endif

#ifndef NEC_98
#ifdef MONITOR
#error Excluded unsupported monitor IVT declarations
#endif
#endif // !NEC_98

extern IU8 lcifo[];

/*
 * Exported Data
 */
GLOBAL BOOL VDMForWOW = FALSE;
GLOBAL BOOL fSeparateWow = FALSE;  // TRUE if CREATE_SEPARATE_WOW_VDM flag
GLOBAL BOOL fMeowWOW = FALSE;      // TRUE if this is for MEOW
GLOBAL HANDLE MainThread;
GLOBAL ULONG DosSessionId = 0;
GLOBAL ULONG WowSessionId = 0;
GLOBAL UINT VdmExitCode = 0xFF;
#ifndef NEC_98
GLOBAL BOOL StreamIoSwitchOn = TRUE;
#endif // !NEC_98
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
static BOOL HostApplicationActive = FALSE;

#line 99
/*
 *
 * ============================================================================
 * External functions
 * ===========================================================================
 * =
 */

void
host_reset()
{

#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif

#ifndef NEC_98
    if (host_stream_io_enabled) {
        sc.ScreenState = STREAM_IO;
        ConsoleInitialised = TRUE;
    }
    else {
#endif // !NEC_98

        ConsoleInit();
        MouseAttachMenuItem(sc.ActiveOutputBufferHandle);
        /*::::::::::::::::::::::::::::::::::::::::::::::::: Enable idle detect */
#ifndef NEC_98
    }
#endif // !NEC_98

#ifdef MONITOR
#error Excluded original conditional branch
#endif  /* MONITOR */


#line 145
    //
    // Let the heartbeat thread run and release the ica lock,
    // on x86 needed now, for fullscreen switch notification.
    //
    ResumeThread(ThreadInfo.HeartBeat.Handle);

#ifdef MONITOR
#error Excluded unsupported monitor ICA initialization wait
#endif

    host_ica_unlock();

#ifdef  HUNTER
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#else   /* ! ( HUNTER ) */
    if (sc.ScreenState == FULLSCREEN)   // initialised in ConsoleInit()
        IDLE_ctl(FALSE);
    else
        IDLE_ctl(TRUE);         // can't idle detect fullscreen

    host_idle_init();           // host sleep event creation
#endif  /* HUNTER */


#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 173
}


/*
 * =========================================================================
 *
 * FUNCTION             : host_applInit
 *
 * PURPOSE              : Sets up the keyboard, lpt and error panels.
 *
 * RETURNED STATUS      : None.
 *
 * DESCRIPTION  : Called from main.c.  The keyboard and other GWI pointer
 *                sets are initialised here. The command line arguments are
 *                parsed for those flags that need processing early (ie before
 *                config() is called).
 *
 * =======================================================================
 */
#define  HOUR_BOOST_FOR_WOW 20*1000     // 20 seconds

void  host_applInit(int argc,char *argv[])
{
    char        *psz;
    int  temp_argc = argc;
    HostApplicationActive = TRUE;
#line 198
    char         **temp_argv = argv;
    BOOL         bSwitchF = FALSE;
    ULONG        SessionId = 0;

    working_video_funcs = &nt_video_funcs;
    working_keybd_funcs = &nt_keybd_funcs;
    working_mouse_funcs = &the_mouse_funcs;

    // check that someone has'nt started ntvdm from the cmd prompt. In such a
    // case we should kill ntvdm as we support it only when createprocess
    // executes it. We are checking a this with the argc although lots of
    // fancy things can be done. If a user is bent upon running ntvdm directly
    // they can always fool this code. We are only checking the presence of
    // -f switch.


// Check if the VDM Is for WOW
// Check is for new console session
    while (--temp_argc > 0) {
        psz = *++temp_argv;
        if (*psz == '-' || *psz == '/') {
            psz++;

            if (*psz == 'f') {
                bSwitchF = TRUE;
                continue;
            }
#ifndef MONITOR
            //
            // Check for windowed graphics resize
            //
            if (*psz == 'E') {
               int i;

               i = strlen(achES);
               if (!strncmp(psz, achES, i) && strlen(psz) == (size_t)i+2) {
                   psz += i;
                   BWVKey = (WORD)strtoul(psz, NULL, 16);
                   if (BWVKey > 0xFE)
                       BWVKey = 0;
               }
            }
            else
#endif
            if(tolower(*psz) == 'w') {

               VDMForWOW = TRUE;
               ++psz;
               if (tolower(*psz) == 's') { // VadimB: New code
                  fSeparateWow = TRUE;
               }
               ++psz;
               if (tolower(*psz) == 'm') { // Jarbats: Is it MEOW
                  fMeowWOW = TRUE;
               }
            }
#ifndef NEC_98
           else if (*psz == 'o'){
               StreamIoSwitchOn = FALSE;
            }
#endif // !NEC_98
            else if (*psz++ == 'i' && *psz) {
               SessionId = strtoul(psz, NULL, 16);
            }

        }
    }


    if (bSwitchF == FALSE)
        ExitProcess (0);

    // determine whether the id is for dos or for wow

    if (0 != SessionId) {
       if (VDMForWOW && !fSeparateWow) {
          WowSessionId = SessionId;
       }
       else {
          DosSessionId = SessionId;
       }
    }

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 328
    // If VDM Is for WOW keep showing the glass
    if (VDMForWOW) {
       ShowStartGlass (HOUR_BOOST_FOR_WOW);
#ifndef NEC_98
       StreamIoSwitchOn = FALSE;
#endif // !NEC_98
    }
#ifndef NEC_98
    else if (StreamIoSwitchOn)
            enable_stream_io();
#endif // !NEC_98

    /*
     * Get a handle to the main thread so it can be suspended during
     * hand-shaking.
     */
    DuplicateHandle(GetCurrentProcess(),
                    GetCurrentThread(),
                    GetCurrentProcess(),
                    &MainThread,
                    (DWORD) 0,
                    FALSE,
                    (DWORD) DUPLICATE_SAME_ACCESS);

    InitializeIcaLock();
    host_ica_lock();

    init_host_uis();    /* console setup */
    nt_start_event_thread();      /* Start event processing thread */
}


/*
 * =========================================================================
 *
 * FUNCTION             : host_applClose
 *
 * PURPOSE              : The last chance to close down.
 *
 * RETURNED STATUS      : None.
 *
 * DESCRIPTION  : Called from main.c.
 *
 *
 * =======================================================================
 */

void
host_applClose(void)
{
  ntvdm_worker_lifecycle_authorize_exit();
  if (!HostApplicationActive)
      return;
  HostApplicationActive = FALSE;
#line 378
  nt_remove_event_thread();
  InitSound(FALSE);
  SbCloseDevices();
  TerminateHeartBeat();

  GfxCloseDown();             // ensure video section destroyed
#ifdef X86GFX
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#endif // X86GFX



#line 391
  host_lpt_close_all();       /* Close all open printer ports */
  host_com_close_all();       /* Close all open comms ports */
  MouseDetachMenuItem(TRUE);  /* Force the menu item away on quit */

  DeleteConfigFiles();    // make sure temp config files are deleted
}




/*::::::::::::::::::::::::::::::::::::::::::::::::::: Closedown the VDM */


/*
 * host_terminate
 *
 * This function does not return it exits
 * Most of softpc has been shutdown by the time this
 * code is reached, as host_applClose has already been
 * invoked.
 *
 */
void host_terminate(void)
{

#ifdef HUNTER
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif /* HUNTER */

#line 421
    if(VDMForWOW)
        ExitVDM(VDMForWOW,(ULONG)-1);     // Kill everything for WOW VDM
    else
        ExitVDM(FALSE,0);

    ExitProcess(VdmExitCode);
}



/*  TerminateVDM - used by host to initiate shutdown
 *
 *  Request to start shutting down
 *
 */
VOID TerminateVDM(void)
{

    /*
     *  Do base sepcific cleanup thru terminate().
     *  NOTE: terminate will call host_applClose and host_terminate
     *        after doing base cleanup
     */
    terminate();
}





#ifdef NOTUSEDNOTUSED
#error Unsupported excluded original configuration
#endif
