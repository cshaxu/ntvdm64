#ifndef WOW_CONTROL_STATE_PROVIDER_H
#define WOW_CONTROL_STATE_PROVIDER_H
#include <windows.h>
struct tagWND;
typedef void (WINAPI *wow_control_state_provider)(struct tagWND *, UINT);
typedef struct wow_control_state_providers {
    wow_control_state_provider set, clear;
} wow_control_state_providers;
BOOL WINAPI wow_control_state_bind(const wow_control_state_providers *);
void WINAPI wow_control_set_window_state(struct tagWND *, UINT);
void WINAPI wow_control_clear_window_state(struct tagWND *, UINT);
#endif
