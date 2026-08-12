/* Bounded trace of the historical pre-config GFI initializer only. */
#include "insignia.h"
#include "xt.h"
#include "config.h"

extern void gfi_init(void);

#ifndef NTDOS64_GFI_USE_HISTORICAL_HOST_RUNTIME
static int runtime_set_calls;
static UTINY runtime_set_what;
static SHORT runtime_set_value;
/*
 * `gfi_init()` only establishes the default server selector before config.
 * This recording seam exists solely to prove that call and does not expose a
 * disk, a drive, a host path, or a normal runtime implementation.
 */
void host_runtime_set(UTINY what, SHORT value)
{
    runtime_set_calls++;
    runtime_set_what = what;
    runtime_set_value = value;
}
#endif

int main(void)
{
    gfi_init();

#ifdef NTDOS64_GFI_USE_HISTORICAL_HOST_RUNTIME
    return 0;
#else
    return runtime_set_calls == 1 &&
           runtime_set_what == C_FLOPPY_SERVER &&
           runtime_set_value == GFI_REAL_DISKETTE_SERVER ? 0 : 1;
#endif
}
