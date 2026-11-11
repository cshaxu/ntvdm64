/* Launched through product Run16 so the existing native Hook receives the
 * two CREATE_SEPARATE_WOW_VDM requests. No private broker/task creation. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <share.h>

/* Read only this run's trace. Retain handles while workers are alive, validate
 * their exact package image, and never terminate them to satisfy acceptance. */
static BOOL observe_workers(const char *trace, const char *image,
    HANDLE *workers, DWORD *ids, FILE *log)
{
    ULONGLONG deadline=GetTickCount64()+5000;
    unsigned count=0;
    while(count<2 && GetTickCount64()<deadline){
        FILE *input=_fsopen(trace,"rb",_SH_DENYNO);
        char line[2048];
        if(input){
            while(count<2 && fgets(line,sizeof(line),input)){
                DWORD id,capacity=MAX_PATH;
                char actual[MAX_PATH];
                HANDLE process;
                FILETIME created,exited,kernel,user;
                if(!strstr(line," stage=idle-dispatch status=0") ||
                    sscanf_s(line,"%lu TaskRetirement",&id)!=1 ||
                    (count && id==ids[0]))continue;
                process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,id);
                if(!process || !QueryFullProcessImageNameA(process,0,actual,&capacity) ||
                    _stricmp(actual,image) || WaitForSingleObject(process,0)!=WAIT_TIMEOUT ||
                    !GetProcessTimes(process,&created,&exited,&kernel,&user)){
                    if(process)CloseHandle(process);
                    fclose(input);return FALSE;
                }
                workers[count]=process;ids[count]=id;++count;
                fprintf(log,"worker-observed pid=%lu created=%08lX%08lX image=%s\n",
                    id,created.dwHighDateTime,created.dwLowDateTime,actual);
                fflush(log);
            }
            fclose(input);
        }
        if(count<2)Sleep(10);
    }
    return count==2;
}

int main(int argc,char **argv)
{
    PROCESS_INFORMATION child[2]={{0}};
    STARTUPINFOA start={0};
    char command[1024]; FILE *log=NULL;
    DWORD status[2]={STILL_ACTIVE,STILL_ACTIVE};
    HANDLE workers[2]={0};DWORD ids[2]={0},first,worker_status;
    unsigned i; int result=1;
    if(argc!=6 || fopen_s(&log,argv[3],"wb"))return 2;
    start.cb=sizeof(start);
    for(i=0;i<2;++i){
        if(sprintf_s(command,sizeof(command),"\"%s\"",argv[1+i])<0)goto done;
        if(!CreateProcessA(argv[1+i],command,NULL,NULL,FALSE,
                CREATE_SEPARATE_WOW_VDM,NULL,NULL,&start,&child[i])){
            fprintf(log,"launch%u error=%lu\n",i,GetLastError());goto done;
        }
        fprintf(log,"launch%u returned_pid=%lu flags=CREATE_SEPARATE_WOW_VDM\n",i,child[i].dwProcessId);
        fflush(log);CloseHandle(child[i].hThread);child[i].hThread=NULL;
    }
    if(!observe_workers(argv[4],argv[5],workers,ids,log))goto done;
    /* Hook returns startup-only Run16 processes, not worker lifetime handles.
     * Both launchers must finish while independently pinned workers execute. */
    for(i=0;i<2;++i){
        if(WaitForSingleObject(child[i].hProcess,10000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child[i].hProcess,&status[i]) || status[i])goto done;
        if(WaitForSingleObject(workers[i],0)!=WAIT_TIMEOUT){
            fputs("FAIL worker ended before startup-return proof\n",log);goto done;
        }
    }
    fputs("PASS both startup-only launchers returned with workers alive\n",log);fflush(log);
    first=WaitForMultipleObjects(2,workers,FALSE,5000);
    if(first>WAIT_OBJECT_0+1)goto done;
    first-=WAIT_OBJECT_0;
    if(!GetExitCodeProcess(workers[first],&worker_status) || worker_status ||
        WaitForSingleObject(workers[1-first],0)!=WAIT_TIMEOUT)goto done;
    fputs("first-exited peer-still-running\n",log);fflush(log);
    {
        LARGE_INTEGER size;
        HANDLE trace=CreateFileA(argv[4],GENERIC_READ,
            FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
        if(trace==INVALID_HANDLE_VALUE)goto done;
        if(!GetFileSizeEx(trace,&size)){CloseHandle(trace);goto done;}
        CloseHandle(trace);
        fprintf(log,"worker-exited pid=%lu exit=0 peer=%lu trace_bytes=%lld\n",
            ids[first],ids[1-first],size.QuadPart);fflush(log);
    }
    if(WaitForSingleObject(workers[1-first],5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(workers[1-first],&worker_status) || worker_status)goto done;
    fprintf(log,"worker-exited pid=%lu exit=0 final=1\n",ids[1-first]);
    fputs("PASS both independent requests completed\n",log);result=0;
done:
    fprintf(log,"result=%d exit0=%lu exit1=%lu\n",result,status[0],status[1]);
    fclose(log);
    for(i=0;i<2;++i){if(child[i].hThread)CloseHandle(child[i].hThread);if(child[i].hProcess)CloseHandle(child[i].hProcess);}
    for(i=0;i<2;++i)if(workers[i])CloseHandle(workers[i]);
    /* Exact-package supervisor owns cleanup on failure; never force a child
     * to exit and count that as ordinary completion. */
    return result;
}
