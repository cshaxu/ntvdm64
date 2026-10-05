/* run16 is the public CreateProcess CLI composition, never a VDM worker.
 * Image classification and CheckVDM stay in their selected original OpenNT
 * owners. This entry owns executable discovery, broker startup, task
 * submission and broker receipt waits. NTSRV owns worker creation/rollback. */
#include <nt.h>
#include <base_classifier.h>
#include "ntsrv-exe/opennt/include/base_capture.h"
#include "ntsrv-exe/opennt/include/base_client.h"
#include "ntsrv-exe/opennt/include/base_config.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "frontend_scope.h"
#include "launch_options.h"
#include "image_classification.h"
#include "native_launch.h"
#include "common/protocol/console_io.h"
#include "common/system_root.h"
#include "common/guest_environment.h"
#include "common/application_search.h"
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>

UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
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
    DWORD error;
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
    error = common_system_root_w(root, ARRAYSIZE(root));
    if (error) {
        SetLastError(error);
        return FALSE;
    }
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

static BOOL sibling_path(PCWSTR name, PWSTR output, DWORD capacity)
{
    DWORD error = common_product_path_w(name, output, capacity);
    if (error) SetLastError(error);
    return error == ERROR_SUCCESS;
}

static BOOL resolve_image_path(PCWSTR image, PWSTR output, DWORD capacity)
{
    DWORD error=common_resolve_application(image,output,capacity);
    if(error)SetLastError(error);
    return error==ERROR_SUCCESS;
}

static DWORD connect_broker(void)
{
    WCHAR broker[MAX_PATH];
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0};
    DWORD error=ERROR_GEN_FAILURE, attempt;
    if (!sibling_path(L"system32\\ntsrv.exe", broker, MAX_PATH))
        return GetLastError();
    /* A concurrent launcher may own a healthy endpoint, or may be in the
     * broker-only empty-stop window.  Candidate creation is never readiness:
     * an instance losing the endpoint race exits, while a candidate started
     * after the old listener has drained becomes the fresh singleton. */
    for (attempt = 0; attempt < 100; ++attempt)
    {
        error = OpenNtBaseClientConnectCurrent();
        /* Service loss is relevant from admission onward, not only after a
         * worker has started. The shared authenticated process-handle watcher
         * blocks on broker/stop events, with no periodic liveness RPC. */
        if (!error) return OpenNtBaseClientWatchBroker();
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

static DWORD wait_wow_startup(HANDLE parent)
{
    HANDLE ready=NULL,events[2];
    BOOL started=FALSE;
    DWORD error,wait,result=0;
    error=OpenNtBaseClientWowStartup(parent,&ready,&started);
    if (error) return error;
    events[0]=ready; events[1]=parent;
    wait=started ? WAIT_OBJECT_0 : WaitForMultipleObjects(ARRAYSIZE(events),events,FALSE,INFINITE);
    if (wait==WAIT_FAILED) error=GetLastError();
    CloseHandle(ready); ready=NULL;
    if (error) return error;
    /* Completion can race the first query/wait. Re-read the latched result
     * before consuming the original parent result or declaring load failure. */
    error=OpenNtBaseClientWowStartup(parent,&ready,&started);
    if (ready) CloseHandle(ready);
    if (error) return error;
    if (started) return ERROR_SUCCESS;
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
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION worker = {0};
    HANDLE frontend_capability=run16_frontend_scope_capability(frontend_scope);
    ULONG task = 0;
    HANDLE parent_wait;
    DWORD result = ERROR_GEN_FAILURE;
    BOOL published = FALSE;
    BOOL service_start_attempted = FALSE;
    BOOL resumed = FALSE;
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
    if ((binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_WIN16 ||
        (binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_SEPWOW) {
        CHAR root[MAX_PATH],short_root[MAX_PATH];
        PSTR guest=NULL;DWORD bytes=0,length;
        result=common_system_root_a(root,sizeof(root));
        if(result)goto done;
        length=GetShortPathNameA(root,short_root,sizeof(short_root));
        if(!length){result=GetLastError();goto done;}
        if(length>=sizeof(short_root)){result=ERROR_FILENAME_EXCED_RANGE;goto done;}
        result=common_guest_environment_root(environment.Buffer,environment.Length,
            short_root,&guest,&bytes);
        if(result)goto done;
        RtlFreeAnsiString(&environment);
        environment.Buffer=guest;
        environment.Length=(USHORT)bytes;
        environment.MaximumLength=(USHORT)(bytes+1u);
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
        if ((binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS) {
            result=frontend_capability ? OpenNtBaseClientRequestFrontend(frontend_capability) : ERROR_INVALID_STATE;
            if (result && result!=ERROR_ALREADY_EXISTS) { CloseHandle(parent_wait);goto done; }
        }
        if (binary==BINARY_TYPE_WIN16 && !wait_target) {
            result=wait_wow_startup(parent_wait);
            CloseHandle(parent_wait);
            goto done;
        }
        {
            result=run16_wait_direct_event(parent_wait);
            if(!result) {
                if(!BaseCheckForVDM(parent_wait,&result))result=GetLastError();
                else task_completed=TRUE;
            }
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
    /* NTSRV owns exact sibling configuration, reservation, suspended creation,
     * original Update, frontend admission, Resume and startup rollback. The
     * launcher passes only the already-projected environment and show state. */
    service_start_attempted=TRUE;
    result=OpenNtBaseClientStartVdmWorker(unicode_environment.Buffer,
        unicode_environment.Length/sizeof(WCHAR),startup.wShowWindow,
        frontend_capability,&worker.hProcess,&parent_wait);
    if(result)goto done;
    resumed = TRUE;
    if (binary==BINARY_TYPE_WIN16 && !wait_target) {
        result=wait_wow_startup(parent_wait);
        goto waited;
    }
    /* NTSRV observes true worker failure and the original independent-Console
     * worker-exit branch. Both reach this service receipt; run16 never waits
     * on or queries the worker process as a substitute task completion. */
    result=run16_wait_direct_event(parent_wait);
    if(!result) {
        if(!BaseCheckForVDM(parent_wait,&result))result=GetLastError();
        else task_completed=TRUE;
    }
waited:
    if (parent_wait && parent_wait != worker.hProcess)
        CloseHandle(parent_wait);
done:
    end_worker_win16_directory(&win16_directory);
    NtCurrentPeb()->ProcessParameters->ConsoleHandle=saved_console;
    if (published && !service_start_attempted && !resumed && result)
    {
        HANDLE undo_task = (HANDLE)(ULONG_PTR)task;
        ULONG undo_state = VDM_PARTIALLY_CREATED;
        /* Preserve the launch failure.  This is best-effort only because the
         * original record owner may itself report an earlier cleanup fault. */
        (void)BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION, &undo_task, undo_state, binary);
    }
    if (worker.hThread)
        CloseHandle(worker.hThread);
    if (worker.hProcess)
        CloseHandle(worker.hProcess);
    (void)BaseDestroyVDMEnvironment(&environment, &unicode_environment);
    /* Only an acknowledged DOS completion can resume its native parent's
     * presentation. Startup/fault results must not select another worker or
     * be replaced by an unrelated resume error. */
    if (task_completed && (binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS) {
        DWORD handoff=run16_frontend_scope_resume_parent(frontend_scope);
        if(handoff)result=handoff;
        else {
            handoff=run16_frontend_scope_retire(frontend_scope);
            if(handoff)result=handoff;
            else {
                handoff=run16_frontend_scope_restore_parent(frontend_scope);
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

static DWORD launch_native(run16_frontend_scope *scope,PCWSTR application,PCWSTR command,BOOL text,BOOL wait)
{
    run16_native_start start={0};
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
    error=text ? run16_frontend_scope_launch_win32_text(scope,&start) : run16_frontend_scope_launch_win32_gui(scope,&start);
    if(!error && !wait)result=ERROR_SUCCESS;
    if(!error && wait) {
        DWORD target_completed=0;
        DWORD completion=run16_frontend_scope_wait_native(scope,&result,&target_completed);
        /* The direct native target can have completed even when its final
         * presentation fence reports an error. In either case, a root
         * launcher must not hand an outer CMD its Console until NTCON has
         * reselected the canonical buffer and restored input mode. A live target has not
         * completed the handoff and retains the existing failure path. */
        if(text && target_completed) {
            DWORD handoff=run16_frontend_scope_retire(scope);
            if(!handoff) {
                handoff=run16_frontend_scope_restore_parent(scope);
            }
            if(!completion)completion=handoff;
        }
        error=completion;
    }
done:
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
    BOOL image_resolved,initial_console_only,binary_classified;
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
    initial_console_only=GetConsoleProcessList(&console_member,1)==1 && console_member==GetCurrentProcessId();
    RtlInitUnicodeString(&BaseDotComSuffixName, L".com");
    RtlInitUnicodeString(&BaseDotPifSuffixName, L".pif");
    RtlInitUnicodeString(&BaseDotExeSuffixName, L".exe");
    if (!count || !*arguments[0])
        goto done;
    /* The former one-process product accepted a compact first option such as
     * `command/c`: its bare image part and /option were separate argv items.
     * Restore that entry convention for any resolvable bare user image,
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
    binary_classified=OpenNtBaseGetBinaryTypeW(image_resolved ? application : image_argument,&type);
    if(!binary_classified) {
        DWORD subsystem=0;
        /* The retained historical classifier rejects opposite native machine
         * images. Accept only positively verified native SEC_IMAGE metadata;
         * DOS/WOW, shell syntax and failed discovery keep their original path. */
        if(!run16_classify_native_image(image_resolved ? application : image_argument,&subsystem)) {
            type=SCS_32BIT_BINARY;binary_classified=TRUE;
        }
    }
    if (!binary_classified)
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
        if (!result) result=run16_frontend_scope_begin_lease(&frontend_scope,initial_console_only);
        if (result) goto done;
        result=launch_native(frontend_scope,application,shell_command,TRUE,TRUE);
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
        if (!result)
        {
            if ((binary & ~BINARY_SUBTYPE_MASK)==BINARY_TYPE_DOS)
                result=run16_frontend_scope_begin_lease(&frontend_scope,initial_console_only);
            if (!result) result = launch_vdm(binary, application, launch_command,frontend_scope,initial_console_only,options.wait);
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
        DWORD subsystem=0;
        result=run16_classify_native_image(image_resolved ? application : image_argument,&subsystem);
        if(result)goto done;
        if (subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI) {
            result=connect_broker();
            if (!result) result=run16_frontend_scope_begin_lease(&frontend_scope,initial_console_only);
            if (result) goto done;
        }
    }
    if(frontend_scope) {
        result=launch_native(frontend_scope,image_resolved ? application : image_argument,launch_command,TRUE,TRUE);
        goto done;
    }
    result=connect_broker();
    if(!result)result=run16_frontend_scope_begin_gui(&frontend_scope);
    if(!result)result=launch_native(frontend_scope,image_resolved ? application : image_argument,launch_command,FALSE,options.wait);
done:
    run16_frontend_scope_end(frontend_scope);
    OpenNtBaseClientDisconnectCurrent();
    LocalFree(arguments);
    return (int)result;
}
