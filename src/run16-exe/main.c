/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * Image classification and CheckVDM stay in their selected original OpenNT
 * owners.  This entry owns only executable discovery, broker startup and
 * suspended-worker rollback around the admitted standalone boundary. */
#include <nt.h>
#include <base_classifier.h>
#include "basesrv-exe/opennt/include/base_capture.h"
#include "basesrv-exe/opennt/include/base_client.h"
#include "basesrv-exe/opennt/include/base_config.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
DWORD app_console_probe(void);
PVOID CsrPortHeap;
BOOL BaseCreateVDMEnvironment(PWCHAR environment, ANSI_STRING *ansi,
                              UNICODE_STRING *unicode);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *ansi, UNICODE_STRING *unicode);

static void s34_run16_trace(const char *stage, DWORD value)
{
    char path[MAX_PATH],line[96];
    HANDLE file;
    DWORD bytes,written;
    if (!GetEnvironmentVariableA("MVDM_S34_TRACE_PATH",path,sizeof(path))) return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
        OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return;
    bytes=(DWORD)sprintf_s(line,sizeof(line),"%lu run16-%s %lu\r\n",
        (unsigned long)GetCurrentProcessId(),stage,(unsigned long)value);
    if (bytes) (void)WriteFile(file,line,bytes,&written,NULL);
    CloseHandle(file);
}

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
    DWORD worker_status;
    BOOL prepared = FALSE;
    BOOL published = FALSE;
    BOOL registered = FALSE;
    BOOL resumed = FALSE;
    BOOL startup_failed = FALSE;
    HANDLE startup_job=NULL;
    STARTUPINFOEXW guarded_startup={0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limits={0};
    SIZE_T attributes_bytes=0;
    DWORD check_creation_flags=(binary & BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS_PIF ?
        CREATE_NEW_CONSOLE : 0;

    /* This is the original parent-side VDM environment projection.  The
     * ANSI record is captured by BaseCheckVDM; the matching Unicode record
     * is passed unchanged to the newly-created worker. */
    if (!BaseCreateVDMEnvironment(NULL, &environment, &unicode_environment))
    {
        result = GetLastError();
        goto done;
    }
    GetStartupInfoW(&startup);
    /* The original BaseCheckVDM accepts either caller-supplied STARTF
     * standard handles or the process-parameter equivalents.  The public
     * CLI is itself that CreateProcess-shaped caller.  Modern Terminal may
     * inherit file/pipe handles without reflecting them in GetStartupInfoW,
     * while the historical PEB fallback is intentionally not a host PEB
     * alias.  Publish the inherited stream triple explicitly, without
     * changing BaseCheckVDM's record or classification policy. */
    startup.dwFlags |= STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    /* Original BaseCheckVDM rejects a PIF submitted from an existing Console
     * unless CreateProcess declared its required separate Console.  Preserve
     * that original PIF prerequisite at the public CreateProcess-shaped
     * boundary; the source-owned CheckDOS record still allocates its session
     * id and the later worker creation consumes that result. */
    if (!BaseCheckVDM(binary, application, command, NULL, &environment, &message,
                      &task, check_creation_flags, &startup))
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
    /* PIF is a CheckVDM input subtype, not a distinct worker kind.
     * Config/Update and Console creation consume the original base type. */
    binary &= ~BINARY_SUBTYPE_MASK;
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
    /* The command record owns the stream triple.  Do not also inherit it at
     * process creation: a persistent worker retaining that duplicate pipe
     * writer prevents the caller's downstream pipe from observing EOF after
     * COMMAND exits.  GetNextVDMCommand receives the worker-local, typed
     * attachments before it starts the guest command. */
    guarded_startup.StartupInfo.dwFlags &= ~STARTF_USESTDHANDLES;
    guarded_startup.StartupInfo.hStdInput=NULL;
    guarded_startup.StartupInfo.hStdOutput=NULL;
    guarded_startup.StartupInfo.hStdError=NULL;
    if (!CreateProcessW(worker_path, worker_command.Buffer, NULL, NULL, FALSE,
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
            if (GetExitCodeProcess(worker.hProcess,&code))
                s34_run16_trace("worker-exit",code);
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
    /* Diagnostic only: ERROR_PROCESS_ABORTED is the launcher's public
     * result for an uncompleted worker.  Preserve the actual child status
     * in the existing opt-in S34 trace so failure attribution does not
     * mistake the broker-side result for the worker's own exit code. */
    if (worker.hProcess && WaitForSingleObject(worker.hProcess, 0) == WAIT_OBJECT_0 &&
        GetExitCodeProcess(worker.hProcess, &worker_status))
        s34_run16_trace("worker-status", worker_status);
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
    if (reservation && (!prepared || !resumed))
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
    PCWSTR image_argument;
    PCWSTR launch_command;
    PCWSTR option, tail;
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0};
    WCHAR application[MAX_PATH];
    WCHAR split_image[MAX_PATH];
    WCHAR normalized_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
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
    s34_run16_trace("args",(DWORD)count);
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
    /* The former one-process product accepted a compact first option such as
     * `command/c`: its bare image part and /option were separate argv items.
     * Restore that entry convention for any resolvable bare package image,
     * but do not reinterpret a backslash/drive-qualified forward-slash path. */
    image_argument=arguments[0];
    launch_command=command;
    image_resolved=FALSE;
    option=wcschr(arguments[0],L'/');
    if (option && option!=arguments[0] && !wcschr(arguments[0],L'\\') &&
        !wcschr(arguments[0],L':') && (size_t)(option-arguments[0])<ARRAYSIZE(split_image))
    {
        memcpy(split_image,arguments[0],(size_t)(option-arguments[0])*sizeof(WCHAR));
        split_image[option-arguments[0]]=L'\0';
        image_resolved=resolve_image_path(split_image,application,MAX_PATH);
        tail=command;
        while (*tail==L' ' || *tail==L'\t') ++tail;
        if (*tail==L'\"') {
            ++tail;
            while (*tail && *tail!=L'\"') ++tail;
            if (*tail==L'\"') ++tail;
        } else while (*tail && *tail!=L' ' && *tail!=L'\t') ++tail;
        if (image_resolved && swprintf_s(normalized_command,ARRAYSIZE(normalized_command),L"%ls %ls%ls",
            split_image,option,tail)<0)
        {
            result=ERROR_FILENAME_EXCED_RANGE;
            goto done;
        }
        if (image_resolved) {
            image_argument=split_image;
            launch_command=normalized_command;
        }
    }
    if (!image_resolved)
        image_resolved=resolve_image_path(image_argument,application,MAX_PATH);
    if (!OpenNtBaseGetBinaryTypeW(image_resolved ? application : image_argument, &type))
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
                       L"\"%s\" /c %s", application, launch_command) < 0)
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
        s34_run16_trace("binary",binary);
        PCWSTR image_name;
        if (!image_resolved &&
            !GetFullPathNameW(image_argument, MAX_PATH, application, NULL))
        {
            result = GetLastError();
            goto done;
        }
        image_name=wcsrchr(application,L'\\');
        image_name=image_name ? image_name+1 : application;
        /* `/c` receives one command-text argv item from a CreateProcess
         * caller.  Its outer quotes exist only to preserve that one argv
         * item through Windows tokenization.  Passing those transport quotes
         * verbatim makes original COMMAND try to execute `left | right` as
         * one image name.  For this exact COMMAND /c composite form, rebuild
         * only the outer argv boundary; COMMAND remains the sole parser and
         * owner of its <, > and | syntax. */
        if (binary==BINARY_TYPE_DOS && count==3 &&
            !_wcsicmp(image_name,L"COMMAND.COM") &&
            (!_wcsicmp(arguments[1],L"/c") || !_wcsicmp(arguments[1],L"/C")))
        {
            if (swprintf_s(normalized_command,ARRAYSIZE(normalized_command),L"%ls %ls %ls",
                arguments[0],arguments[1],arguments[2])<0)
            {
                result=ERROR_FILENAME_EXCED_RANGE;
                goto done;
            }
            launch_command=normalized_command;
        }
        CsrPortHeap = HeapCreate(0, 0, 0);
        if (!CsrPortHeap)
        {
            result = ERROR_NOT_ENOUGH_MEMORY;
            goto done;
        }
        result = connect_broker();
        s34_run16_trace("broker",result);
        if (!result)
        {
            result = launch_vdm(binary, application, launch_command);
            s34_run16_trace("worker",result);
        }
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
    bytes = (wcslen(launch_command) + 1) * sizeof(WCHAR);
    childCommand = HeapAlloc(GetProcessHeap(), 0, bytes);
    if (!childCommand)
    {
        result = ERROR_NOT_ENOUGH_MEMORY;
        goto done;
    }
    memcpy(childCommand, launch_command, bytes);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (!CreateProcessW(image_resolved ? application : image_argument, childCommand,
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
    s34_run16_trace("exit",result);
    LocalFree(arguments);
    return (int)result;
}
