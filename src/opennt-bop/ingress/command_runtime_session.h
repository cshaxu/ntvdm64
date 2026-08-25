#ifndef RUNTIME_BOP_COMMAND_RUNTIME_SESSION_H
#define RUNTIME_BOP_COMMAND_RUNTIME_SESSION_H

#include <stdint.h>

int runtime_command_runtime_session_bind(void);
/* Native engine composition binds the same session after copying the already
 * admitted launch declaration from the startup composition. */
int runtime_command_runtime_session_bind_from_startup(const char *application,
    const char *tail, uint16_t drive, uint16_t code_page,
    const char *bootstrap_command);
void runtime_command_runtime_session_reset(void);

#endif
