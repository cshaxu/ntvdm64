/* ADAPTER-WOW-079: original bodies retain all callback/thunk algorithms.
 * This file pairs only the existing native-stack execution boundaries. */
#include "precomp.h"
#include "wow_user_native_extension.h"
#include "wow_user_runtime.h"
#include "wow_user_native_call.h"

BOOL WINAPI wow_original_CallBack16(INT, PPARM16, VPPROC, PVPVOID);
DWORD FASTCALL wow_original_WK32ICallProc32(PVDMFRAME);
ULONG FASTCALL wow_original_WMM32CallProc32(PVDMFRAME);
VOID WINAPI wow_original_WOWYield16(VOID);
VOID WINAPI wow_original_WOWDirectedYield16(WORD);

/* These exact original wow16cal entries invoke kernel memory/resource
 * services, not an application procedure. This admits no numeric range,
 * arbitrary public WOWCallback16, Hook, foreground-idle or WinExec during
 * teardown. The existing scope still requires resource_cleanup_live. */
static BOOL kernel_resource_return(INT id)
{
    switch (id) {
    case RET_LOCALALLOC: case RET_LOCALREALLOC: case RET_LOCALLOCK:
    case RET_LOCALUNLOCK: case RET_LOCALSIZE: case RET_LOCALFREE:
    case RET_GLOBALALLOCLOCK: case RET_GLOBALLOCK: case RET_GLOBALUNLOCK:
    case RET_GLOBALUNLOCKFREE: case RET_FINDRESOURCE: case RET_LOADRESOURCE:
    case RET_FREERESOURCE: case RET_LOCKRESOURCE: case RET_UNLOCKRESOURCE:
    case RET_SIZEOFRESOURCE: case RET_LOCKSEGMENT: case RET_UNLOCKSEGMENT:
    case RET_GETEXEPTR: case RET_GETEXPWINVER: case RET_GETMODULEFILENAME:
    case RET_GETDOSPDB: case RET_GETDOSSFT: case RET_CHANGESELECTOR:
    case RET_FORCESEGMENTFAULT: case RET_GETDIBSIZE: case RET_GETDIBFLAGS:
    case RET_SETDIBSEL: case RET_FREEDIBSEL: case RET_SETCURSORICONFLAG:
    case RET_WOWDDEFREEHANDLE:
    /* wow16cal.asm delegates only to kernel directory/drive services;
     * no application procedure or process launch is selected by these IDs. */
    case RET_GETCURDIR: case RET_SETCURDIR:
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL CallBack16(INT id, PPARM16 parameters, VPPROC procedure, PVPVOID output)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_callback_scope scope;
    BOOL result;
    DWORD error;
    BOOL cleanup = binding && binding->thread &&
        (binding->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP);
    if (!(cleanup && kernel_resource_return(id) ?
            wow_task_resource_cleanup_enter(&scope) : wow_task_callback_enter(&scope)))
        return FALSE;
    __try { result = wow_original_CallBack16(id, parameters, procedure, output); }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
    return result;
}

/* Inline entry never gratuitously releases inherited guest execution before
 * the original raw arguments are copied. Actual callback/yield entry may
 * acquire execution; this is exactly when the original allows memory moves. */
#define EXTENSION_CALL(type, convention, name, declaration, original, arguments, failed) \
type convention name declaration \
{ \
    wow_user_native_call call; \
    type result; \
    DWORD error = GetLastError(); \
    if (!wow_user_native_call_begin_inline(&call)) return failed; \
    __try { SetLastError(error); result = original arguments; } \
    __finally { \
        error = GetLastError(); \
        if (!wow_user_native_call_end(&call)) \
            wow_user_native_call_failed("native-extension-return", GetLastError()); \
        SetLastError(error); \
    } \
    return result; \
}
EXTENSION_CALL(DWORD, FASTCALL, WK32ICallProc32, (PVDMFRAME frame),
    wow_original_WK32ICallProc32, (frame), 0)
EXTENSION_CALL(ULONG, FASTCALL, WMM32CallProc32, (PVDMFRAME frame),
    wow_original_WMM32CallProc32, (frame), 0)
EXTENSION_CALL(HMODULE, WINAPI, wow_extension_load_library_a,
    (LPCSTR name, HANDLE file, DWORD flags), LoadLibraryExA, (name, file, flags), NULL)
EXTENSION_CALL(BOOL, WINAPI, wow_extension_free_library, (HMODULE module),
    FreeLibrary, (module), FALSE)
EXTENSION_CALL(FARPROC, WINAPI, wow_extension_get_proc_address,
    (HMODULE module, LPCSTR name), GetProcAddress, (module, name), NULL)
#undef EXTENSION_CALL

VOID WINAPI WOWYield16(VOID)
{
    wow_task_callback_scope scope;
    DWORD error;
    if (!wow_task_callback_enter(&scope)) return;
    __try { wow_original_WOWYield16(); }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
}

VOID WINAPI WOWDirectedYield16(WORD task)
{
    wow_task_callback_scope scope;
    DWORD error;
    if (!wow_task_callback_enter(&scope)) return;
    __try { wow_original_WOWDirectedYield16(task); }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
}
