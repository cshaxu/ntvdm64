#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Own-thread native boundary experiment; no WOW registration or guest code.
 * Test the built-in dialog/EDIT route without replacing either window proc. */
static HHOOK create_hook, return_hook;
static unsigned sequence, attached, released, missing, first_dialog, child_at;
static unsigned init_at, font_at, init_children;
static unsigned dialogs, edits, auxiliary;
static BOOL active;
static const char property[] = "S40.DialogLifecycle";

#ifdef WOW_ORIGINAL_DIALOG_INIT
DWORD WINAPI W32InitDlg(HWND, LONG);
static HWND original_window;
static DWORD original_words[8], original_data[2] = {0x43210076, 0x12345678};
static DWORD original_parameter;
static unsigned original_at, original_calls, original_writes;
static LONG WINAPI original_get(HWND window, int index)
{
    if (window != original_window || index != -1) { ++missing; return 0; }
    return (LONG)(ULONG_PTR)GetPropA(window, property);
}
static LONG WINAPI original_set(HWND window, int index, LONG value)
{
    DWORD *words = (DWORD *)GetPropA(window, property);
    DWORD previous;
    unsigned slot;
    if (window != original_window || !words || (index != -30 && index != -32)) {
        ++missing;
        return 0;
    }
    slot = (unsigned)(-index - 30);
    previous = words[slot];
    words[slot] = (DWORD)value;
    ++original_writes;
    return (LONG)previous;
}
LONG (WINAPI *fixture_get_import)(HWND, int) = original_get;
LONG (WINAPI *fixture_set_import)(HWND, int, LONG) = original_set;
__declspec(noreturn) void fixture_unexpected(void) { ExitProcess(90); }
void (*fixture_unexpected_import)(void) = fixture_unexpected;
#endif

static LRESULT CALLBACK on_create(int code, WPARAM wp, LPARAM lp)
{
    if (code == HCBT_CREATEWND && active) {
        CBT_CREATEWNDA *data = (CBT_CREATEWNDA *)lp;
        char name[96] = {0};
        unsigned at = ++sequence;
        GetClassNameA((HWND)wp, name, sizeof(name));
        if (!lstrcmpiA(name, "#32770")) ++dialogs;
        else if (!lstrcmpiA(name, "EDIT")) ++edits;
        else ++auxiliary;
#ifdef WOW_ORIGINAL_DIALOG_INIT
        if (!lstrcmpiA(name, "#32770")) {
            original_window = (HWND)wp;
            if (!SetPropA((HWND)wp, property, original_words)) ++missing;
            else ++attached;
        } else
#endif
        if (!SetPropA((HWND)wp, property, (HANDLE)(UINT_PTR)at)) ++missing;
        else ++attached;
        if (data->lpcs->style & WS_CHILD) child_at = at;
        printf("create seq=%u child=%u class=%s\n", at,
            (data->lpcs->style & WS_CHILD) != 0, name);
    }
    return CallNextHookEx(create_hook, code, wp, lp);
}

static LRESULT CALLBACK on_return(int code, WPARAM wp, LPARAM lp)
{
    if (code >= 0 && active) {
        CWPRETSTRUCT *message = (CWPRETSTRUCT *)lp;
#ifdef WOW_ORIGINAL_DIALOG_INIT
        if (message->hwnd == original_window && message->message == WM_CREATE &&
            message->lResult != -1) {
            ++original_calls;
            original_at = ++sequence;
            original_parameter = W32InitDlg(original_window, (LONG)(ULONG_PTR)original_data);
            printf("original_init seq=%u child_exists=%u\n", original_at,
                GetDlgItem(original_window, 100) != NULL);
            if (GetDlgItem(original_window, 100)) ++missing;
        }
#endif
        if (message->message == WM_NCDESTROY) {
            HANDLE identity = RemovePropA(message->hwnd, property);
            printf("post_ncdestroy seq=%u state=%u\n", ++sequence,
                identity != NULL);
            if (identity) ++released;
            else ++missing;
        }
    }
    return CallNextHookEx(return_hook, code, wp, lp);
}

static INT_PTR CALLBACK dialog_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    unsigned at = ++sequence;
    (void)wp;
    if (!first_dialog) first_dialog = at;
    if (!GetPropA(window, property)) ++missing;
#ifdef WOW_ORIGINAL_DIALOG_INIT
    if (!original_at || original_words[0] != MAKELONG(10, 1) ||
        original_words[2] != original_data[0]) ++missing;
    if (message == WM_INITDIALOG) {
        if (lp != (LPARAM)original_data) ++missing;
        lp = original_parameter;
    }
#endif
    if (message == WM_SETFONT) font_at = at;
    if (message == WM_INITDIALOG) {
        init_at = at;
        init_children = GetDlgItem(window, 100) != NULL;
        if (lp != 0x12345678) ++missing;
    }
    printf("dialog seq=%u message=%04x child_exists=%u\n", at, message,
        GetDlgItem(window, 100) != NULL);
    return FALSE;
}

int __cdecl main(void)
{
    DWORD storage[128] = {0};
    DLGTEMPLATE *dialog = (DLGTEMPLATE *)storage;
    DLGITEMTEMPLATE *item;
    WORD *next;
    HWND window, edit;
    char output[16] = {0};
    int result = 0;
    dialog->style = WS_POPUP;
#ifndef WOW_DIALOG_NO_FONT
    dialog->style |= DS_SETFONT;
#endif
    dialog->cdit = 1;
    dialog->cx = 100;
    dialog->cy = 40;
    next = (WORD *)(dialog + 1);
    *next++ = 0; /* no menu */
    *next++ = 0; /* standard dialog class */
    *next++ = 0; /* empty title */
#ifndef WOW_DIALOG_NO_FONT
    *next++ = 8;
    memcpy(next, L"MS Shell Dlg", sizeof(L"MS Shell Dlg"));
    next += ARRAYSIZE(L"MS Shell Dlg");
#endif
    item = (DLGITEMTEMPLATE *)(((UINT_PTR)next + 3) & ~(UINT_PTR)3);
    item->style = WS_CHILD | WS_VISIBLE | ES_LEFT;
    item->x = 2;
    item->y = 2;
    item->cx = 80;
    item->cy = 12;
    item->id = 100;
    next = (WORD *)(item + 1);
    *next++ = 0xffff;
    *next++ = 0x0081; /* built-in EDIT */
    *next++ = 0; /* title */
    *next++ = 0; /* no creation payload */
    create_hook = SetWindowsHookExA(WH_CBT, on_create, NULL, GetCurrentThreadId());
    return_hook = SetWindowsHookExA(WH_CALLWNDPROCRET, on_return, NULL, GetCurrentThreadId());
    if (!create_hook || !return_hook) result = 1;
    else {
        active = TRUE;
        window = CreateDialogIndirectParamA(GetModuleHandleA(NULL), dialog, NULL,
            dialog_proc,
#ifdef WOW_ORIGINAL_DIALOG_INIT
            (LPARAM)original_data);
#else
            0x12345678);
#endif
        if (!window) result = 2;
        else {
            edit = GetDlgItem(window, 100);
            SetWindowTextA(edit, "S40");
            GetWindowTextA(edit, output, sizeof(output));
            if (strcmp(output, "S40")) result = 3;
            DestroyWindow(window);
        }
        active = FALSE;
    }
    if (return_hook) UnhookWindowsHookEx(return_hook);
    if (create_hook) UnhookWindowsHookEx(create_hook);
    printf("attached=%u released=%u missing=%u first_dialog=%u font=%u child=%u init=%u init_children=%u\n",
        attached, released, missing, first_dialog, font_at, child_at, init_at, init_children);
    printf("dialogs=%u edits=%u auxiliary=%u\n", dialogs, edits, auxiliary);
    if (dialogs != 1 || edits != 1 || attached != released || missing ||
        !child_at || !init_at || child_at >= init_at || !init_children) result = 4;
#ifndef WOW_DIALOG_NO_FONT
    if (!font_at || font_at >= child_at) result = 4;
#else
    if (font_at || first_dialog <= child_at) result = 4;
#endif
#ifdef WOW_ORIGINAL_DIALOG_INIT
    printf("original_calls=%u writes=%u init_at=%u\n", original_calls,
        original_writes, original_at);
    if (original_calls != 1 || original_writes != 2 || !original_at ||
        original_at >= child_at || original_at >= first_dialog) result = 5;
    if (!result) puts("WOW_ORIGINAL_INIT_BEFORE_NATIVE_DIALOG_CALLBACKS_OK");
#endif
    if (!result) puts("WOW_NATIVE_DIALOG_CONTROL_POST_DESTROY_OK");
    return result;
}
