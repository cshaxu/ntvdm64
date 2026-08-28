#ifndef ADAPTER_BOCHS_MACHINE_LIFECYCLE_H
#define ADAPTER_BOCHS_MACHINE_LIFECYCLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADAPTER_BOCHS_MACHINE_LIFECYCLE_VERSION UINT32_C(1)
#define ADAPTER_BOCHS_MACHINE_LIFECYCLE_MAX_BYTES UINT32_C(65536)
#define ADAPTER_BOCHS_MACHINE_RESUME_VERSION UINT32_C(1)

enum adapter_bochs_machine_lifecycle_status {
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_OK = 0,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_BUDGET,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INPUT,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_INACTIVE,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_REJECTED_ACTIVE,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_MACHINE_FAILURE,
    ADAPTER_BOCHS_MACHINE_LIFECYCLE_UNEXPECTED_RETURN
};

/* A copied resume boundary for an already constructed machine.  It has no
 * source-service, selector or callback meaning: the caller supplies only a
 * finite timer budget and receives only a mechanical stop classification. */
enum adapter_bochs_machine_resume_status {
    ADAPTER_BOCHS_MACHINE_RESUME_BUDGET = 0,
    ADAPTER_BOCHS_MACHINE_RESUME_REJECTED_INPUT,
    ADAPTER_BOCHS_MACHINE_RESUME_REJECTED_INACTIVE,
    ADAPTER_BOCHS_MACHINE_RESUME_MACHINE_FAILURE,
    ADAPTER_BOCHS_MACHINE_RESUME_UNEXPECTED_RETURN
};

struct adapter_bochs_machine_resume_request {
    uint32_t version;
    uint32_t struct_bytes;
    uint64_t tick_budget;
};

struct adapter_bochs_machine_resume_outcome {
    uint32_t version;
    uint32_t struct_bytes;
    uint32_t status;
    uint32_t reserved0;
};

/* All input is copied before the CPU is entered.  This interface carries
 * machine bytes and real-mode state only; it has no guest service meaning. */
struct adapter_bochs_machine_lifecycle_configuration {
    uint32_t version;
    uint32_t ips;
    uint64_t guest_memory_bytes;
    uint64_t host_memory_bytes;
};

struct adapter_bochs_machine_lifecycle_load {
    uint32_t version;
    uint64_t physical_address;
    uint32_t byte_count;
    uint16_t cs;
    uint16_t reserved0;
    uint32_t eip;
    uint8_t bytes[ADAPTER_BOCHS_MACHINE_LIFECYCLE_MAX_BYTES];
};

enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_create(
    const struct adapter_bochs_machine_lifecycle_configuration *configuration);
enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_load_realmode(
    const struct adapter_bochs_machine_lifecycle_load *load);
enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_run_budget(uint64_t tick_budget);
int adapter_bochs_machine_lifecycle_resume(
    const struct adapter_bochs_machine_resume_request *request,
    struct adapter_bochs_machine_resume_outcome *outcome);
enum adapter_bochs_machine_lifecycle_status
adapter_bochs_machine_lifecycle_destroy(void);
int adapter_bochs_machine_lifecycle_active(void);

#ifdef __cplusplus
}
#endif

#endif
