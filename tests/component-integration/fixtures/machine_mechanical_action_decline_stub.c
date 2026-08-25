#include "adapter-softpc/mechanical_action.h"

/* The focused readonly-file fixture invokes only direct adapter results.
 * The package source closure also contains unrelated paths which name the
 * machine ABI; this deliberately declines those paths instead of emulating
 * memory or Bochs behaviour. */
int runtime_machine_execute_mechanical_action(
    struct runtime_mechanical_action *action)
{
    (void)action;
    return 0;
}
