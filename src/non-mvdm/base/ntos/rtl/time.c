/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    Time.c

Abstract:

    This module implements the absolute time conversion routines for NT.

    Absolute LARGE_INTEGER in NT is represented by a 64-bit large integer accurate
    to 100ns resolution.  The smallest time resolution used by this package
    is One millisecond.  The basis for NT time is the start of 1601 which
    was chosen because it is the start of a new quadricentury.  Some facts
    to note are:

    o At 100ns resolution 32 bits is good for about 429 seconds (or 7 minutes)

    o At 100ns resolution a large integer (i.e., 63 bits) is good for
      about 29,247 years, or around 10,682,247 days.

    o At 1 second resolution 31 bits is good for about 68 years

    o At 1 second resolution 32 bits is good for about 136 years

    o 100ns Time (ignoring time less than a millisecond) can be expressed
      as two values, Days and Milliseconds.  Where Days is the number of
      whole days and Milliseconds is the number of milliseconds for the
      partial day.  Both of these values are ULONG.

    Given these facts most of the conversions are done by first splitting
    LARGE_INTEGER into Days and Milliseconds.

Author:

    Gary Kimura     [GaryKi]    26-Aug-1989

Environment:

    Pure utility routine

Revision History:

--*/

#include "ntrtlp.h"
#line 50
#if defined(ALLOC_PRAGMA) && defined(NTOS_KERNEL_RUNTIME)
#line 51
#error Excluded unsupported kernel RTL time allocation profile
#endif
#line 54

/*
 * The standalone redirector needs precisely this exported RTL conversion.
 * Keep the original source file as its owner.  The rest of this NT4 unit
 * requires the retired SYSTEM_TIMEOFDAY_INFORMATION layout and is not a
 * user-mode dependency of the selected VDMREDIR closure.
 */
#if defined(MVDM_STANDALONE_TIME1970_ONLY)

#line 1222
VOID
RtlSecondsSince1970ToTime (
    IN ULONG ElapsedSeconds,
    OUT PLARGE_INTEGER Time
    )
{
    LARGE_INTEGER Seconds;

    Seconds.LowPart = ElapsedSeconds;
    Seconds.HighPart = 0;
    Seconds.QuadPart = Seconds.QuadPart + 0x00000002b6109100i64;
    Time->QuadPart = Seconds.QuadPart * 10000000i64;
}

#else
#error Unsupported excluded original configuration
#endif /* MVDM_STANDALONE_TIME1970_ONLY */
#line 1359

