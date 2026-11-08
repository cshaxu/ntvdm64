# Project Status

## Current Work

**Active: M0 T481 S4 — Win95 VM lifecycle and entry-path qualification.**
T481 owns the complete queue-first guest multitask payload package. S1 and S2
are closed. S3 is closed with the [fixed-scope evidence](../etc/evidence/m0-t481-s3-closure.md);
the T remains open. The [working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
retains the original obligations, source audit, evidence and approved limits.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S4, Ordinary Mode. |
| Admission And Approval | Owner approved continuous execution and per-S delivery; Win95 follows the CCPU and font-bank repairs. |
| Objective | Complete Win95 VM lifetime/reuse, initialization rollback, wait/wake, input and presentation qualification; investigate the reproduced batch-to-native entry path without losing the interactive prompt behavior. |
| Non-goals | No original guest modification/rebuild, hardware V86 monitor, unrelated 486 work, speculative protocol coverage or new lifecycle authority. |
| Reference Baseline | Candidate579, package `2f22fbfe-ee21-4df6-a59b-ef50997e42ff`, S3 checkpoint `c11779ca1`; Win95 formal keyboard/editor test `6824d3853`. |
| Files And ABI Surface | Authored NTVDM64/INSIGNIA add-ons, existing NTVDM VM I/O and lifecycle seams, and their integration tests; changes follow the reached caller/provider chain. |
| Applicable Rules | Execution, architecture, coding, source policy and the approved BIOS/INT16 unread-input return boundary. |
| Verification | Original Win95 guest on isolated copy; actual two-VM receipts, lifetime, initialization rollback, input/presentation, batch entry disposition, cached builds and applicable Full gates. Distinguish diagnostic links from exact production binaries. |
| Expected Markers | No cross-VM delivery, stranded worker or blocked retirement; no false success on failed admission; original guest installation hashes unchanged. |
| Asset Needs | Existing immutable Win95 installation, authored addon builds, isolated package/desktop probes and the retained S3 evidence. |
| Reporting Requirements | Identify source owner, reached entry, production versus diagnostic evidence, exact failures and complete package hashes. |
| Stop Conditions | New semantic concession or original guest patch required; preserve failure evidence and continue independent work. |
| Exit Criteria | Win95 obligations and reproduced batch entry classified with actual evidence; unresolved coverage-only boundaries stated; S5 combined qualification follows. |
| Original Owner Request | Complete the queue-first multitask payload; publish the full host package and update W31 for additional owner testing. |
| Similar-Issue Sweep | VM generation reuse, cancellation owners, input ownership, nested handback and NTSRV coordination. |
| Temporary Evidence | `build/t481-s4`; task-owned runs, 24-hour/4-GiB review budget. Archive necessary hashes and outcomes before cleanup. |

## Current Technical Baseline

Candidate579 is synchronized as eleven host images plus manifest to
`assets/release` and `O:/winnt/system32`; `NTVDM64.386` and `INSIGNIA.386`
also match `O:/w31/PATCH`. Full549 passes 16/16; the Win3.1 standard/enhanced
and DOSONLY four-case matrix passes. The owner has not yet reported manual
results for candidate579. Evidence: [publication](../../artifacts/t481-s3/publication579.json),
[qualification](../../artifacts/t481-s3/qualification549.json), and
[Win95 editors](../../artifacts/t481-s3/win95-editors593.json).

Win95 interactive prompt tests with the diagnostic real-command host pass
keyboard, mouse menus, saved contents and two independent VM completions.
The actual observer exits with code 1, matching historical and current
no-addon controls; its original meaning is unproved. Win95 batch-launched native
CMD has empty output on both historical and current baselines and has not been
classified. Neither result is evidence of a new S3 regression. The Win95 VMM
blocked-status bit and exact production-host prompt behavior remain unproved.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): owner-directed closure; remaining
  guest multitask obligations transferred to T481.

## Recent Governance

S3's bounded closure retains the owner-approved input-return semantics,
original guest immutability and complete-package delivery rules. The
[working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
records the S4 handoff and remaining evidence boundaries.
