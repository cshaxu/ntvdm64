# Presentation-window mouse input recovery

Recover mouse input for text-mode `EDIT.COM` without replacing the existing
OpenNT/SoftPC route. The app presentation window translates focused `WM_MOUSE*`
messages into bounded `MOUSE_EVENT_RECORD`s on its existing `CONIN$` handle;
the selected original `nt_event.c`, `nt_mouse.c` and `base/keymouse/mouse_io.c`
remain unchanged.

The admitted implementation may change only `src/app/presentation_window.[ch]`
and focused tests/build/evidence. It uses current text dimensions, clamps the
client coordinate to that grid, preserves button state, and holds normal Win32
capture only while a button is pressed. It excludes guest drivers/media, BOPs,
SoftPC or original OpenNT mouse changes, graphics input and input brokering.

Acceptance requires a source-path audit, formal x86 build/link, focused record
and capture tests, and owner runtime verification of movement, click/release,
and an outside-client drag return in `EDIT.COM`.
