#include "base_vdm_broker.h"

#include "session/session.h"

#include <string.h>

static int bytes_valid(const uint8_t *bytes, uint32_t count, uint32_t maximum)
{
    return count <= maximum && (count == 0u || bytes != NULL);
}

static int binding_valid(const base_vdm_broker *binding)
{
    return binding != NULL && binding->version == BASE_VDM_BROKER_VERSION &&
        binding->struct_bytes == sizeof(*binding) && binding->owner != NULL &&
        session_valid(binding->owner) && binding->owner->state == SESSION_STATE_ACTIVE &&
        binding->broker_id != 0u && binding->session_id != 0u &&
        binding->session_id == binding->owner->identity && binding->open == 1u;
}

static void base_vdm_broker_teardown(void *context)
{
    base_vdm_broker *binding = (base_vdm_broker *)context;
    if (binding == NULL || binding->open == 0u) return;
    (void)broker_base_vdm_disconnect(&binding->records, binding->broker_id,
        binding->session_id);
    binding->open = 0u;
    binding->owner = NULL;
}

void base_vdm_broker_initialize(base_vdm_broker *binding)
{
    if (binding == NULL) return;
    memset(binding, 0, sizeof(*binding));
    binding->version = BASE_VDM_BROKER_VERSION;
    binding->struct_bytes = (uint32_t)sizeof(*binding);
    broker_base_vdm_initialize(&binding->records);
}

int base_vdm_broker_bind(base_vdm_broker *binding, session *owner,
    uint32_t broker_id)
{
    if (binding == NULL || binding->version != BASE_VDM_BROKER_VERSION ||
        binding->struct_bytes != sizeof(*binding) || binding->owner != NULL ||
        binding->open != 0u || owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || broker_id == 0u ||
        owner->identity == 0u)
        return 0;
    if (broker_base_vdm_register(&binding->records, broker_id,
        owner->identity) != BROKER_BASE_VDM_STATUS_OK)
        return 0;
    binding->owner = owner;
    binding->broker_id = broker_id;
    binding->session_id = owner->identity;
    binding->open = 1u;
    if (!session_register_teardown(owner, base_vdm_broker_teardown, binding)) {
        base_vdm_broker_teardown(binding);
        return 0;
    }
    return 1;
}

int base_vdm_broker_publish(base_vdm_broker *binding,
    const base_vdm_command *command)
{
    broker_base_vdm_record record;
    if (!binding_valid(binding) || command == NULL ||
        command->struct_bytes != sizeof(*command) || command->reserved0 != 0u ||
        command->reserved1 != 0u || command->command_owner != BASE_VDM_COMMAND_DOS ||
        command->command_bytes == 0u ||
        !bytes_valid(command->command, command->command_bytes,
            MAXIMUM_VDM_COMMAND_LENGTH) ||
        !bytes_valid(command->application, command->application_bytes,
            BROKER_BASE_VDM_APPLICATION_BYTES) ||
        !bytes_valid(command->environment, command->environment_bytes,
            MAXIMUM_VDM_ENVIORNMENT) ||
        !bytes_valid(command->current_directory, command->current_directory_bytes,
            BROKER_BASE_VDM_CURRENT_DIRECTORY_BYTES))
        return 0;
    memset(&record, 0, sizeof(record));
    record.version = BROKER_BASE_VDM_RECORD_VERSION;
    record.struct_bytes = (uint32_t)sizeof(record);
    record.broker_id = binding->broker_id;
    record.session_id = binding->session_id;
    record.request_id = 1u;
    record.task_id = command->task;
    record.creation_flags = command->creation_flags;
    record.error_code = command->error_code;
    record.code_page = command->code_page;
    record.vdm_state = ASKING_FOR_DOS_BINARY;
    record.current_drive = command->current_drive;
    record.command_bytes = command->command_bytes;
    record.application_bytes = command->application_bytes;
    record.environment_bytes = command->environment_bytes;
    record.current_directory_bytes = command->current_directory_bytes;
    record.coming_from_bat = command->coming_from_bat;
    memcpy(record.command, command->command, command->command_bytes);
    if (command->application_bytes != 0u)
        memcpy(record.application, command->application, command->application_bytes);
    if (command->environment_bytes != 0u)
        memcpy(record.environment, command->environment, command->environment_bytes);
    if (command->current_directory_bytes != 0u)
        memcpy(record.current_directory, command->current_directory,
            command->current_directory_bytes);
    return broker_base_vdm_publish(&binding->records, &record) ==
        BROKER_BASE_VDM_STATUS_OK;
}

int base_vdm_broker_deliver(base_vdm_broker *binding,
    base_vdm_local *destination)
{
    broker_base_vdm_record record;
    base_vdm_command command;
    uint32_t status;
    if (!binding_valid(binding) || !base_vdm_local_valid(destination) ||
        destination->available != 0u)
        return BASE_VDM_BROKER_DELIVERY_ERROR;
    status = broker_base_vdm_peek_next(&binding->records, binding->broker_id,
        binding->session_id, ASKING_FOR_DOS_BINARY, &record);
    if (status == BROKER_BASE_VDM_STATUS_PENDING ||
        status == BROKER_BASE_VDM_STATUS_NO_COMMAND)
        return BASE_VDM_BROKER_DELIVERY_PENDING;
    if (status != BROKER_BASE_VDM_STATUS_OK)
        return BASE_VDM_BROKER_DELIVERY_ERROR;
    memset(&command, 0, sizeof(command));
    command.struct_bytes = (uint32_t)sizeof(command);
    command.task = record.task_id;
    command.creation_flags = record.creation_flags;
    command.error_code = record.error_code;
    command.code_page = record.code_page;
    command.current_drive = record.current_drive;
    command.coming_from_bat = record.coming_from_bat;
    command.command_owner = BASE_VDM_COMMAND_DOS;
    command.command = record.command;
    command.command_bytes = record.command_bytes;
    command.application = record.application;
    command.application_bytes = record.application_bytes;
    command.environment = record.environment;
    command.environment_bytes = record.environment_bytes;
    command.current_directory = record.current_directory;
    command.current_directory_bytes = record.current_directory_bytes;
    if (!base_vdm_local_publish(destination, &command))
        return BASE_VDM_BROKER_DELIVERY_ERROR;
    return broker_base_vdm_consume(&binding->records, &record) ==
        BROKER_BASE_VDM_STATUS_OK ? BASE_VDM_BROKER_DELIVERY_COMPLETE :
        BASE_VDM_BROKER_DELIVERY_ERROR;
}

int base_vdm_broker_unbind(base_vdm_broker *binding)
{
    if (!binding_valid(binding)) return 0;
    base_vdm_broker_teardown(binding);
    return 1;
}
