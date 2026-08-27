#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

extern "C" {
#include "mvdm_command_registers.h"
#include "dpmi_session_state.h"
#include "session/session.h"

void DpmiInitDosx(void);
void DpmiInitApp(void);
void DpmiPassTableAddress(void);
void DpmiAllocateXmem(void);
void DpmiFreeXmem(void);
void DpmiReallocateXmem(void);
void DpmiFreeAppXmem(void);
void DpmiFreeAllXmem(void);
void DpmiSetDescriptorEntry(void);
void DpmiGetMemoryInfo(void);

void DpmiSetProtectedmodeInterrupt(void) {}
void switch_to_protected_mode(void) {}
void DpmiGetFastBopEntry(void) {}
void DpmiXlatInt21Call(void) {}
void DpmiSetFaultHandler(void) {}
void DpmiDpmiInUse(void) {}
void DpmiDpmiNoLongerInUse(void) {}
void DpmiSetDebugRegisters(void) {}
void DpmiPassPmStackInfo(void) {}
void DpmiVcdPmSvcCall32(void) {}
void DpmiIntHandlerIret16(void) {}
void DpmiIntHandlerIret32(void) {}
void DpmiFaultHandlerIret16(void) {}
void DpmiFaultHandlerIret32(void) {}
void DpmiUnhandledExceptionHandler(void) {}
unsigned char *Sim32pGetVDMPointer(unsigned long, unsigned char) { return 0; }
void *ExtMemSA = 0;
}

#include <string.h>

static int stop_on_ud(void *, const void *event, unsigned event_bytes,
    void *outcome, unsigned outcome_bytes)
{
    const runtime_generic_ud_event *source =
        (const runtime_generic_ud_event *)event;
    runtime_generic_ud_outcome *result =
        (runtime_generic_ud_outcome *)outcome;
    if (source == 0 || event_bytes != sizeof(*source) ||
        outcome_bytes != sizeof(*result) || source->vector != 6u) return 0;
    memset(result, 0, sizeof(*result));
    result->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
    result->disposition = RUNTIME_GENERIC_UD_STOP;
    return 1;
}

static int machine_read(void *, uint32_t address, uint8_t *bytes,
    uint32_t count)
{
    return machine_facade_memory_read(address, count, bytes);
}

static int machine_write(void *, uint32_t address, const uint8_t *bytes,
    uint32_t count)
{
    return machine_facade_memory_write(address, count, bytes);
}

static void put16(uint8_t *bytes, uint32_t offset, uint16_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *bytes, uint32_t offset, uint32_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
    bytes[offset + 2u] = (uint8_t)(value >> 16);
    bytes[offset + 3u] = (uint8_t)(value >> 24);
}

static int enter_protected_stop(void)
{
    static const uint8_t gdt[] = {
        0x17, 0x00, 0x20, 0x08, 0x00, 0x00, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
    };
    static const uint8_t transition[] = {
        0x0f, 0x01, 0x16, 0x00, 0x08,
        0x66, 0xb8, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x22, 0xc0,
        0xea, 0x00, 0x09, 0x08, 0x00
    };
    static const uint8_t protected_stop[] = {
        0x66, 0xb8, 0x10, 0x00,
        0x8e, 0xd8, 0x8e, 0xc0, 0x8e, 0xd0,
        0x0f, 0x0b
    };
    if (!machine_facade_machine_begin(0x400000u, 0x400000u) ||
        !machine_facade_memory_write(0x800u, sizeof(gdt), gdt) ||
        !machine_facade_memory_write(0x700u, sizeof(transition), transition) ||
        !machine_facade_memory_write(0x900u, sizeof(protected_stop),
            protected_stop) || !machine_facade_bind_opaque_callback(stop_on_ud,
                0)) return 0;
    machine_facade_apply_real_mode_entry(0x70u, 0u);
    machine_facade_cpu_loop();
    return 1;
}

int main()
{
    session instance;
    mvdm_dpmi_session_state_snapshot state;
    machine_facade_protected_frame frame;
    uint8_t shared[58];
    uint8_t app[6];
    uint8_t descriptor[8] = { 0xffu, 0xffu, 0u, 0u, 0u, 0x92u, 0x40u, 0u };
    uint8_t memory_info[36];
    uint16_t handle_hi, handle_lo;
    int result = 1;

    if (!enter_protected_stop()) goto done;
    session_initialize(&instance, 289u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, 0, machine_read, machine_write)) {
        result = 2; goto done;
    }

    memset(shared, 0, sizeof(shared));
    put16(shared, 0u, 0x1234u); put32(shared, 2u, 0x11112222u);
    put32(shared, 6u, 0x33334444u); put32(shared, 10u, 0x55556666u);
    put16(shared, 14u, 0x77u); put32(shared, 16u, 0x88889999u);
    put16(shared, 20u, 0xaaaau); put32(shared, 22u, 0xbbbbccccu);
    put16(shared, 26u, 0x10u); put16(shared, 28u, 0x8u);
    put32(shared, 30u, 0xddddeeeeu); put32(shared, 34u, 0x01020304u);
    put32(shared, 38u, 0x05060708u); put32(shared, 42u, 0x11121314u);
    put32(shared, 46u, 0x15161718u); put32(shared, 50u, 0x21222324u);
    put32(shared, 54u, 0x25262728u);
    if (!machine_facade_memory_write(0x2000u, sizeof(shared), shared) ||
        !machine_facade_set_si16(0x2000u)) { result = 3; goto done_session; }
    DpmiInitDosx();
    if (!mvdm_dpmi_session_snapshot(&state) || state.dosx_stack_segment !=
        0x1234u || state.small_xlat_buffer != 0x11112222u ||
        state.dosx_iretd != 0x25262728u) { result = 4; goto done_session; }

    put16(app, 0u, 0x2440u); put16(app, 2u, 0x10u); put16(app, 4u, 0x28u);
    if (!machine_facade_memory_write(0x2400u, sizeof(app), app) ||
        !machine_facade_set_sp16(0x2400u) || !machine_facade_set_ax16(1u)) {
        result = 5; goto done_session;
    }
    DpmiInitApp();
    if (!mvdm_dpmi_session_snapshot(&state)) { result = 61; goto done_session; }
    if (state.current_app_flags != 1u) { result = 62; goto done_session; }
    if (state.current_dta != 0x00102440u) { result = 63; goto done_session; }
    if (state.current_psp_selector != 0x28u) { result = 64; goto done_session; }

    if (!machine_facade_set_ax16(0x20u)) { result = 7; goto done_session; }
    DpmiPassTableAddress();
    if (!mvdm_dpmi_session_snapshot(&state) || state.selector_table !=
        0x00200000u) { result = 8; goto done_session; }

    if (!machine_facade_set_bx16(1u) || !machine_facade_set_cx16(0u) ||
        !machine_facade_set_dx16(0x28u)) { result = 9; goto done_session; }
    DpmiAllocateXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || (frame.eflags & 1u)) {
        result = 10; goto done_session;
    }
    handle_hi = getSI(); handle_lo = getDI();
    if (handle_hi == 0u && handle_lo == 0u) { result = 11; goto done_session; }
    if (!machine_facade_set_bx16(2u) || !machine_facade_set_cx16(0u)) {
        result = 12; goto done_session;
    }
    DpmiReallocateXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || (frame.eflags & 1u)) {
        result = 13; goto done_session;
    }
    DpmiFreeXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || (frame.eflags & 1u)) {
        result = 14; goto done_session;
    }

    /* 53:10 keeps the original per-owner sweep.  The stale opaque identity
     * must then take the original carry failure of DpmiFreeXmem. */
    if (!machine_facade_set_bx16(1u) || !machine_facade_set_cx16(0u) ||
        !machine_facade_set_dx16(0x28u)) { result = 141; goto done_session; }
    DpmiAllocateXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || (frame.eflags & 1u)) {
        result = 142; goto done_session;
    }
    handle_hi = getSI(); handle_lo = getDI();
    DpmiFreeAppXmem();
    if (!machine_facade_set_si16(handle_hi) || !machine_facade_set_di16(handle_lo)) {
        result = 143; goto done_session;
    }
    DpmiFreeXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || !(frame.eflags & 1u)) {
        result = 144; goto done_session;
    }

    /* 53:13 retains the original whole-list release and also retires every
     * session opaque identity which had represented a released XMEM block. */
    if (!machine_facade_set_bx16(1u) || !machine_facade_set_cx16(0u) ||
        !machine_facade_set_dx16(0x29u)) { result = 145; goto done_session; }
    DpmiAllocateXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || (frame.eflags & 1u)) {
        result = 146; goto done_session;
    }
    handle_hi = getSI(); handle_lo = getDI();
    DpmiFreeAllXmem();
    if (!machine_facade_set_si16(handle_hi) || !machine_facade_set_di16(handle_lo)) {
        result = 147; goto done_session;
    }
    DpmiFreeXmem();
    if (machine_facade_copy_protected_frame(&frame) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || !(frame.eflags & 1u)) {
        result = 148; goto done_session;
    }

    if (!machine_facade_memory_write(0x2500u, sizeof(descriptor), descriptor) ||
        !machine_facade_set_ax16(0x20u) || !machine_facade_set_bx16(0x2500u) ||
        !machine_facade_set_cx16(1u)) { result = 15; goto done_session; }
    DpmiSetDescriptorEntry();
    if (getAX() != 0x20u) { result = 16; goto done_session; }

    memset(memory_info, 0, sizeof(memory_info));
    if (!machine_facade_memory_write(0x2600u, sizeof(memory_info), memory_info) ||
        !machine_facade_set_di16(0x2600u)) { result = 17; goto done_session; }
    DpmiGetMemoryInfo();
    if (!machine_facade_memory_read(0x2600u, sizeof(memory_info), memory_info) ||
        memory_info[0] == 0u && memory_info[1] == 0u && memory_info[2] == 0u &&
        memory_info[3] == 0u) { result = 18; goto done_session; }
    result = 0;

done_session:
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        result = result == 0 ? 19 : result;
done:
    machine_facade_unbind_opaque_callback();
    if (!machine_facade_machine_cleanup() && result == 0) result = 20;
    return result;
}
