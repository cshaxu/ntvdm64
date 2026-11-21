#ifndef WOW_STANDARD_CLASS_PROVIDER_H
#define WOW_STANDARD_CLASS_PROVIDER_H
#include <windows.h>
/* Immutable image-lifetime provider; bind before original DLL attach.
 * Calls use the caller's existing USER class-domain lock. Names are borrowed. */
typedef struct wow_standard_class_provider {
    INT (WINAPI *classify)(LPSTR);
    LPCSTR (WINAPI *name)(UINT);
    ATOM (WINAPI *atom)(UINT);
} wow_standard_class_provider;
BOOL WINAPI wow_standard_class_bind(const wow_standard_class_provider *);
#endif
