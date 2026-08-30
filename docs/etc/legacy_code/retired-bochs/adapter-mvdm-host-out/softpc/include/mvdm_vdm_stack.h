#ifndef MVDM_VDM_STACK_H
#define MVDM_VDM_STACK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same-shaped non-fast WOW32 monitor forms from wow32.h.  A VPVOID remains a
 * numeric selector:offset value; no host pointer is reconstructed. */
int mvdm_vdm_stack_copy(uint32_t *vp_stack_out);
int mvdm_vdm_stack_set(uint32_t vp_stack);

/* Exact historical softpc.h spelling reached by non-fast CallBack16. It
 * resumes only an already-composed finite machine run; it has no provider,
 * selector or callback meaning. */
void host_simulate(void);

#ifdef __cplusplus
}
#endif

#endif
