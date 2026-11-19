#ifndef MVDM_SOFTPC_FIRMWARE_H
#define MVDM_SOFTPC_FIRMWARE_H

#include <stdint.h>
#include "compat/include/system_root.h"
#include "compat/include/pif_path.h"
#include "compat/include/mvdm_firmware.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Finished ANSI guest boot block only. Success replaces CRT-owned storage;
 * failure preserves input. Never changes the worker's native environment. */
int mvdm_softpc_project_wow_initial_environment(char **environment,
    unsigned long *bytes); /* Original Win32 DWORD cchEnv carrier. */

/* Same-shaped media-location binding for original MVDM system files.
 * It returns a caller-owned path only when the selected image exists. */
int __cdecl mvdm_softpc_system_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes);

/* Modern profile APIs redirect every file named SYSTEM.INI through the host
 * mapping layer, even for an absolute package pathname.  Copy the selected
 * immutable system profile to a worker-local temporary name and return that
 * caller-owned path.  The session teardown deletes the copy; neither guest
 * media nor the host profile is written. */
int __cdecl mvdm_softpc_profile_shadow_system_ini(char *path_out,
    uint32_t path_out_bytes);

/* Copy the configured immutable MVDM system directory into the original
 * caller-owned buffer.  This is the directory-shaped counterpart of the
 * existing file lookup; it neither opens media nor changes host state. */
int __cdecl mvdm_softpc_system_copy_root(char *path_out,
    uint32_t path_out_bytes);

/* Copy the existing `system32` child of the selected MVDM system root into
 * an original caller-owned buffer.  This is the directory counterpart of the
 * established root/file adapters; it neither probes the host system directory
 * nor creates package media. */
int __cdecl mvdm_softpc_system_copy_system_directory(char *path_out,
    uint32_t path_out_bytes);

#ifdef __cplusplus
}
#endif

#endif
