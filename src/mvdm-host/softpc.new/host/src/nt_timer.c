/*                      INSIGNIA MODULE SPECIFICATION
                        -----------------------------


        THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
        CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
        NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
        AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.

DOCUMENT                :

RELATED DOCS            :

DESIGNER                : Dave Bartlett

REVISION HISTORY        :
First version           : 20 May 1991           Dave Bartlett

SUBMODULE NAME          : nt_timer

SOURCE FILE NAME        : nt_timer.c

PURPOSE                 : To provide the source of timing information
                          for the Win32 SoftPC, so that actions which
                          need to be taken at regular intervals may be
                          correctly scheduled.
*/


/*
[1.INTERMODULE INTERFACE SPECIFICATION]

[1.0 INCLUDE FILE NEEDED TO ACCESS THIS INTERFACE FROM OTHER SUBMODULES]

        INCLUDE FILE : nt_time.h

[1.1    INTERMODULE EXPORTS]

        PROCEDURES() :  int nt_timer_init()
                        int nt_timer_setup()
                        int nt_timer_shutdown()
                        int nt_timer_event()

-------------------------------------------------------------------------
[1.2 DATATYPES FOR [1.1]

        STRUCTURES/TYPEDEFS/ENUMS:

-------------------------------------------------------------------------
[1.3 INTERMODULE IMPORTS]

        PROCEDURES() :  do_key_repeats()                        (module keyboard)

-------------------------------------------------------------------------
=========================================================================
PROCEDURE                 :     int nt_timer_init()

PURPOSE           :     To initialise the host timing subsystem

PARAMETERS        :     none

GLOBALS           :     none

RETURNED VALUE    :     0  => failure
                          :     ~0 => success

DESCRIPTION       :     This function initialises the timing subsystem

ERROR INDICATIONS :     return value

ERROR RECOVERY    :     Timing subsystem has not been initialised
=========================================================================
PROCEDURE                 :     int nt_timer_setup()

PURPOSE           :     To start the host timing subsystem

PARAMETERS        :     none

GLOBALS           :     none

RETURNED VALUE    :     0  => failure
                          :     ~0 => success

DESCRIPTION       :     This function starts the timing subsystem

ERROR INDICATIONS :     return value

ERROR RECOVERY    :     Timing subsystem has not been started
=========================================================================
PROCEDURE                 :     int nt_timer_shutdown()

PURPOSE           :     To stop the host timing subsystem

PARAMETERS        :     none

GLOBALS           :     none

RETURNED VALUE    :     0  => failure
                          :     ~0 => success

DESCRIPTION       :     This function stops the timing subsystem

ERROR INDICATIONS :     return value

ERROR RECOVERY    :     Timing subsystem has not been stopped
=========================================================================
PROCEDURE                 :     int nt_timer_event()

PURPOSE           :     To indicate to the timing subsystem that a timer
                                event may now take place, and to cause any time-based
                                activities to occur.

PARAMETERS        :     none

GLOBALS           :     none

DESCRIPTION       :     All functions implementing time-based functions
                                are called if their turn has arrived.

ERROR INDICATIONS :     none

ERROR RECOVERY    :     errors are ignored
/*=======================================================================
[3.INTERMODULE INTERFACE DECLARATIONS]
========================================================================*/

/* [3.1 INTERMODULE IMPORTS] */

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: Include files*/
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <excpt.h>

#include "xt.h"
#include CpuH
#include "bios.h"
#include "sas.h"
#include "timer.h"
#include "tmstrobe.h"
#include "gmi.h"
#include "gfx_upd.h"
#include "timeval.h"
#include "timestmp.h"
#include "host_rrr.h"
#include "error.h"
#include "quick_ev.h"
#include "nt_timer.h"
#include "nt_uis.h"
#include "idetect.h"

#include "debug.h"
#line 162
#ifndef PROD
#line 163
#error DIVERGENCE MVDM-HOST-DIV-501: non-PROD debug branch is not selected; see mirror register.
#line 165
#endif
#line 166

#include "ica.h"
#include "nt_uis.h"
#include "nt_thred.h"
#include "nt_com.h"
#include <ntddvdeo.h>
#include "conapi.h"
#include "nt_fulsc.h"
#include "nt_graph.h"
#include "nt_det.h"
#include "nt_reset.h"
#include "nt_pif.h"
#include "nt_eoi.h"
#include "nt_event.h"



#line 182
/*::::::::::::::::::::::::::::::::::::::::::::::::::::: INTERMODULE EXPORTS */

#line 184
IMPORT void ReinitIdealTime(struct host_timeval *);
THREAD_DATA ThreadInfo;
CRITICAL_SECTION TimerTickCS;
CRITICAL_SECTION HBSuspendCS;


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::: Local Declarations */

DWORD Win32_host_timer(void);
NTSTATUS DelayHeartBeat(LONG Delay);
VOID  host_init_bda_timer(void);
GLOBAL void  rtc_init IFN0();
VOID InitPerfCounter(VOID);
DWORD HeartBeatThread(PVOID pv);
void CreepAdjust(LARGE_INTEGER DiffTime);
void DemHeartBeat(void);

#ifndef MONITOR
#line 204
void quick_tick_recalibrate(void);
#endif
#line 206

void rtc_init(void);
void RtcTick(struct host_timeval *time);

/*::::::::::::::::::::::::::::::::::::::::::::::: INTERNAL DATA DEFINITIONS */

//
// Perfcounter frequency calculation constants
//
ULONG ulFreqHusec;
ULONG ulFreqSec;


//
// Events for resuming\suspending heartbeat
//
HANDLE hHBResumeEvent;
HANDLE hHBSuspendEvent;

//
// HeartBeat TimeStamps in usec
//
LARGE_INTEGER CurrHeartBeat;
LARGE_INTEGER TimerEventUSec;
LARGE_INTEGER CumUSec;
LARGE_INTEGER CreepUSec;
LARGE_INTEGER CreepTicCount;

int    HeartBeatResumes=0;
BOOL   bDoingTicInterrupt=FALSE;
BOOL   bUpdateRtc;




#line 241
#if defined (MONITOR) && defined (X86GFX)
#line 242
#error Excluded unsupported monitor timer switch state
#endif
#line 244



#line 247
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::: NT timer initialise ::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#line 251
void host_timer_init(void)
{

    ThreadInfo.HeartBeat.Handle = CreateThread(
                                  NULL,
                                  8192,
                                  HeartBeatThread,
                                  NULL,
                                  CREATE_SUSPENDED,
                                  &ThreadInfo.HeartBeat.ID
                                  );

    if(!ThreadInfo.HeartBeat.Handle)  {
        DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
        TerminateVDM();
        }

    InitSound(TRUE);

    return;
}

/*
 *  TimerInit
 *
 *  Some of the timerinit stuff was split off, because it needs to be
 *  done before any chance of calling vdm error popups.
 *  Until I understand why creating the heartbeat thread very early
 *  causes a console-ntvdm deadlock, the functions should remain split
 *
 */
void TimerInit(void)
{

    if(!(hHBResumeEvent = CreateEvent(NULL, FALSE, FALSE, NULL))) {
        DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
        TerminateVDM();
        }

    if(!(hHBSuspendEvent = CreateEvent(NULL, FALSE, TRUE, NULL))) {
        DisplayErrorTerm(EHS_FUNC_FAILED,GetLastError(),__FILE__,__LINE__);
        TerminateVDM();
        }

    InitializeCriticalSection(&TimerTickCS);
    InitializeCriticalSection(&HBSuspendCS);

    InitPerfCounter();

}


/*
 *  HeartBeat Termination
 *
 */
void TerminateHeartBeat(void)
{
    NtAlertThread(ThreadInfo.HeartBeat.Handle);
    if (ThreadInfo.HeartBeat.ID != GetCurrentThreadId())
        WaitForSingleObjectEx(ThreadInfo.HeartBeat.Handle, 10000, TRUE);

    return;
}


//
//  Initialized by base, initialize frequencies for perf counter
//
VOID InitPerfCounter(VOID)
{
  LARGE_INTEGER li, liFreq;


  NtQueryPerformanceCounter(&li, &liFreq);
  /* we assumed the frequency never goes beyond 4Ghz(32bits)
   * if it does someday, this assumption must be removed
   * and code must be rewritten
   */
  ASSERT(liFreq.HighPart == 0);
  ulFreqSec = liFreq.LowPart;
  ulFreqHusec = liFreq.LowPart / 10000;

}

//
// returns perf counter in 100's usecs (0.1 millisec)
//
//
ULONG GetPerfCounter(VOID)
{
  LARGE_INTEGER li;

  NtQueryPerformanceCounter(&li, NULL);
  li = RtlExtendedLargeIntegerDivide(li, ulFreqHusec, NULL);
  return(li.LowPart);
}



//
// returns perf counter in usec
//
//
void GetPerfCounterUsecs(struct host_timeval *time, PLARGE_INTEGER pliTime)
{
  LARGE_INTEGER liSecs;
  LARGE_INTEGER liUsecs;

    // get time in secs and usecs
  NtQueryPerformanceCounter(&liSecs, NULL);
  liSecs = RtlExtendedLargeIntegerDivide(liSecs, ulFreqSec, &liUsecs.LowPart);
  liUsecs.QuadPart = Int32x32To64(liUsecs.LowPart, 1000000);
  liUsecs = RtlExtendedLargeIntegerDivide(liUsecs, ulFreqSec, NULL);

    // fill in time if specified
  if (time) {
      time->tv_usec = liUsecs.LowPart;
      time->tv_sec  = liSecs.LowPart;
      }

    // fill in pliTime if specified
  if (pliTime) {
      pliTime->QuadPart = liUsecs.QuadPart + liSecs.QuadPart * 1000000;
      }
  return;
}



/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::: Timer Event Code :::::::::::::::::::::::::*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::;::::::::::::::::*/
void host_timer_event()
{

    if (!VDMForWOW)  {
        unsigned char FgBgPriority;

#ifdef X86GFX
#line 391
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#line 398
#endif /* X86GFX */
#line 399

        host_graphics_tick();               // video graphics stuff


#ifndef X86GFX
         /* Are there any screen scale events to process. */
         GetScaleEvent();
#endif

        IDLE_tick();                        // IDLE accounting

        /*
         * We can't detect idling on all apps (eg Multiplan). For these apps
         * a PIF setting for 'Foreground Priority' of < 100% is recomended.
         * Where this happens, we idle for the 'unwanted' portion of a tick
         * period.
         */
        FgBgPriority  = sc.Focus ? WNTPifFgPr : WNTPifBgPr;
        if (FgBgPriority < 100)
            PrioWaitIfIdle(FgBgPriority);
        }

#ifndef MONITOR
#line 422
   quick_tick_recalibrate();
#endif
#line 415



#line 427
#ifdef YODA
#line 428
#error Excluded unsupported YODA configuration
#endif
#line 430

    host_com_heart_beat();              //  com  device

    host_lpt_heart_beat();              //  printer devuce

    host_flpy_heart_beat();             //  direct floppy device

    DemHeartBeat();

    time_strobe();                      // time/date etc. (NOT time ticks)

    PlayContinuousTone();               // sound emulation

}


/* 
 * Called to set up the Bios Data area time update vars.
 * and the heart beat's counters
 */
VOID host_init_bda_timer(void)
{
    SYSTEMTIME TimeDate;
    ULONG      Ticks;
    struct host_timeval time;


    CreepTicCount.QuadPart = NtGetTickCount();
    GetPerfCounterUsecs(&time, &CumUSec);
    GetLocalTime(&TimeDate);

    Ticks = (ULONG)TimeDate.wHour * 65543 +
            (ULONG)TimeDate.wMinute * 1092 +
            (ULONG)TimeDate.wSecond * 18 ;

    if (TimeDate.wHour)
        Ticks += (ULONG)TimeDate.wHour/3;
    if (TimeDate.wMinute)
        Ticks += (ULONG)(TimeDate.wMinute*4)/10;
    if (TimeDate.wSecond)
        Ticks += (ULONG)TimeDate.wSecond/5;
    if (TimeDate.wMilliseconds)
        Ticks += ((ULONG)TimeDate.wMilliseconds)/54;

    Ticks++;  // fudge factor!

    CreepUSec = CumUSec;
    TimerEventUSec.QuadPart = CumUSec.QuadPart + SYSTEM_TICK_INTV;
    ReinitIdealTime(&time);


      /*
       * BUGBUG with sas strange errors when writing from non cpu thread
       *
       *     sas_storew(TIMER_LOW, BDA & 0xffff);
       *     sas_storew(TIMER_HIGH, (BDA >> 16) & 0xffff);
       *     sas_store(TIMER_OVFL,  0x01);
       */
    * (word *)(Start_of_M_area + TIMER_HIGH)      = (word)(Ticks >> 16);
    * (word *)(Start_of_M_area + TIMER_LOW)       = (word)Ticks;
    * (half_word *)(Start_of_M_area + TIMER_OVFL) = (half_word)0;


    // reset the Real Time Clock
    rtc_init();

#ifndef MONITOR
#line 497
    q_event_init();
#endif
#line 488

#line 500
}



/*   host_GetSysTime, replacement for the base function
 *
 *
 *   This routine does not return the system's time of day.
 *   Uses the NT performance counter to obtain time stamping
 *   information for the base to use. The resolution is microsecs.
 *
 *   Returns nothing, fills in time structure
 *
 */
void host_GetSysTime(struct host_timeval *time)
{
    LARGE_INTEGER liTime;

#line 518
        // Don't call kernel unless we have to.
#line 519
    if (bDoingTicInterrupt) {
        liTime = RtlExtendedLargeIntegerDivide(
                                        CurrHeartBeat,
                                        1000000,
                                        &time->tv_usec);
#line 524
        time->tv_sec = liTime.LowPart;
        }
    else {
        GetPerfCounterUsecs(time, NULL);
        }
}


/*   host_TimeStamp
 *
 *   This routine does not return the system's time of day.
 *   Uses the NT performance counter to obtain time stamping
 *   information for the base to use. Returns LARGE_INTEGER
 *   with time since boot in usecs.
 *
 */
void host_TimeStamp(PLARGE_INTEGER pliTime)
{
   host_ica_lock();

   if (bDoingTicInterrupt) {
       *pliTime = CurrHeartBeat;
       }
   else {
       GetPerfCounterUsecs(NULL, pliTime);
       }

   host_ica_unlock();
}








/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::::::::::::::::: Win32 timer function entry point :::::::::::::::::::::::*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

DWORD HeartBeatThread(PVOID pv)
{
   DWORD dwRet = (DWORD)-1;

   try {

#line 571
#ifdef MONITOR
#line 572
#error Excluded unsupported monitor timer lock setup
#line 578
#endif
#line 578


       //
       // Set our priority above normal, and wait for signal to
       // start heartbeat pulses.
       //
       // For Wow we raise to time critical because wow apps can
       // easily invoke a tight client-csr-server bound loop with
       // boosted priority starving the heartbeat thread. Winbench 311
       // shows this problem when doing polylines test.
       //
#line 590
      SetThreadPriority(ThreadInfo.HeartBeat.Handle,
                        !(dwWNTPifFlags & COMPAT_TIMERTIC)
                           ? THREAD_PRIORITY_TIME_CRITICAL
                           : THREAD_PRIORITY_HIGHEST
                        );

#ifdef X86GFX
#line 597
#error DIVERGENCE MVDM-HOST-DIV-510: X86GFX branch is not selected for this source; see mirror register.
#line 606
#endif
#line 607

#line 608
#ifdef MONITOR
#line 609
#error Excluded unsupported monitor timer lock release
#endif
#line 612

#line 612
      dwRet = Win32_host_timer();

      }
   except(VdmUnhandledExceptionFilter(GetExceptionInformation())) {
      ;  // we shouldn't arrive here
      }

   return dwRet;
}


#ifdef PIG
#line 624
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#endif /* PIG */
#line 626

DWORD Win32_host_timer(void)
{
    NTSTATUS      status;
#ifdef PIG
#line 631
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#endif /* PIG */
#line 633
    LONG          DelayPeriod;
    LARGE_INTEGER DiffTime;
    LARGE_INTEGER SystemTickIntv;
    LARGE_INTEGER SecIntv;
    LARGE_INTEGER HalfSysIntv;
    LARGE_INTEGER CreepIntv;


    struct host_timeval time;

    DelayPeriod = 50000;
    SystemTickIntv.QuadPart  = SYSTEM_TICK_INTV;
    HalfSysIntv.QuadPart  = SYSTEM_TICK_INTV/2;
    SecIntv.QuadPart  = SYSTEM_TICK_INTV*18;
    CreepIntv.QuadPart  = Int32x32To64(SYSTEM_TICK_INTV, 1200);   // >1 hr


    /* Start timing loop. */
    while(1)  {
       status = DelayHeartBeat(DelayPeriod);
       if (!status) {   // reinitialize counters
HBresume:
           host_ica_lock();
           host_init_bda_timer();
           DelayPeriod = SYSTEM_TICK_INTV - 6000;
           host_ica_unlock();
           continue;
           }

       host_ica_lock();
       bDoingTicInterrupt = TRUE;
       /*
        *  Get the current perf counter time, We ignore wrap
        *  since it only happens every few hundred years.
        */
       GetPerfCounterUsecs(&time, &CurrHeartBeat);

        /*
         *  Increment the cumulative counter
         */
       CumUSec.QuadPart = CumUSec.QuadPart + SYSTEM_TICK_INTV;

        /*
         * if we have passed the creep interval, Adjust the cumulative
         * counter for drift between perfcounter and tic counter.
         */
       DiffTime.QuadPart = CurrHeartBeat.QuadPart - CreepUSec.QuadPart;
       if (DiffTime.QuadPart > CreepIntv.QuadPart) {
           CreepAdjust(DiffTime);
           }

        /*
         *  Calculate Next Delay Period, based on how far
         *  behind we are. ie CurrTime - CumTime.
         */
       DiffTime.QuadPart = CurrHeartBeat.QuadPart - CumUSec.QuadPart;

       if (DiffTime.QuadPart > SecIntv.QuadPart)
         {
          DelayPeriod = 13000;
          }
       else if (DiffTime.QuadPart > SYSTEM_TICK_INTV)
         {
          DelayPeriod = SYSTEM_TICK_INTV/3;
          }
       else if (DiffTime.QuadPart >= HalfSysIntv.QuadPart)
         {
          DiffTime.QuadPart = (LONGLONG)SYSTEM_TICK_INTV - DiffTime.QuadPart;
          DelayPeriod = DiffTime.LowPart - 6000;
          }
       else {
          DelayPeriod = SYSTEM_TICK_INTV - 6000 - DiffTime.LowPart;
          }



         /*
          * Update the VirtualTimerHardware
          */
#ifdef PIG
#line 713
#error DIVERGENCE MVDM-HOST-DIV-498: PIG comparison profile is not selected; see mirror register.
#line 718
#else
#line 719
       time_tick();
#endif /* PIG */
#line 721


          /*
           *  Update the Real Time Clock
           */
       RtcTick(&time);

       bDoingTicInterrupt = FALSE;
       host_ica_unlock();


           /*  Timer Event should occur around 18 times per sec
            *  The count doesn't have to be all that accurate, so we
            *  don't try to make up for lost events, and we do this last
            *  to give a chance for hw interrupts to get thru first.
            */
       if (TimerEventUSec.QuadPart <= CurrHeartBeat.QuadPart) {
           TimerEventUSec.QuadPart = CurrHeartBeat.QuadPart + SYSTEM_TICK_INTV;
           cpu_interrupt(CPU_TIMER_TICK, 0);
           WOWIdle(TRUE);
           }
       }

   return(1);
}


/*
 *  DelayHeartBeat
 *
 *  waits the Delay as required by caller
 *  while also checking for the following:
 *   - suspend\resume events
 *   - screen switching event (x86 graphics)
 *
 *   entry : delay time in micro secs
 *   exit  : TRUE - reinit counters
 */

NTSTATUS DelayHeartBeat(LONG Delay)
{
     NTSTATUS status;
     LARGE_INTEGER liDelay;


     liDelay.QuadPart  = Int32x32To64(Delay, -10);

#ifdef MONITOR
#line 769
#error Excluded unsupported monitor heartbeat wait
#line 819
#else          // ndef MONITOR
#line 820
//
// On Risc platforms we only have to deal with the
// HeartBeat Resume\Suspend objects so things are much simpler
//

     status = NtWaitForSingleObject(hHBSuspendEvent,
                                    TRUE,
                                    &liDelay);

     if (status == STATUS_TIMEOUT) {
         return status;
         }

     if (status == STATUS_SUCCESS) {  // suspend event
         status = NtWaitForSingleObject(hHBResumeEvent, TRUE, NULL);
         if (status == STATUS_SUCCESS) {
             return status;
             }
         }

#line 840
#endif

#line 842
         // alerted to die
     if (status == STATUS_ALERTED)  {
#line 844
         CloseHandle(ThreadInfo.HeartBeat.Handle);
         ThreadInfo.HeartBeat.Handle = NULL;
         ThreadInfo.HeartBeat.ID = 0;
         ExitThread(0);
         }


#line 851
      // Must be an error, announce it to the world
#line 852
     DisplayErrorTerm(EHS_FUNC_FAILED, status,__FILE__,__LINE__);
     TerminateVDM();
     return status;
}

/*
 *  CreepAdjust
 *
 *  Adjusts the perfcounter cum time stamp for drift from system time of
 *  day (Kernel Tick Count)
 */
void CreepAdjust(LARGE_INTEGER DiffTime)
{
  LARGE_INTEGER DiffTicCount;
  ULONG         ulTicCount;

   // Calculate the elapsed ticcount in usecs
  ulTicCount = NtGetTickCount();
  DiffTicCount.LowPart  = ulTicCount;
  DiffTicCount.HighPart = CreepTicCount.HighPart;
  if (DiffTicCount.LowPart < CreepTicCount.LowPart) {
      DiffTicCount.HighPart++;
      }
  DiffTicCount.QuadPart = DiffTicCount.QuadPart - CreepTicCount.QuadPart;
  DiffTicCount = RtlExtendedIntegerMultiply(DiffTicCount, 1000);

   // Adjust the CumUsec perfcounter time by the diff
   // between tick count and perfcounter.
  DiffTicCount.QuadPart = DiffTicCount.QuadPart - DiffTime.QuadPart;
  CumUSec.QuadPart = CumUSec.QuadPart - DiffTicCount.QuadPart;

    // Reset the Creep Time stamps
  CreepTicCount.QuadPart = ulTicCount;
  CreepUSec     = CurrHeartBeat;
}


/*  SuspendTimerThread\ResumeTimerThread
 *
 *  functions to supsend\resume the heartbeat thread
 *  - used by ntvdm when dos apps exit
 *  - used by wow when only wowexec is running
 *  - used by wow for tasks requiring timer tics\BDA tic count updates
 *
 *  These two functions keep an internal suspend counter, to manage
 *  wows multiple tasks, some which require tics, some don't. As long
 *  as one task requires tics\bda updates, we will deliver them for all
 *  tasks.
 *
 */


/*  SuspendTimerThread
 *
 *  Blocks the timer thread on an event
 *  Increments internal suspend count
 *
 *  This function will NOT wait until the heartbeat is safely blocked
 *  before returning.
 *
 *  entry: void
 *  exit:  void
 *
 */
GLOBAL VOID SuspendTimerThread(VOID)
{
    RtlEnterCriticalSection(&HBSuspendCS);

    if (!--HeartBeatResumes)  {
        SetEvent(hHBSuspendEvent);
        }

    RtlLeaveCriticalSection(&HBSuspendCS);
}



/*  ResumeTimerThread
 *
 *  restarts the heart beat thread, by setting event
 *  decrements internal suspend count
 *
 *  entry: void
 *  exit:  void
 *
 */
GLOBAL VOID ResumeTimerThread(VOID)
{
    RtlEnterCriticalSection(&HBSuspendCS);

    if (!HeartBeatResumes++) {
        SetEvent(hHBResumeEvent);
        }

    RtlLeaveCriticalSection(&HBSuspendCS);
}



/*
 *  This function handles all of the toplevel
 *  exceptions for all ntvdm threads which are known.
 *  This includes the event thread, heartbeat thread, comms thread,
 *  and all application threads (those which use host_CreateThread()).
 *
 *  Threads which are not covered are those created by unknown Vdds.
 *
 *  If the UnHandleExecptionFilter api returns EXECEPTION_EXECUTE_HANDLER
 *  the process will be terminated and this routine will not return.
 *
 */
LONG
VdmUnhandledExceptionFilter(
    struct _EXCEPTION_POINTERS *ExceptionInfo
    )
{
    LONG lRet;

    SuspendTimerThread();

    lRet = UnhandledExceptionFilter(ExceptionInfo);

    if (lRet == EXCEPTION_EXECUTE_HANDLER) {
        NtTerminateProcess(NtCurrentProcess(),
                           ExceptionInfo->ExceptionRecord->ExceptionCode
                           );
        }

    ResumeTimerThread();
    return lRet;
}
