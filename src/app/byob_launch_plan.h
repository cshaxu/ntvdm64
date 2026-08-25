#ifndef BYOB_LAUNCH_PLAN_H
#define BYOB_LAUNCH_PLAN_H

#include <stdint.h>
#include <wchar.h>

#include "byob_launch_declaration.h"
#include "byob_profile.h"

#define BYOB_LAUNCH_PLAN_ENV_CHARS 256u

/* Fixed composition input for one declared target, optionally followed by a
 * profile-declared QUIT.COM terminal slot. It carries no host pathname or
 * queue operation. */
typedef struct byob_launch_plan {
    uint32_t version;
    uint32_t slot_count;
    byob_launch_declaration first;
} byob_launch_plan;

int byob_launch_plan_from_arguments(byob_launch_plan *out_value,
    const byob_profile_selection *selection, int argument_count,
    wchar_t *const *arguments);
int byob_launch_plan_to_environment(const byob_launch_plan *value,
    wchar_t output[BYOB_LAUNCH_PLAN_ENV_CHARS]);
int byob_launch_plan_from_environment(byob_launch_plan *out_value,
    const wchar_t *input);

#endif
