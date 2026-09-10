# M0 T271 S3 P15 — selector-blind bounded multisz extraction

The temporary COMMAND composition contained a generic loop that copied a
bounded guest multisz through the active checked-RAM callback.  It had no
COMMAND service, provider, session or selector meaning; it is CCPU/SAS guest
memory mechanics.

`adapter-softpc/ccpu_frame_context` now owns
`runtime_ccpu_copy_multisz(address, maximum, buffer, bytes)`.  The operation
uses only the borrowed active frame's checked read callback, retains no guest
pointer or state after the call, rejects missing double-NUL input and returns
an owned bounded copy.  The COMMAND `GetVDMAddr` source-specific allocation,
writeback and service ordering remains in the mirror composition; it merely
uses this generic primitive for its environment multisz branch.

This remains within registered `SOFTPC-DIV-005`: the source product supplied
a process-global SAS alias, whereas the current same-shaped CCPU facade
supplies a call-scoped checked copy.

Verification was performed outside the sandbox after a formal Ninja relink
in `build/M0-T271-S3/r003`:

- `t271-s3-ccpu-frame-context-fixture.exe` exited zero, including ordinary
  register/result behavior and a five-byte bounded `A\0B\0\0` multisz copy.
- `t231-s8-command-lifecycle-direct-fixture.exe` exited zero.
- `t236-s1-command-local-child-fixture.exe` exited zero, retaining the
  pending worker and opaque stream lifecycle contract.
