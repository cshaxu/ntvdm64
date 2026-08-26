#ifndef BROKER_H
#define BROKER_H

#include "wire.h"

#define BROKER_MAXIMUM_CLIENTS 16u

typedef struct broker_client {
    uint32_t broker_id;
    uint32_t active;
    uint8_t user_key[BROKER_USER_KEY_BYTES];
} broker_client;

typedef struct broker_state {
    uint32_t next_broker_id;
    broker_client clients[BROKER_MAXIMUM_CLIENTS];
} broker_state;

void broker_initialize(broker_state *state);
uint32_t broker_dispatch(broker_state *state,
    const broker_wire_message *request, broker_wire_message *response);

#endif
