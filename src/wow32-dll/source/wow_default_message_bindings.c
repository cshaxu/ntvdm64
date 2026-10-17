#include "wow_default_message_bindings.h"

/* ADAPTER-WOW-063: original bitmap algorithm; finite host allocation owner.
 * The only USER operation is message-name registration; no window operation
 * or application callback is performed during once-initialization. */
wow_default_message_state_type wow_default_message_state;
static INIT_ONCE message_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK initialize_messages(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    wow_default_message_state_type candidate = {0};
    BOOL complete = FALSE;
    UINT index;
    (void)once; (void)parameter; (void)context;
    __try {
        __try {
            InitWindowMsgTable(&candidate.DefWindowMsgs.abMsgs,
                &candidate.DefWindowMsgs.maxMsgs, gawDefWindowMsgs);
            InitWindowMsgTable(&candidate.DefWindowSpecMsgs.abMsgs,
                &candidate.DefWindowSpecMsgs.maxMsgs, gawDefWindowSpecMsgs);
            InitWindowMsgTable(&candidate.awmControl[FNID_BUTTON - FNID_START].abMsgs,
                &candidate.awmControl[FNID_BUTTON - FNID_START].maxMsgs, gawButtonWndProc);
            InitWindowMsgTable(&candidate.awmControl[FNID_EDIT - FNID_START].abMsgs,
                &candidate.awmControl[FNID_EDIT - FNID_START].maxMsgs, gawEditWndProc);
            InitWindowMsgTable(&candidate.awmControl[FNID_LISTBOX - FNID_START].abMsgs,
                &candidate.awmControl[FNID_LISTBOX - FNID_START].maxMsgs, gawListboxWndProc);
            InitWindowMsgTable(&candidate.awmControl[FNID_COMBOBOX - FNID_START].abMsgs,
                &candidate.awmControl[FNID_COMBOBOX - FNID_START].maxMsgs, gawComboWndProc);
            InitWindowMsgTable(&candidate.awmControl[FNID_COMBOLISTBOX - FNID_START].abMsgs,
                &candidate.awmControl[FNID_COMBOLISTBOX - FNID_START].maxMsgs, gawListboxWndProc);
            /* Original server.c initializes uiShellMsg with the USER atom
             * "SHELLHOOK". RegisterWindowMessage uses that same namespace;
             * never call SetTaskmanWindow(NULL) on the host desktop. This
             * is the last fallible operation, avoiding repeated successful
             * atom registration when an earlier allocation must retry. */
            candidate.uiShellMsg = RegisterWindowMessageW(L"SHELLHOOK");
            complete = candidate.uiShellMsg != 0;
        } __finally {
            if (!complete) {
                DWORD error = GetLastError();
                if (candidate.DefWindowMsgs.abMsgs)
                    HeapFree(GetProcessHeap(), 0, candidate.DefWindowMsgs.abMsgs);
                if (candidate.DefWindowSpecMsgs.abMsgs)
                    HeapFree(GetProcessHeap(), 0, candidate.DefWindowSpecMsgs.abMsgs);
                for (index = 0; index < ARRAYSIZE(candidate.awmControl); ++index)
                    if (candidate.awmControl[index].abMsgs)
                        HeapFree(GetProcessHeap(), 0, candidate.awmControl[index].abMsgs);
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
     * a failed attempt publishes none of the tables and may be retried. */
    wow_default_message_state = candidate;
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
