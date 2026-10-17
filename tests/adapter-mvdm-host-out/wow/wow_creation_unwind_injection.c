#include "wow_window_creation_binding.h"

/* Separate fault-only object. Ordinary native-creation keeps the unchanged
 * production object. Inject after native destruction, outside USER's callback
 * exception filter, so the outer creation finally must unwind deterministically. */
static BOOL unwind_armed, unwind_returned;
static unsigned unwind_throws, unwind_hooks, unwind_releases;
static wow_window_words_binding *unwind_borrow;
#define CREATION_UNWIND_EXCEPTION ((DWORD)0xe0424474)

static HWND WINAPI unwind_create(DWORD, LPCSTR, LPCSTR, DWORD, int, int,
    int, int, HWND, HMENU, HINSTANCE, LPVOID);
static BOOL WINAPI unwind_terminal(HWND, wow_window_words_binding *);
static void WINAPI unwind_release(wow_window_words_binding *);
static HHOOK WINAPI unwind_hook(int, HOOKPROC, HINSTANCE, DWORD);
static BOOL WINAPI unwind_unhook(HHOOK);

#define CreateWindowExA unwind_create
#define wow_window_words_terminal unwind_terminal
#define wow_window_words_release unwind_release
#define SetWindowsHookExA unwind_hook
#define UnhookWindowsHookEx unwind_unhook
#include "../../../src/wow32-dll/source/wow_window_creation_binding.c"
#undef CreateWindowExA
#undef wow_window_words_terminal
#undef wow_window_words_release
#undef SetWindowsHookExA
#undef UnhookWindowsHookEx

VOID WINAPI wow_fixture_creation_unwind_arm(void)
{
    unwind_armed = TRUE;
    unwind_returned = FALSE;
    unwind_throws = unwind_releases = 0;
    unwind_borrow = NULL;
}

BOOL WINAPI wow_fixture_creation_unwind_complete(void)
{
    BOOL complete = !unwind_armed && !unwind_returned && unwind_throws == 1 &&
        unwind_releases == 1 && unwind_hooks == 0 && current_scope == NULL;
    if (complete) unwind_borrow = NULL; /* Do not track a later reused address. */
    return complete;
}

static HWND WINAPI unwind_create(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style,
    int x, int y, int width, int height, HWND parent, HMENU menu,
    HINSTANCE instance, LPVOID parameter)
{
    HWND window = CreateWindowExA(ex, cls, title, style, x, y, width, height,
        parent, menu, instance, parameter);
    if (unwind_armed && window) {
        /* Native rollback finishes first; the retained scope still owns its
         * exact storage reference when the outer finally runs. */
        if (!DestroyWindow(window)) RaiseException(0xe0424475, 0, 0, NULL);
        unwind_armed = FALSE;
        unwind_returned = TRUE;
        SetLastError(7447);
        return NULL;
    }
    return window;
}

static BOOL WINAPI unwind_terminal(HWND window, wow_window_words_binding *borrow)
{
    BOOL result = wow_window_words_terminal(window, borrow);
    if (unwind_returned) {
        unwind_returned = FALSE;
        unwind_borrow = borrow;
        ++unwind_throws;
        SetLastError(7447);
        RaiseException(CREATION_UNWIND_EXCEPTION, 0, 0, NULL);
    }
    return result;
}

static void WINAPI unwind_release(wow_window_words_binding *borrow)
{
    if (borrow && borrow == unwind_borrow) ++unwind_releases;
    wow_window_words_release(borrow);
}

static HHOOK WINAPI unwind_hook(int id, HOOKPROC procedure, HINSTANCE instance,
    DWORD thread)
{
    HHOOK hook = SetWindowsHookExA(id, procedure, instance, thread);
    if (hook) ++unwind_hooks;
    return hook;
}

static BOOL WINAPI unwind_unhook(HHOOK hook)
{
    BOOL result = UnhookWindowsHookEx(hook);
    if (result) --unwind_hooks;
    return result;
}
