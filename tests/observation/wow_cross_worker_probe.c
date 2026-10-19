/* Independent native coordinator. Run16 installs the real native Hook;
 * CreateProcess's existing separate-WOW flag selects two real workers.
 * No private task creation, broker fixture, forced exit or helper in product. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct worker_record {
    HWND window;
    HANDLE process;
    DWORD pid, thread;
} worker_record;

static BOOL pin_worker(const char *cls, const char *image,
    worker_record *worker, FILE *log, const char *role)
{
    ULONGLONG deadline=GetTickCount64()+10000;
    while(GetTickCount64()<deadline){
        DWORD pid=0,tid,capacity=MAX_PATH,again=0;
        char actual[MAX_PATH]; FILETIME created,exited,kernel,user;
        HWND window=FindWindowA(cls,NULL);
        HANDLE process;
        if(!window){Sleep(10);continue;}
        tid=GetWindowThreadProcessId(window,&pid);
        process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
        if(!process)return FALSE;
        if(!tid || !QueryFullProcessImageNameA(process,0,actual,&capacity) ||
           _stricmp(actual,image) || WaitForSingleObject(process,0)!=WAIT_TIMEOUT ||
           !GetProcessTimes(process,&created,&exited,&kernel,&user) ||
           GetWindowThreadProcessId(window,&again)!=tid || again!=pid){
            CloseHandle(process);return FALSE;
        }
        worker->window=window;worker->process=process;worker->pid=pid;worker->thread=tid;
        fprintf(log,"worker role=%s pid=%lu tid=%lu hwnd=%08lX created=%08lX%08lX image=%s\n",
            role,pid,tid,(unsigned long)(ULONG_PTR)window,
            created.dwHighDateTime,created.dwLowDateTime,actual);
        fflush(log);return TRUE;
    }
    fprintf(log,"FAIL no-window role=%s\n",role);fflush(log);return FALSE;
}

int main(int argc,char **argv)
{
    STARTUPINFOA startup={0};PROCESS_INFORMATION children[2]={{0}};
    worker_record workers[2]={{0}};
    const char *roles[2]={"receiver","sender"};
    const char *classes[2]={"T447_MSG_RECV","T447_MSG_SEND"};
    FILE *log=NULL;char command[1024];DWORD flags,code=STILL_ACTIVE;
    DWORD_PTR result=0;unsigned i;int status=1;
    if(argc!=6 || fopen_s(&log,argv[3],"wb"))return 2;
    startup.cb=sizeof(startup);
    flags=!strcmp(argv[5],"1") ? 0 : CREATE_SEPARATE_WOW_VDM;
    for(i=0;i<2;++i){
        const char *application=argv[2-i];
        if(sprintf_s(command,sizeof(command),"\"%s\"",application)<0)goto done;
        if(!CreateProcessA(application,command,NULL,NULL,FALSE,flags,
                NULL,NULL,&startup,&children[i])){
            fprintf(log,"FAIL launch role=%s error=%lu\n",roles[i],GetLastError());goto done;
        }
        fprintf(log,"launch role=%s returned_pid=%lu flags=%08lX\n",roles[i],children[i].dwProcessId,flags);
        fflush(log);CloseHandle(children[i].hThread);children[i].hThread=NULL;
        if(!pin_worker(classes[i],argv[4],&workers[i],log,roles[i]))goto done;
    }
    if(workers[0].pid==workers[1].pid){
        fputs("FAIL worker-not-distinct\n",log);goto done;
    }
    if(!SendMessageTimeoutA(workers[1].window,0x479,0x5341,0x574f5732,
            SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT,5000,&result) || result!=0x447a0079){
        fprintf(log,"FAIL sender-ready error=%lu result=%llX\n",GetLastError(),(unsigned long long)result);goto done;
    }
    fputs("sender-ready distinct-workers=1\n",log);fflush(log);
    for(i=0;i<2;++i){
        if(WaitForSingleObject(children[i].hProcess,15000)!=WAIT_OBJECT_0 ||
           !GetExitCodeProcess(children[i].hProcess,&code) || code){
            fprintf(log,"FAIL child-exit role=%s exit=%lu\n",roles[i],code);goto done;
        }
        if(WaitForSingleObject(workers[i].process,5000)!=WAIT_OBJECT_0 ||
           !GetExitCodeProcess(workers[i].process,&code) || code || IsWindow(workers[i].window)){
            fprintf(log,"FAIL worker-exit role=%s exit=%lu\n",roles[i],code);goto done;
        }
        fprintf(log,"worker-exited role=%s pid=%lu exit=0 windowAlive=0\n",roles[i],workers[i].pid);
        fflush(log);
    }
    fputs("PASS cross-worker normal process retirement\n",log);status=0;
done:
    fprintf(log,"result=%d\n",status);fclose(log);
    for(i=0;i<2;++i){
        if(children[i].hThread)CloseHandle(children[i].hThread);
        if(children[i].hProcess)CloseHandle(children[i].hProcess);
        if(workers[i].process)CloseHandle(workers[i].process);
    }
    return status;
}
