#include "wow_dialog_creation_binding.h"
#include "wow_user_private_access.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"
#include "wow_window_words_binding.h"
#include "ntvdm-exe/session/session.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

#include <stdio.h>
#include <string.h>

static unsigned errors;
static unsigned init_messages;
static unsigned terminal_messages;

#define CHECK(value) do { if (!(value)) { ++errors; \
    printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

/* Link-time poison for unselected functions in the unchanged wcall32.c
 * translation unit.  /Gy does not prevent MSVC from requiring their symbols,
 * but no test is permitted to turn one into a success stub. */
static DWORD WINAPI unexpected1(DWORD value)
{
    UNREFERENCED_PARAMETER(value);
    ExitProcess(90);
}
static DWORD WINAPI unexpected2(DWORD first, DWORD second)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    ExitProcess(90);
}
static DWORD WINAPI unexpected3(DWORD first, DWORD second, DWORD third)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    UNREFERENCED_PARAMETER(third);
    ExitProcess(90);
}
static DWORD WINAPI unexpected4(DWORD first, DWORD second, DWORD third,
    DWORD fourth)
{
    UNREFERENCED_PARAMETER(first);
    UNREFERENCED_PARAMETER(second);
    UNREFERENCED_PARAMETER(third);
    UNREFERENCED_PARAMETER(fourth);
    ExitProcess(90);
}
DWORD WINAPI GetPModeVDMPointerAssert(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalAlloc16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI LocalReAlloc16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI LocalLock16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalUnlock16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalSize16(DWORD value) { return unexpected1(value); }
DWORD WINAPI LocalFree16(DWORD value) { return unexpected1(value); }
DWORD WINAPI CallBack16(DWORD a, DWORD b, DWORD c, DWORD d) { return unexpected4(a,b,c,d); }
DWORD WINAPI malloc16(DWORD value) { return unexpected1(value); }
DWORD WINAPI free16(DWORD value) { return unexpected1(value); }
DWORD WINAPI ParamMapUpdateNode(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI WOWGlobalAllocLock16(DWORD a, DWORD b, DWORD c) { return unexpected3(a,b,c); }
DWORD WINAPI WOWGlobalUnlockFree16(DWORD value) { return unexpected1(value); }

static VOID WINAPI end_task(void)
{
    ++errors;
}

static INT_PTR CALLBACK dialog_proc(HWND window, UINT message, WPARAM wp,
    LPARAM lp)
{
    UNREFERENCED_PARAMETER(wp);
    if (message == WM_INITDIALOG) {
        ++init_messages;
        CHECK(lp == 0x12345678);
        CHECK(wow_user_get_window_longA(window, GWL_WOWDWORD1) ==
            (LONG)MAKELONG(10, 1));
        CHECK(wow_user_get_window_longA(window, GWL_WOWDWORD3) ==
            (LONG)0x43210076);
        CHECK(GetDlgItem(window, 100) != NULL);
    }
    if (message == WM_NCDESTROY) {
        ++terminal_messages;
        CHECK(wow_window_words_acquire(window) != NULL);
    }
    return FALSE;
}

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    wow_user_task_lifecycle lifecycle = {0};
    wow_user_task_lifecycle_callbacks callbacks = {0};
    HANDLE wowexec = CreateEventW(NULL, FALSE, FALSE, NULL);
    DWORD template_storage[128] = {0};
    DLGTEMPLATE *dialog = (DLGTEMPLATE *)template_storage;
    DLGITEMTEMPLATE *item;
    WORD *next;
    DWORD dialog_data[2] = { 0x43210076u, 0x12345678u };
    HWND window;

    session_initialize(&owner, 42041u);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_initialize(&runtime));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(wow_user_task_lifecycle_initialize(&lifecycle, &runtime, &callbacks));
    CHECK(wow_user_task_lifecycle_register(&lifecycle, end_task, wowexec));
    CHECK(wow_user_task_lifecycle_init(&lifecycle, 0x0400, "DIALOG.EXE",
        0x1235, 0, 0, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_SHOW));

    dialog->style = WS_POPUP | DS_SETFONT;
    dialog->cdit = 1;
    dialog->cx = 100;
    dialog->cy = 40;
    next = (WORD *)(dialog + 1);
    *next++ = 0;
    *next++ = 0;
    *next++ = 0;
    *next++ = 8;
    memcpy(next, L"MS Shell Dlg", sizeof(L"MS Shell Dlg"));
    next += ARRAYSIZE(L"MS Shell Dlg");
    item = (DLGITEMTEMPLATE *)(((UINT_PTR)next + 3) & ~(UINT_PTR)3);
    item->style = WS_CHILD | WS_VISIBLE | ES_LEFT;
    item->x = 2;
    item->y = 2;
    item->cx = 80;
    item->cy = 12;
    item->id = 100;
    next = (WORD *)(item + 1);
    *next++ = 0xffff;
    *next++ = 0x0081;
    *next++ = 0;
    *next++ = 0;

    CHECK(!wow_user_server_create_dialog(GetModuleHandleA(NULL), NULL, 0,
        NULL, dialog_proc, (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    window = wow_user_server_create_dialog(GetModuleHandleA(NULL), dialog,
        (DWORD)((BYTE *)next - (BYTE *)dialog), NULL, dialog_proc,
        (LONG)dialog_data, SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE);
    CHECK(window != NULL);
    CHECK(init_messages == 1);
    CHECK(DestroyWindow(window));
    CHECK(terminal_messages == 1);
    CHECK(wow_window_words_acquire(window) == NULL);
    CHECK(wow_user_task_lifecycle_cleanup(&lifecycle, 0x1235));
    wow_user_task_lifecycle_dispose(&lifecycle);
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    if (wowexec) CloseHandle(wowexec);
    printf("WOW_USER_TASK_DIALOG_LIFECYCLE errors=%u init=%u terminal=%u\n",
        errors, init_messages, terminal_messages);
    return errors ? 1 : 0;
}
