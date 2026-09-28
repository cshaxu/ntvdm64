/* Real x86 child creation through the production frontend launch binding.
 * No desktop interaction, helper role, guest, or installed-package mutation. */
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include "ntkvm-exe/native_conpty.h"
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

typedef struct report {
    DWORD console_mask, file_mask, output_mask, stale_environment;
    DWORD aliases, capability_ok, input_ok;
} report;
typedef struct output { DWORD count; } output;

static DWORD capture_output(void *context,const BYTE *bytes,DWORD count)
{
    output *capture=context;
    (void)bytes;capture->count+=count;
    return 0;
}

static HANDLE capability(PCWSTR name)
{
    WCHAR value[64];
    if(!GetEnvironmentVariableW(name,value,64))return NULL;
    return (HANDLE)(ULONG_PTR)wcstoul(value,NULL,16);
}

static int child(void)
{
    HANDLE mapping=capability(L"NTVDM_FRONTEND_CAPABILITY");
    HANDLE event=capability(L"NTVDM_EXECUTION_CONSOLE");
    report *result=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(report));
    HANDLE streams[3];DWORD i,mode,written;WCHAR value[64];
    if(!result)return 80;
    for(i=0;i<3;++i) {
        streams[i]=GetStdHandle(i==0 ? STD_INPUT_HANDLE : i==1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        if(GetConsoleMode(streams[i],&mode))result->console_mask|=1u<<i;
        if(GetFileType(streams[i])==FILE_TYPE_DISK)result->file_mask|=1u<<i;
        if(i && WriteFile(streams[i],i==1 ? "OUT" : "ERR",3,&written,NULL) && written==3)
            result->output_mask|=1u<<i;
    }
    if(result->console_mask&1) {
        INPUT_RECORD record;DWORD read,attempt;
        SetConsoleMode(streams[0],0);
        for(attempt=0;attempt<16;++attempt) {
            if(WaitForSingleObject(streams[0],3000)!=WAIT_OBJECT_0 ||
                !ReadConsoleInputW(streams[0],&record,1,&read) || read!=1)break;
            if(record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown &&
                record.Event.KeyEvent.uChar.UnicodeChar==L'Z') {result->input_ok=1;break;}
        }
    } else {
        char input_byte=0;DWORD read=0;
        result->input_ok=ReadFile(streams[0],&input_byte,1,&read,NULL) && read==1 && input_byte=='I';
    }
    result->aliases=streams[1]==streams[2];
    result->stale_environment=GetEnvironmentVariableW(L"NTVDM_COMMAND_STREAMS_V1",value,64)!=0;
    result->capability_ok=SetEvent(event);
    UnmapViewOfFile(result);CloseHandle(mapping);CloseHandle(event);
    return 37;
}

static int run_case(PCWSTR self,PCWSTR directory,DWORD mask,BOOL alias,DWORD negative)
{
    HANDLE mapping=NULL,event=NULL,files[3]={NULL},unique[3]={NULL};
    ntkvm_conpty *console=NULL;PROCESS_INFORMATION process={0};
    run16_native_start start={0};report *result=NULL;
    WCHAR command[1024],path[MAX_PATH];DWORD error=0,code=MAXDWORD,i,count=0;
    COORD size={80,25};output capture={0};BOOL pass=FALSE;
    static const WCHAR environment[]=L"NTVDM_FRONTEND_CAPABILITY=dead\0NTVDM_EXECUTION_CONSOLE=beef\0NTVDM_COMMAND_STREAMS_V1=stale\0\0";
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(report),NULL);
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!mapping || !event)goto done;
    result=MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(report));
    if(!result)goto done;
    for(i=0;i<3;++i) {
        if(mask&(1u<<i))continue;
        if(i==2 && alias && files[1]) {files[i]=files[1];continue;}
        if(!GetTempFileNameW(directory,L"pty",0,path))goto done;
        files[i]=CreateFileW(path,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
            FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,NULL);
        if(files[i]==INVALID_HANDLE_VALUE)goto done;
        unique[count++]=files[i];
        if(i==0) {
            DWORD written;
            if(!WriteFile(files[i],"I",1,&written,NULL) || written!=1 ||
                SetFilePointer(files[i],0,NULL,FILE_BEGIN)==INVALID_SET_FILE_POINTER)goto done;
        }
    }
    error=ntkvm_conpty_open(size,capture_output,&capture,&console);
    if(error)goto done;
    swprintf_s(command,1024,L"\"%ls\" --child",self);
    start.application=self;start.command=command;start.directory=directory;start.environment=environment;
    start.console_mask=mask;start.capabilities[0]=mapping;start.capabilities[1]=event;
    for(i=0;i<3;++i)start.standard[i]=files[i];
    if(negative==1)start.capabilities[1]=NULL;
    if(negative==2)start.capabilities[1]=INVALID_HANDLE_VALUE;
    if(negative==3)start.application=L"?:\\invalid-conpty-target.exe";
    error=ntkvm_conpty_launch(console,&start,&process);
    if(negative) {
        pass=(negative==1 ? error==ERROR_INVALID_PARAMETER :
            negative==2 ? error==ERROR_INVALID_HANDLE : error!=ERROR_SUCCESS) &&
            !process.hProcess && !process.hThread;
        goto done;
    }
    if(error)goto done;
    if(mask&1) {
        static const char key[]="\x1b[90;44;90;1;0;1_\x1b[90;44;90;0;0;1_";
        DWORD delivered;
        error=ntkvm_conpty_write(console,key,sizeof(key)-1,&delivered);
        if(error || delivered!=sizeof(key)-1)goto done;
    }
    if(WaitForSingleObject(process.hProcess,10000)!=WAIT_OBJECT_0)goto done;
    GetExitCodeProcess(process.hProcess,&code);
    pass=code==37 && result->console_mask==mask && result->file_mask==(7u^mask) &&
        result->output_mask==6 && result->input_ok && result->capability_ok && !result->stale_environment &&
        WaitForSingleObject(event,0)==WAIT_OBJECT_0 &&
        (!alias || (mask&6) || result->aliases);
    for(i=1;i<3 && pass;++i)if(files[i]) {
        char data[8]={0};DWORD length=0;
        const char *expected=alias && !(mask&6) ? "OUTERR" : i==1 ? "OUT" : "ERR";
        pass=SetFilePointer(files[i],0,NULL,FILE_BEGIN)!=INVALID_SET_FILE_POINTER &&
            ReadFile(files[i],data,sizeof(data),&length,NULL) &&
            length==strlen(expected) && !memcmp(data,expected,length);
    }
    if(pass) {
        PROCESS_INFORMATION rejected={0};COORD resized={90,30};
        pass=ntkvm_conpty_resize(console,resized)==0 && ntkvm_conpty_release(console)==0 &&
            ntkvm_conpty_release(console)==0 &&
            WaitForSingleObject(ntkvm_conpty_ended(console),5000)==WAIT_OBJECT_0 &&
            ntkvm_conpty_error(console)==0 &&
            ntkvm_conpty_launch(console,&start,&rejected)==ERROR_BROKEN_PIPE && !rejected.hProcess;
    }
done:
    if(process.hProcess && WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT) {
        TerminateProcess(process.hProcess,99);WaitForSingleObject(process.hProcess,5000);
    }
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(console)ntkvm_conpty_close(console);
    printf("mask=%lu alias=%u negative=%lu error=%lu exit=%lu console=%lu files=%lu writes=%lu capabilities=%lu output=%lu %s\n",
        mask,alias,negative,error,code,result ? result->console_mask : 0,result ? result->file_mask : 0,
        result ? result->output_mask : 0,result ? result->capability_ok : 0,capture.count,pass ? "PASS" : "FAIL");
    if(result)UnmapViewOfFile(result);
    if(mapping)CloseHandle(mapping);if(event)CloseHandle(event);
    for(i=0;i<count;++i)CloseHandle(unique[i]);
    return pass ? 0 : 1;
}

static int cancelled_input(void)
{
    ntkvm_conpty *console=NULL;COORD size={80,25};output capture={0};
    DWORD delivered=99,error;BOOL pass;
    error=ntkvm_conpty_open(size,capture_output,&capture,&console);
    if(error)return 1;
    ntkvm_conpty_cancel(console);
    error=ntkvm_conpty_write(console,"cancelled",9,&delivered);
    pass=error==ERROR_BROKEN_PIPE && delivered==0 &&
        WaitForSingleObject(ntkvm_conpty_ended(console),0)==WAIT_OBJECT_0 &&
        ntkvm_conpty_error(console)==error;
    ntkvm_conpty_close(console);
    printf("cancelled-input error=%lu delivered=%lu %s\n",error,delivered,pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}

int wmain(int argc,WCHAR **argv)
{
    WCHAR self[MAX_PATH],directory[MAX_PATH];DWORD mask;int failures=0;
    if(argc==2 && !wcscmp(argv[1],L"--child"))return child();
    if(argc!=2 || !GetModuleFileNameW(NULL,self,MAX_PATH) ||
        !GetFullPathNameW(argv[1],MAX_PATH,directory,NULL))return 2;
    for(mask=0;mask<8;++mask)failures+=run_case(self,directory,mask,FALSE,0);
    failures+=run_case(self,directory,1,TRUE,0);
    for(mask=1;mask<=3;++mask)failures+=run_case(self,directory,7,FALSE,mask);
    failures+=cancelled_input();
    printf("CONPTY-LAUNCH failures=%d\n",failures);return failures ? 1 : 0;
}
