#include "wow_task_profile_bindings.h"

static wow_profile_string_provider profile_string;
static wow_profile_int_provider profile_int;

BOOL WINAPI wow_profile_bind(wow_profile_string_provider string_provider,
    wow_profile_int_provider int_provider)
{
    if (!string_provider || !int_provider ||
        (profile_string && (profile_string != string_provider || profile_int != int_provider))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    profile_string = string_provider;
    profile_int = int_provider;
    return TRUE;
}

UINT WINAPI wow_task_profile(wow_task_order_process *process, UINT section,
    LPCWSTR key, LPCWSTR fallback, LPWSTR output, DWORD capacity)
{
    (void)process;
    if (!profile_string) RaiseFailFastException(NULL, NULL, 0);
    return profile_string(section, key, fallback, output, capacity);
}

UINT WINAPI wow_profile_int(UINT section, LPCWSTR key, UINT fallback)
{
    if (!profile_int) RaiseFailFastException(NULL, NULL, 0);
    return profile_int(section, key, fallback);
}
