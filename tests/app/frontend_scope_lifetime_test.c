/* Unit resource-lifetime test of the production root pump. Broker discovery
 * and channel transport are substitutes; real threads/events exercise joining.
 * This does not replace authenticated RPC or real guest Console tests. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "run16-exe/frontend_scope.h"
#include "run16-exe/console_channel.h"

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %u error %lu: %s\n", \
    (unsigned)__LINE__,GetLastError(),#x); ExitProcess(1); } } while (0)
#define COUNT 34
struct run16_console_channel { HANDLE thread,release; DWORD index; };
static run16_console_channel *channels[COUNT];
static HANDLE notification,ready;
static CRITICAL_SECTION lock;
static DWORD pending,created;
static LONG stopped;

DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE value)
{
    return DuplicateHandle(GetCurrentProcess(),value,GetCurrentProcess(),
        &notification,0,FALSE,DUPLICATE_SAME_ACCESS) ? 0 : GetLastError();
}
DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE value,HANDLE *root,DWORD *generation)
{
    (void)value;(void)root;(void)generation;
    return ERROR_ACCESS_DENIED;
}
DWORD OpenNtBaseClientFrontendRequest(DWORD *request,HANDLE *worker)
{
    DWORD error=ERROR_NOT_FOUND;
    EnterCriticalSection(&lock);
    if (pending) {
        *request=pending;pending=0;
        error=DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),
            GetCurrentProcess(),worker,SYNCHRONIZE,FALSE,0) ? 0 : GetLastError();
    } else ResetEvent(notification);
    LeaveCriticalSection(&lock);
    return error;
}
static DWORD WINAPI channel_main(void *value)
{
    run16_console_channel *channel=value;
    return WaitForSingleObject(channel->release,INFINITE)==WAIT_OBJECT_0 ? 0 : 1;
}
DWORD run16_console_channel_start_request(DWORD request,HANDLE worker,
    run16_console_channel **output)
{
    run16_console_channel *channel;
    CloseHandle(worker);
    CHECK(request && created<COUNT);
    channel=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*channel));
    CHECK(channel);
    channel->index=created;
    channel->release=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(channel->release);
    channel->thread=CreateThread(NULL,0,channel_main,channel,0,NULL);
    CHECK(channel->thread);
    channels[created++]=channel;
    *output=channel;
    return 0;
}
HANDLE run16_console_channel_thread(run16_console_channel *channel)
{
    return channel->thread;
}
void run16_console_channel_stop(run16_console_channel *channel)
{
    CHECK(SetEvent(channel->release));
    CHECK(WaitForSingleObject(channel->thread,5000)==WAIT_OBJECT_0);
    CloseHandle(channel->thread);CloseHandle(channel->release);
    channels[channel->index]=NULL;
    HeapFree(GetProcessHeap(),0,channel);
    InterlockedIncrement(&stopped);
}
static void attached(void) { CHECK(SetEvent(ready)); }
static void submit(DWORD id)
{
    EnterCriticalSection(&lock);
    CHECK(!pending);pending=id;
    CHECK(SetEvent(notification));
    LeaveCriticalSection(&lock);
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
}
int main(void)
{
    run16_frontend_scope *scope=NULL;
    DWORD i;
    CHECK(SetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY",NULL));
    InitializeCriticalSection(&lock);
    ready=CreateEventW(NULL,FALSE,FALSE,NULL);CHECK(ready);
    CHECK(run16_frontend_scope_begin(&scope,attached)==0);
    submit(1); /* Keep this channel live throughout the repeated short ones. */
    for (i=1;i<COUNT;++i) {
        HANDLE release,thread;
        submit(i+1);
        CHECK(channels[0] && WaitForSingleObject(channels[0]->thread,0)==WAIT_TIMEOUT);
        CHECK(InterlockedCompareExchange(&stopped,0,0)==(LONG)i-1);
        /* The pump may reclaim the entry immediately after thread exit. */
        CHECK(DuplicateHandle(GetCurrentProcess(),channels[i]->release,GetCurrentProcess(),
            &release,0,FALSE,DUPLICATE_SAME_ACCESS));
        CHECK(DuplicateHandle(GetCurrentProcess(),channels[i]->thread,GetCurrentProcess(),
            &thread,SYNCHRONIZE,FALSE,0));
        CHECK(SetEvent(release));
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CloseHandle(release);CloseHandle(thread);
    }
    run16_frontend_scope_end(scope);
    CHECK(stopped==COUNT);
    for (i=0;i<COUNT;++i) CHECK(!channels[i]);
    CloseHandle(notification);CloseHandle(ready);DeleteCriticalSection(&lock);
    puts("PASS completed channels reclaimed; live channel preserved; final join complete");
    return 0;
}
