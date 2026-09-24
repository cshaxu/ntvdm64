#include "wow_callback_frame_lease.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

/* This is the fixed-width source contract exported by the SoftPC adapter.
 * Do not include the full historical SoftPC macro environment here: that
 * environment is owned by original mirror translation units, while this
 * narrow WOW binding only consumes this one numeric conversion. */
extern int __cdecl mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);
extern int __cdecl mvdm_softpc_protected_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);
extern unsigned short __cdecl getCS(void);
extern unsigned short __cdecl getIP(void);
extern unsigned short __cdecl getSS(void);
extern unsigned short __cdecl getSP(void);
extern unsigned short __cdecl getAX(void);
extern unsigned short __cdecl getBX(void);
extern unsigned short __cdecl getBP(void);
extern unsigned short __cdecl getSI(void);
extern unsigned short __cdecl getDI(void);

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

int wow_callback_frame_acquire_vp(uint32_t vp, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    uint32_t linear;

    if (!mvdm_softpc_effective_address((uint16_t)(vp >> 16),
        (uint16_t)vp, &linear)) return 0;
    return wow_callback_frame_acquire_linear(linear, byte_count, access,
        view_out);
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

void wow_callback_frame_trace(const char *phase, uint32_t ret_id,
    uint32_t vp_stack, uint32_t vp_callback_stack,
    uint32_t callback_linear, uint16_t task, uint32_t procedure,
    uint16_t message, uint32_t parameter)
{
    CHAR path[MAX_PATH];
    CHAR line[400];
    HANDLE file;
    DWORD length;
    DWORD written;
    DWORD last_error = GetLastError();

    if (GetEnvironmentVariableA("MVDM_WOW_CALLBACK_FRAME_TRACE_PATH", path,
        sizeof(path)) != 0u) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
            FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            length = (DWORD)sprintf_s(line, sizeof(line),
                "%lu %s ret=%04lX stack=%08lX cbstack=%08lX linear=%08lX "
                "task=%04X proc=%08lX msg=%04X param=%08lX cpu=%04X:%04X %04X:%04X "
                "regs=%04X:%04X:%04X:%04X:%04X\r\n",
                (unsigned long)GetCurrentProcessId(), phase,
                (unsigned long)ret_id, (unsigned long)vp_stack,
                (unsigned long)vp_callback_stack,
                (unsigned long)callback_linear, (unsigned)task,
                (unsigned long)procedure, (unsigned)message,
                (unsigned long)parameter, (unsigned)getCS(), (unsigned)getIP(),
                (unsigned)getSS(), (unsigned)getSP(), (unsigned)getAX(),
                (unsigned)getBX(), (unsigned)getBP(), (unsigned)getSI(),
                (unsigned)getDI());
            if (length != 0u) (void)WriteFile(file, line, length, &written,
                NULL);
            CloseHandle(file);
        }
    }
    SetLastError(last_error);
}
