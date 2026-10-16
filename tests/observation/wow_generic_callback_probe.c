/* Independent native half of the Win16 generic-thunk witness. Loaded only
 * by the NE probe through original LoadLibraryEx32W/GetProcAddress32W.
 * No mock callback, task binding or replacement guest provider. */
#include <windows.h>

typedef DWORD (WINAPI *callback16_fn)(DWORD, DWORD);
typedef BOOL (WINAPI *callback16ex_fn)(DWORD, DWORD, DWORD, PVOID, PDWORD);
static __declspec(thread) unsigned depth;

DWORD WINAPI ResolveGenericSegment(DWORD address)
{
    typedef PVOID (WINAPI *resolve_fn)(DWORD, DWORD, BOOL);
    HMODULE module = GetModuleHandleW(L"WOW32.DLL");
    resolve_fn resolve = (resolve_fn)GetProcAddress(module, "WOWGetVDMPointer");
    const DWORD *first, *second;
    if (!resolve || !HIWORD(address) || LOWORD(address)) return 0;
    /* Original resolver owns faulting in the probe's non-preloaded segment.
     * Never manufacture an LDT/FlatAddress entry or patch guest code. */
    first = (const DWORD *)resolve(address, sizeof(DWORD), TRUE);
    if (!first || *first != 0x447a1234) return 0;
    second = (const DWORD *)resolve(address, sizeof(DWORD), TRUE);
    if (!second || *second != 0x447a1234) return 0;
    if (resolve(0, 0, TRUE) != NULL) return 0;
    return 0x447a0001;
}

DWORD WINAPI RunGenericProbe(DWORD pascal_proc, DWORD cdecl_proc, DWORD nested)
{
#ifdef TEST_GUEST_EXCEPTION
    /* Fault within the real original generic thunk, not directly across a
     * native Shell callback. W32Dispatch/W32TryCall own the disposition. */
    (void)pascal_proc;(void)cdecl_proc;(void)nested;
    RaiseException(0xe4470004,0,0,NULL);
    return 0;
#else
    HMODULE module = GetModuleHandleW(L"WOW32.DLL");
    callback16_fn callback = (callback16_fn)GetProcAddress(module, "WOWCallback16");
    callback16ex_fn callback_ex = (callback16ex_fn)GetProcAddress(module, "WOWCallback16Ex");
    DWORD arguments[4] = {0x33445566, 0, 0, 0};
    DWORD result, thread = GetCurrentThreadId();
    unsigned errors = 0;
    if (!callback || !callback_ex || !pascal_proc || !cdecl_proc) return 0;
    ++depth;
    if (depth != (nested ? 2u : 1u)) ++errors;
    if (nested) {
        if (callback(pascal_proc, 0x22334455) != 0x22334456) ++errors;
    } else {
        /* The first guest callback calls this native export recursively. */
        if (callback(pascal_proc, 0x10203040) != 0x10203041) ++errors;
        result = 0;
        if (!callback_ex(pascal_proc, 0, 4, arguments, &result) || result != 0x33445567)
            ++errors;
        arguments[0] = 0x55667788;
        result = 0;
        if (!callback_ex(cdecl_proc, 1, 4, arguments, &result) || result != 0x55667789)
            ++errors;
        result = 0xdeadbeef;
        SetLastError(ERROR_SUCCESS);
        if (callback_ex(pascal_proc, 0, 17, NULL, &result) ||
                GetLastError() != ERROR_INVALID_PARAMETER || result != 0xdeadbeef)
            ++errors;
    }
    if (GetCurrentThreadId() != thread) ++errors;
    --depth;
    return errors ? 0 : 0x447a0001;
#endif
}
