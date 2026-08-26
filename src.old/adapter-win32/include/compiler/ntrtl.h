#ifndef STAGE_NTRTL_H
#define STAGE_NTRTL_H

#include <nt.h>

/* Reached OpenNT routines use the RTL spelling.  Public Win32 exposes the
 * identical bounded-memory primitive as ZeroMemory. */
#ifndef RtlZeroMemory
#define RtlZeroMemory(Destination, Length) ZeroMemory((Destination), (Length))
#endif

#endif
