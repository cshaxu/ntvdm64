#ifndef APP_XMS_SESSION_BINDING_H
#define APP_XMS_SESSION_BINDING_H

/* This session binds the original XMS package only after the machine has
 * admitted the matching whole-machine capacity: OpenNT config.c uses
 * XMS + EMS + conventional memory.  It never invents an extended aperture;
 * a later machine-memory package supplies the declared XMS capacity here. */

#include <stdint.h>

int app_xms_session_bind(uint32_t xms_memory_kib);
void app_xms_session_reset(void);

#endif
