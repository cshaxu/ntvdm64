#ifndef WOW_CLIPBOARD_EPOCH_H
#define WOW_CLIPBOARD_EPOCH_H

#include <windows.h>

/* Call only after successful OpenClipboard, while its lock is held.
 * Preserve original USER serial/owner checks and W32EmptyClipboard ownership. */
void WINAPI wow_clipboard_opened(void);

#endif
