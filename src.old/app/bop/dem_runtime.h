#ifndef RUNTIME_BOP_DEM_RUNTIME_SESSION_H
#define RUNTIME_BOP_DEM_RUNTIME_SESSION_H

/* Process-local Direct DEM session lifecycle.  It belongs to bx-vdm: callers
 * receive no DEM selector, guest pointer or Bochs object.  The checked-RAM
 * callbacks remain invoked only after the machine has an active machine stage. */

int runtime_dem_runtime_session_bind(void);
void runtime_dem_runtime_session_reset(void);

#endif
