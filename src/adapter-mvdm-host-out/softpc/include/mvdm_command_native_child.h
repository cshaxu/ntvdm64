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
void mvdm_command_native_child_finish(void);
void mvdm_command_native_child_abort(void);
/* Capture and remove the default-off, host-private payload report selector
 * before original COMMAND imports inherited variables into guest state. */
void mvdm_command_native_child_capture_report_path(void);

#endif
