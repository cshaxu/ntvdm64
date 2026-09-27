/* Test-only library-output boundary. Does NOT validate Raw Input/capture.
 * Called on the actual private Window UI thread; downstream worker input,
 * original ICA/IRQ and guest INT33 remain production code. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "lib/kvm-window/window.h"

LRESULT CALLBACK MouseInputHook(int code, WPARAM removed, LPARAM value)
{
    if (code == HC_ACTION && removed == PM_REMOVE) {
        MSG *message = (MSG *)value;
        if (message->message == WM_APP + 0x5f0) {
            /* Pinned win32/component.c context starts with kvm_window*. */
            kvm_window **context = (kvm_window **)GetWindowLongPtrW(message->hwnd, GWLP_USERDATA);
            if (context && *context) {
                kvm_component *component = &(*context)->base;
                kvm_input_event event = {0};
                event.source = component;
                event.source_identity = component->source_identity;
                event.type = KVM_EVENT_MOUSE;
                event.data.mouse.relative = LIB_TRUE;
                event.data.mouse.delta_x = (SHORT)LOWORD(message->lParam);
                event.data.mouse.delta_y = (SHORT)HIWORD(message->lParam);
                event.data.mouse.buttons = (lib_u32)message->wParam;
                if (message->wParam == 0x80000000u)
                    event.type = KVM_EVENT_SOURCE_RETIRED;
                if (component->input_sink)
                    component->input_sink(component->input_context, &event);
            }
            message->message = WM_NULL;
        }
    }
    return CallNextHookEx(NULL, code, removed, value);
}
