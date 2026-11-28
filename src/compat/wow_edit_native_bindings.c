#include "wow_edit_native_bindings.h"
#include <imm.h>
#include "wow_native_text_descriptor.h"

static wow_text_capture_provider text_capture;

BOOL WINAPI wow_native_text_bind(wow_text_capture_provider capture)
{
    if (!capture || (text_capture && text_capture != capture)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    text_capture = capture;
    return TRUE;
}

void WINAPI wow_native_text_capture(wow_native_text_descriptor *target,
    const struct _LARGE_STRING *source)
{
    if (!text_capture) RaiseFailFastException(NULL, NULL, 0);
    text_capture(target, source);
}

BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND window, const struct _LARGE_STRING *text)
{
    wow_native_text_descriptor native_text;
    if (!wow_edit_native_ready()) return FALSE;
    if (text) wow_native_text_capture(&native_text, text);
    return wow_native_set_window_text(window, text ? &native_text : NULL);
}

/* XP windows/core/ntuser/client/imectl.c:1549-1570. Original synchronous
 * soft-keyboard conversion callback; native public IMM is the provider. */
BOOL WINAPI SyncSoftKbdState(
    HIMC hImc,
    LPARAM lParam)
{
    DWORD fdwConversion, fdwSentence, fdwNewConversion;

    ImmGetConversionStatus(hImc, &fdwConversion, &fdwSentence);

    if (lParam) {
        fdwNewConversion = fdwConversion | IME_CMODE_SOFTKBD;
    } else {
        fdwNewConversion = fdwConversion & ~IME_CMODE_SOFTKBD;
    }

    if (fdwNewConversion != fdwConversion) {
        ImmSetConversionStatus(hImc, fdwNewConversion, fdwSentence);
    }

    return TRUE;
}
