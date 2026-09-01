# M0 T342 S2 — Debugger startup formal closure

## Verification

The fixed formal CPU40 targets were invoked outside the sandbox for
`original-softpc-process.exe`.

- x86 entered the formal directory and reported `no work to do`.
- x64 rebuilt the affected formal objects, libraries and final executable,
  including `debugger-bindings.lib`, and completed the final `link.exe`
  command successfully.

Both final maps resolve `DBGInit` to `debugger-bindings:dbg_init.obj`.

## Result

This closes the sole selected debugger cohort: the original call shape from
`scs_init` is retained and the existing narrow adapter provides the historical
debug-port probe without importing NT4 debug-event transport.

The build emitted pre-existing source warnings in unrelated SoftPC/CCPU and
`nt_bop.c` areas. They remain visible and are transferred to the repository's
cross-tree ABI/warning owner-contract audit; this task neither suppresses nor
changes them.

No BDE, VDMDBG, VDMEXTS, FAX, WOW debug notification or debugger event
transport was enabled by this verification.
