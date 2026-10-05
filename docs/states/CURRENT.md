# Project Status

## Current Work

**Active: M0 T432 S1** (Ordinary Mode).
Owner verifies nthook32 works and directs preserving that accepted baseline,
closing T431 within its delivered x86 scope and opening a separate T for dual
NTVWM/Hook support. T431's unimplemented width expansion is transferred, not
claimed complete. General run16/ntcon/ntmon x64 migration stays Queue head
after this active package.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T432 S1; Ordinary Mode. |
| Admission And Approval | Owner: “我已经验证过，nthook32正确运行了…保留着个基线，不如开一个新的T任务来做以上这些”. Admit design/adaptation review for the separate dual-worker/Hook package; production implementation is staged in subsequent bounded S packets. |
| Candidate Proposal | [Dual-width native workers and hooks](../proposals/proposal-dual-width-native-workers-hooks-001.md). |
| Objective | Freeze exact shared/native-width source, classification, worker selection, installer/context and rollback boundaries for NTVWM32/64 and nthook32/64; preserve the accepted x86 package. |
| Non-goals | No production build/injection/deployment in S1; no general run16/ntcon/ntmon x64 migration, x64 NTSRV/MVDM/WOW32/VDMREDIR, new scheduler/registry, resident helper, guest change or native child replacement. |
| Reference Baseline | Owner-accepted T431 S2 P2 at12160c657; sealed S2/r027-runtime and r031-publication; APP0.0.427/RPC38/I/O25, x86 /MT CCPU40. [Closure](../history/m0-t431-native-hook32-closure.md). |
| Files And ABI Surface | CURRENT, ARCHITECTURE, new package proposal/plan, supporting index and T431 transfer notes. Source review: shared search/classifier, native create/install/context, NTSRV machine selection and common/worker-base dependencies. |
| Applicable Rules | Full AGENTS authorities/source policy; minimal original-shaped adaptation before mirror diff/new behavior; one active S; preserve other-session changes and build-only outputs. |
| Verification | Source/link-map/primary-API review, documentation governance, relative links and diff checks; later runtime gates are specified, not claimed passed. |
| Expected Markers | One shared discovery/classification contract, isolated x86/x64 objects/RPC stubs, exact four-direction install/context gates and single existing handoff; all nonworker/nonhook components remain x86. |
| Asset Needs | Reuse T431 S1/S3 research, S2 production tests and pinned MIT Detours4.0.1; no new guest media or imported executable in S1. |
| Reporting Requirements | Separate existing runtime proof, source feasibility and unresolved mechanisms; identify any installer-only transient helper and its exact finite review gate before implementation. |
| Stop Conditions | Security bypass/host mutation, unknown provenance, changed child identity/wait/suspension, uncontrolled helper role or unexplained baseline regression; do not publish incomplete mixed packages. |
| Exit Criteria | Source-backed finite design, shared-versus-width-specific ledger, actual target selection and context transaction contracts, reproducible positive/negative/lifecycle gates and staged implementation plan. No runtime capability closure from discussion. |
| Original Owner Request | “当前…专注于实现NTVWM32，NTVWM64，NT Hook32和NT Hook64…其他组件继续还是使用32位”; subsequently owner accepts Hook32 and requests a new T to preserve that baseline. |
| Similar-Issue Sweep | Both native widths, A/W and COM/EXE/BAT/PIF discovery, machine mismatch, context-only x86 run16, flags/handles/environment/CWD, GUI/new Console propagation, suspension, recursion, early exit and root isolation. |

## Plan and retained boundary

The [T432 plan](../etc/operations/t432-dual-width-native-workers-hooks-plan.md)
owns the sequential S1–S5 design/build/Hook64/cross-width/final-audit split.
Only S1 is active. Each worker owns its hidden Console; NTSRV owns registration,
selection, association and retirement; NTCON stays worker/width-neutral with
zero or one authorized I/O pipe. Ordinary native children retain real Windows
handles, waiting, exit codes and inherited Console. A bitness change alone
does not replace them with run16 or create a new broker direct task.
Transient installer-only helper use is permitted if necessary after exact
mechanism/provenance/rollback review, never a new resident execution owner.

T431 S3 closes design/replanning only. Its planned S4–S6 were never admitted
or implemented and transfer to T432; no x64 result is claimed. Original
[T431 design](../etc/operations/t431-native-launch-hook-design.md) remains
source evidence, superseded for live scope/sequence by the T432 plan.
Owner acceptance supersedes the former personal-verification-pending wording,
not the retained API/default-geometry limitations.

## Current Technical Baseline

The unchanged T431 S2 P2 nine-file set build/M0-T431/S2/r027-runtime is
published at O:/winnt/system32: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe,
ntvwm.exe, ntmon.exe, WOW32.DLL, VDMREDIR.DLL and nthook32.dll, plus MIT notice.
Production commit12160c657 and sealed artifacts/publication manifests preserve
this baseline independently of future dual-width changes. MSVC14.43/SDK22621,
x86 /MT CCPU40, APP0.0.427/RPC38/I/O25 are unchanged by this admission.

[T431 S2 evidence](../etc/evidence/m0-t431-s2-nthook32-implementation.md)
retains installer93 assertions, selected NTVWM lifetime1077 checks, shared
search, actual CMD/MEM parent return, Console17/Window17, independent WOW
frontiers and published identity. S2/r031-publication preserves P1 recovery
and all nine deployed hashes. Existing default private-desktop prepare-text
error87 and unsupported API forms remain explicit limits, not passing results.
WINMINE visible UI and SOL/WRITE retained error frontiers remain distinct.

Formal reusable cache: build/M0-T427/S2/r001. Sealed S2/r027-runtime is the
accepted reference; S2/r010-runtime and T430 S6/r007-runtime remain older
recovery sets. No source/build/runtime/process/package is changed by this
documentation-only transition. Future changes require affected rebuild,
runtime non-regression and coherent publication before production P delivery.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T431 | Owner-accepted32-bit native Hook/search/propagation; design-only width expansion transferred, not delivered. [Closure](../history/m0-t431-native-hook32-closure.md). |
| T430 | Owner-accepted non-WOW contract repairs/proofs; [closure](../history/m0-t430-non-wow-contract-closure.md). |
| T429 | Owner-accepted performance/shared worker I/O; [closure](../history/m0-t429-performance-worker-io-closure.md). |
| T428 | Owner-accepted worker interface unification; [closure](../history/m0-t428-worker-interface-unification-closure.md). |

## Recent Governance

T431 closes by owner scope revision and acceptance. T432 S1 is the sole active
packet; queued component x64 migration has no numeric allocation or admission.
Other-session proposal/TODO changes remain untouched and excluded.
