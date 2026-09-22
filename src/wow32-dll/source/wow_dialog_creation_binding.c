#include "wow_dialog_creation_binding.h"

#include "wow_user_runtime.h"
#include "wow_user_borrow_scope.h"
#include "wow_window_words_binding.h"
#include "wow_task_order_bindings.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

typedef VOID (WINAPI *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

/* This is the original WOW32 input callback, retained in wcall32.c.  Its
 * private WW reads/writes are already redirected to the scoped binding by
 * precomp.h; this carrier supplies the same WW before calling it. */
DWORD WINAPI W32InitDlg(HWND, LONG);

typedef struct wow_dialog_creation {
    struct wow_dialog_creation *previous;
    HHOOK create_hook;
    HHOOK return_hook;
    HWND window;
    DLGPROC procedure;
    LONG input_parameter;
    LONG init_parameter;
    wow_window_words_binding *borrow;
    wow_task_order_thread *thread;
    DWORD failure;
    BOOL initialized;
    LONG references;
} wow_dialog_creation;

static __declspec(thread) wow_dialog_creation *current_creation;
static const WCHAR dialog_property[] = L"NTVDM.WOW.DialogCreation";

static void retain_creation(wow_dialog_creation *creation)
{
    if (creation) InterlockedIncrement(&creation->references);
}

static void release_creation(wow_dialog_creation *creation)
{
    if (!creation || InterlockedDecrement(&creation->references) != 0) return;
    wow_window_words_release(creation->borrow);
    HeapFree(GetProcessHeap(), 0, creation);
}

static void discard_creation(wow_dialog_creation *creation)
{
    wow_dialog_creation *property;

    if (!creation) return;
    if (creation->window) {
        property = (wow_dialog_creation *)RemovePropW(creation->window,
            dialog_property);
        if (property) release_creation(property);
        if (creation->borrow)
            (void)wow_window_words_detach_if(creation->window,
                creation->borrow);
    }
    creation->window = NULL;
    wow_window_words_release(creation->borrow);
    creation->borrow = NULL;
    release_creation(creation);
}

static LRESULT CALLBACK dialog_create_hook(int code, WPARAM wp, LPARAM lp)
{
    wow_dialog_creation *creation = current_creation;

    if (code == HCBT_CREATEWND && creation && !creation->window) {
        CBT_CREATEWNDA *cbt = (CBT_CREATEWNDA *)lp;
        CREATESTRUCTA *request = cbt ? cbt->lpcs : NULL;
        char class_name[16] = {0};
        WW words = {0};

        if (request && !(request->style & WS_CHILD) &&
                GetClassNameA((HWND)wp, class_name, ARRAYSIZE(class_name)) &&
                !lstrcmpiA(class_name, "#32770")) {
            words.dwStyle = request->style;
            words.dwExStyle = request->dwExStyle;
            words.hInstance = request->hInstance;
            if (!wow_window_words_attach((HWND)wp, &words) ||
                    !(creation->borrow = wow_window_words_acquire((HWND)wp)) ||
                    !wow_window_words_publish_owner((HWND)wp,
                        creation->borrow, creation->thread)) {
                creation->failure = GetLastError();
                return 1;
            }
            if (!SetPropW((HWND)wp, dialog_property, creation)) {
                creation->failure = GetLastError();
                return 1;
            }
            retain_creation(creation); /* native window property ownership */
            creation->window = (HWND)wp;
        }
    }
    return CallNextHookEx(NULL, code, wp, lp);
}

static LRESULT CALLBACK dialog_return_hook(int code, WPARAM wp, LPARAM lp)
{
    wow_dialog_creation *creation = current_creation;

    UNREFERENCED_PARAMETER(wp);
    if (code >= 0 && creation && creation->window && !creation->initialized) {
        CWPRETSTRUCT *message = (CWPRETSTRUCT *)lp;
        if (message && message->hwnd == creation->window &&
                message->message == WM_CREATE && message->lResult != -1) {
            wow_user_borrow_scope scope;

            /* W32InitDlg is original WOW32 code.  Its GWL_WOW* accessors
             * intentionally require a bounded worker-local borrow scope;
             * establish it at the native callback boundary rather than
             * weakening the private access contract. */
            if (!wow_user_borrow_enter(&scope)) {
                creation->failure = GetLastError();
            } else {
                __try {
                    creation->init_parameter = (LONG)W32InitDlg(
                        creation->window, creation->input_parameter);
                    creation->initialized = TRUE;
                } __finally {
                    (void)wow_user_borrow_leave(&scope);
                }
            }
        }
    }
    return CallNextHookEx(NULL, code, wp, lp);
}

static INT_PTR CALLBACK dialog_proc(HWND window, UINT message, WPARAM wp,
    LPARAM lp)
{
    wow_dialog_creation *creation = (wow_dialog_creation *)GetPropW(window,
        dialog_property);
    INT_PTR result = FALSE;

    if (!creation || !creation->initialized) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    retain_creation(creation); /* survive a nested terminal callback */
    {
        wow_user_borrow_scope scope;

        if (!wow_user_borrow_enter(&scope)) {
            release_creation(creation);
            return FALSE;
        }
        __try {
            if (creation->procedure) {
                result = creation->procedure(window, message, wp,
                    message == WM_INITDIALOG ? creation->init_parameter : lp);
            }
            if (message == WM_NCDESTROY) {
                wow_dialog_creation *property =
                    (wow_dialog_creation *)RemovePropW(window, dialog_property);
                if (property) release_creation(property);
                if (creation->borrow)
                    (void)wow_window_words_detach_if(window, creation->borrow);
                creation->window = NULL;
                wow_window_words_release(creation->borrow);
                creation->borrow = NULL;
            }
        } __finally {
            (void)wow_user_borrow_leave(&scope);
        }
    }
    release_creation(creation);
    return result;
}

HWND WINAPI wow_user_server_create_dialog(HANDLE module,
    LPDLGTEMPLATE dialog_template, DWORD bytes, HWND owner, DLGPROC procedure,
    LONG init_parameter, UINT flags)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_dialog_creation *creation;
    wow_dialog_creation *previous;
    HWND result = NULL;
    DWORD error = ERROR_SUCCESS;
    BOOL failed = FALSE;

    if (!dialog_template || bytes == 0 || !binding || !binding->thread ||
            !binding->thread->ppi || !binding->thread->ppi->objects ||
            (flags & ~(SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE |
                SCDLG_16BIT))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    creation = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*creation));
    if (!creation) return NULL;
    creation->procedure = procedure;
    creation->input_parameter = init_parameter;
    creation->thread = binding->thread;
    creation->previous = current_creation;
    creation->references = 1; /* creator owns until CreateDialog returns */
    creation->create_hook = SetWindowsHookExA(WH_CBT, dialog_create_hook, NULL,
        GetCurrentThreadId());
    creation->return_hook = SetWindowsHookExA(WH_CALLWNDPROCRET,
        dialog_return_hook, NULL, GetCurrentThreadId());
    if (!creation->create_hook || !creation->return_hook) {
        error = GetLastError();
        if (creation->create_hook) UnhookWindowsHookEx(creation->create_hook);
        if (creation->return_hook) UnhookWindowsHookEx(creation->return_hook);
        HeapFree(GetProcessHeap(), 0, creation);
        SetLastError(error);
        return NULL;
    }
    current_creation = creation;
    previous = creation->previous;
    __try {
        if (flags & SCDLG_ANSI) {
            result = CreateDialogIndirectParamA((HINSTANCE)module,
                (LPCDLGTEMPLATEA)dialog_template, owner, dialog_proc,
                init_parameter);
        } else {
            result = CreateDialogIndirectParamW((HINSTANCE)module,
                dialog_template, owner, dialog_proc, init_parameter);
        }
        error = creation->failure ? creation->failure : GetLastError();
        if (!result || !creation->window || !creation->initialized) {
            if (result) DestroyWindow(result);
            result = NULL;
            if (!error) error = ERROR_INVALID_DATA;
            failed = TRUE;
        }
    } __finally {
        current_creation = previous;
        if (creation->create_hook)
            UnhookWindowsHookEx(creation->create_hook);
        if (creation->return_hook)
            UnhookWindowsHookEx(creation->return_hook);
    }
    if (failed) discard_creation(creation);
    else release_creation(creation);
    SetLastError(error);
    return result;
}
