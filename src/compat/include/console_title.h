#ifndef OPENNT_CONSOLE_TITLE_H
#define OPENNT_CONSOLE_TITLE_H
#include "console_services.h"
/* Original selected ANSI title callers; frontend owns the native Console. */
#define GetConsoleTitleA MvdmGetConsoleTitleA
#define SetConsoleTitleA MvdmSetConsoleTitleA
#endif
