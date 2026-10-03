/* Production channel/dispatcher, real pipes, threads and Console APIs.
 * Only broker attachment is substituted; this is not an RPC/authentication
 * or guest test. Run on an unswitched private-desktop Console. */
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <tlhelp32.h>
#include "ntcon-exe/native_console_frontend.h"
#include "console_geometry_fixture.h"
#define run16_console_dispatch actual_dispatch
#include "../../src/ntcon-exe/console_frontend.c"
#undef run16_console_dispatch
#include "../../src/ntcon-exe/console_video.c"
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
#ifdef NTCON_FRAME_FAILURE_TEST
    if((request->operation==CONSOLE_IO_VIDEO_DATA || request->operation==CONSOLE_IO_PUBLICATION_END) &&
        InterlockedCompareExchange(&arm_pipe_projection,0,1)==1)
        fail_projection_target=test_frontend->console_output;
#endif
    if(held_dispatch && request->operation==CONSOLE_IO_BARRIER) {
        run16_native_frontend_snapshot_begin(test_frontend);
        SetEvent(held_dispatch);
        WaitForSingleObject(release_dispatch,INFINITE);
        run16_native_frontend_snapshot_end(test_frontend);
    }
    if(request->operation==CONSOLE_IO_READ_INPUT) SetEvent(read_entered);
    return actual_dispatch(owner,request,reply);
}
#include "../../src/ntcon-exe/console_channel.c"
#define CHECK(x) do { if(!(x)) { DWORD check_error=GetLastError(); \
    fprintf(private_report ? private_report : stderr,"FAIL line=%u error=%lu\n", \
    (unsigned)__LINE__,check_error);if(private_report)fflush(private_report);ExitProcess(1); } } while(0)
/* Fixtures explicitly compose acquisition with the worker's requested VGA
 * geometry. Production bind itself is worker-neutral and selects no mode. */
static DWORD test_bind(run16_native_frontend *frontend,const void *owner,BOOL active,BOOL request_vga)
{
    DWORD error=run16_native_frontend_bind(frontend,owner,active);
    if(!error && active && request_vga) {
        SMALL_RECT region;
        error=run16_native_frontend_enter(frontend,owner);
        if(!error) {
            region=*run16_native_frontend_text_region(frontend);
            run16_native_frontend_leave(frontend);
            error=run16_native_frontend_prepare_text(frontend,owner,(COORD){80,
                ntvdm_console_return_height(region.Bottom-region.Top+1)},NULL);
        }
    }
    return error;
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
    DWORD i,count,pending;HANDLE ready=run16_native_frontend_ready(test_frontend);
    for(i=0;i<130;++i) {
        first[i].EventType=second[i].EventType=KEY_EVENT;
        first[i].Event.KeyEvent.wRepeatCount=second[i].Event.KeyEvent.wRepeatCount=1;
        first[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x100+i);
        second[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x200+i);
    }
    CHECK(!run16_native_frontend_enter(test_frontend,channel));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(!run16_native_frontend_prepend(test_frontend,second,130));
    CHECK(!run16_native_frontend_prepend(test_frontend,first,130));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_read(test_frontend,TRUE,received,260,&count) && count==260);
    CHECK(!memcmp(received,first,sizeof(first)) && !memcmp(received+130,second,sizeof(second)));
    CHECK(!run16_native_frontend_read(test_frontend,FALSE,received,17,&count) && count==17);
    CHECK(!memcmp(received,first,17*sizeof(*first)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(run16_native_frontend_prepend(test_frontend,first,MAXDWORD)==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(!run16_native_frontend_prepend(test_frontend,first,0));
    CHECK(!run16_native_frontend_read(test_frontend,FALSE,received,260,&count) && count==243);
    CHECK(!memcmp(received,first+17,113*sizeof(*first)) && !memcmp(received+113,second,sizeof(second)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    CHECK(!run16_native_frontend_prepend(test_frontend,first,3));
    run16_native_frontend_leave(test_frontend);
    /* Both backend channels consume the same frontend-owned unsent queue.
     * Handoff must not inject these records into the visible Console. */
    CHECK(!test_bind(test_frontend,channel,FALSE,TRUE));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(GetNumberOfConsoleInputEvents(channel->console.input,&pending) && pending==0);
    CHECK(!test_bind(test_frontend,channel,TRUE,FALSE));
    CHECK(!run16_native_frontend_enter(test_frontend,channel));
    CHECK(!run16_native_frontend_read(test_frontend,FALSE,received,3,&count) && count==3);
    CHECK(!memcmp(received,first,3*sizeof(*first)));
    run16_native_frontend_leave(test_frontend);
    CHECK(!test_bind(test_frontend,channel,FALSE,FALSE));
    CHECK(!test_bind(test_frontend,channel,TRUE,TRUE));
    CHECK(!run16_native_frontend_enter(test_frontend,channel));
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    do { CHECK(!run16_native_frontend_read(test_frontend,FALSE,received,260,&count)); } while(count);
    run16_native_frontend_leave(test_frontend);
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
    CHECK(!activate(channel,TRUE));
    if(mode<5)CHECK(!prepare_text(channel,(COORD){80,25}));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->thread,GetCurrentProcess(),
        &thread,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->ready,GetCurrentProcess(),
        &ready,0,FALSE,DUPLICATE_SAME_ACCESS));
    {
        INPUT_RECORD stale[64];DWORD count;
        CHECK(!run16_native_frontend_enter(test_frontend,channel));
        CHECK(FlushConsoleInputBuffer(channel->console.input));
        do { CHECK(!run16_native_frontend_read(test_frontend,FALSE,stale,64,&count)); } while(count);
        run16_native_frontend_leave(test_frontend);
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
        Sleep(100);CHECK(!FindWindowW(L"LibKvmWindow",NULL));
        request.sequence=4;request.state.count=4;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+4);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        do {
            window=FindWindowW(L"LibKvmWindow",NULL);
            if(window)break;
            Sleep(10);
        } while(GetTickCount64()<deadline);
        CHECK(window && GetWindowThreadProcessId(window,&window_pid) && window_pid==GetCurrentProcessId());
        if(!round) {
            INPUT_RECORD physical={0},records[64];DWORD count,written,i,keys=0;
            CHECK(!run16_native_frontend_enter(test_frontend,channel));
            do { CHECK(!run16_native_frontend_read(test_frontend,FALSE,records,64,&count)); } while(count);
            run16_native_frontend_leave(test_frontend);
            physical.EventType=KEY_EVENT;physical.Event.KeyEvent.bKeyDown=TRUE;
            physical.Event.KeyEvent.wRepeatCount=1;physical.Event.KeyEvent.wVirtualKeyCode='Z';
            CHECK(WriteConsoleInputW(channel->console.input,&physical,1,&written) && written==1);
            CHECK(PostMessageW(window,WM_KEYDOWN,'A',0x001e0001));
            CHECK(PostMessageW(window,WM_KEYUP,'A',(LPARAM)0xc01e0001));
            deadline=GetTickCount64()+5000;
            while(keys<2 && GetTickCount64()<deadline) {
                CHECK(!run16_native_frontend_enter(test_frontend,channel));
                CHECK(!run16_native_frontend_read(test_frontend,FALSE,records,64,&count));
                run16_native_frontend_leave(test_frontend);
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
            CHECK(!run16_native_frontend_enter(test_frontend,channel));
            CHECK(!run16_native_frontend_read(test_frontend,FALSE,records,64,&count));
            run16_native_frontend_leave(test_frontend);
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
        while(!FindWindowW(L"LibKvmWindow",NULL) && GetTickCount64()<deadline)Sleep(10);
        CHECK(FindWindowW(L"LibKvmWindow",NULL));
    }
    if(mode==0 && !round) {
        uint32_t serial=channel->console.video.serial;
        console_video_description stale={0};
        CHECK(serial && channel->console.video.pixels);
        CHECK(!activate(channel,FALSE));
        CHECK(!channel->console.video.pixels && !channel->console.video.pending &&
            channel->console.video.serial==serial);
        CHECK(!activate(channel,TRUE));
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
                CHECK(WaitForSingleObject(run16_native_frontend_ready(test_frontend),5000)==WAIT_OBJECT_0);
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
             CHECK(reply.state.left==wanted && !FindWindowW(L"LibKvmWindow",NULL));
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
            request.sequence=6;request.operation=CONSOLE_IO_ACTIVATE;
            peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
            CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_INVALID_DATA);
        } else { CloseHandle(peer);peer=NULL; }
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(acquired,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(contender,5000)==WAIT_OBJECT_0);
        CloseHandle(contender);CloseHandle(acquired);
        CHECK(!channel->snapshot_held);
        if(!round)puts("PASS channel snapshot blocks concurrent screen owner until END; EOF and invalid activation release held lock");
    }
    if(mode==6) {
        HANDLE committed;WCHAR cell;DWORD count;
        console_io_cell replacement={'R',0x4f};
        request.sequence=2;request.operation=CONSOLE_IO_PUBLICATION_BEGIN;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        request.sequence=3;request.operation=CONSOLE_IO_WRITE_CELLS_W;
        request.state.width=request.state.height=1;request.bytes=sizeof(replacement);
        memcpy(request.data,&replacement,sizeof(replacement));
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        CloseHandle(peer);peer=NULL;
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(!channel->publication_surface);
        CHECK(!run16_native_frontend_logical_console(test_frontend,&committed));
        CHECK(ReadConsoleOutputCharacterW(committed,&cell,1,(COORD){0,0},&count) && count==1 && cell==L'K');
        CloseHandle(committed);
        fprintf(private_report ? private_report : stdout,"PASS real pipe EOF aborts unpublished channel grid without replacing committed cells\n");
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
    if(mode==0 && round<4)CHECK(!FindWindowW(L"LibKvmWindow",NULL));
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
static void test_native_projected_viewport(HANDLE output)
{
    run16_console_frontend owner={0};console_io_request request={0};console_io_reply reply;
    SMALL_RECT logical={0,0,79,27};CONSOLE_SCREEN_BUFFER_INFO info;DWORD count;WCHAR cell;
    CHECK(SetConsoleWindowInfo(output,TRUE,&(SMALL_RECT){0,0,79,29}));
    CHECK(SetConsoleScreenBufferSize(output,(COORD){80,30}));
    CHECK(WriteConsoleOutputCharacterW(output,L"N",1,(COORD){0,29},&count) && count==1);
    owner.output=output;owner.generation=1;owner.logical_window=&logical;
    request.version=CONSOLE_IO_VERSION;request.generation=1;request.sequence=1;
    request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
    request.state.top=2;request.state.right=79;request.state.bottom=29;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.Y==30);
    CHECK(logical.Top==2 && logical.Bottom==29);
    ++request.sequence;request.operation=CONSOLE_IO_CURSOR_POSITION;
    request.state.x=0;request.state.y=29;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwCursorPosition.Y==29);
    CHECK(ReadConsoleOutputCharacterW(output,&cell,1,(COORD){0,29},&count) && count==1 && cell==L'N');
    /* A genuine storage shrink must still work for native full-screen TUIs. */
    ++request.sequence;request.operation=CONSOLE_IO_BUFFER_SIZE;
    request.state.width=80;request.state.height=25;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.Y==25 && info.dwCursorPosition.Y==24);
    fprintf(private_report ? private_report : stdout,
        "PASS native viewport metadata preserves storage/last-row cursor; explicit TUI shrink remains valid\n");
}
/* A DOS logical viewport can already match the native alternate screen while
 * the canonical Console still has the larger caller viewport. A WINDOW_RECT
 * acknowledgment must cover the real physical projection before BUFFER_SIZE. */
static void test_native_geometry_projection(SHORT rows)
{
    run16_console_frontend owner={0};
    console_io_request request={0};console_io_reply reply;
    SMALL_RECT logical={0,0,79,(SHORT)(rows-1)},large={0,0,79,29};
    CONSOLE_SCREEN_BUFFER_INFO info;
    WCHAR cell;DWORD count;
    HANDLE output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(output!=INVALID_HANDLE_VALUE);
    CHECK(GetConsoleScreenBufferInfo(output,&info));
    CHECK(SetConsoleScreenBufferSize(output,(COORD){max(info.dwSize.X,80),max(info.dwSize.Y,60)}));
    CHECK(SetConsoleWindowInfo(output,TRUE,&large));
    CHECK(SetConsoleScreenBufferSize(output,(COORD){80,60}));
    CHECK(WriteConsoleOutputCharacterW(output,L"P",1,(COORD){7,17},&count) && count==1);
    CHECK(SetConsoleCursorPosition(output,(COORD){3,11}));
    owner.output=output;owner.generation=1;owner.logical_window=&logical;
    request.version=CONSOLE_IO_VERSION;request.generation=1;request.sequence=1;
    request.operation=CONSOLE_IO_WINDOW_RECT;request.state.mode=1;
    request.state.right=logical.Right;request.state.bottom=logical.Bottom;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info));
    CHECK(!memcmp(&info.srWindow,&large,sizeof(large)));
    CHECK(logical.Bottom==rows-1 && info.dwSize.Y==60);
    CHECK(info.dwCursorPosition.X==3 && info.dwCursorPosition.Y==11);
    /* A second identical request is a verified no-op, not a cached guess. */
    ++request.sequence;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    ++request.sequence;request.operation=CONSOLE_IO_BUFFER_SIZE;
    request.state.width=80;request.state.height=rows;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.X==80 && info.dwSize.Y==rows);
    CHECK(ReadConsoleOutputCharacterW(output,&cell,1,(COORD){7,17},&count) && count==1 && cell==L'P');
    CHECK(info.dwCursorPosition.X==3 && info.dwCursorPosition.Y==11);
    ++request.sequence;request.operation=CONSOLE_IO_WINDOW_RECT;
    request.state.bottom=rows;
    CHECK(!actual_dispatch(&owner,&request,&reply) && reply.error==ERROR_INVALID_PARAMETER);
    CHECK(logical.Bottom==rows-1);
    ++request.sequence;request.operation=CONSOLE_IO_BUFFER_SIZE;
    request.state.height=60;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    ++request.sequence;request.operation=CONSOLE_IO_WINDOW_RECT;
    request.state.bottom=29;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.Y==60 && logical.Bottom==29);
    CHECK(ReadConsoleOutputCharacterW(output,&cell,1,(COORD){7,17},&count) && count==1 && cell==L'P');
    CHECK(info.dwCursorPosition.X==3 && info.dwCursorPosition.Y==11);
    test_native_projected_viewport(output);
    CloseHandle(output);
    fprintf(private_report ? private_report : stdout,
        "PASS native logical/physical 80x30 -> 80x%d projection, repeat, shrink/grow, cells/cursor and invalid rectangle\n",rows);
}
static void test_native_seed_origin(HANDLE canonical)
{
    run16_native_frontend *frontend=NULL;
    run16_console_frontend owner={0};
    console_io_request request={0};console_io_reply reply;
    CONSOLE_SCREEN_BUFFER_INFO info;
    SMALL_RECT tiny={0,0,0,0},view={4,2,83,29},*logical;
    DWORD mode,written;WCHAR cell;
    HANDLE input,output;
    CHECK(SetConsoleActiveScreenBuffer(canonical));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&tiny));
    CHECK(SetConsoleScreenBufferSize(canonical,(COORD){120,100}));
    CHECK(SetConsoleCursorPosition(canonical,(COORD){4,29}));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&view));
    CHECK(GetConsoleMode(canonical,&mode));
    CHECK(SetConsoleMode(canonical,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING));
    CHECK(WriteConsoleOutputCharacterW(canonical,L"K",1,(COORD){83,2},&written) && written==1);
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_console(frontend,&input,&output));
    logical=run16_native_frontend_text_region(frontend);
    CHECK(!test_bind(frontend,&owner,TRUE,FALSE));
    CloseHandle(output);
    CHECK(!run16_native_frontend_logical_console(frontend,&output));
    owner.output=output;owner.logical_window=logical;owner.generation=1;
    request.version=CONSOLE_IO_VERSION;request.generation=1;request.sequence=1;
    request.operation=CONSOLE_IO_SCREEN_INFO;
    CHECK(!actual_dispatch(&owner,&request,&reply) && !reply.error);
    CHECK(reply.state.left==4 && reply.state.top==2 &&
        reply.state.right==83 && reply.state.bottom==29 &&
        reply.state.x==4 && reply.state.y==29);
    CHECK(GetConsoleScreenBufferInfo(canonical,&info));
    CHECK(!memcmp(&info.srWindow,&view,sizeof(view)) && info.dwSize.X==120 && info.dwSize.Y==100);
    CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,(COORD){83,2},&written) && written==1 && cell==L'K');
    CHECK(GetConsoleMode(canonical,&written) && written==(mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING));
    CHECK(WriteConsoleW(canonical,L"S10-SEED\r\n",10,&written,NULL) && written==10);
    CHECK(GetConsoleScreenBufferInfo(canonical,&info) &&
        info.dwCursorPosition.X==0 && info.dwCursorPosition.Y==30);
    CHECK(!test_bind(frontend,&owner,FALSE,FALSE));
    CHECK(!run16_native_frontend_destroy(frontend));
    CHECK(GetConsoleScreenBufferInfo(canonical,&info) && info.dwCursorPosition.Y==30);
    CHECK(SetConsoleMode(canonical,mode));
    CloseHandle(output);CloseHandle(input);
    fprintf(private_report ? private_report : stdout,
        "PASS native seed absolute origin, unchanged logical extent/grid/cursor/VT, real CRLF and teardown\n");
}
static void test_dos_conversion_thresholds(void)
{
    const SHORT heights[]={23,24,26,27,30,35,36,46,47,50};
    const SHORT expected[]={22,25,25,28,28,28,43,43,50,50};
    for(unsigned index=0;index<sizeof(heights)/sizeof(*heights);++index) {
        HANDLE output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        CONSOLE_SCREEN_BUFFER_INFO info;SMALL_RECT physical={0,0,79,19};
        SMALL_RECT logical={0,0,79,heights[index]-1};DWORD count;WCHAR cell;
        CHECK(output!=INVALID_HANDLE_VALUE && SetConsoleWindowInfo(output,TRUE,&physical));
        CHECK(SetConsoleScreenBufferSize(output,(COORD){80,heights[index]}));
        for(SHORT row=0;row<heights[index];++row) {
            WCHAR marker=L'A'+row%26;
            CHECK(WriteConsoleOutputCharacterW(output,&marker,1,(COORD){0,row},&count) && count==1);
        }
        CHECK(SetConsoleCursorPosition(output,(COORD){0,heights[index]-1}));
        CHECK(!test_prepare_vga(output,&logical));
        CHECK(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.X==80 &&
            info.dwSize.Y==expected[index] && info.dwCursorPosition.Y==min(heights[index]-1,expected[index]-1));
        CHECK(logical.Top==0 && logical.Bottom==expected[index]-1);
        CHECK(ReadConsoleOutputCharacterW(output,&cell,1,(COORD){0,0},&count) && count==1 &&
            cell==L'A'+max(0,heights[index]-expected[index])%26);
        if(expected[index]>heights[index])
            CHECK(ReadConsoleOutputCharacterW(output,&cell,1,(COORD){0,expected[index]-1},&count) && count==1 && cell==L' ');
        CloseHandle(output);
    }
    fprintf(private_report ? private_report : stdout,
        "PASS ten original VGA height thresholds; cursor-containing row retention and blank growth\n");
}
typedef struct publication_read_case {
    WCHAR cell;
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD error;
} publication_read_case;
static DWORD WINAPI publication_reader(void *context)
{
    publication_read_case *read=context;
    HANDLE logical=NULL;DWORD count;
    run16_native_frontend_snapshot_begin(test_frontend);
    read->error=run16_native_frontend_logical_console(test_frontend,&logical);
    if(!read->error && (!ReadConsoleOutputCharacterW(logical,&read->cell,1,
        (COORD){0,2},&count) || count!=1 || !GetConsoleScreenBufferInfo(logical,&read->info)))
        read->error=GetLastError() ? GetLastError() : ERROR_READ_FAULT;
    if(logical)CloseHandle(logical);
    run16_native_frontend_snapshot_end(test_frontend);
    return read->error;
}
static void test_prepare_operation(void)
{
    run16_console_channel channel={0};console_io_request request={0};console_io_reply reply;
    CONSOLE_SCREEN_BUFFER_INFO info;
    channel.root=test_frontend;channel.console.generation=1;
    channel.console.io_context=&channel;channel.console.activate=activate;
    channel.console.prepare_text=prepare_text;channel.console.enter=enter;channel.console.leave=leave;
    request.version=CONSOLE_IO_VERSION;request.generation=1;request.sequence=1;
    request.operation=CONSOLE_IO_PREPARE_TEXT_REGION;request.state.width=100;request.state.height=35;
    CHECK(!actual_dispatch(&channel.console,&request,&reply) && !reply.result && reply.error==ERROR_NOT_READY);
    request.sequence=2;request.operation=CONSOLE_IO_ACTIVATE;request.state.mode=1;
    CHECK(actual_dispatch(&channel.console,&request,&reply)==ERROR_INVALID_DATA && channel.console.sequence==1);
    request.state.mode=0;request.state.input=1;
    CHECK(!actual_dispatch(&channel.console,&request,&reply) && reply.result);
    request.sequence=3;request.operation=CONSOLE_IO_PREPARE_TEXT_REGION;
    CHECK(!actual_dispatch(&channel.console,&request,&reply) && reply.result);
    CHECK(GetConsoleScreenBufferInfo(channel.console.output,&info) && info.dwSize.X==100 && info.dwSize.Y==35);
    request.sequence=4;request.state.width=0;
    CHECK(actual_dispatch(&channel.console,&request,&reply)==ERROR_INVALID_DATA && channel.console.sequence==3);
    CHECK(GetConsoleScreenBufferInfo(channel.console.output,&info) && info.dwSize.X==100 && info.dwSize.Y==35);
    CHECK(!activate(&channel,FALSE));
    run16_native_frontend_forget(test_frontend,&channel);CloseHandle(channel.console.output);
    fprintf(private_report ? private_report : stdout,
        "PASS operation-only activation and exact text-region request: inactive denied, reserved kind rejected, invalid geometry preserves state\n");
}
static void test_standalone_text(BOOL native,BOOL styled)
{
    run16_console_channel channel={0};
    console_video_description description={0};
    console_text_style *style;
    BYTE *payload,*text;
    HANDLE logical;
    DWORD count,index,bytes,step=styled ? 3 : 2;
    WCHAR cell;WORD attribute;
    channel.root=test_frontend;
    CHECK(!test_bind(test_frontend,&channel,TRUE,!native));
    CHECK(!run16_native_frontend_logical_console(test_frontend,&channel.console.output));
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    description.width=80;description.height=25;description.stride=80*step;
    description.bytes=sizeof(console_text_style)+description.stride*25;
    bytes=description.bytes;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);CHECK(payload);
    style=(console_text_style *)payload;style->font_height=16;
    style->cursor_visible=1;style->cursor_column=3;style->cursor_row=24;style->cursor_height=4;
    text=payload+sizeof(*style);
    for(index=0;index<80*25;++index){text[index*step]=' ';text[index*step+1]=7;}
    text[0]='K';text[1]=0x1e;if(styled)text[2]=CONSOLE_TEXT_UNDERLINE;
    CHECK(!enter(&channel));
    CHECK(!run16_console_video_begin(&channel.console.video,1,&description));
    CHECK(!video_data(&channel,1,0,payload,bytes/2));
    CHECK(!channel.console.video.pixels && channel.console.video.pending && !channel.console.video.pending_validated);
    CHECK(!video_data(&channel,1,bytes/2,payload+bytes/2,bytes-bytes/2));
    CHECK(channel.console.video.published_serial==1 && !channel.console.video.pending && !channel.publication_terminal_error);
    CHECK(!run16_native_frontend_logical_console(test_frontend,&logical));
    CHECK(ReadConsoleOutputCharacterW(logical,&cell,1,(COORD){0,0},&count) && count==1 && cell==L'K');
    CHECK(ReadConsoleOutputAttribute(logical,&attribute,1,(COORD){0,0},&count) && count==1 &&
        attribute==(WORD)(0x1e|(styled ? COMMON_LVB_UNDERSCORE : 0)));
    if(styled) {
        /* A bad optional style cannot replace either the committed frame
         * or grid; the type-neutral validator still rejects it. */
        text[2]=CONSOLE_TEXT_STYLE_MASK+1;
        CHECK(!run16_console_video_begin(&channel.console.video,2,&description));
        CHECK(video_data(&channel,2,0,payload,bytes)==ERROR_INVALID_DATA);
        CHECK(channel.console.video.published_serial==1 && !channel.console.video.pending);
        CHECK(ReadConsoleOutputCharacterW(logical,&cell,1,(COORD){0,0},&count) && cell==L'K');
    }
    CloseHandle(logical);
    leave(&channel);
    CHECK(!test_bind(test_frontend,&channel,FALSE,FALSE));
    run16_native_frontend_forget(test_frontend,&channel);
    run16_console_video_dispose(&channel.console.video);HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(channel.console.output);
    fprintf(private_report ? private_report : stdout,
        "PASS standalone text kind=%u styled=%u committed glyph/attribute, optional underline and malformed-style preservation\n",
        (unsigned)native,(unsigned)styled);
}
#ifdef NTCON_FRAME_FAILURE_TEST
static void test_projection_failure_pipe(BOOL batch)
{
    run16_console_channel *channel=NULL;
    console_io_request request={0};console_io_reply reply;
    console_video_description description={0};
    console_text_style *style;
    BYTE *payload,*text;HANDLE worker;
    DWORD bytes,index,offset,count,code;WCHAR cell;
    expected_generation=batch ? 922 : 921;
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),&worker,SYNCHRONIZE,FALSE,0));
    CHECK(!run16_console_channel_start_request(expected_generation,worker,test_frontend,&channel));
    CHECK(!activate(channel,TRUE));
    CHECK(!prepare_text(channel,(COORD){80,25}));
    request.version=CONSOLE_IO_VERSION;request.generation=expected_generation;
    if(batch) {
        request.sequence++;request.operation=CONSOLE_IO_PUBLICATION_BEGIN;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        CHECK(!enter(channel));
        CHECK(SetConsoleCursorPosition(channel->console.output,(COORD){0,0}));
        CHECK(WriteConsoleOutputCharacterW(channel->console.output,L"P",1,(COORD){0,0},&count));
        leave(channel);
    }
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=80;description.height=25;
    description.stride=160;bytes=description.bytes=sizeof(*style)+160*25;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);CHECK(payload);
    style=(console_text_style *)payload;style->font_height=16;
    style->cursor_visible=1;style->cursor_height=4;
    text=payload+sizeof(*style);
    for(index=0;index<80*25;++index){text[index*2]=' ';text[index*2+1]=7;}
    text[0]='P';
    request.sequence++;request.operation=CONSOLE_IO_VIDEO_BEGIN;
    request.state.mode=1;request.bytes=sizeof(description);
    memcpy(request.data,&description,sizeof(description));
    peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
    peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
    for(offset=0;offset<bytes;offset+=count) {
        count=min(bytes-offset,CONSOLE_IO_DATA_BYTES);
        request.sequence++;request.operation=CONSOLE_IO_VIDEO_DATA;
        request.state.count=offset;request.bytes=count;memcpy(request.data,payload+offset,count);
        if(!batch && offset+count==bytes)InterlockedExchange(&arm_pipe_projection,1);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+count);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result==(BOOL)(batch || offset+count<bytes));
    }
    if(batch) {
        request.sequence++;request.operation=CONSOLE_IO_PUBLICATION_END;request.bytes=0;
        InterlockedExchange(&arm_pipe_projection,1);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(!reply.result);
    }
    CHECK(reply.error==ERROR_WRITE_FAULT && !arm_pipe_projection);
    CHECK(WaitForSingleObject(channel->thread,3000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(channel->thread,&code) && code==ERROR_WRITE_FAULT);
    CHECK(channel->publication_terminal_error==ERROR_WRITE_FAULT && !channel->publication_surface);
    CHECK(!run16_native_frontend_enter(test_frontend,channel));
    CHECK(test_frontend->video==&channel->console.video && channel->console.video.published_serial==1);
    CHECK(ReadConsoleOutputCharacterW(test_frontend->logical_surface,&cell,1,(COORD){0,0},&count) && cell==L'P');
    CHECK(channel->console.video.pixels[sizeof(*style)]=='P');
    run16_native_frontend_leave(test_frontend);
    CHECK(!PeekNamedPipe(peer,NULL,0,NULL,&count,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
    CloseHandle(peer);peer=NULL;
    CHECK(!run16_console_channel_stop(channel));HeapFree(GetProcessHeap(),0,payload);
    fprintf(private_report ? private_report : stdout,
        "PASS real pipe postcommit projection failure batch=%u: error reply, coherent grid/frame, terminal thread and EOF, no unsafe retry\n",(unsigned)batch);
}
static void test_failed_text_commit(BOOL styled)
{
    run16_console_channel channel={0};
    console_video_description description={0};
    console_text_configuration saved_configuration;
    CONSOLE_SCREEN_BUFFER_INFO before,after;
    console_text_style *style;
    BYTE *payload,*text,*previous;
    HANDLE old_surface;
    DWORD bytes,count,index,revision,step=styled ? 3 : 2;
    WCHAR cell;WORD attribute;
    channel.root=test_frontend;
    CHECK(!test_bind(test_frontend,&channel,TRUE,FALSE));
    CHECK(!run16_native_frontend_logical_console(test_frontend,&channel.console.output));
    CHECK(!enter(&channel));
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    description.width=80;description.height=25;description.stride=80*step;
    bytes=description.bytes=sizeof(*style)+description.stride*25;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);CHECK(payload);
    style=(console_text_style *)payload;style->font_height=16;
    style->cursor_visible=1;style->cursor_column=3;style->cursor_row=24;style->cursor_height=4;
    text=payload+sizeof(*style);
    for(index=0;index<80*25;++index){text[index*step]=' ';text[index*step+1]=7;}
    text[0]='K';text[1]=0x1e;if(styled)text[2]=CONSOLE_TEXT_UNDERLINE;
    CHECK(!run16_console_video_begin(&channel.console.video,1,&description));
    CHECK(!video_data(&channel,1,0,payload,bytes));
    previous=channel.console.video.pixels;old_surface=test_frontend->logical_surface;
    revision=test_frontend->text_revision;saved_configuration=test_frontend->text_configuration;
    CHECK(GetConsoleScreenBufferInfo(old_surface,&before));

    text[0]='Z';style->cursor_column=1;style->cursor_row=0;style->font_height=8;
    CHECK(!run16_console_video_begin(&channel.console.video,2,&description));
    fail_frame_allocation=TRUE;
    CHECK(video_data(&channel,2,0,payload,bytes)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!fail_frame_allocation && !channel.publication_terminal_error);
    CHECK(channel.console.video.pixels==previous && channel.console.video.published_serial==1);
    CHECK(!channel.console.video.pending && channel.console.video.serial==2);
    CHECK(test_frontend->logical_surface==old_surface && test_frontend->video==&channel.console.video);
    CHECK(test_frontend->text_revision==revision &&
        !memcmp(&test_frontend->text_configuration,&saved_configuration,sizeof(saved_configuration)));
    CHECK(GetConsoleScreenBufferInfo(old_surface,&after) &&
        !memcmp(&before.dwSize,&after.dwSize,sizeof(before.dwSize)) &&
        !memcmp(&before.dwCursorPosition,&after.dwCursorPosition,sizeof(before.dwCursorPosition)));
    CHECK(ReadConsoleOutputCharacterW(old_surface,&cell,1,(COORD){0,0},&count) && cell==L'K');
    CHECK(ReadConsoleOutputAttribute(old_surface,&attribute,1,(COORD){0,0},&count) &&
        attribute==(WORD)(0x1e|(styled ? COMMON_LVB_UNDERSCORE : 0)));
    CHECK(run16_console_video_begin(&channel.console.video,2,&description)==ERROR_INVALID_DATA);

    /* Revision exhaustion is another precommit failure, before any grid or
     * frame swap. The test alters only the real instance's boundary counter. */
    test_frontend->text_revision=UINT32_MAX;
    CHECK(!run16_console_video_begin(&channel.console.video,3,&description));
    CHECK(video_data(&channel,3,0,payload,bytes)==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(test_frontend->logical_surface==old_surface && channel.console.video.pixels==previous);
    CHECK(!channel.console.video.pending && channel.console.video.published_serial==1);
    CHECK(!channel.publication_terminal_error &&
        !memcmp(&test_frontend->text_configuration,&saved_configuration,sizeof(saved_configuration)));
    test_frontend->text_revision=revision;
    CHECK(!run16_console_video_begin(&channel.console.video,4,&description));
    CHECK(!video_data(&channel,4,0,payload,bytes));
    CHECK(channel.console.video.published_serial==4 && !channel.console.video.pending);
    CHECK(ReadConsoleOutputCharacterW(channel.console.output,&cell,1,(COORD){0,0},&count) && cell==L'Z');
    CHECK(GetConsoleScreenBufferInfo(channel.console.output,&after) && after.dwCursorPosition.X==1 && after.dwCursorPosition.Y==0);
    /* A staged batch must not consume its private grid before font metadata
     * is admitted. Failure leaves the old complete grid/frame available to
     * the renderer; abort returns its ownership to the channel. */
    old_surface=test_frontend->logical_surface;
    revision=test_frontend->text_revision;saved_configuration=test_frontend->text_configuration;
    previous=channel.console.video.pixels;
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN));
    CHECK(WriteConsoleOutputCharacterW(channel.console.output,L"Y",1,(COORD){0,0},&count));
    text[0]='Y';style->font_height=16;
    CHECK(!run16_console_video_begin(&channel.console.video,5,&description));
    CHECK(!video_data(&channel,5,0,payload,bytes));
    test_frontend->text_revision=UINT32_MAX;
    CHECK(publication(&channel,CONSOLE_IO_PUBLICATION_END)==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(channel.publication_surface && !channel.publication_terminal_error);
    CHECK(test_frontend->logical_surface==old_surface && test_frontend->video==&channel.committed_video);
    CHECK(channel.committed_video.pixels==previous && channel.committed_video.published_serial==4);
    CHECK(!memcmp(&test_frontend->text_configuration,&saved_configuration,sizeof(saved_configuration)));
    CHECK(ReadConsoleOutputCharacterW(old_surface,&cell,1,(COORD){0,0},&count) && cell==L'Z');
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_ABORT));
    CHECK(test_frontend->video==&channel.console.video && channel.console.video.pixels==previous);
    CHECK(channel.console.video.serial==5 && channel.console.video.published_serial==4);
    test_frontend->text_revision=revision;
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN));
    CHECK(WriteConsoleOutputCharacterW(channel.console.output,L"Y",1,(COORD){0,0},&count));
    CHECK(!run16_console_video_begin(&channel.console.video,6,&description));
    CHECK(!video_data(&channel,6,0,payload,bytes));
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_END));
    CHECK(!channel.publication_surface && !channel.committed_video.pixels && !channel.publication_terminal_error);
    CHECK(test_frontend->video==&channel.console.video && channel.console.video.published_serial==6);
    CHECK(ReadConsoleOutputCharacterW(channel.console.output,&cell,1,(COORD){0,0},&count) && cell==L'Y');
    /* A committed geometry conversion must detach the old frame even if
     * canonical projection fails; the transport must not silently continue. */
    fail_projection_target=test_frontend->console_output;
    CHECK(prepare_text(&channel,(COORD){80,28})==ERROR_WRITE_FAULT);
    CHECK(!fail_projection_target && channel.publication_terminal_error==ERROR_WRITE_FAULT);
    CHECK(!test_frontend->video && !test_frontend->video_serial);
    CHECK(GetConsoleScreenBufferInfo(test_frontend->logical_surface,&after));
    CHECK(after.dwSize.X==80 && after.dwSize.Y==28);
    CHECK(ReadConsoleOutputCharacterW(test_frontend->logical_surface,&cell,1,(COORD){0,0},&count) && cell==L'Y');
    fprintf(private_report ? private_report : stdout,
        "PASS actual batch revision failure styled=%u: grid/frame/font commit remains coherent, abort preserves high-water and subsequent batch succeeds\n",(unsigned)styled);
    leave(&channel);
    CHECK(!test_bind(test_frontend,&channel,FALSE,FALSE));
    run16_native_frontend_forget(test_frontend,&channel);
    run16_console_video_dispose(&channel.console.video);
    CloseHandle(channel.console.output);HeapFree(GetProcessHeap(),0,payload);
    fprintf(private_report ? private_report : stdout,
        "PASS actual standalone import failure styled=%u: allocation/revision failures preserve frame/grid/cursor/font, serial high-water and next success\n",(unsigned)styled);
}
#endif
static void test_logical_publication(BOOL native)
{
    run16_console_channel channel={0};
    HANDLE committed,staging,canonical,input;
    CONSOLE_SCREEN_BUFFER_INFO info,after;
    console_video_description description={0};
    console_text_style *style;
    BYTE *payload;
    DWORD count,bytes;WCHAR cell;
    SMALL_RECT window;
    channel.root=test_frontend;
    CHECK(!test_bind(test_frontend,&channel,TRUE,FALSE));
    CHECK(!run16_native_frontend_console(test_frontend,&input,&canonical));
    /* Each operation-mode run starts with the same physical canvas. The
     * previous run deliberately finishes with a smaller clipped canvas. */
    CHECK(SetConsoleCursorPosition(canonical,(COORD){0,0}));
    CHECK(SetConsoleScreenBufferSize(canonical,(COORD){120,60}));
    CHECK(SetConsoleWindowInfo(canonical,TRUE,&(SMALL_RECT){0,0,79,29}));
    CHECK(!run16_native_frontend_logical_console(test_frontend,&channel.console.output));
    channel.console.logical_window=run16_native_frontend_text_region(test_frontend);
    CHECK(!enter(&channel));
    CHECK(!run16_native_frontend_clone_text(test_frontend,&staging,&window));
    CHECK(SetConsoleWindowInfo(staging,TRUE,&(SMALL_RECT){0,0,79,24}));
    CHECK(SetConsoleScreenBufferSize(staging,(COORD){80,30}));
    CHECK(SetConsoleCursorPosition(staging,(COORD){7,29}));
    CHECK(WriteConsoleOutputCharacterW(staging,L"A",1,(COORD){0,2},&count) && count==1);
    CHECK(!run16_native_frontend_commit_text(test_frontend,staging,(SMALL_RECT){0,2,79,29}));
    CloseHandle(channel.console.output);
    CHECK(!run16_native_frontend_logical_console(test_frontend,&channel.console.output));
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN));
    CHECK(publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN)==ERROR_BUSY);
    CHECK(WriteConsoleOutputCharacterW(channel.console.output,L"B",1,(COORD){0,2},&count) && count==1);
    {
        publication_read_case read={0};HANDLE reader;
        /* Unlike a single locked fixture call, production releases the root
         * lock between incoming tiles. A different reader must see the prior
         * complete grid, not this publication's candidate cell/geometry. */
        leave(&channel);
        reader=CreateThread(NULL,0,publication_reader,&read,0,NULL);CHECK(reader);
        CHECK(WaitForSingleObject(reader,3000)==WAIT_OBJECT_0);
        CloseHandle(reader);
        CHECK(!read.error && read.cell==L'A' && read.info.dwSize.Y==30 &&
            read.info.dwCursorPosition.Y==29);
        CHECK(!enter(&channel));
    }
    CHECK(!run16_native_frontend_logical_console(test_frontend,&committed));
    CHECK(ReadConsoleOutputCharacterW(committed,&cell,1,(COORD){0,2},&count) && count==1 && cell==L'A');
    CHECK(GetConsoleScreenBufferInfo(committed,&info) && info.dwSize.Y==30 && info.dwCursorPosition.Y==29);
    CloseHandle(committed);
    CHECK(publication(&channel,CONSOLE_IO_PUBLICATION_END)==ERROR_INVALID_DATA);
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_ABORT));
    CHECK(ReadConsoleOutputCharacterW(channel.console.output,&cell,1,(COORD){0,2},&count) && cell==L'A');
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN));
    CHECK(WriteConsoleOutputCharacterW(channel.console.output,L"C",1,(COORD){0,2},&count) && count==1);
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=80;description.height=28;
    description.stride=160;description.bytes=sizeof(console_text_style)+160*28;
    bytes=description.bytes;payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);CHECK(payload);
    style=(console_text_style *)payload;style->font_height=16;style->cursor_column=7;
    style->cursor_row=27;style->cursor_height=4;style->cursor_visible=1;
    CHECK(!run16_console_video_begin(&channel.console.video,1,&description));
    CHECK(!run16_console_video_data(&channel.console.video,1,0,payload,bytes/2));
    CHECK(publication(&channel,CONSOLE_IO_PUBLICATION_END)==ERROR_INVALID_DATA);
    CHECK(!run16_console_video_data(&channel.console.video,1,bytes/2,payload+bytes/2,bytes-bytes/2));
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_END));
    CHECK(ReadConsoleOutputCharacterW(channel.console.output,&cell,1,(COORD){0,2},&count) && cell==L'C');
    CHECK(GetConsoleScreenBufferInfo(channel.console.output,&after) &&
        after.dwSize.Y==30 && after.dwCursorPosition.Y==29);
    CHECK(!run16_native_frontend_project_text(test_frontend));
    CHECK(GetConsoleScreenBufferInfo(canonical,&after) &&
        after.dwCursorPosition.X==after.srWindow.Left+7 &&
        after.dwCursorPosition.Y==after.srWindow.Top+27);
    /* Host-only canvas changes must neither clip storage nor pan the logical
     * viewport. The same upper-left mapping covers all four intersections. */
    for(unsigned index=0;index<4;++index) {
        SHORT width=index&1 ? 60 : 100,height=index&2 ? 20 : 30;
        SMALL_RECT canvas={0,0,width-1,height-1};
        CONSOLE_CURSOR_INFO cursor;
        CHECK(SetConsoleCursorPosition(canonical,(COORD){0,0}));
        CHECK(SetConsoleScreenBufferSize(canonical,(COORD){120,60}));
        CHECK(SetConsoleWindowInfo(canonical,TRUE,&canvas));
        CHECK(FillConsoleOutputCharacterW(canonical,L'Z',120*60,(COORD){0,0},&count));
        CHECK(FillConsoleOutputAttribute(canonical,0x4f,120*60,(COORD){0,0},&count));
        CHECK(!run16_native_frontend_project_text(test_frontend));
        CHECK(GetConsoleScreenBufferInfo(canonical,&after) && !memcmp(&after.srWindow,&canvas,sizeof(canvas)));
        CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,(COORD){0,0},&count) && cell==L'C');
        CHECK(GetConsoleCursorInfo(canonical,&cursor) && cursor.bVisible==(height>=28));
        if(width>80) {
            WORD attr;
            CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,(COORD){80,0},&count) && cell==L' ');
            CHECK(ReadConsoleOutputAttribute(canonical,&attr,1,(COORD){80,0},&count) && attr==info.wAttributes);
        }
        if(height>28)CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,(COORD){0,28},&count) && cell==L' ');
        CHECK(GetConsoleScreenBufferInfo(channel.console.output,&after) && after.dwSize.Y==30 && after.dwCursorPosition.Y==29);
        CHECK(channel.console.logical_window->Top==2 && channel.console.logical_window->Bottom==29);
    }
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_BEGIN));
    CHECK(WriteConsoleOutputCharacterW(channel.console.output,L"D",1,(COORD){0,2},&count));
    CHECK(!run16_console_video_begin(&channel.console.video,2,&description));
    CHECK(!run16_console_video_data(&channel.console.video,2,0,payload,bytes/2));
    CHECK(!publication(&channel,CONSOLE_IO_PUBLICATION_ABORT));
    CHECK(channel.console.video.serial==2 && channel.console.video.published_serial==1);
    CHECK(run16_console_video_begin(&channel.console.video,2,&description)==ERROR_INVALID_DATA);
    CHECK(ReadConsoleOutputCharacterW(channel.console.output,&cell,1,(COORD){0,2},&count) && cell==L'C');
    leave(&channel);
    CHECK(!test_bind(test_frontend,&channel,FALSE,FALSE));
    CHECK(enter(&channel)==ERROR_NOT_READY);
    run16_native_frontend_forget(test_frontend,&channel);
    run16_console_video_dispose(&channel.console.video);HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(channel.console.output);CloseHandle(input);CloseHandle(canonical);
    fprintf(private_report ? private_report : stdout,
        "PASS logical publication complete/partial/abort; offset viewport cursor; four canvas intersections, attributes and stale owner\n");
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
        test_native_geometry_projection(25);
        test_native_geometry_projection(28);
        test_dos_geometry_handoff(geometry_output,25);
        test_dos_geometry_handoff(geometry_output,28);
        test_native_seed_origin(geometry_output);
        CloseHandle(geometry_output);
        fprintf(private_report,"PASS private 80x30 to 80x25/80x28 Console API handoff\n");
        fclose(private_report);return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--worker-wait")) {Sleep(INFINITE);return 0;}
    test_native_geometry_projection(25);
    test_native_geometry_projection(28);
    test_dos_conversion_thresholds();
    read_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(read_entered);
    CHECK(GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&original_cursor));
    CHECK(!run16_native_frontend_create(&test_frontend));
    test_prepare_operation();
    test_standalone_text(FALSE,FALSE);
    test_standalone_text(FALSE,TRUE);
    test_standalone_text(TRUE,FALSE);
    test_standalone_text(TRUE,TRUE);
#ifdef NTCON_FRAME_FAILURE_TEST
    test_failed_text_commit(FALSE);
    test_failed_text_commit(TRUE);
    test_projection_failure_pipe(FALSE);
    test_projection_failure_pipe(TRUE);
#endif
    test_logical_publication(FALSE);
    test_logical_publication(TRUE);
    /* The publication fixture deliberately leaves an offset viewport. Start
     * the older channel marker fixture after the real DOS conversion edge. */
    CHECK(!test_bind(test_frontend,&before,TRUE,TRUE));
    CHECK(!test_bind(test_frontend,&before,FALSE,TRUE));
    {
        HANDLE logical;WCHAR cell;DWORD copied;
        CHECK(!run16_native_frontend_logical_console(test_frontend,&logical));
        CHECK(ReadConsoleOutputCharacterW(logical,&cell,1,(COORD){0,0},&copied) &&
            copied==1 && cell==L'C');
        CHECK(GetConsoleScreenBufferInfo(logical,&current) &&
            current.dwSize.X==80 && current.dwSize.Y==28 && current.dwCursorPosition.Y==27);
        CloseHandle(logical);
    }
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
        CHECK(!test_bind(test_frontend,channel,TRUE,TRUE));
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
    {
        HANDLE logical;
        CHECK(!run16_native_frontend_logical_console(test_frontend,&logical));
        CHECK(WriteConsoleOutputCharacterW(logical,L"K",1,origin,&count) && count==1);
        CHECK(!run16_native_frontend_project_text(test_frontend));CloseHandle(logical);
    }
    alternate=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(alternate!=INVALID_HANDLE_VALUE);
    CHECK(WriteConsoleOutputCharacterW(alternate,L"A",1,origin,&count) && count==1);
    CHECK(SetConsoleActiveScreenBuffer(alternate));
    /* Warm up lazy runtime/Console resources before counting owned handles. */
    for(mode=0;mode<6;++mode) run_case(mode,0);
    run_case(6,0);
    run_case(6,1);
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
    /* Connection waiting, cancellation and peer-death arbitration belong to
     * NTSRV (production-linked --io-authority and frontend-wait cases).
     * The frontend has no waiting owners: conflicting binds fail immediately
     * and cannot alter the current channel or consume its queued input. */
    {
        unsigned pattern,iteration;
        for(pattern=0;pattern<4;++pattern) {
            INPUT_RECORD queued={0},received={0};DWORD read=0;
            unsigned previous=0,incoming=0,unrelated=0;
            CHECK(!test_bind(test_frontend,&previous,TRUE,(pattern&2)!=0));
            CHECK(!run16_native_frontend_enter(test_frontend,&previous));
            queued.EventType=KEY_EVENT;queued.Event.KeyEvent.wVirtualKeyCode='Q';
            CHECK(!run16_native_frontend_prepend(test_frontend,&queued,1));
            queued.Event.KeyEvent.wVirtualKeyCode='R';
            CHECK(!run16_native_frontend_prepend(test_frontend,&queued,1));
            run16_native_frontend_leave(test_frontend);
            for(iteration=0;iteration<32;++iteration) {
                CHECK(test_bind(test_frontend,&incoming,TRUE,(pattern&1)!=0)==ERROR_BUSY);
                CHECK(test_bind(test_frontend,&unrelated,TRUE,FALSE)==ERROR_BUSY);
                CHECK(test_bind(test_frontend,&incoming,FALSE,FALSE)==ERROR_BUSY);
                CHECK(run16_native_frontend_enter(test_frontend,&incoming)==ERROR_NOT_READY);
            }
            run16_native_frontend_forget(test_frontend,&unrelated);
            CHECK(!run16_native_frontend_enter(test_frontend,&previous));
            CHECK(!run16_native_frontend_read(test_frontend,FALSE,&received,1,&read) &&
                read==1 && received.Event.KeyEvent.wVirtualKeyCode=='R');
            CHECK(!run16_native_frontend_read(test_frontend,TRUE,&received,1,&read) &&
                read==1 && received.Event.KeyEvent.wVirtualKeyCode=='Q');
            run16_native_frontend_leave(test_frontend);
            CHECK(!test_bind(test_frontend,&previous,FALSE,FALSE));
            CHECK(!test_bind(test_frontend,&incoming,TRUE,(pattern&1)!=0));
            CHECK(!run16_native_frontend_enter(test_frontend,&incoming));
            CHECK(!run16_native_frontend_read(test_frontend,FALSE,&received,1,&read) &&
                read==1 && received.Event.KeyEvent.wVirtualKeyCode=='Q');
            run16_native_frontend_leave(test_frontend);
            CHECK(run16_native_frontend_enter(test_frontend,&previous)==ERROR_NOT_READY);
            CHECK(!test_bind(test_frontend,&incoming,FALSE,FALSE));
        }
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
        CHECK(after==before);
        fprintf(private_report ? private_report : stdout,
            "PASS sole-channel binding: immediate conflict rejection, ordered input, stale isolation and stable handles=%lu\n",after);
    }
    {
        DWORD caller_mode,worker_mode,observed;
        CHECK(GetConsoleMode(input,&caller_mode));
        worker_mode=caller_mode^ENABLE_PROCESSED_INPUT;
        CHECK(SetConsoleMode(input,worker_mode));
        CHECK(!run16_native_frontend_park(test_frontend));
        CHECK(GetConsoleMode(input,&observed) && observed==caller_mode);
        CHECK(!test_bind(test_frontend,&round,TRUE,TRUE));
        CHECK(GetConsoleMode(input,&observed) && observed==worker_mode);
        CHECK(!test_bind(test_frontend,&round,FALSE,TRUE));
        CHECK(!run16_native_frontend_park(test_frontend));
        CHECK(GetConsoleMode(input,&observed) && observed==caller_mode);
    }
    {
        ULONGLONG began=GetTickCount64();
        run16_native_frontend_cancel(test_frontend);
        CHECK(test_bind(test_frontend,&before,TRUE,TRUE)==ERROR_OPERATION_ABORTED);
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
    puts("PASS conflicting channel binds cannot preempt the sole owner or create waiting owners");
    puts("PASS borrowed frontend park twice: caller input mode restored, resident worker binding resumes its prior mode");
    if(private_report) {fprintf(private_report,"PASS private Console handoff fixture\n");fclose(private_report);}
    return 0;
}
