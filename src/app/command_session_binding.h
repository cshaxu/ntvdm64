#ifndef APP_COMMAND_SESSION_BINDING_H
#define APP_COMMAND_SESSION_BINDING_H

#include <stdint.h>

int app_command_session_bind(void);
/* Native engine composition binds the same session after copying the already
 * admitted launch declaration from the startup composition. */
int app_command_session_bind_inputs(const char *application,
    const char *tail, uint16_t drive, uint16_t code_page,
    const char *bootstrap_command);
void app_command_session_reset(void);

/* App-only final composition: copy the admitted launch declaration and bind
 * it to the recovered COMMAND provider session.  It does not decode a BOP or
 * alter guest state. */
int app_command_session_bind_from_startup(void);

#endif
