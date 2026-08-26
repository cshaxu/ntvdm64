#ifndef BYOB_PROFILE_H
#define BYOB_PROFILE_H

#include <stdint.h>
#include <wchar.h>

#define BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS 4u
#define BYOB_PROFILE_DRIVE_SLOT_COUNT 26u
#define BYOB_PROFILE_GUEST_PATH_MAX_CHARS 64u
#define BYOB_PROFILE_MAX_DECLARED_TARGETS 2u

typedef struct byob_component_descriptor {
    wchar_t file_name[64];
    wchar_t sha256[65];
    uint64_t bytes;
} byob_component_descriptor;

/* Profile metadata only. The runner never reads guest memory from this
 * declaration; an adapter may later bind it to its own controlled boundary. */
typedef struct byob_machine_observation_descriptor {
    uint32_t id;
    uint64_t physical_address;
    uint64_t byte_count;
} byob_machine_observation_descriptor;

/* A v0 passive-observation trigger has no guest meaning. It is deliberately
 * fixed to one profile-owned range and one copied #UD byte prefix. */
typedef struct byob_machine_observation_trigger_descriptor {
    uint32_t observation_id;
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint32_t resume_bytes;
} byob_machine_observation_trigger_descriptor;

/* One profile-owned multi-range snapshot trigger. The IDs select existing
 * immutable observation declarations; they are not raw memory addresses. */
typedef struct byob_machine_startup_snapshot_trigger_descriptor {
    uint32_t observation_count;
    uint32_t observation_ids[BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS];
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint32_t resume_bytes;
} byob_machine_startup_snapshot_trigger_descriptor;

/* Closed native-startup rendezvous.  This is a profile declaration, not a
 * general exception or guest-service interface. */
typedef struct byob_machine_startup_handoff_descriptor {
    uint32_t exception_vector;
    uint8_t instruction_bytes[2];
    uint16_t cs;
    uint16_t ip;
} byob_machine_startup_handoff_descriptor;

/* Host-capability input only. Values are the historical Win32 drive-type
 * domain 0..6; this declaration never names a host path or Bochs device. */
typedef struct byob_host_drive_inventory_descriptor {
    uint8_t types[BYOB_PROFILE_DRIVE_SLOT_COUNT];
} byob_host_drive_inventory_descriptor;

/* guest-composition metadata.  This is a DOS identity, never a local
 * pathname: the adapter receives immutable selected bytes separately. */
typedef struct byob_guest_artifact_placement_descriptor {
    wchar_t path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
    uint32_t drive_index;
} byob_guest_artifact_placement_descriptor;

/* These are profile-selected, adapter-materialized resources.  They never
 * name a host file and are intentionally a closed set for the first profile. */
typedef enum byob_guest_boot_file_materialization {
    BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT = 1,
    BYOB_GUEST_BOOT_FILE_EMPTY = 2
} byob_guest_boot_file_materialization;

typedef struct byob_guest_boot_file_descriptor {
    wchar_t path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
    uint32_t materialization;
} byob_guest_boot_file_descriptor;

/* Immutable DOS-visible metadata for the closed first-profile namespace.
 * This is supplied by the profile author; it is never obtained from the
 * host filesystem or clock at runtime. */
typedef struct byob_guest_dos_metadata_descriptor {
    uint32_t attributes;
    uint16_t dos_time;
    uint16_t dos_date;
} byob_guest_dos_metadata_descriptor;

/* The first non-invasive CLI profile has one display disposition.  This is a
 * closed guest-composition value, not a user-facing host console setting. */
typedef enum byob_guest_display_state {
    BYOB_GUEST_DISPLAY_STATE_NONE = 0,
    BYOB_GUEST_DISPLAY_STATE_STREAM_IO = 2
} byob_guest_display_state;

/* A fixed guest command sequence is profile data, not a host queue.  The
 * first v5 profile has exactly two entries: the selected target then QUIT. */
typedef struct byob_declared_target_descriptor {
    byob_component_descriptor component;
    byob_guest_artifact_placement_descriptor placement;
    byob_guest_dos_metadata_descriptor metadata;
    uint32_t terminal;
} byob_declared_target_descriptor;

typedef struct byob_profile_selection {
    byob_component_descriptor ntio;
    byob_component_descriptor ntdos;
    byob_component_descriptor command;
    byob_component_descriptor target;
    byob_component_descriptor terminal_quit;
    uint32_t machine_observation_count;
    byob_machine_observation_descriptor
        machine_observations[BYOB_PROFILE_MAX_MACHINE_OBSERVATIONS];
    uint32_t has_machine_observation_trigger;
    byob_machine_observation_trigger_descriptor machine_observation_trigger;
    uint32_t has_machine_startup_snapshot_trigger;
    byob_machine_startup_snapshot_trigger_descriptor
        machine_startup_snapshot_trigger;
    wchar_t machine_startup_snapshot_evidence_file_name[64];
    uint32_t machine_startup_plan_enabled;
    uint32_t machine_startup_entry_ntio_v0;
    uint32_t has_machine_external_initial_state;
    wchar_t machine_external_initial_state_evidence_sha256[65];
    uint32_t has_machine_startup_handoff;
    byob_machine_startup_handoff_descriptor machine_startup_handoff;
    uint32_t has_host_drive_inventory;
    byob_host_drive_inventory_descriptor host_drive_inventory;
    uint32_t has_command_placement;
    byob_guest_artifact_placement_descriptor command_placement;
    uint32_t has_target_placement;
    byob_guest_artifact_placement_descriptor target_placement;
    uint32_t has_guest_boot_files;
    byob_guest_boot_file_descriptor config_file;
    byob_guest_boot_file_descriptor autoexec_file;
    uint32_t has_guest_search_metadata;
    byob_guest_dos_metadata_descriptor command_metadata;
    byob_guest_dos_metadata_descriptor target_metadata;
    byob_guest_dos_metadata_descriptor terminal_quit_metadata;
    byob_guest_dos_metadata_descriptor config_metadata;
    byob_guest_dos_metadata_descriptor autoexec_metadata;
    uint32_t guest_display_state;
    uint32_t declared_target_count;
    byob_declared_target_descriptor
        declared_targets[BYOB_PROFILE_MAX_DECLARED_TARGETS];
} byob_profile_selection;

#endif
