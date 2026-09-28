/* Compile this source as both a Console supervisor and a windowless GUI
 * target. Exercise the real run16 entry without touching the owner desktop. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#pragma comment(lib,"user32.lib")
static wchar_t test_desktop[80];
static BOOL contains_text(const wchar_t *report,const wchar_t *suffix,const char *marker)
{
    wchar_t path[MAX_PATH]; char data[65536]; FILE *file=NULL; size_t count;
    if(swprintf_s(path,MAX_PATH,L"%ls%ls",report,suffix)<0 ||
        _wfopen_s(&file,path,L"rb") || !file) return FALSE;
    count=fread(data,1,sizeof(data)-1,file); data[count]=0; fclose(file);
    return strstr(data,marker)!=NULL;
}
static BOOL CALLBACK describe_window(HWND window,LPARAM ignored)
{
    DWORD pid=0; wchar_t name[128]={0},title[256]={0};
    (void)ignored;
    GetWindowThreadProcessId(window,&pid);
    GetClassNameW(window,name,_countof(name)); GetWindowTextW(window,title,_countof(title));
    wprintf(L"CONTROL-WINDOW pid=%lu visible=%u class=%ls title=%ls\n",pid,IsWindowVisible(window),name,title);
    return TRUE;
}
static BOOL CALLBACK dismiss_shell_error(HWND window,LPARAM owner)
{
    DWORD pid=0; wchar_t name[64],title[128];
    GetWindowThreadProcessId(window,&pid);
    if(!GetClassNameW(window,name,_countof(name))) return TRUE;
    /* Enumerated only on this fixture's fresh private desktop. The OS error
     * host is not the CMD PID. Close its rejection, never approve elevation. */
    if(!wcscmp(name,L"Shell_Dialog") && IsWindowVisible(window) &&
        GetWindowTextW(window,title,_countof(title)) &&
        !wcsncmp(title,L"This app can",12)) {
        puts("CONTROL private-desktop OS application rejection closed");
        PostMessageW(window,WM_CLOSE,0,0);
    } else if(pid==(DWORD)owner && !wcscmp(name,L"#32770")) {
        puts("CONTROL shell error dialog acknowledged on private desktop");
        PostMessageW(window,WM_COMMAND,IDOK,0);
    }
    return TRUE;
}
static DWORD wait_shell_control(PROCESS_INFORMATION *process)
{
    HDESK desktop=OpenDesktopW(test_desktop,0,FALSE,DESKTOP_ENUMERATE|DESKTOP_READOBJECTS);
    DWORD result; ULONGLONG until=GetTickCount64()+10000;
    do {
        result=WaitForSingleObject(process->hProcess,100);
        if(result!=WAIT_TIMEOUT) break;
        if(desktop) EnumDesktopWindows(desktop,dismiss_shell_error,(LPARAM)process->dwProcessId);
    } while(GetTickCount64()<until);
    if(desktop) {
        if(result==WAIT_TIMEOUT) EnumDesktopWindows(desktop,describe_window,0);
        CloseDesktop(desktop);
    }
    return result;
}

static HANDLE named_event(const wchar_t *prefix, const wchar_t *suffix, BOOL create)
{
    wchar_t name[160];
    swprintf_s(name, _countof(name), L"%ls.%ls", prefix, suffix);
    return create ? CreateEventW(NULL, TRUE, FALSE, name) :
        OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, name);
}

static int target(const wchar_t *prefix)
{
    wchar_t name[160];
    HANDLE mapping, ready, release;
    DWORD *pid, result;
    swprintf_s(name, _countof(name), L"%ls.state", prefix);
    mapping = OpenFileMappingW(FILE_MAP_WRITE, FALSE, name);
    if (!mapping) return 91;
    pid = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, sizeof(*pid));
    ready = named_event(prefix, L"ready", FALSE);
    release = named_event(prefix, L"release", FALSE);
    if (!pid || !ready || !release) return 92;
    *pid = GetCurrentProcessId();
    if (!SetEvent(ready)) return 93;
    result = WaitForSingleObject(release, 90000);
    CloseHandle(release);
    CloseHandle(ready);
    UnmapViewOfFile(pid);
    CloseHandle(mapping);
    return result == WAIT_OBJECT_0 ? 37 : 94;
}

static int check_case(const wchar_t *launcher, const wchar_t *image,
    const wchar_t *prefix_options, BOOL wait, BOOL kill_launcher, unsigned index,unsigned caller)
{
    wchar_t prefix[120], name[160], command[2048],shell[MAX_PATH],wrapped[4096],batch[MAX_PATH];
    wchar_t report[MAX_PATH]={0},root[MAX_PATH];
    const wchar_t *application=launcher;
    wchar_t *arguments=command;
    BOOL batch_created=FALSE;
    HANDLE mapping = NULL, ready = NULL, release = NULL, child = NULL;
    HANDLE input_read=NULL,input_write=NULL,null_output=INVALID_HANDLE_VALUE;
    DWORD *pid = NULL, launcher_result = 0, target_result = 0;
    PROCESS_INFORMATION process = {0};
    STARTUPINFOW startup = {sizeof(startup)};
    startup.lpDesktop=test_desktop;
    int failed = 1;
    swprintf_s(prefix, _countof(prefix), L"Local\\gw-%lu-%lu-%u",
        GetCurrentProcessId(), GetTickCount(), index);
    swprintf_s(name, _countof(name), L"%ls.state", prefix);
    mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
        0, sizeof(*pid), name);
    if (!mapping || GetLastError() == ERROR_ALREADY_EXISTS) goto done;
    pid = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, sizeof(*pid));
    ready = named_event(prefix, L"ready", TRUE);
    release = named_event(prefix, L"release", TRUE);
    if (!pid || !ready || !release) goto done;
    *pid = 0;
    if (swprintf_s(command, _countof(command), L"\"%ls\" %ls \"%ls\" --target %ls",
        launcher, prefix_options, image, prefix) < 0) goto done;
    if(caller==4) {
        DWORD n=GetEnvironmentVariableW(L"GUI_TEST_OBSERVER",shell,MAX_PATH);
        DWORD r=GetEnvironmentVariableW(L"GUI_TEST_REPORT",report,MAX_PATH);
        wchar_t *slash;
        if(!n || n>=MAX_PATH || !r || r>=MAX_PATH ||
            GetFileAttributesW(report)!=INVALID_FILE_ATTRIBUTES ||
            wcschr(launcher,L' ') || wcschr(image,L' ') ||
            wcscpy_s(root,MAX_PATH,launcher)) goto done;
        slash=wcsrchr(root,L'\\'); if(!slash) goto done; slash[1]=0;
        if(!SetEnvironmentVariableW(L"SystemRoot",root) ||
            !SetEnvironmentVariableW(L"MVDM_OBSERVER_PRIVATE_DESKTOP",NULL)) goto done;
        if(swprintf_s(wrapped,_countof(wrapped),L"\"%ls\" %ls %ls \"%ls\" COMMAND.COM "
            L"--observe-console-input-text \"%ls %ls %ls --target %ls\rmem\rexit\r\" "
            L"--observe-console-line-delay-ms 1500 --observation-timeout-ms 60000",
            shell,launcher,root,report,launcher,prefix_options,image,prefix)<0) goto done;
        application=shell; arguments=wrapped;
    } else if(caller) {
        DWORD n=GetEnvironmentVariableW(L"COMSPEC",shell,_countof(shell));
        if(!n || n>=_countof(shell)) goto done;
        application=shell; arguments=wrapped;
        if(caller==2) {
            wchar_t contents[4096]; char encoded[8192]; BOOL loss=FALSE;
            HANDLE file; DWORD written; int bytes;
            if(swprintf_s(batch,_countof(batch),L"%ls.%lu-%u.cmd",image,GetCurrentProcessId(),index)<0 ||
                swprintf_s(contents,_countof(contents),L"@echo off\r\n%ls\r\nexit /b %%errorlevel%%\r\n",command)<0)
                goto done;
            bytes=WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,contents,-1,encoded,sizeof(encoded),NULL,&loss);
            if(!bytes || loss) goto done;
            file=CreateFileW(batch,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
            if(file==INVALID_HANDLE_VALUE) goto done;
            batch_created=TRUE;
            n=WriteFile(file,encoded,(DWORD)bytes-1,&written,NULL) && written==(DWORD)bytes-1;
            CloseHandle(file); if(!n) goto done;
            if(swprintf_s(wrapped,_countof(wrapped),L"\"%ls\" /d /s /c \"\"%ls\"\"",shell,batch)<0) goto done;
        } else if(caller==3) {
            SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
            wchar_t contents[4096]; char encoded[8192];
            DWORD written; BOOL loss=FALSE; int bytes;
            if(!CreatePipe(&input_read,&input_write,&security,0) ||
                !SetHandleInformation(input_write,HANDLE_FLAG_INHERIT,0)) goto done;
            null_output=CreateFileW(L"NUL",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                &security,OPEN_EXISTING,0,NULL);
            if(null_output==INVALID_HANDLE_VALUE) goto done;
            startup.dwFlags|=STARTF_USESTDHANDLES;
            startup.hStdInput=input_read; startup.hStdOutput=startup.hStdError=null_output;
            if(swprintf_s(contents,_countof(contents),L"%ls\r\nexit /b %%errorlevel%%\r\n",command)<0 ||
                swprintf_s(wrapped,_countof(wrapped),L"\"%ls\" /d /q",shell)<0) goto done;
            bytes=WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,contents,-1,encoded,sizeof(encoded),NULL,&loss);
            if(!bytes || loss || !WriteFile(input_write,encoded,(DWORD)bytes-1,&written,NULL) ||
                written!=(DWORD)bytes-1) goto done;
        } else if(swprintf_s(wrapped,_countof(wrapped),L"\"%ls\" /d /s /c \"%ls\"",shell,command)<0) goto done;
    }
    if (!CreateProcessW(application, arguments, NULL, NULL, caller==3,
        caller==4 ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW,
        NULL, caller==4 ? root : NULL, &startup, &process)) goto done;
    if(input_read){CloseHandle(input_read);input_read=NULL;}
    if(input_write){CloseHandle(input_write);input_write=NULL;}
    if(null_output!=INVALID_HANDLE_VALUE){CloseHandle(null_output);null_output=INVALID_HANDLE_VALUE;}
    if (WaitForSingleObject(ready, caller==4 ? 30000 : 10000) != WAIT_OBJECT_0 || !*pid) goto done;
    child = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, *pid);
    if (!child || WaitForSingleObject(child, 0) != WAIT_TIMEOUT) goto done;
    if (wait) {
        /* The observer itself has paced input. Outwait that pacing before
         * claiming the DOS caller is blocked by GUI completion. */
        if (WaitForSingleObject(process.hProcess, caller==4 ? 6000 : 200) != WAIT_TIMEOUT) goto done;
        if(caller==4 && contains_text(report,L".line-02.console.txt",
            "bytes total conventional memory")) goto done;
    } else {
        if (WaitForSingleObject(process.hProcess, caller==4 ? 60000 : 5000) != WAIT_OBJECT_0 ||
            !GetExitCodeProcess(process.hProcess, &launcher_result) || launcher_result ||
            WaitForSingleObject(child, 0) != WAIT_TIMEOUT) goto done;
    }
    if (kill_launcher && (!TerminateProcess(process.hProcess, 96) ||
        WaitForSingleObject(process.hProcess, caller==4 ? 60000 : 5000) != WAIT_OBJECT_0 ||
        WaitForSingleObject(child, 0) != WAIT_TIMEOUT)) goto done;
    if (!SetEvent(release) || WaitForSingleObject(child, 5000) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child, &target_result) || target_result != 37 ||
        WaitForSingleObject(process.hProcess, caller==4 ? 60000 : 5000) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, &launcher_result) ||
        launcher_result != (caller==4 ? 0u : (kill_launcher ? 96u : (wait ? 37u : 0u)))) goto done;
    if(caller==4 && (!contains_text(report,L"","scripted-console-input=delivered") ||
        !contains_text(report,L".console.txt","bytes total conventional memory"))) goto done;
    failed = 0;
done:
    if(input_read) CloseHandle(input_read);
    if(input_write) CloseHandle(input_write);
    if(null_output!=INVALID_HANDLE_VALUE) CloseHandle(null_output);
    if (release) SetEvent(release);
    if (child) { WaitForSingleObject(child, 5000); CloseHandle(child); }
    if (process.hProcess) {
        if (WaitForSingleObject(process.hProcess, 5000) == WAIT_TIMEOUT) {
            TerminateProcess(process.hProcess, 95);
            WaitForSingleObject(process.hProcess, 5000);
        }
        CloseHandle(process.hProcess);
    }
    if (process.hThread) CloseHandle(process.hThread);
    if (ready) CloseHandle(ready);
    if (release) CloseHandle(release);
    if (pid) UnmapViewOfFile(pid);
    if (mapping) CloseHandle(mapping);
    if(batch_created && !DeleteFileW(batch)) failed=1;
    printf("GUI-WAIT case=%u caller=%u wait=%u launcher=%lu target=%lu %s\n",
        index,caller,wait,launcher_result,target_result,failed ? "FAIL" : "PASS");
    return failed;
}

static int check_locked_image(const wchar_t *launcher,const wchar_t *image)
{
    HANDLE lock=INVALID_HANDLE_VALUE;
    PROCESS_INFORMATION process={0};
    STARTUPINFOW startup={sizeof(startup)};
    wchar_t command[2048];
    DWORD expected=0,actual=0;
    unsigned index;
    int failed=1;
    startup.lpDesktop=test_desktop;
    /* A write-open image fails both native creation and original classifier
     * image mapping. run16's existing CMD fallback reports shell failure 1;
     * this is not a post-classification CreateProcess failure injection. */
    lock=CreateFileW(image,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (lock==INVALID_HANDLE_VALUE) goto done;
    swprintf_s(command,_countof(command),L"\"%ls\"",image);
    if (CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
        NULL,NULL,&startup,&process)) goto done;
    expected=GetLastError();
    if (expected!=ERROR_SHARING_VIOLATION) goto done;
    for(index=0;index<2;++index) {
        swprintf_s(command,_countof(command),L"\"%ls\" %ls \"%ls\"",
            launcher,index ? L"--wait" : L"",image);
        if (!CreateProcessW(launcher,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
            NULL,NULL,&startup,&process)) goto done;
        if (WaitForSingleObject(process.hProcess,10000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(process.hProcess,&actual) || actual!=1) goto done;
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
        ZeroMemory(&process,sizeof(process));
        printf("GUI-LOCKED-IMAGE wait=%u shell=%lu native-error=%lu PASS\n",index,actual,expected);
    }
    failed=0;
done:
    if(process.hProcess) {
        if(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT) {
            TerminateProcess(process.hProcess,95); WaitForSingleObject(process.hProcess,5000);
        }
        CloseHandle(process.hProcess);
    }
    if(process.hThread) CloseHandle(process.hThread);
    if(lock!=INVALID_HANDLE_VALUE) CloseHandle(lock);
    if(failed) printf("GUI-LOCKED-IMAGE actual=%lu native-error=%lu error=%lu FAIL\n",actual,expected,GetLastError());
    return failed;
}

static int check_missing_bad_image(const wchar_t *launcher,const wchar_t *image)
{
    wchar_t path[MAX_PATH],command[2048],shell[MAX_PATH];
    HANDLE file=INVALID_HANDLE_VALUE;
    BOOL created=FALSE;
    IMAGE_DOS_HEADER dos;
    IMAGE_FILE_HEADER pe;
    unsigned mode,wait;
    DWORD bytes,actual=0,expected=0,step=0;
    int failed=1;
    PROCESS_INFORMATION process={0};
    STARTUPINFOW startup={sizeof(startup)};
    startup.lpDesktop=test_desktop;
    bytes=GetEnvironmentVariableW(L"COMSPEC",shell,_countof(shell));
    if(!bytes || bytes>=_countof(shell)) return 1;
    if(swprintf_s(path,_countof(path),L"%ls.invalid-%lu.exe",image,GetCurrentProcessId())<0)
        return 1;
    SetLastError(0);
    if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES || GetLastError()!=ERROR_FILE_NOT_FOUND)
        return 1;
    for(mode=0;mode<2;++mode) {
        if(mode) {
            step=1;
            /* Copy only our authored fixture. DLL characteristics make a
             * valid PE image explicitly non-launchable, without the original
             * classifier's malformed-image DOS fallback ambiguity. */
            if(!CopyFileW(image,path,TRUE)) goto done;
            created=TRUE;
            file=CreateFileW(path,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
            if(file==INVALID_HANDLE_VALUE) goto done;
            if(!ReadFile(file,&dos,sizeof(dos),&bytes,NULL) || bytes!=sizeof(dos) ||
                dos.e_magic!=IMAGE_DOS_SIGNATURE || dos.e_lfanew<sizeof(dos)) goto done;
            if(SetFilePointer(file,dos.e_lfanew+sizeof(DWORD),NULL,FILE_BEGIN)==INVALID_SET_FILE_POINTER ||
                !ReadFile(file,&pe,sizeof(pe),&bytes,NULL) || bytes!=sizeof(pe)) goto done;
            pe.Characteristics |= IMAGE_FILE_DLL;
            if(SetFilePointer(file,dos.e_lfanew+sizeof(DWORD),NULL,FILE_BEGIN)==INVALID_SET_FILE_POINTER ||
                !WriteFile(file,&pe,sizeof(pe),&bytes,NULL) || bytes!=sizeof(pe)) goto done;
            CloseHandle(file); file=INVALID_HANDLE_VALUE;
            step=2;
            swprintf_s(command,_countof(command),L"\"%ls\"",path);
            if(CreateProcessW(path,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                NULL,NULL,&startup,&process) || GetLastError()!=ERROR_BAD_EXE_FORMAT) goto done;
        }
        /* Preserve the selected host shell's real failure result. Different
         * invalid images need not have the missing-command result 1. */
        swprintf_s(command,_countof(command),L"\"%ls\" /c \"%ls\"",shell,path);
        step=3;
        if(!CreateProcessW(shell,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
            NULL,NULL,&startup,&process)) goto done;
        {
            DWORD control_wait=wait_shell_control(&process);
            BOOL queried=GetExitCodeProcess(process.hProcess,&expected);
            printf("SHELL-CONTROL bad=%u wait=%lu queried=%u exit=%lu\n",mode,control_wait,queried,expected);
            if(control_wait!=WAIT_OBJECT_0 || !queried || !expected) goto done;
        }
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
        ZeroMemory(&process,sizeof(process));
        for(wait=0;wait<2;++wait) {
            step=4+wait;
            swprintf_s(command,_countof(command),L"\"%ls\" %ls \"%ls\"",
                launcher,wait ? L"--wait" : L"",path);
            if(!CreateProcessW(launcher,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
                NULL,NULL,&startup,&process)) goto done;
            if(wait_shell_control(&process)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(process.hProcess,&actual) || actual!=expected) goto done;
            CloseHandle(process.hThread); CloseHandle(process.hProcess);
            ZeroMemory(&process,sizeof(process));
            printf("GUI-IMAGE-REJECTION bad=%u wait=%u result=%lu shell=%lu PASS\n",mode,wait,actual,expected);
        }
    }
    failed=0;
done:
    if(process.hProcess) {
        if(WaitForSingleObject(process.hProcess,0)==WAIT_TIMEOUT) {
            TerminateProcess(process.hProcess,95); WaitForSingleObject(process.hProcess,5000);
        }
        CloseHandle(process.hProcess);
    }
    if(process.hThread) CloseHandle(process.hThread);
    if(file!=INVALID_HANDLE_VALUE) CloseHandle(file);
    if(created && !DeleteFileW(path)) failed=1;
    if(failed) printf("GUI-IMAGE-REJECTION step=%lu result=%lu shell=%lu error=%lu FAIL\n",step,actual,expected,GetLastError());
    return failed;
}

int wmain(int argc, wchar_t **argv)
{
    HDESK desktop;
    int failed;
    if(argc==3 && !wcscmp(argv[1],L"--target")) {
        wchar_t path[MAX_PATH]; FILE *log=NULL;
        DWORD n=GetEnvironmentVariableW(L"GUI_TEST_TARGET_LOG",path,MAX_PATH);
        int result=target(argv[2]); size_t length=wcslen(argv[2]);
        if(n && n<MAX_PATH && !_wfopen_s(&log,path,L"ab") && log) {
            fprintf(log,"TARGET pid=%lu result=%d length=%zu last=%04x previous=%04x\n",
                GetCurrentProcessId(),result,length,length ? argv[2][length-1] : 0,
                length>1 ? argv[2][length-2] : 0);
            fclose(log);
        }
        return result;
    }
    if (argc != 3 && !(argc==4 && (!wcscmp(argv[3],L"--dos-async") ||
        !wcscmp(argv[3],L"--dos-wait")))) return 87;
    /* Invalid-image controls must report failure, not await an invisible
     * system error dialog on this private desktop. Inherited by test children. */
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
    swprintf_s(test_desktop,_countof(test_desktop),L"NTVDMNativeGui-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(test_desktop,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop) return 1;
    if(argc==4) {
        BOOL wait=!wcscmp(argv[3],L"--dos-wait");
        failed=check_case(argv[1],argv[2],wait ? L"--wait" : L"",wait,FALSE,11,4);
        CloseDesktop(desktop);
        if(!failed) puts("RUN16-DOS-GUI-PROMPT-PASS");
        return failed;
    }
    failed=check_case(argv[1], argv[2], L"", FALSE, FALSE, 1,0) ||
        check_case(argv[1], argv[2], L"--wait", TRUE, FALSE, 2,0) ||
        check_case(argv[1], argv[2], L"--wait --", TRUE, FALSE, 3,0) ||
        check_case(argv[1], argv[2], L"--wait", TRUE, TRUE, 4,0) ||
        check_case(argv[1], argv[2], L"", FALSE, FALSE, 5,1) ||
        check_case(argv[1], argv[2], L"--wait", TRUE, FALSE, 6,1) ||
        check_case(argv[1], argv[2], L"", FALSE, FALSE, 7,2) ||
        check_case(argv[1], argv[2], L"--wait", TRUE, FALSE, 8,2) ||
        check_case(argv[1], argv[2], L"", FALSE, FALSE, 9,3) ||
        check_case(argv[1], argv[2], L"--wait", TRUE, FALSE, 10,3) ||
        check_locked_image(argv[1],argv[2]) ||
        check_missing_bad_image(argv[1],argv[2]);
    CloseDesktop(desktop);
    if(failed) return 1;
    puts("RUN16-GUI-WAIT-PASS cases=16");
    return 0;
}
