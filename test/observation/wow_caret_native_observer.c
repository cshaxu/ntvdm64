/* Read-only observer for independently authored Win16 caret probe. */
#include <windows.h>
#include <string.h>
__declspec(dllexport) DWORD WINAPI CaretObserve(DWORD phase,DWORD unused1,DWORD unused2)
{
    GUITHREADINFO info = {sizeof(info)};
    char name[64] = {0};
    BOOL shown;
    (void)unused1; (void)unused2;
    if (!GetGUIThreadInfo(GetCurrentThreadId(), &info)) return 0x10;
    if (phase == 5) return info.hwndCaret ? 0x11 : 1;
    if (!info.hwndCaret || !GetClassNameA(info.hwndCaret,name,sizeof(name)) ||
        strcmp(name,"T478_CARET")) return 0x12;
    shown = !!(info.flags & GUI_CARETBLINKING);
    if (shown != (phase == 1 || phase == 4)) return 0x13;
    if (info.rcCaret.right-info.rcCaret.left != (phase < 3 ? 2 : 3) ||
        info.rcCaret.bottom-info.rcCaret.top != (phase < 3 ? 12 : 11)) return 0x14;
    if ((phase == 1 || phase == 2) &&
        (info.rcCaret.left != 7 || info.rcCaret.top != 9)) return 0x15;
    if (phase == 4 && (info.rcCaret.left != 4 || info.rcCaret.top != 6)) return 0x16;
    return 1;
}
