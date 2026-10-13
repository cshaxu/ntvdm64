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
/* Copied with the command before the detached worker starts. */
int mvdm_command_native_child_window_style(DWORD *style_out);
const char *mvdm_command_native_child_command(void);
const char *mvdm_command_native_child_environment(void);
int mvdm_command_native_child_replace_command(const char *command);
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
#endif
