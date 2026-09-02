# M0 T356 — Minimal DOS guest execution and controlled return

## Purpose

P07 is the required predecessor of graphics-workload integration.  It recovers
one complete vertical path from an original COMMAND-provided DOS command
record, through the guest load and original DOS `EXEC` lifecycle, to a
controlled parent/COMMAND return.  It does not use a trace hit as a service
implementation request.

## Admission sequence

1. **S1 — source/ABI/failure topology.** Audit the whole original record
   consumer, guest-load, NTDOS EXEC, PSP/arena/JFN/environment, parent return,
   SoftPC re-entry and Base VDM completion chain.  Classify every immediate
   edge as direct mirror, existing same-shaped adapter, required recovery,
   unavailable historical product shell, or out of scope.  Select the earliest
   complete recoverable owner cohort.
2. **S2 — selected owner recovery.** Reuse the original source body through
   the smallest existing or newly admitted same-shaped adapter.  Do not create
   a synthetic command result, guest image mutation, private pointer mapper or
   a new executor.
3. **S3 — local and formal closure.** Exercise the original-owner result and
   failure directions with focused x86 tests, then formally link the selected
   CPU40/x86 product.  Re-audit every modified mirror divergence and adapter
   ownership.
4. **S4 — one integration observation.** With frozen product media and one
   declared local DOS workload, make one fixed-container observation.  It may
   establish the controlled result/return or transfer exactly one later source
   owner; it may not select an ad-hoc repair.

## Explicit exclusions

No graphics controller or presentation modification, guest rebuild/media
rewrite, BOP leaf success fabrication, CPU30, Bochs, x64 runtime gate,
CSRSS/kernel VDM recreation, ambient command reader, cross-process broker
extension, WOW lifecycle completion, PIF policy expansion or trace retry.

## Completion

T356 closes only if the selected original local DOS `.COM` or minimal `.EXE`
has one source-attributed controlled result route back through original parent
and COMMAND ownership, with the exact unsupported routes recorded.  A linked
product, a Base VDM record, a BOP ingress or a watchdog timeout is not a DOS
execution claim.
