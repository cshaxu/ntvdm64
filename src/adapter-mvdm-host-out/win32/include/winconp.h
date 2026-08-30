#ifndef OPENNT_SUPPORT_WINCONP_H
#define OPENNT_SUPPORT_WINCONP_H

/* DIVERGENCE(ADAPTER-WIN32-034): preserve all other original private-console
 * declarations verbatim, but prevent its DLL-import declaration from being
 * attached to the one ANSI layout query supplied by console_compat.c. */
#define GetConsoleKeyboardLayoutNameA opennt_private_GetConsoleKeyboardLayoutNameA
#include "../../../opennt-abi/source/public/internal/windows/inc/winconp.h"
#undef GetConsoleKeyboardLayoutNameA

BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR layout_name);
#endif
