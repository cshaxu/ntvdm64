#include <nt.h>

BOOLEAN VrInitialized(VOID);
BOOLEAN VrInitialize(VOID);
VOID VrUninitialize(VOID);

static ULONG fixture_carry;
static ULONG fixture_hook_calls;
static ULONG fixture_netbios_calls;
static ULONG fixture_dlc_calls;
static ULONG fixture_interrupt_calls;

DWORD VrPeekNamedPipeTickCount;
CRITICAL_SECTION VrNmpRequestQueueCritSec;
CRITICAL_SECTION VrNamedPipeCancelCritSec;

/* The S3-selected source-defined unavailable direction: without the original
 * VDD product hook, VrInitialize must fail before touching guest state. */
int VDDInstallUserHook(HANDLE module, FARPROC create, FARPROC terminate,
    FARPROC suspend, FARPROC resume)
{
    (void)module;
    (void)create;
    (void)terminate;
    (void)suspend;
    (void)resume;
    ++fixture_hook_calls;
    return 0;
}

VOID VrNetbios5cInitialize(VOID)
{
    ++fixture_netbios_calls;
}

VOID VrDlcInitialize(VOID)
{
    ++fixture_dlc_calls;
}

VOID setCF(ULONG value)
{
    fixture_carry = value;
}

USHORT getCS(VOID)
{
    return 0;
}

VOID call_ica_hw_interrupt(int adapter, int line, int state)
{
    (void)adapter;
    (void)line;
    (void)state;
    ++fixture_interrupt_calls;
}

/* vrdisp.c owns this historical helper.  The selected VDD-unavailable branch
 * does not call it, but the exact vrinit translation unit retains its link
 * reference.  A later selector-disabled dispatcher task owns its composition. */
LPVOID _inlinePointerFromWords(WORD segment, WORD offset)
{
    (void)segment;
    (void)offset;
    return NULL;
}

VOID VrNetbios5cInterrupt(VOID)
{
}

VOID SoftPcEoi(int adapter, int *line)
{
    (void)adapter;
    if (line != NULL)
        *line = -1;
}

BOOLEAN IsPmNcbAtQueueHead(VOID)
{
    return FALSE;
}

VOID setZF(BOOLEAN value)
{
    (void)value;
}

int main(void)
{
    if (VrInitialized())
        return 1;
    if (VrInitialize())
        return 2;
    if (fixture_hook_calls != 1u || fixture_netbios_calls != 0u ||
        fixture_dlc_calls != 0u || fixture_interrupt_calls != 0u)
        return 3;
    if (VrInitialized())
        return 4;

    fixture_carry = 1u;
    VrUninitialize();
    if (VrInitialized() || fixture_carry != 0u)
        return 5;
    return 0;
}
