# Project Status

## Current Work

**Active: M0 T481 S2 — Guest VM input return and lifecycle completion.**
Owner expands T481 to own the complete queue-first guest multitask completion
package. S1 integration delivery is committed and pushed at ceea7f1f8; its
package remains the published baseline, not proof of complete VM I/O.
The [working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
retains every T479 S6 obligation and the remaining sequential stages.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | Queue-first guest multitask I/O completion, now admitted into T481. |
| Identifier Mode | M0 T481 S2, Ordinary Mode. |
| Admission And Approval | Owner directs this T to finish the queue-first task and sets active goal: complete multitask payload functionality. |
| Objective | Complete Win3.1 VM input return and normal/cancelled lifecycle without suspending unrelated guest VMs. |
| Non-goals | No original guest modification/rebuild; no Win95-first work, hardware V86 monitor, speculative protocol completion or new lifecycle authority. |
| Reference Baseline | ceea7f1f8; archived T479 S6 candidate based on cedebcab55ec1da1b15557454985da78dca7ccf8. |
| Files And ABI Surface | Authored ntvdm64-386, NTVDM VM executor/I/O/wake/frontend adapters and their tests; retain existing RPC and I/O control ownership. |
| Applicable Rules | Execution, architecture, coding, source policy and the approved BIOS/INT16 input-return boundary. |
| Verification | Review candidate per file; input order/repeats/extended keys/two-VM reuse; attribute fill002 and119; cancellation/normal exit; cached builds, Full gates and actual guest probes. |
| Expected Markers | No lost returned keys, no cross-VM delivery, no stranded worker or blocked retirement; actual guest receipts rather than dispatch-only coverage. |
| Asset Needs | Existing immutable Win3.1 media and owned probe runners; independently authored addon build. |
| Reporting Requirements | Selected code versus candidate; caller/provider evidence; exact open failures and coherent package hashes. |
| Stop Conditions | New semantic concession or original guest patch required; retain failing evidence and continue independent work. |
| Exit Criteria | Input/lifecycle matrix proved; complete host/addon publication, review, commit and push. No known foundation defect transferred to force closure. |
| Original Owner Request | This T must also own complete queue-first multitask payload completion. |
| Similar-Issue Sweep | VM generation reuse, all four cancellation owners, input ownership, nested handback and NTSRV coordination. |
| Temporary Evidence | build/t481-s2, task-owned runs; 24-hour/4-GiB review budget, inspect no-progress runs at their contract deadlines; archive required evidence before disposal. |

## Current Technical Baseline

Combined T479/T480 package 25408bd6-e51b-489d-9255-0951c53ad9f3 is published at both release destinations after cached x86/x64 builds, Full16/16, CHAIN2 provider checks, governance and real Win3.1 transition, input, commands and normal-exit verification. Awaiting owner acceptance.


## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure and qualification limits.
- [T479](../history/m0-t479-closure.md): owner-directed closure; unfinished guest multitasking is preserved in the queued successor.

## Recent Governance

S1 integration delivery is committed and pushed at ceea7f1f8. S2 is admitted by the owner; no new runtime release is claimed.
