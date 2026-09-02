# M0 T366 — Declared DOS workload command delivery

## Purpose

Recover one ordinary DOS `.COM` workload declaration through the existing
original-shaped Base VDM command route. This is the dependency-first successor
to T365: its one permitted fixed run consumed the bootstrap `/C EXIT` record
and reached original `54:01` with no next record, before guest `$Exec`.
T366 therefore closes the record-delivery prerequisite before it asks the
unchanged product to observe DOS execution again.

## Source boundary

- Original COMMAND obtains `VDMINFO` through
  `mvdm-host/dos/command/cmdmisc.c::cmdGetNextCmd`.
- Original BaseClient forms/copies the `VDMINFO` request/result in
  `opennt-host/base/win32/client/vdm.c::GetNextVDMCommand`.
- Original BaseSrv distinguishes DOS queue delivery from the separate WOW
  request behavior in `opennt-host/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand`.
- The selected local boundary is the existing same-shaped copied record route
  in `adapter-mvdm-host-out/basesrv`, with app as the sole initial declaration
  owner and session as the bounded record lifetime owner.

The historical CSR transport is not composable on modern Windows. T366 may
use only the already selected copied BaseClient/BaseSrv-shaped local route; it
must not rebuild CSRSS or create a general command interpreter.

## S plan

### S1 — Original declared-record owner ledger

Audit the original `VDMINFO` fields and caller ordering for a DOS application
record: application, CR/LF-terminated command, environment, current directory,
task/code-page/state, capture/copy size retry, consume and empty/wait result.
Compare them with current app declaration/broker behavior. Classify every
immediate edge by the mandatory source-first ladder and select the smallest
complete S2 cohort. No production behavior changes or runtime observation.

### S2 — Source-shaped declared DOS record recovery

Recover the selected complete record-delivery cohort. Preserve original
COMMAND and BaseClient result ordering, copied-record boundaries and failure
semantics. The app may accept a declared target only as a product input and
may not parse DOS syntax or load guest bytes. Use the existing session-owned
copy/identity mechanisms; no guest or native pointer may survive the call.

### S3 — Formal record-delivery closure

Build and inspect the selected CPU40/x86 product graph. Prove each original
COMMAND/BaseClient source unit and selected adapter/session/app owner is a
final input. A focused lifecycle fixture must cover one declared record,
one consumed/empty result and one malformed/oversize rejection.

### S4 — One frozen declared-workload observation

Use the fixed console-owning CPU40/x86 container exactly once with the
selected immutable `.COM` workload. Record only the earliest original `$Exec`
edge, child result/return boundary, or earlier source-defined terminal. A
runtime observation cannot authorize a trace-driven repair.

## Completion boundary

T366 closes the source-shaped declaration and its one frozen outcome. It does
not claim arbitrary command input, MZ relocation, child/parent completion,
native process execution, WOW, Redirector, graphics or x64 guest runtime.
