#ifndef COMPAT_NATIVE_CONSOLE_DIAGNOSTICS_H
#define COMPAT_NATIVE_CONSOLE_DIAGNOSTICS_H
#include <windows.h>

void ntvwm_trace_error(const char *stage,DWORD operation,DWORD error);

#endif
