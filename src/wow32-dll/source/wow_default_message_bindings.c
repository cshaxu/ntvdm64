#include "wow_default_message_bindings.h"

/* ADAPTER-WOW-063: original bitmap algorithm; finite host allocation owner.
 * The only USER operation is message-name registration; no window operation
 * or application callback is performed during once-initialization. */
wow_default_message_state_type wow_default_message_state;
static INIT_ONCE message_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK initialize_messages(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    BOOL complete = FALSE;
    UINT index;
    (void)once; (void)parameter; (void)context;
    __try {
        __try {
            InitMessageTables();
            /* Original server.c initializes uiShellMsg with the USER atom
             * "SHELLHOOK". RegisterWindowMessage uses that same namespace;
             * never call SetTaskmanWindow(NULL) on the host desktop. This
             * is the last fallible operation, avoiding repeated successful
             * atom registration when an earlier allocation must retry. */
            wow_default_message_state.uiShellMsg = RegisterWindowMessageW(L"SHELLHOOK");
            complete = wow_default_message_state.uiShellMsg != 0;
        } __finally {
            if (!complete) {
                DWORD error = GetLastError();
                if (wow_default_message_state.DefWindowMsgs.abMsgs)
                    HeapFree(GetProcessHeap(), 0, wow_default_message_state.DefWindowMsgs.abMsgs);
                if (wow_default_message_state.DefWindowSpecMsgs.abMsgs)
                    HeapFree(GetProcessHeap(), 0, wow_default_message_state.DefWindowSpecMsgs.abMsgs);
                for (index = 0; index < ARRAYSIZE(wow_default_message_state.awmControl); ++index)
                    if (wow_default_message_state.awmControl[index].abMsgs)
                        HeapFree(GetProcessHeap(), 0, wow_default_message_state.awmControl[index].abMsgs);
                ZeroMemory(&wow_default_message_state, sizeof(wow_default_message_state));
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
