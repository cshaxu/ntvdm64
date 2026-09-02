#ifndef BASE_VDM_LOCAL_H
#define BASE_VDM_LOCAL_H

#include <stdint.h>

#include <nt.h>
#include <vdmapi.h>

typedef struct session session;

#define BASE_VDM_LOCAL_VERSION UINT32_C(4)

/* These identify which original BaseSrv command queue owns a copied record.
 * They are not guest values and never enter VDMINFO: the original client
 * derives the same choice from its VDMState and ConsoleHandle. */
#define BASE_VDM_COMMAND_DOS UINT16_C(0)
#define BASE_VDM_COMMAND_WOW ASKING_FOR_WOW_BINARY

/* Reached BaseSrv DOS-record states.  These are adapter-private host state,
 * never VDMINFO or guest state.  A one-session record has no NT4 parent
 * process/wait-handle pair, but it must still distinguish a command waiting
 * to run, an active child, and the child's completed return. */
#define BASE_VDM_DOS_RECORD_EMPTY UINT32_C(0)
#define BASE_VDM_DOS_RECORD_TO_TAKE_A_COMMAND UINT32_C(1)
#define BASE_VDM_DOS_RECORD_BUSY UINT32_C(2)
#define BASE_VDM_DOS_RECORD_HAS_RETURNED_ERROR_CODE UINT32_C(3)

/* A copied record for one reached original BaseSrv command path. */
typedef struct base_vdm_command {
    uint32_t struct_bytes;
    uint32_t task;
    uint32_t creation_flags;
    uint32_t error_code;
    uint32_t code_page;
    uint16_t current_drive;
    uint8_t coming_from_bat;
    uint8_t reserved0;
    uint16_t command_owner;
    uint16_t reserved1;
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
    uint16_t command_owner;
    uint16_t reserved1;
    uint16_t command_bytes;
    uint16_t application_bytes;
    uint32_t environment_bytes;
    uint16_t current_directory_bytes;
    uint32_t current_directories_bytes;
    uint8_t *current_directories;
    /* Adapter-private equivalents of the original BaseSrv DOS-record lock
     * and `hWaitForVDM`.  They never enter VDMINFO, MVDM or guest state. */
    CRITICAL_SECTION lock;
    HANDLE wake_event;
    uint32_t lock_initialized;
    uint32_t pending_request;
    uint32_t dos_record_state;
    uint8_t command[MAXIMUM_VDM_COMMAND_LENGTH];
    /* BaseClient/BaseSrv carry the host application path independently of
     * the DOS-shaped guest path fields.  The original client uses MAX_PATH
     * path storage for this host-side value. */
    uint8_t application[MAX_PATH];
    uint8_t environment[MAXIMUM_VDM_ENVIORNMENT];
    /* `VDMINFO.CurDirectory` is a host-side BaseClient/BaseSrv carrier.
     * Original COMMAND advertises MAX_PATH + 1 bytes for it, independently of
     * the smaller guest drive-relative component limit. */
    uint8_t current_directory[MAX_PATH + 1u];
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
int base_vdm_local_wait_for_command(PVDMINFO information);
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
