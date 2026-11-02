/*
 * File:        idetect.h
 *
 * Author:      J.Roper
 *
 * Import Necessary items for IDLE DETECT
 *
 * SccsID @(#)idetect.h 1.6 08/25/93 Copyright Insignia Solutions Ltd
 */

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 16

void    idetect();
void    idle_set();
void    idle_ctl();

#define IDLE_INIT               0
#define IDLE_KYBD_POLL  1
#define IDLE_TIME_TICK  2
#define IDLE_WAITIO             3


#ifdef NTVDM
/*
 * NT VDM idling system based on waits - call funcs for counter idle indicators
 * to allow them to cancel waits.
 */
VOID HostIdleNoActivity(VOID);

#define IDLE_video()    HostIdleNoActivity()
#define IDLE_comlpt()   HostIdleNoActivity()
#define IDLE_disk()     HostIdleNoActivity()

/* exported just for kb_setup_vector intialization */
extern word *pICounter;
extern word *pCharPollsPerTick;
extern word *pMinConsecutiveTicks;
extern word IdleNoActivity;
extern word ienabled;

#else
#error Unsupported excluded original configuration
#endif
#line 48

#define IDLE_tick()             idetect(IDLE_TIME_TICK)
#define IDLE_poll()             idetect(IDLE_KYBD_POLL)
#define IDLE_waitio()   idetect(IDLE_WAITIO)
#define IDLE_init()             idetect(IDLE_INIT)
#define IDLE_set(a,b)   idle_set(a,b)
#define IDLE_ctl(a)             idle_ctl(a)

