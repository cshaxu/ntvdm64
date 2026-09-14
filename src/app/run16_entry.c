/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * Image classification and CheckVDM stay in their selected original OpenNT
 * owners.  This entry owns only executable discovery, broker startup and
 * suspended-worker rollback around the admitted standalone boundary. */
#include <nt.h>
#include <base_classifier.h>
#include "adapter-opennt-host/basesrv/include/base_capture.h"
#include "adapter-opennt-host/basesrv/include/base_client.h"
#include "adapter-opennt-host/basesrv/include/base_config.h"
#include "adapter-opennt-host/basesrv/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
DWORD app_console_probe(void);
PVOID CsrPortHeap;
BOOL BaseCreateVDMEnvironment(PWCHAR environment,ANSI_STRING *ansi,
    UNICODE_STRING *unicode);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *ansi,UNICODE_STRING *unicode);

static BOOL sibling_path(PCWSTR name,PWSTR output,DWORD capacity)
{
    DWORD length;
    PWSTR slash;
    if (!name || !output || !capacity) return FALSE;
    length=GetModuleFileNameW(NULL,output,capacity);
    if (!length || length>=capacity) return FALSE;
    slash=wcsrchr(output,L'\\');
    if (!slash) return FALSE;
    ++slash;
    if ((DWORD)(slash-output)+lstrlenW(name)+1>capacity) return FALSE;
    lstrcpyW(slash,name);
    return TRUE;
}

static DWORD connect_broker(void)
{
    WCHAR broker[MAX_PATH];
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    DWORD error=OpenNtBaseClientConnectCurrent(),attempt;
    if (!error) return ERROR_SUCCESS;
    if (!sibling_path(L"basesrv.exe",broker,MAX_PATH)) return GetLastError();
    /* A concurrent launcher may win endpoint creation.  CreateProcess only
     * starts a candidate; successful authenticated Connect is readiness. */
    if (!CreateProcessW(broker,NULL,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,
            &startup,&child)) return GetLastError();
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    for (attempt=0;attempt<100;++attempt) {
        Sleep(50);
        error=OpenNtBaseClientConnectCurrent();
        if (!error) return ERROR_SUCCESS;
    }
    return error;
}

static DWORD launch_vdm(ULONG binary,PCWSTR application,PCWSTR command)
{
    BASE_API_MSG message={0};
    ANSI_STRING environment={0};
    UNICODE_STRING unicode_environment={0};
    UNICODE_STRING worker_command={0};
    OPENNT_BASE_VDM_CONFIG configuration;
    const OPENNT_BASE_VDM_CONFIG *previous_configuration;
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION worker={0};
    WCHAR worker_path[MAX_PATH];
    CHAR worker_image[MAX_PATH];
    CHAR kernel_stem[MAX_PATH];
    PCHAR image_slash;
    ULONG task=0;
    ULONG vdm_size=0;
    uint64_t reservation=0;
    HANDLE parent_wait;
    DWORD result=ERROR_GEN_FAILURE;
    BOOL prepared=FALSE;

    /* This is the original parent-side VDM environment projection.  The
     * ANSI record is captured by BaseCheckVDM; the matching Unicode record
     * is passed unchanged to the newly-created worker. */
    if (!BaseCreateVDMEnvironment(NULL,&environment,&unicode_environment)) {
        result=GetLastError(); goto done;
    }
    GetStartupInfoW(&startup);
    if (!BaseCheckVDM(binary,application,command,NULL,&environment,&message,&task,0,&startup)) {
        result=GetLastError(); goto done;
    }
    /* Worker reuse is source-owned selection work for S5.  Do not send a new
     * command into a currently resident guest before that contract exists. */
    if (message.u.CheckVDM.VDMState!=VDM_NOT_PRESENT) {
        result=ERROR_NOT_SUPPORTED; goto done;
    }
    result=OpenNtBaseClientReserveWorker(task,&reservation);
    if (result) goto done;
    if (!sibling_path(L"ntvdm.exe",worker_path,MAX_PATH)) { result=GetLastError(); goto done; }
    if (!WideCharToMultiByte(CP_ACP,0,worker_path,-1,worker_image,
            sizeof(worker_image),NULL,NULL)) { result=GetLastError(); goto done; }
    image_slash=strrchr(worker_image,'\\');
    if (!image_slash || sprintf_s(kernel_stem,sizeof(kernel_stem),
            "%.*s\\system32\\krnl386",(int)(image_slash-worker_image),
            worker_image)<=0 ||
        !OpenNtBaseInitializeVdmConfig(&configuration,worker_image,kernel_stem)) {
        result=GetLastError(); goto done;
    }
    previous_configuration=OpenNtBaseBindVdmConfig(&configuration);
    if (!BaseGetVdmConfigInfo(worker_path,task,binary,&worker_command,&vdm_size)) {
        result=GetLastError();
        (void)OpenNtBaseBindVdmConfig(previous_configuration);
        goto done;
    }
    (void)OpenNtBaseBindVdmConfig(previous_configuration);
    if (!CreateProcessW(worker_path,worker_command.Buffer,NULL,NULL,TRUE,
            CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT,
            unicode_environment.Buffer,NULL,&startup,&worker)) {
        result=GetLastError(); goto done;
    }
    result=OpenNtBaseClientPrepareWorker(reservation,worker.hProcess);
    if (result) goto done;
    prepared=TRUE;
    /* Preserve the original post-CreateProcess registration shape.  The
     * broker resolves the worker from the authenticated reservation; it does
     * not receive this raw handle in its command wire. */
    parent_wait=worker.hProcess;
    if (!BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,&parent_wait,task,binary)) {
        result=GetLastError(); goto done;
    }
    if (ResumeThread(worker.hThread)==(DWORD)-1) { result=GetLastError(); goto done; }
    if (WaitForSingleObject(parent_wait ? parent_wait : worker.hProcess,INFINITE)!=WAIT_OBJECT_0) {
        result=GetLastError();
    } else if (parent_wait && parent_wait!=worker.hProcess) {
        if (!BaseCheckForVDM(parent_wait,&result)) result=GetLastError();
    } else if (!GetExitCodeProcess(worker.hProcess,&result)) {
        result=GetLastError();
    }
    if (parent_wait && parent_wait!=worker.hProcess) CloseHandle(parent_wait);
done:
    if (worker.hProcess && !prepared) {
        (void)TerminateProcess(worker.hProcess,result ? result : ERROR_PROCESS_ABORTED);
        (void)WaitForSingleObject(worker.hProcess,INFINITE);
    }
    if (reservation) {
        DWORD release=OpenNtBaseClientReleaseWorker(reservation);
        if (!result && release) result=release;
    }
    if (worker.hThread) CloseHandle(worker.hThread);
    if (worker.hProcess) CloseHandle(worker.hProcess);
    if (worker_command.Buffer) RtlFreeUnicodeString(&worker_command);
    (void)BaseDestroyVDMEnvironment(&environment,&unicode_environment);
    return result;
}

int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    LPWSTR *arguments;
    PWSTR childCommand;
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0};
    WCHAR application[MAX_PATH];
    WCHAR shell_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
    DWORD type,result=ERROR_INVALID_PARAMETER,binary=0,comspec_bytes;
    int count;
    size_t bytes;
    (void)instance;(void)previous;(void)show;
    if (!command || !*command) {
        fputs("Usage: run16.exe <binary> [arguments]\\n",stderr);
        return ERROR_INVALID_PARAMETER;
    }
    /* Decode only to identify the target; original BaseCheckVDM receives the
     * untouched tail and supplies its original OEM command representation. */
    arguments=CommandLineToArgvW(command,&count);
    if (!arguments) return (int)GetLastError();
    if (count==1 && !wcscmp(arguments[0],L"--internal-console-probe")) {
        LocalFree(arguments);return (int)app_console_probe();
    }
    RtlInitUnicodeString(&BaseDotComSuffixName,L".com");
    RtlInitUnicodeString(&BaseDotPifSuffixName,L".pif");
    RtlInitUnicodeString(&BaseDotExeSuffixName,L".exe");
    if (!count || !*arguments[0]) goto done;
    if (!OpenNtBaseGetBinaryTypeW(arguments[0],&type)) {
        /* The original COMMAND worker has already chosen COMSPEC /c before
         * this public launcher sees a native-child tail.  A token which is
         * not an image may be a command built-in, batch file, or shell
         * syntax.  Preserve its copied text for the public shell; do not add
         * a second classifier/parser to run16 or redirect it into the VDM.
         * This entry is already Unicode CreateProcessW-based, so retain its
         * standard-handle and command-text convention rather than crossing
         * an unrelated ANSI classification helper. */
        comspec_bytes=GetEnvironmentVariableW(L"COMSPEC",application,MAX_PATH);
        if (!comspec_bytes || comspec_bytes>=MAX_PATH) {
            result=comspec_bytes ? ERROR_FILENAME_EXCED_RANGE : GetLastError();
            goto done;
        }
        if (swprintf_s(shell_command,sizeof(shell_command)/sizeof(shell_command[0]),
                L"\"%s\" /c %s",application,command)<0) {
            result=ERROR_FILENAME_EXCED_RANGE; goto done;
        }
        startup.dwFlags=STARTF_USESTDHANDLES;
        startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
        startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
        startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
        if (!CreateProcessW(application,shell_command,NULL,NULL,TRUE,0,NULL,NULL,
                &startup,&child)) result=GetLastError();
        else {
            CloseHandle(child.hThread);
            if (WaitForSingleObject(child.hProcess,INFINITE)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(child.hProcess,&result)) result=GetLastError();
            CloseHandle(child.hProcess);
        }
        goto done;
    }
    if (type==SCS_DOS_BINARY) binary=BINARY_TYPE_DOS;
    else if (type==SCS_PIF_BINARY) binary=BINARY_TYPE_DOS|BINARY_TYPE_DOS_PIF;
    else if (type==SCS_WOW_BINARY) binary=BINARY_TYPE_WIN16;
    if (binary) {
        if (!GetFullPathNameW(arguments[0],MAX_PATH,application,NULL)) { result=GetLastError(); goto done; }
        CsrPortHeap=HeapCreate(0,0,0);
        if (!CsrPortHeap) { result=ERROR_NOT_ENOUGH_MEMORY; goto done; }
        result=connect_broker();
        if (!result) result=launch_vdm(binary,application,command);
        OpenNtBaseClientDisconnectCurrent();
        if (!HeapDestroy(CsrPortHeap) && !result) result=ERROR_BUSY;
        CsrPortHeap=NULL;
        goto done;
    }
    if (type!=SCS_32BIT_BINARY) { result=ERROR_NOT_SUPPORTED; goto done; }
    bytes=(wcslen(command)+1)*sizeof(WCHAR);
    childCommand=HeapAlloc(GetProcessHeap(),0,bytes);
    if (!childCommand) { result=ERROR_NOT_ENOUGH_MEMORY; goto done; }
    memcpy(childCommand,command,bytes);
    startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    if (!CreateProcessW(arguments[0],childCommand,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child))
        result=GetLastError();
    else {
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess,INFINITE)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child.hProcess,&result)) result=GetLastError();
        CloseHandle(child.hProcess);
    }
    HeapFree(GetProcessHeap(),0,childCommand);
done:
    LocalFree(arguments);
    return (int)result;
}
