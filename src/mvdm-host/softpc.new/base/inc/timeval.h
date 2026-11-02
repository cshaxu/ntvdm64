/*
 * SoftPC/SoftWindows
 *
 * Name:        : timeval.h
 *
 * Title        : Host Time structure definitions
 *
 * Sccs ID:     : @(#)timeval.h 1.5 04/20/94
 *
 * Description  : Definitions for users of host-specific time functions.
 *
 * Author       : David Rees
 *
 * Notes        : Make time structures available: host_timeval,
 *                host_timezone, host_tm.
 */

/* SccsID[]="@(#)timeval.h	1.5 04/20/94 Copyright Insignia Solutions Ltd."; */

/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

struct host_timeval {
    IS32 tv_sec;
    IS32 tv_usec;
};



struct host_timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

#ifndef NTVDM
#error Unsupported excluded original configuration
#endif
#line 51


/*
 * External Declarations
 */

extern long host_time IPT1(long *, tloc);
extern struct host_tm *host_localtime IPT1(time_t *, clock);

#ifndef NTVDM
#error Unsupported excluded original configuration
#else
#line 64
extern void host_GetSysTime(struct host_timeval *);
#define host_gettimeofday(timeval, timezn) host_GetSysTime((timeval))
#endif
#line 67
