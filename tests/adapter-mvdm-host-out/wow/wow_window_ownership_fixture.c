#include "wow_window_words_binding.h"
#include "wow_private_user_compat.h"
#include <stdio.h>
#include <stdlib.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

static unsigned errors;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static int child(char **argv)
{
    HANDLE map = (HANDLE)strtoul(argv[2], NULL, 10);
    HANDLE ready = (HANDLE)strtoul(argv[3], NULL, 10);
    HANDLE done = (HANDLE)strtoul(argv[4], NULL, 10);
    DWORD *shared = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, sizeof(DWORD));
    HWND window;
    if (!shared) return 3;
    window = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    if (!window) return 3;
    /* Deliberately invalid foreign pointer: ownership must be checked before
     * any dereference, for both complete and legacy short HWNDs. */
    if (!SetPropW(window, L"NTVDM.WOW.WindowWords", (HANDLE)1)) return 3;
    *shared = (DWORD)window;
    SetEvent(ready);
    WaitForSingleObject(done, 10000);
    RemovePropW(window, L"NTVDM.WOW.WindowWords");
    DestroyWindow(window);
    UnmapViewOfFile(shared);
    return 0;
}
int __cdecl main(int argc, char **argv)
{
    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    HANDLE map, ready, done;
    DWORD *shared, native_error, exit_code = 3;
    HWND window, variants[3];
    HWND paint_window;
    HDC dc;
    HBITMAP bitmap, previous;
    WW words = {0};
    char path[MAX_PATH], command[MAX_PATH + 100];
    unsigned i;
    BOOL launched;
    if (argc == 5 && !strcmp(argv[1], "--child")) return child(argv);
    paint_window = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    dc = CreateCompatibleDC(NULL);
    bitmap = CreateBitmap(16, 16, 1, 32, NULL);
    CHECK(paint_window && dc && bitmap);
    if (!paint_window || !dc || !bitmap) return 3;
    previous = SelectObject(dc, bitmap);
    map = CreateFileMappingA(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, sizeof(DWORD), NULL);
    ready = CreateEventA(&sa, TRUE, FALSE, NULL);
    done = CreateEventA(&sa, TRUE, FALSE, NULL);
    CHECK(map && ready && done);
    if (!map || !ready || !done) return 3;
    shared = MapViewOfFile(map, FILE_MAP_READ, 0, 0, sizeof(DWORD));
    CHECK(shared != NULL); if (!shared) return 3;
    CHECK(GetModuleFileNameA(NULL, path, sizeof(path)) != 0);
    sprintf_s(command, sizeof(command), "\"%s\" --child %lu %lu %lu",
        path, (DWORD)map, (DWORD)ready, (DWORD)done);
    si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_HIDE;
    launched = CreateProcessA(path, command, NULL, NULL, TRUE, CREATE_NO_WINDOW,
        NULL, NULL, &si, &pi);
    CHECK(launched);
    if (launched && WaitForSingleObject(ready, 5000) == WAIT_OBJECT_0) {
        window = (HWND)*shared;
        variants[0] = window;
        variants[1] = (HWND)(DWORD)LOWORD(window);
        variants[2] = (HWND)(LONG)(SHORT)LOWORD(window);
        for (i = 0; i != 3; ++i) {
            SetLastError(0);
            CHECK(GetWindowLongA(variants[i], GWL_WNDPROC) == 0);
            native_error = GetLastError();
            CHECK(native_error == ERROR_ACCESS_DENIED);
            CHECK(!wow_window_words_acquire(variants[i]));
            CHECK(GetLastError() == ERROR_ACCESS_DENIED);
            CHECK(!wow_window_words_attach(variants[i], &words));
            CHECK(GetLastError() == ERROR_ACCESS_DENIED);
            CHECK(GetPropW(variants[i], L"NTVDM.WOW.WindowWords") == (HANDLE)1);
            /* Child is waiting, not pumping sent messages. The original
             * foreign-process branch must use DefWindowProc, not send HDC. */
            CHECK(wow_private_user_fill_window(variants[i], paint_window, dc,
                (HBRUSH)CTLCOLOR_STATIC));
        }
    } else { CHECK(FALSE); }
    SetEvent(done);
    if (launched) {
        if (WaitForSingleObject(pi.hProcess, 5000) != WAIT_OBJECT_0) {
            TerminateProcess(pi.hProcess, 3);
            WaitForSingleObject(pi.hProcess, 5000); CHECK(FALSE);
        }
        CHECK(GetExitCodeProcess(pi.hProcess, &exit_code) && exit_code == 0);
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    }
    UnmapViewOfFile(shared);
    CloseHandle(map); CloseHandle(ready); CloseHandle(done);
    SelectObject(dc, previous); DeleteObject(bitmap); DeleteDC(dc);
    CHECK(DestroyWindow(paint_window));
    printf("WOW_WINDOW_OWNERSHIP errors=%u variants=3\n", errors);
    return errors ? 3 : 0;
}
