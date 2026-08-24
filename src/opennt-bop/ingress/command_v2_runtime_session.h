#ifndef RUNTIME_BOP_COMMAND_V2_RUNTIME_SESSION_H
#define RUNTIME_BOP_COMMAND_V2_RUNTIME_SESSION_H

int runtime_command_v2_runtime_session_bind(void);
/* Native engine composition binds the same session after copying the already
 * admitted launch declaration from the startup composition. */
int runtime_command_v2_runtime_session_bind_from_startup(void);
void runtime_command_v2_runtime_session_reset(void);

#endif
