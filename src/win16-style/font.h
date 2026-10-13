#ifndef WIN16_STYLE_FONT_H
#define WIN16_STYLE_FONT_H
#include <windows.h>
/* Same-GUI-thread references to a process-private, 24-pixel System bitmap
 * font. No guest font, disk asset or global font installation. Release only
 * after every borrowing HWND/DC has restored its previous font. */
HFONT win16_style_font_acquire(void);
void win16_style_font_release(HFONT font);
#endif
