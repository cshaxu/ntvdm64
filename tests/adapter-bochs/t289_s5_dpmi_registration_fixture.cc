#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

extern "C" {
#include "mvdm_command_registers.h"
#include "mvdm_host_identity.h"
#include "session/session.h"
#include "vdm.h"

void DpmiSetProtectedmodeInterrupt(void);
void DpmiSetFaultHandler(void);
void DpmiPassPmStackInfo(void);
void BeginUseLockedPMStack(void);
int EndUseLockedPMStack(void);
unsigned char *Sim32pGetVDMPointer(unsigned long, unsigned char) { return 0; }
void *Sim32GetVDMPointer(unsigned long, unsigned char) { return 0; }
void RcErrorDialogBox(unsigned long, char *, void *) {}
void setDS(unsigned short) {}
void setES(unsigned short) {}
void setFS(unsigned short) {}
void setGS(unsigned short) {}
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

static int machine_read(void *, uint32_t address, uint8_t *bytes, uint32_t count)
{ return machine_facade_memory_read(address, count, bytes); }
static int machine_write(void *, uint32_t address, const uint8_t *bytes, uint32_t count)
{ return machine_facade_memory_write(address, count, bytes); }
static void put16(uint8_t *bytes, uint32_t offset, uint16_t value)
{ bytes[offset] = (uint8_t)value; bytes[offset + 1u] = (uint8_t)(value >> 8); }
static void put32(uint8_t *bytes, uint32_t offset, uint32_t value)
{ bytes[offset] = (uint8_t)value; bytes[offset + 1u] = (uint8_t)(value >> 8); bytes[offset + 2u] = (uint8_t)(value >> 16); bytes[offset + 3u] = (uint8_t)(value >> 24); }

static int enter_protected_stop(void)
{
    static const uint8_t gdt[] = {0x17,0,0x20,8,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0xff,0xff,0,0,0,0x9a,0xcf,0, 0xff,0xff,0,0,0,0x92,0xcf,0};
    static const uint8_t transition[] = {0x0f,1,0x16,0,8,0x66,0xb8,1,0,0,0,0x0f,0x22,0xc0,0xea,0,9,8,0};
    static const uint8_t stop[] = {0x66,0xb8,0x10,0,0x8e,0xd8,0x8e,0xc0,0x8e,0xd0,0x0f,0x0b};
    if (!machine_facade_machine_begin(0x400000u, 0x400000u) ||
        !machine_facade_memory_write(0x800u, sizeof(gdt), gdt) ||
        !machine_facade_memory_write(0x700u, sizeof(transition), transition) ||
        !machine_facade_memory_write(0x900u, sizeof(stop), stop) ||
        !machine_facade_bind_opaque_callback(stop_on_ud, 0)) return 0;
    machine_facade_apply_real_mode_entry(0x70u, 0u);
    machine_facade_cpu_loop();
    return 1;
}

int main()
{
    session instance;
    machine_facade_protected_frame before, after;
    uint32_t pm_stack_identity = 0u;
    uint8_t interrupt_frame[10] = {0};
    uint8_t fault_frame[18] = {0};
    int result = 1;
    if (!enter_protected_stop()) goto done;
    session_initialize(&instance, 289u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, 0, machine_read, machine_write) ||
        !mvdm_monitor_bind_current_thread()) { result = 2; goto done; }
    put32(interrupt_frame, 0u, 0x11223344u); put16(interrupt_frame, 4u, 8u);
    put16(interrupt_frame, 6u, 0x21u); put16(interrupt_frame, 8u, VDM_INT_32);
    if (!machine_facade_memory_write(0x2100u, sizeof(interrupt_frame), interrupt_frame) ||
        !machine_facade_set_sp16(0x2100u) || !machine_facade_set_ax16(0xaaaau)) { result = 3; goto done_session; }
    DpmiSetProtectedmodeInterrupt();
    if (getAX() != 0u || VdmTib.VdmInterruptHandlers[0x21].CsSelector != 8u ||
        VdmTib.VdmInterruptHandlers[0x21].Flags != VDM_INT_32 ||
        VdmTib.VdmInterruptHandlers[0x21].Eip != 0x11223344u) { result = 4; goto done_session; }
    put32(fault_frame, 0u, 0x55667788u); put16(fault_frame, 4u, 0x10u);
    put32(fault_frame, 6u, 0x99aabbccu); put16(fault_frame, 10u, 8u);
    put16(fault_frame, 12u, 13u); put32(fault_frame, 14u, 0x102u);
    if (!machine_facade_memory_write(0x2200u, sizeof(fault_frame), fault_frame) ||
        !machine_facade_set_sp16(0x2200u)) { result = 5; goto done_session; }
    DpmiSetFaultHandler();
    if (getAX() != 0u || VdmTib.VdmFaultHandlers[13].SsSelector != 0x10u ||
        VdmTib.VdmFaultHandlers[13].CsSelector != 8u ||
        VdmTib.VdmFaultHandlers[13].Esp != 0x55667788u ||
        VdmTib.VdmFaultHandlers[13].Eip != 0x99aabbccu ||
        VdmTib.VdmFaultHandlers[13].Flags != 0x102u) { result = 6; goto done_session; }
    put16(interrupt_frame, 6u, 0x100u);
    if (!machine_facade_memory_write(0x2300u, sizeof(interrupt_frame), interrupt_frame) ||
        !machine_facade_set_sp16(0x2300u) || !machine_facade_set_ax16(0xbbbbu)) { result = 7; goto done_session; }
    DpmiSetProtectedmodeInterrupt();
    if (getAX() != 0xbbbbu) { result = 8; goto done_session; }
    if (machine_facade_copy_protected_frame(&before) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK) { result = 9; goto done_session; }
    DpmiPassPmStackInfo();
    if (!mvdm_host_identity_lookup((uintptr_t)&VdmTib.PmStackInfo,
        &pm_stack_identity) || getCX() != (uint16_t)(pm_stack_identity >> 16) ||
        getDX() != (uint16_t)pm_stack_identity) { result = 10; goto done_session; }
    BeginUseLockedPMStack();
    if (machine_facade_copy_protected_frame(&after) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || after.ss != before.es ||
        after.esp != 0x1000u || VdmTib.PmStackInfo.LockCount != 1u) {
        result = 11; goto done_session;
    }
    if (!EndUseLockedPMStack() ||
        machine_facade_copy_protected_frame(&after) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || after.ss != before.ss ||
        after.esp != before.esp || VdmTib.PmStackInfo.LockCount != 0u) {
        result = 12; goto done_session;
    }
    result = 0;
done_session:
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) result = result == 0 ? 13 : result;
done:
    machine_facade_unbind_opaque_callback();
    if (!machine_facade_machine_cleanup() && result == 0) result = 14;
    return result;
}
