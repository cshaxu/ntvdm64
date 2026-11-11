#ifndef WOW_BUTTON_LIFETIME_TEST_PRECOMP_H
#define WOW_BUTTON_LIFETIME_TEST_PRECOMP_H
#include "control-client/precomp.h"
/* Observe only DC acquisition/release; original color and DC bodies compile
 * unchanged against their production facade. */
HDC WINAPI fixture_button_get_dc(HWND);
int WINAPI fixture_button_release_dc(HWND, HDC);
#undef NtUserGetDC
#define NtUserGetDC fixture_button_get_dc
#define ReleaseDC fixture_button_release_dc
#endif
