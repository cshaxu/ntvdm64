#ifndef COMPAT_COMMAND_NATIVE_CHILD_H
#define COMPAT_COMMAND_NATIVE_CHILD_H

#include "command_services.h"

/* Source-shaped names for the selected COMMAND caller. Product providers
 * include command_services.h instead, so definitions keep their own names. */
#define mvdm_command_native_child_capture_guest compat_command_current_services()->capture_guest
#define mvdm_command_native_child_capture_host_command compat_command_current_services()->capture_host_command
#define mvdm_command_native_child_std_handles compat_command_current_services()->std_handles
#define mvdm_command_native_child_command compat_command_current_services()->command
#define mvdm_command_native_child_environment compat_command_current_services()->environment
#define mvdm_command_native_child_replace_command compat_command_current_services()->replace_command
#define mvdm_command_native_child_select_command_service compat_command_current_services()->select_service
#define mvdm_command_native_child_finish compat_command_current_services()->finish
#define mvdm_command_native_child_abort compat_command_current_services()->finish

#endif
