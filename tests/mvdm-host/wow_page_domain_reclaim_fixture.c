/* Test-only failed decommit boundary. Compile the production page-domain
 * translation unit unchanged except for this one unavailable-operation leaf.
 * Successful frees still execute original nt_mem; no production fault hook. */
#include <windows.h>

extern LONG VdmFreeVirtualMemory(ULONG address);
static ULONG failed_address;
static unsigned failure_calls;

void __cdecl wow_fixture_fail_window_free(ULONG address)
{
    failed_address = address;
    failure_calls = 0;
}

unsigned __cdecl wow_fixture_window_free_failures(void)
{
    return failure_calls;
}

static LONG wow_fixture_free_memory(ULONG address)
{
    if (failed_address && address == failed_address) {
        ++failure_calls;
        return (LONG)0xc0000001u;
    }
    return VdmFreeVirtualMemory(address);
}

#define VdmFreeVirtualMemory wow_fixture_free_memory
#include "ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"
#undef VdmFreeVirtualMemory
