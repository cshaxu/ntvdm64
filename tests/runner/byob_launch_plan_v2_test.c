#include "byob_launch_plan_v2.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    byob_profile_selection selection = {0};
    byob_launch_plan_v2 plan, restored;
    wchar_t environment[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    wchar_t *arguments[] = { L"/c", L"hello" };
    selection.declared_target_count = 2u;
    wcscpy(selection.declared_targets[0].component.file_name, L"TARGET.COM");
    wcscpy(selection.declared_targets[1].component.file_name, L"QUIT.COM");
    selection.declared_targets[1].terminal = 1u;
    if (!byob_launch_plan_v2_from_arguments(&plan, &selection, 2, arguments) ||
        plan.slot_count != 2u || plan.first.tail_bytes != 8u ||
        !byob_launch_plan_v2_to_environment(&plan, environment) ||
        !byob_launch_plan_v2_from_environment(&restored, environment) ||
        memcmp(&plan, &restored, sizeof(plan)) != 0 ||
        byob_launch_plan_v2_from_environment(&restored, L"2,2,c,0x") ||
        byob_launch_plan_v2_from_arguments(&plan, &selection, -1, arguments)) return 1;
    puts("byob-launch-plan-v2-test: immutable two-slot handoff verified");
    return 0;
}
