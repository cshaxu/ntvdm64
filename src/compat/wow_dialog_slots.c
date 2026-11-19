#include "include/wow_dialog_slots.h"

LONG WINAPI wow_dialog_host_get(HWND w, int index, BOOL ansi)
{
    return ansi ? GetWindowLongA(w, index) : GetWindowLongW(w, index);
}

LONG WINAPI wow_dialog_host_set(HWND w, int index, LONG value, BOOL ansi)
{
    return ansi ? SetWindowLongA(w, index, value) : SetWindowLongW(w, index, value);
}

