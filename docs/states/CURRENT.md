# Project Status

## Current Work

**Active: M0 T433 S1** (Ordinary Mode; dependency/ABI audit and detailed design).

The owner authorizes closing the delivered single-worker/dual-Hook package
without manual acceptance and admitting the next Queue candidate. This is
owner-directed closure on automated evidence, not a claim of hands-on testing.
[T432 closure](../history/m0-t432-single-worker-dual-hook-closure.md) retains
the delivered baseline, superseded research and known limitations.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T433 S1; Ordinary Mode. |
| Candidate Proposal | [Native x64 launcher/frontend/monitor](../proposals/proposal-project-components-native-x64-001.md). |
| Admission And Approval | Owner admits the next package after T432 closure without manual testing, then expands this S: “除了上面这三个以外，还要再加上NTVWM的64位原生化审计与设计。然后请你开始这个SE吧。” Retain one NTVWM; only S1 audit/design is active. |
| Objective | Produce a source/link/ABI ledger and a detailed minimal migration design for x64-only run16.exe, ntcon.exe, ntmon.exe and the single ntvwm.exe, including architecture-local static dependencies, worker-base and clients. Reuse delivered Hook64 conclusions; preserve search, true native-child creation/handles/waits, NTSRV-authorized I/O/lifecycle and both target widths. |
| Non-goals | No production implementation/publication in S1; no x64 NTSRV, NTVDM, WOW32 or VDMREDIR; no dual NTVWM builds, new executable/component/resident helper, guest rewrite, new classifier/search policy, scheduler or registry. |
| Reference Baseline | Production b97041e748772dce813fb963e7527781c7321909; documentation31140866907fccd35aaddf12ccca13b8c54a0517. Sealed build/M0-T432/S6/r048-runtime and r029-publication match O:/winnt/system32: ten images, APP0.0.432/RPC41/I/O25. Only Hook64 is AMD64; other images remain x86 /MT CCPU40. |
| Files And ABI Surface | S1 owns current state, migration plan and indexed audit/design evidence. Inspect actual selected run16/NTCON/NTMON/NTVWM link closures, common/worker-base/RPC/native declarations, target creation/presentation and both Hook-to-launcher context paths. No source/header/IDL/build-graph change is admitted by this design packet. |
| Applicable Rules | AGENTS authorities, source-first provenance, unchanged guest media, mirror-minimality, owner-local resource policy, fixed-width wire contracts and one active S. Preserve other-session changes. |
| Verification | Build-only audit outputs under build/M0-T433/S1/r001; inspect dependency files, link maps and current source identities. Mechanically check layout/width conclusions where necessary without starting a product matrix. Run documentation governance, relative links and diff review. Production version remains432 until a later source-delivery S advances it to433. |
| Expected Markers | Consumer-by-consumer original/project/existing-common ledger; actual pulled symbols versus unused archive members; native/public versus historical/private structure inventory; explicit pointer/handle/resource ownership and failure/rollback rules; finite shared dependencies and bounded sequential implementation gates. |
| Asset Needs | Existing MSVC14.43/SDK22621 x86/x64 toolchains, retained T432 package/maps and Hook64 manifests; no new guest media, external source import or runtime component. |
| Reporting Requirements | Separate prior runtime proof, current source feasibility and planned x64 behavior. Identify any original slice that cannot compose unchanged and the smallest earlier-rung facade before proposing a registered mirror diff. Record unresolved owner choices, not assumed authorizations. |
| Stop Conditions | Unexplained baseline regression; changed launch syntax/search/child identity/wait/suspension; mirror rewrite, broad OpenNT runtime import, new helper/worker or wire policy; unknown source provenance. Escalate a material design change before implementation. |
| Exit Criteria | Complete bounded dependency/ABI ledger and migration design, exact source alternatives/failure contracts and test matrix, reviewed implementation sequence, governance/link/diff checks and committed/pushed design-only delivery. No x64 product capability or publication is claimed by S1 closure. |
| Original Owner Request | Owner selects run16, NTCON and NTMON migration, then explicitly adds NTVWM64 native audit/design to this S and instructs starting it. NTVWM remains one worker, not a restored width-selected pair. |
| Similar-Issue Sweep | Both Hook widths delivering context to x64 run16; original classification of DOS/Win16/native32/native64; SEC_IMAGE/native structures, CSR capture/RTL selection, HANDLE versus task-ID width, RPC resource attachments, Window callback userdata, text/DIB/grid lengths, CWD/PATH/System32/Sysnative redirection, shutdown/restoration and mixed-worker handoff. |

## Plan and retained boundaries

[Native component migration plan](../etc/operations/t433-native-components-x64-plan.md)
owns S1 audit/design, then the owner's order: S2 NTVWM, S3 NTCON, S4 NTMON,
S5 RUN16, followed by S6 integrated delivery. S2 and later are planned,
not admitted. Each component stage retains its own verification/publication gate.

NTSRV remains the sole lifecycle/task/I/O connection authority. NTCON stays
worker-neutral, holding zero or one authorized direct I/O pipe. NTVDM keeps
the original x86 CCPU40/DOS/WOW execution boundary; NTVWM remains one worker
executing both native target widths, with its x64 migration added by owner.
Hook32/64 are retained. The intended final package still has ten images:
four migrated EXEs plus Hook64 are AMD64, the remaining five images are x86.
Current published NTVWM is still x86. Names/system32-relative paths stay.

Each shared dependency is compiled for its actual consumer ABI; no mixed
object/library architecture or CRT in one image. Copied wire records retain
fixed-width types and authenticated recipient-local resource attachments.
No global WOW64-redirection disable, executable-directory search priority,
launcher syntax change, bitness-only worker selection or private transition.

## Current Technical Baseline

T432 is closed by owner direction without hands-on acceptance. Its ten-image
package is the latest usable deployment; this admission changes no source,
application/protocol version, binary, guest/configuration or running process.

[Final S6 evidence](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
retains Hook147/147, metadata402, RPC220, native lifetime1084, service29,
Console17/Window17, independent WOW frontiers, same-worker cross-width reentry,
both-way DOS/native handoff, scoped version negatives and deployed DOS/32/64
smoke. Full-run failed observations remain recorded; corrected Control gates
and unchanged-image product gates are separate results. WINMINE visible UI is
not gameplay; SOL/WRITE retain known error frontiers. Default private-desktop
geometry and arbitrary large-environment behavior are not claimed repaired.

Formal x86 reusable cache: build/M0-T427/S2/r001. Hook64 cache:
build/M0-T432/S6/r002-hook64. Keep these as evidenced inputs, not architecture-
mixed output roots. Superseded dual-worker candidate remains archive-only at
evidence/t432-dual-worker-superseded-20261005; it is not a migration baseline.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T432 | Single x86 NTVWM, dual Hook32/64, native cross-width execution and NTSRV I/O release authority delivered; owner closes without manual testing. [Closure](../history/m0-t432-single-worker-dual-hook-closure.md). |
| T431 | Owner-accepted Hook32 baseline, preserved independently. [Closure](../history/m0-t431-native-hook32-closure.md). |
| T430 | Accepted non-WOW contract repairs/proofs. [Closure](../history/m0-t430-non-wow-contract-closure.md). |
| T429 | Accepted performance/shared worker I/O. [Closure](../history/m0-t429-performance-worker-io-closure.md). |

## Recent Governance

Owner adds the single NTVWM's x64 audit/design to S1. The initial
[four-consumer ledger](../etc/evidence/m0-t433-s1-native-width-audit.md)
is generated from image-matched current maps and graph: run16 selects three
original BaseClient and two RTL objects; NTCON/NTMON select none; NTVWM
selects original RTL error only. Handle text narrowing and historical local
capture metadata/alignment need design work. S1 remains open; no T433
production implementation or x64 runtime result exists. Later Queue candidates
retain their relative order and the T432 deployment is unchanged.
