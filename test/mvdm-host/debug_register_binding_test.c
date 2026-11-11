/* Boundary unit evidence, not guest/CPU or descriptor-decoder acceptance.
 * Compile the actual binding; mock only descriptor/memory/register endpoints. */
#include <stdio.h>
#include <windows.h>
static BOOL fail_debug_allocation;
static LPVOID WINAPI test_debug_heap_alloc(HANDLE heap, DWORD flags, SIZE_T bytes);
#define HeapAlloc test_debug_heap_alloc
#include "../../src/ntvdm-exe/debugger/source/dbg_registers.c"
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
int mvdm_softpc_effective_address(IU16 selector, IU32 offset, IU32 *result)
{ (void)selector; (void)offset; (void)result; abort(); return 0; }
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
#define CHECK(x) do { if (!(x)) { printf("FAIL line=%d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    ULONG output[6], input[6] = {10,11,12,13,16,17};
    ULONG i;
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
