/* Real run16/native completion and launcher-loss test. Run only through the
 * unswitched private-desktop observer. Build as CUI and GUI to cover both
 * classification paths; events acknowledge work after the injected death. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line=%u error=%lu\n", \
    (unsigned)__LINE__,GetLastError()); ExitProcess(1); } } while (0)

int wmain(int argc,WCHAR **argv)
{
    WCHAR image[MAX_PATH],root[MAX_PATH],launcher[MAX_PATH],command[2048];
    WCHAR ready_name[96],release_name[96],pid_name[96];
    HANDLE ready,release,mapping,target;
    DWORD *target_pid,code,round;
    WCHAR report_path[MAX_PATH];
    FILE *report;
    if (argc==5 && !wcscmp(argv[1],L"--descendant")) {
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION child={0};
        CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
        CHECK(swprintf_s(command,2048,L"\"%s\" --target %s %s %s",
            image,argv[2],argv[3],argv[4])>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child));
        CloseHandle(child.hThread);CloseHandle(child.hProcess);return 37;
    }
    if (argc==5 && !wcscmp(argv[1],L"--target")) {
        ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[2]);
        release=OpenEventW(SYNCHRONIZE,FALSE,argv[3]);
        mapping=OpenFileMappingW(FILE_MAP_WRITE,FALSE,argv[4]);
        CHECK(ready && release && mapping);
        target_pid=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(DWORD));
        CHECK(target_pid);
        *target_pid=GetCurrentProcessId();CHECK(SetEvent(ready));
        CHECK(WaitForSingleObject(release,15000)==WAIT_OBJECT_0);
        /* A completion after the controller releases us proves actual work
         * after launcher death, not just a stale process-table observation. */
        UnmapViewOfFile(target_pid);CloseHandle(mapping);
        CloseHandle(ready);CloseHandle(release);return 37;
    }
    CHECK(argc==1);
    CHECK(GetEnvironmentVariableW(L"UNPAIRED_NATIVE_REPORT",report_path,MAX_PATH)>0);
    CHECK(!_wfopen_s(&report,report_path,L"w"));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    CHECK(GetEnvironmentVariableW(L"TEST_RUNTIME_ROOT",root,MAX_PATH)>0);
    CHECK(swprintf_s(launcher,MAX_PATH,L"%s\\run16.exe",root)>0);
    for (round=0;round<3;++round) {
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION child={0};
        CHECK(swprintf_s(ready_name,96,L"Local\\UnpairedReady-%lu-%lu",GetCurrentProcessId(),round)>0);
        CHECK(swprintf_s(release_name,96,L"Local\\UnpairedRelease-%lu-%lu",GetCurrentProcessId(),round)>0);
        CHECK(swprintf_s(pid_name,96,L"Local\\UnpairedPid-%lu-%lu",GetCurrentProcessId(),round)>0);
        ready=CreateEventW(NULL,TRUE,FALSE,ready_name);
        release=CreateEventW(NULL,TRUE,FALSE,release_name);
        mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(DWORD),pid_name);
        CHECK(ready && release && mapping);
        target_pid=MapViewOfFile(mapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,sizeof(DWORD));
        CHECK(target_pid);*target_pid=0;
        CHECK(swprintf_s(command,2048,L"\"%s\" \"%s\" %s %s %s %s",
            launcher,image,round==2 ? L"--descendant" : L"--target",ready_name,release_name,pid_name)>0);
        CHECK(CreateProcessW(launcher,command,NULL,NULL,FALSE,0,NULL,root,&startup,&child));
        CloseHandle(child.hThread);
        CHECK(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0 && *target_pid);
        target=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,*target_pid);
        CHECK(target);
        if (round!=2) CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
        if (round==1) {
            CHECK(TerminateProcess(child.hProcess,91));
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeProcess(child.hProcess,&code) && code==91);
            CHECK(WaitForSingleObject(target,500)==WAIT_TIMEOUT);
        }
        if (round==2) {
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeProcess(child.hProcess,&code) && code==37);
            CHECK(WaitForSingleObject(target,500)==WAIT_TIMEOUT);
        }
        CHECK(SetEvent(release));
        CHECK(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(target,&code) && code==37);
        if (!round) {
            CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
            CHECK(GetExitCodeProcess(child.hProcess,&code) && code==37);
        }
        fprintf(stdout,"PASS round=%lu native target=37 launcher=%lu\n",round,round==1 ? 91UL : 37UL);
        fprintf(report,"PASS round=%lu native target=37 launcher=%lu\n",round,round==1 ? 91UL : 37UL);
        CHECK(!fflush(report));
        CloseHandle(target);CloseHandle(child.hProcess);
        UnmapViewOfFile(target_pid);CloseHandle(mapping);CloseHandle(ready);CloseHandle(release);
    }
    CHECK(!fclose(report));return 0;
}
