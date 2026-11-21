#ifndef NTVDM_BASE_TRANSPORT_H
#define NTVDM_BASE_TRANSPORT_H

#include <windows.h>
struct _BASE_CHECKVDM_MSG;
struct _BASE_GET_NEXT_VDM_COMMAND_MSG;
struct _BASE_UPDATE_VDM_ENTRY_MSG;
/* Scalar projection and borrowed output fields of one synchronous request.
 * No native field pointer may outlive NtvdmBaseClientCallServer. */
typedef enum ntvdm_base_operation {
    ntvdm_base_unknown, ntvdm_base_check, ntvdm_base_get, ntvdm_base_update,
    ntvdm_base_exit, ntvdm_base_code, ntvdm_base_reenter,
    ntvdm_base_register, ntvdm_base_first
} ntvdm_base_operation;
typedef struct ntvdm_base_request {
    ntvdm_base_operation operation;
    void *payload;
    LONG *status;
    DWORD error, requested, value;
    HANDLE parent;
    HANDLE *wait;
    ULONG *exit_code;
    BOOL *first;
    BOOL wow, notify_started;
} ntvdm_base_request;
typedef struct ntvdm_base_command_state { BOOL observe, activate; } ntvdm_base_command_state;

/* The owner binds this immutable table only for
 * an authenticated execution client and joins callers before unbinding. */
typedef struct ntvdm_base_transport {
    LONG (*status)(DWORD error);
    LONG (*check)(struct _BASE_CHECKVDM_MSG *);
    LONG (*get)(struct _BASE_GET_NEXT_VDM_COMMAND_MSG *, HANDLE *wait, ULONGLONG *direct);
    LONG (*update)(struct _BASE_UPDATE_VDM_ENTRY_MSG *);
    LONG (*exit_vdm)(BOOL wow, BOOL notify_started, ULONG task, ULONG *release_wait);
    LONG (*exit_code)(HANDLE parent, ULONG *code);
    LONG (*reenter)(ULONG change);
    LONG (*register_wow)(ULONG window);
    LONG (*first)(ULONG *value);
    void (*decode)(void *message, ULONG number, ULONG length, ntvdm_base_request *);
    void (*command_state)(const ntvdm_base_request *, LONG status, ntvdm_base_command_state *);
} ntvdm_base_transport;

void ntvdm_base_bind_transport(const ntvdm_base_transport *transport);
struct compat_console_activation;
typedef DWORD (*ntvdm_base_activate)(struct compat_console_activation *, BOOL);
void ntvdm_base_set_activation(ntvdm_base_activate activate, struct compat_console_activation *context);
ULONGLONG ntvdm_base_observation_direct(void);
#endif
