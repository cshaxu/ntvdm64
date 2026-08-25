#ifndef APP_DOS_SAFE_ALIAS_H
#define APP_DOS_SAFE_ALIAS_H

#include <windows.h>
#include <stdint.h>

/* A CLI-session admission record.  It never owns the target directory: when
 * an alias is needed it owns only the directory symbolic link it created. */
struct app_dos_safe_alias {
    wchar_t admitted_root[MAX_PATH];
    uint32_t owns_alias;
};

void app_dos_safe_alias_clear(struct app_dos_safe_alias *value);

/* Admit `root` for a guest pathname with at most `maximum_oem_chars` bytes.
 * The result is either an existing DOS-representable spelling of root, or a
 * session-owned directory link below a DOS-representable public temp path.
 * No target file is copied, altered, or deleted. */
int app_dos_safe_alias_admit(const wchar_t *root,
    uint32_t maximum_oem_chars, struct app_dos_safe_alias *value);

/* Remove only the session-owned directory link, if any. */
void app_dos_safe_alias_release(struct app_dos_safe_alias *value);

#endif
