#include <windows.h>

/* Test-owned allocation/registration boundaries; the original initializer runs. */
LPVOID WINAPI fixture_message_alloc(HANDLE heap, DWORD flags, SIZE_T bytes);
BOOL WINAPI fixture_message_free(HANDLE heap, DWORD flags, LPVOID memory);
UINT WINAPI fixture_message_register(LPCWSTR name);
#define HeapAlloc fixture_message_alloc
#define HeapFree fixture_message_free
#define RegisterWindowMessageW fixture_message_register
