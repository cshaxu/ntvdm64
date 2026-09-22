#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_window_words_binding.h"
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* Link original wcall32.c /Gy, retaining W32InitDlg, not a copied body.
 * The only selected private imports are the two WOW word accessors.
 * WW is eight DWORDs; DLGDATA is two DWORDs (original wowuserp.h/walias.h). */
DWORD WINAPI W32InitDlg(HWND window, LONG parameter);
static DWORD *words;
static HWND expected_window;
static wow_window_words_binding *scope;
static unsigned gets, sets, errors, callbacks;

static LONG WINAPI get_words(HWND window, int index)
{
    ++gets;
    if (window != expected_window || index != -1) ++errors;
    /* Test-owned synchronous borrow, NOT a product raw-pointer lifetime rule. */
    return (LONG)(ULONG_PTR)wow_window_words_value(scope);
}
static LONG WINAPI set_words(HWND window, int index, LONG value)
{
    unsigned offset;
    DWORD old;
    wow_window_words_binding *write;
    DWORD *actual;
    if (window != expected_window || (index != -30 && index != -32)) {
        ++errors;
        return 0;
    }
    if ((sets == 0 && index != -30) || (sets == 1 && index != -32)) ++errors;
    ++sets;
    offset = (unsigned)(-index - 30);
    write = wow_window_words_acquire(window);
    actual = (DWORD *)wow_window_words_value(write);
    if (!actual) { ++errors; return 0; }
    if (actual != words) ++errors;
    old = actual[offset];
    actual[offset] = (DWORD)value;
    wow_window_words_release(write);
    return (LONG)old;
}
LONG (WINAPI *fixture_get_import)(HWND, int) = get_words;
LONG (WINAPI *fixture_set_import)(HWND, int, LONG) = set_words;

/* MSVC resolves the other COMDATs before /OPT:REF discards them. They must
 * never execute in this bounded test; a reached dependency is a hard failure,
 * not a successful guest/provider stub. */
__declspec(noreturn) void fixture_unexpected(void)
{
    ExitProcess(90);
}
void (*fixture_unexpected_import)(void) = fixture_unexpected;

static DWORD WINAPI observe(HWND window, UINT message, UINT wp, LONG lp,
    DWORD proc, PVOID storage)
{
    (void)wp; (void)lp;
    ++callbacks;
    if (window != expected_window || message != WM_USER || proc != 0x81234567 ||
        storage != words || words[0] != MAKELONG(10, 1) ||
        words[2] != 0x12340056) ++errors;
    return 0x87654321;
}

int main(void)
{
    DWORD data[2] = {0x12340056, 0x89abcdef};
    DWORD before[8], result;
    WW initial;
    LONG dispatched = 0;
    unsigned i;
    memset(&initial, 0x5a, sizeof(initial));
    expected_window = CreateWindowExA(0, "STATIC", "", WS_POPUP,
        0, 0, 32, 32, NULL, NULL, GetModuleHandleA(NULL), NULL);
    if (!expected_window || !wow_window_words_attach(expected_window, &initial)) return 5;
    scope = wow_window_words_acquire(expected_window);
    words = (DWORD *)wow_window_words_value(scope);
    if (!words) return 6;
    memcpy(before, words, sizeof(before));
    result = W32InitDlg(expected_window, (LONG)(ULONG_PTR)data);
    if (result != data[1] || gets != 1 || sets != 2 || errors ||
        words[0] != MAKELONG(10, 1) || words[2] != data[0]) return 1;
    for (i = 0; i < ARRAYSIZE(before); ++i)
        if (i != 0 && i != 2 && words[i] != before[i]) return 2;
    if (!wow_window_dispatch_bound(expected_window, WM_USER, 0, 0,
        0x81234567, observe, &dispatched) || (DWORD)dispatched != 0x87654321 ||
        callbacks != 1 || errors) return 7;
    gets = sets = 0;
    if (W32InitDlg(expected_window, 0) != 0 || gets || sets) return 3;
    wow_window_words_detach(expected_window);
    wow_window_words_release(scope);
    scope = NULL;
    words = NULL;
    if (W32InitDlg(expected_window, (LONG)(ULONG_PTR)data) != 0 || gets != 1 || sets)
        return 4;
    if (!DestroyWindow(expected_window)) return 8;
    puts("WOW_ORIGINAL_W32INITDLG_TWO_FIELD_CONTRACT_OK");
    puts("WOW_ORIGINAL_DIALOG_INIT_AND_DISPATCH_SAME_STORAGE_OK");
    return 0;
}
