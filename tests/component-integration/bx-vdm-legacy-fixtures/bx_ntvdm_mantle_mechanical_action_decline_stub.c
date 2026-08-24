#include "adapter-softpc/mechanical_action.h"

/* The focused readonly-file fixture invokes only direct adapter results.
 * The package source closure also contains unrelated paths which name the
 * mantle ABI; this deliberately declines those paths instead of emulating
 * memory or Bochs behaviour. */
int runtime_mantle_execute_mechanical_action_v1(
    struct runtime_mechanical_action_v1 *action)
{
    (void)action;
    return 0;
}
