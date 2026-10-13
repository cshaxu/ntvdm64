#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/session/session.h"
#include "mvdm_guest_location.h"
#include "mvdm_command_native_child.h"

/* Supply only session/guest-memory boundaries and the delivered task value.
 * Command capture, lifetime and policy reads are the production unit. */
static session owner;
static session_teardown_fn teardown;
static void *teardown_context;
static DWORD delivered_style;
static BYTE guest[128];
static unsigned reads;
static int deny_lease;
DWORD OpenNtBaseClientDosWindowStyle(void) { ++reads;return delivered_style; }
session *__cdecl session_thread_current(void) { return &owner; }
int session_valid(const session *value) { return value==&owner; }
int session_register_teardown(session *value,session_teardown_fn fn,void *context)
{ (void)value;teardown=fn;teardown_context=context;return 1; }
int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment,uint16_t offset)
{ location->segment=segment;location->offset=offset;location->valid=1;return 1; }
int mvdm_guest_location_acquire(const mvdm_guest_location *location,
    uint32_t bytes,uint32_t access,mvdm_guest_location_lease *lease)
{
    (void)access;
    if(deny_lease || location->segment || location->offset+bytes>sizeof(guest))return 0;
    lease->lease=NULL;lease->bytes=guest+location->offset;return 1;
}
int mvdm_guest_location_release(mvdm_guest_location_lease *lease,int commit)
{ (void)commit;lease->bytes=NULL;return 1; }
#define CHECK(x) do { if(!(x)){printf("FAIL line %d\n",__LINE__);return 1;} } while(0)
static DWORD WINAPI detached_read(void *context)
{
    DWORD actual=99;
    return mvdm_command_native_child_window_style(&actual) &&
        actual==*(DWORD *)context ? 0:1;
}
int main(void)
{
    DWORD expected,actual=99,exit_code;
    HANDLE thread;
    owner.state=SESSION_STATE_ACTIVE;owner.epoch=1;
    CHECK(!mvdm_command_native_child_window_style(&actual));
    for(expected=0;expected<3;++expected){
        delivered_style=expected;
        CHECK(mvdm_command_native_child_capture_host_command("command",0,0,0,16));
        CHECK(reads==expected+1);
        /* Deterministically deliver a different task before the child reader. */
        delivered_style=(expected+1)%3;
        thread=CreateThread(NULL,0,detached_read,&expected,0,NULL);
        CHECK(thread && WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && !exit_code);
        CloseHandle(thread);
        CHECK(reads==expected+1);
        CHECK(!mvdm_command_native_child_capture_host_command("other",0,0,0,16));
        mvdm_command_native_child_finish();
        CHECK(!mvdm_command_native_child_window_style(&actual));
    }
    deny_lease=1;
    CHECK(!mvdm_command_native_child_capture_host_command("bad",0,0,0,16));
    CHECK(!mvdm_command_native_child_window_style(&actual) && reads==3);
    deny_lease=0;memcpy(guest+32,"command\r",8);
    delivered_style=2;
    CHECK(mvdm_command_native_child_capture_guest(0,32,0,0,0,16));
    delivered_style=0;
    CHECK(mvdm_command_native_child_window_style(&actual) && actual==2);
    CHECK(!mvdm_command_native_child_window_style(NULL));
    mvdm_command_native_child_abort();
    CHECK(!mvdm_command_native_child_window_style(&actual));
    teardown(teardown_context);
    puts("PASS DOS policy capture: three modes, cross-thread task change, guest/host capture, finish/abort/failure");
    return 0;
}
