#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
static unsigned hooks;
static UINT_PTR CALLBACK hook(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)wp; (void)lp;
    if (message == WM_INITDIALOG) ++hooks;
    return 0;
}
int main(void)
{
    unsigned mode, errors = 0;
    HWND owner = CreateWindowExA(0, "STATIC", "COMMDLG native reference", 0,
        0, 0, 1, 1, NULL, NULL, GetModuleHandleA(NULL), NULL);
    if (!owner) return 2;
    (void)RegisterWindowMessageA(FINDMSGSTRINGA);
    for (mode = 0; mode < 2; ++mode) {
        FINDREPLACEA request = {0};
        char find[128] = "needle", replace[128] = "replacement";
        HWND dialog;
        request.lStructSize = sizeof(request); request.hwndOwner = owner;
        request.lpstrFindWhat = find; request.wFindWhatLen = sizeof(find);
        request.lpstrReplaceWith = replace; request.wReplaceWithLen = sizeof(replace);
        request.Flags = FR_DOWN | FR_ENABLEHOOK; request.lpfnHook = hook;
        SetLastError(0);
        dialog = mode ? ReplaceTextA(&request) : FindTextA(&request);
        printf("NATIVE_COMMDLG_REFERENCE mode=%u hwnd=%p error=%lu extended=%lu hooks=%u size=%lu\n",
            mode, dialog, GetLastError(), CommDlgExtendedError(), hooks, request.lStructSize);
        if (!dialog || !IsWindow(dialog) || hooks != mode + 1) ++errors;
        if (dialog && (!DestroyWindow(dialog) || IsWindow(dialog))) ++errors;
    }
    if (!DestroyWindow(owner)) ++errors;
    printf("NATIVE_COMMDLG_REFERENCE_RESULT errors=%u modes=2\n", errors);
    return errors ? 1 : 0;
}
