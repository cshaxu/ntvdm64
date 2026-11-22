#ifndef WOW_EDIT_LOOKASIDE_PROVIDER_H
#define WOW_EDIT_LOOKASIDE_PROVIDER_H
#include <windows.h>
BOOL WINAPI wow_edit_lookaside_bind(CRITICAL_SECTION *);
BOOL WINAPI wow_edit_lookaside_ready(void);
#endif
