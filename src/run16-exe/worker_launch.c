#include "worker_launch.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

/* Extracted from run16 main's verified VDM startup. The temporary job covers
 * only the otherwise orphanable create-before-Prepare interval. DOS and native
 * execution policy, record registration and Resume remain with their callers. */
DWORD run16_worker_prepare(uint64_t reservation,PCWSTR image,PWSTR command,
    void *environment,DWORD flags,const STARTUPINFOW *options,PROCESS_INFORMATION *worker)
{
    HANDLE job=NULL;STARTUPINFOEXW startup={0};SIZE_T bytes=0;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};
    BOOL initialized=FALSE;DWORD error=0;
    if(!worker)return ERROR_INVALID_PARAMETER;
    ZeroMemory(worker,sizeof(*worker));
    if(!reservation || !image || !*image || !command || !options ||
        options->cb!=sizeof(*options))return ERROR_INVALID_PARAMETER;
    job=CreateJobObjectW(NULL,NULL);
    if(!job)return GetLastError();
    limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)))
        {error=GetLastError();goto done;}
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!startup.lpAttributeList){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&bytes))
        {error=GetLastError();goto done;}
    initialized=TRUE;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_JOB_LIST,
        &job,sizeof(job),NULL,NULL)){error=GetLastError();goto done;}
    startup.StartupInfo=*options;startup.StartupInfo.cb=sizeof(startup);
    /* Command submission, not resident-worker creation, owns target streams.
     * A retained pipe writer here would prevent completion from delivering EOF. */
    startup.StartupInfo.dwFlags&=~STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput=NULL;startup.StartupInfo.hStdOutput=NULL;
    startup.StartupInfo.hStdError=NULL;
    if(!CreateProcessW(image,command,NULL,NULL,FALSE,
        flags|CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT,environment,NULL,
        &startup.StartupInfo,worker)){error=GetLastError();goto done;}
    error=OpenNtBaseClientPrepareWorker(reservation,worker->hProcess);
    if(error)goto done;
    limits.BasicLimitInformation.LimitFlags=0;
    if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)))
        error=GetLastError();
done:
    if(error && worker->hProcess) {
        /* Never resumed, therefore still startup rollback-owned. The caller
         * releases its reservation, including a failed post-Prepare disarm. */
        TerminateProcess(worker->hProcess,error);
        WaitForSingleObject(worker->hProcess,INFINITE);
        CloseHandle(worker->hThread);CloseHandle(worker->hProcess);
        ZeroMemory(worker,sizeof(*worker));
    }
    if(initialized)DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList)HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    CloseHandle(job);return error;
}
