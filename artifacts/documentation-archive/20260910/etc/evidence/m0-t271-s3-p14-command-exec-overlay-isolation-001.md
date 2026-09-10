# M0 T271 S3 P14 — COMMAND `cmdExec32` overlay isolation

The original OpenNT `cmdexec.c:cmdExec32` combines a CCPU worker thread,
BaseSrv re-entry broker and process-wide standard-handle replacement.  Its
bounded one-session implementation is necessarily more than a local mirror
expression change.  Keeping that body in `opennt-bop/command/cmdexec.c` would
violate the mirror-component rule for a substantial added semantic intrusion.

P14 makes the mirror boundary explicit:

- `command/cmdexec.c` is cropped at the complete original `cmdExec32` body
  and includes one matching private overlay body at that point.
- `opennt-bop-overlay/command/cmdexec32.c` retains the same public source
  signature and the visible source order: child start, pending re-entry,
  completion, `VDMINFO` result and COMMAND CF/AL return contract.
- It uses only the already-admitted bounded child/session seams.  No adapter,
  app, host or test calls the overlay directly.
- The exact source divergence is registered as `BOP-DIV-102` in both the
  mirror and private-overlay README registers.

The pinned original has 650 lines; the resulting mirror contains 620 lines.
The textual comparison is 95 additions and 125 deletions, below the fifty
percent mirror threshold.  The larger source-derived replacement is outside
the mirror file, not hidden in it.

Verification was performed outside the sandbox after a formal Ninja relink
in `build/M0-T271-S3/r003`:

- `t231-s8-command-lifecycle-direct-fixture.exe` exited zero and verified
  execute, COMSPEC, return, guest-tail and standard-handle-token behavior.
- `t236-s1-command-local-child-fixture.exe` exited zero and verified pending
  worker, opaque stream isolation, direct/COMSPEC, failures, pipe, double
  completion and cancellation.
- `t231-s10-command-native-session-fixture.exe` exited zero and retained the
  native `54:00` source-entry route without a v1 fallback.
