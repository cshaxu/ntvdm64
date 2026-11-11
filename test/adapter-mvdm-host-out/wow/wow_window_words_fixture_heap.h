#include <windows.h>
void *WINAPI wow_words_fixture_alloc(HANDLE, DWORD, SIZE_T);
BOOL WINAPI wow_words_fixture_free(HANDLE, DWORD, void *);
BOOL WINAPI wow_words_fixture_setprop(HWND, LPCWSTR, HANDLE);
BOOL WINAPI wow_words_fixture_info(HWND, PWINDOWINFO);
LONG WINAPI wow_words_fixture_long(HWND, int);
/* Force-included only when compiling the binding for the isolated fixture. */
#define HeapAlloc wow_words_fixture_alloc
#define HeapFree wow_words_fixture_free
#define SetPropW wow_words_fixture_setprop
#define GetWindowInfo wow_words_fixture_info
#define GetWindowLongA wow_words_fixture_long
