#include "app/machine_shell.h"
#include "app/launch_declaration.h"
#include "app/package_layout.h"
#include "app/presentation_window.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_termination.h"

#include <stdio.h>
#include <windows.h>

/* App owns these process-assembly outcomes.  They deliberately do not
 * describe an original SoftPC/guest result: a result returned by the
 * original entry remains untouched below.  Keeping the early outcomes
 * distinct lets the fixed non-debug launch container identify whether the
 * product reached original host startup without changing console ownership,
 * debugger state, or launch arguments. */
enum app_startup_status {
    APP_STARTUP_OPTIONS_REJECTED = 64,
    APP_STARTUP_MEDIA_REJECTED = 65,
    APP_STARTUP_MACHINE_REJECTED = 66,
    APP_STARTUP_SESSION_REJECTED = 67,
    APP_STARTUP_DECLARATION_REJECTED = 68,
    APP_STARTUP_COMMAND_REJECTED = 69,
    APP_STARTUP_SHELL_REJECTED = 70,
    APP_STARTUP_MACHINE_FAILURE = 71,
    APP_STARTUP_DISPOSE_FAILURE = 72
};

static void app_report_media_root_rejected(void)
{
    MessageBoxA(NULL,
        "NTVDM64 cannot start from this package location.\r\n\r\n"
        "The original NTDOS COMMAND startup buffer accepts at most 63 "
        "characters for its generated shell path. Install or move the "
        "package so its root directory has a shorter Windows path, then "
        "start NTVDM64 again.",
        "NTVDM64 package path too long", MB_OK | MB_ICONERROR);
}

static const char *app_dispose_reason_name(uint32_t reason)
{
    switch (reason) {
    case SESSION_DISPOSE_REASON_INVALID:
        return "invalid";
    case SESSION_DISPOSE_REASON_BINDING_COUNT:
        return "binding-count";
    case SESSION_DISPOSE_REASON_TERMINATION_ARMED:
        return "termination-armed";
    default:
        return "none";
    }
}

static void app_record_dispose_failure(const session *owner, uint32_t reason)
{
    session_binding_diagnostic binding;
    char path[MAX_PATH];
    char message[256];
    DWORD path_bytes;
    DWORD written;
    HANDLE output;
    int formatted;

    /* Default-off fixed-container observation only.  It neither changes the
     * dispose result nor attempts recovery; the report contains no guest,
     * MVDM, host-handle or pointer state. */
    path_bytes = GetEnvironmentVariableA("MVDM_SESSION_DISPOSE_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (path_bytes == 0u || path_bytes >= sizeof(path)) return;
    if (!session_binding_diagnostic_snapshot(owner, &binding)) return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-SESSION-DISPOSE reason=%s code=%lu total=%lu entry=%lu worker=%lu unspecified=%lu worker-source=%s\r\n",
        app_dispose_reason_name(reason), (unsigned long)reason,
        (unsigned long)binding.total, (unsigned long)binding.softpc_entry,
        (unsigned long)binding.original_worker,
        (unsigned long)binding.unspecified,
        binding.original_worker_source[0] != '\0' ?
            binding.original_worker_source : "unattributed");
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    output = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(output, message, (DWORD)formatted, &written, NULL);
    CloseHandle(output);
}

/* The application owns only process/session assembly.  Guest loading,
 * host initialization and CPU execution stay in the original SoftPC entry
 * selected by app_machine_shell_run(). */
int main(int argc, char **argv)
{
    app_machine_shell shell;
    app_launch_declaration declaration;
    app_presentation_window presentation;
    session owner;
    uint32_t dispose_reason;
    int result = 1;

    session_initialize(&owner, 1u);
    app_machine_shell_initialize(&shell);
    app_launch_declaration_initialize(&declaration);
    app_presentation_window_initialize(&presentation);
    /* Capture default-off host diagnostics before original cmdenv.c obtains
     * inherited process variables for the guest DOS environment. */
    mvdm_softpc_capture_command_continuation_report_path();
    if (!app_launch_declaration_consume_options(&declaration, &argc, argv)) {
        result = APP_STARTUP_OPTIONS_REJECTED;
        goto finish;
    }
    if (!app_package_layout_set_process_media_roots(&owner)) {
        result = APP_STARTUP_MEDIA_REJECTED;
        goto finish;
    }
    if (!app_package_layout_validate_command_configuration_root(&owner)) {
        app_report_media_root_rejected();
        result = APP_STARTUP_MEDIA_REJECTED;
        goto finish;
    }
    if (!app_machine_shell_select_backend(&owner,
            SESSION_MACHINE_BACKEND_NONE)) {
        result = APP_STARTUP_MACHINE_REJECTED;
        goto finish;
    }
    /* The optional app presentation surface is opened while session is still
     * ready so its event sink can bind without changing original SoftPC
     * startup. Failure deliberately falls back to the existing console-only
     * composition and never changes guest or controller state. */
    if (app_presentation_window_prepare(&presentation, &owner) &&
        app_presentation_window_open(&presentation)) {
        /* The selected app-owned public window is now ready before source
         * graphics output can invalidate its DIB. */
    } else {
        (void)app_presentation_window_close(&presentation);
        (void)session_set_video_event_sink(&owner, NULL, NULL);
    }
    if (!session_activate(&owner)) {
        result = APP_STARTUP_SESSION_REJECTED;
        goto finish;
    }
    /* The adapter owns the copied Base VDM/VDMINFO contract.  App only binds
     * the session-local declaration before the original entry asks whether it
     * is the first DOS VDM; it neither loads guest bytes nor supplies a guest
     * lifecycle. */
    if (!app_launch_declaration_bind(&declaration, &owner)) {
        result = APP_STARTUP_DECLARATION_REJECTED;
        goto finish;
    }
    if (declaration.command_declared != 0u &&
        !app_launch_declaration_publish(&declaration, &owner)) {
        result = APP_STARTUP_COMMAND_REJECTED;
        goto finish;
    }

    /* The current shell ABI validates nonzero capacity arguments but does
     * not consume them.  Original SoftPC owns machine initialization. */
    if (app_machine_shell_open(&shell, &owner, 1u, UINT64_C(1)) !=
            APP_MACHINE_SHELL_OK) {
        result = APP_STARTUP_SHELL_REJECTED;
        goto finish;
    }
    if (app_machine_shell_run(&shell, argc, argv, &result) !=
            APP_MACHINE_SHELL_OK) {
        result = APP_STARTUP_MACHINE_FAILURE;
    }

finish:
    (void)app_presentation_window_close(&presentation);
    if (!session_dispose_with_reason(&owner, &dispose_reason)) {
        app_record_dispose_failure(&owner, dispose_reason);
        return APP_STARTUP_DISPOSE_FAILURE;
    }
    return result;
}
