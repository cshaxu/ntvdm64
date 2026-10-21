/* Read the original task descriptor, never a guessed TEB/TD layout. */
#include "precomp.h"

BOOL WINAPI wow_current_separate_launch(void)
{
    PTD task=CURRENTPTD();
    return fSeparateWow && task && task->VDMInfoiTaskID!=(INT)-1;
}
