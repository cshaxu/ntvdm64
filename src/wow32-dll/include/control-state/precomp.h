#ifndef WOW_CONTROL_STATE_PRECOMP_H
#define WOW_CONTROL_STATE_PRECOMP_H

/* OPENNT-HOST-077/083: compile environment for the retained CopyInflateRect
 * and SetDialogPointer bodies. WND/DIALOG declarations remain in their mirror owner;
 * no native USER heap overlay, alternate layout or state transition here.
 * Select only for these finite subsets, not the general USER source tree. */
#include "wow_dialog_state_bindings.h"

BOOL APIENTRY CopyInflateRect(LPRECT, CONST RECT *, int, int);

#endif
