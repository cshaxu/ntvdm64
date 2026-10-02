#include "worker_spawn.h"

/* Project-added startup transaction recovered from run16_worker_prepare.
 * The transient Job protects only unaccepted startup, not target lifetime.
 * Neither Job observation nor process-tree management is performed. */
DWORD broker_worker_start_admitted(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint64_t reservation,PCWSTR image,PWSTR command,void *environment,DWORD flags,
    const STARTUPINFOW *options,broker_worker_admit admit,void *context,HANDLE *worker)
{
    HANDLE job=NULL;STARTUPINFOEXW startup={0};PROCESS_INFORMATION process={0};SIZE_T bytes=0;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits={0};
    BOOL initialized=FALSE;DWORD error=0;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if(!connection || !reservation || !image || !*image || !command || !options ||
        options->cb!=sizeof(*options))return ERROR_INVALID_PARAMETER;
    job=CreateJobObjectW(NULL,NULL);if(!job)return GetLastError();
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
    /* Command records, not a resident worker, own inherited target streams. */
    startup.StartupInfo.dwFlags&=~STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput=NULL;startup.StartupInfo.hStdOutput=NULL;
    startup.StartupInfo.hStdError=NULL;
    if(!CreateProcessW(image,command,NULL,NULL,FALSE,
        flags|CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT,environment,NULL,
        &startup.StartupInfo,&process)){error=GetLastError();goto done;}
    error=OpenNtBaseServicePrepareWorker(connection,pid,generation,reservation,process.hProcess);
    if(error)goto done;
    /* Original DOS/WOW Update and frontend admission must precede Resume.
     * This callback is service-private, never a remotely supplied operation. */
    if(admit && (error=admit(context,process.hProcess)))goto done;
    /* Finish all fallible handle preparation while the worker is suspended.
     * After Resume it may immediately consume the already-published DOS record;
     * no later export failure may turn that running task into startup rollback. */
    if(!DuplicateHandle(GetCurrentProcess(),process.hProcess,GetCurrentProcess(),worker,
        PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0))
        {error=GetLastError();goto done;}
    limits.BasicLimitInformation.LimitFlags=0;
    if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)))
        {error=GetLastError();goto done;}
    if(ResumeThread(process.hThread)==(DWORD)-1){error=GetLastError();goto done;}
done:
    if(error && process.hProcess) {
        /* No task delivery has succeeded. Never kill a handed-off worker. */
        TerminateProcess(process.hProcess,error);WaitForSingleObject(process.hProcess,INFINITE);
    }
    if(error && *worker){CloseHandle(*worker);*worker=NULL;}
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(initialized)DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList)HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    CloseHandle(job);return error;
}
DWORD broker_worker_start(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint64_t reservation,PCWSTR image,PWSTR command,void *environment,DWORD flags,
    const STARTUPINFOW *options,HANDLE *worker)
{
    return broker_worker_start_admitted(connection,pid,generation,reservation,image,
        command,environment,flags,options,NULL,NULL,worker);
}
