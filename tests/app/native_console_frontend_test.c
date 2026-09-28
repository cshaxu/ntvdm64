/* Production frontend/ConPTY/view composition on a private desktop.
 * Native completion and frontend cancellation are independently observed. */
#include "ntkvm-exe/native_console_frontend.h"
#include "product-abi/console_mouse.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static HANDLE target,finished;
static HANDLE signal_received;
static volatile LONG signal_type;
static WCHAR prefix[96];
#define CHECK(x) do { if(!(x)) { \
    fprintf(stderr,"FAIL line=%u error=%lu: %s\n",(unsigned)__LINE__,GetLastError(),#x); \
    if(target) { HANDLE p=OpenProcess(PROCESS_TERMINATE,FALSE,GetProcessId(target)); \
        if(p) { TerminateProcess(p,1);CloseHandle(p); } } \
    ExitProcess(1); } } while(0)

static HANDLE named_event(const WCHAR *suffix,BOOL create)
{
    WCHAR name[128];HANDLE event;
    CHECK(swprintf_s(name,128,L"%ls-%ls",prefix,suffix)>0);
    event=create ? CreateEventW(NULL,TRUE,FALSE,name) :
        OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);
    CHECK(event);return event;
}
static DWORD WINAPI watchdog(void *context)
{
    (void)context;
    CHECK(WaitForSingleObject(finished,45000)==WAIT_OBJECT_0);
    return 0;
}
static DWORD WINAPI check_native_mouse(void *context)
{
    INPUT_RECORD records[64];DWORD count,i,mouse_count;CONSOLE_SCREEN_BUFFER_INFO screen;
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),received=named_event(L"mouse",FALSE);
    ULONGLONG deadline=GetTickCount64()+10000;
    const DWORD buttons[]={FROM_LEFT_1ST_BUTTON_PRESSED,0,RIGHTMOST_BUTTON_PRESSED,0};
    (void)context;
    do {
        CHECK(PeekConsoleInputW(input,records,64,&count));
        mouse_count=0;
        for(i=0;i<count;++i)if(records[i].EventType==MOUSE_EVENT)++mouse_count;
        if(mouse_count>=4)break;
        Sleep(10);
    }while(GetTickCount64()<deadline);
    CHECK(mouse_count==4 && GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&screen));
    CHECK(ReadConsoleInputW(input,records,count,&count));
    mouse_count=0;
    for(i=0;i<count;++i)if(records[i].EventType==MOUSE_EVENT) {
        MOUSE_EVENT_RECORD *mouse=&records[i].Event.MouseEvent;
        CHECK(mouse->dwButtonState==buttons[mouse_count++] && !mouse->dwEventFlags);
        CHECK(mouse->dwMousePosition.X==screen.srWindow.Left+(screen.srWindow.Right-screen.srWindow.Left+1)/2);
        CHECK(mouse->dwMousePosition.Y==screen.srWindow.Top+(screen.srWindow.Bottom-screen.srWindow.Top+1)/2);
    }
    CHECK(SetEvent(received));CloseHandle(received);return 0;
}
static void exercise(DWORD expected,BOOL completed,BOOL cancel)
{
    run16_native_frontend *frontend=NULL;
    run16_native_start start={0};
    run16_console_video lazy_video={0};
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    HANDLE done,ready=NULL,mouse_ready=NULL;
    DWORD error,result=0xdeadbeef,actual;
    ULONGLONG began,elapsed;
    CHECK(swprintf_s(prefix,96,L"Local\\NTVDMFrontend-%lu-%lu-%u-%u",
        GetCurrentProcessId(),expected,completed,cancel)>0);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix));
    done=named_event(L"done",TRUE);
    if(expected==39)ready=named_event(L"ready",TRUE);
    if(expected==37)mouse_ready=named_event(L"mouse",TRUE);
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf_s(command,1024,L"\"%ls\" --target %lu",image,expected)>0);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=GetEnvironmentStringsW();start.console_mask=7;CHECK(start.environment);
    CHECK(!run16_native_frontend_create(&frontend));
    if(expected==39) {
        console_video_description description={4,2,4,8,8};BYTE pixels[8]={1};
        HWND window=NULL;DWORD window_pid;DWORD_PTR reply;
        ULONGLONG deadline=GetTickCount64()+5000;
        INPUT_RECORD records[16];DWORD count,index,keys=0,mouse=0;
        description.palette[1]=0xffffff;
        CHECK(!run16_console_video_begin(&lazy_video,1,&description));
        CHECK(!run16_console_video_data(&lazy_video,1,0,pixels,sizeof(pixels)));
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,TRUE));
        CHECK(!run16_native_frontend_display(frontend,TRUE));
        CHECK(!run16_native_frontend_dos_video(frontend,&frontend,&lazy_video));
        do {
            window=FindWindowW(NULL,L"NTVDM");
            if(window && GetWindowThreadProcessId(window,&window_pid) &&
                window_pid==GetCurrentProcessId() && IsWindowVisible(window))break;
            Sleep(10);
        }while(GetTickCount64()<deadline);
        CHECK(window && IsWindowVisible(window));
        /* Capture gesture is not a guest click; the second pair reaches the
         * actual frontend DOS queue, then must be excluded from native I/O. */
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONDOWN,MK_LBUTTON,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONUP,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONDOWN,MK_LBUTTON,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONUP,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'L',0x00260001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_KEYUP,'L',(LPARAM)0xc0260001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,TRUE));
        CHECK(!run16_native_frontend_dos_enter(frontend,&frontend));
        CHECK(!run16_native_frontend_dos_read(frontend,FALSE,records,ARRAYSIZE(records),&count));
        /* Console setup may also publish WINDOW_BUFFER_SIZE_EVENT. Preserve
         * that record; only the two keyboard records are the L contract. */
        for(index=0;index<count;++index)if(records[index].EventType==KEY_EVENT) {
            CHECK(keys<2 && records[index].Event.KeyEvent.wVirtualKeyCode=='L');
            CHECK(records[index].Event.KeyEvent.bKeyDown==!keys);
            ++keys;
        }
        CHECK(keys==2);
        for(index=0;index<count;++index)if(records[index].EventType==CONSOLE_INPUT_RELATIVE_MOUSE) {
            console_mouse_input event;
            memcpy(&event,&records[index].Event,sizeof(event));
            CHECK(console_mouse_input_valid(&event) && mouse<3);
            CHECK(event.action==(mouse ? CONSOLE_MOUSE_MOVE : CONSOLE_MOUSE_ENTER));
            CHECK(event.width==4 && event.height==2 && !event.dx && !event.dy);
            CHECK(event.buttons==(mouse==1 ? 1 : 0));++mouse;
        }
        CHECK(mouse==3);
        CHECK(!run16_native_frontend_dos_prepend(frontend,records,count));
        run16_native_frontend_dos_leave(frontend);
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,FALSE));
        CHECK(IsWindow(window)); /* No native backend has been launched yet. */
        CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'M',0x00320001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_KEYUP,'M',(LPARAM)0xc0320001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,FALSE));
        CHECK(IsWindow(window));
    }
    CHECK(!run16_native_frontend_launch(frontend,&start,&target));
    FreeEnvironmentStringsW((WCHAR *)start.environment);
    CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);
    if(ready) { CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);CloseHandle(ready); }
    if(expected==37 && completed && !cancel) {
        HWND window=NULL;DWORD pid;DWORD_PTR reply;ULONGLONG deadline=GetTickCount64()+5000;
        HANDLE input,canonical,visible;CONSOLE_CURSOR_INFO cursor;WCHAR cell;DWORD read;
        COORD origin={0,0};
        INPUT_RECORD hotkey[3]={0};unsigned key_index;
        CHECK(!run16_native_frontend_console(frontend,&input,&canonical));
        for(key_index=0;key_index<3;++key_index) {
            hotkey[key_index].EventType=KEY_EVENT;
            hotkey[key_index].Event.KeyEvent.bKeyDown=key_index!=2;
            hotkey[key_index].Event.KeyEvent.wRepeatCount=1;
            hotkey[key_index].Event.KeyEvent.wVirtualKeyCode='F';
            hotkey[key_index].Event.KeyEvent.wVirtualScanCode=0x21;
            hotkey[key_index].Event.KeyEvent.dwControlKeyState=LEFT_CTRL_PRESSED|LEFT_ALT_PRESSED;
        }
        CHECK(WriteConsoleInputW(input,hotkey,3,&read) && read==3);
        while(GetTickCount64()<deadline) {
            window=FindWindowW(NULL,L"NTVDM");
            if(window && GetWindowThreadProcessId(window,&pid) &&
                pid==GetCurrentProcessId() && IsWindowVisible(window))break;
            Sleep(10);
        }
        CHECK(window && IsWindowVisible(window));
        /* Window creation precedes its route callback; wait for the selected
         * blank surface rather than racing that owner-thread transition. */
        do {
            visible=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
            CHECK(visible!=INVALID_HANDLE_VALUE && GetConsoleCursorInfo(visible,&cursor));
            if(!cursor.bVisible)break;
            CloseHandle(visible);Sleep(10);
        } while(GetTickCount64()<deadline);
        CHECK(!cursor.bVisible);
        CHECK(WriteConsoleOutputCharacterW(canonical,L"~",1,origin,&read) && read==1);
        CHECK(ReadConsoleOutputCharacterW(visible,&cell,1,origin,&read) && read==1 && cell==L' ');
        CHECK(ReadConsoleOutputCharacterW(canonical,&cell,1,origin,&read) && read==1 && cell==L'~');
        CloseHandle(visible);
        /* Real library button messages -> copied frontend queue -> native
         * converter -> ConPTY -> separate native target's reader.
         * This does not simulate physical raw-input motion or desktop focus. */
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONDOWN,MK_LBUTTON,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONUP,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONDOWN,MK_LBUTTON,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_LBUTTONUP,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_RBUTTONDOWN,MK_RBUTTON,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_KILLFOCUS,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(WaitForSingleObject(mouse_ready,5000)==WAIT_OBJECT_0);
        puts("PASS real Window capture click excluded; native reader receives left/right press/release and focus release at viewport center");
        /* Only Window A may reach the hidden native Console. Physical Z on
         * the inactive visible Console must not leak during X return. */
        hotkey[0].Event.KeyEvent.wVirtualKeyCode='Z';
        hotkey[0].Event.KeyEvent.wVirtualScanCode=0x2c;
        hotkey[0].Event.KeyEvent.dwControlKeyState=0;
        CHECK(WriteConsoleInputW(input,hotkey,1,&read) && read==1);
        CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'A',0x001e0001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_KEYUP,'A',(LPARAM)0xc01e0001,SMTO_ABORTIFHUNG,3000,&reply));
        /* Deliberately omit B key-up: closing this source must release the
         * delivered key to the same target before Console input resumes. */
        CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'B',0x00300001,SMTO_ABORTIFHUNG,3000,&reply));
        CHECK(SendMessageTimeoutW(window,WM_CLOSE,0,0,SMTO_ABORTIFHUNG,3000,&reply));
        deadline=GetTickCount64()+5000;
        while(IsWindow(window) && GetTickCount64()<deadline)Sleep(10);
        CHECK(!IsWindow(window) && WaitForSingleObject(target,0)==WAIT_TIMEOUT);
        /* Original COMMAND returns unread typeahead before relinquishing
         * DOS I/O. It must reach the native target even when Window policy
         * means physical visible-Console input is suppressed. */
        CHECK(!run16_native_frontend_display(frontend,TRUE));
        deadline=GetTickCount64()+5000;
        do {
            window=FindWindowW(NULL,L"NTVDM");
            if(window && GetWindowThreadProcessId(window,&pid) &&
                pid==GetCurrentProcessId() && IsWindowVisible(window))break;
            Sleep(10);
        }while(GetTickCount64()<deadline);
        CHECK(window && IsWindowVisible(window));
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,TRUE));
        /* A program handoff is not physical keyboard/source retirement. */
        CHECK(IsWindow(window) && IsWindowVisible(window));
        CHECK(!run16_native_frontend_dos_enter(frontend,&frontend));
        hotkey[0].Event.KeyEvent.wVirtualKeyCode='C';
        hotkey[0].Event.KeyEvent.wVirtualScanCode=0x2e;
        hotkey[0].Event.KeyEvent.uChar.UnicodeChar=L'c';
        hotkey[0].Event.KeyEvent.dwControlKeyState=0;
        hotkey[1]=hotkey[0];hotkey[1].Event.KeyEvent.bKeyDown=FALSE;
        CHECK(!run16_native_frontend_dos_prepend(frontend,hotkey,2));
        run16_native_frontend_dos_leave(frontend);
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,FALSE));
        CHECK(IsWindow(window) && IsWindowVisible(window));
        /* A second no-op owner request is a presentation-thread barrier.
         * Final drain is not a barrier: it stops the pump. */
        CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,FALSE));
        CloseHandle(input);CloseHandle(canonical);
        puts("PASS Console CAF make/repeat/break opens production Window; X returns without ending native target");
    }
    if(completed)CHECK(SetEvent(done) && WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    if(cancel || !completed)run16_native_frontend_cancel(frontend);
    began=GetTickCount64();
    error=run16_native_frontend_wait(frontend,target,&result);
    elapsed=GetTickCount64()-began;
    printf("frontend wait expected=%lu completed=%u cancel=%u error=%lu result=%lu elapsed=%llu ms\n",
        expected,completed,cancel,error,result,elapsed);
    if(completed)CHECK(!error && result==expected && elapsed<3000);
    else {
        CHECK(error && result==0xdeadbeef && WaitForSingleObject(target,0)==WAIT_TIMEOUT);
        CHECK(SetEvent(done) && WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    }
    CHECK(GetExitCodeProcess(target,&actual) && actual==expected);
    began=GetTickCount64();
    run16_native_frontend_destroy(frontend);
    run16_console_video_dispose(&lazy_video);
    elapsed=GetTickCount64()-began;
    printf("frontend destroy completed=%u retained-stall=%u elapsed=%llu ms\n",completed,completed && cancel,elapsed);
    CHECK(elapsed<10000);
    CloseHandle(target);target=NULL;
    CloseHandle(done);
    if(mouse_ready)CloseHandle(mouse_ready);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",NULL));
}
static BOOL WINAPI signal_handler(DWORD event)
{
    if(event!=CTRL_C_EVENT && event!=CTRL_BREAK_EVENT)return FALSE;
    InterlockedExchange(&signal_type,(LONG)event);SetEvent(signal_received);return TRUE;
}
static int control_root(DWORD kind)
{
    run16_native_frontend *frontend=NULL;
    run16_native_start start={0};
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    HANDLE ready;
    DWORD result=0,event=(kind==1 || kind==3) ? CTRL_BREAK_EVENT : CTRL_C_EVENT;
    DWORD expected=kind==2 ? 0xc000013au : event==CTRL_C_EVENT ? 42 : 44;
    DWORD members[4];
    CHECK(swprintf_s(prefix,96,L"Local\\NativeRootControl-%lu",GetCurrentProcessId())>0);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix) &&
        SetEnvironmentVariableW(L"NTVDM_TEST_CONTROL",L"1"));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf_s(command,1024,L"\"%ls\" --signal-target %lu",image,kind)>0);
    ready=named_event(L"ready",TRUE);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=GetEnvironmentStringsW();start.console_mask=7;CHECK(start.environment);
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_launch(frontend,&start,&target));
    FreeEnvironmentStringsW((WCHAR *)start.environment);
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    /* Never broadcast into the observer/user Console. The parent creates this
     * root on its own hidden Console, and the target uses ConPTY. */
    CHECK(GetConsoleProcessList(members,4)==1 && members[0]==GetCurrentProcessId());
    if(kind==3)CHECK(!run16_native_frontend_dos_bind(frontend,&ready,TRUE));
    CHECK(GenerateConsoleCtrlEvent(event,0));
    CHECK(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_wait(frontend,target,&result));
    CHECK(result==expected);
    if(kind==3)run16_native_frontend_dos_forget(frontend,&ready);
    CloseHandle(target);target=NULL;CloseHandle(ready);
    run16_native_frontend_destroy(frontend);
    return 0;
}
static void controls(void)
{
    WCHAR image[MAX_PATH],command[1024];DWORD kind,code;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    for(kind=0;kind<4;++kind) {
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION process={0};
        startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        CHECK(swprintf_s(command,1024,L"\"%ls\" --control-root %lu",image,kind)>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process));
        CloseHandle(process.hThread);
        if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0) {
            TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);
        }
        CHECK(GetExitCodeProcess(process.hProcess,&code));CloseHandle(process.hProcess);
        printf("root control case=%lu exit=%lu\n",kind,code);CHECK(!code);
    }
    puts("PASS actual root Console Ctrl+C/Break forwards to hidden targets, default exit and paused-DOS I/O state");
}
static BOOL console_contains(HANDLE output,PCWSTR expected)
{
    WCHAR text[4097];DWORD count=0;COORD origin={0,0};
    CHECK(ReadConsoleOutputCharacterW(output,text,4096,origin,&count));
    text[count]=0;return wcsstr(text,expected)!=NULL;
}
static int concurrent_target(void)
{
    HANDLE ready,release;DWORD count;
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
    ready=named_event(L"ready",FALSE);release=named_event(L"release",FALSE);
    CHECK(WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),L"NATIVE-BASE\r\n",13,&count,NULL) && count==13);
    CHECK(SetEvent(ready));CHECK(WaitForSingleObject(release,10000)==WAIT_OBJECT_0);
    CHECK(WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),L"NATIVE-LATE\r\n",13,&count,NULL) && count==13);
    CloseHandle(ready);CloseHandle(release);return 37;
}
static int concurrent_frontend(void)
{
    run16_native_frontend *frontend=NULL;run16_native_start start={0};
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    HANDLE ready,release,input,output;DWORD count,result;ULONGLONG deadline;
    CHECK(swprintf_s(prefix,96,L"Local\\NTVDMConcurrent-%lu",GetCurrentProcessId())>0);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix));
    ready=named_event(L"ready",TRUE);release=named_event(L"release",TRUE);
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf_s(command,1024,L"\"%ls\" --concurrent-target",image)>0);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=GetEnvironmentStringsW();start.console_mask=7;CHECK(start.environment);
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_launch(frontend,&start,&target));
    CHECK(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_console(frontend,&input,&output));
    CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,TRUE));
    deadline=GetTickCount64()+5000;
    do {
        BOOL found;
        CHECK(!run16_native_frontend_dos_enter(frontend,&frontend));
        CHECK(!run16_native_frontend_screen_begin(frontend));
        found=console_contains(output,L"NATIVE-BASE");
        CHECK(!run16_native_frontend_screen_end(frontend,FALSE));
        run16_native_frontend_dos_leave(frontend);
        if(found)break;
        CHECK(GetTickCount64()<deadline);Sleep(1);
    }while(TRUE);
    CHECK(!run16_native_frontend_dos_enter(frontend,&frontend));
    CHECK(!run16_native_frontend_screen_begin(frontend));
    CHECK(WriteConsoleW(output,L"DOS-INTERVAL\r\n",14,&count,NULL) && count==14);
    CHECK(SetEvent(release)); /* Actual ConPTY child writes while screen is locked. */
    CHECK(!run16_native_frontend_screen_end(frontend,TRUE));
    run16_native_frontend_dos_leave(frontend);
    CHECK(WaitForSingleObject(target,10000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeProcess(target,&result) && result==37);
    deadline=GetTickCount64()+5000;
    do {
        BOOL found;
        CHECK(!run16_native_frontend_dos_enter(frontend,&frontend));
        CHECK(!run16_native_frontend_screen_begin(frontend));
        found=console_contains(output,L"NATIVE-LATE");
        CHECK(!run16_native_frontend_screen_end(frontend,FALSE));
        run16_native_frontend_dos_leave(frontend);
        if(found)break;
        CHECK(GetTickCount64()<deadline);Sleep(1);
    }while(TRUE);
    CHECK(console_contains(output,L"NATIVE-BASE") && console_contains(output,L"DOS-INTERVAL"));
    CHECK(!run16_native_frontend_dos_bind(frontend,&frontend,FALSE));
    CHECK(!run16_native_frontend_wait(frontend,target,&result) && result==37);
    CloseHandle(target);target=NULL;CloseHandle(input);CloseHandle(output);
    run16_native_frontend_destroy(frontend);FreeEnvironmentStringsW((LPWCH)start.environment);
    CloseHandle(ready);CloseHandle(release);
    puts("PASS real ConPTY child output survives frontend DOS screen transaction; native direct result=37");
    return 0;
}

int wmain(int argc,WCHAR **argv)
{
    if(argc==2 && !wcscmp(argv[1],L"--concurrent-target"))return concurrent_target();
    if(argc==2 && !wcscmp(argv[1],L"--concurrent"))return concurrent_frontend();
    HANDLE guard;
    if(argc==3 && !wcscmp(argv[1],L"--control-root"))return control_root(wcstoul(argv[2],NULL,10));
    if(argc==3 && !wcscmp(argv[1],L"--signal-target")) {
        HANDLE ready;DWORD kind=wcstoul(argv[2],NULL,10);
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
        ready=named_event(L"ready",FALSE);
        signal_received=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(signal_received);
        if(kind!=2)CHECK(SetConsoleCtrlHandler(signal_handler,TRUE));
        CHECK(SetEvent(ready));CloseHandle(ready);
        if(WaitForSingleObject(signal_received,3000)!=WAIT_OBJECT_0)return 8;
        return signal_type==CTRL_C_EVENT ? 42 : 44;
    }
    if(argc==2 && !wcscmp(argv[1],L"--controls")) { controls();return 0; }
    if(argc==3 && !wcscmp(argv[1],L"--target")) {
        HANDLE done;DWORD wait;
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
        if(wcstoul(argv[2],NULL,10)==37) {
            HANDLE thread=CreateThread(NULL,0,check_native_mouse,NULL,0,NULL);
            CHECK(thread);CloseHandle(thread);
        }
        if(wcstoul(argv[2],NULL,10)==39) {
            INPUT_RECORD records[16];DWORD count,index,keys;
            HANDLE ready=named_event(L"ready",FALSE);
            ULONGLONG deadline=GetTickCount64()+4000;
            do {
                CHECK(PeekConsoleInputW(GetStdHandle(STD_INPUT_HANDLE),records,16,&count));
                keys=0;
                for(index=0;index<count;++index)if(records[index].EventType==KEY_EVENT)++keys;
                if(keys>=4)break;
                Sleep(10);
            }while(GetTickCount64()<deadline);
            CHECK(keys==4 && SetEvent(ready));CloseHandle(ready);
        }
        done=named_event(L"done",FALSE);wait=WaitForSingleObject(done,30000);CloseHandle(done);
        if(wait==WAIT_OBJECT_0 && wcstoul(argv[2],NULL,10)==37) {
            INPUT_RECORD records[64];DWORD count,index,keys=0,held_keys=0,returned_keys=0;
            CHECK(PeekConsoleInputW(GetStdHandle(STD_INPUT_HANDLE),records,64,&count));
            for(index=0;index<count;++index)if(records[index].EventType==KEY_EVENT) {
                CHECK(records[index].Event.KeyEvent.wVirtualKeyCode!='F' &&
                    records[index].Event.KeyEvent.wVirtualKeyCode!='Z');
                if(records[index].Event.KeyEvent.wVirtualKeyCode=='A') {
                    CHECK(records[index].Event.KeyEvent.bKeyDown==(keys==0));++keys;
                }
                if(records[index].Event.KeyEvent.wVirtualKeyCode=='B') {
                    CHECK(records[index].Event.KeyEvent.bKeyDown==(held_keys==0));++held_keys;
                }
                if(records[index].Event.KeyEvent.wVirtualKeyCode=='C') {
                    CHECK(records[index].Event.KeyEvent.bKeyDown==(returned_keys==0));++returned_keys;
                }
            }
            CHECK(keys==2 && held_keys==2 && returned_keys==2);
        }
        if(wait==WAIT_OBJECT_0 && wcstoul(argv[2],NULL,10)==39) {
            INPUT_RECORD records[16];DWORD count,index,keys=0;
            CHECK(PeekConsoleInputW(GetStdHandle(STD_INPUT_HANDLE),records,16,&count));
            for(index=0;index<count;++index)if(records[index].EventType==KEY_EVENT) {
                CHECK(keys<4 && records[index].Event.KeyEvent.wVirtualKeyCode==(keys<2 ? 'L' : 'M'));
                CHECK(records[index].Event.KeyEvent.bKeyDown==!(keys&1));
                CHECK(records[index].Event.KeyEvent.uChar.UnicodeChar==((keys&1) ? 0 : keys<2 ? L'l' : L'm'));
                ++keys;
            }
            CHECK(keys==4);
        }
        return wait==WAIT_OBJECT_0 ? (int)wcstoul(argv[2],NULL,10) : (int)ERROR_TIMEOUT;
    }
    finished=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(finished);
    guard=CreateThread(NULL,0,watchdog,NULL,0,NULL);CHECK(guard);
    exercise(37,TRUE,FALSE);exercise(0,TRUE,FALSE);exercise(STILL_ACTIVE,TRUE,FALSE);
    exercise(39,TRUE,FALSE);
    puts("PASS stable Window retains DOS-returned L and gap-typed M before first native launch, ordered exactly once with native characters");
    puts("PASS production async frontend preserves completed 37/0/259 after direct target completion");
    exercise(41,FALSE,FALSE);exercise(43,FALSE,TRUE);
    puts("PASS frontend cancellation is I/O failure, not live target completion or termination");
    exercise(47,TRUE,TRUE);
    puts("PASS production teardown cancels outstanding presentation and bounds ConPTY cleanup");
    puts("PASS Window mouse through production frontend/ConPTY to native ReadConsoleInput: capture gesture excluded, left/right pairs and focus release");
    SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
    CloseHandle(guard);CloseHandle(finished);return 0;
}
