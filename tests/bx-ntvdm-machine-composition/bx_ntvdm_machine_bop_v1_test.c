#include <stdint.h>
#include <stdio.h>

#include "bx_ntvdm_machine_composition_v2.h"

typedef struct test_machine {
    uint8_t master_isr, master_imr, slave_isr, slave_imr, intr_flag;
    uint32_t reports;
    uint8_t ram[0x30000];
} test_machine;

static int read8(void *opaque, uint16_t port, uint8_t *value)
{
    test_machine *m = (test_machine *)opaque;
    if (port == 0x20u) *value = m->master_isr;
    else if (port == 0x21u) *value = m->master_imr;
    else if (port == 0xa0u) *value = m->slave_isr;
    else if (port == 0xa1u) *value = m->slave_imr;
    else return 0;
    return 1;
}
static int write8(void *opaque, uint16_t port, uint8_t value)
{
    test_machine *m = (test_machine *)opaque;
    if (port == 0x21u) m->master_imr = value;
    else if (port == 0xa1u) m->slave_imr = value;
    else if ((port != 0x20u && port != 0xa0u) ||
             (value != 0x20u && value != 0x0bu)) return 0;
    return 1;
}
static int store8(void *opaque, uint32_t address, uint8_t value)
{
    test_machine *m = (test_machine *)opaque;
    if (address >= sizeof(m->ram)) return 0;
    m->ram[address] = value;
    if (address == 0x46bu) m->intr_flag = value;
    return 1;
}
static int load8(void *opaque, uint32_t address, uint8_t *value)
{
    test_machine *m = (test_machine *)opaque;
    if (value == 0 || address >= sizeof(m->ram)) return 0;
    *value = m->ram[address];
    return 1;
}
static void report(void *opaque, uint32_t error, uint32_t options,
    const char *message)
{
    test_machine *m = (test_machine *)opaque;
    (void)error; (void)options; (void)message;
    ++m->reports;
}

/* Source-shaped stand-ins for the separately linked original islands. */
void unexpected_int(void)
{
    uint8_t isr, imr;
    outb(0x20u, 0x0bu);
    inb(0x20u, &isr);
    inb(0x21u, &imr);
    outb(0x21u, (uint8_t)(imr | isr));
    outb(0x20u, 0x20u);
    c_sas_store(0x46bu, isr);
}
void illegal_op_int(void)
{
    uint32_t stack = c_effective_addr(c_getSS(), c_getESP());
    (void)c_sas_hw_at(stack);
    (void)host_error(9, 7, "illegal opcode");
    unexpected_int();
}

static int require(int condition, const char *message)
{
    if (condition) return 1;
    fprintf(stderr, "%s\n", message);
    return 0;
}

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x100u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_machine_mechanics_v1 mechanics;
    bx_ntvdm_cpu_result_v2 result;
    test_machine machine = { 0x04u, 0x10u, 0u, 0u, 0u, 0u, { 0 } };
    uint8_t bytes[3] = { 0xc4u, 0xc4u, 0x02u };
    int ok = 1;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    bx_ntvdm_machine_mechanics_v1_initialize(&mechanics);
    mechanics.opaque = &machine;
    mechanics.read8 = read8; mechanics.write8 = write8;
    mechanics.store8 = store8; mechanics.load8 = load8; mechanics.report = report;
    mechanics.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    mechanics.ss = 0x1000u; mechanics.esp = 0x20u;

    ok &= require(bx_ntvdm_machine_composition_v2_dispatch(&event, &cpu,
        &window, &mechanics, &result) && result.disposition ==
        BX_NTVDM_CPU_RESULT_V2_RESUME && result.resume_rip == 0x103u,
        "adapter machine selector did not produce generic 02h resume");
    ok &= require(machine.intr_flag == 0x04u, "original 02h handler lost mechanics");
    bytes[2] = 0x06u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    ok &= require(bx_ntvdm_machine_composition_v2_dispatch(&event, &cpu,
        &window, &mechanics, &result) && result.disposition ==
        BX_NTVDM_CPU_RESULT_V2_RESUME && machine.reports == 1u,
        "adapter machine selector did not preserve 06h island");
    bytes[2] = 0x50u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    ok &= require(bx_ntvdm_machine_composition_v2_dispatch(&event, &cpu,
        &window, &mechanics, &result) && result.disposition ==
        BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH,
        "non-machine BOP did not pass through adapter machine plane");
    return ok ? 0 : 1;
}
