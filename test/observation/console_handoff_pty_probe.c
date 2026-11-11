/* Test-only ConPTY container for console_startup_observer's owned CAF test.
 * Captures terminal output, not just ReadConsoleOutput buffer contents.
 * Usage: probe <observer.exe> <runtime-root> <report-prefix> [conhost.exe]
 * The child and all its descendants belong to a kill-on-close test Job. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <stdio.h>

static HANDLE output_pipe, capture;
static FILE *chunks;
static DWORD WINAPI drain(void *unused)
{
    char data[8192];DWORD read,written;unsigned long long offset=0;(void)unused;
    while(ReadFile(output_pipe,data,sizeof(data),&read,NULL) && read) {
        if(chunks){fprintf(chunks,"%llu %llu %lu\n",GetTickCount64(),offset,read);fflush(chunks);}
        if(!WriteFile(capture,data,read,&written,NULL) || written!=read)return 1;
        offset+=read;
    }
    return 0;
}
int main(int argc,char **argv)
{
    HANDLE input_read=NULL,input_write=NULL,output_write=NULL,thread=NULL,job=NULL;
    HPCON pty=NULL;STARTUPINFOEXA startup={0};PROCESS_INFORMATION process={0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};SIZE_T bytes=0;
    char command[4096],client[2048],system[MAX_PATH],path[MAX_PATH];DWORD result=1,wait,written;
    BOOL explicit_host=argc==5;
    if(argc!=4 && argc!=5)return 64;
    snprintf(path,sizeof(path),"%s.pty.raw",argv[3]);
    capture=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
    if(capture==INVALID_HANDLE_VALUE)return 65;
    snprintf(path,sizeof(path),"%s.pty.chunks.txt",argv[3]);chunks=fopen(path,"w");
    if(!CreatePipe(&input_read,&input_write,NULL,0) ||
       !CreatePipe(&output_pipe,&output_write,NULL,0))goto done;
    if(!explicit_host) {
        if(FAILED(CreatePseudoConsole((COORD){100,35},input_read,output_write,0,&pty)))goto done;
        CloseHandle(input_read);input_read=NULL;CloseHandle(output_write);output_write=NULL;
    }
    thread=CreateThread(NULL,0,drain,NULL,0,NULL);if(!thread)goto done;
    if(!explicit_host) {
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!startup.lpAttributeList || !InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&bytes) ||
       !UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,pty,sizeof(pty),NULL,NULL))goto done;
    }
    startup.StartupInfo.cb=explicit_host ? sizeof(STARTUPINFOA) : sizeof(startup);
    job=CreateJobObjectA(NULL,NULL);if(!job)goto done;
    limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)))goto done;
    SetEnvironmentVariableA("MVDM_OBSERVER_OWNED_PTY","1");
    if(!explicit_host){ char handle_text[32];snprintf(handle_text,sizeof(handle_text),"%llx",(unsigned long long)(ULONG_PTR)input_write);
      SetHandleInformation(input_write,HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT);
      SetEnvironmentVariableA("MVDM_OBSERVER_PTY_INPUT",handle_text); }
    else {
        SetEnvironmentVariableA("MVDM_OBSERVER_PTY_INPUT",NULL);
        SetEnvironmentVariableA("MVDM_OBSERVER_DETACH_BEFORE_CLOSE",NULL);
        if(!SetHandleInformation(input_read,HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT) ||
           !SetHandleInformation(output_write,HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT))goto done;
        startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
        startup.StartupInfo.wShowWindow=SW_HIDE;
        startup.StartupInfo.hStdInput=input_read;
        startup.StartupInfo.hStdOutput=startup.StartupInfo.hStdError=output_write;
    }
    if(!GetSystemDirectoryA(system,sizeof(system)))goto done;
    snprintf(client,sizeof(client),"\"%s\" \"%s\\cmd.exe /d /q /k\" \"%s\" \"%s\" --observation-timeout-ms 30000 --observe-console-input-marker \"%s>\"",argv[1],system,argv[2],argv[3],argv[2]);
    if(explicit_host)snprintf(command,sizeof(command),"\"%s\" --headless --width 100 --height 35 -- %s",argv[4],client);
    else strcpy_s(command,sizeof(command),client);
    SetStdHandle(STD_INPUT_HANDLE,NULL);SetStdHandle(STD_OUTPUT_HANDLE,NULL);SetStdHandle(STD_ERROR_HANDLE,NULL);
    if(!CreateProcessA(explicit_host ? argv[4] : NULL,command,NULL,NULL,TRUE,
        CREATE_SUSPENDED|(explicit_host ? 0 : EXTENDED_STARTUPINFO_PRESENT),NULL,argv[2],&startup.StartupInfo,&process))goto done;
    if(!AssignProcessToJobObject(job,process.hProcess)){TerminateProcess(process.hProcess,1);goto done;}
    ResumeThread(process.hThread);
    if(explicit_host){CloseHandle(input_read);input_read=NULL;CloseHandle(output_write);output_write=NULL;}
    WriteFile(input_write,"\x1b[I",3,&written,NULL);
    printf("owned-pty observer=%lu start=%llu\n",process.dwProcessId,GetTickCount64());fflush(stdout);
    wait=WaitForSingleObject(process.hProcess,45000);
    if(wait==WAIT_OBJECT_0)GetExitCodeProcess(process.hProcess,&result);
    else result=ERROR_TIMEOUT;
done:
    if(chunks){fprintf(chunks,"host-outcome result=%lu error=%lu process=%lu explicit=%d\n",result,GetLastError(),process.dwProcessId,explicit_host);fflush(chunks);}
    printf("owned-pty exit=%lu error=%lu\n",result,GetLastError());
    if(job)CloseHandle(job);
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(pty)ClosePseudoConsole(pty);
    if(input_read)CloseHandle(input_read);if(input_write)CloseHandle(input_write);
    if(output_write)CloseHandle(output_write);
    if(thread){
        if(WaitForSingleObject(thread,10000)!=WAIT_OBJECT_0) {
            CancelSynchronousIo(thread);
            if(WaitForSingleObject(thread,10000)!=WAIT_OBJECT_0)
                return ERROR_TIMEOUT; /* Process cleanup retains live storage. */
        }
        CloseHandle(thread);
    }
    if(output_pipe)CloseHandle(output_pipe);
    if(chunks)fclose(chunks);
    CloseHandle(capture);
    return (int)result;
}
