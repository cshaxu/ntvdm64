#ifndef MVDM_XMS_MEMORY_H
#define MVDM_XMS_MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Preserve the existing controlled-stop direction if the one unavailable
 * guest alias operation cannot acquire its bounded synchronous lease. */
void mvdm_xms_cancel_current_operation(void);

#ifdef __cplusplus
}
#endif

#endif
