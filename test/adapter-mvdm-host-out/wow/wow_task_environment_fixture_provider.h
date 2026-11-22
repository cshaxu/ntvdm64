#ifndef WOW_TASK_ENVIRONMENT_FIXTURE_PROVIDER_H
#define WOW_TASK_ENVIRONMENT_FIXTURE_PROVIDER_H
#include "wow_task_lifecycle_fixture_provider.h"
#include "wow_class_client_fixture_provider.h"
#include "compat/include/wow_standard_class_provider.h"
#include "compat/include/wow_task_profile_bindings.h"
#include "compat/include/wow_cleanup_bindings.h"
#include "compat/include/wow_task_cleanup_entry.h"
#include "compat/include/wow_idle_hook_binding.h"
INT WINAPI GetStdClassNumber(LPSTR);
LPCSTR WINAPI wow_original_class_name(UINT);
ATOM WINAPI wow_original_class_atom(UINT);
BOOL WINAPI wow_original_class_capture(LPCSTR, PUNICODE_STRING);
UINT WINAPI wow_original_task_profile(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
extern const wow_cleanup_provider wow_original_cleanup_provider;
BOOL WINAPI wow_original_task_cleanup_uninitialized(DWORD);
VOID WINAPI wow_original_task_retire_async(void);
VOID WINAPI WowOriginalFreeTask(PVOID);
BOOL WINAPI wow_original_idle_registered(DWORD);
BOOL WINAPI wow_original_separate_launch(void);
LRESULT CALLBACK WowOriginalForegroundIdleHook(int, WPARAM, LPARAM);
BOOL __cdecl ntvdm_dpmi_initialize(void);
static BOOL wow_fixture_bind_task_environment(void)
{
    static const wow_standard_class_provider classes = {
        GetStdClassNumber, wow_original_class_name, wow_original_class_atom,
        wow_original_class_capture };
    static const wow_task_cleanup_provider cleanup = {
        wow_original_task_cleanup_uninitialized, wow_original_task_retire_async,
        WowOriginalFreeTask };
    static const wow_task_state_callbacks state = {
        wow_original_idle_registered, wow_original_separate_launch,
        WowOriginalForegroundIdleHook };
    return ntvdm_dpmi_initialize() &&
        wow_task_lifecycle_bind(&wow_fixture_task_lifecycle_provider) &&
        wow_window_access_bind(&wow_fixture_window_access_provider) &&
        wow_class_client_bind(&wow_fixture_class_client_provider) &&
        wow_standard_class_bind(&classes) &&
        wow_profile_bind(wow_original_task_profile, FastGetProfileIntW) &&
        wow_cleanup_bind(&wow_original_cleanup_provider) &&
        wow_task_state_bind(&state) && wow_task_cleanup_bind(&cleanup);
}
#endif
