#ifndef FRONTEND_SESSION_SERVICE_H
#define FRONTEND_SESSION_SERVICE_H
#include <windows.h>
typedef struct frontend_session_service frontend_session_service;
/* Called by the process already registered as the authenticated frontend.
 * Borrowed notification remains valid until service_close joins. */
DWORD frontend_service_start(HANDLE,void (*)(void),frontend_session_service **);
/* Independent process: creator is a borrowed failure/early-start hold;
 * retire is the authenticated NTSRV Console-return signal. The service
 * reports restoration through NTSRV, never directly to the launcher. */
DWORD frontend_service_start_process(HANDLE,HANDLE,HANDLE,BOOL,frontend_session_service **);
/* Returns only after the native Console has been restored.  A failure leaves
 * the root caller unacknowledged rather than returning it to a half-restored
 * Console. */
DWORD frontend_service_close(frontend_session_service *);
HANDLE frontend_service_thread(frontend_session_service *);
#endif
