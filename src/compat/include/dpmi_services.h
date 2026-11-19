#ifndef COMPAT_DPMI_SERVICES_H
#define COMPAT_DPMI_SERVICES_H
#include <windows.h>

/* Immutable process binding. Thread contexts, page tables and their lifetime
 * remain with the product; this contract stores no guest or session state. */
typedef struct compat_dpmi_services {
    BOOL (*set_debug_context)(PULONG);
    BOOL (*read_debug_registers)(USHORT, USHORT, PULONG);
    void (*leave_protected)(void);
    int (*reenter_protected)(void);
} compat_dpmi_services;
BOOL compat_dpmi_install(const compat_dpmi_services *services);
const compat_dpmi_services *compat_dpmi_current_services(void);
#endif
