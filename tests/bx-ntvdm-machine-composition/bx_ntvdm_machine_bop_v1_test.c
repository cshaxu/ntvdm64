#include <stdint.h>
#include <stdio.h>

#include "bx_ntvdm_machine_bop_v1.h"
#include "bx_ntvdm_machine_composition_seam.h"

typedef struct test_machine {
    uint8_t master_isr, master_imr, slave_isr, slave_imr, intr_flag;
    unsigned int reads, writes, stores;
    unsigned int reports;
    uint32_t last_error, last_options;
    uint8_t ram[0x30000];
} test_machine;

static int test_read8(void *opaque, uint16_t port, uint8_t *value)
{
    test_machine *machine = (test_machine *)opaque;
    ++machine->reads;
    if (port == 0x20u) *value = machine->master_isr;
    else if (port == 0x21u) *value = machine->master_imr;
    else if (port == 0xa0u) *value = machine->slave_isr;
    else if (port == 0xa1u) *value = machine->slave_imr;
    else return 0;
    return 1;
}

static int test_write8(void *opaque, uint16_t port, uint8_t value)
{
    test_machine *machine = (test_machine *)opaque;
    ++machine->writes;
    if (port == 0x21u) machine->master_imr = value;
    else if (port == 0xa1u) machine->slave_imr = value;
    else if ((port != 0x20u && port != 0xa0u) || value != 0x20u && value != 0x0bu)
        return 0;
    return 1;
}

static int test_store8(void *opaque, uint32_t address, uint8_t value)
{
    test_machine *machine = (test_machine *)opaque;
    ++machine->stores;
    if (address >= sizeof(machine->ram)) return 0;
    machine->ram[address] = value;
    if (address == 0x46bu) machine->intr_flag = value;
    return 1;
}

static int test_load8(void *opaque, uint32_t address, uint8_t *value)
{
    test_machine *machine = (test_machine *)opaque;
    if (value == 0 || address >= sizeof(machine->ram)) return 0;
    *value = machine->ram[address];
    return 1;
}

static void test_report(void *opaque, uint32_t error_number, uint32_t options,
    const char *message)
{
    test_machine *machine = (test_machine *)opaque;
    (void)message;
    ++machine->reports;
    machine->last_error = error_number;
    machine->last_options = options;
}

/* Source-shaped stand-in for the separately compiled original handler.  It
 * exercises precisely the three compatibility imports, not a copied BIOS
 * dispatcher or a device model. */
void unexpected_int(void)
{
    uint8_t m_isr, m_imr, s_isr, s_imr;
    outb(0x20u, 0x0bu);
    inb(0x20u, &m_isr);
    if (m_isr == 0u) m_isr = 0xffu;
    else {
        inb(0x21u, &m_imr);
        m_imr = (uint8_t)((m_imr | m_isr) & 0xfbu);
        outb(0x20u, 0x0bu);
        inb(0xa0u, &s_isr);
        if (s_isr != 0u) {
            inb(0xa1u, &s_imr);
            outb(0xa1u, (uint8_t)(s_imr | s_isr));
            outb(0xa0u, 0x20u);
        }
        outb(0x21u, m_imr);
        outb(0x20u, 0x20u);
    }
    c_sas_store(0x46bu, m_isr);
}

/* A source-shaped test double for the separately compiled illegalp.c object:
 * inspect the saved real-mode frame, report/continue, increment saved IP,
 * then enter the original unexpected-int path. */
void illegal_op_int(void)
{
    uint32_t stack = c_effective_addr(c_getSS(), c_getESP());
    uint16_t ip = (uint16_t)(c_sas_hw_at(stack) |
        ((uint16_t)c_sas_hw_at(stack + 1u) << 8));
    uint16_t cs = (uint16_t)(c_sas_hw_at(stack + 2u) |
        ((uint16_t)c_sas_hw_at(stack + 3u) << 8));
    uint32_t code = c_effective_addr(cs, ip);
    char message[] = "illegal opcode";
    (void)c_sas_hw_at(code);
    (void)c_sas_hw_at(code + 1u);
    (void)c_sas_hw_at(code + 2u);
    (void)c_sas_hw_at(code + 3u);
    (void)c_sas_hw_at(code + 4u);
    (void)host_error(9, 7, message);
    ++ip;
    c_sas_store(stack, (uint8_t)ip);
    c_sas_store(stack + 1u, (uint8_t)(ip >> 8));
    unexpected_int();
}

static void fail_handler(void)
{
    uint8_t ignored;
    inb(0x1234u, &ignored);
}

static int require(int condition, const char *message)
{
    if (condition) return 1;
    fprintf(stderr, "%s\n", message);
    return 0;
}

int main(void)
{
    bx_ntvdm_machine_bop_v1_request request;
    bx_ntvdm_machine_bop_v1_mechanics mechanics;
    test_machine machine = { 0 };
    int ok = 1;

    machine.master_isr = 0x04u;
    machine.master_imr = 0x10u;
    machine.slave_isr = 0x02u;
    machine.slave_imr = 0x40u;

    bx_ntvdm_machine_bop_v1_initialize_request(&request);
    bx_ntvdm_machine_bop_v1_initialize_mechanics(&mechanics);
    mechanics.opaque = &machine;
    mechanics.read8 = test_read8;
    mechanics.write8 = test_write8;
    mechanics.store8 = test_store8;
    request.classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE;
    request.selector = BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
        &mechanics, unexpected_int) ==
        BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME, "02h did not resume");
    ok &= require(machine.intr_flag == 0x04u && machine.stores == 1u,
        "02h did not record the master ISR at the BDA address");
    ok &= require(machine.master_imr == 0x10u && machine.slave_imr == 0x42u,
        "02h did not retain original PIC mask semantics");
    request.classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_HOST_SERVICE;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
        &mechanics, unexpected_int) == BX_NTVDM_MACHINE_BOP_V1_DECLINED,
        "host-service selector reached machine composition");
    request.classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_UNAVAILABLE;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
        &mechanics, unexpected_int) == BX_NTVDM_MACHINE_BOP_V1_DECLINED,
        "unavailable selector reached machine composition");
    request.classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE;
    request.selector = 0x03u;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
        &mechanics, unexpected_int) == BX_NTVDM_MACHINE_BOP_V1_DECLINED,
        "non-02 machine selector reached handler");
    request.selector = BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
        &mechanics, fail_handler) == BX_NTVDM_MACHINE_BOP_V1_MECHANICS_FAILURE,
        "mechanics failure did not remain explicit");
    mechanics.load8 = test_load8;
    mechanics.report = test_report;
    mechanics.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    mechanics.ss = 0x1000u;
    mechanics.esp = 0x20u;
    machine.ram[0x10020u] = 0x45u;
    machine.ram[0x10021u] = 0x23u;
    machine.ram[0x10022u] = 0x00u;
    machine.ram[0x10023u] = 0x20u;
    machine.ram[0x22345u] = 0x0fu;
    machine.ram[0x22346u] = 0x0bu;
    request.selector = BX_NTVDM_MACHINE_BOP_V1_SELECTOR_ILLEGAL_OP_INT;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_illegal_op_int(&request,
        &mechanics, illegal_op_int) == BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME,
        "06h did not resume through the bounded original-handler contract");
    ok &= require(machine.ram[0x10020u] == 0x46u &&
        machine.ram[0x10021u] == 0x23u, "06h did not advance the saved IP");
    ok &= require(machine.reports == 1u && machine.last_error == 9u &&
        machine.last_options == 7u, "06h did not preserve report/continue");
    mechanics.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    ok &= require(bx_ntvdm_machine_bop_v1_execute_illegal_op_int(&request,
        &mechanics, illegal_op_int) == BX_NTVDM_MACHINE_BOP_V1_DECLINED,
        "non-real-mode 06h reached the original handler");
    mechanics.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    {
        bx_ntvdm_exception_event_v1 event;
        bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_instruction_window_v1 window;
        uint8_t code[3] = { 0xc4u, 0xc4u,
            BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT };
        event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
        event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
        event.struct_bytes = sizeof(event);
        event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
        event.cpu_id = 0u;
        event.vector = 6u;
        event.error_code = 0u;
        event.reserved0 = 0u;
        event.fault_rip = 0u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        bx_ntvdm_instruction_window_v1_capture(&window, code, sizeof(code));
        ok &= require(bx_ntvdm_machine_composition_v1_probe(&event, &cpu,
            &window, &mechanics) == BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME,
            "02h machine seam did not handle");
        window.bytes[2] = BX_NTVDM_MACHINE_BOP_V1_SELECTOR_ILLEGAL_OP_INT;
        ok &= require(bx_ntvdm_machine_composition_v1_probe(&event, &cpu,
            &window, &mechanics) == BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME,
            "06h machine seam did not handle");
        window.bytes[2] = 0x50u;
        ok &= require(bx_ntvdm_machine_composition_v1_probe(&event, &cpu,
            &window, &mechanics) == BX_NTVDM_MACHINE_BOP_V1_DECLINED,
            "host selector did not decline");
    }
    return ok ? 0 : 1;
}
