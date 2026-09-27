#ifndef BROKER_CONSOLE_PROBE_H
#define BROKER_CONSOLE_PROBE_H
#include <stdint.h>
/* Private inherited-pipe protocol, not a public broker request or identity.
 * The spawning owner authenticates/pins candidates and discards late results. */
#define BROKER_CONSOLE_PROBE_VERSION 1u
#define BROKER_CONSOLE_PROBE_MAX_CANDIDATES 4096u
typedef struct {
    uint32_t version, caller, count, reserved;
} BROKER_CONSOLE_PROBE_REQUEST;
typedef struct {
    uint32_t version, status, count, reserved;
} BROKER_CONSOLE_PROBE_REPLY;
#endif
