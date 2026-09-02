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

/* Configure the immutable MVDM system root and its SoftPC firmware child:
 * the executable directory itself and its `softpc` child. */
int app_package_layout_set_media_roots(session *owner,
    const char *executable_path);

/* Product admission for the unchanged original COMMAND configuration form.
 * The selected MVDM root must exist, have a 1..63-byte short-path spelling,
 * and produce the original cmdconf.c SHELL value within NTDOS's 64-byte
 * `commnd` buffer (63 visible bytes plus NUL).  It must also leave room for
 * each original default system-media filename in the caller's MAX_PATH+12
 * buffer.  This does not test media existence: a PIF may replace
 * CONFIG.NT/AUTOEXEC.NT, and original MVDM owns missing-file errors. */
int app_package_layout_validate_command_configuration_root(
    const session *owner);

/* Resolve the current executable through the public process API, then apply
 * the same installation layout. */
int app_package_layout_set_process_firmware_root(session *owner);
int app_package_layout_set_process_media_roots(session *owner);

#ifdef __cplusplus
}
#endif

#endif
