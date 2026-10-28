#include <windows.h>
static void unexpected(void) { ExitProcess(91); }
int seam_free_count;
void *seam_expected_free;
void *__stdcall opennt_support_current_teb(void) { unexpected(); return 0; }
void __stdcall getrect16(DWORD p, void *q) { (void)p; (void)q; unexpected(); }
void __stdcall putrect16(DWORD p, void *q) { (void)p; (void)q; unexpected(); }
void __stdcall getintarray16(DWORD p, int n, void *q) { (void)p; (void)n; (void)q; unexpected(); }
void *__stdcall malloc_w(unsigned n) { (void)n; unexpected(); return 0; }
void __stdcall free_w(void *p) { if(!seam_expected_free || p!=seam_expected_free || seam_free_count++) unexpected(); if(!HeapFree(GetProcessHeap(),0,p)) unexpected(); }
void *__stdcall GetPModeVDMPointerAssert(DWORD p) { (void)p; unexpected(); return 0; }
DWORD __stdcall WU32PostMessage(void *p) { (void)p; unexpected(); return 0; }
LRESULT __stdcall wow_native_SendMessageA(HWND h, UINT m, WPARAM w, LPARAM l) { (void)h; (void)m; (void)w; (void)l; unexpected(); return 0; }
void *aw32WOW[1];
