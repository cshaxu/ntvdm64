#include "broker.h"

#include <string.h>

static broker_client *find_client(broker_state *state, uint32_t broker_id)
{
    uint32_t index;
    for (index = 0u; index < BROKER_MAXIMUM_CLIENTS; ++index) {
        if (state->clients[index].active != 0u &&
            state->clients[index].broker_id == broker_id) return &state->clients[index];
    }
    return NULL;
}

static broker_client *available_client(broker_state *state)
{
    uint32_t index;
    for (index = 0u; index < BROKER_MAXIMUM_CLIENTS; ++index) {
        if (state->clients[index].active == 0u) return &state->clients[index];
    }
    return NULL;
}

static int caller_owns(const broker_client *client,
    const broker_wire_message *request)
{
    return client != NULL && memcmp(client->user_key, request->user_key,
        BROKER_USER_KEY_BYTES) == 0;
}

void broker_initialize(broker_state *state)
{
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->next_broker_id = 1u;
}

uint32_t broker_dispatch(broker_state *state,
    const broker_wire_message *request, broker_wire_message *response)
{
    broker_client *client;
    uint32_t status = BROKER_WIRE_STATUS_INVALID;
    if (response != NULL) memset(response, 0, sizeof(*response));
    if (state == NULL || !broker_wire_message_valid(request) || response == NULL)
        return status;
    response->magic = BROKER_WIRE_MAGIC;
    response->version = BROKER_WIRE_VERSION;
    response->byte_count = (uint32_t)sizeof(*response);
    response->operation = request->operation;
    response->request_id = request->request_id;
    memcpy(response->user_key, request->user_key, BROKER_USER_KEY_BYTES);
    if (request->operation == BROKER_WIRE_REGISTER) {
        client = available_client(state);
        if (client == NULL || state->next_broker_id == 0u) status = BROKER_WIRE_STATUS_CAPACITY;
        else {
            client->broker_id = state->next_broker_id++;
            client->active = 1u;
            memcpy(client->user_key, request->user_key, BROKER_USER_KEY_BYTES);
            response->broker_id = client->broker_id;
            status = BROKER_WIRE_STATUS_OK;
        }
    } else {
        client = find_client(state, request->broker_id);
        response->broker_id = request->broker_id;
        if (client == NULL) status = BROKER_WIRE_STATUS_UNKNOWN;
        else if (!caller_owns(client, request)) status = BROKER_WIRE_STATUS_DENIED;
        else if (request->operation == BROKER_WIRE_DISCONNECT) {
            memset(client, 0, sizeof(*client));
            status = BROKER_WIRE_STATUS_OK;
        } else status = BROKER_WIRE_STATUS_OK;
    }
    response->payload_value = status;
    return status;
}
