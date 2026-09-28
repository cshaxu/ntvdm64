#ifndef NTKVM_NATIVE_TERMINAL_SCREEN_H
#define NTKVM_NATIVE_TERMINAL_SCREEN_H
#include <vterm.h>
/* Frontend-local, pinned libvterm screen seam; caller holds the terminal lock.
 * No VT is injected and no native parser/saved-state reset is performed. */
int ntkvm_vterm_replace_screen(VTermScreen *,int,int,const VTermScreenCell *,VTermPos);
#endif
