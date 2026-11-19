#ifndef COMPAT_COMMAND_SERVICES_H
#define COMPAT_COMMAND_SERVICES_H

#include <stdint.h>
#include <windows.h>

enum {
    MVDM_COMMAND_SERVICE_NONE = 0,
    MVDM_COMMAND_SERVICE_INTERACTIVE = 1,
    MVDM_COMMAND_SERVICE_EXECUTE = 2
};

/* Installed once before COMMAND runs. The product owns capture, borrowed
 * strings, release and native launch policy. This boundary owns no request,
 * session, guest mapping or standard-handle state. All signatures preserve
 * the existing COMMAND binding; strings remain borrowed until finish. */
typedef struct compat_command_services {
    int (*capture_guest)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    int (*capture_host_command)(const char *, uint16_t, uint16_t, uint16_t, uint16_t);
    int (*std_handles)(ULONG [3]);
    const char *(*command)(void);
    const char *(*environment)(void);
    int (*replace_command)(const char *);
    int (*select_service)(unsigned int);
    void (*finish)(void);
    BOOL (*set_std_handle)(DWORD, HANDLE);
    BOOL (*create_process)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR,
        LPSTARTUPINFOA, LPPROCESS_INFORMATION);
    void (*clear_doswow)(void);
    int (*register_doswow)(uint16_t, uint16_t);
} compat_command_services;

BOOL compat_command_install(const compat_command_services *services);
const compat_command_services *compat_command_current_services(void);

#endif
