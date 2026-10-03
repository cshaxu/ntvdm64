#ifndef RUN16_IMAGE_CLASSIFICATION_H
#define RUN16_IMAGE_CLASSIFICATION_H
#include <windows.h>

/* Launcher classification before service admission, not a PE parser. Zero the
 * output on failure; successful output is WINDOWS_GUI or WINDOWS_CUI. */
DWORD run16_classify_native_image(PCWSTR application,DWORD *subsystem);
#endif
