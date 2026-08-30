# M0 T317 S3 — COMMAND child lifecycle and local stream endpoints

## Selected original cohort

`cmdexec.c`, `cmdexit.c`, `cmdredir.c`, and `cmdpif.c` from the original
COMMAND `sources` manifest.

## Scope

Recover the original local child-process control flow as one cohort: command
classification, process creation, Base VDM re-entry balance, exit result
handoff, and local standard-stream redirection records.  The original bodies
remain the providers.  Reached private product boundaries use only existing
same-shaped Base VDM, session, SoftPC mapping, and public Win32 bindings.

## Required recovery order

1. Inventory the original `cmdExec`/`cmdReturnExitCode` request, re-entry,
   process and result sequence before changing any source or binding.
2. Reconcile standard-stream/redirector records with the session host-resource
   mapping manager.  No native handle may enter guest state or a durable
   original DWORD field.
3. Reconcile the original Base VDM calls with the S2 local protocol; do not
   add a COMMAND-private dispatcher, scheduler, or worker.
4. Formally compose all four original units with their existing shared state,
   then add focused positive and explicit-unavailable local-contract tests.

## Progress record

- **P1 — child-local standard streams:** closed. The original
  `SetStdHandle -> CreateProcess -> restore` call order in `cmdexec.c` remains
  intact; the translation-unit binding supplies only the child `STARTUPINFO`
  endpoints, without mutating the CLI's process-global standard handles.
- **P2 — Base VDM worker context:** closed. Original `cmdCreateProcess`
  performs `INCREMENT_REENTER_COUNT` and `DECREMENT_REENTER_COUNT` on its
  child worker. The worker receives the already-bound Base VDM record through
  an owner-private session thread hook, rather than a COMMAND-private
  dispatcher or captured native record pointer. The Base VDM focused fixture
  invokes both operations on a detached cdecl-compatible worker and verifies
  that the count returns to zero.

## Explicit exclusions and transfers

- DOS `EXEC`, PSP creation, parent restoration and ordinary guest return are
  guest-execution owners; S3 cannot invent them as a substitute for a host
  child lifecycle.
- Remote/named-pipe Redirector semantics and full PIF/console product shell
  transfer to their named owner packages.
- WOW child broker, BaseSrv/CSRSS server transport, VDD/debugger, BOP trace,
  CPU30/V86 and Bochs are excluded.
