#ifndef BROKER_WIRE_H
#define BROKER_WIRE_H

#include <stdint.h>

#define BROKER_WIRE_MAGIC UINT32_C(0x42524b52)
#define BROKER_WIRE_VERSION UINT32_C(1)
#define BROKER_USER_KEY_BYTES 16u

enum broker_wire_operation {
    BROKER_WIRE_REGISTER = 1u,
    BROKER_WIRE_DISCONNECT = 2u,
    BROKER_WIRE_NOTIFY = 3u
};

enum broker_wire_status {
    BROKER_WIRE_STATUS_OK = 0u,
    BROKER_WIRE_STATUS_INVALID = 1u,
    BROKER_WIRE_STATUS_DENIED = 2u,
    BROKER_WIRE_STATUS_UNKNOWN = 3u,
    BROKER_WIRE_STATUS_CAPACITY = 4u
};

typedef struct broker_wire_message {
    uint32_t magic;
    uint32_t version;
    uint32_t byte_count;
    uint32_t operation;
    uint32_t broker_id;
    uint32_t request_id;
    uint32_t payload_kind;
    uint32_t payload_value;
    uint8_t user_key[BROKER_USER_KEY_BYTES];
} broker_wire_message;

int broker_wire_message_valid(const broker_wire_message *message);
int broker_wire_message_has_native_value(const broker_wire_message *message);

#endif
