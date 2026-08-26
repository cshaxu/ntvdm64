#ifndef APP_MACHINE_SHELL_H
#define APP_MACHINE_SHELL_H

#include <stdint.h>

#include "session/session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_MACHINE_SHELL_VERSION UINT32_C(1)

enum app_machine_shell_status {
    APP_MACHINE_SHELL_OK = 0,
    APP_MACHINE_SHELL_REJECTED_INPUT,
    APP_MACHINE_SHELL_REJECTED_STATE,
    APP_MACHINE_SHELL_MACHINE_FAILURE
};

/* Application assembly record.  It carries no Bochs object, guest locator,
 * service selector, host handle, or callback across its public surface. */
typedef struct app_machine_shell {
    uint32_t abi_version;
    uint32_t struct_bytes;
    session *owner;
    uint32_t active;
    uint32_t reserved0;
} app_machine_shell;

void app_machine_shell_initialize(app_machine_shell *shell);
int app_machine_shell_valid(const app_machine_shell *shell);
enum app_machine_shell_status app_machine_shell_open(app_machine_shell *shell,
    session *owner, uint32_t ips, uint64_t machine_memory_bytes);
enum app_machine_shell_status app_machine_shell_close(app_machine_shell *shell);
int app_machine_shell_active(const app_machine_shell *shell);

#ifdef __cplusplus
}
#endif

#endif
