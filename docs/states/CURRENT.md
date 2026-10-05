# Project Status

## Current Work

**Active: M0 T432 S6** (Ordinary Mode; accepted-baseline reconstruction).
Owner verifies nthook32 works and directs preserving that accepted baseline,
closing T431 within its delivered x86 scope and opening a separate T for dual
Hooks with one x86 NTVWM. The later owner override rejects dual workers.
T431's unimplemented width expansion is transferred, not
claimed complete. General run16/ntcon/ntmon x64 migration stays Queue head
after this active package.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T432 S6; Ordinary Mode. |
| Admission And Approval | Owner supersedes dual workers: retain one x86 ntvwm.exe and nthook32/64.dll; all superseded implementation is evidence only and must not enter main. Restore accepted T431 S2 P2 production sources before implementing the new contract. S2–S5 candidate is archived at evidence/t432-dual-worker-superseded-20261005, commit3d5bc9466cf7208112ea53b9ff3ebf97534b0354; S5 is superseded, not delivered. |
| Candidate Proposal | [Dual-width native workers and hooks](../proposals/proposal-dual-width-native-workers-hooks-001.md). |
| Objective | Reconstruct from12160c657 with one x86 NTVWM and matching Hook32/64; implement actual16/32/64 propagation and existing I/O handoff without worker-width selection. NTSRV distinguishes actual64 tasks and NTMON displays WIN64. Verify and publish a coherent ten-image set, then review/commit/push and await owner T acceptance. |
| Non-goals | No general component x64 migration, guest/mirror execution change, new EXE/component/resident helper, scheduler/registry, private WOW64 transition or child replacement. Cross-worker I/O retains the existing protocol. |
| Reference Baseline | Owner-accepted T431 S2 P2 at12160c657; sealed S2/r027-runtime and r031-publication; APP0.0.427/RPC38/I/O25, x86 /MT CCPU40. [Closure](../history/m0-t431-native-hook32-closure.md). |
| Files And ABI Surface | Restore src/, tests/ and tools/ to accepted12160c657; remove untracked candidate source/build files after archival. Reimplement only reviewed shared Hook/native-metadata bindings, x64 DLL build and actual-task projection; no original mirror rewrite, worker-width reservation or frontend type branch. Evidence remains historical, never a build input. |
| Applicable Rules | Full AGENTS authorities/source policy; minimal original-shaped adaptation before mirror diff/new behavior; one active S; preserve other-session changes and build-only outputs. |
| Verification | build/M0-T432/S6/r001 onward. Verify baseline source identity before rebuild. Revalidate actual32/64 targets in one x86 worker, matching/cross-width Hooks, actual WIN64 task projection, handoff, typeahead/failure/reuse/isolation; Console17/Window17 and independent WOW frontiers. Ten-image manifest/preflight/recovery/publication plus source/ABI/governance/link/diff checks. No archived candidate package is an acceptance substitute. |
| Expected Markers | One ntvwm.exe (I386), nthook32.dll (I386), nthook64.dll (AMD64); no ntvwm32/64 production targets, width-selected worker records or reservations. Actual output/result/input return and WIN64 task projection; tested/deployed ten-image hashes match with recoverable accepted baseline. |
| Asset Needs | Existing MSVC14.43/SDK22621 native toolchains, pinned MIT Detours4.0.1 and retained guest/runtime artifacts; no new guest media. |
| Reporting Requirements | Separate existing runtime proof, source feasibility and unresolved mechanisms; identify any installer-only transient helper and its exact finite review gate before implementation. |
| Stop Conditions | Security bypass/host mutation, unknown provenance, changed child identity/wait/suspension, uncontrolled helper role or unexplained baseline regression; do not publish incomplete mixed packages. |
| Exit Criteria | Integrated contracts and retained product gates pass without weakened assertions; known immutable/baseline limitations explicit; reviewed production diff committed/pushed after coherent publication and deployed identity verification. T awaits owner acceptance, not automatic closure. |
| Original Owner Request | “当前…专注于实现NTVWM32，NTVWM64，NT Hook32和NT Hook64…其他组件继续还是使用32位”; subsequently owner accepts Hook32 and requests a new T to preserve that baseline. |
| Similar-Issue Sweep | Both native widths, A/W and COM/EXE/BAT/PIF discovery, machine mismatch, context-only x86 run16, flags/handles/environment/CWD, GUI/new Console propagation, suspension, recursion, early exit and root isolation. |

## Plan and retained boundary

The [T432 plan](../etc/operations/t432-dual-width-native-workers-hooks-plan.md)
owns the sequential S1–S5 design/build/Hook64/cross-width/final-audit split.
Only S6 is active. S1–S5 below are superseded historical research, not live
implementation authority. Each worker owns its hidden Console; NTSRV owns registration,
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

## S1 Closure Record

[Design-only S1 evidence](../etc/evidence/m0-t432-s1-dual-width-design.md).

The [S1 design](../etc/operations/t432-dual-width-native-workers-hooks-design.md)
records actual linked original versus project bodies, unchanged shared search,
native-machine selection/reuse and caller-resolved system path handling,
native ABI facade/build islands, versioned copied context, true-child ownership
and the bounded opposite-width Detours helper adaptation. No new component,
resident helper, frontend width policy or mirror rewrite is selected.
The helper's existing infinite wait/double resume cannot be fixed through its
creation callback alone; S4 requires a narrow registered adaptation and actual
timeout/rollback proof. Runtime addressability, x64 classifier composition and
RPC/resource interoperability remain explicit implementation gates, not passes.
S1 governance, relative-link and diff checks pass; P1 at795f6d00f is pushed.
S2's bounded conclusion and deferred delivery are recorded below. Production
baseline and the two other-session proposal/TODO modifications are unchanged.

## S2 Closure Record

Bounded engineering conclusion only; production P/publication deferred.

The working candidate now links isolated native-worker profiles from one
source list: `build/M0-T432/S2/r001/x86/ntvwm32.exe` and
`build/M0-T432/S2/r001/x64/ntvwm64.exe`, each with architecture-local RPC39
stubs and project/common/worker-base dependencies. No original mirror body
was changed. Shared host-native image/process metadata lives in common;
NTSRV selection/reservation/registration and actual direct-target binding
carry verified machine identity. NTMON projects WIN64 from service data.
Candidate version is APP0.0.428/RPC39/I/O25, not the published baseline.

Focused results in `build/M0-T432/S2/r001`: reciprocal real-image/process
machine fixtures111 checks each (steady-state handle delta0), retained native
GUI/CUI/DOS classifier227 checks with zero failures/delta, Hook32 installer93
assertions, native execution lifetime1077 checks/zero failures/zero remaining
handles, reservation/root lifetime/basic service and native-worker service
fixtures pass. Real AMD64 target management projection and wrong-machine
binding rejection pass through the production service provider; this trusted
fixture is not proof of cross-process RPC authentication. Reports are
`x86-machine.log`, `x64-machine.log`, `image-classification.log`,
`hook32-regression.log`, `native-lifetime.txt`, `native-service.log`,
`management32.log`, `management64.log` and the architecture build logs.

The formal `worker-identity-version-test.exe` now links the current generated
RPC39 client. Against a real isolated NTSRV it rejects the previous application,
wrong application protocol and retained RPC38 run16 client. Its
`<ntsrv> <previous-run16> --native-workers` case exercises actual service-created
I386 and AMD64 workers, READY/WIN32/WIN64 projection, a new authenticated
connection reusing the same worker PID, and rejection of changing a selected
worker's width. `rpc-identity.log` and `rpc-native-workers.log` retain results.
The minimal `r001/rpc-runtime/system32` set is a control-plane fixture only,
not a release package or proof of Hook64, direct execution or text I/O.
Fixture cleanup uses only this run's returned process identity with creation
time verification; it is not proof of normal retirement.

The [S2 evidence](../etc/evidence/m0-t432-s2-dual-worker-build-selection.md)
records formal shared-family32/64 build/copy integration and same-root
different-width registration. The production provider accepts actual32 and64
workers at one root and retains duplicate-same-width rejection. Actual native32
GUI direct execution twice verifies loaded Hook32, process exit0 and broker
receipt (`rpc-native-direct32.log`). Package preflight negatives pass; the
final eleven-image positive awaits actual Hook64.

S2 reaches only the admitted bounded engineering conclusion. Its source remains
uncommitted and its production P/publication is explicitly deferred; real64
direct execution, native text-I/O and retained product gates are not passes.
Matching Hook64 is still the declared S3 runtime prerequisite. The x64
installer deliberately rejects Hook32 installation; direct requests cannot
silently run unhooked or with an opposite-width DLL. No candidate was
published, committed or claimed as a delivered S2 P; O:/winnt/system32 retains
the owner-accepted T431 package and unrelated session edits are preserved.

## S3 Closure Record

Bounded engineering conclusion only; production P/publication deferred until
cross-width installation and retained product gates are complete. The
[S3 evidence](../etc/evidence/m0-t432-s3-matching-hook-progress.md) records
actual Hook32/64 installer113/114 assertions, real service direct execution
twice per width, unchanged search/metadata, and eleven real CMD legacy/Win16
chains at each width on the same integration-only package. Actual64 CMD-to-x86
Run16/MEM and repeated return pass with real output/exit assertions. Default
private geometry remains the same failing baseline limitation, not a pass.
Source review confirms recipient-local context, exact pinned launcher,
same-width fail-closed install and caller-owned rollback; original mirror and
Detours sources unchanged in S3. No committed/pushed S3 P or publication is
claimed. S4 is admitted next under the approved design.

## S4 Closure Record

Bounded engineering conclusion; production P/publication deferred to S5.
[S4 evidence](../etc/evidence/m0-t432-s4-cross-width-progress.md) records
actual four-direction matching-Hook install and alternating descendants,
228/230 installer assertions, actual10-second failure/owned-helper cleanup,
recipient partial-attachment rollback and wrong-DLL rejection. Actual-process
native-path metadata avoids proven x86 System32 reopening into SysWOW64;
both-width169-check fixtures have zero steady-state handle growth. On the
final-source eleven-image S4/r007-runtime, both actual native CMD widths pass
ten retained legacy routes, reciprocal native-child-to-DOS/parent output and
visible WINMINE startup. Original mirror bodies remain unchanged. The retained
short-history fixture does not close default private geometry limits. No P,
commit/push or production publication is claimed. S5 alone is now admitted.

## Current Technical Baseline

## S5 Closure Record

Owner supersedes S5; this is replanning disposition, not functional closure
or a production P. The [S5 evidence](../etc/evidence/m0-t432-s5-integrated-verification.md)
retains candidate build, focused service fixtures and the failed native-zero
product gate. No full product pass, publication, commit or push is claimed.
All candidate code is preserved on the evidence branch named by S6 and
removed from production selection. The [S6 reconstruction record](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
supersedes its implementation plan; verification restarts from accepted source.

## Reconstructed Technical Baseline

S6 is active. src/tests/tools match accepted12160c657 exactly; Hook64 and
WIN64 task projection are not yet implemented on the reconstructed tree.
The [S6 record](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
records archive/source identity and remaining gates. Historical S3's
[implementation evidence](../etc/evidence/m0-t432-s3-matching-hook-progress.md)
records matching Hook32/64 shared-family formal builds, actual same-width
installer113/114 assertions,64-to-x86 context-only fixture, and real service
direct execution twice per width with matching Hook and receipts. Both widths
pass unchanged search and227-check native metadata tests. No original mirror
body changed. Actual32 and64 CMD legacy search/Run16/DOS parent return and
visible WINMINE startup pass on the integration-only eleven-image package,
using the retained short-history fixture; default private-desktop geometry
fails in both candidate and accepted baseline and remains a non-pass limit.
S4's [current implementation evidence](../etc/evidence/m0-t432-s4-cross-width-progress.md)
records actual four-direction/alternating Hook propagation, inherited handles/
Unicode/CWD/streams, actual exit37 and finite helper failure cleanup. Both-way
actual mixed CMD-to-DOS return, final path correction and partial rollback
pass; integrated S5 product gates still require verification. No S3/S4
P/publication is claimed; the accepted package below remains untouched.

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

T431 closes by owner scope revision and acceptance. T432 S6 is the sole active
packet; queued component x64 migration has no numeric allocation or admission.
Owner now authorizes including other-session proposal/TODO changes after
review. Their historical queue wording is corrected against T428 closure;
debt changes are checked against delivered T429 and retired source policy.
S6 P1 is documentation/replanning only, not production delivery or S6 closure.
