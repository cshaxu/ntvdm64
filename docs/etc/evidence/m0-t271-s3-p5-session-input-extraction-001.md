# M0 T271 S3 P5 — COMMAND neutral session-input extraction

The temporary `runtime_command_misc_session` previously owned copied startup
text, multisz input/output buffers and delivery/retry markers under
`command_source_*` names.  Those fields are host/session payload lifetime,
not original COMMAND provider globals and not a BOP ABI.

They now reside in `session/session_input.{c,h}` as `session_input`:

- copied target and argument text;
- two neutral 16-bit metadata values;
- copied source and transformed multisz buffers;
- copied published-directory multisz buffer; and
- ready/delivered/retry state.

The type has no guest pointer, native handle, BOP, DOS, VDM, Win32 or Bochs
member. COMMAND keeps only its source-specific globals and uses its existing
setter contracts to populate this record. The split is the required
dependency direction for the next P: `adapter-win32` can bind the neutral
record and provide the historical `GetNextVDMCommand` and
`SetVDMCurrentDirectories` names without importing `opennt-bop`.

On 2026-08-25, refreshed formal Ninja root `build/M0-T271-S3/r001` compiled
the new `session` library and affected COMMAND library. The following focused
regressions exited zero outside the sandbox:

- `t231-s7-command-get-next-direct-fixture.exe`;
- `t234-s2-command-dynamic-environment-fixture.exe`;
- `t231-s10-command-native-session-fixture.exe`.

This P is a lifecycle/data-ownership extraction. It does not claim the
historical Kernel32/CSR VDM APIs have already moved or that BaseSrv behavior
has been recreated.
