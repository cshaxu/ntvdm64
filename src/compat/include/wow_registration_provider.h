#ifndef WOW_REGISTRATION_PROVIDER_H
#define WOW_REGISTRATION_PROVIDER_H
#include <windows.h>

typedef struct wow_registration_provider {
    VOID (WINAPI *free_dde_data)(HANDLE, BOOL, BOOL);
    LONG (WINAPI *class_words)(HINSTANCE, LPCTSTR);
} wow_registration_provider;

BOOL WINAPI wow_registration_bind(const wow_registration_provider *);
#endif
