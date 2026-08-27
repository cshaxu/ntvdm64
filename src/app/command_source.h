#ifndef APP_COMMAND_SOURCE_H
#define APP_COMMAND_SOURCE_H

#include <stdint.h>

#include <nt.h>
#include <vdmapi.h>

typedef struct session session;

#define APP_COMMAND_SOURCE_VERSION UINT32_C(1)

/*
 * A copied, one-session replacement for the reached DOS command record from
 * BaseSrv.  It carries only byte payloads and scalar metadata: no caller
 * pointer, Windows HANDLE, CSR record or guest address crosses this boundary.
 */
typedef struct app_command_payload {
    uint32_t struct_bytes;
    uint32_t task;
    uint32_t creation_flags;
    uint32_t error_code;
    uint32_t code_page;
    uint16_t current_drive;
    uint8_t coming_from_bat;
    uint8_t reserved0;
    const uint8_t *command;
    uint16_t command_bytes;
    const uint8_t *application;
    uint16_t application_bytes;
    const uint8_t *environment;
    uint32_t environment_bytes;
    const uint8_t *current_directory;
    uint16_t current_directory_bytes;
} app_command_payload;

typedef struct app_command_source {
    uint32_t version;
    uint32_t struct_bytes;
    session *owner;
    uint32_t reentry_count;
    uint32_t available;
    uint32_t task;
    uint32_t creation_flags;
    uint32_t error_code;
    uint32_t code_page;
    uint16_t current_drive;
    uint8_t coming_from_bat;
    uint8_t reserved0;
    uint16_t command_bytes;
    uint16_t application_bytes;
    uint32_t environment_bytes;
    uint16_t current_directory_bytes;
    uint8_t command[MAXIMUM_VDM_COMMAND_LENGTH];
    uint8_t application[MAXIMUM_VDM_PATH_STRING];
    uint8_t environment[MAXIMUM_VDM_ENVIORNMENT];
    uint8_t current_directory[MAXIMUM_VDM_CURRENT_DIR];
} app_command_source;

#ifdef __cplusplus
extern "C" {
#endif

void app_command_source_initialize(app_command_source *source);
int app_command_source_valid(const app_command_source *source);
int app_command_source_publish(app_command_source *source,
    const app_command_payload *payload);
int app_command_source_bind(app_command_source *source, session *owner);
int app_command_source_unbind(app_command_source *source);

#ifdef __cplusplus
}
#endif

#endif
