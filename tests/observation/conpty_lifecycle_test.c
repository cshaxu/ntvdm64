/* S9 capability probe, not a product backend. No desktop windows or guest.
 * Each case owns only its exact authored processes and one ConPTY session. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

typedef struct shared_state { DWORD leaf_pid; } shared_state;
typedef struct capture {
    HANDLE pipe;
    DWORD error, used;
    BOOL overflow;
    char bytes[65536];
} capture;
static HANDLE close_seen;

static BOOL WINAPI close_handler(DWORD event)
{
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
    HANDLE mapping,ready,release,screen;
    DWORD written;
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
    name(key,128,prefix,L"close");close_seen=OpenEventW(EVENT_MODIFY_STATE,FALSE,key);
    if(!state || !ready || !release || !close_seen || !SetConsoleCtrlHandler(close_handler,TRUE))return 63;
    screen=CreateFileW(L"CONOUT$",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    if(screen==INVALID_HANDLE_VALUE)return 65;
    state->leaf_pid=GetCurrentProcessId();
    if(!WriteFile(screen,"CONPTY-LEAF-READY\r\n",19,&written,NULL) || written!=19)return 66;
    SetEvent(ready);
    if(WaitForSingleObject(release,30000)!=WAIT_OBJECT_0)return 64;
    if(!WriteFile(screen,"CONPTY-LEAF-FINAL\r\n",19,&written,NULL) || written!=19)return 67;
    CloseHandle(screen);
    UnmapViewOfFile(state);CloseHandle(mapping);CloseHandle(ready);
    CloseHandle(release);CloseHandle(close_seen);
    return 23;
}
static int run_case(unsigned mode)
{
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
        mode==2 ? "released-natural-retirement" : explicit_close ? "explicit-session-close" : "descendant-lifetime",result ? "no" : "yes",
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
    return run_case(0) | run_case(1) | run_case(2);
}
