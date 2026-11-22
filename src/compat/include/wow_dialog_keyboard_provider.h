#ifndef WOW_DIALOG_KEYBOARD_PROVIDER_H
#define WOW_DIALOG_KEYBOARD_PROVIDER_H
#include <windows.h>
typedef struct wow_dialog_keyboard_provider {
    BOOL (WINAPI *ansi)(HWND, LPMSG);
    BOOL (WINAPI *unicode)(HWND, LPMSG);
} wow_dialog_keyboard_provider;
BOOL WINAPI wow_dialog_keyboard_bind(const wow_dialog_keyboard_provider *);
#endif
