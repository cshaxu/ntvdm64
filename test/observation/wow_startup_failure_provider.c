/* Diagnostic-only replacement object for the selected original queue.c.
 * Link into a separately labelled WOW32 test package, never release assets.
 * The independently authored GENPROBE input fails the second allocation in
 * original zzzInitTask; all original retry, rollback and termination code runs.
 * No guest image/code/data is altered and no product fault switch is added. */
#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_init_bindings.h"
#include <stdio.h>

static __declspec(thread) unsigned failing_allocation;

static void startup_identity(DWORD task)
{
    char path[MAX_PATH], line[96];
    DWORD error = GetLastError(), size, written;
    HANDLE file;
    size = GetEnvironmentVariableA("MVDM_TEST_STARTUP_IDENTITY", path, sizeof(path));
    if (!size || size >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        size = (DWORD)sprintf_s(line, sizeof(line), "%lu %lu %lu\r\n",
            GetCurrentProcessId(), GetCurrentThreadId(), task);
        WriteFile(file, line, size, &written, NULL);
        CloseHandle(file);
    }
    SetLastError(error);
}

static PVOID WINAPI startup_allocate(SIZE_T bytes)
{
    if (failing_allocation && --failing_allocation == 0) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    return HeapAlloc(GetProcessHeap(), 0, bytes);
}

#undef ExAllocatePoolWithQuota
#define ExAllocatePoolWithQuota(pool, bytes) startup_allocate(bytes)
#define zzzInitTask diagnostic_original_InitTask
#include "../../src/non-mvdm/windows/core/ntuser/kernel/queue.c"
#undef zzzInitTask

NTSTATUS WINAPI zzzInitTask(UINT version, DWORD compat_flags, DWORD user_flags,
    PUNICODE_STRING name, PUNICODE_STRING base_name, DWORD task,
    DWORD hotkey, DWORD id, DWORD x, DWORD y, DWORD width, DWORD height)
{
    static const WCHAR selected[] = L"GENPROBE";
    unsigned saved = failing_allocation;
    NTSTATUS result;
    /* Match the test module's exact counted name, not arbitrary applications
     * or an environment-dependent filename. Preserve nested caller state. */
    failing_allocation = name && name->Buffer &&
        name->Length == sizeof(selected) - sizeof(WCHAR) &&
        !memcmp(name->Buffer, selected, sizeof(selected) - sizeof(WCHAR)) ? 2 : 0;
    if (failing_allocation) startup_identity(LOWORD(task));
    __try {
        result = diagnostic_original_InitTask(version, compat_flags, user_flags, name, base_name,
            task, hotkey, id, x, y, width, height);
    } __finally {
        failing_allocation = saved;
    }
    return result;
}
