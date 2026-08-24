#ifndef NTDOS64_DOS_SAFE_ALIAS_V1_H
#define NTDOS64_DOS_SAFE_ALIAS_V1_H

#include <windows.h>
#include <stdint.h>

/* A CLI-session admission record.  It never owns the target directory: when
 * an alias is needed it owns only the directory symbolic link it created. */
struct ntdos64_dos_safe_alias_v1 {
    wchar_t admitted_root[MAX_PATH];
    uint32_t owns_alias;
};

void ntdos64_dos_safe_alias_v1_clear(struct ntdos64_dos_safe_alias_v1 *value);

/* Admit `root` for a guest pathname with at most `maximum_oem_chars` bytes.
 * The result is either an existing DOS-representable spelling of root, or a
 * session-owned directory link below a DOS-representable public temp path.
 * No target file is copied, altered, or deleted. */
int ntdos64_dos_safe_alias_v1_admit(const wchar_t *root,
    uint32_t maximum_oem_chars, struct ntdos64_dos_safe_alias_v1 *value);

/* Remove only the session-owned directory link, if any. */
void ntdos64_dos_safe_alias_v1_release(struct ntdos64_dos_safe_alias_v1 *value);

#endif
