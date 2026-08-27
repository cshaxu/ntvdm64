#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

#include <string.h>

extern "C" {
#include "mvdm_command_registers.h"

void DpmiDpmiInUse(void);
void DpmiDpmiNoLongerInUse(void);
void DpmiVcdPmSvcCall32(void);
}

/* The fixture reaches a stopped protected-mode frame through only the
 * selector-blind Bochs facade.  It does not decode or inject a BOP. */
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

static int enter_protected_stop(void)
{
    static const unsigned char gdt[] = {
        0x17, 0x00, 0x20, 0x08, 0x00, 0x00, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
    };
    static const unsigned char transition[] = {
        0x0f, 0x01, 0x16, 0x00, 0x08,
        0x66, 0xb8, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x22, 0xc0,
        0xea, 0x00, 0x09, 0x08, 0x00
    };
    static const unsigned char protected_stop[] = {
        0x66, 0xb8, 0x10, 0x00,
        0x8e, 0xd8, 0x8e, 0xc0, 0x8e, 0xd0,
        0x0f, 0x0b
    };

    return machine_facade_machine_begin(0x400000u, 0x400000u) &&
        machine_facade_memory_write(0x800u, sizeof(gdt), gdt) &&
        machine_facade_memory_write(0x700u, sizeof(transition), transition) &&
        machine_facade_memory_write(0x900u, sizeof(protected_stop),
            protected_stop) && machine_facade_bind_opaque_callback(stop_on_ud,
            0) && (machine_facade_apply_real_mode_entry(0x70u, 0u), 1) &&
        (machine_facade_cpu_loop(), 1);
}

int main(void)
{
    int result = 1;

    if (!enter_protected_stop()) goto done;

    /* Original vxd.c: VCD_PM_Get_Version. */
    if (!machine_facade_set_dx16(0u) || !machine_facade_set_ax16(0u)) {
        result = 2;
        goto done;
    }
    DpmiVcdPmSvcCall32();
    if (getAX() != 0x030au) {
        result = 3;
        goto done;
    }

    /* Original vxd.c: VCD_PM_Get_Port_Array.  The host registry may have no
     * serial ports; the documented result is therefore any nine-bit bitmap. */
    if (!machine_facade_set_dx16(1u) || !machine_facade_set_ax16(0xffffu)) {
        result = 4;
        goto done;
    }
    DpmiVcdPmSvcCall32();
    if ((getAX() & 0xfe00u) != 0u) {
        result = 5;
        goto done;
    }

    /* i386/dpmimisc.c specifies these lifecycle calls as deliberate no-ops. */
    if (!machine_facade_set_ax16(0xbeefu) ||
        !machine_facade_set_dx16(0xcafeu)) {
        result = 6;
        goto done;
    }
    DpmiDpmiInUse();
    DpmiDpmiNoLongerInUse();
    if (getAX() != 0xbeefu || getDX() != 0xcafeu) {
        result = 7;
        goto done;
    }
    result = 0;

done:
    machine_facade_unbind_opaque_callback();
    if (!machine_facade_machine_cleanup() && result == 0) result = 8;
    return result;
}
