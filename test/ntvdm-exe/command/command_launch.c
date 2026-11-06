#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntvdm-exe/command/include/command_launch.h"
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"
#include "common/native-hook/context.h"
#include "common/protocol/console_io.h"

static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"line %u: %s error=%lu\n",__LINE__,#x,GetLastError());exit(1);} } while(0)
static DWORD install_error,path_error;
static HANDLE endpoint;
static LONG ambient_reads;

DWORD common_product_path_a(PCWSTR relative,char *out,size_t capacity)
{
    (void)relative;
    if(path_error)return path_error;
    return GetModuleFileNameA(NULL,out,(DWORD)capacity) ? 0 : GetLastError();
}
DWORD nthook_context_paths(nthook_context *context)
{ ZeroMemory(context,sizeof(*context));return 0; }
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode)
{ (void)child;(void)context;(void)mode;return install_error; }
BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *a,HANDLE *b)
{ InterlockedIncrement(&ambient_reads);*a=*b=NULL;SetLastError(ERROR_INVALID_STATE);return FALSE; }
DWORD ntvdm_console_handle_kind(HANDLE stream)
{ return stream==endpoint ? 2 : 0; }
unsigned int mvdm_command_native_child_command_service(const char **tail)
{ InterlockedIncrement(&ambient_reads);*tail=NULL;return MVDM_COMMAND_SERVICE_NONE; }

static int child(int argc,char **argv)
{
    typedef BOOL (WINAPI *compare_fn)(HANDLE,HANDLE);
    compare_fn compare=(compare_fn)(void *)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    char cwd[MAX_PATH],value[128],expected[MAX_PATH];
    DWORD written,flags;
    HANDLE forbidden,reference;
    BOOL present;
    if(argc!=4)return 91;
    forbidden=(HANDLE)(ULONG_PTR)_strtoui64(argv[3],NULL,16);
    present=GetHandleInformation(forbidden,&flags);
    if(!compare || !GetEnvironmentVariableA("FOREIGN_EVENT",value,sizeof(value)))return 92;
    reference=OpenEventA(SYNCHRONIZE,FALSE,value);
    if(!reference)return 92;
    /* Numeric handle values can be reused by child startup. Compare the
     * object, not merely whether that integer is valid in the new process. */
    if(present && compare(forbidden,reference)){CloseHandle(reference);return 92;}
    CloseHandle(reference);
    if(!GetCurrentDirectoryA(sizeof(cwd),cwd) ||
        !GetEnvironmentVariableA("EXPECTED_CWD",expected,sizeof(expected)) ||
        _stricmp(cwd,expected))return 93;
    if(!GetEnvironmentVariableA("CASE",value,sizeof(value)) || strcmp(value,argv[2]))return 94;
    if(!GetEnvironmentVariableA("NTVDM_FRONTEND_CAPABILITY",value,sizeof(value)) ||
        !GetHandleInformation((HANDLE)(ULONG_PTR)_strtoui64(value,NULL,16),&flags))return 95;
    if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),argv[2],1,&written,NULL) || written!=1)return 96;
    return argv[2][0];
}

typedef struct job { ntvdm_command_launch *request;HANDLE context[2];PROCESS_INFORMATION process;DWORD error; } job;
static DWORD WINAPI launch(void *argument)
{
    job *j=argument;
    j->error=ntvdm_command_launch_start(j->request,j->context[0],j->context[1],0,&j->process);
    return 0;
}

int main(int argc,char **argv)
{
    HANDLE input,readers[2],writers[2],streams[3],caps[2],threads[2],foreign;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    char cwd[MAX_PATH],alternate[MAX_PATH],command[256],environment[1024],output[8],event_name[80];
    DWORD count_before,count_after,bytes,code;
    job jobs[2]={0};ntvdm_command_launch *bad=NULL;
    size_t used;
    unsigned i;
    if(argc>1 && !strcmp(argv[1],"--warmup"))return 0;
    if(argc==2 && !strcmp(argv[1],"command")){
        char mask[8];
        return GetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,mask,sizeof(mask)) &&
            !strcmp(mask,"6") ? 73 : 97;
    }
    if(argc>1 && !strcmp(argv[1],"--child"))return child(argc,argv);
    {
        STARTUPINFOA startup={sizeof(startup)};
        PROCESS_INFORMATION process={0};
        char self[MAX_PATH],warmup[MAX_PATH+32];
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&count_before));
        CHECK(GetModuleFileNameA(NULL,self,sizeof(self)));
        sprintf_s(warmup,sizeof(warmup),"\"%s\" --warmup",self);
        CHECK(CreateProcessA(NULL,warmup,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        CHECK(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0);
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&count_after));
        printf("raw CreateProcess startup handles before=%lu after=%lu\n",count_before,count_after);
        /* Windows also retains a NamedPipe root handle on the first
         * CreatePipe. Initialize that OS facility before measuring requests. */
        count_before=count_after;
        CHECK(CreatePipe(&readers[0],&writers[0],NULL,0));
        CloseHandle(readers[0]);CloseHandle(writers[0]);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&count_after));
        printf("raw CreatePipe startup handles before=%lu after=%lu\n",count_before,count_after);
    }
    CHECK(GetCurrentDirectoryA(sizeof(cwd),cwd));
    CHECK(GetWindowsDirectoryA(alternate,sizeof(alternate)));
    input=CreateFileA("NUL",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(input!=INVALID_HANDLE_VALUE);
    sprintf_s(event_name,sizeof(event_name),"Local\\ntvdm-s3-forbidden-%lu",GetCurrentProcessId());
    foreign=CreateEventA(&security,TRUE,FALSE,event_name);CHECK(foreign);
    for(i=0;i<2;++i){caps[i]=CreateEventA(NULL,TRUE,FALSE,NULL);CHECK(caps[i]);}
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&count_before));
    CloseHandle(caps[0]);CloseHandle(caps[1]);caps[0]=caps[1]=NULL;
    for(i=0;i<2;++i){
        CHECK(CreatePipe(&readers[i],&writers[i],NULL,0));
        streams[0]=input;streams[1]=streams[2]=writers[i];
        sprintf_s(command,sizeof(command),"--child %c %llx",'A'+i,(unsigned long long)(ULONG_PTR)foreign);
        used=(size_t)sprintf_s(environment,sizeof(environment),"CASE=%c",'A'+i)+1;
        used+=(size_t)sprintf_s(environment+used,sizeof(environment)-used,"EXPECTED_CWD=%s",i ? alternate : cwd)+1;
        used+=(size_t)sprintf_s(environment+used,sizeof(environment)-used,"FOREIGN_EVENT=%s",event_name)+1;
        environment[used++]=0;
        CHECK(!ntvdm_command_launch_prepare(i ? MVDM_COMMAND_SERVICE_EXECUTE : MVDM_COMMAND_SERVICE_NONE,
            command,environment,used,i ? alternate : cwd,streams,&jobs[i].request));
        /* Request must outlive its input arrays and source handles. */
        memset(command,'x',sizeof(command));memset(environment,'x',used);
        CloseHandle(writers[i]);
        {
            PROCESS_INFORMATION rejected={0};
            CHECK(ntvdm_command_launch_start(jobs[i].request,NULL,NULL,0,&rejected)==ERROR_INVALID_PARAMETER);
            CHECK(!rejected.hProcess && !rejected.hThread);
        }
    }
    /* Authenticated contexts arrive after preparation and source closure. */
    for(i=0;i<2;++i){caps[i]=CreateEventA(NULL,TRUE,FALSE,NULL);CHECK(caps[i]);}
    for(i=0;i<2;++i){
        jobs[i].context[0]=caps[0];jobs[i].context[1]=caps[1];
        threads[i]=CreateThread(NULL,0,launch,&jobs[i],0,NULL);CHECK(threads[i]);
    }
    CHECK(WaitForMultipleObjects(2,threads,TRUE,10000)==WAIT_OBJECT_0);
    for(i=0;i<2;++i){
        PROCESS_INFORMATION rejected={0};
        CHECK(!jobs[i].error);
        CHECK(WaitForSingleObject(jobs[i].process.hProcess,10000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(jobs[i].process.hProcess,&code));
        printf("request %u child exit=%lu\n",i,code);CHECK(code==(DWORD)'A'+i);
        CHECK(ReadFile(readers[i],output,sizeof(output),&bytes,NULL));CHECK(bytes==1 && output[0]==(char)('A'+i));
        CHECK(ntvdm_command_launch_start(jobs[i].request,caps[0],caps[1],0,&rejected)==ERROR_INVALID_STATE);
        CloseHandle(jobs[i].process.hThread);CloseHandle(jobs[i].process.hProcess);
        CloseHandle(readers[i]);CloseHandle(threads[i]);
        ntvdm_command_launch_destroy(jobs[i].request);
    }
    CloseHandle(caps[0]);CloseHandle(caps[1]);
    /* Failure coverage uses valid borrowed inputs, then invalidates one late
     * duplicate to exercise rollback after earlier stream duplicates exist. */
    caps[0]=CreateEventA(NULL,TRUE,FALSE,NULL);caps[1]=CreateEventA(NULL,TRUE,FALSE,NULL);
    {
        PROCESS_INFORMATION process={0};
        HANDLE output_endpoint=CreateEventA(NULL,TRUE,FALSE,NULL);CHECK(output_endpoint);
        streams[0]=input;streams[1]=streams[2]=output_endpoint;endpoint=output_endpoint;
        CHECK(!ntvdm_command_launch_prepare(MVDM_COMMAND_SERVICE_INTERACTIVE,"ignored",
            "\0",2,cwd,streams,&bad));
        endpoint=NULL;CloseHandle(output_endpoint);
        CHECK(!ntvdm_command_launch_start(bad,caps[0],caps[1],CREATE_SUSPENDED,&process));
        CHECK(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT);
        CHECK(ResumeThread(process.hThread)!=(DWORD)-1);
        CHECK(WaitForSingleObject(process.hProcess,10000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(process.hProcess,&code));CHECK(code==73);
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        ntvdm_command_launch_destroy(bad);bad=NULL;
    }
    streams[0]=input;streams[1]=input;streams[2]=INVALID_HANDLE_VALUE;
    CHECK(ntvdm_command_launch_prepare(0,"x","\0",2,cwd,streams,&bad)==ERROR_INVALID_HANDLE);
    CHECK(!bad);
    CHECK(ntvdm_command_launch_prepare(0,"x","\0",2,"relative",streams,&bad)==ERROR_BAD_PATHNAME);
    CHECK(ntvdm_command_launch_prepare(0,"x","A=1\0junk",9,cwd,streams,&bad)==ERROR_INVALID_DATA);
    streams[2]=input;endpoint=input;
    CHECK(ntvdm_command_launch_prepare(0,"x","\0",2,cwd,streams,&bad)==ERROR_INVALID_HANDLE);
    endpoint=NULL;
    for(i=0;i<2;++i){
        PROCESS_INFORMATION process={0};
        CHECK(!ntvdm_command_launch_prepare(0,"x","\0",2,cwd,streams,&bad));
        if(i)install_error=ERROR_ACCESS_DENIED;else path_error=ERROR_FILE_NOT_FOUND;
        CHECK(ntvdm_command_launch_start(bad,caps[0],caps[1],0,&process)==(DWORD)(i ? ERROR_ACCESS_DENIED : ERROR_FILE_NOT_FOUND));
        CHECK(!process.hProcess && !process.hThread);
        CHECK(ntvdm_command_launch_start(bad,caps[0],caps[1],0,&process)==ERROR_INVALID_STATE);
        path_error=install_error=0;ntvdm_command_launch_destroy(bad);
    }
    CloseHandle(caps[0]);CloseHandle(caps[1]);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&count_after));
    printf("handles before=%lu after=%lu expected=%lu\n",count_before,count_after,count_before-2);
    CHECK(count_after==count_before-2);CHECK(!ambient_reads);
    CloseHandle(input);CloseHandle(foreign);
    printf("PASS %u per-request launch assertions\n",checks);return 0;
}
