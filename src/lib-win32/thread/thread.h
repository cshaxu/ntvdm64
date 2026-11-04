#ifndef LIB_WIN32_THREAD_H
#define LIB_WIN32_THREAD_H
#include <windows.h>
typedef DWORD (__cdecl *win32_thread_cdecl)(LPVOID);
typedef VOID (__cdecl *win32_thread_void)(VOID);
typedef VOID (__cdecl *win32_thread_void_parameter)(LPVOID);
typedef struct win32_thread_entry {
    win32_thread_cdecl cdecl_entry;
    LPTHREAD_START_ROUTINE winapi_entry;
    win32_thread_void void_entry;
    win32_thread_void_parameter void_parameter_entry;
} win32_thread_entry;
typedef struct win32_thread_hooks {
    void *owner;
    const char *source_name;
    int (*enter)(void *, const char *);
    void (*finish)(void);
} win32_thread_hooks;
HANDLE win32_thread_create(LPSECURITY_ATTRIBUTES, SIZE_T, win32_thread_entry,
    LPVOID, DWORD, LPDWORD, const win32_thread_hooks *);
#endif
