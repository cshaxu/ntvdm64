#include "include/wow_clipboard_epoch.h"

/* Original wuclip.c owner, using the selected WOW32 stdcall ABI. */
void WINAPI W32EmptyClipboard(void);

/* OpenNT xxxOpenClipboard updates the process serial before pfnEmptyCB and
 * preserves handles owned by another Win16 task in the same process. */
static volatile LONG wow_clipboard_epoch;

void WINAPI wow_clipboard_opened(void)
{
    DWORD sequence = GetClipboardSequenceNumber();
    DWORD owner = 0;
    if ((DWORD)InterlockedExchange(&wow_clipboard_epoch, (LONG)sequence) == sequence)
        return;
    GetWindowThreadProcessId(GetClipboardOwner(), &owner);
    if (owner != GetCurrentProcessId())
        W32EmptyClipboard();
}
