/* Production channel/dispatcher, real pipes, threads and Console APIs.
 * Only broker attachment is substituted; this is not an RPC/authentication
 * or guest test. Run on an unswitched private-desktop Console. */
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <tlhelp32.h>
#include "ntkvm-exe/native_console_frontend.h"
#define run16_console_dispatch actual_dispatch
#include "../../src/ntkvm-exe/console_frontend.c"
#undef run16_console_dispatch
#include "../../src/ntkvm-exe/console_video.c"
static HANDLE read_entered,peer;
static FILE *private_report;
static HANDLE held_dispatch,release_dispatch;
static run16_native_frontend *test_frontend;
static DWORD expected_generation;
static DWORD WINAPI snapshot_contender(void *context)
{
    run16_native_frontend_snapshot_begin(test_frontend);
    SetEvent((HANDLE)context);
    run16_native_frontend_snapshot_end(test_frontend);
    return 0;
}
DWORD run16_console_dispatch(run16_console_frontend *owner,
    const console_io_request *request,console_io_reply *reply)
{
    if(held_dispatch && request->operation==CONSOLE_IO_BARRIER) {
        run16_native_frontend_snapshot_begin(test_frontend);
        SetEvent(held_dispatch);
        WaitForSingleObject(release_dispatch,INFINITE);
        run16_native_frontend_snapshot_end(test_frontend);
    }
    if(request->operation==CONSOLE_IO_READ_INPUT) SetEvent(read_entered);
    return actual_dispatch(owner,request,reply);
}
#include "../../src/ntkvm-exe/console_channel.c"
#define CHECK(x) do { if(!(x)) { DWORD check_error=GetLastError(); \
    fprintf(private_report ? private_report : stderr,"FAIL line=%u error=%lu\n", \
    (unsigned)__LINE__,check_error);if(private_report)fflush(private_report);ExitProcess(1); } } while(0)
typedef struct dos_binding_wait_case {
    const void *owner;
    HANDLE cancel,started;
    DWORD result;
} dos_binding_wait_case;
static DWORD WINAPI wait_for_dos_binding(void *context)
{
    dos_binding_wait_case *test=context;
    CHECK(run16_native_frontend_dos_bind(test_frontend,test->owner,TRUE)==ERROR_BUSY);
    CHECK(SetEvent(test->started));
    test->result=run16_native_frontend_wait_dos_ready(test_frontend,test->owner,
        test->cancel,10000);
    if(test->result)run16_native_frontend_cancel_dos_pending(test_frontend,test->owner);
    return 0;
}
/* Diagnostic-only self-process snapshot; never enters a product binary. */
static void audit_handles(const char *stage)
{
    typedef LONG (NTAPI *query_process)(HANDLE,ULONG,void *,ULONG,ULONG *);
    typedef LONG (NTAPI *query_object)(HANDLE,ULONG,void *,ULONG,ULONG *);
    typedef struct entry { HANDLE handle;ULONG_PTR handles,pointers;ULONG access,type,attributes,reserved; } entry;
    typedef struct snapshot { ULONG_PTR count,reserved;entry entries[1]; } snapshot;
    typedef struct text { USHORT length,capacity;WCHAR *value; } text;
    query_process qp=(query_process)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");
    query_object qo=(query_object)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryObject");
    ULONG bytes=1024*1024,needed=0,counts[256]={0};HANDLE representatives[256]={0};
    snapshot *info=HeapAlloc(GetProcessHeap(),0,bytes);ULONG_PTR i;DWORD total;
    CHECK(info && qp && qo && qp(GetCurrentProcess(),51,info,bytes,&needed)>=0);
    CHECK(info->count<=(bytes-sizeof(ULONG_PTR)*2)/sizeof(entry));
    for(i=0;i<info->count;++i) {
        CHECK(info->entries[i].type<256);
        ++counts[info->entries[i].type];representatives[info->entries[i].type]=info->entries[i].handle;
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&total));printf("AUDIT %s total=%lu\n",stage,total);
    for(i=0;i<256;++i)if(counts[i]) {
        BYTE buffer[8192];text *name=(text *)buffer;
        if(qo(representatives[i],2,buffer,sizeof(buffer),&needed)>=0)
            printf("AUDIT type=%lu count=%lu name=%.*ls\n",(ULONG)i,counts[i],name->length/2,name->value);
    }
    HeapFree(GetProcessHeap(),0,info);
}
DWORD OpenNtBaseClientAttachFrontendRequest(DWORD request,HANDLE pipe,
    HANDLE ready,DWORD *generation)
{
    (void)ready;
    if(request!=expected_generation) return ERROR_INVALID_PARAMETER;
    if(!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&peer,
        0,FALSE,DUPLICATE_SAME_ACCESS)) return GetLastError();
    *generation=request;return 0;
}
static void peer_io(BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while(bytes) {
        OVERLAPPED io={0};DWORD done=0;BOOL ok;
        io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(io.hEvent);
        ok=write ? WriteFile(peer,cursor,bytes,&done,&io) : ReadFile(peer,cursor,bytes,&done,&io);
        if(!ok) {
            CHECK(GetLastError()==ERROR_IO_PENDING);
            CHECK(WaitForSingleObject(io.hEvent,5000)==WAIT_OBJECT_0);
            CHECK(GetOverlappedResult(peer,&io,&done,FALSE));
        }
        CHECK(done && done<=bytes);CloseHandle(io.hEvent);cursor+=done;bytes-=done;
    }
}
static void verify_dos_input_queue(run16_console_channel *channel)
{
    INPUT_RECORD first[130]={0},second[130]={0},received[260];
    DWORD i,count,pending;HANDLE ready=run16_native_frontend_dos_ready(test_frontend);
    for(i=0;i<130;++i) {
        first[i].EventType=second[i].EventType=KEY_EVENT;
        first[i].Event.KeyEvent.wRepeatCount=second[i].Event.KeyEvent.wRepeatCount=1;
        first[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x100+i);
        second[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x200+i);
    }
    CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,second,130));
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,130));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_dos_read(test_frontend,TRUE,received,260,&count) && count==260);
    CHECK(!memcmp(received,first,sizeof(first)) && !memcmp(received+130,second,sizeof(second)));
    CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,17,&count) && count==17);
    CHECK(!memcmp(received,first,17*sizeof(*first)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(run16_native_frontend_dos_prepend(test_frontend,first,MAXDWORD)==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,0));
    CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,260,&count) && count==243);
    CHECK(!memcmp(received,first+17,113*sizeof(*first)) && !memcmp(received+113,second,sizeof(second)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,3));
    run16_native_frontend_dos_leave(test_frontend);
    /* Both backend channels consume the same frontend-owned unsent queue.
     * Handoff must not inject these records into the visible Console. */
    CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,FALSE));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(GetNumberOfConsoleInputEvents(channel->console.input,&pending) && pending==0);
    CHECK(!run16_native_frontend_native_bind(test_frontend,channel,TRUE));
    CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
    CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,3,&count) && count==3);
    CHECK(!memcmp(received,first,3*sizeof(*first)));
    run16_native_frontend_dos_leave(test_frontend);
    CHECK(!run16_native_frontend_native_bind(test_frontend,channel,FALSE));
    CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,TRUE));
    CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,260,&count)); } while(count);
    run16_native_frontend_dos_leave(test_frontend);
    puts("PASS DOS input growth, atomic prepend/order, peek, partial drain/readiness, overflow preservation and shared native-channel handoff");
}
static void run_case(unsigned mode,unsigned round)
{
    run16_console_channel *channel=NULL;
    HANDLE worker,thread,ready;
    PROCESS_INFORMATION child={0};
    console_io_request request={0};console_io_reply reply={0};
    DWORD exit_code;ULONGLONG started;
    expected_generation=1+round*6+mode;
    CHECK(ResetEvent(read_entered));
    if(mode==4) {
        STARTUPINFOW startup={sizeof(startup)};
        WCHAR image[MAX_PATH],command[MAX_PATH+32];
        DWORD length=GetModuleFileNameW(NULL,image,MAX_PATH);
        CHECK(length && length<MAX_PATH);
        CHECK(swprintf_s(command,MAX_PATH+32,L"\"%s\" --worker-wait",image)>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED,
            NULL,NULL,&startup,&child));
        CHECK(DuplicateHandle(GetCurrentProcess(),child.hProcess,GetCurrentProcess(),
            &worker,SYNCHRONIZE,FALSE,0));
    } else CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &worker,SYNCHRONIZE,FALSE,0));
    CHECK(!run16_console_channel_start_request(expected_generation,worker,test_frontend,&channel));
    {
        WCHAR cell;DWORD count,flags;COORD origin={0,0};
        CHECK(ReadConsoleOutputCharacterW(channel->console.output,&cell,1,origin,&count) && count==1 && cell==L'K');
        CHECK(GetHandleInformation(channel->console.output,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(GetHandleInformation(channel->console.input,&flags) && !(flags&HANDLE_FLAG_INHERIT));
    }
    if(mode==5)CHECK(!activate(channel,TRUE,CONSOLE_IO_WORKER_NATIVE));
    else CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,TRUE));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->thread,GetCurrentProcess(),
        &thread,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->ready,GetCurrentProcess(),
        &ready,0,FALSE,DUPLICATE_SAME_ACCESS));
    {
        INPUT_RECORD stale[64];DWORD count;
        CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
        CHECK(FlushConsoleInputBuffer(channel->console.input));
        do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,stale,64,&count)); } while(count);
        run16_native_frontend_dos_leave(test_frontend);
    }
    request.version=CONSOLE_IO_VERSION;request.generation=expected_generation;
    request.sequence=1;request.operation=CONSOLE_IO_BARRIER;
    peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
    peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
    CHECK(reply.result && reply.sequence==1 && reply.generation==expected_generation && !reply.bytes);
    if(mode==0 && round<4) {
        console_video_description description={4,2,4,8,8,{0}};
        HWND window=NULL;DWORD window_pid;ULONGLONG deadline;
        description.palette[1]=0xffffff;
        request.sequence=2;request.operation=CONSOLE_IO_VIDEO_BEGIN;
        request.state.mode=1;request.bytes=sizeof(description);
        memcpy(request.data,&description,sizeof(description));
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        request.sequence=3;request.operation=CONSOLE_IO_VIDEO_DATA;request.bytes=4;
        memset(request.data,1,4);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+4);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        Sleep(100);CHECK(!FindWindowW(NULL,L"NTVDM"));
        request.sequence=4;request.state.count=4;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+4);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        do {
            window=FindWindowW(NULL,L"NTVDM");
            if(window)break;
            Sleep(10);
        } while(GetTickCount64()<deadline);
        CHECK(window && GetWindowThreadProcessId(window,&window_pid) && window_pid==GetCurrentProcessId());
        if(!round) {
            INPUT_RECORD physical={0},records[64];DWORD count,written,i,keys=0;
            CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
            do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count)); } while(count);
            run16_native_frontend_dos_leave(test_frontend);
            physical.EventType=KEY_EVENT;physical.Event.KeyEvent.bKeyDown=TRUE;
            physical.Event.KeyEvent.wRepeatCount=1;physical.Event.KeyEvent.wVirtualKeyCode='Z';
            CHECK(WriteConsoleInputW(channel->console.input,&physical,1,&written) && written==1);
            CHECK(PostMessageW(window,WM_KEYDOWN,'A',0x001e0001));
            CHECK(PostMessageW(window,WM_KEYUP,'A',(LPARAM)0xc01e0001));
            deadline=GetTickCount64()+5000;
            while(keys<2 && GetTickCount64()<deadline) {
                CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
                CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count));
                run16_native_frontend_dos_leave(test_frontend);
                for(i=0;i<count;++i)if(records[i].EventType==KEY_EVENT) {
                    CHECK(records[i].Event.KeyEvent.wVirtualKeyCode=='A');
                    CHECK(records[i].Event.KeyEvent.bKeyDown==(keys==0));++keys;
                }
                if(keys<2)Sleep(10);
            }
            CHECK(keys==2);
            { DWORD_PTR result;
              CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'B',0x00300001,
                  SMTO_ABORTIFHUNG,3000,&result)); }
        }
        Sleep(100);CHECK(IsWindow(window)); /* No new frame is not TEXT. */
        request.sequence=5;request.operation=CONSOLE_IO_VIDEO_TEXT;
        request.state.mode=2;request.bytes=0;request.state.count=0;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        while(IsWindow(window) && GetTickCount64()<deadline)Sleep(10);
        CHECK(!IsWindow(window));
        if(!round) {
            INPUT_RECORD records[64];DWORD count,i,keys=0;
            CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
            CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count));
            run16_native_frontend_dos_leave(test_frontend);
            for(i=0;i<count;++i)if(records[i].EventType==KEY_EVENT) {
                CHECK(records[i].Event.KeyEvent.wVirtualKeyCode=='B');
                CHECK(records[i].Event.KeyEvent.bKeyDown==(keys==0));++keys;
            }
            CHECK(keys==2);
            puts("PASS Window retirement releases held DOS B before returning to Console");
        }
        if(!round)puts("PASS production channel complete graphics opens Window; partial/static frames and explicit TEXT retain correct mode");
        /* TEXT restored canonical output; fixture diagnostics above therefore
         * changed its first cell. Re-establish the next case's marker. */
        { DWORD written;COORD origin={0,0};
          CHECK(WriteConsoleOutputCharacterW(channel->console.output,L"K",1,origin,&written) && written==1); }
        request.sequence=6;request.operation=CONSOLE_IO_VIDEO_BEGIN;
        request.state.mode=3;request.bytes=sizeof(description);
        memcpy(request.data,&description,sizeof(description));
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        request.sequence=7;request.operation=CONSOLE_IO_VIDEO_DATA;request.bytes=8;
        memset(request.data,1,8);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+8);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        while(!FindWindowW(NULL,L"NTVDM") && GetTickCount64()<deadline)Sleep(10);
        CHECK(FindWindowW(NULL,L"NTVDM"));
    }
    if(mode==0 && !round) {
        uint32_t serial=channel->console.video.serial;
        console_video_description stale={0};
        CHECK(serial && channel->console.video.pixels);
        CHECK(!activate(channel,FALSE,CONSOLE_IO_WORKER_DOS));
        CHECK(!channel->console.video.pixels && !channel->console.video.pending &&
            channel->console.video.serial==serial);
        CHECK(!activate(channel,TRUE,CONSOLE_IO_WORKER_DOS));
        CHECK(run16_console_video_begin(&channel->console.video,serial,&stale)==ERROR_INVALID_DATA);
        puts("PASS ownership return discards stale pixels but retains the anti-replay serial");
    }
    if(mode==1) {
        if(!round)verify_dos_input_queue(channel);
        request.sequence=2;request.operation=CONSOLE_IO_READ_INPUT;
        request.state.input=1;request.state.count=1;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(read_entered,5000)==WAIT_OBJECT_0);
        /* Production binding holds the I/O lock: an empty read must return
         * immediately, otherwise native handoff/teardown would deadlock. */
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && !reply.state.count && !reply.bytes && reply.sequence==2);
        CHECK(WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
    } else if(mode==2) {
        /* Real channel callback reads frontend policy under the I/O lock.
         * A request alone must not fabricate a DOS frame or open a Window. */
        unsigned selection;
        request.sequence=2;request.operation=CONSOLE_IO_WINDOW_QUERY;
        request.state.mode=CONSOLE_WINDOW_TEXT_FRAME_REQUIRED;
        for(selection=0;selection<3;++selection) {
            BOOL wanted=selection==1;
            ULONGLONG deadline=GetTickCount64()+5000;
            if(wanted) {
                INPUT_RECORD keys[4]={0};console_io_input received[4];DWORD written;
                unsigned key;
                for(key=0;key<4;++key) {
                    keys[key].EventType=KEY_EVENT;
                    keys[key].Event.KeyEvent.bKeyDown=key!=2;
                    keys[key].Event.KeyEvent.wRepeatCount=1;
                    keys[key].Event.KeyEvent.wVirtualKeyCode=key==3 ? 'X' : 'F';
                    keys[key].Event.KeyEvent.wVirtualScanCode=key==3 ? 0x2d : 0x21;
                    keys[key].Event.KeyEvent.uChar.UnicodeChar=key==3 ? L'x' : 0;
                    keys[key].Event.KeyEvent.dwControlKeyState=key==3 ? 0 : LEFT_CTRL_PRESSED|LEFT_ALT_PRESSED;
                }
                CHECK(WriteConsoleInputW(channel->console.input,keys,4,&written) && written==4);
                CHECK(WaitForSingleObject(run16_native_frontend_dos_ready(test_frontend),5000)==WAIT_OBJECT_0);
                request.operation=CONSOLE_IO_PEEK_INPUT;request.state.mode=0;request.state.count=4;
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                if(!reply.result || reply.state.count!=1 || reply.bytes!=sizeof(console_io_input))
                    printf("CAF peek round=%u result=%lu count=%lu bytes=%lu status=%lu\n",
                        round,(DWORD)reply.result,(DWORD)reply.state.count,(DWORD)reply.bytes,(DWORD)reply.error);
                CHECK(reply.result && reply.state.count==1 && reply.bytes==sizeof(console_io_input));
                peer_io(FALSE,received,reply.bytes);++request.sequence;
                CHECK(received[0].virtual_key=='X'); /* Peek does not consume. */
                request.operation=CONSOLE_IO_READ_INPUT;
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                CHECK(reply.result && reply.state.count==1 && reply.bytes==sizeof(console_io_input));
                peer_io(FALSE,received,reply.bytes);++request.sequence;
                CHECK(received[0].virtual_key=='X' && received[0].character=='x');
                request.operation=CONSOLE_IO_WINDOW_QUERY;request.state.count=0;
                request.state.mode=CONSOLE_WINDOW_TEXT_FRAME_REQUIRED;
            } else CHECK(!run16_native_frontend_display(test_frontend,FALSE));
            do {
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                CHECK(reply.result && !reply.bytes);++request.sequence;
                if(reply.state.left==wanted)break;
                Sleep(10);
            } while(GetTickCount64()<deadline);
             /* A frame request disables the guest's original stream output.
              * Console must retain that path and its scrollback. */
             CHECK(reply.state.left==wanted && !FindWindowW(NULL,L"NTVDM"));
        }
        /* Acknowledged normal output followed by peer EOF races owner stop. */
        CHECK(CloseHandle(peer));peer=NULL;
    } else if(mode==3) {
        request.sequence=2;request.bytes=CONSOLE_IO_DATA_BYTES+1;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_INVALID_DATA);
        CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    } else if(mode==4) {
        CHECK(TerminateProcess(child.hProcess,91));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_PROCESS_ABORTED);
        CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    } else if(mode==5) {
        HANDLE acquired=CreateEventW(NULL,TRUE,FALSE,NULL),contender;
        CHECK(acquired);
        request.sequence=2;request.operation=CONSOLE_IO_SNAPSHOT_BEGIN;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && channel->snapshot_held);
        contender=CreateThread(NULL,0,snapshot_contender,acquired,0,NULL);CHECK(contender);
        CHECK(WaitForSingleObject(acquired,0)==WAIT_TIMEOUT);
        request.sequence=3;request.operation=CONSOLE_IO_SCREEN_INFO;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && reply.state.width>0 && reply.state.height>0);
        CHECK(WaitForSingleObject(acquired,0)==WAIT_TIMEOUT);
        request.sequence=4;request.operation=CONSOLE_IO_SNAPSHOT_END;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && !channel->snapshot_held);
        CHECK(WaitForSingleObject(acquired,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(contender,5000)==WAIT_OBJECT_0);
        CloseHandle(contender);ResetEvent(acquired);
        request.sequence=5;request.operation=CONSOLE_IO_SNAPSHOT_BEGIN;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && channel->snapshot_held);
        contender=CreateThread(NULL,0,snapshot_contender,acquired,0,NULL);CHECK(contender);
        CHECK(WaitForSingleObject(acquired,0)==WAIT_TIMEOUT);
        if(round&1) {
            request.sequence=6;request.operation=CONSOLE_IO_DOS_ACTIVE;
            peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
            CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_INVALID_DATA);
        } else { CloseHandle(peer);peer=NULL; }
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(acquired,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(contender,5000)==WAIT_OBJECT_0);
        CloseHandle(contender);CloseHandle(acquired);
        CHECK(!channel->snapshot_held);
        if(!round)puts("PASS native snapshot blocks concurrent screen owner until END; EOF and invalid activation release held lock");
    }
    if(mode==3 || mode==4) {
        OVERLAPPED io={0};BYTE byte;DWORD done=0,error;
        io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(io.hEvent);
        CHECK(!ReadFile(peer,&byte,1,&done,&io));error=GetLastError();
        if(error==ERROR_IO_PENDING) {
            CHECK(WaitForSingleObject(io.hEvent,5000)==WAIT_OBJECT_0);
            CHECK(!GetOverlappedResult(peer,&io,&done,FALSE));error=GetLastError();
        }
        CHECK(error==ERROR_BROKEN_PIPE && done==0);
        CloseHandle(io.hEvent);
    }
    started=GetTickCount64();
    CHECK(!run16_console_channel_stop(channel));
    CHECK(GetTickCount64()-started<5000);
    if(mode==0 && round<4)CHECK(!FindWindowW(NULL,L"NTVDM"));
    CHECK(WaitForSingleObject(thread,0)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&exit_code) && exit_code!=STILL_ACTIVE && exit_code!=0);
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CloseHandle(thread);CloseHandle(ready);
    if(child.hProcess) {CloseHandle(child.hThread);CloseHandle(child.hProcess);}
    if(peer) {CloseHandle(peer);peer=NULL;}
    if(mode==4) { DWORD handles,written;COORD origin={0,0};
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles));
        printf("completed graphics/channel round=%u handles=%lu tick=%llu\n",round,handles,GetTickCount64());
        CHECK(WriteConsoleOutputCharacterW(GetStdHandle(STD_OUTPUT_HANDLE),L"K",1,origin,&written)); }
}
static void wait_initial_window_resources(void)
{
    DWORD previous,current,quiet=0;ULONGLONG deadline=GetTickCount64()+10000;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&previous));
    /* The first Window starts OS text-input initialization after it closes.
     * No channels run during this bounded warm-up. Subsequent Window/channel
     * cycles still require exact handle equality, without a leak allowance. */
    do {
        Sleep(100);CHECK(GetProcessHandleCount(GetCurrentProcess(),&current));
        quiet=current==previous ? quiet+1 : 0;previous=current;
        if(quiet==20)return;
    } while(GetTickCount64()<deadline);
    CHECK(FALSE);
}
static void test_dos_geometry_handoff(HANDLE canonical,SHORT rows)
{
    run16_native_frontend *frontend=NULL;
    HANDLE input,output,active;
    CONSOLE_SCREEN_BUFFER_INFO final;
    CONSOLE_CURSOR_INFO shape,changed,restored;
    SMALL_RECT tiny={0,0,0,0},full={0,0,79,29},dos_view={0,0,79,rows-1};
    COORD outer_size={80,30},dos_size={80,rows};
    COORD old_cursor={0,29},dos_cursor={0,rows-1},old_cell={0,29};
    WCHAR cell;DWORD written,mode,restored_mode;

    /* This is a private Console API fixture, not a synthesized terminal.
     * The caller presents 80x30; the DOS mode switch retains the original
     * cursor-containing tail without reflowing any of its cells. */
    CHECK(SetConsoleActiveScreenBuffer(canonical));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&tiny));
    CHECK(opennt_console_resize_grid(canonical,&outer_size,FALSE,NULL));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&full));
    CHECK(WriteConsoleOutputCharacterW(canonical,L"C",1,old_cell,&written) && written==1);
    CHECK(SetConsoleCursorPosition(canonical,old_cursor));
    CHECK(GetConsoleCursorInfo(canonical,&shape));
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_console(frontend,&input,&output));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&dos_view));
    CHECK(opennt_console_resize_grid(canonical,&dos_size,FALSE,NULL));
    CHECK(SetConsoleCursorPosition(canonical,dos_cursor));
    changed=shape;changed.bVisible=!shape.bVisible;
    CHECK(SetConsoleCursorInfo(canonical,&changed));
    CHECK(GetConsoleMode(input,&mode));
    CHECK(SetConsoleMode(input,mode^ENABLE_PROCESSED_INPUT));
    CHECK(!run16_native_frontend_destroy(frontend));
    CHECK(GetConsoleScreenBufferInfo(canonical,&final));
    CHECK(final.dwSize.X==80 && final.dwSize.Y==rows &&
        final.srWindow.Left==0 && final.srWindow.Right==79 &&
        final.srWindow.Top==0 && final.srWindow.Bottom==rows-1 &&
        final.dwCursorPosition.X==0 && final.dwCursorPosition.Y==rows-1);
    CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,dos_cursor,&written) &&
        written==1 && cell==L'C');
    CHECK(GetConsoleCursorInfo(canonical,&restored));
    CHECK(restored.dwSize==shape.dwSize && restored.bVisible==shape.bVisible);
    CHECK(GetConsoleMode(input,&restored_mode) && restored_mode==mode);
    /* CONOUT$ opened after teardown must address the canonical active
     * buffer, not the temporary Window presentation surface. */
    active=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(active!=INVALID_HANDLE_VALUE);
    CHECK(ReadConsoleOutputCharacterW(active,&cell,1,dos_cursor,&written) &&
        written==1 && cell==L'C');
    CloseHandle(active);CloseHandle(output);CloseHandle(input);
}
static int run_private_desktop(const char *report,BOOL geometry_only)
{
    char name[64],image[MAX_PATH],command[2*MAX_PATH];
    HDESK desktop;STARTUPINFOA start={sizeof(start)};
    PROCESS_INFORMATION child={0};DWORD code=ERROR_GEN_FAILURE;
    sprintf_s(name,sizeof(name),"NTVDMHandoff-%lu",GetCurrentProcessId());
    desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop)return (int)GetLastError();
    CHECK(GetModuleFileNameA(NULL,image,sizeof(image)));
    sprintf_s(command,sizeof(command),"\"%s\" %s \"%s\"",image,
        geometry_only ? "--geometry-child" : "--private-full",report);
    start.lpDesktop=name;start.dwFlags=STARTF_USESHOWWINDOW;start.wShowWindow=SW_HIDE;
    if(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&start,&child)) {
        if(WaitForSingleObject(child.hProcess,120000)==WAIT_OBJECT_0)
            GetExitCodeProcess(child.hProcess,&code);
        else {TerminateProcess(child.hProcess,ERROR_TIMEOUT);
            WaitForSingleObject(child.hProcess,1000);code=ERROR_TIMEOUT;}
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    }else code=GetLastError();
    CloseDesktop(desktop);
    printf("private Console handoff exit=%lu report=%s\n",code,report);
    return (int)code;
}
int main(int argc,char **argv)
{
    unsigned round,mode;DWORD before,after;
    HANDLE input,canonical,alternate;DWORD count;COORD origin={0,0};
    CONSOLE_SCREEN_BUFFER_INFO current,restored;
    CONSOLE_CURSOR_INFO original_cursor={0},current_cursor={0},restored_cursor={0};
    SMALL_RECT reduced;
    if(argc==3 && !strcmp(argv[1],"--private-desktop"))return run_private_desktop(argv[2],TRUE);
    if(argc==3 && !strcmp(argv[1],"--private-desktop-full"))return run_private_desktop(argv[2],FALSE);
    if(argc==3 && !strcmp(argv[1],"--private-full")) {
        CHECK(!fopen_s(&private_report,argv[2],"w") && private_report);
        setvbuf(private_report,NULL,_IONBF,0);
    }
    if(argc==3 && !strcmp(argv[1],"--geometry-child")) {
        HANDLE geometry_output;
        CHECK(!fopen_s(&private_report,argv[2],"w") && private_report);
        setvbuf(private_report,NULL,_IONBF,0);
        geometry_output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        CHECK(geometry_output!=INVALID_HANDLE_VALUE);
        test_dos_geometry_handoff(geometry_output,25);
        test_dos_geometry_handoff(geometry_output,28);
        CloseHandle(geometry_output);
        fprintf(private_report,"PASS private 80x30 to 80x25/80x28 Console API handoff\n");
        fclose(private_report);return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--worker-wait")) {Sleep(INFINITE);return 0;}
    read_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(read_entered);
    CHECK(!run16_native_frontend_create(&test_frontend));
    if(argc==2 && !strcmp(argv[1],"--stop-timeout")) {
        run16_console_channel *channel=NULL;
        console_io_request request={0};HANDLE worker;ULONGLONG began;
        DWORD error;
        held_dispatch=CreateEventW(NULL,TRUE,FALSE,NULL);
        release_dispatch=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(held_dispatch && release_dispatch);
        expected_generation=777;
        CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            &worker,SYNCHRONIZE,FALSE,0));
        CHECK(!run16_console_channel_start_request(expected_generation,worker,test_frontend,&channel));
        CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,TRUE));
        request.version=CONSOLE_IO_VERSION;request.generation=expected_generation;
        request.sequence=1;request.operation=CONSOLE_IO_BARRIER;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(held_dispatch,1000)==WAIT_OBJECT_0);
        began=GetTickCount64();
        error=run16_console_channel_stop(channel);
        CHECK(error==ERROR_TIMEOUT && GetTickCount64()-began>=9500 &&
            GetTickCount64()-began<12000);
        /* Timeout retains the borrowed channel/root. Once the fault clears,
         * a second stop joins normally and all resources can be reclaimed. */
        CHECK(SetEvent(release_dispatch));
        CHECK(!run16_console_channel_stop(channel));
        CHECK(!run16_native_frontend_destroy(test_frontend));
        CloseHandle(peer);CloseHandle(held_dispatch);CloseHandle(release_dispatch);
        CloseHandle(read_entered);
        puts("PASS stopped channel returns ERROR_TIMEOUT within bound, retains borrowed storage, then joins after fault release");
        return 0;
    }
    CHECK(!run16_native_frontend_console(test_frontend,&input,&canonical));
    CHECK(GetConsoleCursorInfo(canonical,&original_cursor));
    CHECK(WriteConsoleOutputCharacterW(canonical,L"K",1,origin,&count) && count==1);
    alternate=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(alternate!=INVALID_HANDLE_VALUE);
    CHECK(WriteConsoleOutputCharacterW(alternate,L"A",1,origin,&count) && count==1);
    CHECK(SetConsoleActiveScreenBuffer(alternate));
    /* Warm up lazy runtime/Console resources before counting owned handles. */
    for(mode=0;mode<6;++mode) run_case(mode,0);
    if(argc==2 && !strcmp(argv[1],"--handle-audit")) {
        HANDLE modules;MODULEENTRY32W module={sizeof(module)};
        audit_handles("before-idle");Sleep(3000);audit_handles("after-idle-no-channels");
        modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
        CHECK(modules!=INVALID_HANDLE_VALUE && Module32FirstW(modules,&module));
        do { printf("MODULE %ls\n",module.szExePath); } while(Module32NextW(modules,&module));
        CloseHandle(modules);
        CHECK(WriteConsoleOutputCharacterW(canonical,L"K",1,origin,&count) && count==1);
    }
    wait_initial_window_resources();
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(round=1;round<=16;++round)
        for(mode=0;mode<6;++mode) run_case(mode,round);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    printf("channel handles before=%lu after=%lu\n",before,after);
    CHECK(after==before);
    {
        unsigned iteration;
        for(iteration=0;iteration<32;++iteration) {
            dos_binding_wait_case test={0};HANDLE waiter;
            test.owner=&after;
            test.cancel=CreateEventW(NULL,TRUE,FALSE,NULL);
            test.started=CreateEventW(NULL,TRUE,FALSE,NULL);
            CHECK(test.cancel && test.started);
            CHECK(!run16_native_frontend_native_bind(test_frontend,&before,TRUE));
            waiter=CreateThread(NULL,0,wait_for_dos_binding,&test,0,NULL);
            CHECK(waiter && WaitForSingleObject(test.started,1000)==WAIT_OBJECT_0);
            CHECK(SetEvent(test.cancel));
            CHECK(WaitForSingleObject(waiter,1000)==WAIT_OBJECT_0);
            CHECK(test.result==ERROR_OPERATION_ABORTED);
            CHECK(!run16_native_frontend_native_bind(test_frontend,&before,FALSE));
            /* A canceled DOS waiter must not reserve the next owner's slot. */
            CHECK(!run16_native_frontend_dos_bind(test_frontend,&round,TRUE));
            CHECK(!run16_native_frontend_dos_bind(test_frontend,&round,FALSE));
            CloseHandle(waiter);CloseHandle(test.cancel);CloseHandle(test.started);
        }
    }
    {
        ULONGLONG began=GetTickCount64();
        run16_native_frontend_cancel(test_frontend);
        CHECK(run16_native_frontend_dos_bind(test_frontend,&before,TRUE)==ERROR_OPERATION_ABORTED);
        CHECK(GetTickCount64()-began<1000);
    }
    /* Ordinary teardown keeps the most recent shared Console geometry and
     * cursor while restoring cursor shape and input mode. */
    CHECK(GetConsoleScreenBufferInfo(canonical,&current));
    CHECK(GetConsoleCursorInfo(canonical,&current_cursor));
    reduced=current.srWindow;
    CHECK(reduced.Bottom>reduced.Top);
    --reduced.Bottom;
    CHECK(opennt_console_resize_grid(canonical,NULL,TRUE,&reduced));
    CHECK(SetConsoleCursorPosition(canonical,(COORD){reduced.Left,reduced.Top}));
    current_cursor.bVisible=!current_cursor.bVisible;
    CHECK(SetConsoleCursorInfo(canonical,&current_cursor));
    CHECK(!run16_native_frontend_destroy(test_frontend));
    CHECK(GetConsoleScreenBufferInfo(canonical,&restored));
    CHECK(!memcmp(&restored.srWindow,&reduced,sizeof(reduced)) &&
        restored.dwCursorPosition.X==reduced.Left &&
        restored.dwCursorPosition.Y==reduced.Top);
    CHECK(GetConsoleCursorInfo(canonical,&restored_cursor));
    CHECK(restored_cursor.dwSize==original_cursor.dwSize &&
        restored_cursor.bVisible==original_cursor.bVisible);
    test_dos_geometry_handoff(canonical,25);
    test_dos_geometry_handoff(canonical,28);
    CloseHandle(alternate);CloseHandle(canonical);CloseHandle(input);
    CloseHandle(read_entered);
    puts("PASS 102 real channel lifetimes: pipe cancel, nonblocking empty input, barrier/EOF/stop race, oversized request, real process death, snapshot lock; joined threads, EOF readiness, no handle growth");
    puts("PASS four graphics channels each create/retire two Windows, including active graphics disposal; exact post-warm-up handle equality");
    puts("PASS all nested channels retain canonical K while active surface is A; private handle copies survive frontend teardown");
    puts("PASS ordinary teardown retains the final 80x25/80x28 DOS grid and cursor; restores canonical buffer, input mode and cursor shape");
    puts("PASS stopped presentation owner rejects handoff without waiting for execution or restarting a helper");
    puts("PASS 32 canceled DOS binding waits: no lost stop wake and no pending-owner leak");
    if(private_report) {fprintf(private_report,"PASS private Console handoff fixture\n");fclose(private_report);}
    return 0;
}
