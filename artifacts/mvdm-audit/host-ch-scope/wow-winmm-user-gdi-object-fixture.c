/* Selected-object USER/GDI branches with a controlled WINMM lookup and GDI seams. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

extern BOOL WINAPI WOW32ResolveHandle(UINT, UINT, WORD, LPWORD, DWORD, LPDWORD);
static int missing, zero_mapping, gdi_to, gdi_from;
static HMODULE WINAPI fake_load(LPCSTR name) { (void)name; return (HMODULE)1; }
static BOOL WINAPI unexpected_mm_provider(UINT a, UINT b, WORD c, LPWORD d, DWORD e, LPDWORD f)
{ (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; ExitProcess(94); return FALSE; }
static FARPROC WINAPI fake_getproc(HMODULE module, LPCSTR name)
{ (void)module; (void)name; return missing ? NULL : (FARPROC)unexpected_mm_provider; }
void *fake_imp_LoadLibraryA = (void *)fake_load;
void *fake_imp_GetProcAddress = (void *)fake_getproc;
void *mvdm_wow_gdi_handle_to_native(unsigned short h)
{ (void)h; ++gdi_to; return zero_mapping ? NULL : (void *)(ULONG_PTR)0x55667788UL; }
unsigned short mvdm_wow_gdi_handle_from_native(void *h)
{ (void)h; ++gdi_from; return zero_mapping ? 0 : 0x6789; }
void *WINAPI GetPModeVDMPointerAssert(DWORD vp) { (void)vp; ExitProcess(90); return NULL; }
void WINAPI UpdateDosCurrentDirectory(DWORD direction) { (void)direction; ExitProcess(91); }

int main(int argc, char **argv)
{
    UINT kind, direction;
    WORD out16 = 0xaaaa;
    DWORD out32 = 0xccccccccUL;
    BOOL result;
    if (argc != 2) return 20;
    missing = strstr(argv[1], "missing") != NULL;
    zero_mapping = strstr(argv[1], "zero") != NULL;
    kind = strstr(argv[1], "gdi") ? 2 : 1;
    direction = strstr(argv[1], "32to16") ? 2 : 1;
    result = WOW32ResolveHandle(kind, direction, 0x1234, &out16, 0x11112222, &out32);
    printf("%s result=%d out16=%04x out32=%08lx gdi_to=%d gdi_from=%d\n",
           argv[1], result, out16, out32, gdi_to, gdi_from);
    return 0;
}
