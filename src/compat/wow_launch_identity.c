/* Read the original task descriptor, never a guessed TEB/TD layout. */
#include "non-mvdm/public/sdk/inc/nt.h"
#include "wow32_provider_private.h"
#undef _X86_
#include "mvdm-host/wow32/wow32.h"
#include "include/wow_startup_report.h"

BOOL WINAPI wow_current_separate_launch(void)
{
    PTD task=CURRENTPTD();
    return fSeparateWow && task && task->VDMInfoiTaskID!=(INT)-1;
}
