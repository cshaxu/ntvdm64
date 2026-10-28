/* Isolated open/query/close only: no wave buffer is sent and no audio plays. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>

typedef BOOL (WINAPI *resolve_fn)(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
#ifdef TEST_WOW_WRAPPER
extern BOOL WINAPI WOW32ResolveHandle(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
extern HANDLE hWinmm;
void *mvdm_wow_gdi_handle_to_native(unsigned short h) { (void)h; ExitProcess(92); return NULL; }
unsigned short mvdm_wow_gdi_handle_from_native(void *h) { (void)h; ExitProcess(93); return 0; }
void *WINAPI GetPModeVDMPointerAssert(DWORD vp) { (void)vp; ExitProcess(90); return NULL; }
void WINAPI UpdateDosCurrentDirectory(DWORD direction) { (void)direction; ExitProcess(91); }
#define PROBE_MODE "wrapper"
#else
#define PROBE_MODE "provider"
#endif

int main(void)
{
    WAVEFORMATEX format;
    HWAVEOUT device = NULL;
    MMRESULT query, open_result, close_result = (MMRESULT)-1;
    HMODULE module;
    resolve_fn resolve;
    WORD out16 = 0xaaaa;
    DWORD out32 = 0xccccccccUL, exception = 0, error = 0;
    BOOL result = FALSE;
    DWORD before = 0, after = 0;
    char path[MAX_PATH], resolved[MAX_PATH];
    HANDLE image_file;
    ZeroMemory(&format, sizeof(format));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = 22050;
    format.wBitsPerSample = 16;
    format.nBlockAlign = 2;
    format.nAvgBytesPerSec = 44100;
    if (waveOutGetNumDevs() == 0) { puts("no_wave_output"); return 30; }
    query = waveOutOpen(NULL, 0, &format, 0, 0, WAVE_FORMAT_QUERY);
    if (query != MMSYSERR_NOERROR) { printf("query=%u\n", query); return 31; }
    open_result = waveOutOpen(&device, 0, &format, 0, 0, CALLBACK_NULL);
    if (open_result != MMSYSERR_NOERROR) { printf("open=%u\n", open_result); return 32; }
    if (!GetSystemDirectoryA(path, MAX_PATH) || strlen(path) + 11 >= MAX_PATH) { waveOutClose(device); return 33; }
    strcat_s(path, sizeof(path), "\\winmm.dll");
    module = LoadLibraryExA(path, NULL, 0);
    if (!module) { waveOutClose(device); return 34; }
    resolve = (resolve_fn)GetProcAddress(module, "WOW32ResolveMultiMediaHandle");
    if (!resolve || !GetModuleFileNameA(module, path, MAX_PATH)) { waveOutClose(device); return 35; }
#ifdef TEST_WOW_WRAPPER
    hWinmm = module;
    resolve = WOW32ResolveHandle;
#endif
    image_file = CreateFileA(path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (image_file == INVALID_HANDLE_VALUE) { waveOutClose(device); return 36; }
    if (!GetFinalPathNameByHandleA(image_file, resolved, MAX_PATH, FILE_NAME_NORMALIZED)) { CloseHandle(image_file); waveOutClose(device); return 37; }
    CloseHandle(image_file);
    SetLastError(0);
    __asm mov before, esp
    __try {
        result = resolve(4, 2, 0, &out16, (DWORD)(ULONG_PTR)device, &out32);
        error = GetLastError();
    } __except (EXCEPTION_EXECUTE_HANDLER) { exception = GetExceptionCode(); }
    __asm mov after, esp
    close_result = waveOutClose(device);
    printf("mode=%s resolved=%s query=%u open=%u handle_nonzero=%d handle_low16=%04x result=%d error=%lu exception=%08lx out16=%04x out32=%08lx stack_equal=%d close=%u\n",
           PROBE_MODE, resolved, query, open_result, device != NULL, (WORD)(ULONG_PTR)device, result, error,
           exception, out16, out32, before == after, close_result);
    FreeLibrary(module);
    return before == after && close_result == MMSYSERR_NOERROR ? 0 : 38;
}
