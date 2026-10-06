#ifndef NTCON_SESSION_ARGUMENTS_H
#define NTCON_SESSION_ARGUMENTS_H
#include <windows.h>
#include <stdint.h>
/* Numeric bootstrap values identify inherited resources, not authorization.
 * NTSRV still authenticates the registering process and actual resources. */
BOOL frontend_session_resource(const WCHAR *text,UINT_PTR *value);
BOOL frontend_session_window(const WCHAR *text,uint64_t *value);
#endif
