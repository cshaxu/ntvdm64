#ifndef FRONTEND_SESSION_SERVICE_H
#define FRONTEND_SESSION_SERVICE_H
#include <windows.h>
typedef struct frontend_session_service frontend_session_service;
/* Called by the process already registered as the authenticated frontend.
 * Borrowed capability/notification remain valid until service_close joins. */
DWORD frontend_service_start(HANDLE,HANDLE,void (*)(void),frontend_session_service **);
/* Independent process: creator is a borrowed startup hold, never target
 * lifetime. It is ignored after the first admitted I/O request. */
DWORD frontend_service_start_process(HANDLE,HANDLE,HANDLE,frontend_session_service **);
void frontend_service_close(frontend_session_service *);
HANDLE frontend_service_thread(frontend_session_service *);
#endif
