#ifndef BX_NTVDM_COMPOSITION_RUNTIME_V1_H
#define BX_NTVDM_COMPOSITION_RUNTIME_V1_H

#ifdef __cplusplus
extern "C" {
#endif

/* Installs the one process-local whole-package composition from the immutable
 * CLI launch environment.  Returns 1 when bound, 0 when no CLI environment
 * is present, and -1 for a declared-but-invalid composition. */
int bx_ntvdm_composition_runtime_v1_install_from_environment(void);
void bx_ntvdm_composition_runtime_v1_reset(void);

#ifdef __cplusplus
}
#endif

#endif
