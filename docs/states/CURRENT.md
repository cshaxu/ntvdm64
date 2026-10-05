# Project Status

## Current Work

**No active M/T/S packet.** T430 remains open between stages.
Owner approves updating the inherited audit, closing S1 and planning required
non-WOW completion, with known WOW gaps handed to existing WOW32 candidates.

S1 is closed as research/documentation only; its
[updated conclusions](../etc/evidence/m0-t430-s1-inherited-contract-audit.md)
record verified identities, current receivers and qualified outstanding gaps.
No new production repair, runtime proof or deployment is claimed.

The [owner-replanned sequence](../etc/operations/t430-non-wow-contract-plan.md)
replaces the original audit-only S2–S4 plan: S2 CPU stack transitions,
S3 8042/PIC, S4 redirector copies, S5 directory reset, S6 bounded non-WOW
evidence completion, S7 integrated closure. S2 is planned, not admitted.
WOW task/shared-view, WRITE first-provider investigation and SOUND limits
have unique existing proposal receivers; queue order is unchanged.

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
