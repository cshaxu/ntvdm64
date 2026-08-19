#ifndef BX_NTVDM_COMMAND_INITIAL_ENVIRONMENT_V1_H
#define BX_NTVDM_COMMAND_INITIAL_ENVIRONMENT_V1_H

#include "bx_ntvdm_command_host_context_v1.h"

/* Source-derived initial COMMAND environment capture.  It owns only a copied
 * CLI-process snapshot; no host pointer or environment handle escapes. */
int bx_ntvdm_command_initial_environment_v1_capture(
    bx_ntvdm_command_host_context_v1 *context);

#endif
