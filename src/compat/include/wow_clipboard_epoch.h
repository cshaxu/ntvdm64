#ifndef WOW_CLIPBOARD_EPOCH_H
#define WOW_CLIPBOARD_EPOCH_H

#include <windows.h>

/* Call only after successful OpenClipboard, while its lock is held.
 * Preserve original USER serial/owner checks and W32EmptyClipboard ownership. */
typedef void (WINAPI *wow_clipboard_retire)(void);
/* The caller lends its original retirement callback for this call only. */
void WINAPI wow_clipboard_opened(wow_clipboard_retire retire);

#endif
