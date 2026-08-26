#include "broker/broker.h"

#include <string.h>

static void initialize_message(broker_wire_message *message, uint32_t operation,
    uint8_t user_value)
{
    memset(message, 0, sizeof(*message));
    message->magic = BROKER_WIRE_MAGIC;
    message->version = BROKER_WIRE_VERSION;
    message->byte_count = (uint32_t)sizeof(*message);
    message->operation = operation;
    message->request_id = 7u;
    message->user_key[0] = user_value;
}

int main(void)
{
    broker_state state;
    broker_wire_message request, response;
    uint32_t broker_id;
    uint32_t wire_bytes = (uint32_t)sizeof(broker_wire_message);

    if (wire_bytes != 48u) return 10;
    broker_initialize(&state);
    initialize_message(&request, BROKER_WIRE_REGISTER, 1u);
    if (!broker_wire_message_valid(&request) ||
        broker_wire_message_has_native_value(&request) != 0 ||
        broker_dispatch(&state, &request, &response) != BROKER_WIRE_STATUS_OK ||
        (broker_id = response.broker_id) == 0u)
        return 1;
    initialize_message(&request, BROKER_WIRE_NOTIFY, 2u);
    request.broker_id = broker_id;
    if (broker_dispatch(&state, &request, &response) != BROKER_WIRE_STATUS_DENIED)
        return 2;
    initialize_message(&request, BROKER_WIRE_DISCONNECT, 1u);
    request.broker_id = broker_id;
    if (broker_dispatch(&state, &request, &response) != BROKER_WIRE_STATUS_OK ||
        broker_dispatch(&state, &request, &response) != BROKER_WIRE_STATUS_UNKNOWN)
        return 3;
    return 0;
}
