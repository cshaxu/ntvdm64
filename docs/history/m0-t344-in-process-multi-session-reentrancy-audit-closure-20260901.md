# M0 T344 — In-process multi-session reentrancy audit closure

## Closed result

T344 audited the reached BaseSrv/BaseClient, COMMAND, monitor, WOW, CCPU and
SoftPC state carriers before any multi-session enablement. The result is an
explicit one-active-SoftPC-machine/session-per-process profile.

- The [state-owner ledger](../etc/operations/m0-t344-s1-state-owner-ledger.tsv)
  separates per-session mapping managers and per-thread caller bindings from
  original CCPU, SAS, controller, video and monitor globals.
- CCPU's `ThreadSimBuf` is thread-local only after process-wide TLS-slot and
  worker registration initialization; it is not an independent machine.
- Original `CurrentMonitorTeb`, SAS/M-area and device/controller state prove
  that enabling a second active machine would require a complete lifecycle
  owner package, not a scheduler-local copy.
- The [formal closure evidence](../etc/evidence/m0-t344-s3-single-session-formal-closure-001.md)
  records unchanged paired CPU40 graph verification.

## Explicit non-claims

T344 does not recreate CSRSS/BaseSrv, kernel VDM, the original shared/separate
WOW product shell, cross-process control or a second machine executor. It
does not alter guest behavior, BOP routing, CCPU or machine code.

The next ordered package is host capability expansion. It begins with an
original owner/capability inventory and must not infer a capability merely
because a modern Win32 API exists.
