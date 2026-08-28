#ifndef BASE_VDM_LOCAL_H
#define BASE_VDM_LOCAL_H

#include <stdint.h>

#include <nt.h>
#include <vdmapi.h>

typedef struct session session;

#define BASE_VDM_LOCAL_VERSION UINT32_C(1)

/* A copied DOS record for the reached BaseSrv command path. */
typedef struct base_vdm_command {
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
} base_vdm_command;

typedef struct base_vdm_local {
    uint32_t version;
    uint32_t struct_bytes;
    session *owner;
    uint32_t reentry_count;
    uint32_t available;
    uint32_t first_vdm_available;
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
    uint32_t current_directories_bytes;
    uint8_t *current_directories;
    uint8_t command[MAXIMUM_VDM_COMMAND_LENGTH];
    uint8_t application[MAXIMUM_VDM_PATH_STRING];
    uint8_t environment[MAXIMUM_VDM_ENVIORNMENT];
    uint8_t current_directory[MAXIMUM_VDM_CURRENT_DIR];
} base_vdm_local;

#ifdef __cplusplus
extern "C" {
#endif

void base_vdm_local_initialize(base_vdm_local *record);
int base_vdm_local_valid(const base_vdm_local *record);
int base_vdm_local_publish(base_vdm_local *record,
    const base_vdm_command *command);
int base_vdm_local_bind(base_vdm_local *record, session *owner);
int base_vdm_local_unbind(base_vdm_local *record);
BOOL base_vdm_local_dispatch(PVDMINFO information);
BOOL base_vdm_local_is_first(void);
BOOL base_vdm_local_set_current_directories(ULONG byte_count,
    const CHAR *directories);
ULONG base_vdm_local_get_current_directories(ULONG byte_count,
    CHAR *directories);
VOID base_vdm_local_exit(BOOL wow_caller, ULONG wow_task);
BOOL APIENTRY GetNextVDMCommand(PVDMINFO information);
VOID APIENTRY ExitVDM(BOOL wow_caller, ULONG wow_task);
BOOL APIENTRY SetVDMCurrentDirectories(ULONG byte_count, CHAR *directories);
ULONG APIENTRY GetVDMCurrentDirectories(ULONG byte_count, CHAR *directories);

#ifdef __cplusplus
}
#endif

#endif
