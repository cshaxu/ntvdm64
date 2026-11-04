#include "lib-win32/process-hook/staging.h"
#include "lib-win32/process-hook/helper.h"
#include "lib-win32/process-hook/startup_hooks.h"
#include "lib-win32/process-hook/late_patch.h"
#include <stdio.h>
int main(void)
{
    if(process_hook_stage_process(NULL,NULL,NULL,0)!=ERROR_INVALID_PARAMETER)return 1;
    if(process_hook_stage_read(NULL,NULL,NULL)!=ERROR_INVALID_PARAMETER)return 2;
    if(process_hook_install_helper(NULL,NULL,NULL,0,NULL)!=ERROR_INVALID_PARAMETER)return 3;
    if(process_hook_run_helper(NULL,NULL)!=ERROR_INVALID_PARAMETER)return 4;
    if(process_hook_late_patch_discard(NULL)!=ERROR_INVALID_STATE)return 5;
    if(process_hook_startup_begin()!=ERROR_SUCCESS)return 6;
    process_hook_startup_abort();
    puts("PROCESS_HOOK_LINK_PASS");return 0;
}
