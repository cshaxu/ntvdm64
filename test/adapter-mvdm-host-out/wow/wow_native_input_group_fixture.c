#include <windows.h>
#include <stdio.h>

/* Read/write only this fixture's private USER keyboard-state tables.
 * No SendInput, user-thread attachment, visible window or guest mutation. */
typedef struct input_probe {
    HANDLE ready,request,reply,stop;
    BYTE state[256];
    BOOL ok;
} input_probe;
static unsigned errors;
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError());} } while(0)
static DWORD WINAPI probe_thread(void *argument)
{
    input_probe *probe=argument;
    HANDLE waits[2]={probe->stop,probe->request};
    MSG message;
    BYTE state[256]={0};
    PeekMessageW(&message,NULL,0,0,PM_NOREMOVE);
    probe->ok=SetKeyboardState(state);
    SetEvent(probe->ready);
    for(;;) {
        DWORD result=WaitForMultipleObjects(2,waits,FALSE,5000);
        if(result==WAIT_OBJECT_0)return 0;
        if(result!=WAIT_OBJECT_0+1)return 2;
        probe->ok=GetKeyboardState(probe->state);
        SetEvent(probe->reply);
    }
}
static BOOL read_probe(input_probe *probe)
{
    if(!SetEvent(probe->request) || WaitForSingleObject(probe->reply,3000)!=WAIT_OBJECT_0)return FALSE;
    return probe->ok;
}
static HANDLE start_probe(input_probe *probe,DWORD *id)
{
    HANDLE thread;
    ZeroMemory(probe,sizeof(*probe));
    probe->ready=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe->request=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe->reply=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!probe->ready || !probe->request || !probe->reply || !probe->stop)ExitProcess(99);
    thread=CreateThread(NULL,0,probe_thread,probe,0,id);
    if(!thread)ExitProcess(99);
    CHECK(WaitForSingleObject(probe->ready,3000)==WAIT_OBJECT_0 && probe->ok);
    return thread;
}
static void close_probe(input_probe *probe,HANDLE thread)
{
    DWORD code;
    SetEvent(probe->stop);
    CHECK(WaitForSingleObject(thread,3000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&code) && code==0);
    CloseHandle(thread);CloseHandle(probe->ready);CloseHandle(probe->request);
    CloseHandle(probe->reply);CloseHandle(probe->stop);
}
static void middle_exit(BOOL retain_membership)
{
    input_probe middle,peer,newcomer;
    DWORD middle_id,peer_id,newcomer_id=0;
    HANDLE middle_thread=start_probe(&middle,&middle_id);
    HANDLE peer_thread=start_probe(&peer,&peer_id);
    HANDLE newcomer_thread=NULL;
    BYTE state[256]={0};
    BOOL first=FALSE,second=FALSE,repaired=FALSE,joined=FALSE;
    CHECK(SetKeyboardState(state));
    first=AttachThreadInput(GetCurrentThreadId(),middle_id,TRUE);
    CHECK(first);if(!first)goto done;
    second=AttachThreadInput(middle_id,peer_id,TRUE);
    CHECK(second);if(!second)goto done;
    state['D']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&middle) && (middle.state['D']&0x80));
    CHECK(read_probe(&peer) && (peer.state['D']&0x80));
    SetEvent(middle.stop);
    if(WaitForSingleObject(middle_thread,3000)!=WAIT_OBJECT_0) {
        CHECK(FALSE);goto done;
    }
    first=second=FALSE; /* Both links refer to the now-exited middle thread. */
    ZeroMemory(state,sizeof(state));state['E']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&peer));
    printf("middle_exit_remaining_shared=%u\n",(unsigned)!!(peer.state['E']&0x80));
    CHECK(peer.state['E']&0x80);
    if(retain_membership) {
        repaired=AttachThreadInput(GetCurrentThreadId(),peer_id,TRUE);
        CHECK(repaired);if(!repaired)goto done;
    }
    newcomer_thread=start_probe(&newcomer,&newcomer_id);
    joined=AttachThreadInput(GetCurrentThreadId(),newcomer_id,TRUE);
    CHECK(joined);if(!joined)goto done;
    ZeroMemory(state,sizeof(state));state['G']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&peer) && read_probe(&newcomer));
    printf("maintained=%u after_new_join_peer_shared=%u newcomer_shared=%u\n",
        (unsigned)retain_membership,(unsigned)!!(peer.state['G']&0x80),(unsigned)!!(newcomer.state['G']&0x80));
    /* Preserve the unsafe strategy as an expected negative mechanism case. */
    CHECK(!!(peer.state['G']&0x80)==!!retain_membership && (newcomer.state['G']&0x80));
    if(!repaired)repaired=AttachThreadInput(GetCurrentThreadId(),peer_id,TRUE);
    CHECK(repaired);if(!repaired)goto done;
    ZeroMemory(state,sizeof(state));state['F']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&peer) && (peer.state['F']&0x80));
    CHECK(read_probe(&newcomer) && (newcomer.state['F']&0x80));
done:
    if(joined)CHECK(AttachThreadInput(GetCurrentThreadId(),newcomer_id,FALSE));
    if(repaired)CHECK(AttachThreadInput(GetCurrentThreadId(),peer_id,FALSE));
    if(second)CHECK(AttachThreadInput(middle_id,peer_id,FALSE));
    if(first)CHECK(AttachThreadInput(GetCurrentThreadId(),middle_id,FALSE));
    ZeroMemory(state,sizeof(state));SetKeyboardState(state);
    close_probe(&middle,middle_thread);close_probe(&peer,peer_thread);
    if(newcomer_thread)close_probe(&newcomer,newcomer_thread);
}

static void repeated_attachment(void)
{
    input_probe peer;
    DWORD id;
    HANDLE thread=start_probe(&peer,&id);
    BYTE state[256]={0};
    unsigned owned=0;
    BOOL result;
    CHECK(SetKeyboardState(state));
    result=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(result);if(!result)goto done;++owned;
    result=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(result);if(!result)goto done;++owned;
    result=AttachThreadInput(GetCurrentThreadId(),id,FALSE);
    CHECK(result);if(!result)goto done;--owned;
    state['J']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&peer));
    printf("duplicate_attach_after_one_detach_shared=%u\n",(unsigned)!!(peer.state['J']&0x80));
    CHECK(peer.state['J']&0x80);
    result=AttachThreadInput(GetCurrentThreadId(),id,FALSE);
    CHECK(result);if(!result)goto done;--owned;
    ZeroMemory(state,sizeof(state));state['K']=0x80;CHECK(SetKeyboardState(state));
    CHECK(read_probe(&peer) && !(peer.state['K']&0x80));
    /* Original _AttachThreadInput rejects removal without a matching record.
     * A redundant cleanup must not be counted as an owned successful detach. */
    CHECK(!AttachThreadInput(GetCurrentThreadId(),id,FALSE));
done:
    while(owned) {
        CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));--owned;
    }
    ZeroMemory(state,sizeof(state));SetKeyboardState(state);
    close_probe(&peer,thread);
}

int __cdecl main(void)
{
    input_probe probe={0};
    HANDLE thread=NULL;
    DWORD id=0,exit_code=0;
    BYTE state[256]={0},current[256];
    MSG message;
    BOOL attached=FALSE;
    probe.ready=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe.request=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe.reply=CreateEventW(NULL,FALSE,FALSE,NULL);
    probe.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(probe.ready && probe.request && probe.reply && probe.stop);
    if(!probe.ready || !probe.request || !probe.reply || !probe.stop)return 99;
    PeekMessageW(&message,NULL,0,0,PM_NOREMOVE);
    thread=CreateThread(NULL,0,probe_thread,&probe,0,&id);
    CHECK(thread!=NULL);if(!thread)goto done;
    CHECK(WaitForSingleObject(probe.ready,3000)==WAIT_OBJECT_0 && probe.ok);
    state['A']=0x80;
    CHECK(SetKeyboardState(state) && GetKeyboardState(current) && (current['A']&0x80));
    CHECK(read_probe(&probe) && !(probe.state['A']&0x80));
    attached=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(attached);if(!attached)goto done;
    CHECK(GetKeyboardState(current));
    printf("after_attach_a=%u\n",(unsigned)(current['A']&0x80));
    CHECK(SetKeyboardState(state));
    CHECK(read_probe(&probe) && (probe.state['A']&0x80));
    CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));attached=FALSE;
    ZeroMemory(state,sizeof(state));
    CHECK(SetKeyboardState(state));
    CHECK(read_probe(&probe));
    /* Use a different key after detach; no assumption about retained A state. */
    CHECK(!(probe.state['B']&0x80));
    state['B']=0x80;
    CHECK(SetKeyboardState(state));
    CHECK(read_probe(&probe) && !(probe.state['B']&0x80));
    CHECK(!AttachThreadInput(GetCurrentThreadId(),GetCurrentThreadId(),TRUE));
    attached=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(attached);
    if(attached) {
        DWORD waited;
        CHECK(SetEvent(probe.stop));
        waited=WaitForSingleObject(thread,3000);
        CHECK(waited==WAIT_OBJECT_0);
        if(waited==WAIT_OBJECT_0) {
            attached=FALSE; /* Native USER retires an exited thread's input links. */
            CHECK(!AttachThreadInput(GetCurrentThreadId(),id,TRUE));
            ZeroMemory(state,sizeof(state));state['C']=0x80;
            CHECK(SetKeyboardState(state) && GetKeyboardState(current) && (current['C']&0x80));
        }
    }
done:
    if(attached)CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));
    ZeroMemory(state,sizeof(state));SetKeyboardState(state);
    if(thread) {
        SetEvent(probe.stop);
        CHECK(WaitForSingleObject(thread,3000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==0);
        CloseHandle(thread);
    }
    CloseHandle(probe.ready);CloseHandle(probe.request);CloseHandle(probe.reply);CloseHandle(probe.stop);
    middle_exit(FALSE);
    middle_exit(TRUE);
    repeated_attachment();
    printf("WOW_NATIVE_INPUT_GROUP errors=%u isolated=2 shared=1 self_reject=1 peer_exit=1 naive_join_split=1 maintained_join=1 repeated=1\n",errors);
    return errors!=0;
}
