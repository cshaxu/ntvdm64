#include <windows.h>
/* Test-only fault boundary; selected for the initialization fixture object,
 * never for the production adapter or the real native-owner probe. */
LONG WINAPI fixture_dialog_set_window_longW(HWND, int, LONG);
#define SetWindowLongW fixture_dialog_set_window_longW
