#include "wow_clipboard_epoch.h"

/* Zero means that the original cache has no host generation attached. */
static volatile LONG wow_clipboard_epoch;

void WINAPI wow_clipboard_epoch_note_current(void)
{
    InterlockedExchange(&wow_clipboard_epoch,
        (LONG)GetClipboardSequenceNumber());
}

BOOL WINAPI wow_clipboard_epoch_matches_current(void)
{
    LONG known = InterlockedCompareExchange(&wow_clipboard_epoch, 0, 0);
    return known == 0 || (DWORD)known == GetClipboardSequenceNumber();
}

void WINAPI wow_clipboard_epoch_reset(void)
{
    InterlockedExchange(&wow_clipboard_epoch, 0);
}
