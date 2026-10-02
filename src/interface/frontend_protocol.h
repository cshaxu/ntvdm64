#ifndef NTVDM_FRONTEND_PROTOCOL_H
#define NTVDM_FRONTEND_PROTOCOL_H
#include <stdint.h>
#include "version.h"

#define FRONTEND_BOOTSTRAP_VERSION 2u
typedef struct frontend_bootstrap_reply {
    uint32_t version,status;
    char application[APP_VERSION_BYTES];
} frontend_bootstrap_reply;

/* NTSRV owns the direct result; NTVWM reports its I/O release separately. */
#define NATIVE_REQUEST_VERSION 6u
/* Zero bytes requests an acknowledged native presentation resume, not a
 * launch. It returns no target/receipt handles and creates no target. */
typedef struct native_request_header { uint32_t version,bytes; } native_request_header;
typedef struct native_request_reply { uint32_t version,error,request,reserved;uint64_t target,receipt; } native_request_reply;
/* Resource state at the final I/O boundary, not task membership or an exit
 * instruction. Only NTSRV decides whether an owned Console can retire. */
#define NATIVE_COMPLETION_CONSOLE_EMPTY 1u
typedef struct native_request_completion { uint32_t version,error,flags; } native_request_completion;

/* Followed by application/command/directory/environment strings. Numeric
 * resource slots are never authority: the authenticated receiver materializes
 * and checks only the allowed resources of its pinned sender. */
typedef struct run16_native_launch_packet {
    uint32_t characters[4],console_mask;
    uint64_t standard[3],capabilities[2];
} run16_native_launch_packet;
#endif
