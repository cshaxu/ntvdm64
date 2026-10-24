# M0 T452 S3 — alias and WinOldAp ownership audit

## Scope and method

This receipt closes the bounded S3 source review; it does not claim that the
separate Win16-to-native-to-Win16 integration is accepted.  It reads the
content-pinned OpenNT audit inputs named in the
[T452 plan](../operations/t452-modules-memory-aliases-plan.md), compares the
selected product owners, and exercises the actual CCPU40 `wole2.c` compile
command.  No guest source or binary was changed.

The review deliberately treats four objects as separate:

| Object | Sole owner and disposition |
| --- | --- |
| OLE HTASK alias | Original process-local 32-slot `wole2.c` table.  A selector-shaped value is neither a guest task nor a process handle. |
| WinOldAp record | The creator task's original `PTD::pWOAList` node in `wkman.c`. |
| WinOldAp representative | `WOAINST::ptdWOA`, initially null and associated by original `W32Thread` through `pptdWOA`. |
| Child process lifetime | The original waiter branch owns `GetExitCodeProcess`, list unlink and `CloseHandle`; neither `CleanupWOAList` nor `W32FreeTask` closes or terminates a child. |

## Original-owner comparison

The relevant current call graph remains original-shaped:

```text
WK32LoadModule32 -> pWOAList node / pptdWOA
W32Thread        -> ptdWOA association
WK32WowGetModuleHandle / walias / wuwind -> caller-task list lookup
WinOldAp waiter  -> CleanupWOAList -> CloseHandle
W32FreeTask      -> node-only disposal
```

`wkman.c` has pre-existing lifecycle and USER divergences, but no diff hunk
intersects `CleanupWOAList`, WOA allocation/association, the normal wait and
close branch, or `WK32WowGetModuleHandle`.  `walias.c` and `wuwind.c` have
unrelated existing USER/legacy-provider divergences; their WOA selection
branches remain the selected original bodies.  This excludes a project-owned
replacement registry, but it does **not** prove all original races safe on a
modern host.

The original WOA branches intentionally differ:

- `WK32WowGetModuleHandle` returns zero while its matching record has no
  representative.
- two instance/window lookups dereference `ptdWOA` after finding a matching
  child record.  The pinned audit identifies that as an original pending
  association assumption.  There is no current reproduced project-created
  interleaving that licenses inventing a fallback instance/module value.
- A record's destruction is not an ownership transfer of its process handle.
  Adding `CloseHandle` or `TerminateProcess` to generic retirement would
  change the surviving-waiter and Explorer-substitution behavior.

Accordingly S3 adds no WOA coordinator, lock, process-handle cache, close, or
termination policy.  That is a measured zero production-line change, not a
claim that the historical implicit association is a general modern
concurrency contract.

## Actual alias-owner evidence

`tests/observation/verify-wow-module-alias-bounds.ps1` extracts the exact
production `wole2.c` CCPU40 command from
`build/.ninja/msvc-x86-release`, recompiles that owner with the fixture, and
runs it.  `build/t452-s3/alias-lifetime-r1/exit.txt` is `0`; the fixture
prints `WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK`.

It proves, against the actual selected alias implementation:

1. existing bounded name behavior: full, truncated, one-byte and unavailable
   alias image-name output;
2. the original 32-entry selector shape and oldest-creation-time overwrite:
   after an initial alias and 32 later live native threads, the thirty-third
   alias reuses the initial slot and the old thread no longer resolves through
   `FindHtaskAlias`;
3. a thread's signaled state alone does not invalidate an alias.  This is the
   original contract: `GetHtaskAlias` validates thread ID, process ID and
   creation time, not signaled state.  Explicit `RemoveHtaskAlias` withdraws
   the record; future ID reuse is detected only when the identity query
   differs.

The test therefore does not silently strengthen the contract with generations,
permanent handles, an unlimited registry, or a thread-termination policy.

## Explicit S4 handoff

The earlier immutable `WINOLDAP.MOD` test control recorded in
[real chains](m0-t445-s6-real-chains.md#r062-matched-pre-s6-return-chain-control)
reached representative task creation but did not complete the concurrent
return chain.  Its source trace points to the original USER `MsgWait` /
directed-yield service boundary, not to alias selection, WOA node ownership or
module retirement.  S3 cannot repair it without importing/replacing a
scheduler or USER execution mechanism prohibited by its admission.

T452 S4 must keep that integration as an explicit non-pass and either prove a
small already-owned source binding or hand it to a newly admitted execution
packet.  It must not close W03 by relabelling the static WOA result as a
runtime success.  No guest modification, process termination, or synthetic
second task registry is authorized by this handoff.

