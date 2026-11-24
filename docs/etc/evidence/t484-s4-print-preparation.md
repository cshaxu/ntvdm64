# T484 S4 print preparation and stale diagnostic cleanup

P11 reuses CMD TestFile staging for the canonical PRNPROBE.EXE. It removes the
redundant intermediate executable copy and interpreted runtime copying. The
existing native build still embeds the caller's output directory; the assembly
listing and guest report copies remain evidence, not production inputs.

## Failure attribution and correction

The first final-package run generated a normal PDF and returned all four guest
records with errors0,5 normal callbacks,2 cancel callbacks,2 deleted DCs and1
freed procedure instance. It then failed opening callbacks.txt. Commit
c158c342d (T483 S3 P6) removed the host callback-frame reporter and its environment
setting, but left this file reader and regex assertions in the print test.
Current production has no provider for that old text format.

Remove those stale assertions rather than restore unused production diagnostics.
The existing authored guest AbortProc checks its HDC and GetCurrentTask on every
callback and increments errors on mismatch. The retained assertions require
errors0, callback counts, cancellation, two DC deletions, instance release, real
PDF bytes and no cancelled file. They retain this observable behavior; the
retired host reporter's exact before/after text timeline is no longer asserted.
No guest source, original binaries or product implementation changed.
A sibling WINMINE resource-cleanup branch still matches the old after-ret format;
its already-pending file remains open for provider review, not counted complete.

## Evidence

build/t484-s4-print records PID27144,2026-11-24T16:57:08Z-16:58:31Z. Cached full
host build and native print producer pass; this run returns1 on the stale file
assertion. The failure is retained, not retroactively declared successful.

Final source runs under PID6664,17:01:31Z-17:01:55Z,exit0, in final/ beneath that
root. The real private-desktop Win16 normal/cancel test passes all retained
assertions. Its guest records show errors0,normalCalls5,cancelCalls2,cancelled1,
deleted2,freed1 and task6079. Approved WINSPOOL identity is checked by the runner;
it uses Microsoft Print to PDF and does not select a default/physical printer.
This is an HDC document test, not a raw spool or complete printing acceptance.

The final test regenerates its authored probe with the new output directory.
No product source changed after the successful cached full build. Final11 raw
inputs and22 dual-published hashes remain identical. No new Full runtime pass,
original guest rebuild or winnt.zip change is claimed.

## Accounting

The print entry becomes migrated-delivered. Census336/355 disposed,19 pending:
18 producer entries and1 reconciliation entry. The24 overlapping supplemental
preparation chains remain separately pending. S4 continues; S5 is not complete.
