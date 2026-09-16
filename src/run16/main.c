/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * Image classification and CheckVDM stay in their selected original OpenNT
 * owners.  This entry owns only executable discovery, broker startup and
 * suspended-worker rollback around the admitted standalone boundary. */
#include <nt.h>
#include <base_classifier.h>
#include "basesrv/opennt/include/base_capture.h"
#include "basesrv/opennt/include/base_client.h"
#include "basesrv/opennt/include/base_config.h"
#include "basesrv/opennt/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
DWORD app_console_probe(void);
PVOID CsrPortHeap;
BOOL BaseCreateVDMEnvironment(PWCHAR environment, ANSI_STRING *ansi,
                              UNICODE_STRING *unicode);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *ansi, UNICODE_STRING *unicode);

static BOOL sibling_path(PCWSTR name, PWSTR output, DWORD capacity)
{
    DWORD length;
    PWSTR slash;
    if (!name || !output || !capacity)
        return FALSE;
    length = GetModuleFileNameW(NULL, output, capacity);
    if (!length || length >= capacity)
        return FALSE;
    slash = wcsrchr(output, L'\\');
    if (!slash)
        return FALSE;
    ++slash;
    if ((DWORD)(slash - output) + lstrlenW(name) + 1 > capacity)
        return FALSE;
    lstrcpyW(slash, name);
    return TRUE;
}

static BOOL image_has_extension(PCWSTR image)
{
    PCWSTR dot, slash, forward;
    if (!image || !*image)
        return FALSE;
    dot = wcsrchr(image, L'.');
    slash = wcsrchr(image, L'\\');
    forward = wcsrchr(image, L'/');
    if (forward && (!slash || forward > slash))
        slash = forward;
    return dot && (!slash || dot > slash);
}

/* This is product executable discovery, not image classification.  Resolve a
 * bare target beside the installed three-program package before consulting the
 * process search path, then pass only that canonical path to the selected
 * original OpenNT classifier. */
static BOOL resolve_image_path(PCWSTR image, PWSTR output, DWORD capacity)
{
    static PCWSTR const extensions[] = {L".com", L".exe", L".pif", L".bat"};
    WCHAR package[MAX_PATH];
    PWSTR slash;
    DWORD result;
    size_t index, count;
    BOOL bare;

    if (!image || !*image || !output || !capacity)
        return FALSE;
    bare = !wcschr(image, L'\\') && !wcschr(image, L'/');
    package[0] = L'\0';
    if (bare)
    {
        if (!GetModuleFileNameW(NULL, package, MAX_PATH))
            return FALSE;
        slash = wcsrchr(package, L'\\');
        if (!slash)
            return FALSE;
        *slash = L'\0';
    }
    count = image_has_extension(image) ? 1u : sizeof(extensions) / sizeof(extensions[0]);
    for (index = 0; index < count; ++index)
    {
        PCWSTR extension = image_has_extension(image) ? NULL : extensions[index];
        if (bare)
        {
            result = SearchPathW(package, image, extension, capacity, output, NULL);
            if (result && result < capacity)
                return TRUE;
        }
        result = SearchPathW(NULL, image, extension, capacity, output, NULL);
        if (result && result < capacity)
            return TRUE;
    }
    return FALSE;
}

static DWORD connect_broker(void)
{
    WCHAR broker[MAX_PATH];
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0};
    DWORD error=ERROR_GEN_FAILURE, attempt;
    if (!sibling_path(L"basesrv.exe", broker, MAX_PATH))
        return GetLastError();
    /* A concurrent launcher may own a healthy endpoint, or may be in the
     * broker-only empty-stop window.  Candidate creation is never readiness:
     * an instance losing the endpoint race exits, while a candidate started
     * after the old listener has drained becomes the fresh singleton. */
    for (attempt = 0; attempt < 100; ++attempt)
    {
        error = OpenNtBaseClientConnectCurrent();
        if (!error)
            return ERROR_SUCCESS;
        if (error == ERROR_REVISION_MISMATCH)
            return error; /* Never start/retry a broker for an incompatible peer. */
        /* Re-try only at bounded intervals.  This covers a listener which
         * has stopped between the failed Connect and the first candidate's
         * endpoint registration without turning the launcher into a broker
         * supervisor or keeping any product-local lifecycle state. */
        if (attempt==0 || attempt==20 || attempt==60)
        {
            if (CreateProcessW(broker, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                NULL, NULL, &startup, &child))
            {
                CloseHandle(child.hThread);
                CloseHandle(child.hProcess);
                ZeroMemory(&child,sizeof(child));
            }
        }
        Sleep(50);
    }
    return error;
}

static DWORD launch_vdm(ULONG binary, PCWSTR application, PCWSTR command)
{
    BASE_API_MSG message = {0};
    ANSI_STRING environment = {0};
    UNICODE_STRING unicode_environment = {0};
    UNICODE_STRING worker_command = {0};
    OPENNT_BASE_VDM_CONFIG configuration;
    const OPENNT_BASE_VDM_CONFIG *previous_configuration;
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION worker = {0};
    WCHAR worker_path[MAX_PATH];
    CHAR worker_image[MAX_PATH];
    CHAR kernel_stem[MAX_PATH];
    PCHAR image_slash;
    ULONG task = 0;
    ULONG vdm_size = 0;
    uint64_t reservation = 0;
    HANDLE parent_wait;
    DWORD result = ERROR_GEN_FAILURE;
    BOOL prepared = FALSE;
    BOOL published = FALSE;
    BOOL registered = FALSE;
    BOOL resumed = FALSE;
    BOOL startup_failed = FALSE;
    HANDLE startup_job=NULL;
    STARTUPINFOEXW guarded_startup={0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limits={0};
    SIZE_T attributes_bytes=0;

    /* This is the original parent-side VDM environment projection.  The
     * ANSI record is captured by BaseCheckVDM; the matching Unicode record
     * is passed unchanged to the newly-created worker. */
    if (!BaseCreateVDMEnvironment(NULL, &environment, &unicode_environment))
    {
        result = GetLastError();
        goto done;
    }
    GetStartupInfoW(&startup);
    if (!BaseCheckVDM(binary, application, command, NULL, &environment, &message, &task, 0, &startup))
    {
        result = GetLastError();
        goto done;
    }
    /* srvvdm.c has already selected and queued a same-Console resident DOS
     * record.  Its original Check reply carries the parent completion event;
     * wait and query the original exit-code route, never create another VDM. */
    if (message.u.CheckVDM.VDMState == VDM_PRESENT_AND_READY)
    {
        parent_wait = message.u.CheckVDM.WaitObjectForParent;
        result=OpenNtBaseClientWatchBroker();
        if (result) { CloseHandle(parent_wait); goto done; }
        if (!parent_wait || WaitForSingleObject(parent_wait, INFINITE) != WAIT_OBJECT_0 ||
            !BaseCheckForVDM(parent_wait, &result))
            result = GetLastError();
        CloseHandle(parent_wait);
        goto done;
    }
    if (message.u.CheckVDM.VDMState != VDM_NOT_PRESENT)
    {
        result = ERROR_INVALID_DATA;
        goto done;
    }
    /* CheckVDM has published an original DOS/WOW record.  Every failure
     * before the worker actually runs must use the matching original
     * UPDATE_VDM_UNDO_CREATION cleanup, not leave that record to a later
     * unrelated launcher. */
    published = TRUE;
    result = OpenNtBaseClientReserveWorker(task, &reservation);
    if (result)
        goto done;
    if (!sibling_path(L"ntvdm.exe", worker_path, MAX_PATH))
    {
        result = GetLastError();
        goto done;
    }
    if (!WideCharToMultiByte(CP_ACP, 0, worker_path, -1, worker_image,
                             sizeof(worker_image), NULL, NULL))
    {
        result = GetLastError();
        goto done;
    }
    image_slash = strrchr(worker_image, '\\');
    if (!image_slash || sprintf_s(kernel_stem, sizeof(kernel_stem), "%.*s\\system32\\krnl386", (int)(image_slash - worker_image), worker_image) <= 0 ||
        !OpenNtBaseInitializeVdmConfig(&configuration, worker_image, kernel_stem))
    {
        result = GetLastError();
        goto done;
    }
    previous_configuration = OpenNtBaseBindVdmConfig(&configuration);
    /* BaseGetVdmConfigInfo's second parameter is only the DOS new-console
     * session id.  BaseCheckVDM also returns a nonzero WOW task id, but that
     * is owned by the shared-WOW record and must not become the worker's
     * original -i switch (which selects separate WOW). */
    if (!BaseGetVdmConfigInfo(worker_path,
                              binary == BINARY_TYPE_DOS ? task : 0, binary, &worker_command, &vdm_size))
    {
        result = GetLastError();
        (void)OpenNtBaseBindVdmConfig(previous_configuration);
        goto done;
    }
    (void)OpenNtBaseBindVdmConfig(previous_configuration);
    /* A nonzero DOS session id is CheckDOS's original no-console result.
     * Create the matching physical Console rather than letting that worker
     * inherit the resident COMMAND Console it was deliberately separated
     * from. */
    /* Atomic startup containment. If this launcher dies before Prepare, the
     * broker cannot know this child yet. A non-inherited kill-on-close job,
     * installed as part of CreateProcess, closes that otherwise orphaned
     * suspended-child window. Disarm only after broker ownership is bound. */
    startup_job=CreateJobObjectW(NULL,NULL);
    if (!startup_job) { result=GetLastError(); goto done; }
    job_limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(startup_job,JobObjectExtendedLimitInformation,
            &job_limits,sizeof(job_limits))) { result=GetLastError(); goto done; }
    InitializeProcThreadAttributeList(NULL,1,0,&attributes_bytes);
    guarded_startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,attributes_bytes);
    if (!guarded_startup.lpAttributeList) { result=ERROR_NOT_ENOUGH_MEMORY; goto done; }
    if (!InitializeProcThreadAttributeList(guarded_startup.lpAttributeList,1,0,&attributes_bytes)) {
        result=GetLastError(); HeapFree(GetProcessHeap(),0,guarded_startup.lpAttributeList);
        guarded_startup.lpAttributeList=NULL; goto done;
    }
    if (!UpdateProcThreadAttribute(guarded_startup.lpAttributeList,0,
            PROC_THREAD_ATTRIBUTE_JOB_LIST,&startup_job,sizeof(startup_job),NULL,NULL)) {
        result=GetLastError(); goto done;
    }
    guarded_startup.StartupInfo=startup;
    guarded_startup.StartupInfo.cb=sizeof(guarded_startup);
    if (!CreateProcessW(worker_path, worker_command.Buffer, NULL, NULL, TRUE,
                        CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | EXTENDED_STARTUPINFO_PRESENT |
                        (binary == BINARY_TYPE_DOS && task ? CREATE_NEW_CONSOLE : 0),
                        unicode_environment.Buffer, NULL, &guarded_startup.StartupInfo, &worker))
    {
        result = GetLastError();
        goto done;
    }
    result = OpenNtBaseClientPrepareWorker(reservation, worker.hProcess);
    if (result)
        goto done;
    prepared = TRUE;
    job_limits.BasicLimitInformation.LimitFlags=0;
    if (!SetInformationJobObject(startup_job,JobObjectExtendedLimitInformation,
            &job_limits,sizeof(job_limits))) { result=GetLastError(); goto done; }
    CloseHandle(startup_job);startup_job=NULL;
    /* Preserve the original post-CreateProcess registration shape.  The
     * broker resolves the worker from the authenticated reservation; it does
     * not receive this raw handle in its command wire. */
    parent_wait = worker.hProcess;
    if (!BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE, &parent_wait, task, binary))
    {
        result = GetLastError();
        goto done;
    }
    registered = TRUE;
    if (ResumeThread(worker.hThread) == (DWORD)-1)
    {
        result = GetLastError();
        goto done;
    }
    resumed = TRUE;
    result=OpenNtBaseClientWatchBroker();
    if (result) {
        TerminateProcess(worker.hProcess,result);
        startup_failed=TRUE;
        goto waited;
    }
    /* A dead worker cannot deliver another completion. Keep original task
     * results, but never return to an infinite event wait after process exit. */
    {
        HANDLE completion[2]={parent_wait ? parent_wait : worker.hProcess,worker.hProcess};
        DWORD code,wait=WaitForMultipleObjects(completion[0]==completion[1] ? 1 : 2,
            completion,FALSE,INFINITE);
        if (wait==WAIT_FAILED) {
            result=GetLastError();
            goto waited;
        }
        if (wait==WAIT_OBJECT_0+1 &&
            WaitForSingleObject(parent_wait,2000)!=WAIT_OBJECT_0) {
            result=ERROR_PROCESS_ABORTED;
            fputs("run16: ntvdm exited without task completion\n",stderr);
            goto waited;
        }
        if (WaitForSingleObject(worker.hProcess,0)==WAIT_OBJECT_0 &&
            GetExitCodeProcess(worker.hProcess,&code) &&
            (code==ERROR_REVISION_MISMATCH || code==RPC_S_UNKNOWN_IF ||
             code==RPC_S_PROCNUM_OUT_OF_RANGE)) {
            fputs("run16: version mismatch: ntvdm worker rejected the broker protocol/application version\n",stderr);
            result=ERROR_REVISION_MISMATCH;
            startup_failed=TRUE;
            goto waited;
        }
    }
    if (parent_wait && parent_wait != worker.hProcess)
    {
        if (!BaseCheckForVDM(parent_wait, &result))
            result = GetLastError();
    }
    else if (!GetExitCodeProcess(worker.hProcess, &result))
    {
        result = GetLastError();
    }
waited:
    if (parent_wait && parent_wait != worker.hProcess)
        CloseHandle(parent_wait);
done:
    if (guarded_startup.lpAttributeList) {
        DeleteProcThreadAttributeList(guarded_startup.lpAttributeList);
        HeapFree(GetProcessHeap(),0,guarded_startup.lpAttributeList);
    }
    if (startup_job) CloseHandle(startup_job);
    if (published && (!resumed || startup_failed) && result)
    {
        HANDLE undo_task = (HANDLE)(ULONG_PTR)task;
        ULONG undo_state = registered ? VDM_FULLY_CREATED : VDM_PARTIALLY_CREATED;
        /* Preserve the launch failure.  This is best-effort only because the
         * original record owner may itself report an earlier cleanup fault. */
        (void)BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION, &undo_task, undo_state, binary);
    }
    if (worker.hProcess && (!prepared || !resumed))
    {
        (void)TerminateProcess(worker.hProcess, result ? result : ERROR_PROCESS_ABORTED);
        (void)WaitForSingleObject(worker.hProcess, INFINITE);
    }
    if (reservation)
    {
        DWORD release = OpenNtBaseClientReleaseWorker(reservation);
        if (!result && release)
            result = release;
    }
    if (worker.hThread)
        CloseHandle(worker.hThread);
    if (worker.hProcess)
        CloseHandle(worker.hProcess);
    if (worker_command.Buffer)
        RtlFreeUnicodeString(&worker_command);
    (void)BaseDestroyVDMEnvironment(&environment, &unicode_environment);
    return result;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    LPWSTR *arguments;
    PWSTR childCommand;
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0};
    WCHAR application[MAX_PATH];
    WCHAR shell_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
    DWORD type, result = ERROR_INVALID_PARAMETER, binary = 0, comspec_bytes;
    BOOL image_resolved;
    int count;
    size_t bytes;
    (void)instance;
    (void)previous;
    (void)show;
    if (!command || !*command)
    {
        fputs("Usage: run16.exe <binary> [arguments]\n", stderr);
        return ERROR_INVALID_PARAMETER;
    }
    /* Decode only to identify the target; original BaseCheckVDM receives the
     * untouched tail and supplies its original OEM command representation. */
    arguments = CommandLineToArgvW(command, &count);
    if (!arguments)
        return (int)GetLastError();
    if (count == 1 && !wcscmp(arguments[0], L"--internal-console-probe"))
    {
        LocalFree(arguments);
        return (int)app_console_probe();
    }
    RtlInitUnicodeString(&BaseDotComSuffixName, L".com");
    RtlInitUnicodeString(&BaseDotPifSuffixName, L".pif");
    RtlInitUnicodeString(&BaseDotExeSuffixName, L".exe");
    if (!count || !*arguments[0])
        goto done;
    image_resolved = resolve_image_path(arguments[0], application, MAX_PATH);
    if (!OpenNtBaseGetBinaryTypeW(image_resolved ? application : arguments[0], &type))
    {
        /* The original COMMAND worker has already chosen COMSPEC /c before
         * this public launcher sees a native-child tail.  A token which is
         * not an image may be a command built-in, batch file, or shell
         * syntax.  Preserve its copied text for the public shell; do not add
         * a second classifier/parser to run16 or redirect it into the VDM.
         * This entry is already Unicode CreateProcessW-based, so retain its
         * standard-handle and command-text convention rather than crossing
         * an unrelated ANSI classification helper. */
        comspec_bytes = GetEnvironmentVariableW(L"COMSPEC", application, MAX_PATH);
        if (!comspec_bytes || comspec_bytes >= MAX_PATH)
        {
            result = comspec_bytes ? ERROR_FILENAME_EXCED_RANGE : GetLastError();
            goto done;
        }
        if (swprintf_s(shell_command, sizeof(shell_command) / sizeof(shell_command[0]),
                       L"\"%s\" /c %s", application, command) < 0)
        {
            result = ERROR_FILENAME_EXCED_RANGE;
            goto done;
        }
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        if (!CreateProcessW(application, shell_command, NULL, NULL, TRUE, 0, NULL, NULL,
                            &startup, &child))
            result = GetLastError();
        else
        {
            CloseHandle(child.hThread);
            if (WaitForSingleObject(child.hProcess, INFINITE) != WAIT_OBJECT_0 ||
                !GetExitCodeProcess(child.hProcess, &result))
                result = GetLastError();
            CloseHandle(child.hProcess);
        }
        goto done;
    }
    if (type == SCS_DOS_BINARY)
        binary = BINARY_TYPE_DOS;
    else if (type == SCS_PIF_BINARY)
        binary = BINARY_TYPE_DOS | BINARY_TYPE_DOS_PIF;
    else if (type == SCS_WOW_BINARY)
        binary = BINARY_TYPE_WIN16;
    if (binary)
    {
        if (!image_resolved &&
            !GetFullPathNameW(arguments[0], MAX_PATH, application, NULL))
        {
            result = GetLastError();
            goto done;
        }
        CsrPortHeap = HeapCreate(0, 0, 0);
        if (!CsrPortHeap)
        {
            result = ERROR_NOT_ENOUGH_MEMORY;
            goto done;
        }
        result = connect_broker();
        if (!result)
            result = launch_vdm(binary, application, command);
        OpenNtBaseClientDisconnectCurrent();
        if (!HeapDestroy(CsrPortHeap) && !result)
            result = ERROR_BUSY;
        CsrPortHeap = NULL;
        goto done;
    }
    if (type != SCS_32BIT_BINARY)
    {
        result = ERROR_NOT_SUPPORTED;
        goto done;
    }
    bytes = (wcslen(command) + 1) * sizeof(WCHAR);
    childCommand = HeapAlloc(GetProcessHeap(), 0, bytes);
    if (!childCommand)
    {
        result = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }
    memcpy(childCommand, command, bytes);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (!CreateProcessW(image_resolved ? application : arguments[0], childCommand,
                        NULL, NULL, TRUE, 0, NULL, NULL, &startup, &child))
        result = GetLastError();
    else
    {
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess, INFINITE) != WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child.hProcess, &result))
            result = GetLastError();
        CloseHandle(child.hProcess);
    }
    HeapFree(GetProcessHeap(), 0, childCommand);
done:
    LocalFree(arguments);
    return (int)result;
}
