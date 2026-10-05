# Project Status

## Current Work

**Active: M0 T432 S6** (Ordinary Mode; implementation delivered, awaiting owner T acceptance).
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
| Objective | Reconstruct from12160c657 with one x86 NTVWM and matching Hook32/64; implement actual16/32/64 propagation without worker-width selection. NTSRV distinguishes actual64 tasks and NTMON displays WIN64. Owner further requires NTSRV to decide I/O channel retention/release on both worker and NTCON endpoints; worker-base shares execution/acknowledgement mechanics, never a task registry or release policy. Verify and publish a coherent ten-image set, then review/commit/push and await owner T acceptance. |
| Non-goals | No general component x64 migration, guest/mirror execution change, new EXE/component/resident helper, scheduler/registry, private WOW64 transition or child replacement. Cross-worker I/O retains the existing protocol. |
| Reference Baseline | Owner-accepted T431 S2 P2 at12160c657; sealed S2/r027-runtime and r031-publication; APP0.0.427/RPC38/I/O25, x86 /MT CCPU40. [Closure](../history/m0-t431-native-hook32-closure.md). |
| Files And ABI Surface | Restore src/, tests/ and tools/ to accepted12160c657; remove untracked candidate source/build files after archival. Reimplement reviewed shared Hook/native-metadata bindings, x64 DLL build and actual-task projection; no original mirror rewrite, worker-width reservation or frontend type branch. Owner-approved release-authority correction covers NTSRV frontend/native completion, worker-base connection mechanics and worker/NTCON adapters with focused nesting/final-I/O tests. Evidence remains historical, never a build input. |
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

Owner confirms the S6 I/O correction: the worker reports completion/pause
facts, NTSRV decides retain/release from its existing records, and the worker
performs final paint/input return at its safe boundary. NTSRV then orders
frontend disconnection and waits for both endpoint acknowledgements. No local
worker admission count may decide closure. This preserves original DOS/WOW
execution ownership and adds no second relationship/task registry. RPC41
candidate integration is active; RPC40 r031 remains sealed evidence only,
not the delivered package or proof of this newly approved correction.

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

S6 implementation is closed at P2b97041e74, pushed to main. Reconstruction started from accepted12160c657;
superseded dual-worker sources remain archive-only. The final r048-runtime
ten-image APP0.0.432/RPC41/I/O25 package is published at O:/winnt/system32.
Only nthook64.dll is AMD64; all other images remain MSVC14.43/SDK22621 x86
/MT, with the original CCPU40 executor. One ntvwm.exe handles both actual
native widths. No original MVDM/OpenNT-host body differs from12160c657.

Current matching/alternating-width Hook fixtures147/147, native metadata402,
RPC220, native lifetime1084 and service29 pass. r051 supplies identical-image
Console17/Window17 and three independent WOW frontiers; its final handoff
failure remains recorded. Environment contrast identifies leaked version-test
configuration as the trigger. The runner scopes those values without changing
guest or product behavior; its six restoration/failure cases pass. r057 then
passes every affected Control gate, including the previously failed actual
Window/DOS/parent-return and both real frontend-loss/receipt1067 cases.
r050 additionally proves all four32/64 Console/Window handoffs; r052/r053/r054
prove native search/legacy routes, actual cross-width same-worker reentry and
GUI startup/projection/close. Default private-desktop geometry/environment
limits remain explicit non-passes, not silently repaired by fixture profiles.

Publication r029 preserves the coherent T431 nine-image/configuration recovery
set and verifies all ten deployed images plus MIT notice. r059 published
DOS MEM and actual32/64 CMD output/direct-receipt smoke tests pass; post-test
deployed hashes still match. An attempted r058 isolated probe rejected
O:/winnt before launching anything, as its build-only cleanup contract requires;
publication smoke uses the distinct approved exact-image/creation-time boundary.
The [S6 record](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
retains all failed/intermediate observations and final evidence selection.
Production review/commit/push are complete and the worktree was clean at P2.
This documentation-only P3 records the observed delivery. T432 remains open
for owner acceptance; no further implementation or successor T is admitted.

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

Latest S6 integration: r044-full passes WOW frontiers, Console17/Window17,
RPC/GUI/version negatives, strict DIR and modern EDIT return, then fails the
cooked-return fixture because bare COMMAND/MEM were unresolved from package
root. Owner confirms normal search semantics: fixture cwd is corrected to
System32 with bare COMMAND/MEM; existing assertions and product PATH remain.
Final release-order ACQUIRE rejection passes all29 service fixtures in r045;
RPC220/native-lifetime1084 checks also pass. r048-runtime is the latest sealed
ten-image candidate. r049 passes cooked return/rapid relaunch/isolation, then
its retirement fixture fails bare COMMAND lookup from package root; that cwd
is now System32. r050 passes both real retirement cases and all four32/64
Console/Window handoffs using the existing checked80-column observer profile.
The earlier default private-desktop environment failure remains separately
retained, not claimed repaired. Current dual-Hook147/147 and metadata402
assertions pass. r051-full ends with exit1: every preceding group passes,
but final nested-window-handoff times out with an actual NTVDM illegal
instruction dialog. r055's controlled RPC40/RPC41 pair both fail at the same
guest address under the full-run version-test environment; this does not
prove the cause or pass the handoff gate. Current r052 native chains pass
ten legacy routes and visible WINMINE startup at each actual width; r053
passes both same-worker cross-width reentry directions, and r054 actual64
GUI startup/projection/management close passes. After approval service
recovery, r056 proves absent and build-only environments pass, while
version-only reproduces the illegal instruction. The runner now restores
version-negative variables at that gate's own finally; the actual block's
absent/present x success/nonzero/exception fixture passes. r057 serial
Control gates pass on the identical r048 ten images, including final handoff.
Publication and deployed DOS/32/64 smoke pass; P2b97041e74 is pushed and
synchronized. S6 implementation is closed; T432 awaits owner acceptance.

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
S6 P1 was documentation/replanning only. P2b97041e74 delivers the reviewed
production implementation and coherent published package. Documentation-only
P3 records S6 implementation closure, not owner acceptance or T432 closure.
