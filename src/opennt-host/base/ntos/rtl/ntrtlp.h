#ifndef OPENNT_HOST_NTOS_RTL_NTRTLP_H
#define OPENNT_HOST_NTOS_RTL_NTRTLP_H

/* The selected source body is original.  This header supplies only the
 * established standalone declaration boundary for its finite user-mode
 * dependencies; it supplies no environment algorithm. */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

/* `winternl.h` exposes a reduced modern PEB layout.  The original body
 * reaches only these adapter-private historical fields, so bind its source
 * spelling to the established surrogate rather than casting the host PEB. */
#define PPEB POPENNT_SUPPORT_PEB
/* error.c needs the original per-thread LastStatusValue carrier. */
#define PTEB POPENNT_SUPPORT_TEB

#endif
