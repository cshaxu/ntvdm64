#include <windows.h>
#include <stdio.h>
#include "wow_user_runtime.h"

static wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
static volatile LONG errors, peer_entered;
static HANDLE begin_peer, peer_done;

#define CHECK(value) do { if (!(value)) { InterlockedIncrement(&errors); \
    printf("FAIL line=%d\\n", __LINE__); } } while (0)

static DWORD WINAPI peer(LPVOID ignored)
{
    wow_user_runtime_thread binding = {0};
    UNREFERENCED_PARAMETER(ignored);
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(WaitForSingleObject(begin_peer, 5000) == WAIT_OBJECT_0);
    CHECK(wow_user_runtime_enter(&binding));
    CHECK(runtime.owner_thread_id == GetCurrentThreadId());
    InterlockedIncrement(&peer_entered);
    CHECK(wow_user_runtime_leave(&binding));
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(SetEvent(peer_done));
    return errors ? 1 : 0;
}

int __cdecl main(void)
{
    wow_user_runtime_thread caller = {0};
    HANDLE thread;
    DWORD exit_code;
    begin_peer = CreateEventA(NULL, TRUE, FALSE, NULL);
    peer_done = CreateEventA(NULL, TRUE, FALSE, NULL);
    CHECK(begin_peer && peer_done);
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&caller, &runtime, NULL, NULL));
    CHECK(wow_user_runtime_current() == &caller);
    thread = CreateThread(NULL, 0, peer, NULL, 0, NULL);
    CHECK(thread != NULL);
    CHECK(wow_user_runtime_enter(&caller));
    CHECK(!wow_user_runtime_unbind(&caller));
    CHECK(!wow_user_runtime_enter(&caller));
    CHECK(wow_user_runtime_leave_for_callout(&caller));
    CHECK(SetEvent(begin_peer));
    CHECK(WaitForSingleObject(peer_done, 5000) == WAIT_OBJECT_0);
    CHECK(wow_user_runtime_reenter_after_callout(&caller));
    CHECK(runtime.owner_thread_id == GetCurrentThreadId());
    CHECK(wow_user_runtime_leave(&caller));
    CHECK(WaitForSingleObject(thread, 5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &exit_code) && exit_code == 0);
    CHECK(CloseHandle(thread));
    CHECK(wow_user_runtime_unbind(&caller));
    CHECK(CloseHandle(begin_peer));
    CHECK(CloseHandle(peer_done));
    printf("WOW_USER_RUNTIME errors=%ld peer=%ld callout_reacquire=1\n",
        errors, peer_entered);
    return errors ? 3 : 0;
}
