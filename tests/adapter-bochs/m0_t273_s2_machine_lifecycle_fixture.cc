#include "adapter-bochs/machine_lifecycle.h"

#include <string.h>

int main()
{
    adapter_bochs_machine_lifecycle_configuration configuration;
    adapter_bochs_machine_lifecycle_load load;

    memset(&configuration, 0, sizeof(configuration));
    configuration.version = ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION;
    configuration.ips = 1000000;
    configuration.guest_memory_bytes = UINT64_C(0x100000);
    configuration.host_memory_bytes = UINT64_C(0x100000);
    if (adapter_bochs_machine_lifecycle_create(&configuration) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK)
        return 1;

    memset(&load, 0, sizeof(load));
    load.version = ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION;
    load.byte_count = 2;
    load.bytes[0] = 0xeb;
    load.bytes[1] = 0xfe;
    if (adapter_bochs_machine_lifecycle_load_realmode(&load) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK)
        return 2;
    if (adapter_bochs_machine_lifecycle_run_budget(64) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_BUDGET ||
        !adapter_bochs_machine_lifecycle_active())
        return 3;
    if (adapter_bochs_machine_lifecycle_run_budget(64) !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_BUDGET ||
        !adapter_bochs_machine_lifecycle_active())
        return 4;
    if (adapter_bochs_machine_lifecycle_destroy() !=
        ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK ||
        adapter_bochs_machine_lifecycle_active())
        return 5;
    return 0;
}
