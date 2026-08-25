#ifndef MACHINE_FACADE_H
#define MACHINE_FACADE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int machine_facade_v1_get_a20(uint32_t *enabled);
int machine_facade_v1_set_a20(uint32_t enabled);

#ifdef __cplusplus
}
#endif

#endif
