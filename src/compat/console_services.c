#include "compat/include/console_services.h"

static const compat_console_services *binding;

BOOL compat_console_services_install(const compat_console_services *services)
{
    const compat_console_services *previous;
    if (!services || !services->code_page || !services->layout_name ||
            !services->get_title || !services->set_title) {
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

DWORD WINAPI MvdmGetConsoleTitleA(LPSTR title, DWORD capacity)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->get_title(title, capacity);
}

BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR title)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->set_title(title);
}
