#ifndef LIB_WIN32_IO_PIPE_PAIR_H
#define LIB_WIN32_IO_PIPE_PAIR_H
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Create a local, byte-mode overlapped duplex pair under the supplied name.
 * event is borrowed. On success both handles are caller-owned. On failure
 * both outputs are INVALID_HANDLE_VALUE and pending connection I/O is drained. */
DWORD win32_pipe_pair_create(PCWSTR name,HANDLE event,HANDLE *server,HANDLE *client);
#ifdef __cplusplus
}
#endif
#endif
