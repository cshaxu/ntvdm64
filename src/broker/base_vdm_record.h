#ifndef BROKER_BASE_VDM_RECORD_H
#define BROKER_BASE_VDM_RECORD_H

#include <windows.h>

#include <stdint.h>

#include <vdmapi.h>

#define BROKER_BASE_VDM_RECORD_VERSION UINT32_C(1)
#define BROKER_BASE_VDM_MAXIMUM_RECORDS 16u
#define BROKER_BASE_VDM_APPLICATION_BYTES 260u
#define BROKER_BASE_VDM_CURRENT_DIRECTORY_BYTES 261u

enum broker_base_vdm_status {
    BROKER_BASE_VDM_STATUS_OK = 0u,
    BROKER_BASE_VDM_STATUS_INVALID = 1u,
    BROKER_BASE_VDM_STATUS_UNKNOWN = 2u,
    BROKER_BASE_VDM_STATUS_CAPACITY = 3u,
    BROKER_BASE_VDM_STATUS_PENDING = 4u,
    BROKER_BASE_VDM_STATUS_NO_COMMAND = 5u,
    BROKER_BASE_VDM_STATUS_BUSY = 6u
};

/* This is the fixed-width, copied DOS subset of the historical VDMINFO
 * exchange. It intentionally contains neither the original CSR capture
 * pointers/HANDLEs nor any guest or mapping-manager values. */
typedef struct broker_base_vdm_record {
    uint32_t version;
    uint32_t struct_bytes;
    uint32_t broker_id;
    uint32_t session_id;
    uint32_t request_id;
    uint32_t task_id;
    uint32_t creation_flags;
    uint32_t error_code;
    uint32_t code_page;
    uint32_t environment_bytes;
    /* Preserve the original VDMINFO field's role: this is VDMState, not a
     * native request object nor a host-process state. */
    uint16_t vdm_state;
    uint16_t current_drive;
    uint16_t command_bytes;
    uint16_t application_bytes;
    uint16_t current_directory_bytes;
    uint8_t coming_from_bat;
    uint8_t reserved0;
    uint8_t command[MAXIMUM_VDM_COMMAND_LENGTH];
    uint8_t application[BROKER_BASE_VDM_APPLICATION_BYTES];
    uint8_t environment[MAXIMUM_VDM_ENVIORNMENT];
    uint8_t current_directory[BROKER_BASE_VDM_CURRENT_DIRECTORY_BYTES];
} broker_base_vdm_record;

typedef struct broker_base_vdm_slot {
    uint32_t active;
    uint32_t available;
    uint32_t pending;
    broker_base_vdm_record record;
} broker_base_vdm_slot;

typedef struct broker_base_vdm_state {
    uint32_t version;
    uint32_t struct_bytes;
    broker_base_vdm_slot slots[BROKER_BASE_VDM_MAXIMUM_RECORDS];
} broker_base_vdm_state;

void broker_base_vdm_initialize(broker_base_vdm_state *state);
uint32_t broker_base_vdm_register(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id);
uint32_t broker_base_vdm_publish(broker_base_vdm_state *state,
    const broker_base_vdm_record *record);
uint32_t broker_base_vdm_get_next(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id, uint16_t request_vdm_state,
    broker_base_vdm_record *result);
uint32_t broker_base_vdm_peek_next(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id, uint16_t request_vdm_state,
    broker_base_vdm_record *result);
uint32_t broker_base_vdm_consume(broker_base_vdm_state *state,
    const broker_base_vdm_record *record);
uint32_t broker_base_vdm_disconnect(broker_base_vdm_state *state,
    uint32_t broker_id, uint32_t session_id);

#endif
