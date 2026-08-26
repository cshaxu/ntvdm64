#ifndef RUNTIME_HEADLESS_8042_H
#define RUNTIME_HEADLESS_8042_H

#include "bochs.h"

/* Selector-blind lifetime of one native 8042 controller.  The interface has
 * no guest-service, BOP, OpenNT, DOS, host-input or device-policy terms. */
bx_bool bx_machine_headless_8042_create(void);
bx_bool bx_machine_headless_8042_destroy(void);

#endif
