/* Finite modern binding for the original NTDLL DbgPrompt import. */

#include <windows.h>

unsigned long DbgPrompt(char *prompt, char *response,
    unsigned long maximum_response_length)
{
    typedef ULONG (NTAPI *DBG_PROMPT)(PCH, PCH, ULONG);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    DBG_PROMPT debugger_prompt = ntdll == NULL ? NULL :
        (DBG_PROMPT)GetProcAddress(ntdll, "DbgPrompt");

    if (debugger_prompt == NULL) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0u;
    }
    return debugger_prompt(prompt, response, maximum_response_length);
}
