#include "base_vdm_broker.h"
#include "session/session.h"

#include <string.h>

static void initialize_command(base_vdm_command *command)
{
    static const uint8_t command_bytes[] = { '/', 'C', '\r', '\n', '\0' };
    static const uint8_t application_bytes[] = { 'C', ':', '\\', 'C', 'M',
        'D', '.', 'C', 'O', 'M', '\0' };
    static const uint8_t environment_bytes[] = { '\0', '\0' };
    static const uint8_t directory_bytes[] = { 'C', ':', '\\', '\0' };

    memset(command, 0, sizeof(*command));
    command->struct_bytes = (uint32_t)sizeof(*command);
    command->task = 1u;
    command->code_page = 437u;
    command->command_owner = BASE_VDM_COMMAND_DOS;
    command->command = command_bytes;
    command->command_bytes = (uint16_t)sizeof(command_bytes);
    command->application = application_bytes;
    command->application_bytes = (uint16_t)sizeof(application_bytes);
    command->environment = environment_bytes;
    command->environment_bytes = (uint32_t)sizeof(environment_bytes);
    command->current_directory = directory_bytes;
    command->current_directory_bytes = (uint16_t)sizeof(directory_bytes);
}

int main(void)
{
    session owner;
    base_vdm_local local;
    base_vdm_broker broker;
    base_vdm_command command;

    session_initialize(&owner, 23u);
    if (!session_activate(&owner)) return 1;
    base_vdm_local_initialize(&local);
    base_vdm_broker_initialize(&broker);
    if (!base_vdm_local_bind(&local, &owner) ||
        !base_vdm_broker_bind(&broker, &owner, 11u))
        return 2;
    initialize_command(&command);
    if (!base_vdm_broker_publish(&broker, &command)) return 3;
    local.available = 1u;
    if (base_vdm_broker_deliver(&broker, &local) !=
        BASE_VDM_BROKER_DELIVERY_ERROR)
        return 4;
    local.available = 0u;
    if (base_vdm_broker_deliver(&broker, &local) !=
        BASE_VDM_BROKER_DELIVERY_COMPLETE || local.available == 0u ||
        local.command_bytes != command.command_bytes ||
        memcmp(local.command, command.command, command.command_bytes) != 0)
        return 5;
    if (!base_vdm_broker_unbind(&broker) ||
        !base_vdm_local_unbind(&local) || !session_dispose(&owner))
        return 6;
    return 0;
}
