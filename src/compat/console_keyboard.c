#include "compat/include/console_keyboard.h"

static const compat_console_keyboard *binding;

BOOL compat_console_keyboard_install(const compat_console_keyboard *services)
{
    const compat_console_keyboard *previous;
    if (!services || !services->code_page || !services->layout_name) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)services, NULL);
    if (previous && previous != services) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

UINT WINAPI MvdmGetConsoleCP(void)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->code_page();
}

BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->layout_name(name);
}
