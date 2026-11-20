#ifndef COMPAT_WOW_STARTUP_EXIT_H
#define COMPAT_WOW_STARTUP_EXIT_H

/* A successful WOWEXEC no-op in the already running shared worker.
 * The private ConsoleHandle selector exists only in the local CSR message.
 * The RPC client uses the ordinary WOW startup and Exit operations. */
#define OPENNT_WOW_STARTUP_NOOP_EXIT 2
#define OPENNT_WOW_STARTUP_NOOP_CONSOLE ((HANDLE)(LONG_PTR)-2)

#endif
