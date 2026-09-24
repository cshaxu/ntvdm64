#ifndef MVDM_SOFTPC_FIRMWARE_H
#define MVDM_SOFTPC_FIRMWARE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same-shaped host resource lookup binding used by the original
 * host_find_file() caller.  It owns no firmware bytes and returns only a
 * caller-provided, synchronous path buffer. */
int mvdm_softpc_firmware_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes);

/* Copy one selected immutable firmware image from the product executable into
 * an original caller-owned buffer.  It is deliberately not a file-search
 * fallback: only the three fixed firmware identities are accepted. */
long mvdm_softpc_firmware_read_embedded_rom(const char *name, void *bytes_out,
    uint32_t bytes_out_capacity);

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

/* Directory-shaped standalone equivalents of the original host-directory
 * queries.  They expose the selected worker package, not the host Windows
 * installation.  Return values follow GetWindowsDirectoryA/W and
 * GetSystemDirectoryA/W: zero is failure, a too-small caller buffer receives
 * the required character count including NUL, otherwise the copied length
 * excludes NUL. */
uint32_t __cdecl GetNtvdmWindowsDirectoryA(char *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl GetNtvdmSystemDirectoryA(char *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl GetNtvdmWindowsDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars);
uint32_t __cdecl GetNtvdmSystemDirectoryW(wchar_t *path_out,
    uint32_t path_out_chars);

/* In-place, bounded OEM-to-ANSI boundary for original PIF configuration
 * file consumers. Returns zero if conversion cannot fit; opens no file. */
int mvdm_softpc_config_path_to_ansi(char *path, uint32_t capacity);

/* Selected-product disposition for the historical first-session compatibility
 * probe.  The original AddSystemFiles() body creates C:\\MSDOS.SYS and
 * C:\\IO.SYS only to satisfy a legacy installer.  The current product keeps
 * ordinary admitted host-drive access, but never creates these fake boot
 * markers on the real host volume. */
void mvdm_softpc_prepare_system_file_compatibility(void);

#ifdef __cplusplus
}
#endif

#endif
