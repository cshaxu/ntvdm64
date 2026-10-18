#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_runtime.h"
#include "wow_class_client_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_window_procedure_entry.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

static DWORD decoded_procedure(DWORD native, BOOL *guest)
{
    wow_window_dispatch_target target;
    *guest = wow_window_entry_guest_target((WNDPROC)native, &target);
    return *guest ? target.procedure : native;
}

DWORD WINAPI wow_user_get_procedure(HWND window, BOOL class_procedure, BOOL *guest)
{
    DWORD native;
    DWORD error = GetLastError();
    wow_window_dispatch_target target;
    if (!guest) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    if (class_procedure && wow_window_class_target_read(window, &target)) {
        *guest = target.wow_encoded;
        SetLastError(error);
        return target.procedure;
    }
    SetLastError(error);
    native = class_procedure ? GetClassLongA(window, GCL_WNDPROC) :
        (DWORD)GetWindowLongA(window, GWL_WNDPROC);
    return decoded_procedure(native, guest);
}

DWORD WINAPI wow_user_set_procedure(HWND window, BOOL class_procedure,
    DWORD procedure, BOOL guest, BOOL *previous_guest)
{
    wow_window_dispatch_target target;
    wow_user_runtime_thread *runtime;
    wow_window_words_binding *binding;
    DWORD previous, error = GetLastError();
    if (!previous_guest) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    *previous_guest = FALSE;
    if (!guest && procedure == (DWORD)wow_window_class_creation_proc) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    if (class_procedure) {
        if (wow_window_class_target_read(window, &target)) {
            SetLastError(error);
            return wow_window_class_procedure_exchange(window, procedure, guest,
                &previous, previous_guest) ? previous : 0;
        }
        /* Unknown native classes retain native USER's contract. Original
         * CLS behavior storage is not invented for an unowned registration. */
        if (guest) { SetLastError(ERROR_INVALID_STATE); return 0; }
        SetLastError(error);
        previous = wow_native_SetClassLongA(window, GCL_WNDPROC, (LONG)procedure);
        return decoded_procedure(previous, previous_guest);
    }
    if (guest) {
        runtime = wow_user_runtime_current();
        if (!runtime || !runtime->classes || !runtime->classes->callback ||
                !(procedure & WOW_WINDOW_PROC_TAG)) {
            SetLastError(ERROR_INVALID_STATE); return 0;
        }
        target.procedure = procedure;
        target.callback = runtime->classes->callback;
        target.wow_encoded = TRUE;
        /* Allocate before touching either native slot. A saved old entry is
         * immutable, including when this guest address resembles native CPD. */
        procedure = (DWORD)wow_window_entry_for_guest(&target);
        if (!procedure) return 0;
        binding = wow_window_words_acquire(window);
        if (!binding) {
            SetLastError(error);
            if (!wow_window_procedure_adopt_native(window, target.procedure,
                    &previous)) return 0;
            return decoded_procedure(previous, previous_guest);
        }
        wow_window_words_release(binding);
    }
    /* USER consumes a native CPD and its conversion direction atomically.
     * Never retain its source-owned token as the destination's current proc,
     * resolve its private layout, or perform install/query/restore. */
    SetLastError(error);
    previous = (DWORD)wow_native_SetWindowLongA(window, GWL_WNDPROC, (LONG)procedure);
    return decoded_procedure(previous, previous_guest);
}

/* ADAPTER-WOW-033/072: bind private indices to owned WW/WC aliases. Public
 * procedure selection belongs to the real native slot; decode only our own
 * immutable guest entrance, never skip a real native subclass above it.
 * Native USER retains public handle/process and CPD conversion checks.
 * A selected thunk or
 * callback must own the enclosing alias scope. Original class clones share
 * these same two WC words rather than independently copied adapter state. */
LONG WINAPI wow_user_get_window_longA(HWND window, int index)
{
    PWW words;
    LONG client_result;
    if (wow_window_client_try_get_long(window, index, TRUE, &client_result))
        return client_result;
    if (index == GWL_WNDPROC) {
        BOOL guest;
        return (LONG)wow_user_get_procedure(window, FALSE, &guest);
    }
    if (index != GWL_WOWWORDS &&
            (index > GWL_WOWDWORD1 || index < GWL_WOWDWORD3))
        return wow_native_GetWindowLongA(window, index);
    words = wow_user_borrow_window(window);
    if (!words) return 0;
    if (index == GWL_WOWWORDS) return (LONG)words;
    return (LONG)words->adwWOW[GWL_WOWDWORD1 - index];
}

LONG WINAPI wow_user_set_window_longA(HWND window, int index, LONG value)
{
    DWORD previous_word;
    LONG client_previous;
    if (wow_window_client_try_set_long(window, index, value, TRUE, &client_previous))
        return client_previous;
    if (index == GWL_WNDPROC) {
        /* Untyped public/native calls carry native procedures. Original WOW
         * guest branches use the explicit typed service above. Do not guess
         * source intent from the numeric tag or normalize native CPDs here. */
        return wow_native_SetWindowLongA(window, index, value);
    }
    if (index > GWL_WOWDWORD1 || index < GWL_WOWDWORD3)
        return wow_native_SetWindowLongA(window, index, value);
    /* Original private branches store the word but leave dwOld uninitialized.
     * Selected SETWL callers discard it. Return the actual previous word,
     * never reproduce an indeterminate native return register. */
    if (!wow_user_borrow_window_exchange(window, GWL_WOWDWORD1 - index,
            (DWORD)value, &previous_word)) return 0;
    return (LONG)previous_word;
}

DWORD WINAPI wow_user_get_class_longA(HWND window, int index)
{
    if (index == GCL_WNDPROC) {
        BOOL guest;
        return wow_user_get_procedure(window, TRUE, &guest);
    }
    if (index != GCL_WOWWORDS) return GetClassLongA(window, index);
    return (DWORD)wow_user_borrow_window_class(window);
}

DWORD WINAPI wow_user_set_class_longA(HWND window, int index, LONG value)
{
    DWORD *words;
    if (index == GCL_WNDPROC) {
        BOOL previous_guest;
        return wow_user_set_procedure(window, TRUE, (DWORD)value, FALSE, &previous_guest);
    }
    if (index != GCL_WOWDWORD1 && index != GCL_WOWDWORD2)
        return wow_native_SetClassLongA(window, index, value);
    words = wow_user_borrow_window_class(window);
    if (!words) return 0;
    /* Same deterministic previous-value boundary as the private WW setter. */
    return (DWORD)InterlockedExchange((LONG *)&words[GCL_WOWDWORD1 - index], value);
}

BOOL WINAPI wow_user_get_class_infoA(HINSTANCE module, LPCSTR name, LPWNDCLASSA output)
{
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    return wow_class_client_query(runtime ? runtime->classes : NULL,
        module, name, output);
}

BOOL WINAPI wow_user_unregister_classA(LPCSTR name, HINSTANCE module)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL held, result = FALSE;
    DWORD error;
    if (!binding || !binding->classes) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    held = binding->exclusive_held;
    if (!held && !wow_user_runtime_enter(binding)) return FALSE;
    /* Original ntcftxt owns capture, _UnregisterClass selection and returned
     * menu-name release. Unregister cannot call a window procedure: a live
     * class reference is rejected by the original owner and native USER. */
    __try { result = wow_class_client_unregister(binding->classes, name, module); }
    __finally {
        error = GetLastError();
        if (!held && !wow_user_runtime_leave(binding))
            wow_user_native_call_failed("restore-unregister-owner", GetLastError());
        SetLastError(error);
    }
    return result;
}
