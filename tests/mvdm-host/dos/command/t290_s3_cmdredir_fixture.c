#include "cmd.h"

PREDIRCOMPLETE_INFO cmdCheckStandardHandles(PVDMINFO info,
    USHORT UNALIGNED *standard_handles);
BOOL cmdCheckCopyForRedirection(PREDIRCOMPLETE_INFO info);

BOOL fSoftpcRedirection;
BOOL fSoftpcRedirectionOnShellOut;

static BOOL fixture_notification;
static DWORD fixture_notification_count;

VOID nt_std_handle_notification(BOOL redirection)
{
    fixture_notification = redirection;
    ++fixture_notification_count;
}

VOID RcErrorDialogBox(DWORD error, PCHAR first, PCHAR second)
{
    (void)error;
    (void)first;
    (void)second;
}

VOID TerminateVDM(VOID)
{
}

/* cmdGetStdHandle remains selector-disabled until the source's legacy
 * AX:BX pointer/handle marshaling has its separately admitted mapping proof.
 * These carriers let the exact translation unit link while this fixture
 * executes only the host-local standard-handle lifecycle. */
USHORT getAX(VOID) { return 0u; }
USHORT getBX(VOID) { return 0u; }
USHORT getCX(VOID) { return 0u; }
VOID setAX(USHORT value) { (void)value; }
VOID setBX(USHORT value) { (void)value; }
VOID setCX(USHORT value) { (void)value; }
VOID setDX(USHORT value) { (void)value; }
VOID setCF(ULONG value) { (void)value; }

int main(void)
{
    VDMINFO information;
    USHORT standard_handles;
    PREDIRCOMPLETE_INFO redirection;

    RtlZeroMemory(&information, sizeof(information));
    standard_handles = 0xffffu;
    redirection = cmdCheckStandardHandles(&information, &standard_handles);
    if (redirection != NULL || standard_handles != 0u || fSoftpcRedirection ||
        fixture_notification || fixture_notification_count != 1u)
        return 1;

    information.StdIn = (HANDLE)(ULONG_PTR)0x7011u;
    information.StdOut = (HANDLE)(ULONG_PTR)0x7012u;
    information.StdErr = (HANDLE)(ULONG_PTR)0x7013u;
    redirection = cmdCheckStandardHandles(&information, &standard_handles);
    if (redirection == NULL || standard_handles != 7u ||
        !fSoftpcRedirection || !fixture_notification ||
        fixture_notification_count != 2u ||
        redirection->ri_hStdIn != information.StdIn ||
        redirection->ri_hStdOut != information.StdOut ||
        redirection->ri_hStdErr != information.StdErr)
        return 2;
    if (!cmdCheckCopyForRedirection(redirection))
        return 3;
    return 0;
}
