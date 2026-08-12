/* Staging-only replacement for the obsolete public NT umbrella header.
 * It deliberately provides only documented modern Win32/NT base types. */
#ifndef OPENNT_STAGE_NT_H
#define OPENNT_STAGE_NT_H

#include <windows.h>
#include <winternl.h>

/* The modern umbrella headers used by this staging island omit the historical
 * NTSTATUS base typedef. OpenNT's ntdef.h defines it as LONG; importing the
 * full legacy ntdef.h would conflict with the already included windows.h. */
#ifndef OPENNT_STAGE_NTSTATUS_DEFINED
#define OPENNT_STAGE_NTSTATUS_DEFINED
typedef LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;
#endif

/* OpenNT ntstatus.h supplies this base success value.  Keep the staging
 * surface narrow: the original suballocator needs no broader status table. */
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#endif
