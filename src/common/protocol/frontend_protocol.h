#ifndef NTVDM_FRONTEND_PROTOCOL_H
#define NTVDM_FRONTEND_PROTOCOL_H
#include <stdint.h>
#include "version.h"

/* NTSRV owns the direct result and final I/O status in the same record. */
/* Resource state at the final I/O boundary, not task membership or an exit
 * instruction. Only NTSRV decides whether an owned Console can retire. */
#define NATIVE_COMPLETION_CONSOLE_EMPTY 1u

/* Authenticated NTSRV control, never worker/frontend pipe operations.
 * Existing RPC connections identify the endpoints; no second lease token. */
#define WORKER_IO_ACQUIRE 1u
#define WORKER_IO_RELEASE_BEGIN 2u
#define WORKER_IO_RELEASED 3u
/* Execution checkpoints are facts, not permission to close an endpoint. */
#define WORKER_IO_CHECKPOINT_PAUSE 1u
#define WORKER_IO_CHECKPOINT_COMPLETE 2u
#define WORKER_IO_KEEP 0u
#define WORKER_IO_RELEASE 1u

/* Followed by application/command/directory/environment strings. Numeric
 * resource slots are never authority: the authenticated receiver materializes
 * and checks only the allowed resources of its pinned sender. */
typedef struct run16_native_launch_packet {
    uint32_t characters[4],console_mask;
    uint64_t standard[3],capabilities[2];
} run16_native_launch_packet;
#endif
