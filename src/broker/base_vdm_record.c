#include "base_vdm_record.h"

#include <string.h>

static int state_valid(const broker_base_vdm_state *state)
{
    return state != NULL && state->version == BROKER_BASE_VDM_RECORD_VERSION &&
        state->struct_bytes == sizeof(*state);
}

static int record_valid(const broker_base_vdm_record *record)
{
    return record != NULL && record->version == BROKER_BASE_VDM_RECORD_VERSION &&
        record->struct_bytes == sizeof(*record) && record->broker_id != 0u &&
        record->session_id != 0u && record->request_id != 0u &&
        record->reserved0 == 0u &&
        (record->vdm_state & ASKING_FOR_DOS_BINARY) != 0u &&
        (record->vdm_state & (ASKING_FOR_WOW_BINARY |
            ASKING_FOR_PIF | ASKING_FOR_SEPWOW_BINARY)) == 0u &&
        record->command_bytes != 0u &&
        record->command_bytes <= MAXIMUM_VDM_COMMAND_LENGTH &&
        record->application_bytes <= sizeof(record->application) &&
        record->environment_bytes <= MAXIMUM_VDM_ENVIORNMENT &&
        record->current_directory_bytes <= sizeof(record->current_directory);
}

static broker_base_vdm_slot *find_slot(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id)
{
    uint32_t index;
    for (index = 0u; index != BROKER_BASE_VDM_MAXIMUM_RECORDS; ++index) {
        broker_base_vdm_slot *slot = &state->slots[index];
        if (slot->active != 0u && slot->record.broker_id == broker_id &&
            slot->record.session_id == session_id) return slot;
    }
    return NULL;
}

static broker_base_vdm_slot *find_empty_slot(broker_base_vdm_state *state)
{
    uint32_t index;
    for (index = 0u; index != BROKER_BASE_VDM_MAXIMUM_RECORDS; ++index) {
        if (state->slots[index].active == 0u) return &state->slots[index];
    }
    return NULL;
}

void broker_base_vdm_initialize(broker_base_vdm_state *state)
{
    if (state == NULL) return;
    memset(state, 0, sizeof(*state));
    state->version = BROKER_BASE_VDM_RECORD_VERSION;
    state->struct_bytes = (uint32_t)sizeof(*state);
}

uint32_t broker_base_vdm_register(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id)
{
    broker_base_vdm_slot *slot;
    if (!state_valid(state) || broker_id == 0u || session_id == 0u)
        return BROKER_BASE_VDM_STATUS_INVALID;
    if (find_slot(state, broker_id, session_id) != NULL)
        return BROKER_BASE_VDM_STATUS_BUSY;
    slot = find_empty_slot(state);
    if (slot == NULL) return BROKER_BASE_VDM_STATUS_CAPACITY;
    memset(slot, 0, sizeof(*slot));
    slot->active = 1u;
    slot->record.version = BROKER_BASE_VDM_RECORD_VERSION;
    slot->record.struct_bytes = (uint32_t)sizeof(slot->record);
    slot->record.broker_id = broker_id;
    slot->record.session_id = session_id;
    return BROKER_BASE_VDM_STATUS_OK;
}

uint32_t broker_base_vdm_publish(broker_base_vdm_state *state,
    const broker_base_vdm_record *record)
{
    broker_base_vdm_slot *slot;
    if (!state_valid(state) || !record_valid(record))
        return BROKER_BASE_VDM_STATUS_INVALID;
    slot = find_slot(state, record->broker_id, record->session_id);
    if (slot == NULL) return BROKER_BASE_VDM_STATUS_UNKNOWN;
    if (slot->available != 0u) return BROKER_BASE_VDM_STATUS_BUSY;
    slot->record = *record;
    slot->available = 1u;
    return BROKER_BASE_VDM_STATUS_OK;
}

static uint32_t broker_base_vdm_select_next(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id, uint16_t request_vdm_state,
    broker_base_vdm_record *result, int consume)
{
    broker_base_vdm_slot *slot;
    if (!state_valid(state) || broker_id == 0u || session_id == 0u ||
        result == NULL || (request_vdm_state & ASKING_FOR_DOS_BINARY) == 0u ||
        (request_vdm_state & (ASKING_FOR_WOW_BINARY | ASKING_FOR_PIF |
            ASKING_FOR_SEPWOW_BINARY)) != 0u)
        return BROKER_BASE_VDM_STATUS_INVALID;
    slot = find_slot(state, broker_id, session_id);
    if (slot == NULL) return BROKER_BASE_VDM_STATUS_UNKNOWN;
    if (slot->available == 0u) {
        slot->pending = 1u;
        if ((request_vdm_state & RETURN_ON_NO_COMMAND) != 0u &&
            (request_vdm_state & ASKING_FOR_SECOND_TIME) != 0u)
            return BROKER_BASE_VDM_STATUS_NO_COMMAND;
        return BROKER_BASE_VDM_STATUS_PENDING;
    }
    *result = slot->record;
    if (consume != 0) {
        slot->available = 0u;
        slot->pending = 0u;
    }
    return BROKER_BASE_VDM_STATUS_OK;
}

uint32_t broker_base_vdm_get_next(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id, uint16_t request_vdm_state,
    broker_base_vdm_record *result)
{
    return broker_base_vdm_select_next(state, broker_id, session_id,
        request_vdm_state, result, 1);
}

uint32_t broker_base_vdm_peek_next(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id, uint16_t request_vdm_state,
    broker_base_vdm_record *result)
{
    return broker_base_vdm_select_next(state, broker_id, session_id,
        request_vdm_state, result, 0);
}

uint32_t broker_base_vdm_consume(broker_base_vdm_state *state,
    const broker_base_vdm_record *record)
{
    broker_base_vdm_slot *slot;
    if (!state_valid(state) || !record_valid(record))
        return BROKER_BASE_VDM_STATUS_INVALID;
    slot = find_slot(state, record->broker_id, record->session_id);
    if (slot == NULL) return BROKER_BASE_VDM_STATUS_UNKNOWN;
    if (slot->available == 0u || slot->record.request_id != record->request_id)
        return BROKER_BASE_VDM_STATUS_BUSY;
    slot->available = 0u;
    slot->pending = 0u;
    return BROKER_BASE_VDM_STATUS_OK;
}

uint32_t broker_base_vdm_disconnect(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id)
{
    broker_base_vdm_slot *slot;
    if (!state_valid(state) || broker_id == 0u || session_id == 0u)
        return BROKER_BASE_VDM_STATUS_INVALID;
    slot = find_slot(state, broker_id, session_id);
    if (slot == NULL) return BROKER_BASE_VDM_STATUS_UNKNOWN;
    memset(slot, 0, sizeof(*slot));
    return BROKER_BASE_VDM_STATUS_OK;
}
