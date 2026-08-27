#include "adapter-bochs/machine_facade.h"

extern "C" {
#include "session/session.h"
void xmsA20(void);
void xmsInitUMB(void);
int ReserveUMB(unsigned short owner, void **address, unsigned long *size);
}

#include <string.h>

static int machine_read(void *, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_read(address, byte_count, bytes);
}

static int machine_write(void *, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_write(address, byte_count, bytes);
}

/* S3 has no physical UMB policy.  The original xmsInitUMB body must retain
 * its empty-list branch while still binding AX:BX and applying A20 order. */
extern "C" int ReserveUMB(unsigned short, void **address, unsigned long *size)
{
    if (address != 0) *address = 0;
    if (size != 0) *size = 0;
    return 0;
}

static int state_byte(uint8_t expected)
{
    uint8_t value = 0xffu;
    return machine_facade_memory_read(0x2010u, 1u, &value) && value == expected;
}

static int a20(uint32_t expected)
{
    uint32_t enabled = 2u;
    return machine_facade_get_a20(&enabled) && enabled == expected;
}

int main()
{
    session instance;
    uint16_t ax = 0u, bx = 0u;

    if (!machine_facade_machine_begin(0x200000u, 0x200000u)) return 1;
    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, 0, machine_read, machine_write))
        return 2;

    /* Original xmsInitUMB records AX:BX and calls xmsEnableA20Wrapping. */
    if (!machine_facade_set_ax16(0x0200u) || !machine_facade_set_bx16(0x0010u))
        return 3;
    xmsInitUMB();
    if (!a20(0u) || !state_byte(0u)) return 4;

    /* Original xmsA20: AX=1 enables A20 and writes state 1. */
    if (!machine_facade_set_ax16(1u)) return 5;
    xmsA20();
    if (!a20(1u) || !state_byte(1u) ||
        !machine_facade_copy_ax16(&ax) || ax != 1u) return 6;

    /* AX=0 disables A20; AX=2 reports the source wrapping state in AX and
     * clears BL through the same-shaped SoftPC register facade. */
    if (!machine_facade_set_ax16(0u)) return 7;
    xmsA20();
    if (!a20(0u) || !state_byte(0u)) return 8;
    if (!machine_facade_set_ax16(2u) || !machine_facade_set_bx16(0xff00u))
        return 9;
    xmsA20();
    if (!machine_facade_copy_ax16(&ax) || !machine_facade_copy_bx16(&bx) ||
        ax != 0u || bx != 0u) return 10;

    /* An invalid AX:BX location cannot yield a durable pointer.  Its next
     * source A20 request still toggles the mechanical line, but does not
     * modify the earlier valid state byte. */
    if (!machine_facade_set_ax16(0xffffu) || !machine_facade_set_bx16(0xffffu))
        return 11;
    xmsInitUMB();
    if (!a20(0u) || !state_byte(0u) || !machine_facade_set_ax16(1u)) return 12;
    xmsA20();
    if (!a20(1u) || !state_byte(0u)) return 13;

    /* Expiring the lease also prevents a later source write from retaining
     * a host alias; re-entering with a fresh lease is a new lifetime only. */
    session_guest_memory_end(&instance);
    if (!machine_facade_set_ax16(0u)) return 14;
    xmsA20();
    if (!a20(0u) || !state_byte(0u)) return 15;
    if (!session_guest_memory_begin(&instance, 0, machine_read, machine_write) ||
        !session_thread_unbind(&instance) || !session_dispose(&instance) ||
        !machine_facade_machine_cleanup()) return 16;
    return 0;
}
