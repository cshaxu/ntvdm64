#ifndef WOW_NATIVE_GUI_THREAD_H
#define WOW_NATIVE_GUI_THREAD_H
#include <windows.h>
/* Existing ConnectIfNecessary native leaf, shared by control and task entry.
 * This narrow declaration avoids pulling private original EDIT layouts into
 * unrelated lifecycle code. The implementation is not duplicated. */
BOOL WINAPI wow_edit_native_ensure_gui_thread(void);
#endif
