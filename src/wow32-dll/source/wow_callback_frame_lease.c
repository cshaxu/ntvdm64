#include "wow_callback_frame_lease.h"

#include <windows.h>
#include <string.h>

#include "common/guest-memory/access.h"
#include "wow_task_order_bindings.h"

/* This is the fixed-width source contract exported by the SoftPC adapter.
 * Do not include the full historical SoftPC macro environment here: that
 * environment is owned by original mirror translation units, while this
 * narrow WOW binding only consumes this one numeric conversion. */
extern int __cdecl mvdm_softpc_protected_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);
extern unsigned short __cdecl getCS(void);
extern unsigned short __cdecl getIP(void);
/* ADAPTER-WOW-099: existing original CCPU bodies, selected Win32 ABI. */
extern int32_t __cdecl c_getPE(void);
extern int32_t __cdecl c_getVM(void);
extern int32_t __cdecl c_getCPL(void);
extern void __cdecl c_setPE(int32_t value);
extern void __cdecl c_setVM(int32_t value);
extern void __cdecl c_setCPL(int32_t value);
extern int32_t __cdecl c_setCS(uint16_t value);
extern int32_t __cdecl c_setSS(uint16_t value);
extern void __cdecl setSP(uint16_t value);
extern void __cdecl setIP(uint16_t value);
extern BOOL WINAPI wow_user_worker_active(void);

void __cdecl wow_callback_control_capture(wow_callback_control *control)
{
    control->cs = getCS();
    control->ip = getIP();
    control->pe = c_getPE();
    control->vm = c_getVM();
    control->cpl = c_getCPL();
    control->captured = 1;
}

int __cdecl wow_callback_control_restore(const wow_callback_control *control,
    uint32_t caller_stack)
{
    DWORD error = GetLastError();
    if (!wow_user_worker_active()) { SetLastError(error); return 0; }
    if (control->captured) {
        /* Reload against current descriptor/memory authorities. In particular
         * no saved CR3, descriptor cache, video or NPX value is reinstated. */
        c_setPE(control->pe);
        c_setVM(control->vm);
        c_setCPL(control->cpl);
        if (c_setCS(control->cs) || c_setSS((uint16_t)(caller_stack >> 16)))
            wow_user_native_call_failed("callback-control-selector", ERROR_INVALID_ADDRESS);
        setSP((uint16_t)caller_stack);
        setIP(control->ip);
    }
    SetLastError(error);
    return 1;
}

int wow_callback_frame_acquire_linear(uint32_t guest_address, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    guest_memory_lease *lease;
    uint8_t *bytes;

    if (view_out == NULL || byte_count == 0u ||
        (access != GUEST_MEMORY_ACCESS_READ &&
         access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    memset(view_out, 0, sizeof(*view_out));
    if (!mvdm_softpc_guest_memory_acquire(guest_address, byte_count,
        access, &lease, &bytes)) return 0;
    view_out->lease = lease;
    view_out->bytes = bytes;
    view_out->guest_address = guest_address;
    view_out->byte_count = byte_count;
    view_out->access = access;
    return 1;
}

int wow_callback_frame_acquire_protected_vp(uint32_t vp, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    uint32_t linear;

    if (view_out == NULL) return 0;
    memset(view_out, 0, sizeof(*view_out));
    if (!mvdm_softpc_protected_address((uint16_t)(vp >> 16),
        (uint16_t)vp, &linear)) return 0;
    return wow_callback_frame_acquire_linear(linear, byte_count, access,
        view_out);
}

int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    int result;

    if (view == NULL || view->lease == NULL ||
        view->bytes == NULL || view->byte_count == 0u ||
        (view->access != GUEST_MEMORY_ACCESS_READ &&
         view->access != GUEST_MEMORY_ACCESS_WRITE) ||
        (commit != 0 && commit != 1) ||
        (commit != 0 && view->access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    result = mvdm_softpc_guest_memory_release(view->lease, commit);
    memset(view, 0, sizeof(*view));
    return result;
}
