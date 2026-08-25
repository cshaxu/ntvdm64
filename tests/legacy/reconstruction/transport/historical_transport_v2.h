#ifndef NTDOS64_HISTORICAL_TRANSPORT_V2_H
#define NTDOS64_HISTORICAL_TRANSPORT_V2_H

#include <stdint.h>

#define NTDOS64_HISTORICAL_TRANSPORT_V2_MAGIC 0x3254484eu
#define NTDOS64_HISTORICAL_TRANSPORT_V2_ABI 2u

typedef enum ntdos64_historical_transport_v2_disposition {
    NTDOS64_HISTORICAL_TRANSPORT_V2_COMPLETED = 0u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_INVALID_RECORD = 1u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_NOT_INITIALIZED = 2u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_UNSUPPORTED = 3u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_SEQUENCE_ERROR = 4u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_SESSION_ERROR = 5u,
    NTDOS64_HISTORICAL_TRANSPORT_V2_HISTORICAL_FAILURE = 6u
} ntdos64_historical_transport_v2_disposition;

typedef struct ntdos64_historical_transport_v2_state {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;
    uint16_t ip;
    uint16_t flags;
} ntdos64_historical_transport_v2_state;

typedef struct ntdos64_historical_transport_v2_request {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t session_token;
    uint32_t sequence;
    uint8_t selector;
    uint8_t service_byte;
    uint16_t reserved;
    ntdos64_historical_transport_v2_state state;
} ntdos64_historical_transport_v2_request;

typedef struct ntdos64_historical_transport_v2_response {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t session_token;
    uint32_t sequence;
    uint32_t disposition;
    uint32_t event_flags;
    ntdos64_historical_transport_v2_state state;
} ntdos64_historical_transport_v2_response;

_Static_assert(sizeof(ntdos64_historical_transport_v2_state) == 44u,
    "v2 state layout must remain cross-transport stable");
_Static_assert(sizeof(ntdos64_historical_transport_v2_request) == 68u,
    "v2 request layout must remain cross-transport stable");
_Static_assert(sizeof(ntdos64_historical_transport_v2_response) == 72u,
    "v2 response layout must remain cross-transport stable");

#endif
