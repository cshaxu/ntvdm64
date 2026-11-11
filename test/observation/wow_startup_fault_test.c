/* Real new-worker startup faults, on an unswitched private desktop.
 * DEBUG_PROCESS supplies a deterministic pre-InitTask DLL-load boundary;
 * no production fault hook, guest change or timing-only kill. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

static BOOL CALLBACK dismiss_load_error(HWND window,LPARAM owner)
{
    DWORD pid=0; WCHAR name[64],title[256];
    GetWindowThreadProcessId(window,&pid);
    if(pid==(DWORD)owner && IsWindowVisible(window) &&
        GetClassNameW(window,name,64) && !wcscmp(name,L"#32770")) {
        GetWindowTextW(window,title,256);
        wprintf(L"LOAD-ERROR dialog pid=%lu title=%ls\n",pid,title);
        PostMessageW(window,WM_COMMAND,IDOK,0);
    }
    return TRUE;
}

static void cleanup(HANDLE process)
{
    if (!process) return;
    if (WaitForSingleObject(process,0)==WAIT_TIMEOUT) {
        TerminateProcess(process,97); WaitForSingleObject(process,5000);
    }
    CloseHandle(process);
}

int wmain(int argc,WCHAR **argv)
{
    WCHAR desktop_name[80],path[MAX_PATH],command[1024],actual[MAX_PATH];
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION broker={0},launcher={0};
    HDESK desktop=NULL;
    HANDLE worker=NULL,image_lock=INVALID_HANDLE_VALUE;
    DWORD worker_pid=0,expected,exit_code=STILL_ACTIVE,worker_code=STILL_ACTIVE;
    DWORD worker_wait=WAIT_FAILED;
    BOOL broker_fault,loader_fault,injected=FALSE,finished=FALSE,worker_finished=FALSE;
    ULONGLONG deadline;
    int result=1;
    if(argc!=4 || (wcscmp(argv[3],L"worker") && wcscmp(argv[3],L"broker") &&
        wcscmp(argv[3],L"loader"))) return 87;
    broker_fault=!wcscmp(argv[3],L"broker");
    loader_fault=!wcscmp(argv[3],L"loader");
    expected=loader_fault ? ERROR_DLL_INIT_FAILED :
        (broker_fault ? RPC_S_SERVER_UNAVAILABLE : ERROR_PROCESS_ABORTED);
    swprintf_s(desktop_name,80,L"NTVDMStartupFault-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(desktop_name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop || !SetEnvironmentVariableW(L"SystemRoot",argv[1])) goto done;
    startup.lpDesktop=desktop_name;
    swprintf_s(path,MAX_PATH,L"%ls\\ntsrv.exe",argv[1]);
    swprintf_s(command,1024,L"\"%ls\"",path);
    if(!CreateProcessW(path,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,argv[1],&startup,&broker)) goto done;
    swprintf_s(path,MAX_PATH,L"%ls\\run16.exe",argv[1]);
    swprintf_s(command,1024,L"\"%ls\" WINMINE.EXE",path);
    if(!CreateProcessW(path,command,NULL,NULL,FALSE,CREATE_NO_WINDOW|DEBUG_PROCESS,
        NULL,argv[1],&startup,&launcher)) goto done;
    /* Cleanup is explicit and scoped to handles this fixture owns. */
    if(!DebugSetProcessKillOnExit(FALSE)) goto done;
    /* Loader mode must finish original guest/WOWExec initialization after
     * injection; process-loss modes stop at the earlier DLL-load boundary. */
    deadline=GetTickCount64()+(loader_fault ? 60000u : 30000u);
    while(!(finished && (loader_fault || worker_finished)) && GetTickCount64()<deadline) {
        DEBUG_EVENT event;
        DWORD disposition=DBG_CONTINUE;
        if(!WaitForDebugEvent(&event,200)) {
            if(GetLastError()==ERROR_SEM_TIMEOUT) {
                if(loader_fault && injected) EnumDesktopWindows(desktop,dismiss_load_error,(LPARAM)worker_pid);
                continue;
            }
            goto done;
        }
        if(event.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT) {
            DWORD count=MAX_PATH;
            if(event.dwProcessId!=launcher.dwProcessId &&
                QueryFullProcessImageNameW(event.u.CreateProcessInfo.hProcess,0,actual,&count)) {
                swprintf_s(path,MAX_PATH,L"%ls\\ntvdm.exe",argv[2]);
                if(!_wcsicmp(path,actual)) {
                    worker_pid=event.dwProcessId;
                    worker=OpenProcess(PROCESS_TERMINATE|PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,
                        FALSE,worker_pid);
                }
            }
            if(event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
            CloseHandle(event.u.CreateProcessInfo.hThread);
            CloseHandle(event.u.CreateProcessInfo.hProcess);
        } else if(event.dwDebugEventCode==CREATE_THREAD_DEBUG_EVENT) {
            CloseHandle(event.u.CreateThread.hThread);
        } else if(event.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT) {
            HANDLE file=event.u.LoadDll.hFile;
            if(file && event.dwProcessId==worker_pid && !injected) {
                DWORD count=GetFinalPathNameByHandleW(file,actual,MAX_PATH,FILE_NAME_NORMALIZED);
                WCHAR *name=count && count<MAX_PATH ? wcsrchr(actual,L'\\') : NULL;
                if(name && !_wcsicmp(name+1,L"wow32.dll")) {
                    if(loader_fault) {
                        swprintf_s(path,MAX_PATH,L"%ls\\WINMINE.EXE",argv[1]);
                        image_lock=CreateFileW(path,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
                        injected=image_lock!=INVALID_HANDLE_VALUE;
                    } else injected=worker && TerminateProcess(broker_fault ? broker.hProcess : worker,97);
                    printf("INJECT %s at new worker WOW32 load pid=%lu success=%u\n",
                        loader_fault ? "loader-file-lock" : (broker_fault ? "broker" : "worker"),worker_pid,injected);
                }
            }
            if(file) CloseHandle(file);
        } else if(event.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {
            if(event.u.Exception.ExceptionRecord.ExceptionCode!=EXCEPTION_BREAKPOINT)
                disposition=DBG_EXCEPTION_NOT_HANDLED;
        } else if(event.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT) {
            if(event.dwProcessId==launcher.dwProcessId) {
                exit_code=event.u.ExitProcess.dwExitCode; finished=TRUE;
            } else if(event.dwProcessId==worker_pid) {
                worker_code=event.u.ExitProcess.dwExitCode; worker_finished=TRUE;
            }
        }
        if(!ContinueDebugEvent(event.dwProcessId,event.dwThreadId,disposition)) goto done;
    }
    printf("NEW-WOW-STARTUP injected=%u finished=%u exit=%lu expected=%lu\n",
        injected,finished,exit_code,expected);
    printf("NEW-WOW-WORKER finished=%u exit=%lu\n",worker_finished,worker_code);
    if(worker) {
        worker_wait=WaitForSingleObject(worker,0);
        printf("NEW-WOW-WORKER wait=%lu query=%u ",worker_wait,GetExitCodeProcess(worker,&worker_code));
        printf("code=%lu\n",worker_code);
    }
    if(injected && finished && exit_code==expected &&
        (loader_fault ? (!worker_finished && worker_wait==WAIT_TIMEOUT) :
        (worker_finished && worker_code==(broker_fault ? (DWORD)RPC_S_SERVER_UNAVAILABLE : 97u)))) {
        if(loader_fault) {
            /* Release only our read lock, then prove the failed task did not
             * corrupt the original shared worker's next-command lifecycle. */
            CloseHandle(image_lock); image_lock=INVALID_HANDLE_VALUE;
            if(!DebugActiveProcessStop(worker_pid)) goto done;
            CloseHandle(launcher.hProcess); CloseHandle(launcher.hThread);
            ZeroMemory(&launcher,sizeof(launcher));
            swprintf_s(path,MAX_PATH,L"%ls\\run16.exe",argv[1]);
            swprintf_s(command,1024,L"\"%ls\" WINMINE.EXE",path);
            if(!CreateProcessW(path,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                NULL,argv[1],&startup,&launcher) ||
                WaitForSingleObject(launcher.hProcess,20000)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(launcher.hProcess,&exit_code) || exit_code ||
                WaitForSingleObject(worker,0)!=WAIT_TIMEOUT) goto done;
            puts("WOW-LOADER-RECOVERY startup=0 original-worker-alive");
        }
        puts("NEW-WOW-STARTUP-FAULT-PASS"); result=0;
    }
done:
    if(image_lock!=INVALID_HANDLE_VALUE) CloseHandle(image_lock);
    if(worker_pid) DebugActiveProcessStop(worker_pid);
    if(launcher.dwProcessId) DebugActiveProcessStop(launcher.dwProcessId);
    cleanup(worker); cleanup(launcher.hProcess); cleanup(broker.hProcess);
    if(launcher.hThread) CloseHandle(launcher.hThread);
    if(broker.hThread) CloseHandle(broker.hThread);
    if(desktop) CloseDesktop(desktop);
    if(result) fprintf(stderr,"NEW-WOW-STARTUP-FAULT-FAIL error=%lu\n",GetLastError());
    return result;
}
