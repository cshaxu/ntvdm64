# M0 T291 S5 — Source-shaped command broker and monitor re-entry recovery

## Admission dependency

S4 must close first. S5 is the named follow-on, not a claim that the current
single immediate command source is already a BaseSrv/CSR replacement.

## Objective

Recover only the original, publicly reproducible command-broker and monitor
re-entry behavior needed by reached MVDM callers. Reuse the original OpenNT
BaseSrv/client package before authoring any seam. Preserve `GetNextVDMCommand`
and `VDMINFO` shapes, original state/capacity/re-entry ordering, and the
separation between command acquisition, `NtVdmControl`, and synchronous WOW
callback execution.

## Required source-first sequence

1. Audit `vdmapi.h`, `cmdexec.c`, `cmdmisc.c`, `config.c` and `wkman.c` as one
   command-broker package: capture ownership, caller state flags, no-command
   success, capacity retry, re-entry count, notification and wait/wake order.
2. Audit the complete approved OpenNT BaseSrv/client package: at minimum
   `base/win32/server/srvvdm.c`, `srvvdm.h`, `basesrv.h`, `srvinit.c`,
   `base/win32/client/vdm.c` and `base/win32/inc/basemsg.h`. Select the merged
   edition baseline, record hashes and determine the smallest exact original
   subset needed for the VDM service.
3. Import the selected source into a dedicated original `opennt-host` mirror;
   retain the original server/client call order and record every unavailable
   CSR/private-NT dependency before binding it. `mvdm-platform-abi` remains a
   declaration-only shared mirror unless a selected BaseSrv declaration is
   demonstrably private to `opennt-host`.
4. Recover the applicable one-session COMMAND and WOW consumer arbitration
   through the imported source. The BaseSrv-specific `adapter-opennt-host`
   preserves an original server CSR/private-NT operation only where it cannot
   be composed directly, and is consumed only by `opennt-host`. A bounded `broker`
   record/notification seam is permitted only behind such an identified
   original server dependency; it may not become a generic operation
   dispatcher. Every unavailable WOWEXEC/CSR/private-server behavior retains
   an explicit original failure or deferred result.
5. For each admitted `host_simulate` group, compose the original caller's
   preconditions and postconditions around the same-shaped mechanical entry;
   never attach business interpretation to `adapter-softpc` itself.
6. Extend the WOW TD/TEB projection only when a selected original owner body
   requires a copied numeric field. Native task/thread/window pointers remain
   adapter-private and session-bound.

## Non-goals

No self-authored BaseSrv/CSRSS clone, global current task/session, generic dispatcher,
second CPU executor, raw pointer/HANDLE transport, fast WOW assembler or
selector enablement without its owner package.

## Exit criteria

Each newly admitted command or monitor consumer has a source call-order and
failure record, an owning typed capability, x86/x64 focused proof and no
cross-plane fallback. Unadmitted private product-shell behavior remains
explicitly unavailable rather than being silently routed through COMMAND.
