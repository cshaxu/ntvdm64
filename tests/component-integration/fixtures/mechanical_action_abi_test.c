#include <stddef.h>
#include "adapter-softpc/mechanical_action.h"

int main(void)
{
    struct runtime_mechanical_action action;
    if (sizeof(struct runtime_mechanical_action_range) != 16u ||
        offsetof(struct runtime_mechanical_action, ranges) != 32u ||
        offsetof(struct runtime_mechanical_action, payload) != 1056u ||
        sizeof(action) != 66592u) return 1;
    return (RUNTIME_MECHANICAL_ACTION_MAX_RANGES == 64u &&
        RUNTIME_MECHANICAL_ACTION_MAX_BYTES == 65535u) ? 0 : 2;
}
