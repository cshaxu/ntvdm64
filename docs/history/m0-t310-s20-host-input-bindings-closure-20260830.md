# M0 T310 S20 — host-input bindings closure

S20 closed the selected original SoftPC host-input binding cluster for the
Win32/x86 CCPU40 recovery profile.

- `nt_keycd.c` remains byte-exact OpenNT source; its fixed-buffer RTL call
  uses the existing same-shaped public Win32 binding.
- `nt_event.c` now has same-shaped bindings for its original cdecl console
  wait handle, input record read/write, cursor sign and reached icon query.
- `nt_mouse.c` receives only its reached client-rectangle/client-to-screen
  coordinate binding.  System-menu, fullscreen and other display selectors
  are explicit S27 unavailable work.
- The obsolete non-width `key_history_count` type insertion was removed; the
  mirror again has the original spelling and the selected `/W4` x86 build
  keeps its warning visible.
- A selected CCPU40 formal archive and forced-link audit resolve all five S20
  input symbols.  The focused public failure-contract fixture passes.

This is source/binding closure, not a claim of live keyboard/mouse device
delivery.  That integration remains mandatory in S49.
