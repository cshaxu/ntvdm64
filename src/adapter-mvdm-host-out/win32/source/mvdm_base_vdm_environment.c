/* DIVERGENCE(ADAPTER-WIN32-046): Original BaseCheckVDM called
 * BaseCreateVDMEnvironment in a parent process and passed its result to a new
 * ntvdm.exe child. The selected CLI directly enters original SoftPC in one
 * process. This binding installs the exact original Base-client result only
 * for that interval, then restores the app environment. It never filters or
 * caps variables. */
#include "mvdm_base_vdm_environment.h"

#include <nt.h>
#include <ntrtl.h>

BOOL BaseCreateVDMEnvironment(PWCHAR lpEnvironment, ANSI_STRING *pAStringEnv,
    UNICODE_STRING *pUStringEnv);
BOOL BaseDestroyVDMEnvironment(ANSI_STRING *pAStringEnv,
    UNICODE_STRING *pUStringEnv);

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
        return FALSE;
    }
    ZeroMemory(&ansi, sizeof(ansi));
    ZeroMemory(&unicode, sizeof(unicode));
    status = RtlCreateEnvironment(TRUE, (PVOID *)&environment->original_environment);
    if (!NT_SUCCESS(status)) {
        SetLastError(RtlNtStatusToDosError(status));
        return FALSE;
    }
    if (!BaseCreateVDMEnvironment(NULL, &ansi, &unicode)) goto failed;
    if (!SetEnvironmentStringsW(unicode.Buffer)) goto failed;
    environment->ansi_environment = ansi.Buffer;
    environment->projected_environment = unicode.Buffer;
    environment->installed = TRUE;
    return TRUE;
failed:
    (void)BaseDestroyVDMEnvironment(&ansi, &unicode);
    (void)RtlDestroyEnvironment(environment->original_environment);
    environment->original_environment = NULL;
    return FALSE;
}

void mvdm_base_vdm_environment_restore(mvdm_base_vdm_environment *environment)
{
    ANSI_STRING ansi;
    UNICODE_STRING unicode;
    if (environment == NULL) return;
    ZeroMemory(&ansi, sizeof(ansi));
    ZeroMemory(&unicode, sizeof(unicode));
    ansi.Buffer = environment->ansi_environment;
    unicode.Buffer = environment->projected_environment;
    if (environment->installed) (void)SetEnvironmentStringsW(environment->original_environment);
    (void)BaseDestroyVDMEnvironment(&ansi, &unicode);
    if (environment->original_environment != NULL) (void)RtlDestroyEnvironment(environment->original_environment);
    mvdm_base_vdm_environment_initialize(environment);
}
