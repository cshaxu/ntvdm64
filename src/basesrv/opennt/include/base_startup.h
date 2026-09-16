#ifndef OPENNT_BASE_STARTUP_H
#define OPENNT_BASE_STARTUP_H
#include <windows.h>
#include "basesrv/transport/vdm_startup.h"
/* Local caller supplies valid storage; no native value is serialized. */
void OpenNtBaseEncodeStartup(const STARTUPINFOA *, broker_vdm_startup *);
/* Invalid input leaves destination unchanged. On success, absent source
 * produces zeroed storage; caller keeps the original StartupInfo pointer NULL. */
BOOL OpenNtBaseDecodeStartup(const broker_vdm_startup *, STARTUPINFOA *);
#endif
