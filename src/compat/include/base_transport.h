#ifndef NTVDM_BASE_TRANSPORT_H
#define NTVDM_BASE_TRANSPORT_H

/* Include after base_client.h. The owner binds this immutable table only for
 * an authenticated execution client and joins callers before unbinding. */
typedef struct ntvdm_base_transport {
    NTSTATUS (*status)(DWORD error);
    NTSTATUS (*check)(PBASE_CHECKVDM_MSG);
    NTSTATUS (*get)(PBASE_GET_NEXT_VDM_COMMAND_MSG, HANDLE *wait, ULONGLONG *direct);
    NTSTATUS (*update)(PBASE_UPDATE_VDM_ENTRY_MSG);
    NTSTATUS (*exit_vdm)(BOOL wow, BOOL notify_started, ULONG task, ULONG *release_wait);
    NTSTATUS (*exit_code)(HANDLE parent, ULONG *code);
    NTSTATUS (*reenter)(ULONG change);
    NTSTATUS (*register_wow)(ULONG window);
    NTSTATUS (*first)(ULONG *value);
} ntvdm_base_transport;

void ntvdm_base_bind_transport(const ntvdm_base_transport *transport);
struct compat_console_activation;
typedef DWORD (*ntvdm_base_activate)(struct compat_console_activation *, BOOL);
void ntvdm_base_set_activation(ntvdm_base_activate activate, struct compat_console_activation *context);
ULONGLONG ntvdm_base_observation_direct(void);
#endif
