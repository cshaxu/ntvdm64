# M0 T346 S2 P2 — Session graphics-surface closure

## Scope

This record closes the selected data boundary only: the original SoftPC
graphics writers receive a host-local writable DIB byte plane, while app code
can subsequently obtain only bounded snapshots.  It does not create a window,
activate `X86GFX`, recreate Console Server graphics buffers, or claim a
presented guest frame.

## Source and ownership

- Original `softpc.new/host/src/nt_graph.c` retains DIB descriptor creation,
  palette order, resize/update sequencing, and the CGA/EGA/VGA destination
  pointer used by the original renderer.
- `adapter-mvdm-host-out/win32` supplies the sole modern binding for the
  retired private `CreateConsoleScreenBuffer(CONSOLE_GRAPHICS_BUFFER)`
  result.  It validates the source DIB and obtains session-owned bytes.
- `session` owns allocation, geometry, bounded snapshot and teardown.
- `app` owns no raw pointer or handle at this stage; S3 is its sole window and
  input/lifecycle owner.

The two source divergences are registered as `MVDM-HOST-DIV-176` and
`ADAPTER-WIN32-043`.  Neither changes a SoftPC controller algorithm.

## Verification

The focused `console_input_contract_fixture` and
`session_lifecycle_test` exercised both x86 and x64 forms.  They verify:

- rejecting an invalid DIB descriptor;
- a source-facing writable DIB pointer only while the session is active;
- a copied graphics snapshot with the expected aligned stride and byte value;
- deterministic clear and unavailable result after teardown;
- no transfer of the raw pointer or console handle to the app-facing API.

The formally generated CPU40 Ninja graphs additionally rebuilt the affected
`obj/session/session.obj` and `obj/adapter-win32/console_compat.obj`, then
relinked `softpc-win32-bindings.lib`, under both `record-x86` and `record-x64`.

## Result

S2 has one bounded text/graphics surface route for the selected presentation
cohort.  The next work is S3's public Win32 app window and session-scoped
Alt+Enter lifecycle.  The original `nt_fulsc.c` X86GFX path remains excluded.
