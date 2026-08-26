#include "idle_shim.h"

void HostIdleNoActivity(void)
{
    /* The historical timer/idle worker is not an admitted product component.
     * The source-facing void interface is retained without fabricating one. */
}
