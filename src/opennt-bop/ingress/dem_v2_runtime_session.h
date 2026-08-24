#ifndef RUNTIME_BOP_DEM_V2_RUNTIME_SESSION_H
#define RUNTIME_BOP_DEM_V2_RUNTIME_SESSION_H

/* Process-local Direct DEM session lifecycle.  It belongs to bx-vdm: callers
 * receive no DEM selector, guest pointer or Bochs object.  The checked-RAM
 * callbacks remain invoked only after the mantle has an active machine stage. */

int runtime_dem_v2_runtime_session_bind(void);
void runtime_dem_v2_runtime_session_reset(void);

#endif
