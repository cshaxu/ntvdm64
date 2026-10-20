#ifndef RUN16_IMAGE_CLASSIFICATION_H
#define RUN16_IMAGE_CLASSIFICATION_H
#include <windows.h>
#include "common/protocol/native_launch_type.h"

/* Launcher classification before service admission, not a PE parser. Zero the
 * output on failure; successful output is WINDOWS_GUI or WINDOWS_CUI. */
DWORD run16_classify_native_image(PCWSTR application,DWORD *subsystem);
/* Complete native identity used for admission; this retains both machine and
 * subsystem from the existing SEC_IMAGE query instead of collapsing them. */
DWORD run16_classify_native_launch_type(PCWSTR application,DWORD *native_type);
#endif
