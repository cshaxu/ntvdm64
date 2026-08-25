/* Bounded CCPU monitor-ABI segment-to-linear-address fixture. */
#include <stdint.h>

extern uint32_t c_effective_addr(uint16_t segment, uint32_t offset);
extern uint32_t effective_addr(uint16_t segment, uint16_t offset);

typedef struct _RUNNER_CCPU_DESCRIPTOR {
    uint32_t base;
    uint32_t limit;
    uint16_t ar;
} RUNNER_CCPU_DESCRIPTOR;

static int protected_mode;
static int virtual_8086_mode;
static uint32_t gdt_base;
static uint32_t gdt_limit;
static uint32_t ldt_base;
static uint32_t ldt_limit;
static uint16_t ldt_selector;
static RUNNER_CCPU_DESCRIPTOR gdt_entry;
static RUNNER_CCPU_DESCRIPTOR ldt_entry;

int c_getPE(void) { return protected_mode; }
int c_getVM(void) { return virtual_8086_mode; }

int selector_outside_table(uint16_t selector, uint32_t *descriptor_address) {
    uint32_t offset = (uint32_t)(selector & 0xfff8);
    uint32_t base;
    uint32_t limit;

    if (selector & 0x0004) {
        if (ldt_selector == 0) return 1;
        base = ldt_base;
        limit = ldt_limit;
    } else {
        base = gdt_base;
        limit = gdt_limit;
    }

    if (offset == 0 || offset + 7 > limit) return 1;
    *descriptor_address = base + offset;
    return 0;
}

void read_descriptor_linear(uint32_t address,
                            RUNNER_CCPU_DESCRIPTOR *descriptor) {
    if (address == gdt_base + 8) {
        *descriptor = gdt_entry;
    } else if (address == ldt_base + 8) {
        *descriptor = ldt_entry;
    } else {
        descriptor->base = 0;
        descriptor->limit = 0;
        descriptor->ar = 0;
    }
}

int main(void) {
    protected_mode = 0;
    virtual_8086_mode = 0;
    if (c_effective_addr(0x1234, 0x5678) != 0x000179b8) return 1;
    if (effective_addr(0x1234, 0x5678) != 0x000179b8) return 7;

    protected_mode = 1;
    virtual_8086_mode = 1;
    if (c_effective_addr(0x4321, 0x00ff) != 0x0004330f) return 2;

    virtual_8086_mode = 0;
    gdt_base = 0x1000;
    gdt_limit = 0x000f;
    gdt_entry.base = 0x12340000;
    if (c_effective_addr(0x0008, 0x00002345) != 0x12342345) return 3;
    if (c_effective_addr(0x0010, 0) != UINT32_MAX) return 4;

    /* The vector forms a linear address only; access checks happen later. */
    gdt_entry.limit = 0;
    if (c_effective_addr(0x0008, 0x00002345) != 0x12342345) return 8;
    gdt_entry.base = 0xfffffff0;
    if (c_effective_addr(0x0008, 0x00000030) != 0x00000020) return 9;

    ldt_selector = 0x0028;
    ldt_base = 0x2000;
    ldt_limit = 0x000f;
    ldt_entry.base = 0x56780000;
    if (c_effective_addr(0x000c, 0x00000321) != 0x56780321) return 5;

    ldt_selector = 0;
    return c_effective_addr(0x000c, 0) == UINT32_MAX ? 0 : 6;
}
