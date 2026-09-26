/* Run through the private-desktop observer, from the deployed package root.
 * No pointer, focus or window activation. Marker creation after launcher death
 * proves actual DOS work, not just a live worker PID. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
static const WCHAR *markers[]={L"logs\\NIOREADY",L"logs\\NIOGO",L"logs\\NIODONE",L"logs\\NIOPID"};
int main(int argc,char **argv)
{
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    WCHAR command[1024]=L"run16.exe tests\\NOIO.COM",image[MAX_PATH];
    HANDLE file,nested=NULL;
    DWORD i,code=1,bytes,pid;
    BOOL normal=argc==2 && !strcmp(argv[1],"--normal");
    BOOL spawn=argc==2 && !strcmp(argv[1],"--spawn");
    const char *phase="guest-ready";
    ULONGLONG deadline;
    if(argc!=1 && !normal && !spawn) return 64;
    for(i=0;i<4;++i) if(GetFileAttributesW(markers[i])!=INVALID_FILE_ATTRIBUTES) {
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
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        return (int)code; /* Deliberately leave the successfully handed-off child running. */
    }
    if((!normal && !TerminateProcess(child.hProcess,91)) || WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0)
        goto cleanup;
    if(!GetExitCodeProcess(child.hProcess,&code) || code!=(normal ? 37u : 91u)) {code=1;goto cleanup;}
    code=1;
    phase="nested-identity";
    if(normal) {
        file=CreateFileW(markers[3],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        if(!ReadFile(file,&pid,sizeof(pid),&bytes,NULL) || bytes!=sizeof(pid)) {CloseHandle(file);goto cleanup;}
        CloseHandle(file);
        nested=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
        if(!nested || WaitForSingleObject(nested,0)!=WAIT_TIMEOUT) goto cleanup;
    }
    /* Root has actually terminated before guest is allowed to create DONE. */
    phase="guest-release";
    file=CreateFileW(markers[1],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
    if(file==INVALID_HANDLE_VALUE) goto cleanup;
    CloseHandle(file);
    deadline=GetTickCount64()+10000;
    while(GetFileAttributesW(markers[2])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline)
        Sleep(20);
    if(GetFileAttributesW(markers[2])!=INVALID_FILE_ATTRIBUTES) {
        phase="nested-result";
        if(normal && (WaitForSingleObject(nested,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(nested,&code) || code!=7)) {code=1;goto cleanup;}
        puts(normal ? "PASS root returns 37 normally; descendant DOS creates DONE and inner run16 returns 7" :
            "PASS DOS creates DONE after root run16 exited; no Console I/O requested by test guest");
        code=0;
    } else puts("FAIL DOS made no post-root-loss file progress");
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
    /* Controller retains markers until external test-owned worker cleanup,
     * so a late guest cannot be accidentally released by the next test. */
    return (int)code;
}
