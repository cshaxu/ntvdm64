/*****************************************************************************
*          nt_unix.c - miscellaneous stuff that may be needed.               *
*          File derived from hp_unix.c by Philippa Watson.                   *
*                                                                            *
*          This version is written/ported for New Technology OS/2            *
*          by Andrew Watson                                                  *
*                                                                            *
*          Date pending due to ignorance                                     *
*                                                                            *
*          (c) Copyright Insignia Solutions 1991                             *
*                                                                            *
*****************************************************************************/

#include <windows.h>
#include "host_def.h"
#include "mvdm_softpc_firmware.h"
#include "insignia.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <sys\types.h>
#include "xt.h"
#include CpuH
#include "timeval.h"
#include "error.h"
#include "sas.h"
#include "spcfile.h"
#include "idetect.h"
#include "debug.h"
#include "nt_reset.h"
#include "nt_pif.h"
#include "mvdm_softpc_termination.h"


/*****************************************************************************
*    local #define used for stubbing out functions                           *
*****************************************************************************/

#define STUBBED 1

/****    externally visible global variable declarations                 ****/

/* Exported Data */
GLOBAL BOOL ExternalWaitRequest = FALSE;


/* Local Module Data */
HANDLE IdleEvent = NULL;
DWORD MainThreadId = 0;
BOOL NowWaiting = FALSE;

/*****************************************************************************
*    Function: host_get_system_error()                                       *
*    This routine processes an error returned by SoftPC.                     *
*    Returns a pointer to an error message (located in a table) that         *
*    corresponds to the error number passed as a parameter.                  *
*****************************************************************************/

LPSTR host_get_system_error(filename, line, error)
LPSTR  filename;
DWORD  line;
DWORD  error;
{
static  BYTE buf[256];
const char *message;

/* DIVERGENCE(MVDM-HOST-DIV-033): the NT4 CRT exported _sys_errlist and
 * _sys_nerr.  Modern UCRT no longer exposes that storage.  Keep this source
 * function's same char* error-text contract through its public successor. */
message = strerror((int)error);
if (message != NULL)
   return((LPSTR)message);
sprintf(buf, "System error %d occurred in %s (line %d)",
	 error, filename, line);
return(buf);
}


/* This section contains host side of idling system */

/*****************************************************************************
*    Function: host_idle_init()                                              *
* Create Event used in Idling Wait                                           *
*****************************************************************************/
void host_idle_init(void)
{
    if (IdleEvent != NULL)
	return;         //Called already 

    MainThreadId = GetCurrentThreadId();

    IdleEvent = CreateEvent((LPSECURITY_ATTRIBUTES) NULL, FALSE, FALSE, NULL);

#ifndef PROD
    if (IdleEvent == NULL)
	printf("NTVDM:Idlling Event creation failed. Err %d\n",GetLastError());
#endif
}

/*****************************************************************************
*    Function: WaitIfIdle()                                                  *
*  If no counter indications (video, com etc) then do short idle             *
*                                                                            *
*****************************************************************************/
void WaitIfIdle(void)
{
    DWORD idletime;

    if (IdleDisabledFromPIF) {
	return;
	}


    /*
     * If its not wow make sure the main thread also gets idled.
     */
    if (!VDMForWOW && GetCurrentThreadId() != MainThreadId)
    {
	ExternalWaitRequest = TRUE;
    }


    //
    // Check for invalid conditions
    //
    if (!IdleEvent || !IdleNoActivity) {
	Sleep(0);
	return;
	}

    NowWaiting = TRUE;
    idletime = ienabled ? 10 : 1;

    if (WaitForSingleObject(IdleEvent, idletime) == WAIT_FAILED) {
        Sleep(0);
        idletime = 0;
        }
    NowWaiting = FALSE;

#ifndef MONITOR
    if (idletime) {
        ActivityCheckAfterTimeSlice();
        }
#endif

}


/*****************************************************************************
*    Function: WakeUpNow()                                                   *
*  The paired counterpart to WaitIfIdle() - the event that was worth waiting *
*  for has arrived. Wake CPU up so it can deal with it.                      *
*****************************************************************************/
void WakeUpNow(void)
{
   HostIdleNoActivity();
}



/*  HostIdleNoActivity
 *
 *  Set Indicator that video\disk\com\lpt activity
 *  has happened and wake up sleeping CPU if appears to be sleeping.
 */
void HostIdleNoActivity(void)
{

    IdleNoActivity=0;

    /* DIVERGENCE(MVDM-HOST-DIV-208): NT4 used PulseEvent only while the
     * original CPU thread was observed waiting.  PulseEvent is explicitly
     * unreliable on current Win32: an activity notification in the small
     * interval before NowWaiting becomes visible is lost, leaving the CPU
     * asleep despite IdleNoActivity having been cleared.  IdleEvent was
     * already created as the source's auto-reset event.  SetEvent preserves
     * the same one-waiter wake contract, while retaining one pending wake
     * until the CPU reaches its next original WaitForSingleObject call. */
    if (IdleEvent != NULL)
	SetEvent(IdleEvent);
    /* DIVERGENCE(MVDM-HOST-DIV-219): paired scalar-only activity witness;
     * it is deliberately after the unchanged event signal. */
    mvdm_softpc_record_idle_activity(0u, NowWaiting ? 1u : 0u,
        IdleEvent != NULL ? 1u : 0u, 0u);
}



/*****************************************************************************
*    Function: host_release_timeslice()                                     *
*****************************************************************************/
void host_release_timeslice(void)
{
    DWORD idletime;
    DWORD wait_result;

    //
    // If there is counter idle activity no idling so return immediatly
    //
    if (!IdleNoActivity || IdleDisabledFromPIF) {
	return;
    }

    //
    // Check for invalid or unsafe conditions
    //
    if (!IdleEvent || !ienabled) {
	Sleep(0);
	return;
	}

    //
    // If pif Foreground priority is set to less than 100 on every timer
    // event PrioWaitIfIdle will do a wait, so use minimum delay here.
    //
    if (WNTPifFgPr < 100) {
        idletime = 0;
	}

    //
    // Normal idling condition, so use sig portion of 55 ms time tick
    //
    else {
        idletime = 25;
	}

    NowWaiting = TRUE;
    /* DIVERGENCE(MVDM-HOST-DIV-219): default-off observation after the
     * original DOS CON boundary, before and after the unchanged original
     * short idle wait. It reports only host-local scalar event state and
     * cannot change the event, CPU, PIC, BIOS, or guest path. */
    mvdm_softpc_record_idle_activity(1u, 1u, 1u, 0u);
    wait_result = WaitForSingleObject(IdleEvent, idletime);
    if (wait_result == WAIT_FAILED) {
        idletime = 0;
	Sleep(0);
        }
    NowWaiting = FALSE;
    mvdm_softpc_record_idle_activity(2u, 0u, 1u, wait_result);

#ifndef MONITOR
    if (idletime) {
        ActivityCheckAfterTimeSlice();
        }
#endif



}



/*****************************************************************************
*    Function: PrioWaitIfIdle(Percentage)                                    *
*  unsigned char Percentage - Percent of cpu usage desired
*  The smaller the number the bigger the delay time
*
*****************************************************************************/
void PrioWaitIfIdle(unsigned char Percentage)
{
    DWORD idletime;


    //
    // If there is counter idle activity no idling so return immediatly
    //
    if (!IdleNoActivity) {
	return;
    }

    //
    // Check for invalid conditions
    //
    if (!IdleEvent) {
	Sleep(0);
	return;
	}


    idletime = (100 - Percentage) >> 1; // percent of 55ms time tick


    //
    // If idle is disabled, we can't depend on the IdleNoActivity flag
    // or if the delay time is less than the system's time slice
    // shorten the idle so we don't oversleep
    //
    if (!ienabled)
	idletime >>= 2;

    if (idletime < 10)
	idletime >>= 1;

    if (idletime) {
	NowWaiting = TRUE;
        if (WaitForSingleObject(IdleEvent, idletime) == WAIT_FAILED) {
            idletime = 0;
	    Sleep(0);
	    }
	NowWaiting = FALSE;
	}
    else {
	Sleep(0);
        }

#ifndef MONITOR
    if (idletime) {
        ActivityCheckAfterTimeSlice();
        }
#endif

}




/*****************************************************************************
*    function: host_memset()                                                 *
*    This function does what the traditional memset standard library function*
*    does ... i.e. fills a portion of memory with the character represented  *
*    in val.                                                                 *
*    Returns nothing.                                                        *
*****************************************************************************/

void host_memset(addr, val, size)
register char * addr;
register char val;
unsigned int size;
{
memset(addr, val, size);
}



#ifdef NO_LONGER_USED
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 * Host-specific equivalent of the Unix function localtime().
 ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

struct host_tm *host_localtime(clock)
long *clock;
{
    SYSTEMTIME now;
    SAVED struct host_tm host_now;

    UNUSED(clock);
    GetLocalTime(&now);
    host_now.tm_sec = (int) now.wSecond;
    host_now.tm_min = (int) now.wMinute;
    host_now.tm_hour = (int) now.wHour;
    host_now.tm_mday = (int) now.wDay;
    host_now.tm_mon = (int) now.wMonth - 1;    // Unix has 0 based months,NT 1
    host_now.tm_year = (int) now.wYear;
    host_now.tm_wday = (int) now.wDayOfWeek;

    host_now.tm_yday = (int) 0;     // the base doesn't require these.
    host_now.tm_isdst = (int) 0;
    return(&host_now);
}
#endif

/*
 * Host-specific equivalent of the Unix function time().
 */

long host_time(tloc)
long *tloc;
{
    UNUSED(tloc);
    return((long) GetTickCount() / 1000 );
}

/*
 * Check that the file is a character special device.
 */
boolean host_file_is_char_dev(path)
char *path;
{
    return(FALSE);
}


/*
 * Looks for a given file name in the 'ntvdm' subdirectory of the
 * windows system directory. The full path to the first one found is returned
 * in the 'full_path' variable, and as the result of the function.
 */
char *host_find_file(file,full_path,display_error)
char *file,*full_path;
int display_error;
{
    /* DIVERGENCE: the NT4 system-directory resource assumption is not a
     * portable product boundary.  Preserve this original function's name,
     * parameters and host_error failure ordering; source the selected immutable
     * firmware directory from the currently bound session through the named
     * SoftPC adapter.  See MVDM-HOST-DIV-037. */
    if (mvdm_softpc_firmware_find_file(file, full_path, MAXPATHLEN))
        return(full_path);

    /* Haven't managed to find the file. Oh dear... */
    switch( display_error )
    {
	case SILENT:
	    return( NULL );
	    break;

	case STANDARD:
	case CONT_AND_QUIT:
	    host_error(EG_SYS_MISSING_FILE, ERR_CONT | ERR_QUIT, file);
	    break;

	default:
	    host_error(EG_SYS_MISSING_FILE, ERR_QUIT, file);
	    break;
    }

    return (NULL);
}



//
//  this stuff needs to be removed
//
static char temp_copyright[] = "SoftPC-AT Version 3\n\r(C)Copyright Insignia Solutions Inc. 1987-1992";

static int block_level = 0;

GLOBAL void host_block_timer()
{
    if(block_level) return;
    block_level++;
}

GLOBAL void host_release_timer()
{
    block_level=0;
}
GLOBAL CHAR * host_get_years()
{
return ("1987 - 1992");
}
GLOBAL CHAR * host_get_version()
{
return("3.00");
}
GLOBAL CHAR * host_get_unpublished_version()
{
return("");
}
GLOBAL CHAR * host_get_copyright()
{
return("");
}
