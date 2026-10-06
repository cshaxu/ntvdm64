# Project Status

## Current Work

**Active: M0 T433 S4** (Ordinary Mode; native monitor migration).

The owner authorizes closing the delivered single-worker/dual-Hook package
without manual acceptance and admitting the next Queue candidate. This is
owner-directed closure on automated evidence, not a claim of hands-on testing.
[T432 closure](../history/m0-t432-single-worker-dual-hook-closure.md) retains
the delivered baseline, superseded research and known limitations.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T433 S4; Ordinary Mode. |
| Candidate Proposal | [Native x64 launcher/frontend/monitor](../proposals/proposal-project-components-native-x64-001.md). |
| Admission And Approval | Owner says “准入S四，给我汇报一下情况。” then “开始执行，帮我把NT Monitor做成原生64位EXE。” Admit and implement only S4 NTMON after delivered S3. |
| Objective | Migrate the single ntmon.exe to AMD64 with architecture-local management/RPC dependencies, preserving service-only snapshots/tree selection, labels, existing hotkeys, explicit close authorization and disconnected-monitor behavior. Build/test, coherently publish, review/commit/push and report. |
| Non-goals | No new monitor feature, task observation/registry, local process enumeration, worker-base dependency, helper, GUI frontend, protocol redesign, original mirror change or x64 run16/NTSRV/NTVDM/WOW32/VDMREDIR. S5 is not admitted. |
| Reference Baseline | S3 production245d3ac120cc060b04189bfa5118e08b34fd2a76 and closure1de70587b2c175ebafce3e171188c7481e85de83. Sealed build/M0-T433/S3/r002-runtime, publication r008 and smoke r009: ten images APP0.0.433/RPC41/I/O25; NTCON/NTVWM/Hook64 AMD64, seven others I386. Retain recovery and recorded default-desktop/environment limits. |
| Files And ABI Surface | Own ntmon-exe/main.c, selected common management/RPC and NTSRV-owned client transport, architecture-local MIDL, finite build/import/staging/verifier selection and relevant tests. Copied keys/rows retain fixed widths; native handles/pointers stay local. |
| Applicable Rules | AGENTS authorities, original-source provenance and mirror-minimality, one active S, resource ownership and architecture-local MT builds. Preserve other-session modifications and source/guest bytes outside scope. |
| Verification | build/M0-T433/S4/r001 onward. Audit actual selected map/source closure; AMD64 monitor/client/MIDL build; native layout/key/label/hotkey/selection tests, snapshot/close RPC against x86 NTSRV, permissions/stale identity/version/malformed results, disconnection/reconnection and cleanup. Real frontend/DOS/WOW/native32/native64/GUI projection and selected-close isolation; retained product/WOW gates; coherent ten-image publication, hashes and deployed monitor smoke. Governance/link/diff review. |
| Expected Markers | One AMD64 NTCON/NTVWM/NTMON/Hook64 and six I386 images. Service remains the only snapshot/identity/control authority. Same DOS/WIN16/WIN32/WIN64 labels, no MEMBERS or new grouping, title NTVDM Task Monitor and UP/DOWN/DEL/ESC behavior retained. No original mirror diff or protocol bump merely for bitness. |
| Asset Needs | Existing MSVC14.43/SDK22621/MT x64 island, S3 native generator/common/MIDL work, x86 formal cache, current monitor layout/RPC fixtures and immutable package/media. No external source or new runtime asset. |
| Reporting Requirements | Distinguish admission/source audit, compiler/unit results, real service/control proof and publication. Retain prior limitations; no physical-desktop or WOW-gameplay claim. |
| Stop Conditions | Original-body rewrite, unknown provenance, new helper/protocol/registry/control edge, pointer narrowing or copied ABI drift, changed labels/hotkeys/close permissions, unsafe process control or unexplained baseline regression. |
| Exit Criteria | Selected native dependencies and positive/negative/UI/RPC/control/cleanup tests pass with original assertions; existing package capabilities do not regress; complete ten-image publication/deployed checks pass; source/evidence reviewed and committed/pushed, clean worktree. Await owner direction rather than auto-admit S5. |
| Original Owner Request | “S2: NTVWM; S3: NTCON; S4: NTMON; S5: RUN16”; now “准入S四，给我汇报一下情况。” |
| Similar-Issue Sweep | HANDLE/RPC binding width versus fixed PID/key/count/FILETIME fields; DTO/MIDL alignment and allocation ownership, stale row/confirmation identity, native format specifiers, Console render/input/restore, reconnect/error paths, build fallback and package PE/hash expectations. |

## S3 Closure Record

[S3 closure evidence](../etc/evidence/m0-t433-s3-native-frontend-migration.md)
records verified/published production245d3ac12 and pushed closure1de70587b,
including the retained scope and default-desktop limitations.

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T433 S3 closed; Ordinary Mode. Production P1 245d3ac120cc060b04189bfa5118e08b34fd2a76 committed/pushed; this documentation-only P2 records closure. |
| Candidate Proposal | [Native x64 launcher/frontend/monitor](../proposals/proposal-project-components-native-x64-001.md). |
| Admission And Approval | After verified/published/pushed S2 NTVWM delivery, owner says “准入下一个” then “批准 开始执行”; admit only S3 NTCON in the owner-fixed S2 NTVWM / S3 NTCON / S4 NTMON / S5 RUN16 sequence. |
| Objective | Migrate the unique ntcon.exe visible frontend to AMD64 with architecture-local common/Console/Window/RPC dependencies; preserve worker-neutral rendering/input, zero-or-one authorized I/O pipe, broker-owned association/lifecycle, actual caller Console attachment and final restoration ACK. Verify, coherently publish, review/commit/push and report. |
| Non-goals | No x64 run16/NTMON in S3, no x64 NTSRV/NTVDM/WOW32/VDMREDIR, no second frontend/worker, private helper, guest/original mirror rewrite, protocol redesign, renderer duplication, frontend task registry or autonomous worker lifecycle. |
| Reference Baseline | S2 production14796f8ba56a51075f098a532d251531a9937db2 and closure8c755716404a0cdfde5e39f0a7385c0a30f52447. Sealed build/M0-T433/S2/r010-runtime/r018-publication matches O:/winnt/system32: ten images APP0.0.433/RPC41/I/O25, NTVWM/Hook64 AMD64; other eight I386. |
| Files And ABI Surface | Own ntcon-exe and its private lib/Window/Console callbacks, selected common/frontend/RPC ABI adaptations, mixed-width build/staging/verification tools and tests. Reuse S2 native dependency construction where contracts match. Original mirror/guest bodies remain unchanged; app433/RPC41/I/O25 stay unless a genuine wire change is separately justified. |
| Applicable Rules | AGENTS authorities, source-first provenance, unchanged guest media, mirror-minimality, owner-local resource policy, fixed-width wire contracts and one active S. Preserve other-session changes. |
| Verification | build/M0-T433/S3/r001 onward. Audit/rebuild AMD64 frontend and MIDL/static dependencies; focused resource/handle parsing, callbacks/userdata, text/DIB/input/title, clipping/cursor/Console switch, final-I/O/teardown/restoration, peer/broker failure and handle tests. Actual x86 service/NTVDM and AMD64 NTVWM handoff, both native target widths, relaunch/isolation, retained Console17/Window17/WOW frontiers and coherent ten-image publication/deployed smoke. Governance/link/diff review. |
| Expected Markers | Exactly one AMD64 NTCON/NTVWM/Hook64; seven other product images I386. One renderer and zero/one active pipe, fixed copied frame/input ABI, event-driven broker control, current Console geometry/cursor and restoration barrier preserved; no original mirror diff; tested/deployed hashes match. |
| Asset Needs | Existing MSVC14.43/SDK22621 x86/x64 tools, S2 sealed package/maps/source manifests, private imported frontend library provenance, native worker/Hook caches and unchanged guest media. No new external import or runtime component. |
| Reporting Requirements | Separate source/compile/unit/real-process proof and published state. Preserve S2 capabilities and known limits. Report source provenance and original mirror diff; no physical desktop claim where observation remains waived. |
| Stop Conditions | Unknown provenance, mirror rewrite, new renderer/helper/protocol/control edge or frontend lifecycle authority, narrowed resource/callback pointers, lost restore/final-I/O barriers, unsafe host changes or unexplained baseline regression. |
| Exit Criteria | Affected native/x86 dependency builds and positive/negative/lifecycle/frontend/real handoff gates pass without weaker assertions; coherent ten-image package published and verified; reviewed source/evidence committed/pushed and worktree clean. Do not auto-admit S4. |
| Original Owner Request | Owner fixes “S2: NTVWM; S3: NTCON; S4: NTMON; S5: RUN16” and, after S2 delivery, says “准入下一个”. |
| Similar-Issue Sweep | Full-width bootstrap handles, HWND/HDC/LRESULT/WPARAM/LPARAM/userdata/callbacks, native versus copied record sizes/alignment, typed RPC attachments across x86 service, channel cancellation and transport ownership, text/DIB frame limits, keyboard/mouse/title routes, Console identity/teardown, original buffer/modes/cursor and outer CMD return ordering. |

## Plan and retained boundaries

[Native component migration plan](../etc/operations/t433-native-components-x64-plan.md)
owns S1 audit/design, then the owner's order: S2 NTVWM, S3 NTCON, S4 NTMON,
S5 RUN16, followed by S6 integrated delivery. S2 and S3 are delivered.
S4 alone is active; S5 and later remain planned. Each stage retains its own verification/publication gate.

NTSRV remains the sole lifecycle/task/I/O connection authority. NTCON stays
worker-neutral, holding zero or one authorized direct I/O pipe. NTVDM keeps
the original x86 CCPU40/DOS/WOW execution boundary; NTVWM remains one worker
executing both native target widths, with its x64 migration added by owner.
Hook32/64 are retained. The intended final package still has ten images:
four migrated EXEs plus Hook64 are AMD64, the remaining five images are x86.
Current published NTCON and NTVWM are AMD64. Names/system32-relative paths stay.

Each shared dependency is compiled for its actual consumer ABI; no mixed
object/library architecture or CRT in one image. Copied wire records retain
fixed-width types and authenticated recipient-local resource attachments.
No global WOW64-redirection disable, executable-directory search priority,
launcher syntax change, bitness-only worker selection or private transition.

## Current Technical Baseline

T432 is closed by owner direction without hands-on acceptance. Its ten-image
package remains independently recoverable. S3's verified r002-runtime remains
the recoverable preceding baseline. S4 r002-runtime is now at O:/winnt/system32,
identity0.0.433/RPC41/I/O25. NTCON, NTVWM, NTMON and Hook64 are AMD64; six other
images stay I386. Guest/configuration are unchanged; S4 r006 retains S3 recovery.

[Final S6 evidence](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
retains Hook147/147, metadata402, RPC220, native lifetime1084, service29,
Console17/Window17, independent WOW frontiers, same-worker cross-width reentry,
both-way DOS/native handoff, scoped version negatives and deployed DOS/32/64
smoke. Full-run failed observations remain recorded; corrected Control gates
and unchanged-image product gates are separate results. WINMINE visible UI is
not gameplay; SOL/WRITE retain known error frontiers. Default private-desktop
geometry and arbitrary large-environment behavior are not claimed repaired.

Formal x86 reusable cache: build/M0-T427/S2/r001. Native worker cache:
build/M0-T433/S2/r001; Hook64 cache: build/M0-T433/S2/r002-hook64. Keep these as evidenced inputs, not architecture-
mixed output roots. Superseded dual-worker candidate remains archive-only at
evidence/t432-dual-worker-superseded-20261005; it is not a migration baseline.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T432 | Single x86 NTVWM, dual Hook32/64, native cross-width execution and NTSRV I/O release authority delivered; owner closes without manual testing. [Closure](../history/m0-t432-single-worker-dual-hook-closure.md). |
| T431 | Owner-accepted Hook32 baseline, preserved independently. [Closure](../history/m0-t431-native-hook32-closure.md). |
| T430 | Accepted non-WOW contract repairs/proofs. [Closure](../history/m0-t430-non-wow-contract-closure.md). |
| T429 | Accepted performance/shared worker I/O. [Closure](../history/m0-t429-performance-worker-io-closure.md). |

## S1 Closure Record

Owner accepts the presented NTVWM design and authorizes implementation.
[Initial four-consumer ledger](../etc/evidence/m0-t433-s1-native-width-audit.md)
and [migration plan](../etc/operations/t433-native-components-x64-plan.md)
supply the bounded design handoff: actual image-matched dependencies, native
resource/wire separation, one worker and original-error source reuse.
No four-component native compile/runtime closure is claimed. Remaining
run16 local capture/RTL environment and frontend/monitor native probes are
explicit obligations of their later component stages, not silently passed.
S1's design checkpoint9c0301ad7 and sequencing d9f8ba065 are pushed. The
earlier remote500 failure is resolved without a force push.

## S2 Closure Record

S2 implementation is closed at production P1
14796f8ba56a51075f098a532d251531a9937db2, pushed to main with clean worktree.
Closure P2 8c7557164 records the observed delivery; the owner's subsequent
instruction now admits S3. No S4 admission.

[S2 evidence](../etc/evidence/m0-t433-s2-native-worker-migration.md) records
the isolated AMD64 build, RPC220/lifetime1084 unit passes and actual native32
and64 projection. A proven requester/worker file-view gap is corrected by
final DOS/UNC file identity, preserving command text/search; metadata424 and
client receipt/I/O negatives pass. r009's strict CMD version marker fails
with extended-path resource lookup; final short DOS/UNC spelling and the
owned Z: fixture pass the unchanged assertion. r013 Full completes all14 groups;
r015/16/17 prove both-width legacy/Win16 routes, actual same-AMD64-carrier
reentry and GUI management. Current Hook147/147 and final four build-input
negatives pass. r018 publishes r010 and r019 deployed DOS/32/64 CMD VER/output/
receipt smoke passes with matching hashes. Official build/import has no x86
worker fallback and regenerates the same tested bytes. Review/commit/push
are complete; S2 is delivered. T433 remains open.

## Recent Governance

S3 implementation, build and affected verification are complete. Native
frontend fixtures11 and staging negatives2 pass; x86/x64 parser48 checks each,
x86/x64 full channel/failure fixtures pass. Full r006 passes all14 groups,
Console17/Window17 and retained WOW frontiers in399055ms. Published r008/r009
passes DOS MEM and32/64 CMD output/direct results with matching hashes and Z
released. [S3 evidence](../etc/evidence/m0-t433-s3-native-frontend-migration.md)
retains failed observer-width/default53x15 geometry experiments and unchanged
scope limits. Original mirror bodies are unchanged. P1
245d3ac120cc060b04189bfa5118e08b34fd2a76 is committed/pushed; S3 is closed on
the bounded automated evidence, not owner manual acceptance. Documentation-only
P2 records delivery. T433 remains open; owner now admits S4 NTMON only.

S4 admission audit: current NTMON is I386 and its458-line project-owned main
consumes common management/RPC plus broker transport and generated client
stub. S1's image-matched ledger selects no original BaseClient/RTL body.
It already uses native HANDLE/RPC_BINDING_HANDLE and fixed copied management
keys/rows; it does not enumerate Console members or processes. Its750ms
display refresh remains existing behavior, not a migration target. No S4
production edit, build, test or deployment had occurred at admission.

S4 now has a native Monitor build/import with unchanged production main and
zero original mirror diff. [S4 evidence](../etc/evidence/m0-t433-s4-native-monitor-migration.md)
records copied ABI/high-key/layout assertions, x64-client/x86-service RPC,
actual standalone disconnected survival/restart/ESC, real Console/Window UI,
mixed DOS/native/WOW/GUI projection and isolated close, plus input negatives4.
Full r003 passes all14 groups in539462ms, including Console17/Window17 and
independent WOW frontiers. r006/r007 publication and actual NTMON/DOS/32/64
smoke pass with ten-image/notice hashes and Z released. Review/commit/push
is pending; T433 stays open and S5 is not admitted.
