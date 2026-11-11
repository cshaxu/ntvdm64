/* Diagnostic-only replacement object. Original observer behavior is unchanged. */
#include "ntvdm_dos_observer.h"
#include <nt_vdd.h>
#include <stdio.h>
static BOOL identity_install(HANDLE, PFNVDD_UCREATE, PFNVDD_UTERMINATE,
    PFNVDD_UBLOCK, PFNVDD_URESUME);
#define VDDInstallUserHook identity_install
#include "../../src/ntvdm-exe/softpc/ntvdm_dos_observer.c"
#undef VDDInstallUserHook
extern uint32_t c_getCR0(void), c_getCR3(void), c_getCPL(void);
extern int c_getPG(void), c_getVM(void);
extern int xtrn2phy(uint32_t, uint8_t, uint32_t *);
static PFNVDD_UCREATE original_create;
static PFNVDD_UTERMINATE original_exit;
static void identity_record(const char *event, USHORT psp)
{
    DWORD saved = GetLastError();
    uint32_t linear = (uint32_t)psp << 4, physical = linear;
    uint8_t access = (c_getCPL() == 3 ? 2 : 0) | (c_getVM() ? 4 : 0);
    int mapped = !c_getPG() || xtrn2phy(linear, access, &physical);
    ntvdm_dos_occurrence *slot = registered ? active_psp(registered, psp) : NULL;
    unsigned char header[48] = {0};
    int readable = guest_copy(psp, 0, header, sizeof(header));
    FILE *file = fopen(DOS_IDENTITY_LOG, "a");
    if (file) {
        fprintf(file, "%s pid=%lu psp=%04x parent=%04x env=%04x cr0=%08x cr3=%08x linear=%08x mapped=%d physical=%08x occurrence=%llu direct=%llu\n",
            event, GetCurrentProcessId(), psp, readable ? word(header+0x16) : 0,
            readable ? word(header+0x2c) : 0, c_getCR0(), c_getCR3(), linear,
            mapped, physical, slot ? slot->occurrence : 0,
            slot ? slot->direct : 0);
        fclose(file);
    }
    SetLastError(saved);
}
static VOID identity_create(USHORT psp)
{
    identity_record("create-before", psp);
    original_create(psp);
    identity_record("create-after", psp);
}
static VOID identity_exit(USHORT psp)
{
    identity_record("exit-before", psp);
    original_exit(psp);
    identity_record("exit-after", psp);
}
static BOOL identity_install(HANDLE h, PFNVDD_UCREATE c, PFNVDD_UTERMINATE e,
    PFNVDD_UBLOCK b, PFNVDD_URESUME r)
{
    original_create = c; original_exit = e;
    return VDDInstallUserHook(h, identity_create, identity_exit, b, r);
}
