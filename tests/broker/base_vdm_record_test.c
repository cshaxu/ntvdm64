#include "broker/base_vdm_record.h"

#include <string.h>

static void initialize_record(broker_base_vdm_record *record,
    uint32_t broker_id, uint32_t session_id)
{
    memset(record, 0, sizeof(*record));
    record->version = BROKER_BASE_VDM_RECORD_VERSION;
    record->struct_bytes = (uint32_t)sizeof(*record);
    record->broker_id = broker_id;
    record->session_id = session_id;
    record->request_id = 7u;
    record->vdm_state = ASKING_FOR_DOS_BINARY;
    record->command[0] = '/';
    record->command[1] = 'C';
    record->command[2] = '\0';
    record->command_bytes = 3u;
}

int main(void)
{
    broker_base_vdm_state state;
    broker_base_vdm_record source, result;
    const uint32_t broker_id = 11u;
    const uint32_t session_id = 23u;

    broker_base_vdm_initialize(&state);
    if (broker_base_vdm_register(&state, broker_id, session_id) !=
        BROKER_BASE_VDM_STATUS_OK)
        return 1;
    if (broker_base_vdm_get_next(&state, broker_id, session_id,
        ASKING_FOR_DOS_BINARY, &result) != BROKER_BASE_VDM_STATUS_PENDING)
        return 2;
    if (broker_base_vdm_get_next(&state, broker_id, session_id,
        ASKING_FOR_DOS_BINARY | ASKING_FOR_SECOND_TIME |
        RETURN_ON_NO_COMMAND, &result) != BROKER_BASE_VDM_STATUS_NO_COMMAND)
        return 3;

    initialize_record(&source, broker_id, session_id);
    if (broker_base_vdm_publish(&state, &source) != BROKER_BASE_VDM_STATUS_OK)
        return 4;
    memset(&result, 0, sizeof(result));
    if (broker_base_vdm_peek_next(&state, broker_id, session_id,
        ASKING_FOR_DOS_BINARY, &result) != BROKER_BASE_VDM_STATUS_OK ||
        memcmp(&source, &result, sizeof(source)) != 0)
        return 5;
    if (broker_base_vdm_consume(&state, &result) != BROKER_BASE_VDM_STATUS_OK ||
        broker_base_vdm_consume(&state, &result) != BROKER_BASE_VDM_STATUS_BUSY)
        return 6;
    if (broker_base_vdm_get_next(&state, broker_id, session_id,
        ASKING_FOR_DOS_BINARY | ASKING_FOR_WOW_BINARY, &result) !=
        BROKER_BASE_VDM_STATUS_INVALID)
        return 7;
    if (broker_base_vdm_get_next(&state, broker_id, session_id + 1u,
        ASKING_FOR_DOS_BINARY, &result) != BROKER_BASE_VDM_STATUS_UNKNOWN)
        return 8;
    if (broker_base_vdm_disconnect(&state, broker_id, session_id) !=
        BROKER_BASE_VDM_STATUS_OK ||
        broker_base_vdm_disconnect(&state, broker_id, session_id) !=
        BROKER_BASE_VDM_STATUS_UNKNOWN)
        return 9;
    return 0;
}
