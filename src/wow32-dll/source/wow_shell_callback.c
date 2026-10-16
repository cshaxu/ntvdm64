#include "precomp.h"
#include "wowshlp.h"

/* The selected NT4 wowshlp.h callback has two parameters. The modern x86
 * SHELL32 provider supplies three stdcall slots (command, show, directory).
 * Keep original wshell conversion/WinExec and its callback contract intact;
 * adapt only this private host boundary, not the guest or Shell resolver.
 * Scope is synchronous and thread-local, including nested shell calls. */
typedef DWORD (WINAPI *modern_shell_callback)(LPSTR, WORD, LPSTR);
typedef HINSTANCE (WINAPI *modern_shell_execute)(HWND, LPCSTR, LPCSTR,
    LPSTR, LPCSTR, INT, modern_shell_callback);
static __declspec(thread) LPFNWOWSHELLEXECCB current_callback;

static DWORD WINAPI invoke_original(LPSTR command, WORD show, LPSTR directory)
{
    /* Original shlexec.c passes only command/show; wshell.c itself owns
     * UpdateDosCurrentDirectory. Do not introduce process-global cwd changes
     * or replace its original WinExec path merely to consume a new ABI slot. */
    (void)directory;
    return current_callback(command, show);
}

HINSTANCE WINAPI wow_shell_execute(HWND window, LPCSTR operation, LPCSTR file,
    LPSTR parameters, LPCSTR directory, INT show, LPFNWOWSHELLEXECCB callback)
{
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    modern_shell_execute execute = shell ?
        (modern_shell_execute)GetProcAddress(shell,"WOWShellExecute") : NULL;
    LPFNWOWSHELLEXECCB previous;
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
