/* Execute the actual launcher bootstrap with deterministic API boundaries.
 * No real process, clock delay, broker submission or guest is used here. */
#include <nt.h>
#include <stdio.h>
#include <stdlib.h>
static DWORD replies[4], calls, creates, closes, waits, terminal;
static ULONGLONG clock_ms;
static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static DWORD test_connect(DWORD remaining)
{
    CHECK(remaining && remaining <= 5000);
    return replies[calls < 3 ? calls++ : 3];
}
static ULONGLONG WINAPI test_clock(void) { return clock_ms; }
static BOOL WINAPI test_create(LPCWSTR app, LPWSTR command,
    LPSECURITY_ATTRIBUTES process_security, LPSECURITY_ATTRIBUTES thread_security,
    BOOL inherit, DWORD flags, LPVOID environment, LPCWSTR directory,
    LPSTARTUPINFOW startup, LPPROCESS_INFORMATION process)
{
    (void)app; (void)command; (void)process_security; (void)thread_security;
    (void)inherit; (void)flags; (void)environment; (void)directory; (void)startup;
    ++creates; process->hProcess=(HANDLE)1; process->hThread=(HANDLE)2;
    return TRUE;
}
static BOOL WINAPI test_close(HANDLE handle)
{ CHECK(handle == (HANDLE)1 || handle == (HANDLE)2); ++closes; return TRUE; }
static DWORD WINAPI test_wait(HANDLE handle, DWORD interval)
{
    CHECK(handle == (HANDLE)1 && interval <= 50);
    ++waits; clock_ms += interval;
    return terminal ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}
static BOOL WINAPI test_exit(HANDLE handle, LPDWORD code)
{ CHECK(handle == (HANDLE)1); *code=terminal; return TRUE; }

#define OpenNtBaseClientConnectCurrentTimeout test_connect
#define GetTickCount64 test_clock
#define CreateProcessW test_create
#define CloseHandle test_close
#define WaitForSingleObject test_wait
#define GetExitCodeProcess test_exit
#define common_product_path_w test_product_path
#define wWinMain unused_launcher_entry
#include "../../src/run16-exe/main.c"
#undef GetTickCount64

DWORD common_product_path_w(PCWSTR relative, PWSTR out, DWORD capacity)
{ (void)relative; CHECK(capacity >= 12); wcscpy_s(out,capacity,L"ntsrv.exe"); return 0; }
static void reset(DWORD first, DWORD second, DWORD third, DWORD rest)
{
    replies[0]=first; replies[1]=second; replies[2]=third; replies[3]=rest;
    calls=creates=closes=waits=terminal=0; clock_ms=0;
}
int wmain(void)
{
    reset(RPC_S_SERVER_UNAVAILABLE,RPC_S_NO_INTERFACES,0,0);
    CHECK(connect_broker()==0);
    CHECK(calls==3 && creates==1 && closes==2 && waits==2);
    reset(RPC_S_NO_INTERFACES,0,0,0);
    CHECK(connect_broker()==RPC_S_NO_INTERFACES);
    CHECK(calls==1 && !creates && !closes && !waits);
    reset(RPC_S_SERVER_UNAVAILABLE,ERROR_REVISION_MISMATCH,0,0);
    CHECK(connect_broker()==ERROR_REVISION_MISMATCH);
    CHECK(calls==2 && creates==1 && closes==2);
    reset(RPC_S_SERVER_UNAVAILABLE,RPC_S_CALL_FAILED,0,0);
    CHECK(connect_broker()==RPC_S_CALL_FAILED);
    CHECK(calls==2 && creates==1 && closes==2);
    reset(RPC_S_SERVER_UNAVAILABLE,RPC_S_NO_INTERFACES,0,0);
    terminal=ERROR_PROCESS_ABORTED;
    CHECK(connect_broker()==ERROR_PROCESS_ABORTED);
    CHECK(calls==1 && creates==1 && closes==2);
    reset(RPC_S_SERVER_UNAVAILABLE,RPC_S_NO_INTERFACES,
        RPC_S_NO_INTERFACES,RPC_S_NO_INTERFACES);
    CHECK(connect_broker()==ERROR_TIMEOUT);
    CHECK(clock_ms==5000 && creates==1 && closes==2);
    reset(0,0,0,0);
    CHECK(connect_broker()==0 && !creates && !waits && !closes);
    printf("PASS %u launcher bootstrap assertions\n",checks);
    return 0;
}
