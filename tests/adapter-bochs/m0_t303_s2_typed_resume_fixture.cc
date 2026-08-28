#include "adapter-bochs/machine_lifecycle.h"
#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_registers.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_guest_location.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_vdm_stack.h"
#include "session/session.h"

#include <string.h>

static int read_machine(void *, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_read(address, byte_count, bytes);
}

static int write_machine(void *, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    return machine_facade_memory_write(address, byte_count, bytes);
}

int main()
{
    adapter_bochs_machine_lifecycle_configuration configuration;
    adapter_bochs_machine_lifecycle_load load;
    adapter_bochs_machine_resume_request invalid_request;
    adapter_bochs_machine_resume_outcome outcome;
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    session instance;
    uint8_t bytes[2] = { 0x5au, 0xa5u };

    memset(&invalid_request, 0, sizeof(invalid_request));
    memset(&outcome, 0, sizeof(outcome));
    invalid_request.version = ADAPTER_BOCHS_MACHINE_RESUME_VERSION;
    invalid_request.struct_bytes = sizeof(invalid_request);
    if (!adapter_bochs_machine_lifecycle_resume(&invalid_request, &outcome) ||
        outcome.status != ADAPTER_BOCHS_MACHINE_RESUME_REJECTED_INPUT)
        return 1;

    memset(&configuration, 0, sizeof(configuration));
    configuration.version = ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION;
    configuration.ips = 1000000u;
    configuration.guest_memory_bytes = UINT64_C(0x100000);
    configuration.host_memory_bytes = UINT64_C(0x100000);
    if (adapter_bochs_machine_lifecycle_create(&configuration) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK) return 2;

    memset(&load, 0, sizeof(load));
    load.version = ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION;
    load.byte_count = 2u;
    load.bytes[0] = 0xebu; /* jmp $ */
    load.bytes[1] = 0xfeu;
    if (adapter_bochs_machine_lifecycle_load_realmode(&load) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK) return 3;

    session_initialize(&instance, 1u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, 0, read_machine, write_machine) ||
        !session_thread_bind(&instance) ||
        !session_set_mechanical_resume_budget(&instance, 64u)) return 4;

    setAX(0x5a5au);
    if (getAX() != 0x5a5au ||
        !mvdm_guest_location_set_real_mode(&location, 0u, 0x200u) ||
        !mvdm_guest_location_copy_to_guest(&location, bytes, sizeof(bytes)) ||
        !mvdm_guest_location_acquire(&location, sizeof(bytes),
            GUEST_MEMORY_ACCESS_READ, &lease) ||
        lease.bytes[0] != bytes[0] || lease.bytes[1] != bytes[1] ||
        !mvdm_guest_location_release(&lease, 0)) return 5;

    host_simulate();
    if (session_mechanical_resume_status(&instance) !=
        ADAPTER_BOCHS_MACHINE_RESUME_BUDGET) return 6;
    if (!session_thread_unbind(&instance)) return 7;
    session_guest_memory_end(&instance);
    if (!session_dispose(&instance)) return 8;
    if (adapter_bochs_machine_lifecycle_destroy() !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK) return 9;
    return 0;
}
