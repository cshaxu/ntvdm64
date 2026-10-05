# Project Status

## Current Work

**No active M/T/S packet.** T430 remains open. S3 has concluded its bounded
applicability review; subsequent S4–S7 remain planned, not simultaneously active.
The [non-WOW sequence](../etc/operations/t430-non-wow-contract-plan.md) retains
the owner provenance gate: preserve original guest defects; adopt only verified
existing SoftPC fixes for original host defects; otherwise defer originals;
correct demonstrated project-code defects. Known WOW gaps retain their
existing proposal receivers and queue order is unchanged.

## S3 Closure Record

[S3 proof](../etc/evidence/m0-t430-s3-controller-pic-boundary.md) establishes
that the matching sibling IRQ repair is excluded under the selected NTVDM
profile; the C0 one-line assignment alone cannot repair its original response
delivery. No production change is adopted. The actual production controller/
PIC fixture passes18 assertions twice, explicitly reproducing the retained
C0 limitation and proving masked-read/INTACK/EOI/queued-byte ownership with
controlled host callbacks, not a real BIOS ISR or timer/concurrency claim.
Tests and fixture build selection are delivered; original response debt is
retained in TODO. All eight published S2 hashes remain unchanged. S4–S7 and
final owner acceptance remain outstanding.

## S2 Closure Record

[S2 evidence](../etc/evidence/m0-t430-s2-softpc-repair-gate.md) records
MVDM-HOST-DIV-327: the exact existing SoftPC correction for CALL gate, outer
RETF and outer IRET, using original set_current_SP after loading new SS.
No new CPU helper, guest patch, protocol or scheduling policy is introduced.

The pre-repair 24-case actual-entrypoint fixture fails exactly six mismatched
operand/SS-width cases; the repaired complete 36-case profile passes. Original
CCPU high-linear, reset/debug/fault and thread-lifecycle fixtures pass.
Formal x86 build, Console17/Window17, three independent WOW frontiers,
native/nested Console/nested Window input/parent-return/exit23, coherent
eight-file publication and actual published Console/Window smoke all pass.
Fault observation is not exhaustive IDT or whole-CPU equivalence proof.

Source provenance, test-only fail-closed dependency maintenance and remaining
acceptance boundaries are retained in the evidence. Physical RDP behavior and
new WOW capability are not claimed. S3 is the bounded 8042/PIC follow-up;
T430 final closure is not approved or claimed.

## S1 Closure Record

Research-only closure ee57f6d64 is pushed. The
[S1 evidence](../etc/evidence/m0-t430-s1-inherited-contract-audit.md) reconciles
the selected inherited inputs and records qualified gaps and receivers.
It does not certify universal runtime equivalence or production fixes.

## Current Technical Baseline

The S2 coherent eight-file set build/M0-T430/S2/r002-runtime is published
at O:/winnt/system32: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe,
ntmon.exe, WOW32.DLL and VDMREDIR.DLL. MSVC14.43/SDK22621, x86 /MT CCPU40;
APP0.0.427/RPC38/I/O25 unchanged. r005-publication records matching source/
published hashes and recovery of the prior T429 set; r006-published-smoke
passes actual published Console/Window MEM and native-zero.

The formal cache is build/M0-T427/S2/r001, with regenerated source manifest.
The previous accepted package 5b9931b8e/T429 S8 is the recovery baseline.
Both workers retain shared publication/input machinery; original guest media,
VGA presentation, frontend format routing and NTSRV authority are unchanged.
No helper, CPU30, new wire or unrelated sibling repair is included.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T429 | Owner-accepted performance and shared worker I/O; [closure](../history/m0-t429-performance-worker-io-closure.md). |
| T428 | Owner-accepted worker interface unification; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

S2 removes the repaired CPU item from unplanned TODO; S3 replaces the proposed
8042 repair with the verified selected-branch original response limitation.
Other original defects retain their bounded future work or deferral. Detailed proof lives in
indexed S2 evidence, not a second task register. Unrelated side-session
proposal edits remain preserved and excluded. Governance, links, diff review,
commit and push form the sequential deliveries; T430 remains open.
