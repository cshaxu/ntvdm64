/* Test-only selected wmmedia.obj with its loader imports rebound to seams. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

extern BOOL WINAPI WOW32ResolveHandle(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
extern BOOL WINAPI WOW32DriverCallback(DWORD, DWORD, WORD, WORD, DWORD, DWORD, DWORD);
static int mode, calls;

static BOOL WINAPI fake_resolver(UINT kind, UINT direction, WORD h16, LPWORD out16,
                                  DWORD h32, LPDWORD out32)
{
    (void)kind; (void)h16; (void)h32;
    ++calls;
    if (direction == 1) *out32 = 0x13579bdfUL;
    else if (direction == 2) *out16 = 0x2468;
    return mode == 2 ? FALSE : TRUE;
}
static HMODULE WINAPI fake_load(LPCSTR name) { (void)name; return mode == 3 ? NULL : (HMODULE)1; }
static FARPROC WINAPI fake_getproc(HMODULE module, LPCSTR name)
{
    (void)module;
    if (mode == 0) return NULL;
    if (strcmp(name, "WOW32ResolveMultiMediaHandle") == 0) return (FARPROC)fake_resolver;
    return NULL;
}
void *fake_imp_LoadLibraryA = (void *)fake_load;
void *fake_imp_GetProcAddress = (void *)fake_getproc;
/* These two seams are only reached for GDI handles, not the wave cases. */
void *mvdm_wow_gdi_handle_to_native(unsigned short h) { (void)h; ExitProcess(92); return NULL; }
unsigned short mvdm_wow_gdi_handle_from_native(void *h) { (void)h; ExitProcess(93); return 0; }
/* Fail-fast seams for unrelated wmmedia entrypoints; no scenario may use them. */
void *WINAPI GetPModeVDMPointerAssert(DWORD vp) { (void)vp; ExitProcess(90); return NULL; }
void WINAPI UpdateDosCurrentDirectory(DWORD direction) { (void)direction; ExitProcess(91); }

int main(int argc, char **argv)
{
    WORD out16 = 0xaaaa;
    DWORD out32 = 0xccccccccUL;
    BOOL result, callback_result;
    if (argc != 2) return 20;
    if (!strcmp(argv[1], "missing") || !strcmp(argv[1], "missingdll")) {
        mode = !strcmp(argv[1], "missingdll") ? 3 : 0;
        result = WOW32ResolveHandle(4, 1, 0x1234, &out16, 0x11112222, &out32);
        callback_result = WOW32DriverCallback(0, 0, 0, 0, 0, 0, 0);
        printf("%s resolve=%d callback=%d out16=%04x out32=%08lx calls=%d\n",
               argv[1], result, callback_result, out16, out32, calls);
        return result == 6 && callback_result == 6 && calls == 0 ? 0 : 21;
    }
    mode = !strncmp(argv[1], "fail", 4) ? 2 : 1;
    if (strstr(argv[1], "16to32"))
        result = WOW32ResolveHandle(4, 1, 0x1234, &out16, 0x11112222, &out32);
    else if (strstr(argv[1], "32to16"))
        result = WOW32ResolveHandle(4, 2, 0x1234, &out16, 0x11112222, &out32);
    else return 22;
    printf("%s resolve=%d out16=%04x out32=%08lx calls=%d\n",
           argv[1], result, out16, out32, calls);
    return calls == 1 ? 0 : 23;
}
