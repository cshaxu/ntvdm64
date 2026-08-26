#ifndef ADAPTER_BOCHS_BOUNDED_RUN_H
#define ADAPTER_BOCHS_BOUNDED_RUN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADAPTER_BOCHS_BOUNDED_RUN_VERSION UINT32_C(1)
#define ADAPTER_BOCHS_BOUNDED_RUN_MAX_BYTES UINT32_C(65536)

enum adapter_bochs_bounded_run_status {
    ADAPTER_BOCHS_BOUNDED_RUN_BUDGET = 0,
    ADAPTER_BOCHS_BOUNDED_RUN_REJECTED_INPUT,
    ADAPTER_BOCHS_BOUNDED_RUN_MACHINE_FAILURE,
    ADAPTER_BOCHS_BOUNDED_RUN_UNEXPECTED_RETURN
};

/* Copied, machine-only request. `bytes` are opaque instruction storage; no
 * selector, service, guest pointer or Bochs object crosses this boundary. */
struct adapter_bochs_bounded_run_request {
    uint32_t version;
    uint32_t ips;
    uint64_t tick_budget;
    uint64_t physical_address;
    uint32_t byte_count;
    uint16_t cs;
    uint16_t reserved0;
    uint32_t eip;
    uint8_t bytes[ADAPTER_BOCHS_BOUNDED_RUN_MAX_BYTES];
};

enum adapter_bochs_bounded_run_status adapter_bochs_bounded_run(
    const struct adapter_bochs_bounded_run_request *request);

#ifdef __cplusplus
}
#endif

#endif
