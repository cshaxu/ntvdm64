#ifndef WOW_CLIPBOARD_EPOCH_H
#define WOW_CLIPBOARD_EPOCH_H

#include <windows.h>

/* The original USER registration delivered pfnEmptyCB whenever native USER
 * discarded the WOW clipboard cache. Modern USER cannot accept that private
 * registration ABI. This binding keeps only the public clipboard sequence
 * attached to the existing original cache; it never owns clipboard data. */
void WINAPI wow_clipboard_epoch_note_current(void);
BOOL WINAPI wow_clipboard_epoch_matches_current(void);
void WINAPI wow_clipboard_epoch_reset(void);

#endif
