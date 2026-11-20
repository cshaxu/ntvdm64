#ifndef WOW_CONTROL_EXTENT_BINDINGS_H
#define WOW_CONTROL_EXTENT_BINDINGS_H
#include <windows.h>

/* Canonical original client-control registration size, including WND.
 * Not the native class's capacity or a superclass's extra-byte reservation.
 * Unknown/server FNIDs fail without changing the output. */
BOOL WINAPI wow_control_registered_extent(WORD, DWORD *);
#endif
