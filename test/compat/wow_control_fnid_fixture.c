/* Exercises the production compat operation with a controlled HWND boundary. */
#include "wow_edit_state_bindings.h"
#include <stdio.h>
static WND state;
static unsigned errors;
PWND WINAPI wow_window_client_validate_hwnd(HWND hwnd)
{
    return hwnd == (HWND)1 ? &state : NULL;
}
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL %u\n", __LINE__); } } while (0)
int __cdecl main(void)
{
    CHECK(!wow_edit_set_window_fnid(NULL, FNID_EDIT));
    CHECK(state.fnid == 0);
    CHECK(wow_edit_set_window_fnid((HWND)1, FNID_EDIT));
    CHECK(state.fnid == FNID_EDIT);
    CHECK(!wow_edit_set_window_fnid((HWND)1, FNID_BUTTON));
    CHECK(!wow_edit_set_window_fnid((HWND)1, FNID_EDIT));
    CHECK(state.fnid == FNID_EDIT);
    CHECK(wow_edit_set_window_fnid((HWND)1, FNID_CLEANEDUP_BIT));
    CHECK(state.fnid == (FNID_EDIT | FNID_CLEANEDUP_BIT));
    CHECK(wow_edit_set_window_fnid((HWND)1, FNID_CLEANEDUP_BIT));
    CHECK(!wow_edit_set_window_fnid((HWND)1, FNID_BUTTON));
    CHECK(state.fnid == (FNID_EDIT | FNID_CLEANEDUP_BIT));
    printf("WOW_CONTROL_FNID errors=%u\n", errors);
    return errors ? 1 : 0;
}
