/* Run through the private-desktop observer, from the deployed package root.
 * No pointer, focus or window activation. Marker creation after launcher death
 * was the old survival probe. Root session closure now must stop the waiting
 * guest, reap its worker and fail the unfinished nested task explicitly. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
static const WCHAR *markers[]={L"logs\\NIOREADY",L"logs\\NIOGO",L"logs\\NIODONE",L"logs\\NIOPID",L"logs\\NIOROOT"};
static HANDLE find_worker(DWORD parent)
{
    PROCESSENTRY32W entry={sizeof(entry)};
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0),worker=NULL;
    if(snapshot==INVALID_HANDLE_VALUE) return NULL;
    if(Process32FirstW(snapshot,&entry)) do {
        if(entry.th32ParentProcessID==parent && !_wcsicmp(entry.szExeFile,L"ntvdm.exe")) {
            worker=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
            break;
        }
    } while(Process32NextW(snapshot,&entry));
    CloseHandle(snapshot);return worker;
}
int main(int argc,char **argv)
{
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    WCHAR command[1024]=L"run16.exe tests\\NOIO.COM",image[MAX_PATH];
    HANDLE file,nested=NULL,worker=NULL;
    DWORD i,code=1,bytes,pid=0;
    BOOL normal=argc==2 && !strcmp(argv[1],"--normal");
    BOOL spawn=argc==2 && !strcmp(argv[1],"--spawn");
    const char *phase="guest-ready";
    ULONGLONG deadline;
    if(argc!=1 && !normal && !spawn) {
        for(i=0;i<(DWORD)argc;++i) fprintf(stderr,"argument[%lu]=%s\n",i,argv[i]);
        return 64;
    }
    for(i=0;i<5;++i) if(GetFileAttributesW(markers[i])!=INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr,"Existing marker; refusing test\n");return 2;
    }
    if(normal) {
        if(!GetModuleFileNameW(NULL,image,MAX_PATH) ||
           swprintf_s(command,1024,L"run16.exe \"%s\" --spawn",image)<0) return 4;
    }
    /* The nested launcher must inherit the authenticated frontend capability,
     * just as CMD does. The external controller has no such capability. */
    if(!CreateProcessW(L"run16.exe",command,NULL,NULL,spawn,0,NULL,NULL,&startup,&child))
        return 3;
    deadline=GetTickCount64()+15000;
    while(GetFileAttributesW(markers[0])==INVALID_FILE_ATTRIBUTES &&
          WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT && GetTickCount64()<deadline)
        Sleep(20);
    if(GetFileAttributesW(markers[0])==INVALID_FILE_ATTRIBUTES) goto cleanup;
    phase="root-completion";
    if(spawn) {
        file=CreateFileW(markers[3],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        pid=child.dwProcessId;
        if(WriteFile(file,&pid,sizeof(pid),&bytes,NULL) && bytes==sizeof(pid)) code=37;
        CloseHandle(file);
        deadline=GetTickCount64()+15000;
        while(GetFileAttributesW(markers[4])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline) Sleep(20);
        if(GetFileAttributesW(markers[4])==INVALID_FILE_ATTRIBUTES) code=1;
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        return (int)code; /* Deliberately leave the successfully handed-off child running. */
    }
    phase="nested-identity";
    if(normal) {
        deadline=GetTickCount64()+5000;
        while(GetFileAttributesW(markers[3])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline) Sleep(20);
        do {
            file=CreateFileW(markers[3],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
            if(file!=INVALID_HANDLE_VALUE || GetLastError()!=ERROR_SHARING_VIOLATION) break;
            Sleep(20); /* Wait for the writer to close the complete PID record. */
        } while(GetTickCount64()<deadline);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        if(!ReadFile(file,&pid,sizeof(pid),&bytes,NULL) || bytes!=sizeof(pid)) {CloseHandle(file);goto cleanup;}
        CloseHandle(file);
        nested=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
        if(!nested || WaitForSingleObject(nested,0)!=WAIT_TIMEOUT) goto cleanup;
    }
    worker=find_worker(normal ? pid : child.dwProcessId);
    if(!worker) goto cleanup;
    phase="root-close";
    if(normal) {
        file=CreateFileW(markers[4],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        CloseHandle(file);
    } else if(!TerminateProcess(child.hProcess,91)) goto cleanup;
    if(WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child.hProcess,&code) || code!=(normal ? 37u : 91u)) {code=1;goto cleanup;}
    code=1;phase="worker-close";
    if(WaitForSingleObject(worker,8000)!=WAIT_OBJECT_0) goto cleanup;
    phase="nested-result";
    if(normal && (WaitForSingleObject(nested,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(nested,&code) || code!=ERROR_PROCESS_ABORTED)) {code=1;goto cleanup;}
    if(GetFileAttributesW(markers[2])!=INVALID_FILE_ATTRIBUTES) {code=1;goto cleanup;}
    puts(normal ? "PASS root result 37 retained; associated worker closes; unfinished inner task fails 1067" :
        "PASS abnormal root closes associated worker without requiring guest Console I/O");
    code=0;
cleanup:
    if(code) {
        DWORD child_code=0;
        GetExitCodeProcess(child.hProcess,&child_code);
        fprintf(stderr,"FAIL phase=%s spawn=%d normal=%d child=%lu error=%lu\n",
            phase,spawn,normal,child_code,GetLastError());
    }
    if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);
    }
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    if(nested) CloseHandle(nested);
    if(worker) CloseHandle(worker);
    /* Controller retains markers until external test-owned worker cleanup,
     * so a late guest cannot be accidentally released by the next test. */
    return (int)code;
}
