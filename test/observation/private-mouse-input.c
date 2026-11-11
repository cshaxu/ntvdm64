/* Test-only library-output boundary. Does NOT validate Raw Input/capture.
 * Called on the actual private Window UI thread; downstream worker input,
 * original ICA/IRQ and guest INT33 remain production code. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "lib-nxvm/lib/kvm-window/window.h"

#ifdef WIN31_CONSOLE_API_TRACE
BOOL win31_console_api_on_message(void);
#endif
static unsigned accepted;

LRESULT CALLBACK MouseInputHook(int code, WPARAM removed, LPARAM value)
{
#ifdef WIN31_CONSOLE_API_TRACE
    if (!win31_console_api_on_message()) return CallNextHookEx(NULL, code, removed, value);
#endif
    if (code == HC_ACTION && removed == PM_REMOVE) {
        MSG *message = (MSG *)value;
        if (message->message == WM_APP + 0x5f1) {
            WCHAR name[96];
            HANDLE acknowledgment;
            swprintf_s(name,96,L"Local\\NTVDM-Mouse-Probe-%lu-%lu",
                GetCurrentProcessId(),GetCurrentThreadId());
            acknowledgment=OpenEventW(EVENT_MODIFY_STATE,FALSE,name);
            if(acknowledgment) {
                if(accepted==(unsigned)message->wParam)SetEvent(acknowledgment);
                CloseHandle(acknowledgment);
            }
            accepted=0;message->message=WM_NULL;
        }
        if (message->message == WM_APP + 0x5f0 || message->message == WM_APP + 0x5f2) {
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
                if (message->message == WM_APP + 0x5f2) {
                    event.type = KVM_EVENT_KEY;
                    event.data.key.scan_code = LOWORD(message->lParam);
                    event.data.key.key = (base_key)message->wParam;
                    event.data.key.flags = 0;
                    event.data.key.pressed = ((ULONG_PTR)message->lParam >> 16) & 1u;
                    event.data.key.modifiers = (lib_u8)((ULONG_PTR)message->lParam >> 24);
                } else
                if (message->wParam == 0x80000000u)
                    event.type = KVM_EVENT_SOURCE_RETIRED;
                if (component->input_sink &&
                    component->input_sink(component->input_context, &event)) {
                    WCHAR name[96];HANDLE delivered;
                    ++accepted;
                    /* A single input can retire this HWND before a later
                     * acknowledgment message. Confirm acceptance here. The
                     * existing burst observer still uses its count barrier. */
                    swprintf_s(name,96,L"Local\\NTVDM-Mouse-Probe-%lu-%lu-Input",
                        GetCurrentProcessId(),GetCurrentThreadId());
                    delivered=OpenEventW(EVENT_MODIFY_STATE,FALSE,name);
                    if(delivered){SetEvent(delivered);CloseHandle(delivered);}
                }
            }
            message->message = WM_NULL;
        }
    }
    return CallNextHookEx(NULL, code, removed, value);
}
