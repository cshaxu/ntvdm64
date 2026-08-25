#ifndef RUNTIME_MACHINE_BINDING_H
#define RUNTIME_MACHINE_BINDING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*runtime_machine_binding_a20_get_v1)(uint32_t *enabled);
typedef int (*runtime_machine_binding_a20_set_v1)(uint32_t enabled);

/* App owns composition.  The SoftPC-facing layer receives only primitive,
 * copied mechanical operations and never a Bochs object, pointer or header. */
int runtime_machine_binding_v1_bind_a20(
    runtime_machine_binding_a20_get_v1 get_a20,
    runtime_machine_binding_a20_set_v1 set_a20);
void runtime_machine_binding_v1_unbind_a20(void);
int runtime_machine_binding_v1_get_a20(uint32_t *enabled);
int runtime_machine_binding_v1_set_a20(uint32_t enabled);

#ifdef __cplusplus
}
#endif

#endif
