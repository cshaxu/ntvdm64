#include "cmd.h"

#include "session/session.h"

PREDIRCOMPLETE_INFO cmdCheckStandardHandles(PVDMINFO info,
    USHORT UNALIGNED *standard_handles);
BOOL cmdCheckCopyForRedirection(PREDIRCOMPLETE_INFO info);
VOID cmdGetStdHandle(VOID);

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

static USHORT fixture_ax;
static USHORT fixture_bx;
static USHORT fixture_cx;
static USHORT fixture_dx;
static ULONG fixture_carry;
USHORT getAX(VOID) { return fixture_ax; }
USHORT getBX(VOID) { return fixture_bx; }
USHORT getCX(VOID) { return fixture_cx; }
VOID setAX(USHORT value) { fixture_ax = value; }
VOID setBX(USHORT value) { fixture_bx = value; }
VOID setCX(USHORT value) { fixture_cx = value; }
VOID setDX(USHORT value) { fixture_dx = value; }
VOID setCF(ULONG value) { fixture_carry = value; }

int main(void)
{
    VDMINFO information;
    USHORT standard_handles;
    PREDIRCOMPLETE_INFO redirection;
    session instance;
    uint32_t handle_identity;

    RtlZeroMemory(&information, sizeof(information));
    standard_handles = 0xffffu;
    redirection = cmdCheckStandardHandles(&information, &standard_handles);
    if (redirection != NULL || standard_handles != 0u || fSoftpcRedirection ||
        fixture_notification || fixture_notification_count != 1u)
        return 1;

    session_initialize(&instance, 0x2903u);
    if (!session_activate(&instance) || !session_thread_bind(&instance))
        return 2;

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
        return 3;
    fixture_ax = (USHORT)((ULONG)(uintptr_t)redirection >> 16);
    fixture_bx = (USHORT)(ULONG)(uintptr_t)redirection;
    fixture_cx = 0u; /* HANDLE_STDIN from the original cmdsvc.h */
    fixture_dx = 0xffffu;
    fixture_carry = 1u;
    cmdGetStdHandle();
    handle_identity = ((uint32_t)fixture_bx << 16) | fixture_cx;
    if (fixture_carry != 0u || fixture_ax != 0u || fixture_dx != 0u ||
        handle_identity != (uint32_t)(uintptr_t)information.StdIn)
        return 5;
    if (!cmdCheckCopyForRedirection(redirection))
        return 8;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 9;
    return 0;
}
