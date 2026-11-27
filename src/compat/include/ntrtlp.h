#ifndef OPENNT_HOST_NTOS_RTL_NTRTLP_H
#define OPENNT_HOST_NTOS_RTL_NTRTLP_H

/* The selected source body is original.  This header supplies only the
 * established standalone declaration boundary for its finite user-mode
 * dependencies; it supplies no environment algorithm. */
#include <nt.h>

/* `winternl.h` exposes a reduced modern PEB layout.  The original body
 * reaches only these adapter-private historical fields, so bind its source
 * spelling to the established surrogate rather than casting the host PEB. */
#define PPEB POPENNT_SUPPORT_PEB
/* error.c needs the original per-thread LastStatusValue carrier. */
#define PTEB POPENNT_SUPPORT_TEB

#ifndef STATUS_NOT_FOUND
#define STATUS_NOT_FOUND ((NTSTATUS)0xC0000225L)
#endif

/* ntdef.h constant initializer needed by the original environment owner. */
#ifndef RTL_CONSTANT_STRING
#define RTL_CONSTANT_STRING(s) { sizeof(s)-sizeof((s)[0]), sizeof(s), (PWSTR)(s) }
#endif
#endif
