#ifndef BROKER_VDM_STARTUP_H
#define BROKER_VDM_STARTUP_H
#include <stdint.h>
/* Only numeric fields captured by original BaseCheckVDM. The command envelope
 * owns version/length; strings and resources use separate admitted bindings. */
typedef struct broker_vdm_startup {
    uint32_t present;
    uint32_t x, y, x_size, y_size, x_chars, y_chars, fill, flags, show;
} broker_vdm_startup;
#endif
