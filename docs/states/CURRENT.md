# Project Status

## Current Work

**Active: M0 T475 S5 — pure C process-hook replacement**, Ordinary Mode.
The owner admits importing win16style's `lib-win32/process-hook` unchanged,
removing C++ and Detours throughout the codebase, and preserving product
behavior. Worktree-local builds, tests, asset publication and commits remain
the delivery policy; no push or main-runtime publication.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T475 S5, Ordinary Mode. |
| Admission And Approval | Owner directs verbatim import and complete C++/Detours removal with equivalent product behavior. |
| Objective | Use the unchanged shared pure-C library and remove C++ and Detours from product, tests and build selection without changing functional contracts. |
| Non-goals | Change guest media, worker authority, NTVDM runtime import boundary, main worktree or system32. |
| Reference Baseline | Local 900c20484; main baseline 09b58f5c6; ten-image package 5ab28665-fddd-4348-8ba7-6158ee56d015; S4 evidence below. |
| Files And ABI Surface | lib-win32/process-hook, common/native-hook, NTHook, Run16, both workers, tests, build graphs, helper ordinal, payload and hook ABI. |
| Applicable Rules | Preserve product/third-party provenance and licenses, shared library identity, process rights, handles, target state, x86/x64 ABI and failures. |
| Verification | Inventory and identity-check import; x86/x64 compile; four width pairs, payload-only, normal/negative/rollback/cleanup and real guest tests; scan complete codebase and graph; Control, local package/hash and documentation gates per production P. |
| Expected Markers | Original library matches pinned source; no C++ source or Detours dependency in scope; unchanged caller semantics and verified package. |
| Asset Needs | Owner-supplied adjacent win16style library and existing immutable guest/test fixtures; no external fetch. |
| Reporting Requirements | Exact source hashes and license, P evidence, tested paths, omissions and discovered incompatibilities. |
| Stop Conditions | Required semantic change, unavailable source/license, unsupported cross-width path, failure to retain equivalent behavior or original guest constraint. |
| Exit Criteria | Each admitted P built, tested, locally published and committed; full codebase audit and behavior comparison complete. |
| Original Owner Request | Add an S to import win16style process-hook verbatim and completely eliminate C++ and Detours while keeping product functionality identical. |
| Similar-Issue Sweep | Product and test-only C++/Detours, scripts, generated graphs, archives, helper and diagnostic fixtures. |

P1 imports 23 byte-pinned library files and independent x86/x64 MSVC link and
payload fixtures; local package `8ab5e971-e071-41f7-bf24-c5e694fcbbff`
passed Control and ten hash checks. The existing Detours product path remains.
Full and DOS4GW/NESticle were not run. [S5 evidence](../etc/evidence/m0-t475-s5-process-hook-import.md).

P2 moves native Hook packet validation to selected C code and adds an isolated,
helper-free cross-width direct-payload candidate. MSVC and MinGW four-pair,
error, retry and release tests passed; the original Hook regressions and Control
passed with local package `b8fbc836-9f8c-482d-80c7-7861110808ed`. The new
payload API is not yet used by product code. The owner directs changes only
in this worktree; other repositories and worktrees are out of scope. Full and
DOS4GW/NESticle were not run.

P3 selects pure C native Hook/context/installer and direct payload transport;
product link maps contain no Detours. Four actual missing-DLL width pairs,
118 x86, 118 x64 and 168 cross-width Hook assertions, Control and real WRITE
startup snapshots passed. Local package `144d08b7-5c28-49ba-b41f-85d58908098a`
has ten verified hashes. One imported helper has a recorded local cleanup fix;
remaining diagnostic/test C++ and Detours, Full, OS/2 NE and WRITE editing/exit
remain open or unverified. See [S5 evidence](../etc/evidence/m0-t475-s5-process-hook-import.md).

P4 converts the Hook and native observation fixtures to C; 118/118/168 Hook
checks and actual x86/x64 ChildFirst observation passed, as did Control.
Local package `845cebf2-9632-40b0-8509-912517e8fad0` has ten verified hashes.
Diagnostic and legacy C++/Detours sources remain within S5.

P5 converts the startup Console trace to C; x86/x64 interception, original
call and LastError checks, an actual NTVDM diagnostic relink, product build and
Control passed. Local package `f49f5403-2030-433e-87b3-77b5858a2b35` has ten
verified hashes. The diagnostic relink was not run with a real guest.

P6 converts the startup palette trace to C. Both widths passed actual hook,
original-call and LastError checks; a test-only NTVDM relink reached Win16
WRITE main/document/page-info in four snapshots. Local package
`b5eceac1-5472-4266-8287-75ff14854f64` passed Control and hash verification.
WRITE editing and exit, Full and DOS4GW/NESticle were not verified.
P7 removes the obsolete Detours mechanism wrapper and replaces its mock-only
C++ test with C tests of the selected direct-payload implementation. x86/x64
checks passed, as did the product build and Control. Local package
`392e6339-d0e6-4e4c-a602-3c48f24cde5a` has ten verified hashes. Other
C++/Detours diagnostics and legacy fixtures remain open.
P8 converts both test-only installation write guards to C and removes their
Detours build path. Restricted file operations, unapproved target rejection,
two actual NTVDM relinks and both real WRITE startup frontiers passed. Product
build and Control passed; local package `454dd50e-b64b-42ed-bed7-453ae71bfd00`
has ten verified hashes. WRITE editing, save and exit were not verified.
P9 converts the running-NTCON Console observer to C, using the existing late
thread guard and MinHook trampolines. Real NTCON hook installation, mouse
acknowledgment and guest completion passed; four-process call/error/lifetime
and protection-failure rollback tests passed. Product build and Control passed;
local package `0abcb77d-13e9-43f6-8049-37fb5636fa3f` has ten verified hashes.
The late backend does not exclude new-thread races or implement removal.
P10 removes the unreferenced vendored Detours sources and the profile-checker
exception. Product builds, diagnostic rebuilds, product link-map checks and
Control passed; local package `1347cbbf-7cc8-4db4-b3d7-4a68910c8575` has ten
verified hashes. An adapted optional SoftPC profile audit reported existing
x86 flags on three lib-win32 sources; see P10 evidence. Historical C++ files
remain open in S5.
P11 rejects C++ inputs in the active native graph and fixes the x86/WOW32
lib-win32 macro leakage found in P10. The graph audit passed 443 authored
commands, 971 selected compiler commands were C-only, and three C++ extension
negative cases were rejected. Product build and Control passed; local package
`2edc8463-29b6-49b2-a9e6-c369ecd28ac8` has ten verified hashes. Historical
sources and late-hook concurrency limits remain open.
P12 retires six unbuildable historical C++ fixtures, one historical audit
probe and two unreferenced OpenNT C++ declarations; their exact Git objects
and provenance remain indexed in the S5 evidence. The tracked working-tree
C++ extension scan, selected graphs, product build and Control passed. Local
package `3ae63090-eba4-445c-b264-edf54f09b56f` has ten verified hashes.
Historical C++-generating tools and late-hook concurrency remain open.
P13 changes two historical byte-array generators to emit C and makes the
already-unbuildable T290 C++ ingress graph fail explicitly as retired. Synthetic
byte/identity-negative tests passed on x86/x64; the locked historical binaries were absent.
Product build and Control passed; local package
`f4f0567d-2924-4971-bdc3-a5a4d73559ce` has ten verified hashes. A Full
selected-gate comparison against pre-replacement P2 passed on P12; external
DOS4GW/NESticle was unavailable. Archived historical scripts and late-hook
concurrency remain open.
P14 retires three inactive historical C++ build scripts with indexed Git
objects and removes C++ language configuration from CMake. Fresh x86/x64
C-only configurations built the C observation peer without detecting a C++
compiler. Product build, profile checks and Control passed; local package
`8e4a9c29-e13d-4bd3-979a-4e0ed94e1d52` has ten verified hashes. Subsequent
Full selected gates passed on that package against the P2 baseline; four-pair
payload-only and missing-DLL tests also passed. External DOS4GW/NESticle,
Win16 editing and save, and general late-hook concurrency remain unverified.
WRITE normal close and clean restart passed in a subsequent two-round probe.
No untested semantic equivalence is assumed. Prior [S4 evidence](../etc/evidence/m0-t475-s4-nthook-dependency-cleanup.md),
[S3 evidence](../etc/evidence/m0-t475-s3-worker-ntvdm-remediation.md),
[S2 inventory](../etc/evidence/m0-t475-s2-component-dependency-inventory.md),
and [S1 evidence](../etc/evidence/m0-t475-s1-win32-rpc.md) retain the preceding
local closures. NTVDM PE runtime imports remain separately queued.

## S12 Closure Record

The owner accepted [S12 source layout and compile-time boundaries](../etc/evidence/m0-t473-s12-source-layout-closure.md).

## S11 Closure Record

The [S11 compilation-condition closure](../etc/evidence/m0-t473-s11-condition-cleanup-closure.md) records the selected non-MVDM cleanup, published package equivalence and deferred recovery boundaries.

## S10 Closure Record

The [S10 audit](../etc/evidence/m0-t473-s10-closure.md) records bounded source-role decisions and S11–S19 handoff.

## T471 Closure Record

T471 closes at [its closure](../history/m0-t471-closure.md): same-artifact
application/concurrency/abnormal evidence passed, while physical WRITE input
remains the explicitly approved non-pass. No runtime or guest bytes changed.

## T469 Closure Record

T469 COMM records: [S1](../history/m0-t469-s1-closure.md),
[S2](../etc/evidence/m0-t469-s2-comm-source-disposition.md),
[S3](../etc/evidence/m0-t469-s3-comm-retained-route.md),
[closure](../history/m0-t469-closure.md). No guest bytes changed.

## T463 S2 Closure Record

The [T463 S2 record](../etc/evidence/m0-t463-s2-sol-options-attribution.md)
retains the public HWND binding, SOL witness and Full gate. S3 owns remaining
task lifecycle acceptance.

The [T461 S3 record](../etc/evidence/m0-t461-s3-registry-persistence-candidate.md)
retains the durable-overlay delivery and its acceptance evidence.

T461 S1's unchanged inventory is retained in its
[owner matrix](../etc/evidence/m0-t461-s1-shell-registry-owner-matrix.md).

## T461 S2 Closure Record

The [S2 acceptance](../etc/evidence/m0-t461-s2-binding-implementation.md)
records the reviewed Registry bindings and product gate. S3 owns durable
commit, restart tombstones and cross-worker serialization.

## S1 Closure Record

The [T473 baseline](../etc/evidence/m0-t473-s1-whole-closure-baseline.md) records original source and composition inventory.

## S2 Closure Record

The [T473 CPU40/DPMI reconciliation](../etc/evidence/m0-t473-s2-cpu40-dpmi-reconciliation.md) and companion owner records retain the per-package decisions.

## S3 Closure Record

The [T473 CPU40 lifecycle](../etc/evidence/m0-t473-s3-cpu40-execution-lifecycle.md) records the execution and retirement boundary.

## S4 Closure Record

The [T473 DOS/COMMAND/XMS lifecycle](../etc/evidence/m0-t473-s4-dos-command-xms-lifecycle.md) records the selected original owner paths.

## S5 Closure Record

The [T473 SoftPC device lifecycle](../etc/evidence/m0-t473-s5-softpc-device-lifecycle.md) records the bounded device-owner audit.

## S7 Closure Record

S7 closed at [its session/callback ownership audit](../etc/evidence/m0-t473-s7-wow-session-callback-audit.md)
without a runtime change; S8 owns public-USER object identity and retirement.

## S8 Closure Record

S8 closes at its [public-USER evidence](../etc/evidence/m0-t473-s8-public-user-identity-recovery.md).
The ten-image package is published; the skipped direct completion/input
control observation remains a non-pass. S9 owns generic wrappers.

## S9 Closure Record

The [T473 USER wrapper evidence](../etc/evidence/m0-t473-s9-generic-user-wrapper-audit.md) records the mapped carrier family and delivered r10 package.

## S6 Closure Record

S6 was a no-runtime [status-sequence correction](../etc/evidence/m0-t473-s6-status-sequence-correction.md);
S7 owns the WOW32 session/callback audit.

## T459 Closure Record

W18 is closed in [the T459 closure](../history/m0-t459-closure.md). It
preserves original common-dialog and OLE alias authorities, passes real guest
modeless-dialog retirement plus exact alias lifecycle evidence, and records
guest OLE client/server as an explicit immutable stopping boundary. No product
image or guest input changed.

## Closed predecessor evidence

- [T457 closure](../history/m0-t457-closure.md): dialog/input/caret and hook/timer recovery; per-S evidence is indexed there.
- [T456 closure](../history/m0-t456-closure.md): font/text/metafile recovery and retained acceptance limits.
- [T455 closure](../history/m0-t455-closure.md): original GDI/DC/DIB/palette recovery and application observations.
- [T453 closure](../history/m0-t453-closure.md): files, environment and OEM recovery.

Detailed closed-stage chronology belongs to those retained records; the active
packet above remains the only current S authority.

## Current Technical Baseline

The [latest source delivery](../../artifacts/mvdm-audit/latest-source-release-review.md)
builds all ten components from committed production code. Full selected
product gates pass; both deployment targets match. Guest media is unchanged.

## Temporary Evidence

T459 S1--S4 used `build/t459-s1` through `build/t459-s4` only for disposable
evidence. Their retained results are indexed from the closure; stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T467 S3/S4 disposable build and acceptance evidence uses `build/t467-s3`
(60-minute phase, 3-GiB, 10-minute no-progress budget).

T471 S1 package-freeze evidence uses `build/t471-s1` (30-minute phase,
1-GiB, 10-minute no-progress budget); S2/S3 acceptance evidence uses
`build/t471-s2` (60-minute phase, 3-GiB, 10-minute no-progress budget).

T473 S1/S9 may use only `build/t473-s9` for disposable closure-ledger and
WOW-session evidence,
with a 60-minute phase, 3-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S1 may use only `build/t461-s1` for disposable source-audit evidence,
with a 30-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T461 S2/S3 disposable evidence uses `build/t461-s2` and `build/t461-s3`;
each has a 45-minute phase, 2-GiB size and 10-minute no-progress budget.

T462 and T463 disposable evidence remains under their `build/t462-s1` and
`build/t463-s2`/`s3` roots; the shared caches are not evidence roots.

T457's closed-stage evidence is indexed in its [closure](../history/m0-t457-closure.md).

T454 may use only declared `build/t454-s1`, `build/t454-s2`, `build/t454-s3`
and `build/t454-s4` roots for disposable evidence; stable `.ninja` and
`.ccache` remain reusable caches. Retain durable results before exact-target
cleanup; never use a task root as a build cache.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

T456 S1/S2 disposable evidence uses `build/t456-s1` and `build/t456-s2`.

## Recent M0 Closures

[T460](../history/m0-t460-closure.md), [T454](../history/m0-t454-closure.md),
[T453](../history/m0-t453-closure.md), T452,
[T451](../history/m0-t451-closure.md), [T450](../history/m0-t450-closure.md),
[T449](../history/m0-t449-closure.md), [T448](../history/m0-t448-closure.md).

## Recent Governance

T460 package and Clipboard acceptance are recorded in its closure above.
