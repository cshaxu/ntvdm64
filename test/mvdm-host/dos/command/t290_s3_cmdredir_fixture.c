#include "cmd.h"

#include <cmdsvc.h>
#include <stdint.h>

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

static int expect_standard_handle(PREDIRCOMPLETE_INFO redirection,
    USHORT standard_handle, HANDLE expected)
{
    uint32_t observed;

    fixture_ax = (USHORT)((ULONG)(uintptr_t)redirection >> 16);
    fixture_bx = (USHORT)(ULONG)(uintptr_t)redirection;
    fixture_cx = standard_handle;
    fixture_dx = 0xffffu;
    fixture_carry = 1u;
    cmdGetStdHandle();
    observed = ((uint32_t)fixture_bx << 16) | fixture_cx;
    return fixture_carry == 0u && fixture_ax == 0u && fixture_dx == 0u &&
        observed == (uint32_t)(uintptr_t)expected;
}

/* Exercise the original worker creation and completion, without a session
 * bridge or a fake CreateThread provider. The runner bounds hangs. */
static int check_pipe(BOOL input)
{
    HANDLE reader, writer, file, thread;
    VDMINFO info = {0};
    PREDIRCOMPLETE_INFO redirection;
    USHORT mask;
    DWORD transferred, code;
    char bytes[4] = {0};
    if (!CreatePipe(&reader, &writer, NULL, 0)) return 0;
    if (input) info.StdIn = reader;
    else info.StdOut = writer;
    redirection = cmdCheckStandardHandles(&info, &mask);
    if (!redirection) return 0;
    fixture_ax = (USHORT)((uintptr_t)redirection >> 16);
    fixture_bx = (USHORT)(uintptr_t)redirection;
    fixture_cx = input ? HANDLE_STDIN : HANDLE_STDOUT;
    cmdGetStdHandle();
    if (fixture_carry) return 0;
    file = (HANDLE)(uintptr_t)(((uint32_t)fixture_bx << 16) | fixture_cx);
    thread = input ? redirection->ri_pPipeStdIn->hThread :
        redirection->ri_hStdOutThread;
    if (!WriteFile(input ? writer : file, "pipe", 4, &transferred, NULL) ||
        transferred != 4) return 0;
    if (input) CloseHandle(writer);
    else SetEvent(redirection->ri_pPipeStdOut->hExitEvent);
    if (WaitForSingleObject(thread, 5000) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &code) || code != 0) return 0;
    if (!ReadFile(input ? file : reader, bytes, 4, &transferred, NULL) ||
        transferred != 4 || memcmp(bytes, "pipe", 4)) return 0;
    CloseHandle(file);
    if (!cmdCheckCopyForRedirection(redirection)) return 0;
    if (!input) CloseHandle(reader);
    return 1;
}

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
        return 3;
    if (!expect_standard_handle(redirection, HANDLE_STDIN, information.StdIn))
        return 5;
    if (!expect_standard_handle(redirection, HANDLE_STDOUT, information.StdOut))
        return 6;
    if (!expect_standard_handle(redirection, HANDLE_STDERR, information.StdErr))
        return 7;
    if (!cmdCheckCopyForRedirection(redirection))
        return 8;
    if (!check_pipe(FALSE) || !check_pipe(TRUE))
        return 9;
    return 0;
}
