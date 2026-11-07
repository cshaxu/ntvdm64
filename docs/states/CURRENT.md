# Project Status

## Current Work

**Active: M0 T481 S3 — Win3.1 VM I/O and nested-chain completion.**
Owner expands T481 to own the complete queue-first guest multitask completion
package. S1 integration delivery is committed and pushed at ceea7f1f8; its
package remains the comparison baseline, not proof of complete VM I/O.
The [working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
retains every T479 S6 obligation and the remaining sequential stages.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | Queue-first guest multitask I/O completion, now admitted into T481. |
| Identifier Mode | M0 T481 S3, Ordinary Mode. |
| Admission And Approval | Owner directs this T to finish the queue-first task and sets active goal: complete multitask payload functionality. |
| Objective | Complete remaining reached Win3.1 input, text, geometry, fonts, codepages, display/metadata and nested chains without suspending unrelated VMs. |
| Non-goals | No original guest modification/rebuild; no Win95-first work, hardware V86 monitor, speculative protocol completion or new lifecycle authority. |
| Reference Baseline | f25220d98; S2 published package8b216ec7-75d1-4979-a21e-c588ca4d02d6. |
| Files And ABI Surface | Authored ntvdm64-386, NTVDM VM text/I/O/frontend, NTVWM presentation and existing RPC/I/O callers; exact changes follow caller/provider audit. |
| Applicable Rules | Execution, architecture, coding, source policy and the approved BIOS/INT16 input-return boundary. |
| Verification | Reached caller/provider matrix; actual Win3.1/native I/O acknowledgements, nested chains in both completion orders, input/output/redirection and cancellation; cached builds and applicable Full gates. |
| Expected Markers | No lost returned keys, no cross-VM delivery, no stranded worker or blocked retirement; actual guest receipts rather than dispatch-only coverage. |
| Asset Needs | Existing immutable Win3.1 media and owned probe runners; independently authored addon build. |
| Reporting Requirements | Selected code versus candidate; caller/provider evidence; exact open failures and coherent package hashes. |
| Stop Conditions | New semantic concession or original guest patch required; retain failing evidence and continue independent work. |
| Exit Criteria | Required Win3.1 operation and nested-chain matrix proved; coherent host/addon publication, review, commit and push. No known foundation defect transferred to force closure. |
| Original Owner Request | This T must also own complete queue-first multitask payload completion. |
| Similar-Issue Sweep | VM generation reuse, all four cancellation owners, input ownership, nested handback and NTSRV coordination. |
| Temporary Evidence | build/t481-s3, task-owned runs; 24-hour/4-GiB review budget, inspect no-progress runs at their contract deadlines; archive required evidence before disposal. |

## Current Technical Baseline

S2 P1 package8b216ec7-75d1-4979-a21e-c588ca4d02d6 is published at both
release destinations: all11 host images, Full04116/16, final production VM
retirement/reuse, diagnostic unread-input handback and normal Win3.1 exit.
Win3.1 ntvdm64.386 is published only in assets/release. This is a bounded
verified baseline, not complete VM I/O. S2 is closed with owner-approved historical failure disposition in the working plan.


## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure and qualification limits.
- [T479](../history/m0-t479-closure.md): owner-directed closure; unfinished guest multitasking is preserved in the queued successor.

## Recent Governance

S2 P1-P5 are committed/pushed through f25220d98. Cached builds, Full04116/16,
real input/lifecycle probes and input0573126 assertions per width qualify the
bounded S2 closure. Owner directs that unreproduced fill002/119 remain history,
not active defects or TODO; investigate only on a current reproducer. S3 is
admitted under the existing sequential plan and continuous-execution approval.
No whole-T completion or Win95 qualification is claimed.
