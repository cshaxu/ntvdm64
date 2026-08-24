#ifndef RUNTIME_INITIAL_STATE_CATALOG_V1_H
#define RUNTIME_INITIAL_STATE_CATALOG_V1_H

#include <stdint.h>
#include "initial_state_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

int runtime_initial_state_catalog_v1_select(const uint16_t *digest,
    uint32_t chars, runtime_initial_state_v1 *out);

#ifdef __cplusplus
}
#endif
#endif
