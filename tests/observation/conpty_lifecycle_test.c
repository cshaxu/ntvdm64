/* S9 capability probe, not a product backend. No desktop windows or guest.
 * Each case owns only its exact authored processes and one ConPTY session. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

typedef struct shared_state {
    DWORD leaf_pid,count,reclaimed;
    LONG control;
    WCHAR cooked[32];
    char vt[32];
    INPUT_RECORD records[8];
} shared_state;
typedef struct capture {
    HANDLE pipe;
    DWORD error, used;
    BOOL overflow;
    char bytes[65536];
} capture;
static HANDLE close_seen;
static shared_state *control_state;

static BOOL WINAPI close_handler(DWORD event)
{
    if(event==CTRL_C_EVENT || event==CTRL_BREAK_EVENT) {
        if(control_state)InterlockedExchange(&control_state->control,(LONG)event);
        SetEvent(close_seen);return TRUE;
    }
    if(event != CTRL_CLOSE_EVENT) return FALSE;
    SetEvent(close_seen);
    ExitProcess(91);
}
static DWORD WINAPI drain(void *context)
{
    capture *output = context;
    char data[2048];
    DWORD count;
    while(ReadFile(output->pipe,data,sizeof(data),&count,NULL) && count) {
        if(count > sizeof(output->bytes)-1-output->used) output->overflow=TRUE;
        else { memcpy(output->bytes+output->used,data,count);output->used+=count; }
    }
    output->error=GetLastError();
    output->bytes[output->used]=0;
    return 0;
}
static void name(WCHAR *buffer,size_t capacity,PCWSTR prefix,PCWSTR suffix)
{
    swprintf_s(buffer,capacity,L"Local\\%ls-%ls",prefix,suffix);
}
static int child(PCWSTR role,PCWSTR prefix)
{
    WCHAR key[128],self[MAX_PATH],command[1024];
    HANDLE mapping,ready,release,screen,input=NULL;
    DWORD written,mode=wcstoul(wcsrchr(prefix,L'-')+1,NULL,10);
    shared_state *state;
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};
    if(!wcscmp(role,L"leader")) {
        if(!GetModuleFileNameW(NULL,self,MAX_PATH))return 60;
        swprintf_s(command,1024,L"\"%ls\" leaf %ls",self,prefix);
        if(!CreateProcessW(self,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&process))return 61;
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        return 37;
    }
    name(key,128,prefix,L"state");
    mapping=OpenFileMappingW(FILE_MAP_WRITE,FALSE,key);
    if(!mapping)return 62;
    state=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(*state));
    name(key,128,prefix,L"ready");ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,key);
    name(key,128,prefix,L"release");release=OpenEventW(SYNCHRONIZE,FALSE,key);
    name(key,128,prefix,L"close");close_seen=OpenEventW(EVENT_MODIFY_STATE|SYNCHRONIZE,FALSE,key);
    if(!state || !ready || !release || !close_seen || !SetConsoleCtrlHandler(close_handler,TRUE))return 63;
    screen=CreateFileW(L"CONOUT$",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    if(screen==INVALID_HANDLE_VALUE)return 65;
    if(mode>=3) {
        DWORD flags=ENABLE_EXTENDED_FLAGS|ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT;
        if(mode==7)flags|=ENABLE_PROCESSED_INPUT|ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT;
        if(mode==8)flags|=ENABLE_VIRTUAL_TERMINAL_INPUT;
        if(mode==9 || mode==10)flags|=ENABLE_PROCESSED_INPUT;
        input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,OPEN_EXISTING,0,NULL);
        if(input==INVALID_HANDLE_VALUE || !SetConsoleMode(input,flags) ||
            !FlushConsoleInputBuffer(input))return 68;
    }
    control_state=state;state->control=-1;
    if(mode>=9 && !SetConsoleCtrlHandler(NULL,FALSE))return 73;
    state->leaf_pid=GetCurrentProcessId();
    if(!WriteFile(screen,"CONPTY-LEAF-READY\r\n",19,&written,NULL) || written!=19)return 66;
    SetEvent(ready);
    if(mode>=9) {
        if(WaitForSingleObject(close_seen,5000)!=WAIT_OBJECT_0)return 74;
    } else if(mode==7) {
        if(!ReadConsoleW(input,state->cooked,31,&state->count,NULL))return 75;
    } else if(mode==8) {
        if(!ReadFile(input,state->vt,31,&state->count,NULL))return 76;
    } else if(mode>=3) {
        DWORD expected=mode==4 ? 3 : mode==5 ? 2 : 4;
        while(state->count<expected) {
            DWORD count=0;
            if(WaitForSingleObject(input,5000)!=WAIT_OBJECT_0)return 69;
            if(!ReadConsoleInputW(input,state->records+state->count,1,&count) || count!=1)return 70;
            ++state->count;
            if(mode==6 && state->count==2) {
                INPUT_RECORD peek[8];
                /* The first key pair was consumed. Reclaim only records
                 * demonstrably still in the native Console queue. */
                if(WaitForSingleObject(input,5000)!=WAIT_OBJECT_0 ||
                   !PeekConsoleInputW(input,peek,8,&count) || count<2)return 71;
                if(!ReadConsoleInputW(input,state->records+2,2,&count) || count!=2)return 72;
                state->reclaimed=count;state->count+=count;
            }
        }
    }
    if(input)CloseHandle(input);
    if(WaitForSingleObject(release,30000)!=WAIT_OBJECT_0)return 64;
    if(!WriteFile(screen,"CONPTY-LEAF-FINAL\r\n",19,&written,NULL) || written!=19)return 67;
    CloseHandle(screen);
    UnmapViewOfFile(state);CloseHandle(mapping);CloseHandle(ready);
    CloseHandle(release);CloseHandle(close_seen);
    return 23;
}
static int run_case(unsigned mode)
{
    static const char *case_names[]={"descendant-lifetime","explicit-session-close",
        "released-natural-retirement","win32-key-records","sgr-mouse-records",
        "focus-records","unread-input-records","cooked-line","vt-input","ctrl-c","ctrl-break"};
    static const char *key_sequence="\x1b[17;29;0;1;8;1_\x1b[112;59;0;1;8;1_\x1b[112;59;0;0;8;1_\x1b[17;29;0;0;0;1_";
    static const char *mouse_sequence="\x1b[<0;11;7M\x1b[<32;12;8M\x1b[<0;12;8m";
    static const char *unread_sequence="\x1b[65;30;97;1;0;1_\x1b[65;30;97;0;0;1_\x1b[66;48;98;1;0;1_\x1b[66;48;98;0;0;1_";
    typedef HRESULT (WINAPI *release_console_fn)(HPCON);
    release_console_fn release_console=(release_console_fn)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"),"ReleasePseudoConsole");
    BOOL explicit_close=mode==1;
    WCHAR prefix[96],key[128],self[MAX_PATH],command[1024];
    HANDLE mapping=NULL,ready=NULL,release=NULL,closed=NULL,leaf=NULL;
    HANDLE input_read=NULL,input_write=NULL,output_write=NULL,reader=NULL;
    shared_state *state=NULL;
    capture output={0};
    HPCON console=NULL;
    STARTUPINFOEXW startup={0};
    PROCESS_INFORMATION leader={0};
    SIZE_T attribute_bytes=0;
    DWORD code=0,leaf_code=0,preclose=WAIT_FAILED;
    BOOL attributes_ready=FALSE;
    int result=1;
    const char *stage="setup";
    swprintf_s(prefix,96,L"ntvdm-conpty-%lu-%u",GetCurrentProcessId(),mode);
    name(key,128,prefix,L"state");
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(*state),key);
    if(!mapping || GetLastError()==ERROR_ALREADY_EXISTS)goto done;
    state=MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*state));
    name(key,128,prefix,L"ready");ready=CreateEventW(NULL,TRUE,FALSE,key);
    name(key,128,prefix,L"release");release=CreateEventW(NULL,TRUE,FALSE,key);
    name(key,128,prefix,L"close");closed=CreateEventW(NULL,TRUE,FALSE,key);
    if(!state || !ready || !release || !closed)goto done;
    if(!CreatePipe(&input_read,&input_write,NULL,0) ||
       !CreatePipe(&output.pipe,&output_write,NULL,0))goto done;
    stage="CreatePseudoConsole";
    if(FAILED(CreatePseudoConsole((COORD){80,25},input_read,output_write,0,&console)))goto done;
    CloseHandle(input_read);input_read=NULL;CloseHandle(output_write);output_write=NULL;
    reader=CreateThread(NULL,0,drain,&output,0,NULL);
    if(!reader)goto done;
    InitializeProcThreadAttributeList(NULL,1,0,&attribute_bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,attribute_bytes);
    if(!startup.lpAttributeList || !InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attribute_bytes))goto done;
    attributes_ready=TRUE;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
        console,sizeof(console),NULL,NULL))goto done;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH))goto done;
    swprintf_s(command,1024,L"\"%ls\" leader %ls",self,prefix);
    startup.StartupInfo.cb=sizeof(startup);
    stage="launch";
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,EXTENDED_STARTUPINFO_PRESENT,
        NULL,NULL,&startup.StartupInfo,&leader))goto done;
    stage="leader exit and leaf ready";
    if(WaitForSingleObject(ready,10000)!=WAIT_OBJECT_0 ||
       WaitForSingleObject(leader.hProcess,10000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(leader.hProcess,&code) || code!=37)goto done;
    leaf=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,state->leaf_pid);
    stage="descendant survives direct target";
    if(!leaf || WaitForSingleObject(leaf,300)!=WAIT_TIMEOUT ||
       WaitForSingleObject(reader,0)!=WAIT_TIMEOUT)goto done;
    if(mode==2) {
        stage="release Console ownership without closing clients";
        if(!release_console || FAILED(release_console(console)) ||
           WaitForSingleObject(leaf,300)!=WAIT_TIMEOUT ||
           WaitForSingleObject(reader,0)!=WAIT_TIMEOUT)goto done;
    }
    if(mode>=3) {
        const char *sequence=(mode==3 || mode==8) ? key_sequence : mode==4 ? mouse_sequence :
            mode==5 ? "\x1b[I\x1b[O" : mode==6 ? unread_sequence :
            mode==7 ? "\x1b[65;30;97;1;0;1_\x1b[65;30;97;0;0;1_\x1b[13;28;13;1;0;1_\x1b[13;28;13;0;0;1_" :
            mode==9 ? "\x1b[67;46;3;1;8;1_\x1b[67;46;3;0;8;1_" :
            "\x1b[3;70;0;1;264;1_\x1b[3;70;0;0;264;1_";
        DWORD sent;
        stage="send native input sequence";
        if(!WriteFile(input_write,sequence,(DWORD)strlen(sequence),&sent,NULL) || sent!=strlen(sequence))goto done;
    }
    if(explicit_close) {
        stage="explicit close notification";
        ClosePseudoConsole(console);console=NULL;
        if(WaitForSingleObject(closed,10000)!=WAIT_OBJECT_0)goto done;
    } else {
        stage="release descendant";
        SetEvent(release);
    }
    if(WaitForSingleObject(leaf,10000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(leaf,&leaf_code) ||
       leaf_code!=(explicit_close ? 91u : 23u))goto done;
    if(mode>=7) {
        stage="cooked VT and control input";
        printf("input case=%s count=%lu control=%ld vt=",case_names[mode],state->count,state->control);
        if(mode==8)for(DWORD i=0;i<state->count;++i)printf("%02x",(unsigned char)state->vt[i]);
        printf("\n");
        if(mode==7 && (state->count!=3 || wmemcmp(state->cooked,L"a\r\n",3)))goto done;
        if(mode==8 && (state->count!=6 || memcmp(state->vt,"\x1b[1;5P",6)))goto done;
        if(mode==9 && state->control!=CTRL_C_EVENT)goto done;
        if(mode==10 && state->control!=CTRL_BREAK_EVENT)goto done;
    } else if(mode>=3) {
        INPUT_RECORD *r=state->records;
        DWORD i;
        stage="exact target input records";
        for(i=0;i<state->count;++i) {
            if(r[i].EventType==MOUSE_EVENT)printf("input case=%s index=%lu mouse=%d,%d buttons=%lu flags=%lu\n",
                case_names[mode],i,r[i].Event.MouseEvent.dwMousePosition.X,
                r[i].Event.MouseEvent.dwMousePosition.Y,r[i].Event.MouseEvent.dwButtonState,
                r[i].Event.MouseEvent.dwEventFlags);
            else if(r[i].EventType==FOCUS_EVENT)printf("input case=%s index=%lu focus=%d\n",
                case_names[mode],i,r[i].Event.FocusEvent.bSetFocus);
            else printf("input case=%s index=%lu type=%u vk=%u scan=%u down=%u unicode=%u flags=%lu\n",
                case_names[mode],i,r[i].EventType,r[i].Event.KeyEvent.wVirtualKeyCode,
                r[i].Event.KeyEvent.wVirtualScanCode,r[i].Event.KeyEvent.bKeyDown,
                r[i].Event.KeyEvent.uChar.UnicodeChar,r[i].Event.KeyEvent.dwControlKeyState);
        }
        if(mode==3) {
            const WORD vk[4]={VK_CONTROL,VK_F1,VK_F1,VK_CONTROL};
            const WORD scan[4]={29,59,59,29};
            if(state->count!=4)goto done;
            for(i=0;i<4;++i)if(r[i].EventType!=KEY_EVENT ||
                r[i].Event.KeyEvent.wVirtualKeyCode!=vk[i] ||
                r[i].Event.KeyEvent.wVirtualScanCode!=scan[i] ||
                r[i].Event.KeyEvent.bKeyDown!=(i<2) ||
                r[i].Event.KeyEvent.dwControlKeyState!=(i<3 ? (DWORD)LEFT_CTRL_PRESSED : 0u) ||
                r[i].Event.KeyEvent.wRepeatCount!=1 || r[i].Event.KeyEvent.uChar.UnicodeChar)goto done;
        } else if(mode==4) {
            if(state->count!=3)goto done;
            for(i=0;i<3;++i)if(r[i].EventType!=MOUSE_EVENT ||
                r[i].Event.MouseEvent.dwMousePosition.X!=(i ? 11 : 10) ||
                r[i].Event.MouseEvent.dwMousePosition.Y!=(i ? 7 : 6) ||
                r[i].Event.MouseEvent.dwButtonState!=(i==2 ? 0u : (DWORD)FROM_LEFT_1ST_BUTTON_PRESSED) ||
                r[i].Event.MouseEvent.dwEventFlags!=(i==1 ? (DWORD)MOUSE_MOVED : 0u))goto done;
        } else if(mode==5) {
            if(state->count!=2 || r[0].EventType!=FOCUS_EVENT || r[1].EventType!=FOCUS_EVENT ||
                !r[0].Event.FocusEvent.bSetFocus || r[1].Event.FocusEvent.bSetFocus)goto done;
        } else {
            if(state->count!=4 || state->reclaimed!=2)goto done;
            for(i=0;i<4;++i)if(r[i].EventType!=KEY_EVENT ||
                r[i].Event.KeyEvent.wVirtualKeyCode!=(i<2 ? 'A' : 'B') ||
                r[i].Event.KeyEvent.bKeyDown!=(i%2==0))goto done;
        }
    }
    preclose=WaitForSingleObject(reader,mode==2 ? 5000 : 500);
    stage="natural EOF after final attached client";
    if(mode==2 && preclose!=WAIT_OBJECT_0)goto done;
    if(console) { ClosePseudoConsole(console);console=NULL; }
    stage="final output drain";
    if(WaitForSingleObject(reader,10000)!=WAIT_OBJECT_0 || output.overflow ||
       (output.error!=ERROR_BROKEN_PIPE && output.error!=ERROR_SUCCESS) ||
       !strstr(output.bytes,"CONPTY-LEAF-READY") ||
       (!explicit_close && !strstr(output.bytes,"CONPTY-LEAF-FINAL")))goto done;
    result=0;
done:
    printf("conpty case=%s pass=%s stage=%s leader=%lu leaf=%lu eof-before-close=%lu error=%lu\n",
        case_names[mode],result ? "no" : "yes",
        stage,code,leaf_code,preclose,GetLastError());
    if(leader.hProcess && WaitForSingleObject(leader.hProcess,0)==WAIT_TIMEOUT) {
        TerminateProcess(leader.hProcess,99);WaitForSingleObject(leader.hProcess,5000);
    }
    if(release)SetEvent(release);
    if(leaf && WaitForSingleObject(leaf,1000)==WAIT_TIMEOUT) {
        TerminateProcess(leaf,99);WaitForSingleObject(leaf,5000);
    }
    if(console)ClosePseudoConsole(console);
    if(input_write)CloseHandle(input_write);
    if(reader) {
        if(WaitForSingleObject(reader,10000)!=WAIT_OBJECT_0) {
            CancelSynchronousIo(reader);
            if(WaitForSingleObject(reader,5000)!=WAIT_OBJECT_0)ExitProcess(98);
        }
        CloseHandle(reader);
    }
    if(output.pipe)CloseHandle(output.pipe);
    if(input_read)CloseHandle(input_read);
    if(output_write)CloseHandle(output_write);
    if(attributes_ready)DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList)HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(leader.hThread)CloseHandle(leader.hThread);
    if(leader.hProcess)CloseHandle(leader.hProcess);
    if(leaf)CloseHandle(leaf);
    if(state)UnmapViewOfFile(state);
    if(mapping)CloseHandle(mapping);
    if(ready)CloseHandle(ready);
    if(release)CloseHandle(release);
    if(closed)CloseHandle(closed);
    return result;
}
int wmain(int argc,WCHAR **argv)
{
    if(argc==3 && (!wcscmp(argv[1],L"leader") || !wcscmp(argv[1],L"leaf")))return child(argv[1],argv[2]);
    if(argc!=1)return 2;
    { int result=0;for(unsigned mode=0;mode<11;++mode)result|=run_case(mode);return result; }
}
