/* Isolated, test-owned inputs only; no guest, device, UI or system settings. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef BOOL (WINAPI *resolve_fn)(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
#ifdef TEST_WOW_WRAPPER
extern BOOL WINAPI WOW32ResolveHandle(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
extern HANDLE hWinmm;
/* Fail-fast seams only for unrelated GDI and memory entrypoints. */
void *mvdm_wow_gdi_handle_to_native(unsigned short h) { (void)h; ExitProcess(92); return NULL; }
unsigned short mvdm_wow_gdi_handle_from_native(void *h) { (void)h; ExitProcess(93); return 0; }
void *WINAPI GetPModeVDMPointerAssert(DWORD vp) { (void)vp; ExitProcess(90); return NULL; }
void WINAPI UpdateDosCurrentDirectory(DWORD direction) { (void)direction; ExitProcess(91); }
#define PROBE_MODE "wrapper"
#else
#define PROBE_MODE "provider"
#endif

int main(int argc, char **argv)
{
    char path[MAX_PATH], resolved[MAX_PATH];
    HMODULE module;
    HANDLE image_file;
    resolve_fn resolve;
    WORD out16 = 0xaaaa, input16 = 0;
    DWORD out32 = 0xccccccccUL, input32 = 0, error = 0, exception = 0;
    UINT direction;
    BOOL result = FALSE;
    DWORD before = 0, after = 0;
    if (argc != 2) return 20;
    if (!strcmp(argv[1], "zero16")) direction = 1;
    else if (!strcmp(argv[1], "invalid16")) { direction = 1; input16 = 1; }
    else if (!strcmp(argv[1], "zero32")) direction = 2;
    else if (!strcmp(argv[1], "invalid32")) { direction = 2; input32 = 1; }
    else return 21;
    if (!GetSystemDirectoryA(path, MAX_PATH) || strlen(path) + 11 >= MAX_PATH) return 22;
    strcat_s(path, sizeof(path), "\\winmm.dll");
    module = LoadLibraryExA(path, NULL, 0);
    if (!module) return 23;
    resolve = (resolve_fn)GetProcAddress(module, "WOW32ResolveMultiMediaHandle");
    if (!resolve || !GetModuleFileNameA(module, path, MAX_PATH)) return 24;
#ifdef TEST_WOW_WRAPPER
    hWinmm = module;
    resolve = WOW32ResolveHandle;
#endif
    image_file = CreateFileA(path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (image_file == INVALID_HANDLE_VALUE) return 26;
    if (!GetFinalPathNameByHandleA(image_file, resolved, MAX_PATH, FILE_NAME_NORMALIZED)) return 27;
    CloseHandle(image_file);
    SetLastError(0);
    __asm mov before, esp
    __try {
        result = resolve(4, direction, input16, &out16, input32, &out32);
        error = GetLastError();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        exception = GetExceptionCode();
    }
    __asm mov after, esp
    printf("mode=%s case=%s path=%s resolved=%s result=%d error=%lu exception=%08lx out16=%04x out32=%08lx stack_equal=%d\n",
           PROBE_MODE, argv[1], path, resolved, result, error, exception, out16, out32, before == after);
    FreeLibrary(module);
    return before == after ? 0 : 25;
}
