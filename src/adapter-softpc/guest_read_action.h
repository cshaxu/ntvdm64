#ifndef RUNTIME_GUEST_READ_ACTION_V1_H
#define RUNTIME_GUEST_READ_ACTION_V1_H

#include <stdint.h>

#include "cpu_result.h"
#include "guest_range.h"

#define RUNTIME_GUEST_READ_ACTION_V1_MAGIC 0x42584752u
#define RUNTIME_GUEST_READ_ACTION_V1_VERSION 1u
#define RUNTIME_GUEST_READ_ACTION_V1_MAX_BYTES 128u

enum runtime_guest_read_action_v1_disposition {
    RUNTIME_GUEST_READ_ACTION_V1_PASS_THROUGH = 0u,
    RUNTIME_GUEST_READ_ACTION_V1_RESUME = 1u,
    RUNTIME_GUEST_READ_ACTION_V1_NEED_READ = 2u,
    RUNTIME_GUEST_READ_ACTION_V1_STOP = 3u
};

/* One generic adapter outcome. NEED_READ contains an opaque physical range;
 * neither this record nor its Bochs consumer carries a guest or host pointer. */
typedef struct runtime_guest_read_action_v1 {
    uint32_t magic, abi_version, struct_bytes, disposition;
    runtime_cpu_result_v2 cpu_result;
    runtime_guest_range guest_read;
} runtime_guest_read_action_v1;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_guest_read_action_v1_pass_through(
    runtime_guest_read_action_v1 *action);
int runtime_guest_read_action_v1_resume(
    runtime_guest_read_action_v1 *action, uint64_t resume_rip);
int runtime_guest_read_action_v1_stop(runtime_guest_read_action_v1 *action);
int runtime_guest_read_action_v1_need_read(
    runtime_guest_read_action_v1 *action, uint64_t address, uint64_t length);
int runtime_guest_read_action_v1_valid(
    const runtime_guest_read_action_v1 *action);
#ifdef __cplusplus
}
#endif

#endif
