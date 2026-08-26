#ifndef RUNTIME_INITIAL_STATE_CATALOG_H
#define RUNTIME_INITIAL_STATE_CATALOG_H

#include <stdint.h>
#include "initial_state_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

int runtime_initial_state_catalog_select(const uint16_t *digest,
    uint32_t chars, runtime_initial_state *out);

#ifdef __cplusplus
}
#endif
#endif
