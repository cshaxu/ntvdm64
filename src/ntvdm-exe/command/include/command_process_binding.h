#ifndef NTVDM_COMMAND_PROCESS_BINDING_H
#define NTVDM_COMMAND_PROCESS_BINDING_H

#include <windows.h>

/*
 * DIVERGENCE(ADAPTER-WIN32-037): COMMAND's original child path temporarily
 * changes ntvdm.exe-wide standard handles before CreateProcess.  The app owns
 * its own standard streams, so retain the original SetStdHandle/CreateProcess
 * call sequence while making its endpoints child-local STARTUPINFO state.
 */
#include "compat/include/command_services.h"
#include "command_launch.h"

#undef SetStdHandle
#define SetStdHandle opennt_command_set_std_handle
#undef CreateProcess
#define CreateProcess opennt_command_create_process_a

#endif
