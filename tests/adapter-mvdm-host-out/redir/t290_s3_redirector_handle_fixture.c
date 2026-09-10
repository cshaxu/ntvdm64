#include <stdint.h>

#include "mvdm_redirector_handle.h"
#include "session/session.h"

int main(void)
{
    session instance;
    uintptr_t native_value = (uintptr_t)0x12345678u;
    HANDLE resolved;

    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_thread_bind(&instance))
        return 1;
    resolved = mvdm_redirector_handle_from_words((WORD)(native_value >> 16),
        (WORD)native_value);
    if ((uintptr_t)resolved != native_value) return 2;
    SetLastError(ERROR_SUCCESS);
    if (mvdm_redirector_handle_from_words(0u, 0u) != INVALID_HANDLE_VALUE ||
        GetLastError() != ERROR_INVALID_HANDLE) return 3;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 4;
    return 0;
}
