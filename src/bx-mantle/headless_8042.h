#ifndef BX_NTVDM_HEADLESS_8042_H
#define BX_NTVDM_HEADLESS_8042_H

#include "bochs.h"

/* Selector-blind lifetime of one native 8042 controller.  The interface has
 * no guest-service, BOP, OpenNT, DOS, host-input or device-policy terms. */
bx_bool bx_mantle_headless_8042_create_v1(void);
bx_bool bx_mantle_headless_8042_destroy_v1(void);

#endif
