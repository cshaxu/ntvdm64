#include <stdio.h>
#include <string.h>

#include "app/command_source.h"
#include "vdm_command.h"
#include "session/session.h"

static void reset_info(VDMINFO *information)
{
    memset(information, 0, sizeof(*information));
}

int main(void)
{
    static const uint8_t command[] = "C:\\DOS\\COMMAND.COM /C VER";
    static const uint8_t application[] = "C:\\DOS\\COMMAND.COM";
    static const uint8_t environment[] = "COMSPEC=C:\\DOS\\COMMAND.COM\0PATH=C:\\DOS\0\0";
    static const uint8_t directory[] = "C:\\DOS\0";
    app_command_payload payload;
    app_command_source source;
    session instance;
    VDMINFO information;
    uint8_t command_buffer[MAXIMUM_VDM_COMMAND_LENGTH];
    uint8_t application_buffer[MAXIMUM_VDM_PATH_STRING];
    uint8_t environment_buffer[MAXIMUM_VDM_ENVIORNMENT];
    uint8_t directory_buffer[MAXIMUM_VDM_CURRENT_DIR];

    reset_info(&information);
    if (GetNextVDMCommand(NULL) || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return 1;
    session_initialize(&instance, 1u);
    app_command_source_initialize(&source);
    memset(&payload, 0, sizeof(payload));
    payload.struct_bytes = sizeof(payload);
    payload.task = 7u;
    payload.creation_flags = 19u;
    payload.error_code = 23u;
    payload.code_page = 437u;
    payload.current_drive = 2u;
    payload.coming_from_bat = TRUE;
    payload.command = command;
    payload.command_bytes = (uint16_t)sizeof(command);
    payload.application = application;
    payload.application_bytes = (uint16_t)sizeof(application);
    payload.environment = environment;
    payload.environment_bytes = (uint32_t)sizeof(environment);
    payload.current_directory = directory;
    payload.current_directory_bytes = (uint16_t)sizeof(directory);
    if (!app_command_source_publish(&source, &payload) ||
        !session_activate(&instance) || !app_command_source_bind(&source, &instance) ||
        !session_thread_bind(&instance)) return 2;

    reset_info(&information);
    information.VDMState = ASKING_FOR_ENVIRONMENT;
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    if (!GetNextVDMCommand(&information) ||
        information.EnviornmentSize != sizeof(environment) ||
        memcmp(environment_buffer, environment, sizeof(environment)) != 0) return 3;

    reset_info(&information);
    information.CmdLine = command_buffer;
    information.CmdSize = 1u;
    information.AppName = application_buffer;
    information.AppLen = 1u;
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = 1u;
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = 1u;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_INVALID_PARAMETER ||
        information.CmdSize != sizeof(command) || information.AppLen != sizeof(application) ||
        information.EnviornmentSize != sizeof(environment) ||
        information.CurDirectoryLen != sizeof(directory)) return 4;

    reset_info(&information);
    information.CmdLine = command_buffer;
    information.CmdSize = sizeof(command_buffer);
    information.AppName = application_buffer;
    information.AppLen = sizeof(application_buffer);
    information.Enviornment = environment_buffer;
    information.EnviornmentSize = sizeof(environment_buffer);
    information.CurDirectory = directory_buffer;
    information.CurDirectoryLen = sizeof(directory_buffer);
    if (!GetNextVDMCommand(&information) || information.VDMState != 0u ||
        information.iTask != 7u || information.dwCreationFlags != 19u ||
        information.ErrorCode != 23u || information.CodePage != 437u ||
        information.CurDrive != 2u || !information.fComingFromBat ||
        information.StdIn != NULL || information.StdOut != NULL || information.StdErr != NULL ||
        memcmp(command_buffer, command, sizeof(command)) != 0 ||
        memcmp(application_buffer, application, sizeof(application)) != 0 ||
        memcmp(environment_buffer, environment, sizeof(environment)) != 0 ||
        memcmp(directory_buffer, directory, sizeof(directory)) != 0) return 5;

    reset_info(&information);
    information.VDMState = RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_NOT_ENOUGH_MEMORY ||
        information.CmdSize != 0u || information.EnviornmentSize != 0u) return 6;
    reset_info(&information);
    information.VDMState = INCREMENT_REENTER_COUNT;
    if (!GetNextVDMCommand(&information)) return 7;
    information.VDMState = DECREMENT_REENTER_COUNT;
    if (!GetNextVDMCommand(&information)) return 8;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_INVALID_PARAMETER)
        return 9;
    reset_info(&information);
    information.VDMState = ASKING_FOR_WOW_BINARY;
    if (GetNextVDMCommand(&information) || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED)
        return 10;
    if (!app_command_source_unbind(&source)) return 11;
    reset_info(&information);
    if (GetNextVDMCommand(&information) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 12;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 13;
    puts("PASS: local VDM command transport");
    return 0;
}
