/* Boundary unit evidence, not guest/CPU or descriptor-decoder acceptance.
 * Compile the actual binding; mock only descriptor/memory/register endpoints. */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_main.h>
#include <c_bsic.h>
#include <c_tlb.h>
extern void MOV_DR(ULONG, ULONG);
static BOOL fail_debug_allocation;
static LPVOID WINAPI test_debug_heap_alloc(HANDLE heap, DWORD flags, SIZE_T bytes);
#define HeapAlloc test_debug_heap_alloc
#include "../../src/compat/dbg_registers.c"
#undef HeapAlloc

static LPVOID WINAPI test_debug_heap_alloc(HANDLE heap, DWORD flags, SIZE_T bytes)
{
    if (fail_debug_allocation) return NULL;
    return HeapAlloc(heap, flags, bytes);
}

static CPU_DESCR supplied;
static int outside, copy_ok = 1, copies, moves;
static uint32_t copied_address, copied_size;
static ULONG indexes[6], values[6];
IU32 CCPU_DR[8];
BOOL debug_exception_pending;
void ccpu_apply_debug_context(const IU32 *registers)
{
    unsigned index;
    for (index = 0; index != 4; ++index) MOV_DR(index, registers[index]);
    MOV_DR(6, registers[4]);
    MOV_DR(7, registers[5]);
}
void ccpu_restore_debug_context(const IU32 *registers, BOOL pending)
{
    unsigned index;
    for (index = 0; index != 4; ++index) CCPU_DR[index] = registers[index];
    CCPU_DR[6] = registers[4];
    CCPU_DR[7] = registers[5];
    debug_exception_pending = pending;
}
void ccpu_capture_debug_context(ULONG *registers, BOOL *pending)
{
    unsigned i;
    for (i = 0; i != 4; ++i) registers[i] = CCPU_DR[i];
    registers[4] = CCPU_DR[6]; registers[5] = CCPU_DR[7];
    *pending = debug_exception_pending;
}
static IU32 table_address = 0x100, memory_size = 0x200000;
static int paging, mapping_ok = 1, decodes;
IBOOL c_getPG(void) { return paging; }
IBOOL c_getVM(void) { return 0; }
IUH c_getCPL(void) { return 0; }
IU32 c_sas_memory_size(void) { return memory_size; }
IBOOL xtrn2phy(LIN_ADDR address, IUM8 access, PHY_ADDR *physical)
{ (void)access; *physical = address; return mapping_ok; }

/* Unrelated dispatcher endpoints must never be called by this fixture. */
BOOL mvdm_debugger_is_debugged(void) { abort(); return FALSE; }
void mvdm_debugger_set_toolhelp(DWORD hook, BOOL mode)
{ (void)hook; (void)mode; abort(); }
int mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t count,
    uint32_t access, guest_memory_lease **lease, uint8_t **bytes)
{ (void)address; (void)count; (void)access; (void)lease; (void)bytes; abort(); return 0; }
int mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{ (void)lease; (void)commit; abort(); return 0; }
uint16_t c_getSS(void) { abort(); return 0; }
uint16_t c_getSP(void) { abort(); return 0; }
void c_setAX(uint16_t value) { (void)value; abort(); }
void mvdm_debugger_private_transport_unavailable(const char *origin)
{ (void)origin; abort(); }

BOOL selector_outside_GDT_LDT(IU16 selector, IU32 *address)
{
    (void)selector;
    *address = table_address;
    return outside;
}
VOID read_descriptor_linear(IU32 address, CPU_DESCR *descriptor)
{
    (void)address;
    ++decodes;
    *descriptor = supplied;
}
void ccpu_read_descriptor_fields(ULONG address, ULONG *base, ULONG *limit,
    USHORT *access)
{
    CPU_DESCR descriptor;
    read_descriptor_linear(address, &descriptor);
    *base = descriptor.base; *limit = descriptor.limit; *access = descriptor.AR;
}
int mvdm_softpc_guest_memory_copy_from(uint32_t address, uint8_t *bytes,
    uint32_t count)
{
    ++copies;
    copied_address = address;
    copied_size = count;
    if (!copy_ok) return 0;
    memset(bytes, 0x5a, count);
    return 1;
}
void MOV_DR(ULONG index, ULONG value)
{
    if (moves < 6) { indexes[moves] = index; values[moves] = value; }
    ++moves;
    CCPU_DR[index] = value;
}
/* Exercise the same installed boundary used by original DPMI callers. */
static int mode_phase, mode_calls;
void mvdm_softpc_wow_page_domain_leave_protected(void)
{
    mode_phase = 1;
    ++mode_calls;
}
int mvdm_softpc_wow_page_domain_reenter_protected(void)
{
    ++mode_calls;
    return mode_phase;
}
static USHORT supplied_msw;
static ULONG supplied_flags;
static int irq_sequence;
USHORT c_getMSW(void) { return supplied_msw; }
ULONG c_getEFLAGS(void) { return supplied_flags; }
BOOL host_DelayHwInterrupt(int irq, int count, ULONG delay)
{
    irq_sequence = (irq == 1 && count == 0 && delay == 0xffffffff &&
        irq_sequence == 0) ? 1 : -1;
    return TRUE;
}
void ica_hw_interrupt_cancel(ULONG adapter, ULONG line)
{
    irq_sequence = (adapter == 0 && line == 1 && irq_sequence == 1) ? 2 : -1;
}
#include "../../src/compat/mvdm_keyboard_reset.c"
#include "../../src/compat/mvdm_softpc_descriptor_fields.c"
#include "../../src/compat/mvdm_softpc_effective_address.c"
/* These lease operations are not exercised by the debug-only fake CPU. */
int mvdm_guest_location_acquire(const mvdm_guest_location *p, uint32_t n,
    uint32_t access, mvdm_guest_location_lease *lease)
{ (void)p; (void)n; (void)access; (void)lease; return 0; }
int mvdm_guest_location_release(mvdm_guest_location_lease *lease, int commit)
{ (void)lease; (void)commit; return 0; }
int mvdm_guest_location_copy_to_guest(const mvdm_guest_location *p,
    const uint8_t *bytes, uint32_t n)
{ (void)p; (void)bytes; (void)n; return 0; }
/* Page-view operations are outside this debug-only fake CPU. */
ULONG c_sas_dw_at(ULONG address)
{ (void)address; abort(); return 0; }
int c_sas_twenty_bit_wrapping_enabled(void)
{ abort(); return 0; }
void c_sas_fills(ULONG address, BYTE value, ULONG count)
{ (void)address; (void)value; (void)count; abort(); }
void c_sas_storedw(ULONG address, ULONG value)
{ (void)address; (void)value; abort(); }
void c_sas_storew(ULONG address, USHORT value)
{ (void)address; (void)value; abort(); }
void c_sas_loads(ULONG address, BYTE *bytes, ULONG count)
{ (void)address; (void)bytes; (void)count; abort(); }
void c_sas_stores(ULONG address, BYTE *bytes, ULONG count)
{ (void)address; (void)bytes; (void)count; abort(); }
BYTE * c_GetLinAdd(ULONG address)
{ (void)address; abort(); return 0; }
BYTE * c_GetPhyAdd(ULONG address)
{ (void)address; abort(); return 0; }
ULONG c_getCR0(void)
{ abort(); return 0; }
ULONG c_getCR3(void)
{ abort(); return 0; }
void c_setCR0(ULONG value)
{ (void)value; abort(); }
void c_setCR3(ULONG value)
{ (void)value; abort(); }
ULONG c_getGDT_BASE(void)
{ abort(); return 0; }
USHORT c_getGDT_LIMIT(void)
{ abort(); return 0; }
void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit)
{ (void)base; (void)limit; abort(); }
LONG VdmAllocateVirtualMemory(ULONG *address, ULONG bytes, BOOL commit)
{ (void)address; (void)bytes; (void)commit; abort(); return 0; }
LONG VdmFreeVirtualMemory(ULONG address)
{ (void)address; abort(); return 0; }
LONG VdmAddVirtualMemory(ULONG host_address, ULONG bytes, ULONG *intel_address)
{ (void)host_address; (void)bytes; (void)intel_address; abort(); return 0; }
LONG VdmRemoveVirtualMemory(ULONG address)
{ (void)address; abort(); return 0; }
/* BOP register operations are outside this debug-only fake CPU. */
ULONG c_getEAX(void)
{ abort(); return 0; }
ULONG c_getECX(void)
{ abort(); return 0; }
ULONG c_getEDX(void)
{ abort(); return 0; }
ULONG c_getESI(void)
{ abort(); return 0; }
ULONG c_getEBX(void)
{ abort(); return 0; }
int c_getPE(void)
{ abort(); return 0; }
void c_setEAX(ULONG value)
{ (void)value; abort(); }
void c_setEBX(ULONG value)
{ (void)value; abort(); }
void c_setEDX(ULONG value)
{ (void)value; abort(); }
void c_setESI(ULONG value)
{ (void)value; abort(); }
void c_setCF(int value)
{ (void)value; abort(); }
void virtual_device_trap(void)
{ abort(); }
#include "../../src/ntvdm-exe/softpc/mvdm_dpmi_binding.c"
#include "compat/include/dpmi_host.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line=%d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    ULONG output[6], input[6] = {10,11,12,13,16,17};
    ULONG i;
    compat_dpmi_services other = {0};
    mvdm_cpu_access other_cpu = {0};
    CHECK(!mvdm_cpu_access_install(NULL) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!mvdm_cpu_access_install(&other_cpu) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!compat_dpmi_install(NULL) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!compat_dpmi_install(&other) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(ntvdm_dpmi_initialize());
    CHECK(ntvdm_dpmi_initialize());
    mvdm_keyboard_reset_pending_irq(0);
    CHECK(irq_sequence == 0);
    mvdm_keyboard_reset_pending_irq(1);
    CHECK(irq_sequence == 2);
    {
        ULONG base, limit, access, address;
        BYTE narrow_access;
        USHORT narrow_limit;
        supplied.base = 0x12345000; supplied.limit = 0x23456; supplied.AR = 0x1f2;
        retrieve_descr_fields(&narrow_access, &base, &narrow_limit, 0x80);
        CHECK(base == supplied.base && narrow_access == 0xf2 && narrow_limit == 0x3456);
        CHECK(mvdm_softpc_protected_segment(8, &base, &limit, &access));
        CHECK(base == supplied.base && limit == supplied.limit && access == supplied.AR);
        CHECK(!mvdm_softpc_protected_segment(8, NULL, &limit, &access));
        supplied_msw = 0; supplied_flags = 0;
        CHECK(mvdm_softpc_effective_address(0x1234, 0x20, &address) && address == 0x12360);
        supplied_msw = 1; supplied_flags = 0x20000;
        CHECK(mvdm_softpc_effective_address(0x1234, 0x20, &address) && address == 0x12360);
        supplied_flags = 0;
        CHECK(mvdm_softpc_effective_address(8, 0x20, &address) && address == supplied.base + 0x20);
        outside = 1;
        CHECK(!mvdm_softpc_effective_address(8, 0x20, &address));
        outside = 0;
        supplied_msw = 0;
        copies = decodes = 0;
    }
    other_cpu = *mvdm_cpu_access_current();
    CHECK(!mvdm_cpu_access_install(&other_cpu) && GetLastError() == ERROR_ALREADY_INITIALIZED);
    other = *compat_dpmi_current_services();
    CHECK(!compat_dpmi_install(&other) && GetLastError() == ERROR_ALREADY_INITIALIZED);
    CHECK(!mvdm_softpc_wow_page_domain_reenter_protected());
    mvdm_softpc_wow_page_domain_leave_protected();
    CHECK(mvdm_softpc_wow_page_domain_reenter_protected() == 1 && mode_calls == 3);
    supplied.base = 0x1000; supplied.limit = 0xffff; supplied.AR = 0x92;
    CHECK(mvdm_debugger_read_debug_registers(8, 0x20, output));
    CHECK(copies == 1 && copied_address == 0x1020 && copied_size == 24);
    CHECK(output[0] == 0x5a5a5a5a && output[5] == 0x5a5a5a5a);
    copies = 0;
    CHECK(!mvdm_debugger_read_debug_registers(0, 0, output));
    CHECK(!mvdm_debugger_read_debug_registers(3, 0, output));
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, NULL));
    outside = 1;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    outside = 0;
    supplied.AR = 0x12; /* not present */
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.AR = 0x82; /* system descriptor */
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.AR = 0x98; /* execute-only code */
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.AR = 0x92; supplied.limit = 22;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    CHECK(copies == 0);
    supplied.limit = 23;
    CHECK(mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.AR = 0x96; supplied.limit = 0x100; /* expand down */
    CHECK(!mvdm_debugger_read_debug_registers(8, 0x100, output));
    CHECK(mvdm_debugger_read_debug_registers(8, 0x101, output));
    CHECK(!mvdm_debugger_read_debug_registers(8, 0xfff0, output));
    supplied.AR = 0x4096; /* 32-bit expand-down upper bound */
    CHECK(mvdm_debugger_read_debug_registers(8, 0xfff0, output));
    supplied.AR = 0x9a; supplied.limit = 0xffff; /* readable code */
    CHECK(mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.base = 0xfffffff0;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    supplied.base = 0x1000; copy_ok = 0;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    decodes = 0; copy_ok = 1;
    table_address = UINT32_MAX - 6;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    table_address = memory_size - 7;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    table_address = 0x100; paging = 1; mapping_ok = 0;
    CHECK(!mvdm_debugger_read_debug_registers(8, 0, output));
    CHECK(decodes == 0); /* No faulting decoder before failed preflight. */
    mapping_ok = 1;
    CHECK(mvdm_debugger_read_debug_registers(8, 0, output));
    CHECK(decodes == 1);
    CHECK(!ThreadSetDebugContext(NULL) && moves == 0);
    CHECK(ThreadSetDebugContext(input) && moves == 6);
    for (i = 0; i < 6; ++i) {
        CHECK(indexes[i] == (i < 4 ? i : i + 2));
        CHECK(values[i] == input[i]);
    }
    CHECK(!mvdm_debug_task_register(0));
    fail_debug_allocation = TRUE;
    CHECK(!mvdm_debug_task_register(100) && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!mvdm_debug_task_exchange(0, 100) && GetLastError() == ERROR_INVALID_STATE);
    fail_debug_allocation = FALSE;
    CCPU_DR[0] = 88;
    CHECK(mvdm_debug_task_register(100));
    mvdm_debug_task_retire(100, TRUE);
    CCPU_DR[0] = 10;
    CHECK(mvdm_debug_task_register(101));
    CHECK(!mvdm_debug_task_register(101) && GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(mvdm_debug_task_register(102));
    CHECK(mvdm_debug_task_exchange(0, 101));
    CHECK(CCPU_DR[0] == 10);
    CCPU_DR[0] = 31;
    debug_exception_pending = TRUE;
    CHECK(mvdm_debug_task_exchange(101, 102));
    CHECK(CCPU_DR[0] == 10 && !debug_exception_pending);
    CCPU_DR[0] = 32;
    CHECK(mvdm_debug_task_exchange(102, 101));
    CHECK(CCPU_DR[0] == 31 && debug_exception_pending);
    /* Re-selecting the running task must not roll the live CPU bank back to
     * its earlier suspended snapshot, nor clear its pending exception. */
    CCPU_DR[0] = 33;
    SetLastError(0x4321);
    CHECK(mvdm_debug_task_exchange(101, 101));
    CHECK(GetLastError() == 0x4321 && CCPU_DR[0] == 33 && debug_exception_pending);
    CHECK(debug_task_find(101)->registers[0] == 31);
    CHECK(!mvdm_debug_task_exchange(101, 103));
    CHECK(CCPU_DR[0] == 33 && debug_exception_pending);
    CHECK(debug_task_find(101)->registers[0] == 31);
    /* A later valid transfer must capture the current live state. */
    CCPU_DR[0] = 34;
    CHECK(mvdm_debug_task_exchange(101, 102));
    CHECK(mvdm_debug_task_exchange(102, 101));
    CHECK(CCPU_DR[0] == 34 && debug_exception_pending);
    input[0] = 42;
    CHECK(ThreadSetDebugContext(input));
    CHECK(mvdm_debug_task_exchange(101, 102));
    CHECK(CCPU_DR[0] == 42);
    CHECK(mvdm_debug_task_register(103));
    CHECK(mvdm_debug_task_exchange(102, 103));
    CHECK(CCPU_DR[0] == 42);
    mvdm_debug_task_retire(101, FALSE);
    mvdm_debug_task_retire(102, FALSE);
    mvdm_debug_task_retire(103, FALSE);
    CHECK(mvdm_debug_task_register(101));
    CHECK(mvdm_debug_task_exchange(0, 101));
    CHECK(CCPU_DR[0] == 42);
    mvdm_debug_task_retire(101, FALSE);
    input[0] = 99;
    input[5] = 0;
    CHECK(ThreadSetDebugContext(input));
    CHECK(mvdm_debug_task_register(104));
    CHECK(mvdm_debug_task_exchange(0, 104));
    CHECK(CCPU_DR[0] == 0);
    mvdm_debug_task_retire(104, FALSE);
    puts("WOW_DEBUG_CONTEXT_LIFECYCLE_OK alloc_failure=1 bootstrap_rollback=1 broadcast=1 inheritance=1 pending=1 reuse=1");
    puts("S38_DEBUG_BINDING_CHECKED_SPAN_REGISTER_ORDER_OK");
    return 0;
}
