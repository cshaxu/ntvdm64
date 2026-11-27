#include <windows.h>
#include <shellapi.h>

/* XP wshell.c returns WORD and receives command, show and directory.
 * SHELL32 expects a DWORD result. Widen it at this synchronous host boundary;
 * preserve the original guest callback and its current-directory restoration.
 * Scope is thread-local and supports nested Shell calls and exception unwind. */
typedef WORD (WINAPI *wow_shell_guest_callback)(LPSTR, WORD, LPSTR);

typedef DWORD (WINAPI *modern_shell_callback)(LPSTR, WORD, LPSTR);
typedef HINSTANCE (WINAPI *modern_shell_execute)(HWND, LPCSTR, LPCSTR,
    LPSTR, LPCSTR, INT, modern_shell_callback);
static __declspec(thread) wow_shell_guest_callback current_callback;

static DWORD WINAPI invoke_original(LPSTR command, WORD show, LPSTR directory)
{
    return current_callback(command, show, directory);
}

HINSTANCE WINAPI wow_shell_execute(HWND window, LPCSTR operation, LPCSTR file,
    LPSTR parameters, LPCSTR directory, INT show, wow_shell_guest_callback callback)
{
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    modern_shell_execute execute = shell ?
        (modern_shell_execute)GetProcAddress(shell,"WOWShellExecute") : NULL;
    wow_shell_guest_callback previous;
    HINSTANCE result;
    if (!execute) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return (HINSTANCE)(ULONG_PTR)SE_ERR_DLLNOTFOUND;
    }
    previous = current_callback;
    current_callback = callback;
    __try {
        result = execute(window,operation,file,parameters,directory,show,
            callback ? invoke_original : NULL);
    } __finally {
        current_callback = previous;
    }
    return result;
}
