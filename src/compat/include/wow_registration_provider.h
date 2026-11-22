#ifndef WOW_REGISTRATION_PROVIDER_H
#define WOW_REGISTRATION_PROVIDER_H
#include <windows.h>
#include "wow_user_task_lifecycle.h"
struct tagAPFNWOWHANDLERSIN;
struct tagAPFNWOWHANDLERSOUT;

typedef struct wow_registration_provider {
    VOID (WINAPI *free_dde_data)(HANDLE, BOOL, BOOL);
    LONG (WINAPI *class_words)(HINSTANCE, LPCTSTR);
    void (WINAPI *exchange)(struct tagAPFNWOWHANDLERSIN *, struct tagAPFNWOWHANDLERSOUT *);
    const struct tagAPFNWOWHANDLERSIN *(WINAPI *callbacks)(void);
    void (WINAPI *set_callbacks)(struct tagAPFNWOWHANDLERSIN *);
    BOOL (WINAPI *edit_callbacks_ready)(void);
    void (WINAPI *capture_lifecycle)(wow_user_task_lifecycle_callbacks *);
    wow_window_callback (WINAPI *window_callback)(void);
} wow_registration_provider;

const wow_registration_provider *WINAPI wow_registration_provider_current(void);
void WINAPI wow_original_registration_exchange(struct tagAPFNWOWHANDLERSIN *, struct tagAPFNWOWHANDLERSOUT *);
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_original_registered_callbacks(void);
void WINAPI wow_original_set_registered_callbacks(struct tagAPFNWOWHANDLERSIN *);
BOOL WINAPI wow_original_edit_callbacks_ready(void);
void WINAPI wow_original_registration_lifecycle(wow_user_task_lifecycle_callbacks *);
wow_window_callback WINAPI wow_original_registration_window_callback(void);
BOOL WINAPI wow_registration_bind(const wow_registration_provider *);
#endif
