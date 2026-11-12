#include <windows.h>
#include <stdio.h>
#include "wow_input_queue_witness.h"

/* Observe real USER caret ownership on an owned nonactive desktop.
 * No SendInput, user-thread attachment, visible window or guest mutation. */
typedef struct input_probe {
    HANDLE ready,request,reply,stop;
    HWND caret;
    BOOL ok;
} input_probe;
static unsigned errors;
static HDESK desktop;
static input_queue_witness witness;
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError());} } while(0)
static DWORD WINAPI probe_thread(void *argument)
{
    input_probe *probe=argument;
    HANDLE waits[2]={probe->stop,probe->request};
    MSG message;
    probe->ok=SetThreadDesktop(desktop);
    PeekMessageW(&message,NULL,0,0,PM_NOREMOVE);
    SetEvent(probe->ready);
    for(;;) {
        DWORD result=WaitForMultipleObjects(2,waits,FALSE,5000);
        if(result==WAIT_OBJECT_0)return 0;
        if(result!=WAIT_OBJECT_0+1)return 2;
        probe->ok=input_witness_read(&probe->caret);
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
    BOOL first=FALSE,second=FALSE,repaired=FALSE,joined=FALSE;
    first=AttachThreadInput(GetCurrentThreadId(),middle_id,TRUE);
    CHECK(first);if(!first)goto done;
    second=AttachThreadInput(middle_id,peer_id,TRUE);
    CHECK(second);if(!second)goto done;
    CHECK(input_witness_set(&witness,'D'));
    CHECK(read_probe(&middle) && (middle.caret==witness.windows['D'-'A']));
    CHECK(read_probe(&peer) && (peer.caret==witness.windows['D'-'A']));
    SetEvent(middle.stop);
    if(WaitForSingleObject(middle_thread,3000)!=WAIT_OBJECT_0) {
        CHECK(FALSE);goto done;
    }
    first=second=FALSE; /* Both links refer to the now-exited middle thread. */
    CHECK(input_witness_set(&witness,'E'));
    CHECK(read_probe(&peer));
    printf("middle_exit_remaining_shared=%u\n",(unsigned)!!(peer.caret==witness.windows['E'-'A']));
    CHECK(peer.caret==witness.windows['E'-'A']);
    if(retain_membership) {
        repaired=AttachThreadInput(GetCurrentThreadId(),peer_id,TRUE);
        CHECK(repaired);if(!repaired)goto done;
    }
    newcomer_thread=start_probe(&newcomer,&newcomer_id);
    joined=AttachThreadInput(GetCurrentThreadId(),newcomer_id,TRUE);
    CHECK(joined);if(!joined)goto done;
    CHECK(input_witness_set(&witness,'G'));
    CHECK(read_probe(&peer) && read_probe(&newcomer));
    printf("maintained=%u after_new_join_peer_shared=%u newcomer_shared=%u\n",
        (unsigned)retain_membership,(unsigned)!!(peer.caret==witness.windows['G'-'A']),(unsigned)!!(newcomer.caret==witness.windows['G'-'A']));
    /* Preserve the unsafe strategy as an expected negative mechanism case. */
    CHECK(!!(peer.caret==witness.windows['G'-'A'])==!!retain_membership && (newcomer.caret==witness.windows['G'-'A']));
    if(!repaired)repaired=AttachThreadInput(GetCurrentThreadId(),peer_id,TRUE);
    CHECK(repaired);if(!repaired)goto done;
    CHECK(input_witness_set(&witness,'F'));
    CHECK(read_probe(&peer) && (peer.caret==witness.windows['F'-'A']));
    CHECK(read_probe(&newcomer) && (newcomer.caret==witness.windows['F'-'A']));
done:
    if(joined)CHECK(AttachThreadInput(GetCurrentThreadId(),newcomer_id,FALSE));
    if(repaired)CHECK(AttachThreadInput(GetCurrentThreadId(),peer_id,FALSE));
    if(second)CHECK(AttachThreadInput(middle_id,peer_id,FALSE));
    if(first)CHECK(AttachThreadInput(GetCurrentThreadId(),middle_id,FALSE));
    DestroyCaret();
    close_probe(&middle,middle_thread);close_probe(&peer,peer_thread);
    if(newcomer_thread)close_probe(&newcomer,newcomer_thread);
}

static void repeated_attachment(void)
{
    input_probe peer;
    DWORD id;
    HANDLE thread=start_probe(&peer,&id);
    unsigned owned=0;
    BOOL result;
    result=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(result);if(!result)goto done;++owned;
    result=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(result);if(!result)goto done;++owned;
    result=AttachThreadInput(GetCurrentThreadId(),id,FALSE);
    CHECK(result);if(!result)goto done;--owned;
    CHECK(input_witness_set(&witness,'J'));
    CHECK(read_probe(&peer));
    printf("duplicate_attach_after_one_detach_shared=%u\n",(unsigned)!!(peer.caret==witness.windows['J'-'A']));
    CHECK(peer.caret==witness.windows['J'-'A']);
    result=AttachThreadInput(GetCurrentThreadId(),id,FALSE);
    CHECK(result);if(!result)goto done;--owned;
    CHECK(input_witness_set(&witness,'K'));
    CHECK(read_probe(&peer) && !(peer.caret==witness.windows['K'-'A']));
    /* Original _AttachThreadInput rejects removal without a matching record.
     * A redundant cleanup must not be counted as an owned successful detach. */
    CHECK(!AttachThreadInput(GetCurrentThreadId(),id,FALSE));
done:
    while(owned) {
        CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));--owned;
    }
    DestroyCaret();
    close_probe(&peer,thread);
}

static DWORD WINAPI exercise(void *unused)
{
    input_probe probe={0};
    HANDLE thread=NULL;
    DWORD id=0,exit_code=0;
    HWND current = NULL;
    MSG message;
    BOOL attached=FALSE;
    (void)unused;
    if (!SetThreadDesktop(desktop)) return 99;
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
    CHECK(input_witness_set(&witness,'A') && input_witness_read(&current) && current==witness.windows[0]);
    CHECK(read_probe(&probe) && !(probe.caret==witness.windows['A'-'A']));
    attached=AttachThreadInput(GetCurrentThreadId(),id,TRUE);
    CHECK(attached);if(!attached)goto done;
    CHECK(input_witness_set(&witness,'A'));
    CHECK(read_probe(&probe) && (probe.caret==witness.windows['A'-'A']));
    CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));attached=FALSE;
    CHECK(read_probe(&probe));
    /* Distinct window after detach; no assumption about retained prior state. */
    CHECK(input_witness_set(&witness,'B'));
    CHECK(read_probe(&probe) && !(probe.caret==witness.windows['B'-'A']));
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
            CHECK(input_witness_set(&witness,'C') && input_witness_read(&current) && current==witness.windows['C'-'A']);
        }
    }
done:
    if(attached)CHECK(AttachThreadInput(GetCurrentThreadId(),id,FALSE));
    DestroyCaret();
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
    input_witness_close(&witness);
    printf("WOW_NATIVE_INPUT_GROUP errors=%u isolated=2 shared=1 self_reject=1 peer_exit=1 naive_join_split=1 maintained_join=1 repeated=1\n",errors);
    return errors!=0;
}

int __cdecl main(void)
{
    WCHAR name[80];
    HANDLE thread;
    DWORD code = 99;
    wsprintfW(name,L"WowInputGroup-%lu",GetCurrentProcessId());
    desktop = CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if (!desktop) return 99;
    thread = CreateThread(NULL,0,exercise,NULL,0,NULL);
    if (!thread || WaitForSingleObject(thread,25000) != WAIT_OBJECT_0) return 99;
    CHECK(GetExitCodeThread(thread,&code));
    CloseHandle(thread);
    CHECK(CloseDesktop(desktop));
    return code || errors;
}
