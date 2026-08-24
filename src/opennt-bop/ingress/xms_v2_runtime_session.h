#ifndef RUNTIME_BOP_XMS_V2_RUNTIME_SESSION_H
#define RUNTIME_BOP_XMS_V2_RUNTIME_SESSION_H

/* This session binds the original XMS package only after the mantle has
 * admitted the matching whole-machine capacity: OpenNT config.c uses
 * XMS + EMS + conventional memory.  It never invents an extended aperture;
 * a later mantle-memory package supplies the declared XMS capacity here. */

#include <stdint.h>

int runtime_xms_v2_runtime_session_bind(uint32_t xms_memory_kib);
void runtime_xms_v2_runtime_session_reset(void);

#endif
