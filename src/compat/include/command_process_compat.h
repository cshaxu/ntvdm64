#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_COMMAND_PROCESS_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_COMMAND_PROCESS_COMPAT_H

#include <windows.h>

/*
 * DIVERGENCE(ADAPTER-WIN32-037): COMMAND's original child path temporarily
 * changes ntvdm.exe-wide standard handles before CreateProcess.  The app owns
 * its own standard streams, so retain the original SetStdHandle/CreateProcess
 * call sequence while making its endpoints child-local STARTUPINFO state.
 */
#include "command_services.h"

#undef SetStdHandle
#define SetStdHandle compat_command_current_services()->set_std_handle
#undef CreateProcess
#define CreateProcess compat_command_current_services()->create_process

#endif
