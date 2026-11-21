#include "ntvdm-exe/command/include/command_launch.h"
#include "compat/include/mvdm_command_guest_directory.h"
#include "compat/include/mvdm_command_native_child.h"
#include "compat/include/command_services.h"

extern BOOL cmdXformEnvironment(char *, struct _STRING *);
extern int dem_command_directory_instance(struct guest_memory_lease_context *,
    uint32_t, uint16_t, uint16_t, uint32_t *, uint32_t *);

BOOL ntvdm_command_initialize(void)
{
    static const compat_command_services services = {
        mvdm_command_native_child_capture_guest,
        mvdm_command_native_child_capture_host_command,
        mvdm_command_native_child_std_handles,
        mvdm_command_native_child_command,
        mvdm_command_native_child_environment,
        mvdm_command_native_child_replace_command,
        mvdm_command_native_child_select_command_service,
        mvdm_command_native_child_finish,
        mvdm_command_guest_state_clear_doswow,
        mvdm_command_guest_state_register_doswow,
        cmdXformEnvironment,
        mvdm_guest_location_acquire_vm, mvdm_guest_location_release,
        dem_command_directory_instance
    };
    return compat_command_install(&services);
}
