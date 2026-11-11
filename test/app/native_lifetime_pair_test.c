/* Real run16 lifetime-pair test. The GUI-subsystem target has no windows and
 * deliberately leaves its own descendant alive to detect recursive cleanup. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PAIR_STATE { DWORD target,descendant; } PAIR_STATE;

static int target(const char *name)
{
    char eventName[128],image[MAX_PATH],command[MAX_PATH+160];
    HANDLE mapping=OpenFileMappingA(FILE_MAP_ALL_ACCESS,FALSE,name),ready,finish;
    PAIR_STATE *state;
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    if (!mapping) return 10;
    state=MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*state));
    if (!state) return 11;
    sprintf_s(eventName,sizeof(eventName),"%s-ready",name);
    ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,eventName);
    sprintf_s(eventName,sizeof(eventName),"%s-finish",name);
    finish=OpenEventA(SYNCHRONIZE,FALSE,eventName);
    if (!ready || !finish || !GetModuleFileNameA(NULL,image,sizeof(image))) return 12;
    sprintf_s(command,sizeof(command),"\"%s\" --descendant",image);
    if (!CreateProcessA(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child)) return 13;
    state->target=GetCurrentProcessId();state->descendant=child.dwProcessId;
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    if (!SetEvent(ready) || WaitForSingleObject(finish,15000)!=WAIT_OBJECT_0) return 14;
    CloseHandle(ready);CloseHandle(finish);UnmapViewOfFile(state);CloseHandle(mapping);
    return 42;
}

static int run_case(const char *launcher,const char *image,int mode)
{
    char name[96],eventName[128],command[2*MAX_PATH+180];
    HANDLE mapping=NULL,ready=NULL,finish=NULL,targetProcess=NULL,descendant=NULL;
    PAIR_STATE *state=NULL;
    STARTUPINFOA startup={sizeof(startup)};
    PROCESS_INFORMATION run={0};
    DWORD code=0;
    int result=1;
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"case=%d line=%d error=%lu\n",mode,__LINE__,GetLastError());goto done; } } while (0)
    sprintf_s(name,sizeof(name),"Local\\ntvdm-pair-%lu-%d",GetCurrentProcessId(),mode);
    mapping=CreateFileMappingA(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(*state),name);
    REQUIRE(mapping && GetLastError()!=ERROR_ALREADY_EXISTS);
    state=MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*state));REQUIRE(state);
    ZeroMemory(state,sizeof(*state));
    sprintf_s(eventName,sizeof(eventName),"%s-ready",name);
    ready=CreateEventA(NULL,TRUE,FALSE,eventName);REQUIRE(ready);
    sprintf_s(eventName,sizeof(eventName),"%s-finish",name);
    finish=CreateEventA(NULL,TRUE,FALSE,eventName);REQUIRE(finish);
    sprintf_s(command,sizeof(command),"\"%s\" \"%s\" --target %s",launcher,image,name);
    REQUIRE(CreateProcessA(launcher,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&run));
    REQUIRE(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0);
    targetProcess=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,state->target);
    descendant=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,state->descendant);
    REQUIRE(targetProcess && descendant);
    if (mode==0) {
        REQUIRE(TerminateProcess(run.hProcess,91));
        REQUIRE(WaitForSingleObject(run.hProcess,5000)==WAIT_OBJECT_0);
        REQUIRE(WaitForSingleObject(targetProcess,5000)==WAIT_OBJECT_0);
    } else {
        if (mode==1) REQUIRE(TerminateProcess(targetProcess,73));
        else REQUIRE(SetEvent(finish));
        REQUIRE(WaitForSingleObject(run.hProcess,5000)==WAIT_OBJECT_0);
        REQUIRE(GetExitCodeProcess(run.hProcess,&code) && code==(mode==1 ? 73u : 42u));
    }
    REQUIRE(WaitForSingleObject(descendant,300)==WAIT_TIMEOUT);
    printf("PASS pair case=%d launcher/target complete; descendant survives; exit=%lu\n",mode,code);
    result=0;
done:
    /* Only exact handles/PIDs published by this test-owned target are used. */
    if (!targetProcess && state && state->target)
        targetProcess=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,state->target);
    if (!descendant && state && state->descendant)
        descendant=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,FALSE,state->descendant);
    if (run.hProcess) { TerminateProcess(run.hProcess,99);WaitForSingleObject(run.hProcess,5000);CloseHandle(run.hProcess); }
    if (run.hThread) CloseHandle(run.hThread);
    if (targetProcess) { TerminateProcess(targetProcess,99);WaitForSingleObject(targetProcess,5000);CloseHandle(targetProcess); }
    if (descendant) { TerminateProcess(descendant,99);WaitForSingleObject(descendant,5000);CloseHandle(descendant); }
    if (state) UnmapViewOfFile(state);
    if (mapping) CloseHandle(mapping);
    if (ready) CloseHandle(ready);
    if (finish) CloseHandle(finish);
    return result;
#undef REQUIRE
}

int main(int argc,char **argv)
{
    int mode;
    if (argc==2 && !strcmp(argv[1],"--descendant")) { Sleep(30000);return 0; }
    if (argc==3 && !strcmp(argv[1],"--target")) return target(argv[2]);
    if (argc!=3) return 64;
    for (mode=0;mode<3;++mode) if (run_case(argv[1],argv[2],mode)) return 1;
    return 0;
}
