#ifndef RUNTIME_BOP_SHIM_SOFTPC_INT15_WATCH_SHIM_H
#define RUNTIME_BOP_SHIM_SOFTPC_INT15_WATCH_SHIM_H

#include <stdint.h>

/* This is a machine-facing watch seam, not an XMS provider.  It needs only
 * the original fixed-width word spelling; importing the XMS host-policy shim
 * here would reverse the adapter-softpc/opennt-host dependency. */
typedef uint16_t WORD;

typedef WORD word;

/* The adopted source compiled this diagnostic only outside its product build.
 * This direct fragment has no product console owner, so retain production
 * suppression rather than introducing a new host-output side effect. */
#ifndef PROD
#define PROD 1
#endif

/* The direct fragment retains the historical SAS load shape.  This adapter
 * declaration is deliberately narrower than a general SAS interface. */
void runtime_softpc_int15_sas_loadw(uint32_t address, word *value);
#define sas_loadw runtime_softpc_int15_sas_loadw
int runtime_softpc_int15_watch_state_load(word *offset, word *segment);
int runtime_softpc_int15_watch_state_store(word offset, word segment);

#endif
