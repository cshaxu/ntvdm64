# Native Console mouse-input recovery

Recover mouse input for text-mode `EDIT.COM` in its actual `cmd/conhost`
Console, without replacing the selected original OpenNT/SoftPC route. The
original `nt_hosts.c::SetupConsoleMode` already owns the input-handle mode
transition, but enables mouse input only for the historical non-stream display
path. The standalone product intentionally remains in stream I/O, so the real
Console never emits `MOUSE_EVENT_RECORD`s for the unchanged original
`nt_event.c` → `nt_mouse.c` → `base/keymouse/mouse_io.c` route.

The admitted recovery changes only the existing `SetupConsoleMode` calculation:
retain its original `GetConsoleMode`/`SetConsoleMode` ordering, keep window
notifications non-stream-only, and enable public Console mouse input for stream
I/O too. It also uses `ENABLE_EXTENDED_FLAGS` and clears QuickEdit, because
QuickEdit otherwise consumes physical clicks for selection before the Console
event worker can read them. It excludes the self-painted presentation window,
guest drivers/media, BOPs, SoftPC mouse algorithms, graphics input and input
brokering.

Recovery ladder: the directly composable original owner is `nt_hosts.c`; no
external OpenNT package is missing. Its original non-stream branch supplies
the exact mode-setting shape, while the standalone stream-I/O policy is the
only unavailable historical environment. A registered minimal mirror change is
therefore sufficient; no adapter or new input implementation is justified.

Acceptance requires source-path audit, formal x86 build/link, bounded real
Console observation of the requested mode bits and original callback path, and
owner-visible `EDIT.COM` click/release acceptance in ordinary `cmd/conhost`.
