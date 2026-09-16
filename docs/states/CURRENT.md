# Project Status

## Current Work

**No active M/T/S packet.**

M0 T413 original C-VID accessor/layout restoration is closed under the owner's
standing completion objective. The [closure audit](../history/m0-t413-accessor-restoration-closure.md)
maps every admitted requirement to original-source, formal-build and actual
runtime evidence. S1/S2/S3 deliveries are 23a4fa83c, 3cc7851e5 and d9d814b00.
No next candidate is admitted; [Queue](QUEUE.md) retains its existing order.

Owner Terminal/RDP visual confirmation remains separate and has not been
reported as received. A demonstrated regression may reopen this task.

## Current Technical Baseline

- Three programs at `O:\winnt`: run16.exe, basesrv.exe and ntvdm.exe; original
  x86 /MT CCPU40, APP_VERSION 0.0.413 / protocol 3. The exact published hashes
  match formal outputs and the [S3 evidence](../etc/evidence/m0-t413-s3-integrated-verification.md).
- Original cpu/src/evid/vglob.c supplies all 76 external field-access bodies.
  The finite private binding preserves original latch/selector ownership,
  correct GDP offsets and signatures without generated scratch clobber.
- All 17 command regressions, four ConPTY sizes, mouse, live resize, native
  resize contracts and five final-hash prefilled 80x5 runs pass. The latter
  contain 20 independently confirmed post-EDIT MEM opens/completions.
- Mirror diff distance is 419 versus 506 before this task; private overlay
  cohort is 136 versus 125. Eight original files are restored, with retained
  typed glue measured rather than hidden.
- The S11 blank-gap repair remains intact. This task does not alter CCPU
  instruction algorithms, x87, WOW/WRITE, broker semantics or guest media.
  The independent IP-mask finding is explicitly retained in [Debt](TODO.md);
  it is not declared fixed by this closure.
- Formal products remain under build/M0-T413/S2/formal; harnesses, source audit
  and recoverable prior products are under build/M0-T413/S3/integration.
  Runtime captures remain only in O:\winnt\logs.

## Recent M0 Closures

T413 restores the original accessor composition and passes the declared
integration profile. T412 Broker closure remains bounded to its architecture;
its transferred C-VID defect is now handled by T413, not retroactively counted
as a T412 test. T411/T410/T409/T407/T406 retain their recorded scopes.

## Recent Governance

The active packet is removed only after the requirement-by-requirement closure
audit. The original accessor proposal links its completed history. The next
queue candidate remains x87 host-layout restoration; no task number or admission
has been allocated to it here.
