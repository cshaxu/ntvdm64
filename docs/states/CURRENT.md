# Project Status

## Current Work

**Active: M0 T430 S1** (Ordinary Mode; bounded audit, no production repair).
Owner admits the CCPU40/V86 guest-contract audit on 2026-10-04 and requests
reuse of the other session's build-directory research. T429 is owner-closed;
the accepted production package remains unchanged.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T430 S1; Ordinary Mode. |
| Admission And Approval | Owner: "好，准入CCPU40/V86 guest 契约一致性审计，你找一下，build目录下应该已有另外会话做的审计结果了，你研究下。" |
| Candidate Proposal | [Bounded guest-contract audit](../proposals/proposal-ccpu40-v86-guest-contract-audit-001.md). |
| Objective | Freeze the selected current x86 contract boundary and classify inherited source/build/media/runtime evidence; connect reached guest edges to current receivers and explicit unreached/profile dispositions without a new global census. |
| Non-goals | No production or guest repair, CPU30 selection, new CPU correctness proof, global BFS, helper, protocol, scheduler or speculative WRITE attribution. |
| Reference Baseline | Production5b9931b8e, owner closure930a0f2d8; T429 S8 r008 runtime/r011 manifest/r013 smoke, x86 MSVC14.43/SDK22621 /MT CCPU40 APP0.0.427/RPC38/I/O25. Old research snapshot99c458b4b and September30 updates are historical until input identity is verified. |
| Files And ABI Surface | CURRENT/QUEUE, indexed audit ledger and build/M0-T430/S1 output; selected original/adapter/source/build/media files read-only. ABI unchanged. |
| Applicable Rules | AGENTS reading set, source policy, finite proposal scope, provenance/identity before reuse, original-owner recovery ladder, side-session preservation and build-only output. |
| Verification | Check prior report schemas/counts/IDs, current source hashes versus saved census, current actual build selection, media and artifact identity, existing test provenance and evidence confidence; governance/links/diff. No product tests in initial inventory review. |
| Expected Markers | G01–G17 and H/K/G/O/W IDs preserved; source candidate, selected implementation, runtime witness and semantic equivalence distinguished; changed/missing inputs and bounded follow-up explicit. |
| Asset Needs | build/research-ccpu40-v86-guest-contract-20260929; existing T420/T422/T425/T427/T428/T429 evidence and validated formal cache. Fresh audit products only under build/M0-T430/S1. |
| Reporting Requirements | Identify reusable, stale and unproved conclusions; report changed-source/build/media counts and concrete owner handoffs; do not equate static coverage with runtime equivalence. |
| Stop Conditions | Required cross-package repair, changed guest media, new tracing in mirrors, revived CPU30 or enlarged instruction/global inventory needs separate review. |
| Exit Criteria | Bounded source/profile/receiver ledger reconciled against current selected inputs; reached edges have receiver, remaining candidates have honest finite dispositions; evidence/governance review and commit/push. S2 requires sequential admission after S1 closure. |
| Original Owner Request | Admit the queued contract audit and study existing other-session results in build. |
| Similar-Issue Sweep | Stale component names/paths, graph versus binary identity, macro versus decoded runtime evidence, original guest limitations, conditional dispatch and startup-environment attribution. |

[S1 inherited-evidence review](../etc/evidence/m0-t430-s1-inherited-contract-audit.md)
records progress, not closure. Later S2–S4 retain the proposal's bounded
binding review, workload evidence and original-owner handoff sequence.

## Current Technical Baseline

Accepted production delivery: 5b9931b8e, pushed to main/origin.
The coherent eight-file set build/M0-T429/S8/r008/runtime remains published
at O:/winnt/system32: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe,
ntmon.exe, WOW32.DLL and VDMREDIR.DLL. MSVC14.43/SDK22621, x86 /MT CCPU40;
APP0.0.427/RPC38/I/O25 unchanged. S8/r011 pins publication/recovery hashes;
r013 confirms published Console/Window smoke and all-eight hashes.

Both workers use worker-base publication and event-driven input machinery.
The publisher compares immutable complete state against its last successful
commit, with a maximum50Hz cap. Native20ms sampling only captures active
hidden Console output. NTVDM original input processing remains in place;
NTCON remains format-driven without display deduplication. NTSRV retains
connection/lifecycle authority. No helper or new wire is added.

[S8 verification](../etc/evidence/m0-t429-s8-shared-event-input.md) passes
focused fixtures, Console17/Window17/WOW, EDIT200, all8 integration cases and
published smoke. Physical RDP latency, matched SoftPC comparison and universal
mouse-speed improvement are not claimed. Native capture/final-capture retry
and recorded limits remain; host scrollback is not promised.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T429 | Owner-accepted performance and shared worker I/O; [closure](../history/m0-t429-performance-worker-io-closure.md). |
| T428 | Owner-accepted worker interface unification; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

Owner admits the former queue head as T430 S1; it is removed from the
unnumbered queue and later relative order is retained. Admission and initial
evidence review are documentation-only. Unrelated proposal edits remain
untouched and excluded. Production repairs are not admitted by this audit.
