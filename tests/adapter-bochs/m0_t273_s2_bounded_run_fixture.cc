#include "adapter-bochs/bounded_run.h"

#include <string.h>

int main()
{
    adapter_bochs_bounded_run_request request;
    memset(&request, 0, sizeof(request));
    request.version = ADAPTER_BOCHS_BOUNDED_RUN_VERSION;
    request.ips = 1000000;
    request.tick_budget = 64;
    request.physical_address = 0;
    request.byte_count = 2;
    request.cs = 0;
    request.eip = 0;
    request.bytes[0] = 0xeb;
    request.bytes[1] = 0xfe;
    return adapter_bochs_bounded_run(&request) ==
        ADAPTER_BOCHS_BOUNDED_RUN_BUDGET ? 0 : 1;
}
