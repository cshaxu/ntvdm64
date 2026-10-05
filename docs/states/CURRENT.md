# Project Status

## Current Work

**Active: M0 T430 S2** (Ordinary Mode; conditional SoftPC stack-profile recovery).
Owner approves updating the inherited audit, closing S1 and planning required
non-WOW completion, with known WOW gaps handed to existing WOW32 candidates.

S1 is closed as research/documentation only (ee57f6d64, pushed); its
[updated conclusions](../etc/evidence/m0-t430-s1-inherited-contract-audit.md)
record verified identities, current receivers and qualified outstanding gaps.
No new production repair, runtime proof or deployment is claimed.

The [owner-replanned sequence](../etc/operations/t430-non-wow-contract-plan.md)
replaces the original audit-only S2–S4 plan: S2 CPU stack transitions,
S3 8042/PIC, S4 redirector copies, S5 directory reset, S6 bounded non-WOW
evidence completion, S7 integrated closure. Owner now authorizes the provenance
gate: preserve guest defects; original host defects may use verified SoftPC
repairs only, otherwise TODO; correct demonstrated project-code defects.
WOW task/shared-view, WRITE first-provider investigation and SOUND limits
have unique existing proposal receivers; queue order is unchanged.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T430 S2; Ordinary Mode. |
| Candidate Proposal | [Guest-contract recovery](../proposals/proposal-ccpu40-v86-guest-contract-audit-001.md). |
| Candidate Proposal | [Guest-contract recovery](../proposals/proposal-ccpu40-v86-guest-contract-audit-001.md). |
| Admission And Approval | Owner directs retaining original guest defects, adopting existing SoftPC fixes for original MVDM/OpenNT defects only, deferring originals without such fixes, and designing/executing project-code repairs. Sequential S2 starts after delivered S1; no simultaneous S3. |
| Objective | Recover the already evidenced SoftPC CALL/outer RETF/outer IRET stack-width correction as a complete finite CCPU40 profile, with actual-entrypoint positive/fault proof. |
| Non-goals | Guest changes; novel fixes for original host defects; CPU30; global CPU audit; unrelated sibling changes; WOW implementation; helpers/protocol changes. |
| Reference Baseline | S1 ee57f6d64 and accepted production5b9931b8e/T429 eight-file hashes; comparative SoftPC ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7 contains all3 instruction fixes. |
| Files And ABI Surface | CURRENT, source policy, indexed evidence/plan, proposal scope; later source-profile changes limited to ccpu386 call/ret/iret, recovery register and focused tests/build tools. Existing ABI unchanged. |
| Applicable Rules | AGENTS reading set, source policy recovery ladder, owner provenance gate, CPU profile completion, x86 /MT, original guest immutability, side-session preservation. |
| Verification | Pin/compare the sibling fix and original/current bodies; actual CALL gate/outer RETF/IRET across operand16/32 and new SS16/32, high ESP, frames/adjustment and fault-before-commit; affected x86 build and every-P runtime/publication gates before production delivery. Documentation-only P uses governance/links/diff. |
| Expected Markers | Verified pre-existing SoftPC fix, bounded minimal semantic diff, complete positive/negative profile rather than helper-only proof; no production pass from static inspection. |
| Asset Needs | Read-only pinned OpenNT/SoftPC sources and existing current formal cache; fresh intermediates below build/M0-T430/S2. |
| Reporting Requirements | Separate source confirmation, implementation, focused instruction proof, guest regression and coherent publication. Track original defects without SoftPC fixes as deferred non-pass, not silently fixed. |
| Stop Conditions | No usable matching SoftPC fix; wider CPU policy or guest change; unexplained failure; required new owner domain. Record/resolve before expansion. |
| Exit Criteria | Complete profile proof, source provenance/diff registration, production build/regression/publication, review/commit/push. Current planning P does not close S2. |
| Original Owner Request | Preserve original guest defects; original MVDM/OpenNT defects use SoftPC fixes if present, otherwise TODO and no repair; project-code defects receive correct designed fixes. |
| Similar-Issue Sweep | Independent operand versus SS address size; existing VM-return/interrupt set_current_SP paths; original defects versus project hooks even inside mirrors. |

[S2 source gate](../etc/evidence/m0-t430-s2-softpc-repair-gate.md) confirms
available comparative fixes only. Production implementation/test gates remain
open; this initial P changes policy/planning, not executables.

## S1 Closure Record

Research-only closure ee57f6d64 is pushed. The
[S1 evidence](../etc/evidence/m0-t430-s1-inherited-contract-audit.md) reconciles
all selected inherited input changes and assigns bounded follow-ups. It does
not certify universal runtime equivalence or claim production fixes.

## S1 Closure Record

Research-only closure ee57f6d64 is pushed. The
[S1 evidence](../etc/evidence/m0-t430-s1-inherited-contract-audit.md) reconciles
all selected inherited input changes and assigns bounded follow-ups. It does
not certify universal runtime equivalence or claim production fixes.

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

S1 closes the bounded audit and records the owner's non-WOW scope revision.
Verifier r003 passes source/group/media/publication checks and all82 drift
records have existing current boundary receivers, not asserted equivalence.
The accepted production package is unchanged. Documentation gates and
review precede commit/push; unrelated proposal edits are preserved/excluded.
T430 final closure and subsequent S implementation are not claimed.
