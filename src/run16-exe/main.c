/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * Image classification and CheckVDM stay in their selected original OpenNT
 * owners.  This entry owns only executable discovery, broker startup and
 * suspended-worker rollback around the admitted standalone boundary. */
#include <nt.h>
#include <base_classifier.h>
#include "ntsrv-exe/opennt/include/base_capture.h"
#include "ntsrv-exe/opennt/include/base_client.h"
#include "ntsrv-exe/opennt/include/base_config.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "frontend_scope.h"
#include "launch_options.h"
#include "run16-exe/worker_launch.h"
#include "interface/console_io.h"
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
DWORD app_console_probe(void);
PVOID CsrPortHeap;
BOOL BaseCreateVDMEnvironment(PWCHAR environment, ANSI_STRING *ansi,
                              UNICODE_STRING *unicode);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *ansi, UNICODE_STRING *unicode);

typedef struct _WORKER_WIN16DIR_SCOPE {
    PWSTR environment;
} WORKER_WIN16DIR_SCOPE;

/* Original BaseCheckVDM creates the ANSI DOS record and matching Unicode
 * child block through BaseCreateVDMEnvironment. KRNL386 consumes WIN16DIR
 * from the former, so changing only CreateProcess's Unicode block is not
 * sufficient. Build an independent child MULTI_SZ so the original projector
 * receives the derived value without modifying run16's own environment.
 * SYSTEMROOT remains the real host loader identity throughout. */
static BOOL begin_worker_win16_directory(WORKER_WIN16DIR_SCOPE *scope)
{
    WCHAR root[MAX_PATH];
    WCHAR *slash;
    LPWCH current;
    LPWCH cursor;
    PWSTR destination;
    size_t chars = 1u;
    size_t root_chars;
    static const WCHAR name[] = L"WIN16DIR=";

    if (scope == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    ZeroMemory(scope, sizeof(*scope));
    if (!GetModuleFileNameW(NULL, root, ARRAYSIZE(root))) return FALSE;
    slash = wcsrchr(root, L'\\');
    if (slash == NULL || slash == root) {
        SetLastError(ERROR_BAD_PATHNAME);
        return FALSE;
    }
    *slash = L'\0';
    root_chars = wcslen(root);
    current = GetEnvironmentStringsW();
    if (current == NULL) return FALSE;
    for (cursor = current; *cursor != L'\0'; cursor += wcslen(cursor) + 1u) {
        if (_wcsnicmp(cursor, name, ARRAYSIZE(name) - 1u) != 0 &&
            _wcsnicmp(cursor,L"NTVDM_FRONTEND_CAPABILITY=",26)!=0 &&
            _wcsnicmp(cursor,L"NTVDM_EXECUTION_CONSOLE=",24)!=0 &&
            _wcsnicmp(cursor,CONSOLE_COMMAND_STREAMS_WENTRY,wcslen(CONSOLE_COMMAND_STREAMS_WENTRY))!=0)
            chars += wcslen(cursor) + 1u;
    }
    chars += (ARRAYSIZE(name) - 1u) + root_chars + 1u;
    scope->environment = (PWSTR)HeapAlloc(GetProcessHeap(), 0,
        chars * sizeof(*scope->environment));
    if (scope->environment == NULL) {
        FreeEnvironmentStringsW(current);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    destination = scope->environment;
    for (cursor = current; *cursor != L'\0'; cursor += wcslen(cursor) + 1u) {
        size_t entry_chars;
        if (_wcsnicmp(cursor, name, ARRAYSIZE(name) - 1u) == 0) continue;
        if (_wcsnicmp(cursor,L"NTVDM_FRONTEND_CAPABILITY=",26)==0) continue;
        if (_wcsnicmp(cursor,L"NTVDM_EXECUTION_CONSOLE=",24)==0) continue;
        if (_wcsnicmp(cursor,CONSOLE_COMMAND_STREAMS_WENTRY,wcslen(CONSOLE_COMMAND_STREAMS_WENTRY))==0) continue;
        entry_chars = wcslen(cursor) + 1u;
        memcpy(destination, cursor, entry_chars * sizeof(*destination));
        destination += entry_chars;
    }
    memcpy(destination, name, (ARRAYSIZE(name) - 1u) * sizeof(*destination));
    destination += ARRAYSIZE(name) - 1u;
    memcpy(destination, root, (root_chars + 1u) * sizeof(*destination));
    destination += root_chars + 1u;
    *destination = L'\0';
    FreeEnvironmentStringsW(current);
    return TRUE;
}

static void end_worker_win16_directory(WORKER_WIN16DIR_SCOPE *scope)
{
    if (scope == NULL) return;
    if (scope->environment != NULL) HeapFree(GetProcessHeap(), 0,
        scope->environment);
    ZeroMemory(scope, sizeof(*scope));
}

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
    if (!sibling_path(L"ntsrv.exe", broker, MAX_PATH))
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

static DWORD wait_wow_startup(HANDLE parent,HANDLE worker)
{
    HANDLE ready=NULL,events[3];
    BOOL started=FALSE;
    DWORD error,wait,result=0,count=2;
    error=OpenNtBaseClientWowStartup(parent,&ready,&started);
    if (error) return error;
    events[0]=ready; events[1]=parent;
    if (worker) events[count++]=worker;
    wait=started ? WAIT_OBJECT_0 : WaitForMultipleObjects(count,events,FALSE,INFINITE);
    if (wait==WAIT_FAILED) error=GetLastError();
    CloseHandle(ready); ready=NULL;
    if (error) return error;
    /* Completion can race the first query/wait. Re-read the latched result
     * before consuming the original parent result or declaring load failure. */
    error=OpenNtBaseClientWowStartup(parent,&ready,&started);
    if (ready) CloseHandle(ready);
    if (error) return error;
    if (started) return ERROR_SUCCESS;
    if (worker && WaitForSingleObject(worker,0)==WAIT_OBJECT_0)
        return ERROR_PROCESS_ABORTED;
    if (WaitForSingleObject(parent,0)!=WAIT_OBJECT_0) return ERROR_INVALID_STATE;
    if (!BaseCheckForVDM(parent,&result)) return GetLastError();
    /* Shared WOW's original zero completion is not a successful InitTask.
     * No exact guest LoadModule error is available at this interface. */
    return result ? result : ERROR_DLL_INIT_FAILED;
}

static DWORD launch_vdm(ULONG binary, PCWSTR application, PCWSTR command,run16_frontend_scope *frontend_scope,BOOL initial_console_only,BOOL wait_target)
{
    BASE_API_MSG message = {0};
    ANSI_STRING environment = {0};
    UNICODE_STRING unicode_environment = {0};
    WORKER_WIN16DIR_SCOPE win16_directory = {0};
    UNICODE_STRING worker_command = {0};
    OPENNT_BASE_VDM_CONFIG configuration;
    const OPENNT_BASE_VDM_CONFIG *previous_configuration;
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION worker = {0};
    HANDLE frontend_capability=run16_frontend_scope_capability(frontend_scope);
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
    DWORD worker_creation_flags;
    BOOL prepared = FALSE;
    BOOL published = FALSE;
    BOOL registered = FALSE;
    BOOL resumed = FALSE;
    BOOL startup_failed = FALSE;
    BOOL task_completed = FALSE;
    /* A Console-subsystem launcher started by Explorer already has a new
     * Console. Original CreateProcess classified this as a new DOS session
     * before that Console existed. Preserve its session/CloseOnExit path,
     * while letting the worker use the Console already allocated for us.
     * A CMD or nested caller is another attached process and keeps the
     * original shared-Console resident-worker path. */
    BOOL launcher_console_only=(binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS &&
        initial_console_only && !run16_frontend_scope_has_execution(frontend_scope);
    DWORD check_creation_flags=(binary & BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS_PIF ?
        CREATE_NEW_CONSOLE : 0;
    HANDLE saved_console=NtCurrentPeb()->ProcessParameters->ConsoleHandle;
    if (launcher_console_only) check_creation_flags |= CREATE_NEW_CONSOLE;
    /* Original BaseCheckVDM reads the private NT4 PEB projection, not the
     * broker connection. An authenticated execution context is an existing
     * Console identity even when this launcher has no native Console. */
    if (run16_frontend_scope_has_execution(frontend_scope))
        NtCurrentPeb()->ProcessParameters->ConsoleHandle=(HANDLE)1;

    /* This is the original parent-side VDM environment projection.  The
     * ANSI record is captured by BaseCheckVDM; the matching Unicode record
     * is passed unchanged to the newly-created worker. */
    if (!begin_worker_win16_directory(&win16_directory))
    {
        result = GetLastError();
        goto done;
    }
    if (!BaseCreateVDMEnvironment(win16_directory.environment, &environment,
            &unicode_environment))
    {
        result = GetLastError();
        end_worker_win16_directory(&win16_directory);
        goto done;
    }
    end_worker_win16_directory(&win16_directory);
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
        s34_run16_trace("check-failed",result);
        goto done;
    }
    s34_run16_trace("check-flags",check_creation_flags);
    s34_run16_trace("vdm-state",message.u.CheckVDM.VDMState);
    s34_run16_trace("vdm-task",task);
    /* srvvdm.c has already selected and queued a same-Console resident DOS
     * record.  Its original Check reply carries the parent completion event;
     * wait and query the original exit-code route, never create another VDM. */
    if (message.u.CheckVDM.VDMState == VDM_PRESENT_AND_READY)
    {
        parent_wait = message.u.CheckVDM.WaitObjectForParent;
        if ((binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS) {
            result=frontend_capability ? OpenNtBaseClientRequestFrontend(frontend_capability) : ERROR_INVALID_STATE;
            s34_run16_trace("reuse-frontend",result);
            if (result && result!=ERROR_ALREADY_EXISTS) { CloseHandle(parent_wait);goto done; }
        }
        result=OpenNtBaseClientWatchBroker();
        if (result) { CloseHandle(parent_wait); goto done; }
        if (binary==BINARY_TYPE_WIN16 && !wait_target) {
            result=wait_wow_startup(parent_wait,NULL);
            CloseHandle(parent_wait);
            goto done;
        }
        {
            DWORD wait=WaitForSingleObject(parent_wait,INFINITE);
            if (wait!=WAIT_OBJECT_0 || !BaseCheckForVDM(parent_wait,&result))
                result=GetLastError();
            else task_completed=TRUE;
        }
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
    worker_creation_flags=CREATE_UNICODE_ENVIRONMENT;
    if (binary==BINARY_TYPE_WIN16 || binary==BINARY_TYPE_SEPWOW) {
        /* Original OpenNT base/win32/client/process.c starts WOW with
         * CREATE_NO_WINDOW, not an inherited or newly visible Console.
         * Leave the guest command's startup/show state unchanged. */
        worker_creation_flags |= CREATE_NO_WINDOW;
    } else {
        /* DOS I/O belongs to the authenticated frontend, not this worker's
         * Windows Console membership. The original execution Console/task
         * identity was already captured by CheckVDM and the reservation.
         * Inheriting a native hidden Console would count an idle DOS worker
         * as a native user and prevent that frontend from retiring. */
        worker_creation_flags |= DETACHED_PROCESS;
    }
    result = run16_worker_prepare(reservation, worker_path, worker_command.Buffer,
        unicode_environment.Buffer, worker_creation_flags, &startup, &worker);
    if (result)
        goto done;
    prepared = TRUE;
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
    if (binary==BINARY_TYPE_DOS) {
        result=frontend_capability ? OpenNtBaseClientRequestFrontend(frontend_capability) : ERROR_INVALID_STATE;
        s34_run16_trace("new-frontend",result);
        if (result && result!=ERROR_ALREADY_EXISTS) goto done;
    }
    if (ResumeThread(worker.hThread) == (DWORD)-1)
    {
        result = GetLastError();
        goto done;
    }
    resumed = TRUE;
    result=OpenNtBaseClientWatchBroker();
    if (result) {
        goto waited;
    }
    if (binary==BINARY_TYPE_WIN16 && !wait_target) {
        result=wait_wow_startup(parent_wait,worker.hProcess);
        goto waited;
    }
    /* A dead worker cannot deliver another completion. Keep original task
     * results, but never return to an infinite event wait after process exit. */
    {
        HANDLE completion[2]={parent_wait ? parent_wait : worker.hProcess,NULL};
        DWORD code,count=1,worker_index=0,wait;
        if (completion[0]!=worker.hProcess) {
            worker_index=count;completion[count++]=worker.hProcess;
        }
        wait=WaitForMultipleObjects(count,completion,FALSE,INFINITE);
        if (wait==WAIT_FAILED) {
            result=GetLastError();
            goto waited;
        }
        if (worker_index && wait==WAIT_OBJECT_0+worker_index &&
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
        else task_completed=TRUE;
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
    end_worker_win16_directory(&win16_directory);
    NtCurrentPeb()->ProcessParameters->ConsoleHandle=saved_console;
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
    s34_run16_trace("task-completed",task_completed);
    /* Only an acknowledged DOS completion can resume its native parent's
     * presentation. Startup/fault results must not select another worker or
     * be replaced by an unrelated resume error. */
    if (task_completed && (binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS) {
        DWORD handoff=run16_frontend_scope_resume_parent(frontend_scope);
        s34_run16_trace("native-resume",handoff);
        if(handoff)result=handoff;
        else {
            handoff=run16_frontend_scope_retire(frontend_scope);
            s34_run16_trace("frontend-retire",handoff);
            if(handoff)result=handoff;
            else {
                handoff=run16_frontend_scope_restore_parent(frontend_scope);
                s34_run16_trace("frontend-restored",handoff);
                if(handoff)result=handoff;
            }
        }
    }
    return result;
}

static BOOL WINAPI launcher_control(DWORD event)
{
    /* The child/VDM receives the same Console event and owns its response.
     * Keep the parent wait alive to return that task's actual completion.
     * A handler (unlike the NULL ignore attribute) is not inherited. */
    return event == CTRL_C_EVENT || event == CTRL_BREAK_EVENT;
}

/* Native resource materialization is shared with NTCON, not its execution loop. */
static DWORD launch_gui(PCWSTR application,PCWSTR command,BOOL wait)
{
    WCHAR directory[MAX_PATH];
    LPWCH environment=NULL;
    run16_native_start start={0};
    PROCESS_INFORMATION child={0};
    BYTE *packet=NULL;
    DWORD bytes,error,result;
    if(!GetCurrentDirectoryW(ARRAYSIZE(directory),directory))return GetLastError();
    environment=GetEnvironmentStringsW();if(!environment)return GetLastError();
    start.application=application;start.command=command;
    start.directory=directory;start.environment=environment;
    start.standard[0]=GetStdHandle(STD_INPUT_HANDLE);
    start.standard[1]=GetStdHandle(STD_OUTPUT_HANDLE);
    start.standard[2]=GetStdHandle(STD_ERROR_HANDLE);
    /* Reuse restricted handle/environment materialization, but no frontend
     * or execution capability: a GUI segment ends character-session routing. */
    error=run16_native_launch_pack(&start,&packet,&bytes);
    if(!error)error=run16_native_launch_start(packet,bytes,&child);
    if(!error){
        CloseHandle(child.hThread);
        if(wait) {
            if(WaitForSingleObject(child.hProcess,INFINITE)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(child.hProcess,&result))error=GetLastError();
            else error=result;
        }
        CloseHandle(child.hProcess);
    }
    if(packet)HeapFree(GetProcessHeap(),0,packet);
    FreeEnvironmentStringsW(environment);
    return error;
}
static DWORD launch_native(run16_frontend_scope *scope,PCWSTR application,PCWSTR command)
{
    run16_native_start start={0};
    HANDLE target=NULL;
    WCHAR directory[32768];
    PWSTR environment=NULL;
    DWORD error,result=ERROR_PROCESS_ABORTED,mode,i,length;
    if(!scope)return ERROR_INVALID_PARAMETER;
    length=GetCurrentDirectoryW(ARRAYSIZE(directory),directory);
    if(!length || length>=ARRAYSIZE(directory))return ERROR_PATH_NOT_FOUND;
    environment=GetEnvironmentStringsW();
    if(!environment) { error=GetLastError();goto done; }
    start.application=application;start.command=command;
    start.directory=directory;start.environment=environment;
    start.standard[0]=GetStdHandle(STD_INPUT_HANDLE);
    start.standard[1]=GetStdHandle(STD_OUTPUT_HANDLE);
    start.standard[2]=GetStdHandle(STD_ERROR_HANDLE);
    start.console_mask=run16_frontend_scope_console_mask(scope);
    for(i=0;i<3;++i)if(GetConsoleMode(start.standard[i],&mode))start.console_mask|=1u<<i;
    error=run16_frontend_scope_launch_native(scope,&start,&target);
    s34_run16_trace("native-submit",error);
    if(!error) {
        DWORD completion=run16_frontend_scope_wait_native(scope,target,&result);
        /* The direct native target can have completed even when its final
         * presentation fence reports an error. In either case, a root
         * launcher must not hand an outer CMD its Console until NTKVM has
         * restored the original buffer/input mode. A live target has not
         * completed the handoff and retains the existing failure path. */
        if(target && WaitForSingleObject(target,0)==WAIT_OBJECT_0) {
            DWORD handoff=run16_frontend_scope_retire(scope);
            s34_run16_trace("frontend-retire",handoff);
            if(!handoff) {
                handoff=run16_frontend_scope_restore_parent(scope);
                s34_run16_trace("frontend-restored",handoff);
            }
            if(!completion)completion=handoff;
        }
        error=completion;
    }
done:
    if(target)CloseHandle(target);
    if(environment)FreeEnvironmentStringsW(environment);
    return error ? error : result;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    LPWSTR *arguments;
    PCWSTR image_argument;
    PCWSTR launch_command;
    PCWSTR option, tail;
    run16_frontend_scope *frontend_scope=NULL;
    run16_launch_options options;
    WCHAR application[MAX_PATH];
    WCHAR split_image[MAX_PATH];
    WCHAR normalized_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
    WCHAR shell_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
    DWORD type, result = ERROR_INVALID_PARAMETER, binary = 0, comspec_bytes;
    BOOL image_resolved,initial_console_only;
    DWORD console_member;
    int count;
    (void)instance;
    (void)previous;
    (void)show;
    if (!run16_parse_launch_options(command,&options))
    {
        fputs("Usage: run16.exe [--wait] [--] <binary> [arguments]\n", stderr);
        return ERROR_INVALID_PARAMETER;
    }
    command=(PWSTR)options.command;
    /* Decode only to identify the target; original BaseCheckVDM receives the
     * untouched tail and supplies its original OEM command representation. */
    arguments = CommandLineToArgvW(command, &count);
    if (!arguments)
        return (int)GetLastError();
    if (!SetConsoleCtrlHandler(launcher_control, TRUE))
    {
        result = GetLastError();
        goto done;
    }
    s34_run16_trace("args",(DWORD)count);
    if (count == 1 && !wcscmp(arguments[0], L"--internal-console-probe"))
    {
        LocalFree(arguments);
        return (int)app_console_probe();
    }
    initial_console_only=GetConsoleProcessList(&console_member,1)==1 && console_member==GetCurrentProcessId();
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
        result=connect_broker();
        if (!result) result=run16_frontend_scope_begin(&frontend_scope);
        if (!result) result=OpenNtBaseClientWatchBroker();
        if (result) goto done;
        result=launch_native(frontend_scope,application,shell_command);
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
        /* WOW windows need no character Console. Release only our initially
         * exclusive launcher Console, never an inherited CMD/frontend one.
         * Capture Console stream identities before detaching: FreeConsole
         * invalidates those handles, but redirected files/pipes must survive. */
        if (binary==BINARY_TYPE_WIN16 && initial_console_only &&
            GetConsoleProcessList(&console_member,1)==1 && console_member==GetCurrentProcessId())
        {
            const DWORD streams[3]={STD_INPUT_HANDLE,STD_OUTPUT_HANDLE,STD_ERROR_HANDLE};
            BOOL console_stream[3]; DWORD i,mode;
            for(i=0;i<3;++i)console_stream[i]=GetConsoleMode(GetStdHandle(streams[i]),&mode);
            if(!FreeConsole()) { result=GetLastError();goto done; }
            for(i=0;i<3;++i)if(console_stream[i] && !SetStdHandle(streams[i],NULL)) {
                result=GetLastError();goto done;
            }
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
            if ((binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS)
                result=run16_frontend_scope_begin(&frontend_scope);
            if (!result) result = launch_vdm(binary, application, launch_command,frontend_scope,initial_console_only,options.wait);
            s34_run16_trace("worker",result);
        }
        run16_frontend_scope_end(frontend_scope);frontend_scope=NULL;
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
    /* Original vdm.c obtains the subsystem from an image section. Reuse that
     * OS metadata contract for frontend selection; do not parse PE headers. */
    {
        HANDLE file=CreateFileW(image_resolved ? application : image_argument,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,
            NULL,OPEN_EXISTING,0,NULL),section=NULL;
        SECTION_IMAGE_INFORMATION information={0};
        NTSTATUS status;
        if (file==INVALID_HANDLE_VALUE) { result=GetLastError();goto done; }
        status=NtCreateSection(&section,SECTION_QUERY,NULL,NULL,PAGE_READONLY,SEC_IMAGE,file);
        CloseHandle(file);
        if (NT_SUCCESS(status)) {
            status=NtQuerySection(section,SectionImageInformation,&information,sizeof(information),NULL);
            CloseHandle(section);
        }
        if (!NT_SUCCESS(status)) { result=RtlNtStatusToDosError(status);goto done; }
        if (information.SubSystemType==IMAGE_SUBSYSTEM_WINDOWS_CUI) {
            result=connect_broker();
            s34_run16_trace("native-broker",result);
            if (!result) result=run16_frontend_scope_begin(&frontend_scope);
            s34_run16_trace("native-frontend",result);
            if (!result) result=OpenNtBaseClientWatchBroker();
            if (result) goto done;
        }
    }
    if(frontend_scope) {
        result=launch_native(frontend_scope,image_resolved ? application : image_argument,launch_command);
        goto done;
    }
    result=launch_gui(image_resolved ? application : image_argument,launch_command,options.wait);
done:
    run16_frontend_scope_end(frontend_scope);
    OpenNtBaseClientDisconnectCurrent();
    s34_run16_trace("exit",result);
    LocalFree(arguments);
    return (int)result;
}
