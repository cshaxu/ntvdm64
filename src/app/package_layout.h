#ifndef APP_PACKAGE_LAYOUT_H
#define APP_PACKAGE_LAYOUT_H

#include "session/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configure immutable SoftPC firmware below the installed executable's
 * directory.  The caller must do this before session activation. */
int app_package_layout_set_firmware_root(session *owner,
    const char *executable_path);

/* Resolve the current executable through the public process API, then apply
 * the same installation layout. */
int app_package_layout_set_process_firmware_root(session *owner);

#ifdef __cplusplus
}
#endif

#endif
