#ifndef WOW_TASK_LIFECYCLE_FIXTURE_PROVIDER_H
#define WOW_TASK_LIFECYCLE_FIXTURE_PROVIDER_H
#include "compat/include/wow_task_lifecycle_provider.h"
#include "wow_window_access_fixture_provider.h"

/* The same original functions bound by the product DLL entry. */
static const wow_task_lifecycle_provider wow_fixture_task_lifecycle_provider = {
    xxxSleepTask, xxxDirectedYield, xxxInitTask, xxxRegisterUserHungAppHandlers,
    xxxUserYield, DestroyTask
};
#endif
