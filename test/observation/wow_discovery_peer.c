/* Native peer in the observer's isolated desktop. No guest memory access,
 * injection, desktop activation, or original guest modification. */
#include <windows.h>
#include <stdio.h>

int wmain(int argc, WCHAR **argv)
{
    HDESK desktop;
    HWND window;
    HANDLE file;
    WCHAR ready[MAX_PATH], go[MAX_PATH], result[MAX_PATH];
    ULONGLONG deadline;
    MSG message;
    BOOL complete = FALSE;
    if (argc != 3) return 2;
    if (swprintf_s(ready, MAX_PATH, L"%s\\READY.DAT", argv[2]) < 0 ||
        swprintf_s(go, MAX_PATH, L"%s\\GO.DAT", argv[2]) < 0 ||
        swprintf_s(result, MAX_PATH, L"%s\\RESULT.BIN", argv[2]) < 0) return 2;
    deadline = GetTickCount64() + 15000;
    do {
        desktop = OpenDesktopW(argv[1], 0, FALSE,
            DESKTOP_CREATEWINDOW | DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS);
        if (desktop) break;
        Sleep(20);
    } while (GetTickCount64() < deadline);
    if (!desktop || !SetThreadDesktop(desktop)) return 3;
    while (GetFileAttributesW(ready) == INVALID_FILE_ATTRIBUTES) {
        if (GetTickCount64() >= deadline) { CloseDesktop(desktop); return 4; }
        Sleep(20);
    }
    window = CreateWindowExA(0, "STATIC", "S2 late native discovery", WS_POPUP,
        0, 0, 80, 40, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!window) { CloseDesktop(desktop); return 5; }
    if (!SetPropA(window, "T447PROP", (HANDLE)(ULONG_PTR)0x4567)) {
        DestroyWindow(window); CloseDesktop(desktop); return 8;
    }
    printf("native-created-after-ready hwnd=%p pid=%lu\n", (void *)window, GetCurrentProcessId());
    fflush(stdout);
    file = CreateFileW(go, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DestroyWindow(window); CloseDesktop(desktop); return 6;
    }
    CloseHandle(file);
    deadline = GetTickCount64() + 15000;
    do {
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message); DispatchMessageW(&message);
        }
        file = CreateFileW(result, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            complete = GetFileSize(file, NULL) == 18;
            CloseHandle(file);
        }
        if (complete) break;
        Sleep(20);
    } while (GetTickCount64() < deadline);
    RemovePropA(window, "T447PROP");
    DestroyWindow(window); CloseDesktop(desktop);
    printf("native-peer report-complete=%d\n", complete);
    return complete ? 0 : 7;
}
