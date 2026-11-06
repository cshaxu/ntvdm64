#ifndef MVDM_COMMAND_NATIVE_CHILD_H
#define MVDM_COMMAND_NATIVE_CHILD_H

#include <stdint.h>
#include <windows.h>

/*
 * Source-shaped lifetime seam for COMMAND cmdExec32.  OpenNT keeps command,
 * environment and STD_HANDLES as process aliases until cmdCreateProcess runs
 * on its detached worker.  The modern session boundary instead snapshots
 * those exact inputs before the worker starts.  The original COMMAND worker
 * still owns conversion, CreateProcess, wait, exit-code and re-entry order.
 */
int mvdm_command_native_child_capture_guest(uint16_t command_segment,
    uint16_t command_offset, uint16_t environment_segment,
    uint16_t environment_offset, uint16_t standard_segment,
    uint16_t standard_offset);
int mvdm_command_native_child_capture_host_command(const char *command,
    uint16_t environment_segment, uint16_t environment_offset,
    uint16_t standard_segment, uint16_t standard_offset);
int mvdm_command_native_child_activate(const char *source_command,
    const char *source_environment, const char **command_out,
    const char **environment_out);
int mvdm_command_native_child_std_handles(ULONG handles_out[3]);
const char *mvdm_command_native_child_command(void);
const char *mvdm_command_native_child_environment(void);
int mvdm_command_native_child_replace_command(const char *command);
/* Shared original cmdExec/cmdExecComspec32 host COMSPEC preparation.
 * NULL tail selects the executable alone; otherwise append the /c tail. */
DWORD mvdm_command_prepare_comspec(char command[MAX_PATH], const char *tail);
/* The branch itself is the authority.  The adapter must never recover it by
 * parsing COMSPEC, a program name, or shell punctuation. */
enum {
    MVDM_COMMAND_SERVICE_NONE = 0,
    MVDM_COMMAND_SERVICE_INTERACTIVE = 1,
    MVDM_COMMAND_SERVICE_EXECUTE = 2
};
int mvdm_command_native_child_select_command_service(unsigned int mode);
unsigned int mvdm_command_native_child_command_service(const char **tail_out);
void mvdm_command_native_child_finish(void);
void mvdm_command_native_child_abort(void);

typedef struct mvdm_command_native_child_state mvdm_command_capture;
typedef struct mvdm_command_capture_input {
    /* NULL selects the guest command, otherwise copy this host command. */
    const char *host_command;
    uint16_t command_segment,command_offset;
    uint16_t environment_segment,environment_offset;
    uint16_t standard_segment,standard_offset;
    uint8_t default_drive; /* Original AL, zero based. */
    unsigned int service;
    uint32_t vm_linear_base; /* actual VMM CB_High_Linear, zero on ordinary path */
    const char *directory; /* optional independent absolute directory copy */
} mvdm_command_capture_input;
typedef struct mvdm_command_capture_view {
    const char *command,*environment,*directory;
    uint32_t environment_bytes;
    ULONG standard_handles[3]; /* Original tokens; not owned native handles. */
    uint8_t default_drive;
    unsigned int service;
} mvdm_command_capture_view;
/* Synchronous current-VM capture using the same bounded leases as ordinary
 * COMMAND. Does not read/write the session singleton or retain guest mappings.
 * Views are borrowed until destroy. No CCPU activity may race this capture. */
int mvdm_command_capture_create(const mvdm_command_capture_input *,mvdm_command_capture **);
int mvdm_command_capture_read(const mvdm_command_capture *,mvdm_command_capture_view *);
void mvdm_command_capture_destroy(mvdm_command_capture *);
#endif
