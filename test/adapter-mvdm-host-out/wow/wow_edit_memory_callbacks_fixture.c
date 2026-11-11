#include "wow_edit_state_bindings.h"
#include <stdio.h>

/* Supplemental argument-boundary fixture. The real W32 callbacks and guest
 * memory remain required by the production EDIT group; these mocks prove
 * only that original macros reach the existing registration unchanged. */
static unsigned failures, calls;
static BYTE text[8];
static BOOL reject;
static HANDLE seen_module;
static unsigned native_word_calls, wow_word_calls;
static struct { DWORD dwExpWinVer; } test_client_info;
#define GetClientInfo() (&test_client_info)
#define INSTANCE ((HANDLE)(ULONG_PTR)0x45671234)
#define MEMORY ((HLOCAL)(ULONG_PTR)0x56782345)
#define RESIZED ((HLOCAL)(ULONG_PTR)0x56783456)
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)

static int CALLBACK native_word_break(LPSTR buffer, int start, int count, int action)
{
    ++calls; ++native_word_calls;
    CHECK(buffer == (LPSTR)text && start == 2 && count == 7 && action == WB_LEFT);
    return -27;
}

static void exercise_original_macros(void)
{
    PVOID pointer = NULL;
    EDITWORDBREAKPROCA native_proc = native_word_break;
    EDITWORDBREAKPROCA wow_proc = (EDITWORDBREAKPROCA)(ULONG_PTR)0x81234567;
    CHECK(LOCALALLOC(LHND, 123, INSTANCE) == MEMORY);
    CHECK(LOCALREALLOC(MEMORY, 456, LMEM_MOVEABLE, INSTANCE, &pointer) == RESIZED);
    CHECK(pointer == text);
    CHECK(LOCALLOCK(MEMORY, INSTANCE) == text);
    CHECK(LOCALUNLOCK(MEMORY, INSTANCE));
    CHECK(LOCALSIZE(MEMORY, INSTANCE) == 789);
    CHECK(LOCALFREE(MEMORY, INSTANCE) == NULL);
    reject = TRUE;
    CHECK(LOCALALLOC(LHND, 123, INSTANCE) == NULL);
    CHECK(LOCALREALLOC(MEMORY, 456, LMEM_MOVEABLE, INSTANCE, NULL) == NULL);
    CHECK(LOCALLOCK(MEMORY, INSTANCE) == NULL);
    CHECK(!LOCALUNLOCK(MEMORY, INSTANCE));
    CHECK(LOCALSIZE(MEMORY, INSTANCE) == 0);
    CHECK(LOCALFREE(MEMORY, INSTANCE) == MEMORY);
    CHECK(GETEXPWINVER(INSTANCE) == 0x1234030a);
    CHECK(seen_module == INSTANCE);
    CHECK(GETEXPWINVER(NULL) == 0x1234030a);
    CHECK(seen_module == GetModuleHandle(NULL));
    test_client_info.dwExpWinVer = 0x56780400;
    CHECK(GetAppVer(NULL) == 0x56780400);
    CHECK(seen_module == GetModuleHandle(NULL)); /* No module callback. */
    CHECK(((DWORD)(ULONG_PTR)native_proc & WNDPROC_WOW) == 0);
    if (!((DWORD)(ULONG_PTR)native_proc & WNDPROC_WOW)) {
        CHECK(CALLWORDBREAKPROC(native_proc, (LPSTR)text, 2, 7, WB_LEFT) == -27);
        CHECK(native_word_calls == 1 && wow_word_calls == 0);
    }
    CHECK(CALLWORDBREAKPROC(wow_proc, (LPSTR)text, 3, 6, WB_RIGHT) == 31);
    CHECK(native_word_calls == 1 && wow_word_calls == 1);
}

/* The consumer above expands the original variable-name bindings. Below,
 * refer to actual table members without those private consumer macros. */
#undef pfnLocalAlloc
#undef pfnLocalReAlloc
#undef pfnLocalLock
#undef pfnLocalUnlock
#undef pfnLocalSize
#undef pfnLocalFree
#undef pfnGetExpWinVer
#undef pfnWowEditNextWord
#undef pfn16GlobalAlloc
#undef pfn16GlobalFree
static PFNWOWHANDLERSIN callbacks;
static unsigned dialog_calls;
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void)
{
    return &callbacks;
}
static HLOCAL WINAPI allocate(UINT flags, UINT bytes, HANDLE instance)
{
    ++calls;
    CHECK(flags == LHND && bytes == 123 && instance == INSTANCE);
    return reject ? NULL : MEMORY;
}
static HLOCAL WINAPI reallocate(HLOCAL memory, UINT bytes, UINT flags,
    HANDLE instance, PVOID *pointer)
{
    ++calls;
    CHECK(memory == MEMORY && bytes == 456 && flags == LMEM_MOVEABLE && instance == INSTANCE);
    CHECK(reject ? pointer == NULL : pointer != NULL);
    if (pointer) *pointer = text;
    return reject ? NULL : RESIZED;
}
static LPVOID WINAPI lock_memory(HLOCAL memory, HANDLE instance)
{
    ++calls; CHECK(memory == MEMORY && instance == INSTANCE);
    return reject ? NULL : text;
}
static BOOL WINAPI unlock_memory(HLOCAL memory, HANDLE instance)
{
    ++calls; CHECK(memory == MEMORY && instance == INSTANCE);
    return !reject;
}
static UINT WINAPI size_memory(HLOCAL memory, HANDLE instance)
{
    ++calls; CHECK(memory == MEMORY && instance == INSTANCE);
    return reject ? 0 : 789;
}
static HLOCAL WINAPI free_memory(HLOCAL memory, HANDLE instance)
{
    ++calls; CHECK(memory == MEMORY && instance == INSTANCE);
    return reject ? MEMORY : NULL;
}
static DWORD WINAPI module_version(HANDLE module)
{
    ++calls;
    seen_module = module;
    return 0x1234030a;
}
static int WINAPI wow_word_break(LPSTR buffer, int start, int count, int action,
    DWORD procedure)
{
    ++calls; ++wow_word_calls;
    CHECK(buffer == (LPSTR)text && start == 3 && count == 6 && action == WB_RIGHT);
    CHECK(procedure == 0x81234567);
    return 31;
}
static WORD WINAPI allocate_dialog_ds(UINT flags, DWORD bytes)
{
    ++dialog_calls;
    CHECK(flags == (GHND | GMEM_SHARE) && bytes == 256);
    SetLastError(44731);
    return reject ? 0 : 0x4321;
}
static VOID WINAPI free_dialog_ds(WORD memory)
{
    ++dialog_calls;
    CHECK(memory == 0x4321);
    SetLastError(44732);
}
static DWORD WINAPI initialize_dialog(HWND window, LONG parameter)
{
    ++dialog_calls;
    CHECK(window == (HWND)(ULONG_PTR)0x1234 && parameter == -12345);
    SetLastError(44733);
    return reject ? 0 : 0x87654321;
}
static void exercise_original_dialog_callbacks(void)
{
    HANDLE memory;
    callbacks.pfn16GlobalAlloc = allocate_dialog_ds;
    callbacks.pfn16GlobalFree = free_dialog_ds;
    callbacks.pfnInitDlgCb = initialize_dialog;
    reject = FALSE;
    memory = GetEditDS();
    CHECK((ULONG_PTR)memory == 0x4321 && GetLastError() == 44731);
    ReleaseEditDS(memory);
    CHECK(GetLastError() == 44732);
    /* Original ReleaseEditDS passes LOWORD, not a native HGLOBAL/pointer. */
    ReleaseEditDS((HANDLE)(ULONG_PTR)0x98764321);
    CHECK(GetLastError() == 44732);
    CHECK(WOWDlgInit((HWND)(ULONG_PTR)0x1234, -12345) == 0x87654321);
    CHECK(GetLastError() == 44733);
    reject = TRUE;
    CHECK(GetEditDS() == NULL && GetLastError() == 44731);
    CHECK(WOWDlgInit((HWND)(ULONG_PTR)0x1234, -12345) == 0);
    CHECK(GetLastError() == 44733 && dialog_calls == 6);
}
int __cdecl main(void)
{
    callbacks.pfnLocalAlloc = allocate;
    callbacks.pfnLocalReAlloc = reallocate;
    callbacks.pfnLocalLock = lock_memory;
    callbacks.pfnLocalUnlock = unlock_memory;
    callbacks.pfnLocalSize = size_memory;
    callbacks.pfnLocalFree = free_memory;
    callbacks.pfnGetExpWinVer = module_version;
    callbacks.pfnWowEditNextWord = wow_word_break;
    exercise_original_macros();
    CHECK(calls == 16);
    exercise_original_dialog_callbacks();
    printf("WOW_EDIT_ORIGINAL_MEMORY_MACROS failures=%u calls=%u\n", failures, calls);
    return failures ? 3 : 0;
}
