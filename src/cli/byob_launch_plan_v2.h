#ifndef BYOB_LAUNCH_PLAN_V2_H
#define BYOB_LAUNCH_PLAN_V2_H

#include <stdint.h>
#include <wchar.h>

#include "byob_launch_declaration_v1.h"
#include "byob_profile.h"

#define BYOB_LAUNCH_PLAN_V2_ENV_CHARS 256u

/* Fixed composition input for the two declared profile slots.  It carries no
 * host pathname or queue operation: slot 1 is always the profile's QUIT.COM. */
typedef struct byob_launch_plan_v2 {
    uint32_t version;
    uint32_t slot_count;
    byob_launch_declaration_v1 first;
} byob_launch_plan_v2;

int byob_launch_plan_v2_from_arguments(byob_launch_plan_v2 *out_value,
    const byob_profile_selection *selection, int argument_count,
    wchar_t *const *arguments);
int byob_launch_plan_v2_to_environment(const byob_launch_plan_v2 *value,
    wchar_t output[BYOB_LAUNCH_PLAN_V2_ENV_CHARS]);
int byob_launch_plan_v2_from_environment(byob_launch_plan_v2 *out_value,
    const wchar_t *input);

#endif
