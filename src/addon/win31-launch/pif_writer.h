#ifndef WIN31_LAUNCH_PIF_WRITER_H
#define WIN31_LAUNCH_PIF_WRITER_H

#include <windows.h>

/* Creates a self-contained NT PIF with exactly the extension records that
 * run16 needs.  These profiles are launched synchronously by the generated
 * command files, so CloseOnExit is part of their explicit launch contract.
 * All paths are fully-qualified installed-tree paths. */
BOOL win31_write_pif(const wchar_t *path, const wchar_t *title,
                     const wchar_t *program, const wchar_t *directory,
                     const wchar_t *arguments, const wchar_t *config,
                     const wchar_t *autoexec, BOOL close_on_exit);

#endif
