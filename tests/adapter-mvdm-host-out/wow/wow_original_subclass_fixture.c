#include "precomp.h"

/* Execute the unchanged WOW32 policy with controlled native API returns.
 * This establishes the provider/binding contract, not native or guest execution. */
static WW words;
static TD task;
static OPENNT_SUPPORT_TEB teb;
static unsigned errors, cases, native_calls, word_writes, thunk_calls;
static DWORD old_proc, supplied_proc, mapped_proc, thunk_input;
static BOOL found = TRUE;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
static LONG WINAPI fixture_set(HWND window, int index, LONG value);
#undef SetWindowLongA
#define SetWindowLongA fixture_set
#include "original-subclass.inc"

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(void) { return &teb; }
PWW FindPWW(HWND window, INT cls)
{
    CHECK(window == (HWND)0x1234 && cls == WOWCLASS_UNKNOWN);
    return found ? &words : NULL;
}
DWORD IsThunkWindowProc(DWORD proc, PINT cls)
{
    (void)proc;
    if (cls) *cls = WOWCLASS_EDIT;
    return mapped_proc;
}
DWORD GetThunkWindowProc(DWORD proc, LPSTR cls, PWW value, HWND window)
{
    CHECK(cls == NULL && value == &words && window == (HWND)0x1234);
    ++thunk_calls; thunk_input = proc;
    return 0x24680010;
}
LONG W32DialogFunc(HWND hwnd, UINT msg, DWORD wp, LONG lp)
{
    (void)hwnd; (void)msg; (void)wp; (void)lp;
    ++errors; return 0;
}
static LONG WINAPI fixture_set(HWND window, int index, LONG value)
{
    CHECK(window == (HWND)0x1234);
    if (index == GWL_WNDPROC) {
        ++native_calls; supplied_proc = value; return old_proc;
    }
    CHECK(index <= GWL_WOWDWORD1 && index >= GWL_WOWDWORD3);
    if (index <= GWL_WOWDWORD1 && index >= GWL_WOWDWORD3)
        words.adwWOW[GWL_WOWDWORD1 - index] = value;
    ++word_writes; return 0;
}
static ULONG run(DWORD new_proc)
{
    BYTE storage[sizeof(VDMFRAME) + sizeof(SETWINDOWLONG16)] = {0};
    PVDMFRAME frame = (PVDMFRAME)storage;
    PSETWINDOWLONG16 args = (PSETWINDOWLONG16)&frame->bArgs;
    args->f1 = 0x1234; args->f2 = GWL_WNDPROC; args->f3 = new_proc;
    native_calls = word_writes = thunk_calls = 0;
    supplied_proc = thunk_input = 0;
    ++cases;
    return WU32SetWindowLong(frame);
}
int __cdecl main(void)
{
    WW before;
    teb.WOW32Reserved = &task;
    words.iClass = WOWCLASS_WIN16;
    old_proc = 0x92340056;
    CHECK(run(0x12340078) == 0x12340056);
    CHECK(supplied_proc == 0x92340078 && words.vpfnWndProc == 0x12340078);
    CHECK(native_calls == 1 && word_writes == 1 && !thunk_calls);
    old_proc = 0x92300056;
    CHECK(run(0x92340078) == 0x92340056);
    CHECK(supplied_proc == 0x92300078 && words.vpfnWndProc == 0x92340078);
    CHECK(native_calls == 1 && word_writes == 1 && !thunk_calls);
    old_proc = 0xffff1234;
    CHECK(run(0x12340078) == 0x24680010);
    CHECK(thunk_calls == 1 && thunk_input == 0xffff1234);
    old_proc = 0x01234560;
    CHECK(run(0x12340078) == 0x24680010);
    CHECK(thunk_calls == 1 && thunk_input == old_proc);
    old_proc = 0; before = words;
    CHECK(run(0x12340078) == 0);
    CHECK(native_calls == 1 && !word_writes && !thunk_calls);
    CHECK(memcmp(&before, &words, sizeof(words)) == 0);
    mapped_proc = 0xffff5678; before = words;
    CHECK(run(0x24680010) == 0);
    CHECK(supplied_proc == mapped_proc && !word_writes && !thunk_calls);
    CHECK(memcmp(&before, &words, sizeof(words)) == 0);
    old_proc = 0x92300056;
    CHECK(run(0x24680010) == 0x92340056);
    CHECK(supplied_proc == mapped_proc && words.vpfnWndProc == 0);
    CHECK(words.iClass == WOWCLASS_EDIT && (words.flState & WWSTATE_ICLASSISSET));
    CHECK(native_calls == 1 && word_writes == 2 && !thunk_calls);
    found = FALSE; before = words;
    CHECK(run(0x12340078) == 0);
    CHECK(!native_calls && !word_writes && !thunk_calls);
    CHECK(memcmp(&before, &words, sizeof(words)) == 0);
    printf("WOW_ORIGINAL_SUBCLASS errors=%u cases=%u\n", errors, cases);
    return errors ? 3 : 0;
}
