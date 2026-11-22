#include <windows.h>
#include <winternl.h>
#include "wow_default_message_bindings.h"

/* ADAPTER-WOW-063: original bitmap algorithm; finite host allocation owner.
 * The only USER operation is message-name registration; no window operation
 * or application callback is performed during once-initialization. */
UINT wow_default_shell_message;
static INIT_ONCE message_once = INIT_ONCE_STATIC_INIT;
static const wow_default_message_provider *message_provider;

BOOL WINAPI wow_default_messages_bind(const wow_default_message_provider *provider)
{
    const wow_default_message_provider *previous;
    if (!provider || !provider->initialize || !provider->merge || !provider->reset) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&message_provider,
        (PVOID)provider, NULL);
    if (previous && previous != provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}

WORD WINAPI wow_default_messages_merge(PBYTE bits, WORD count)
{
    if (!message_provider) RaiseFailFastException(NULL, NULL, 0);
    return message_provider->merge(bits, count);
}

static BOOL CALLBACK initialize_messages(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    BOOL complete = FALSE;
    (void)once; (void)parameter; (void)context;
    __try {
        __try {
            if (!message_provider) RaiseFailFastException(NULL, NULL, 0);
            message_provider->initialize();
            /* Original server.c initializes uiShellMsg with the USER atom
             * "SHELLHOOK". RegisterWindowMessage uses that same namespace;
             * never call SetTaskmanWindow(NULL) on the host desktop. This
             * is the last fallible operation, avoiding repeated successful
             * atom registration when an earlier allocation must retry. */
            wow_default_shell_message = RegisterWindowMessageW(L"SHELLHOOK");
            complete = wow_default_shell_message != 0;
        } __finally {
            if (!complete) {
                DWORD error = GetLastError();
                message_provider->reset();
                wow_default_shell_message = 0;
                SetLastError(error);
            }
        }
    } __except (GetExceptionCode() == STATUS_NO_MEMORY ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (!complete) return FALSE;
    /* Immutable until process exit. INIT_ONCE supplies publication ordering;
     * readers pass the same gate;failure frees and clears the table before retry. */
    return TRUE;
}

BOOL WINAPI wow_default_messages_ready(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&message_once, initialize_messages, NULL, NULL))
        return FALSE;
    SetLastError(error);
    return TRUE;
}
