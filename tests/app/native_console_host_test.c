/* Actual sibling run16 private helper and native CMD, not a substitute host.
 * Run only through the unswitched private-desktop observation harness. */
#include "frontend-exe/native_console_backend.h"
#include "frontend-exe/native_console_view.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
static HANDLE helper,target,finished;
static HANDLE control_received;
static volatile LONG received_control;
static BOOL WINAPI target_control(DWORD event)
{
    if(event!=CTRL_C_EVENT && event!=CTRL_BREAK_EVENT)return FALSE;
    InterlockedExchange(&received_control,(LONG)event);
    SetEvent(control_received);return TRUE;
}
static run16_native_backend *backend;
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL line=%u error=%lu: %s\n",(unsigned)__LINE__,GetLastError(),#x); \
    if(target)TerminateProcess(target,1);if(helper)TerminateProcess(helper,1);ExitProcess(1); } } while(0)
static DWORD WINAPI watchdog(void *unused)
{
    (void)unused;
    if(WaitForSingleObject(finished,20000)==WAIT_TIMEOUT) {
        if(target)TerminateProcess(target,ERROR_TIMEOUT);
        if(helper)TerminateProcess(helper,ERROR_TIMEOUT);
        ExitProcess(ERROR_TIMEOUT);
    }
    return 0;
}
static run16_native_host_reply request(DWORD operation,void *payload,DWORD bytes,
    DWORD offset,DWORD count,void *response,DWORD capacity)
{
    run16_native_host_request command={RUN16_NATIVE_HOST_VERSION,operation,bytes,offset,count};
    run16_native_host_reply reply;
    CHECK(!run16_native_backend_call(backend,&command,payload,&reply,response,capacity));
    CHECK(reply.version==RUN16_NATIVE_HOST_VERSION && reply.bytes<=capacity);
    return reply;
}
static void start(void)
{
    WCHAR image[MAX_PATH],*slash;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));slash=wcsrchr(image,L'\\');CHECK(slash);
    CHECK(wcscpy_s(slash+1,MAX_PATH-(size_t)(slash+1-image),L"frontend.exe")==0);
    CHECK(!run16_native_backend_open(image,&backend));
    helper=run16_native_backend_process(backend);CHECK(helper);
}
static run16_native_host_reply launch_request(const run16_native_start *start)
{
    BYTE *payload=NULL;
    DWORD bytes;
    run16_native_host_reply reply;
    CHECK(!run16_native_launch_pack(start,&payload,&bytes));
    reply=request(RUN16_NATIVE_LAUNCH,payload,bytes,0,0,NULL,0);
    HeapFree(GetProcessHeap(),0,payload);
    return reply;
}
static void retain_target(run16_native_host_reply reply)
{
    CHECK(!reply.status && reply.process && reply.thread);
    CHECK(DuplicateHandle(helper,(HANDLE)(ULONG_PTR)reply.process,GetCurrentProcess(),&target,
        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,0));
}
static void launch(WCHAR *command)
{
    WCHAR directory[MAX_PATH];
    PWSTR environment=GetEnvironmentStringsW();
    run16_native_start start={0};
    CHECK(environment && GetCurrentDirectoryW(MAX_PATH,directory));
    start.command=command;start.directory=directory;start.environment=environment;start.console_mask=7;
    CHECK(!run16_native_backend_launch(backend,&start,&target));
    FreeEnvironmentStringsW(environment);
}
static void result(DWORD expected)
{
    DWORD code;
    CHECK(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(target,&code));
    if(code!=expected)fprintf(stderr,"native result=%lu expected=%lu\n",code,expected);
    CHECK(code==expected);
    CloseHandle(target);target=NULL;
}
static void text(const WCHAR *value)
{
    INPUT_RECORD records[128]={0};
    DWORD count=(DWORD)wcslen(value),i;
    run16_native_host_reply reply;
    CHECK(count<128);
    for(i=0;i<count;++i) {
        records[i].EventType=KEY_EVENT;records[i].Event.KeyEvent.bKeyDown=TRUE;
        records[i].Event.KeyEvent.wRepeatCount=1;records[i].Event.KeyEvent.uChar.UnicodeChar=value[i];
        if(value[i]==L'\r')records[i].Event.KeyEvent.wVirtualKeyCode=VK_RETURN;
    }
    reply=request(RUN16_NATIVE_INPUT,records,count*sizeof(*records),0,0,NULL,0);
    CHECK(!reply.status && reply.count==count);
}
static void frame_contains(const WCHAR *expected)
{
    run16_native_frame_info frame;
    run16_native_host_reply reply=request(RUN16_NATIVE_FRAME_BEGIN,NULL,0,0,0,&frame,sizeof(frame));
    CHAR_INFO cells[RUN16_NATIVE_HOST_CELLS];
    WCHAR *all;
    DWORD total,offset=0,i;
    CHECK(!reply.status && reply.bytes==sizeof(frame));
    total=(DWORD)frame.screen.dwSize.X*frame.screen.dwSize.Y;
    CHECK(total && total<4000000);
    all=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,((SIZE_T)total+1)*sizeof(WCHAR));CHECK(all);
    while(offset<total) {
        reply=request(RUN16_NATIVE_FRAME_READ,NULL,0,offset,RUN16_NATIVE_HOST_CELLS,cells,sizeof(cells));
        CHECK(!reply.status && reply.count && reply.count<=total-offset && reply.bytes==reply.count*sizeof(*cells));
        for(i=0;i<reply.count;++i) all[offset+i]=cells[i].Char.UnicodeChar ? cells[i].Char.UnicodeChar : L' ';
        offset+=reply.count;
    }
    CHECK(wcsstr(all,expected)!=NULL);HeapFree(GetProcessHeap(),0,all);
    reply=request(RUN16_NATIVE_FRAME_END,NULL,0,0,0,NULL,0);CHECK(!reply.status);
}
static void seed_geometry(void)
{
    run16_native_capture capture;
    run16_native_screen_seed seed={0};
    run16_native_frame_info frame;
    run16_native_host_reply reply;
    CONSOLE_FONT_INFOEX before={sizeof(before)},after={sizeof(after)};
    CHECK(!run16_native_capture_begin(&capture));
    CHECK(GetCurrentConsoleFontEx(capture.buffer,FALSE,&before));
    seed.frame.screen=capture.info;seed.frame.cursor=capture.cursor;
    seed.frame.output_mode=capture.output_mode;
    seed.frame.input_codepage=capture.input_codepage;seed.frame.output_codepage=capture.output_codepage;
    seed.font=before;seed.font.dwFontSize.X=4;seed.font.dwFontSize.Y=8;
    seed.frame.screen.dwSize.X=80;seed.frame.screen.dwSize.Y=300;
    seed.frame.screen.srWindow.Left=0;seed.frame.screen.srWindow.Top=36;
    seed.frame.screen.srWindow.Right=79;seed.frame.screen.srWindow.Bottom=40;
    seed.frame.screen.dwCursorPosition.X=0;seed.frame.screen.dwCursorPosition.Y=40;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);
    CHECK(!reply.status);
    reply=request(RUN16_NATIVE_FRAME_BEGIN,NULL,0,0,0,&frame,sizeof(frame));
    CHECK(!reply.status && reply.bytes==sizeof(frame));
    CHECK(frame.screen.dwSize.X==80 && frame.screen.dwSize.Y==300 &&
        !memcmp(&frame.screen.srWindow,&seed.frame.screen.srWindow,sizeof(SMALL_RECT)) &&
        frame.screen.dwCursorPosition.X==0 && frame.screen.dwCursorPosition.Y==40);
    reply=request(RUN16_NATIVE_FRAME_END,NULL,0,0,0,NULL,0);CHECK(!reply.status);
    seed.font.cbSize=0;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);
    CHECK(reply.status==ERROR_INVALID_DATA);
    CHECK(GetCurrentConsoleFontEx(capture.buffer,FALSE,&after) && !memcmp(&before,&after,sizeof(before)));
    seed.font=before;seed.font.dwFontSize.X=0;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);
    CHECK(!reply.status);
    seed.font.dwFontSize.X=-1;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);
    CHECK(reply.status==ERROR_INVALID_DATA);
    seed.font.dwFontSize.X=0;seed.font.dwFontSize.Y=0;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);
    CHECK(reply.status==ERROR_INVALID_DATA);
    puts("PASS Windows-resolved zero font width; negative width and zero height rejected");
    run16_native_capture_end(&capture);
    puts("PASS hidden font/80x300 scrolled viewport seed, malformed font rejected, visible font unchanged");
}
static void resize_roundtrip(void)
{
    run16_native_console_view view={0};
    run16_native_capture saved;
    run16_native_screen_seed seed={0};
    run16_native_frame_info frame;
    run16_native_host_reply reply;
    CONSOLE_SCREEN_BUFFER_INFO info;
    CHAR_INFO cell={{L'H'},0x1e};
    COORD size={90,300},point={5,0};
    SMALL_RECT tiny={0,0,19,4},window={0,10,29,17};
    INPUT_RECORD events[256],expected[256];
    WCHAR image[MAX_PATH],command[1024],value;
    DWORD count,i,expected_count=0;
    CHECK(!run16_native_capture_begin(&saved));
    {
        COORD initial={80,300},origin={0,0};
        /* Use a known empty native surface, not inherited 9001-row history:
         * Windows may legitimately reflow/drop old rows while shrinking it. */
        CHECK(SetConsoleWindowInfo(saved.buffer,TRUE,&tiny) &&
            SetConsoleCursorPosition(saved.buffer,origin) && SetConsoleScreenBufferSize(saved.buffer,initial));
        CHECK(FillConsoleOutputCharacterW(saved.buffer,L' ',80*300,origin,&count) && count==80*300);
    }
    CHECK(!run16_native_view_begin(backend,&view));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(command,1024,L"\"%ls\" --window-input",image);
    launch(command);result(31);
    reply=request(RUN16_NATIVE_CELLS_WRITE,&cell,sizeof(cell),5,1,NULL,0);CHECK(!reply.status);
    CHECK(FlushConsoleInputBuffer(view.input));
    CHECK(SetConsoleScreenBufferSize(view.output,size) &&
        SetConsoleWindowInfo(view.output,TRUE,&window));
    CHECK(!run16_native_view_present(backend,&view));
    CHECK(PeekConsoleInputW(view.input,expected,256,&expected_count) && expected_count);
    for(i=0;i<expected_count;++i)CHECK(expected[i].EventType==WINDOW_BUFFER_SIZE_EVENT);
    CHECK(!run16_native_view_forward_input(backend,&view));
    CHECK(GetConsoleScreenBufferInfo(view.output,&info) && info.dwSize.X==90 && info.dwSize.Y==300 &&
        !memcmp(&info.srWindow,&window,sizeof(window)));
    CHECK(ReadConsoleOutputCharacterW(view.output,&value,1,point,&count) && count==1 && value==L'H');
    reply=request(RUN16_NATIVE_INPUT_READ,NULL,0,0,256,events,sizeof(events));
    if(reply.count!=expected_count) {
        printf("resize reply status=%lu records=%lu native-expected=%lu\n",reply.status,reply.count,expected_count);
        for(i=0;i<reply.count;++i)printf("resize record type=%u size=%d,%d\n",events[i].EventType,
            events[i].Event.WindowBufferSizeEvent.dwSize.X,events[i].Event.WindowBufferSizeEvent.dwSize.Y);
    }
    /* Windows may report both buffer and viewport changes. Require exactly
     * its own direct-Console sequence, not an invented one-event assumption. */
    CHECK(!reply.status && reply.count==expected_count);
    for(i=0;i<reply.count;++i)CHECK(events[i].EventType==expected[i].EventType &&
        events[i].Event.WindowBufferSizeEvent.dwSize.X==expected[i].Event.WindowBufferSizeEvent.dwSize.X &&
        events[i].Event.WindowBufferSizeEvent.dwSize.Y==expected[i].Event.WindowBufferSizeEvent.dwSize.Y);
    for(i=0;i<3;++i) {
        CHECK(!run16_native_view_present(backend,&view));
        CHECK(!run16_native_view_forward_input(backend,&view));
    }
    reply=request(RUN16_NATIVE_INPUT_READ,NULL,0,0,256,events,sizeof(events));CHECK(!reply.status && !reply.count);
    /* A native app's own geometry change travels in the opposite direction. */
    reply=request(RUN16_NATIVE_FRAME_BEGIN,NULL,0,0,0,&frame,sizeof(frame));CHECK(!reply.status);
    reply=request(RUN16_NATIVE_FRAME_END,NULL,0,0,0,NULL,0);CHECK(!reply.status);
    seed.frame=frame;seed.font.cbSize=sizeof(seed.font);
    CHECK(GetCurrentConsoleFontEx(view.output,FALSE,&seed.font));
    seed.frame.screen.dwSize.X=100;seed.frame.screen.dwSize.Y=350;
    seed.frame.screen.srWindow.Left=0;seed.frame.screen.srWindow.Top=0;
    seed.frame.screen.srWindow.Right=39;seed.frame.screen.srWindow.Bottom=9;
    reply=request(RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,NULL,0);CHECK(!reply.status);
    CHECK(!run16_native_view_present(backend,&view));
    CHECK(GetConsoleScreenBufferInfo(view.output,&info) && info.dwSize.X==100 && info.dwSize.Y==350 &&
        !memcmp(&info.srWindow,&seed.frame.screen.srWindow,sizeof(SMALL_RECT)));
    CHECK(!run16_native_screen_apply(view.output,&saved.info,&saved.cursor));
    run16_native_capture_end(&saved);run16_native_view_end(&view);
    puts("PASS bidirectional native resize/scroll without snap-back, stale-cell reseed or duplicate resize notifications");
}
static void close_host(BOOL stop)
{
    DWORD code,error;
    HANDLE retained;
    CHECK(DuplicateHandle(GetCurrentProcess(),helper,GetCurrentProcess(),&retained,
        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0));
    error=run16_native_backend_close(backend);backend=NULL;helper=NULL;
    CHECK(stop ? error==0 : (error==ERROR_BROKEN_PIPE || error==ERROR_NO_DATA));
    CHECK(WaitForSingleObject(retained,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(retained,&code) && code==(stop ? 0u : 99u));
    CloseHandle(retained);
}
static void streams(void)
{
    HANDLE input_read,input_write,output_read,output_write,events[2],restricted[2];
    WCHAR directory[MAX_PATH],image[MAX_PATH],environment[1024],*end;
    DWORD i,count;
    run16_native_start start={0};
    char output[32]={0};
    CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(wcscat_s(directory,MAX_PATH,L"\\tests")==0);
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    end=environment;
    end+=swprintf_s(end,1024-(size_t)(end-environment),L"NTVDM_TEST_DIRECTORY=%ls",directory)+1;
    end+=swprintf_s(end,1024-(size_t)(end-environment),L"NTVDM_TEST_CUSTOM=caf\x00e9")+1;
    end+=swprintf_s(end,1024-(size_t)(end-environment),L"NTVDM_FRONTEND_CAPABILITY=deadbeef")+1;
    end+=swprintf_s(end,1024-(size_t)(end-environment),L"NTVDM_EXECUTION_CONSOLE=deadbeef")+1;*end=0;
    CHECK(CreatePipe(&input_read,&input_write,NULL,4096));
    CHECK(CreatePipe(&output_read,&output_write,NULL,4096));
    CHECK(WriteFile(input_write,"PIPE-IN",7,&count,NULL) && count==7);CloseHandle(input_write);
    start.application=image;start.command=L"not-the-application --streams";
    start.directory=directory;start.environment=environment;
    start.standard[0]=input_read;start.standard[1]=output_write;start.standard[2]=output_write;
    for(i=0;i<2;++i) {
        events[i]=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(events[i]);
        CHECK(DuplicateHandle(GetCurrentProcess(),events[i],GetCurrentProcess(),&restricted[i],SYNCHRONIZE,FALSE,0));
        start.capabilities[i]=restricted[i];
    }
    CHECK(!run16_native_backend_launch(backend,&start,&target));
    for(i=0;i<2;++i){CloseHandle(restricted[i]);CloseHandle(events[i]);}
    CloseHandle(input_read);CloseHandle(output_write);
    result(29);
    CHECK(ReadFile(output_read,output,sizeof(output),&count,NULL) && count==6 && !memcmp(output,"OUTERR",6));
    CHECK(!ReadFile(output_read,output,sizeof(output),&count,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
    CloseHandle(output_read);
    puts("PASS explicit application/cwd/Unicode environment; pipe stdin EOF, stdout/stderr alias, restricted capability rebinding, no leaked writer");
}
static void reclaim_input(void)
{
    run16_native_console_view view={0};
    INPUT_RECORD expected[605]={0},actual[605]={0};
    run16_native_host_reply reply;
    DWORD count,i,mode,returned;
    view.input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    CHECK(view.input!=INVALID_HANDLE_VALUE && GetConsoleMode(view.input,&mode));
    CHECK(SetConsoleMode(view.input,ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT));
    CHECK(!run16_native_view_reclaim_input(backend,&view,&returned));
    CHECK(FlushConsoleInputBuffer(view.input));
    reply=request(RUN16_NATIVE_INPUT_READ,NULL,0,0,0,NULL,0);
    CHECK(reply.status==ERROR_INVALID_PARAMETER && !reply.bytes && !reply.count);
    reply=request(RUN16_NATIVE_INPUT_READ,NULL,0,0,RUN16_NATIVE_HOST_INPUTS+1,NULL,0);
    CHECK(reply.status==ERROR_INVALID_PARAMETER && !reply.bytes && !reply.count);
    for(i=0;i<600;++i) {
        expected[i].EventType=KEY_EVENT;
        expected[i].Event.KeyEvent.bKeyDown=(i&1)==0;
        expected[i].Event.KeyEvent.wRepeatCount=1;
        expected[i].Event.KeyEvent.wVirtualKeyCode='A'+(WORD)(i/2%26);
        expected[i].Event.KeyEvent.wVirtualScanCode=30;
        expected[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x4e00+i/2);
        expected[i].Event.KeyEvent.dwControlKeyState=SHIFT_PRESSED;
    }
    expected[600].EventType=MOUSE_EVENT;
    expected[600].Event.MouseEvent.dwMousePosition.X=7;
    expected[600].Event.MouseEvent.dwMousePosition.Y=9;
    expected[600].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
    expected[600].Event.MouseEvent.dwEventFlags=MOUSE_MOVED;
    expected[601].EventType=WINDOW_BUFFER_SIZE_EVENT;
    expected[601].Event.WindowBufferSizeEvent.dwSize.X=80;
    expected[601].Event.WindowBufferSizeEvent.dwSize.Y=50;
    expected[602].EventType=FOCUS_EVENT;expected[602].Event.FocusEvent.bSetFocus=TRUE;
    expected[603].EventType=MENU_EVENT;expected[603].Event.MenuEvent.dwCommandId=17;
    expected[604]=expected[0];expected[604].Event.KeyEvent.uChar.UnicodeChar='Z';
    reply=request(RUN16_NATIVE_INPUT,expected,604*sizeof(*expected),0,0,NULL,0);
    CHECK(!reply.status && reply.count==604);
    CHECK(WriteConsoleInputW(view.input,expected+604,1,&count) && count==1);
    CHECK(!run16_native_view_reclaim_input(backend,&view,&returned) && returned==604);
    CHECK(PeekConsoleInputW(view.input,actual,ARRAYSIZE(actual),&count) && count==605);
    if(memcmp(expected,actual,sizeof(actual))) {
        for(i=0;i<ARRAYSIZE(actual);++i)if(memcmp(expected+i,actual+i,sizeof(*actual))) {
            DWORD byte;
            printf("reclaim mismatch record=%lu expected-type=%u actual-type=%u bytes:",i,expected[i].EventType,actual[i].EventType);
            for(byte=0;byte<sizeof(*actual);++byte)printf(" %02x/%02x",((BYTE *)(expected+i))[byte],((BYTE *)(actual+i))[byte]);
            puts("");break;
        }
        CHECK(FALSE);
    }
    CHECK(ReadConsoleInputW(view.input,actual,count,&count) && count==605);
    CHECK(!run16_native_view_reclaim_input(backend,&view,&returned) && returned==0);
    CHECK(PeekConsoleInputW(view.input,actual,1,&count) && count==0);
    CHECK(SetConsoleMode(view.input,mode));CloseHandle(view.input);
    puts("PASS hidden-to-visible input reclaim: 604 records across tiles, Unicode/key-up/mouse/resize/focus/menu, before newer input, empty nonblocking repeat");
}
static void control_input(void)
{
    WCHAR image[MAX_PATH],command[1024],name[96];
    DWORD processed;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    start();
    for(processed=0;processed<4;++processed) {
        HANDLE ready;
        INPUT_RECORD records[2]={0};
        run16_native_host_reply reply;
        swprintf_s(name,96,L"Local\\NativeInputControl-%lu-%lu",GetCurrentProcessId(),processed);
        ready=CreateEventW(NULL,TRUE,FALSE,name);CHECK(ready && GetLastError()!=ERROR_ALREADY_EXISTS);
        swprintf_s(command,1024,L"\"%ls\" --control-child %ls %lu",image,name,processed);
        launch(command);CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
        records[0].EventType=KEY_EVENT;records[0].Event.KeyEvent.bKeyDown=TRUE;
        records[0].Event.KeyEvent.wRepeatCount=1;records[0].Event.KeyEvent.wVirtualKeyCode='C';
        records[0].Event.KeyEvent.wVirtualScanCode=0x2e;
        records[0].Event.KeyEvent.uChar.UnicodeChar=3;
        records[0].Event.KeyEvent.dwControlKeyState=LEFT_CTRL_PRESSED;
        if(processed==2)records[0].Event.KeyEvent.dwControlKeyState|=RIGHT_ALT_PRESSED;
        if(processed==3)records[0].Event.KeyEvent.wVirtualKeyCode=VK_CANCEL;
        records[1]=records[0];records[1].Event.KeyEvent.bKeyDown=FALSE;
        reply=request(RUN16_NATIVE_INPUT,records,sizeof(records),0,0,NULL,0);
        CHECK(!reply.status && reply.count==2);
        result(processed==3 ? 44 : processed==1 ? 42 : 43);CloseHandle(ready);
    }
    close_host(TRUE);
    puts("PASS hidden raw Ctrl+C, processed Ctrl+C, Ctrl+Alt+C and raw-mode Ctrl+Break; helper retained");
}
static DWORD WINAPI blocked_call(void *unused)
{
    run16_native_host_request command={RUN16_NATIVE_HOST_VERSION,RUN16_NATIVE_FRAME_END,0,0,0};
    run16_native_host_reply reply;
    (void)unused;
    return run16_native_backend_call(backend,&command,NULL,&reply,NULL,0);
}
static void cancellation(void)
{
    WCHAR image[MAX_PATH],name[96];
    HANDLE gate,thread,retained;
    DWORD code;
    swprintf_s(name,96,L"Local\\NTVDMBackendCancel-%lu",GetCurrentProcessId());
    gate=CreateEventW(NULL,TRUE,FALSE,name);CHECK(gate);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_CANCEL_GATE",name));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(!run16_native_backend_open(image,&backend)); /* Deliberately stalled test peer. */
    helper=run16_native_backend_process(backend);
    thread=CreateThread(NULL,0,blocked_call,NULL,0,NULL);CHECK(thread);
    CHECK(WaitForSingleObject(gate,5000)==WAIT_OBJECT_0 && WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
    run16_native_backend_cancel(backend);
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0 && GetExitCodeThread(thread,&code) && code==ERROR_OPERATION_ABORTED);
    CloseHandle(thread);
    CHECK(DuplicateHandle(GetCurrentProcess(),helper,GetCurrentProcess(),&retained,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0));
    CHECK(!run16_native_backend_close(backend));backend=NULL;helper=NULL;
    CHECK(WaitForSingleObject(retained,5000)==WAIT_OBJECT_0 && GetExitCodeProcess(retained,&code) && !code);
    CloseHandle(retained);CloseHandle(gate);CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_CANCEL_GATE",NULL));
    puts("PASS production backend cancels a partial pending reply and joins cleanup; intentionally stalled test peer, not native acceptance");
}
static void close_timeout(void)
{
    WCHAR image[MAX_PATH],name[96];
    HANDLE gate,retained;
    DWORD error,code;
    ULONGLONG began,elapsed;
    swprintf_s(name,96,L"Local\\NTVDMBackendClose-%lu",GetCurrentProcessId());
    gate=CreateEventW(NULL,TRUE,FALSE,name);CHECK(gate);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_CANCEL_GATE",name));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(!run16_native_backend_open(image,&backend));
    helper=run16_native_backend_process(backend);
    CHECK(DuplicateHandle(GetCurrentProcess(),helper,GetCurrentProcess(),&retained,
        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0));
    began=GetTickCount64();
    error=run16_native_backend_close(backend);backend=NULL;helper=NULL;
    elapsed=GetTickCount64()-began;
    printf("close partial-reply error=%lu elapsed=%llu ms\n",error,elapsed);
    CHECK(error==ERROR_TIMEOUT && elapsed>=4500 && elapsed<10000);
    CHECK(WaitForSingleObject(gate,0)==WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(retained,5000)==WAIT_OBJECT_0 &&
        GetExitCodeProcess(retained,&code) && !code);
    CloseHandle(retained);CloseHandle(gate);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_CANCEL_GATE",NULL));
    puts("PASS root backend close bounds an unresponsive STOP reply; peer observes EOF and exits without forced termination");
}
static void completed_target_with_lost_view(void)
{
    WCHAR comspec[MAX_PATH],command[1024],image[MAX_PATH];
    run16_native_console_view view={0};
    DWORD code=0xdeadbeef,error,index;
    const DWORD expected[]={37,0,STILL_ACTIVE};
    CHECK(GetEnvironmentVariableW(L"COMSPEC",comspec,MAX_PATH));
    for(index=0;index<ARRAYSIZE(expected);++index) {
        start();
        CHECK(!run16_native_view_begin(backend,&view));
        swprintf_s(command,1024,L"\"%ls\" /d /c exit %lu",comspec,expected[index]);
        launch(command);
        CHECK(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
        /* A lost presentation channel is not a replacement task result. */
        run16_native_backend_cancel(backend);
        error=run16_native_view_wait(backend,&view,target,&code);
        printf("completed target, canceled final frame: error=%lu result=%lu\n",error,code);
        CHECK(!error && code==expected[index] && view.presentation_error==ERROR_OPERATION_ABORTED);
        result(expected[index]);
        run16_native_view_end(&view);
        run16_native_backend_close(backend);backend=NULL;helper=NULL;
    }
    puts("PASS completed native result survives final-frame failure");
    start();CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(!run16_native_view_begin(backend,&view));
    swprintf_s(command,1024,L"\"%ls\" --survivor",image);
    launch(command);CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);
    run16_native_backend_cancel(backend);code=0xdeadbeef;
    error=run16_native_view_wait(backend,&view,target,&code);
    CHECK(error==ERROR_OPERATION_ABORTED && code==0xdeadbeef &&
        view.presentation_error==ERROR_OPERATION_ABORTED);
    CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);
    result(41);
    run16_native_view_end(&view);
    run16_native_backend_close(backend);backend=NULL;helper=NULL;
    puts("PASS I/O failure is not completion and does not terminate a live native target");
}
int main(int argc,char **argv)
{
    WCHAR comspec[MAX_PATH],command[1024],image[MAX_PATH];
    HANDLE guard;
    if(argc==2 && !strcmp(argv[1],"--internal-native-console")) {
        WCHAR name[96];HANDLE gate;
        run16_native_host_request received;
        run16_native_host_reply reply={RUN16_NATIVE_HOST_VERSION,0,0,0,0,0,{0}};
        DWORD count;BYTE byte;
        if(!GetEnvironmentVariableW(L"NTVDM_TEST_CANCEL_GATE",name,96))return 1;
        gate=OpenEventW(EVENT_MODIFY_STATE,FALSE,name);if(!gate)return 2;
        if(!ReadFile(GetStdHandle(STD_INPUT_HANDLE),&received,sizeof(received),&count,NULL) || count!=sizeof(received))return 3;
        if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),&reply,sizeof(reply)-1,&count,NULL) || count!=sizeof(reply)-1)return 4;
        SetEvent(gate);CloseHandle(gate);
        if(ReadFile(GetStdHandle(STD_INPUT_HANDLE),&byte,1,&count,NULL) || GetLastError()!=ERROR_BROKEN_PIPE)return 5;
        return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--streams")) {
        WCHAR expected[MAX_PATH],actual[MAX_PATH],value[64];
        char input[32];DWORD count,i;
        const WCHAR *names[2]={L"NTVDM_FRONTEND_CAPABILITY",L"NTVDM_EXECUTION_CONSOLE"};
        if(!GetEnvironmentVariableW(L"NTVDM_TEST_DIRECTORY",expected,MAX_PATH) ||
            !GetCurrentDirectoryW(MAX_PATH,actual) || _wcsicmp(actual,expected)) return 2;
        if(!GetEnvironmentVariableW(L"NTVDM_TEST_CUSTOM",value,64) || wcscmp(value,L"caf\x00e9")) return 3;
        for(i=0;i<2;++i) {
            HANDLE capability;
            if(!GetEnvironmentVariableW(names[i],value,64) || !wcscmp(value,L"deadbeef")) return 4;
            capability=(HANDLE)(ULONG_PTR)wcstoul(value,NULL,16);
            if(WaitForSingleObject(capability,0)!=WAIT_TIMEOUT || SetEvent(capability) || GetLastError()!=ERROR_ACCESS_DENIED) return 5;
        }
        if(!ReadFile(GetStdHandle(STD_INPUT_HANDLE),input,sizeof(input),&count,NULL) || count!=7 || memcmp(input,"PIPE-IN",7)) return 6;
        if(ReadFile(GetStdHandle(STD_INPUT_HANDLE),input,sizeof(input),&count,NULL) || GetLastError()!=ERROR_BROKEN_PIPE) return 7;
        if(GetStdHandle(STD_OUTPUT_HANDLE)!=GetStdHandle(STD_ERROR_HANDLE)) return 8;
        if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),"OUT",3,&count,NULL) || count!=3 ||
            !WriteFile(GetStdHandle(STD_ERROR_HANDLE),"ERR",3,&count,NULL) || count!=3) return 9;
        return 29;
    }
    if(argc==2 && !strcmp(argv[1],"--survivor")) {
        DWORD mode,count;const WCHAR message[]=L"CHILD-SURVIVED-HELPER\r\n";
        Sleep(1200);
        if(!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),&mode) ||
            !WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),message,ARRAYSIZE(message)-1,&count,NULL)) return 1;
        return 41;
    }
    if(argc==2 && !strcmp(argv[1],"--window-input")) {
        HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
        return SetConsoleMode(input,ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT) && FlushConsoleInputBuffer(input) ? 31 : 7;
    }
    if(argc==4 && !strcmp(argv[1],"--control-child")) {
        HANDLE ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,argv[2]);
        HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD record;DWORD count,mode;
        DWORD kind=(DWORD)atoi(argv[3]);
        BOOL processed=kind==1 || kind==2;
        control_received=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!ready || !control_received || !GetConsoleMode(input,&mode) ||
            !SetConsoleMode(input,processed ? (mode|ENABLE_PROCESSED_INPUT) : (mode&~ENABLE_PROCESSED_INPUT)) ||
            !FlushConsoleInputBuffer(input) || !SetConsoleCtrlHandler(target_control,TRUE) || !SetEvent(ready))return 7;
        CloseHandle(ready);
        if(kind==1 || kind==3) {
            if(WaitForSingleObject(control_received,3000)!=WAIT_OBJECT_0)return 8;
            return received_control==(LONG)(kind==3 ? CTRL_BREAK_EVENT : CTRL_C_EVENT) ? (kind==3 ? 44 : 42) : 10;
        }
        if(!ReadConsoleInputW(input,&record,1,&count) || count!=1 || record.EventType!=KEY_EVENT ||
            record.Event.KeyEvent.uChar.UnicodeChar!=3 || !record.Event.KeyEvent.bKeyDown)return 9;
        return 43;
    }
    finished=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(finished);
    guard=CreateThread(NULL,0,watchdog,NULL,0,NULL);CHECK(guard);
    if(argc==2 && !strcmp(argv[1],"--control-input")) {
        control_input();
        SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
        CloseHandle(guard);CloseHandle(finished);return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--completion")) {
        completed_target_with_lost_view();
        SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
        CloseHandle(guard);CloseHandle(finished);
        return 0;
    }
    if(argc==2 && !strcmp(argv[1],"--close-timeout")) {
        close_timeout();
        SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
        CloseHandle(guard);CloseHandle(finished);
        return 0;
    }
    CHECK(GetEnvironmentVariableW(L"COMSPEC",comspec,MAX_PATH));
    {
        run16_native_backend *failed=NULL;
        DWORD before,after;
        STARTUPINFOW native_startup={sizeof(native_startup)};
        PROCESS_INFORMATION native_process={0};
        WCHAR missing[]=L"?:\\invalid-native-helper.exe";
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
        CHECK(!CreateProcessW(missing,missing,NULL,NULL,FALSE,0,NULL,NULL,&native_startup,&native_process));
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
        printf("direct-Win32-first-failure handles before=%lu after=%lu\n",before,after);
        before=after;
        CHECK(run16_native_backend_open(L"?:\\invalid-native-helper.exe",&failed)!=0 && !failed);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
        printf("startup-failure handles before=%lu after=%lu\n",before,after);
        CHECK(before==after);
        before=after;
        CHECK(run16_native_backend_open(L"?:\\invalid-native-helper.exe",&failed)!=0 && !failed);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
        printf("repeated-startup-failure handles before=%lu after=%lu\n",before,after);
        CHECK(before==after);
    }
    start();
    {
        run16_native_host_reply reply=request(RUN16_NATIVE_RELEASE,NULL,0,0,0,NULL,0);
        CHECK(reply.status==ERROR_INVALID_STATE);
        reply=request(RUN16_NATIVE_FRAME_READ,NULL,0,0,0,NULL,0);
        CHECK(reply.status==ERROR_INVALID_PARAMETER && !reply.bytes && !reply.count);
        {
            WCHAR invalid[]={L'X',0,L'Y',0};
            reply=request(RUN16_NATIVE_LAUNCH,invalid,sizeof(invalid),0,0,NULL,0);
            CHECK(reply.status==ERROR_INVALID_DATA && !reply.process && !reply.thread);
        }
        {
            WCHAR missing[]=L"?:\\__ntvdm_invalid_native_target__\\missing.exe";
            WCHAR directory[MAX_PATH];
            run16_native_start failed={0};
            CHECK(GetCurrentDirectoryW(MAX_PATH,directory));
            failed.command=missing;failed.directory=directory;failed.environment=L"";failed.console_mask=7;
            reply=launch_request(&failed);
            CHECK(reply.status && !reply.process && !reply.thread);
        }
    }
    seed_geometry();
    resize_roundtrip();
    reclaim_input();
    swprintf_s(command,1024,L"\"%ls\" /d /c \"echo NATIVE-HIDDEN-OK & exit /b 37\"",comspec);
    launch(command);result(37);frame_contains(L"NATIVE-HIDDEN-OK");
    swprintf_s(command,1024,L"\"%ls\" /d /c \"set /p native_line=ENTER: & echo INPUT-ACCEPTED & exit /b 23\"",comspec);
    launch(command);text(L"from-root\r");result(23);frame_contains(L"INPUT-ACCEPTED");
    puts("PASS actual run16 hidden helper: native CMD output, cooked input, exact target 37/23, reusable backend");
    streams();
    {
        WCHAR directory[MAX_PATH];
        run16_native_start unacknowledged={0};
        CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
        swprintf_s(command,1024,L"\"%ls\" --survivor",image);
        unacknowledged.command=command;unacknowledged.directory=directory;
        unacknowledged.environment=L"";unacknowledged.console_mask=7;
        retain_target(launch_request(&unacknowledged));
        close_host(TRUE);CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);result(41);
    }
    puts("PASS target survives helper stop before export acknowledgement; original native result 41");
    start();CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(command,1024,L"\"%ls\" --survivor",image);
    launch(command);CHECK(TerminateProcess(helper,99));close_host(FALSE);
    CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);result(41);
    puts("PASS exported native target and hidden Console survive helper death; actual result 41");
    cancellation();
    close_timeout();
    control_input();
    SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
    CloseHandle(guard);CloseHandle(finished);
    return 0;
}
