#include "wow_dialog_native_bindings.h"
#include "wow_native_text_descriptor.h"
#include <stddef.h>
#include "wow_user_object_bindings.h"

HWND WINAPI wow_dialog_native_create_window(DWORD ex_style, ATOM class_atom,
    const struct _LARGE_STRING *title, DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HANDLE module, LPVOID parameter, DWORD version)
{
    wow_native_text_descriptor native_title;
    HWND native_parent;
    if (!class_atom) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    if (!wow_dialog_native_ready()) return NULL;
    /* Original dialog creation retains HWND32 for the parent.  The native
     * dialog service owns the actual HWND relationship, so resolve this one
     * published current-worker identity at the boundary rather than letting
     * win32u interpret a legacy carrier. */
    native_parent = wow_user_window_owned_handle(parent, TRUE);
    if (parent && !native_parent) return NULL;
    if (title) wow_native_text_capture(&native_title, title);
    return wow_native_create_dialog_window(ex_style, class_atom,
        title ? &native_title : NULL, style, x, y, width, height, native_parent,
        menu, module, parameter, version);
}
