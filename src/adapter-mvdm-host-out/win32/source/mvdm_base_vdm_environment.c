/* DIVERGENCE(ADAPTER-WIN32-046): Original BaseCheckVDM called
 * BaseCreateVDMEnvironment in a parent process and passed its result to a new
 * ntvdm.exe child. The selected CLI directly enters original SoftPC in one
 * process. This binding installs the exact original Base-client result only
 * for that interval, then restores the app environment. It never filters or
 * caps variables. */
#include "mvdm_base_vdm_environment.h"

#include <nt.h>
#include "opennt-host/base/ntos/rtl/environapi.h"
#include <ntrtl.h>

BOOL BaseCreateVDMEnvironment(PWCHAR lpEnvironment, ANSI_STRING *pAStringEnv,
    UNICODE_STRING *pUStringEnv);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *pAStringEnv,
    UNICODE_STRING *pUStringEnv);

/* DIVERGENCE(ADAPTER-WIN32-026): the private-PEB ingress is the finite
 * standalone binding for the original D11 body.  Keep observation opt-in and
 * host-local: it records only phase/status, never environment bytes. */
static void mvdm_base_vdm_environment_report(const char *phase, DWORD status)
{
    CHAR path[MAX_PATH];
    CHAR line[96];
    HANDLE file;
    DWORD path_bytes;
    DWORD written;
    DWORD saved_error = GetLastError();
    int line_bytes;

    path_bytes = GetEnvironmentVariableA("MVDM_RTL_ENVIRONMENT_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (path_bytes == 0u || path_bytes >= sizeof(path)) goto done;
    line_bytes = wsprintfA(line, "MVDM-RTL-ENV phase=%s status=%08lX\r\n",
        phase, (unsigned long)status);
    if (line_bytes <= 0 || (size_t)line_bytes >= sizeof(line)) goto done;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        (void)WriteFile(file, line, (DWORD)line_bytes, &written, NULL);
        CloseHandle(file);
    }
done:
    SetLastError(saved_error);
}

void mvdm_base_vdm_environment_initialize(mvdm_base_vdm_environment *environment)
{
    if (environment != NULL) ZeroMemory(environment, sizeof(*environment));
}

BOOL mvdm_base_vdm_environment_prepare(mvdm_base_vdm_environment *environment)
{
    ANSI_STRING ansi;
    UNICODE_STRING unicode;
    NTSTATUS status;

    if (environment == NULL || environment->installed) {
        SetLastError(ERROR_INVALID_PARAMETER);
        mvdm_base_vdm_environment_report("prepare-invalid", ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    ZeroMemory(&ansi, sizeof(ansi));
    ZeroMemory(&unicode, sizeof(unicode));
    if (!OpenNtRtlRefreshEnvironmentSnapshot()) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        mvdm_base_vdm_environment_report("refresh-failed", ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    status = RtlCreateEnvironment(TRUE, (PVOID *)&environment->original_environment);
    if (!NT_SUCCESS(status)) {
        SetLastError(RtlNtStatusToDosError(status));
        mvdm_base_vdm_environment_report("clone-failed", (DWORD)status);
        return FALSE;
    }
    if (!BaseCreateVDMEnvironment(NULL, &ansi, &unicode)) goto failed;
    if (!SetEnvironmentStringsW(unicode.Buffer)) goto failed;
    environment->ansi_environment = ansi.Buffer;
    environment->projected_environment = unicode.Buffer;
    environment->installed = TRUE;
    mvdm_base_vdm_environment_report("prepared", ERROR_SUCCESS);
    return TRUE;
failed:
    mvdm_base_vdm_environment_report("projection-failed", GetLastError());
    (void)BaseDestroyVDMEnvironment(&ansi, &unicode);
    (void)RtlDestroyEnvironment(environment->original_environment);
    environment->original_environment = NULL;
    return FALSE;
}

void mvdm_base_vdm_environment_restore(mvdm_base_vdm_environment *environment)
{
    ANSI_STRING ansi;
    UNICODE_STRING unicode;
    BOOL restored = TRUE;
    DWORD restore_status = ERROR_SUCCESS;
    if (environment == NULL) return;
    mvdm_base_vdm_environment_report("restore-begin", ERROR_SUCCESS);
    ZeroMemory(&ansi, sizeof(ansi));
    ZeroMemory(&unicode, sizeof(unicode));
    ansi.Buffer = environment->ansi_environment;
    unicode.Buffer = environment->projected_environment;
    if (environment->installed &&
        !SetEnvironmentStringsW(environment->original_environment)) {
        restored = FALSE;
        restore_status = GetLastError();
    }
    (void)BaseDestroyVDMEnvironment(&ansi, &unicode);
    if (environment->original_environment != NULL) (void)RtlDestroyEnvironment(environment->original_environment);
    mvdm_base_vdm_environment_initialize(environment);
    mvdm_base_vdm_environment_report(restored ? "restored" :
        "restore-failed", restore_status);
}
