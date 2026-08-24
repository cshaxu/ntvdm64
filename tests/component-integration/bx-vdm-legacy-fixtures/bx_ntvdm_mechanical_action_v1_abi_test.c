#include <stddef.h>
#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"

int main(void)
{
    struct bx_ntvdm_mechanical_action_v1 action;
    if (sizeof(struct bx_ntvdm_mechanical_action_v1_range) != 16u ||
        offsetof(struct bx_ntvdm_mechanical_action_v1, ranges) != 32u ||
        offsetof(struct bx_ntvdm_mechanical_action_v1, payload) != 1056u ||
        sizeof(action) != 66592u) return 1;
    return (BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES == 64u &&
        BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES == 65535u) ? 0 : 2;
}
