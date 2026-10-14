# Project Status

**Active: M0 T445 S8**

S4 r261 is accepted; propagation/core split/integration/final acceptance are S6/S7/S8/S9.
Owner explicitly closes S5 without manual acceptance on 2026-10-13.
Implementation, tests and publication are delivered at 7c6406575; this is
owner-directed closure, not a claim of manual test success.
See [S5 evidence](../etc/evidence/m0-t445-s5-style-dll.md).
See the [S1 closure design](../etc/evidence/m0-t445-s1-installation-design.md).

## Active Packet

### NTW16S product integration design

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T445 S8, Ordinary Mode; design then sequential review. |
| Candidate Proposal | [Window appearance](../proposals/proposal-win31-window-appearance-001.md). |
| Admission And Approval | Owner has no opportunity for S7 manual acceptance and directs admission of S8 with an integration design first. Runtime implementation awaits design approval. |
| Objective | Specify how the existing dual NTW16S DLLs integrate with native Hook and per-task WOW creation, controls and API bindings, without duplicate rendering. |
| Non-goals | No runtime edits/build/publication in this design delivery; no guest/mirror edits, new public option, RPC redesign, global theme mutation, NTCON styling or unrelated WOW recovery. |
| Reference Baseline | Delivered/pushed S7 292664b02; thirteen published images and automated gates retained. S7 manual acceptance was not performed. |
| Files And ABI Surface | Current packet, working plan and indexed S8 design; proposed local DLL creation/lifecycle and scroll/theme bindings only, no wire changes. |
| Applicable Rules | All authorities; background owned build/tests; preserve unrelated changes; no security bypass. |
| Verification | Source/interface review, owner/consumer coverage matrix, documentation governance and diff checks. Later runtime gates are specified, not claimed passed. |
| Expected Markers | One renderer, two installation entries; Off/Win16/All policy table; explicit creation, dynamic children, failure, teardown and API gaps. |
| Asset Needs | Existing admitted NTW16S resources and source; no new import. |
| Reporting Requirements | Design choices, exact source owners, remaining proofs and unchanged release status. |
| Stop Conditions | Implementation before design approval; guest/mirror or global settings change; new resource import or broader application-layout engine. |
| Exit Criteria | Reviewed design and documentation checks, commit/push; S8 remains active pending owner design approval and later implementation gates. |
| Original Owner Request | Admit S8 without waiting for S7 manual acceptance; first provide an integration design draft. |
| Similar-Issue Sweep | Native A/W and both widths; WOW shared/separate tasks; modal/modeless/MDI/dynamic controls; scroll, theme, font and destruction boundaries. |

S8 design is recorded in the [integration draft](../etc/operations/t445-s8-style-integration-design.md).
S7 is delivered at 292664b02, including Queue/proposal edits. Owner directs
the transition without manual acceptance; this is not a manual passing result.
r107 publishes r090; r111 CUI, r129 DOS leaf, r131 full DOS chains and r132
observer checks remain automated evidence. No runtime files change here.

S7 DIV-331's approved environment repair is verified by real guest resize
1184->1200 for1185 bytes, without arena damage. The next failure was bootstrap
InitTask querying policy before application delivery; existing independent
PIF replies now carry the authenticated reservation's policy. r088 passes all12
untraced separate child chains (two widths, A/W, three policies); r089 service29
and r092 governance pass. r091 Product and r095 shared/direct propagation pass;
r100 Control and r106 final shared/separate chains pass. r107 publishes all13
images to assets/release and O:/winnt/system32 with three-way hash checks.
Owner includes the pending Queue/proposal planning edits in the same delivery.
See [S7 evidence](../etc/evidence/m0-t445-s7-base-rpc-client-split.md).

## Retained S7 Bounded Packet

See the retained [S7 packet](../etc/evidence/m0-t445-s7-base-rpc-client-split.md).

S7 local split builds and its graph/map boundary checks pass. New NTCON is
readable and its invalid-entry execution returns87; no Defender clearance is
inferred. Early untraced Product verification failed the SOL window frontier in both
the S7 candidate and a retained pre-split S6 control. Standalone comparisons
do show the expected dialog. The r029 full gate passes with opt-in WOW tracing;
that diagnostic-dependent success alone did not resolve it. Later r091 passes
untraced with DIV-331; this does not establish SOL/WRITE functional completion.
See [S7 evidence](../etc/evidence/m0-t445-s7-base-rpc-client-split.md).
Independent Console17/Window17 and the actual WOW lifecycle/native-send/style
fixtures pass; r026 confirms all13 published images remain the prior release.
That was the historical pre-publication disposition; r107 supersedes it.
S7 commit/push is now 292664b02; manual acceptance was not performed.

## S6 Bounded Disposition

At its bounded disposition S6 was not delivered or accepted. Its then-open
DOS/CUI/package and separate-WOW work is retained in its
[evidence](../etc/evidence/m0-t445-s6-real-chains.md).
Owner inserts S7 to resolve the source/link boundary first. S6 outstanding
verification is retained, not waived. The former integration/final stages
move to S8/S9. Remaining S6 ingress work is now explicitly assigned to S7.

Owner approves completing CREATE_SEPARATE_WOW_VDM propagation through the
Hook-to-Run16 private request in S7, with full child-chain tests.
Reuse the two existing public options --win16-style and --win16-style-all;
add no public option and change no guest. The existing CreateProcess flag
retains its own independent-WOW meaning, separate from the style policy.

## S1 Closure Record

T445 S1 closed at 2151cf750. Its [design closure](../etc/evidence/m0-t445-s1-installation-design.md)
and [retained packet](../etc/evidence/m0-t445-s1-closed-packet.md) preserve the
source-backed design and original brief. Stage numbers in that evidence precede
the owner-inserted standalone S2; current numbering is in the working plan.

## S2 Closure Record

Owner-accepted palette repair closes with this commit/push delivery.
See [repair and verification evidence](../etc/evidence/m0-t445-s2-palette-repair.md).
The standalone experiment is admitted as S3 by the next owner request.

## S3 Closure Record

Owner accepts bounded S3 closure and defers native-menu fresh-session diagnosis
to T445 S7. The [S3 closure evidence](../etc/evidence/m0-t445-s3-style-experiment.md)
retains tested identities, limitations and the runnable standalone demo.
This does not declare the menu blankness repaired or the full T complete.

## Planned Sequence

S1--S4 are owner-accepted. S5 is owner-closed without manual acceptance;
S6 is CLI/policy propagation and title-marker verification (delivery pending).
S7 separates the RPC client core from launch style; S8 integrates the DLL;
S9 is integrated acceptance and the deferred native-menu retest.
The [S4 review](../etc/evidence/m0-t445-s4-menu-mdi-review.md) records r052
delivery de5526a16 and the r261 background/scroll repair; owner accepts r261.
The [working plan](../etc/operations/t445-win31-window-style-plan.md) defines
the contracts. S7 is now admitted. The complete style core now
belongs to src/ntw16s-dll; src/w16style-exe owns the released thin demo.

## S5 Closure Record

Owner states no manual acceptance was performed and explicitly directs S5
closure. Delivery 7c6406575 is built, tested, published and pushed; the
[S5 evidence](../etc/evidence/m0-t445-s5-style-dll.md) records the checks and
limitations. This does not close T445 or automatically admit S6.

## S4 Closure Record

Owner accepts r261; [S4 evidence](../etc/evidence/m0-t445-s4-menu-mdi-review.md)
retains delivery and scope. S5 subsequently moved the accepted core into DLLs.

## Closed S3 Packet

### Host VT conversion and indexed-frame fidelity

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T444 S3`, Ordinary Mode, single-person implementation and separate review. |
| Admission And Approval | Owner directs: “收口S2，准入S3并执行到S3有完整结论为止”. S2 is audit-only closure, not delivered automatic colour negotiation. |
| Objective | Measure host Console VT standard/256/RGB conversion and verify production capture, frame and Console/Window fidelity to the resulting indexed cells/palette. |
| Non-goals | No query responder, environment mutation, RGB cell protocol, second VT parser/quantizer, ConPTY, guest/mirror change or lifecycle redesign. |
| Reference Baseline | Owner-accepted `fb7650ce0`, release 0.0.444, protocol 45, S1 r115/r116/r119 gates. S2 admission at `781830378` has no product changes. |
| Files And ABI Surface | Test sources/scripts under tests/, indexed evidence and governance; existing native capture/packer and frontend presentation owners, no ABI change. |
| Applicable Rules | Execution/architecture/coding/document/source authorities; background builds/tests, owned processes, no subst or computer use. |
| Verification | Real ordinary Console oracle; foreground/background standard/bright/256/RGB, reset, fragmented writes, VT on/off, custom palette, raw file/pipe bytes; production packing/decoding/Console projection and Window colours. Reuse unchanged S1 gates explicitly; production repairs trigger full delivery gates. |
| Expected Markers | Input sequence, observed cells/palette, frame and presentation agree; RGB loss is attributed to the API boundary, not invented frontend loss. |
| Asset Needs | Accepted ten-image release and validated build dependencies, build-owned native probes; no guest media. |
| Reporting Requirements | Exact cases, commands, identities, observations/assertions and limitations; distinguish component tests, deployed execution and physical observation. |
| Stop Conditions | Need for query responder, new protocol/emulator, guest/mirror change or broader architecture requires review; do not assume host nearest-colour behaviour. |
| Exit Criteria | Met: r003 passes 23 frame cases per width, 230,987 checks each, plus raw file/pipe bytes. r004 verifies all ten three-way release hashes, unchanged production, 49 retained palette checks and governance/links. Delivered at `2487156ea`; owner subsequently directs T closure. |
| Original Owner Request | “行，那收口S2，准入S3并执行到S3有完整结论为止。” |
| Similar-Issue Sweep | Split sequences, VT toggles, both colour planes, palette-only changes, API writers, both presentation routes, redirected streams. |

## Closed S2 Packet

### Native indexed-16 capability discovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T444 S2`, Ordinary Mode; single-person implementation and separate review passes. |
| Candidate Proposal | [Native indexed-16 text contract](../proposals/proposal-native-indexed16-text-contract-001.md). |
| Admission And Approval | On 2026-10-11 the owner directs “验收通过，准入S2”: accept delivered S1 and admit the proposal's next capability-discovery stage. |
| Objective | Establish a truthful, discoverable indexed-16 colour contract for native CUI launches, based on actual application detection and the existing hidden Console rather than an invented compatibility claim. |
| Non-goals | No S3-wide VT degradation implementation, arbitrary per-cell RGB, guest/mirror changes, ConPTY replacement, worker-type rendering branches, new helper or lifecycle redesign. No global user-environment mutation. |
| Reference Baseline | Owner-accepted S1 at `fb7650ce0`, published 0.0.444 ten-image package and protocol 45. S1 evidence records palette fidelity and all required regressions; the historical Edit colour audit records unanswered OSC queries and ineffective environment-only advertising. Recheck the selected application version before relying on it. |
| Files And ABI Surface | Audit run16/NTVWM target environment construction, native Hook inheritance and existing Console query consumers. Any minimal target-scope implementation stays with its existing owner and gains focused tests; no frame/control ABI change is admitted. |
| Applicable Rules | Execution, architecture, coding, document and source-provenance authorities; all build/test runs background-owned; `subst` prohibited. |
| Verification | First inventory inherited capability claims, real target detection and Console query behaviour. Verify the selected mechanism on x86/x64 direct and nested CUI targets, unchanged outer environment and redirected bytes, plus a capability-aware consumer selecting indexed output. Production changes require the full coherent-package regression/publication gate; admission-only documentation requires governance, links and diff review. |
| Expected Markers | Exact target-visible detection evidence identifies indexed-16 support without advertising independent RGB/256 cells; ordinary consumer support and unsupported negotiation are explicitly distinguished. |
| Asset Needs | Accepted ten-image baseline, existing native Edit audit, selected source-proven consumer and build-owned x86/x64 capability fixtures. No new guest media. |
| Reporting Requirements | Report detection inputs, ownership, inheritance and redirection effects, actual consumers, negative outcomes and minimal proposed mechanism before claiming compatibility. A private marker alone is not application acceptance. |
| Stop Conditions | An application requires unanswered terminal queries: identify the exact query/consumer and seek owner review before adding responses. Also stop for new protocol, terminal emulator, guest/mirror change or broader architecture. |
| Exit Criteria | Owner-approved audit-only closure, no production modification or automatic application-negotiation claim. S2 implementation/testing of a new capability mechanism is no longer required. |
| Original Owner Request | “验收通过，准入S2”。 |
| Similar-Issue Sweep | Both native target widths, direct and Hook-created descendants, inherited TERM/COLORTERM/terminal identity claims, Console API versus VT query detection, file/pipe redirection and unrelated host/GUI environments. |

## Current Work

S6 progress: [propagation evidence](../etc/evidence/m0-t445-s6-style-propagation.md).
S6 bounded tests remain valid only for their recorded inputs; delivery remains open.
S7 now verifies and publishes that retained work; S8 drawing is not admitted.

## Historical S3 To S4 Transition

The [S3 evidence](../etc/evidence/m0-t445-s3-style-experiment.md) retains that
historical transition. Native-menu session diagnosis now belongs to S8.

## T444 Final Disposition

S2 is owner-closed with no product modifications. Existing palette queries
do not imply universal terminal negotiation; environment mutation had no
demonstrated benefit, and the owner declined query-response expansion.
S3 r003 passes both x86/x64 producer matrices using production capture, packing,
Console projection and Window pixel rendering. Standard/custom indexed colours
are preserved; RGB/256 fallback is already present at the host API observation
boundary. No additional product defect is demonstrated and no runtime binary
has changed. r004 confirms all ten deployed/assets/tested images match, the
49-check retained palette regression passes, and governance/links pass. S3
has complete bounded findings and was delivered at `2487156ea`. The owner
has now directed T444 closure; no implementation or acceptance gap remains
inside the agreed scope. Unsupported negotiation and independent RGB cells
remain explicit capability limits, not passing capabilities.
See
[S2/S3 evidence](../etc/evidence/m0-t444-s3-host-vt-fidelity.md).

## T444 S1 Owner Acceptance

On 2026-10-11 the owner accepted S1 and authorized S2. S1 is closed at
`fb7650ce0`, committed and pushed with a clean worktree before this admission.

The final 0.0.444 ten-image package is published to assets/release and
O:/winnt/system32, with all three copies equal to the tested manifest (r117).
r116 Full passes in 347,227 ms, including Console17, Window17, independent WOW
frontiers and all control gates; real WINMINE gameplay, native fixtures and
service29 also pass. r115 passes 49 palette checks and complete real-pipe
frontend fault/lifecycle coverage. Published COMMAND and CMD both pass
interactive VER/EXIT in r119. Review and documentation governance are recorded
in the linked evidence; owner acceptance is now recorded above.

Repairs are committed-palette projection and restoration, palette-only
geometry preservation, bounded native intermediate viewport, NTVWM keyboard
delivery independent of output snapshot stability, and the registered original
blocked-state guard before project-added video publication. No guest binaries,
frame/control ABI or lifecycle ownership change. SOL/WRITE's independently
matched historical error frontiers are not functional application passes. The large
inherited-environment failure matches the registered original COMMAND INIT
lifetime defect and is not counted as a pass. See
[S1 evidence](../etc/evidence/m0-t444-s1-native-indexed16-palette-projection.md).
The acceptance does not convert the explicitly retained guest/WOW limitations
into functional passes.

## Previous Closure

T443 is owner-closed. Its H01--H10 audit and unique H08 transfer are recorded
in the [T443 closure](../history/m0-t443-special-case-contract-closure.md).

## Previous Closed Packet

The retained T443 S3 packet is indexed in the
[S7 supporting record](../etc/evidence/m0-t445-s7-base-rpc-client-split.md#retained-prior-status-packet).

## T443 S3 closure / next-step disposition

The owner accepted S3 after the published nested `run16 command` repair.
The separately reported visible-Console-close
residual is not attributed to S3 and is now the first unadmitted candidate:
[borrowed Console close lifecycle recovery](../proposals/proposal-borrowed-console-close-lifecycle-recovery-001.md).

## S12 Closure Record

See [S12 COMMAND service Run16 routing closure](../history/m0-t443-s12-command-service-run16-routing-closure.md)
and its [evidence](../etc/evidence/m0-t443-s12-command-service-run16-routing.md).

## S13 Closure Record

No source change was required: the native `NtAlertThread` path and the
existing APC wake-status handling already provide the applicable contract.
The absent-export fallback is retained as a bounded limitation; see the
[S13 disposition](../etc/evidence/m0-t443-s13-ntalertthread-disposition.md).

## T442 S1 Delivery Note

T442's borrowed-Console lifecycle repair was published at `44c25e138`.
NTSRV now owns initial external-member exit waits and frontend closing; NTCON
no longer owns local anchor retirement. Its visible-Console close observation
remains an owner acceptance check, not a reason to keep a second active packet.

## Previously Closed Work

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T438 S2`, Ordinary Mode; closed tool-boundary refactor after S1's recoverable baseline. |
| Admission And Approval | Owner further directs: use CMD as far as practical for INI discovery, backup and rewrite; establish `GRP.EXE` for Program Manager `.GRP` files; use `PIF.EXE` for discovered PIF root fields; no `HASH.EXE` dependency. |
| Objective | Separate Win3.1 path repair into transparent CMD orchestration, structured `.GRP` operations in `GRP.EXE`, and structured PIF operations in `PIF.EXE`, retaining adjacent-backup recovery. |
| Non-goals | No guest binary or host-product change, no Setup repair, no general registry/INI editor, and no release-hash validation of path-dependent files. |
| Reference Baseline | T438 S1 adjacent backup/recovery proof and the existing conservative discovery/PMCC parser. |
| Files And ABI Surface | `tools/win31-path`, `src/addon/grp`, `src/addon/pif`, released add-on utilities if their ABI is completed, focused tests, proposal/history/evidence. |
| Applicable Rules | Execution, architecture, coding, documentation, and source-policy authorities; owner files must never be overwritten before a `.BAK` exists. |
| Verification | Build each AMD64 utility; build-owned fixture proofs for INI, `.GRP`, PIF, repeat Apply and recovery; documentation governance and diff review. |
| Expected Markers | Apply displays old/new roots, names each changed INI/PIF/GRP, creates only adjacent backups, and Unapply restores its manifest list without a hash tool. |
| Asset Needs | Existing AMD64 PIF source/release and copied fixtures only; `GRP.EXE` is new project-authored utility source. |
| Reporting Requirements | Record exact formats and fields handled, CMD/text limitations, recovery behavior, test results and any deliberately unsupported record. |
| Stop Conditions | An unknown PIF/GRP layout, a need for a guest delta, an INI encoding/format unsafe for CMD preservation, or need for a broad rewrite-engine redesign. |
| Exit Criteria | Met: source-backed format audit, bounded utility split, focused tests, governance/diff review, commit, and push. |
| Original Owner Request | “尽可能采用cmd脚本来修改ini文件里的路径…对于GRP文件和PIF文件等，你应该采用PIF.EXE工具和 GRP.EXE工具。” |
| Similar-Issue Sweep | Root INIs, root `.GRP`, root `.PIF`, existing PIF parsing/update behavior, prior path helper, Apply/Unapply and add-on manifest consumers. |

## S11 Closure Record

`win31-launch` now preserves `KRNL386.EXE`, `WIN386.EXE`, and `MOUSE.DRV` as
adjacent `SYSTEM\*.BAK` files. `UNAPPLY.CMD` authenticates the active project
files before restoring those backups, and a later Apply migrates compatible
legacy `PATCH\*.ORIG` copies. First/repeat Apply, migration, profile, and
Unapply fixture coverage passed. See
[S11 evidence](../etc/evidence/m0-t437-s11-win31-adjacent-backups.md).

## S10 Closure Record

`win31-launch` had incorrectly treated `MOUSE.DRV` as an installed-root file.
The tool now uses the standard `SYSTEM\MOUSE.DRV` location for all preflight,
identity, backup, and replacement operations, while retaining its recovery
copy under `PATCH`. A focused command-contract test passed. See
[S10 evidence](../etc/evidence/m0-t437-s10-win31-system-mouse-path.md).

## S9 Closure Record

`tools/win101-setup` no longer contains `MOUSE.DRV`. Its APPLY and UNAPPLY
paths authenticate and consume only `assets/release/MOUSE101.DRV`; the media
PATCH remains the four runtime helpers only. The focused apply/recovery/setup
fixture passed with positive, repeat, failure, and recovery coverage. See
[S9 evidence](../etc/evidence/m0-t437-s9-win101-release-driver-source.md).

## S8 Closure Record

### Released add-on utility consolidation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T437 S8`, Ordinary Mode; owner-approved follow-up to S7. |
| Candidate Proposal | [Installed Windows 3.1 portable launch repair](../proposals/proposal-win31-installed-launch-portability-001.md). |
| Admission And Approval | Owner directs: tools must not carry duplicate `PIF.EXE`, `HASH.EXE`, or `MOUSE31.DRV`; use `assets/release`, keep CMD as the orchestrator, make `PATCH386.EXE` only patch `KRNL386.EXE`/`WIN386.EXE`, and keep backup/configuration/mouse work in `APPLY.CMD`. |
| Objective | Make `win101-setup` consume/redeploy the released utility originals and make `win31-launch` consume released utilities/driver while separating its binary transform from CMD orchestration. |
| Non-goals | No new guest binary delta, Setup-transition recovery, path-repair change, host-product change, or alteration to PIF semantics. |
| Reference Baseline | S7's released `PIF.EXE`/`HASH.EXE`, T437's installed-tree profiles, and the two approved identity-bound Win3.1 candidate hashes. |
| Files And ABI Surface | `tools/win101-setup`, `tools/win31-launch`, `src/addon/win31-launch`, focused component tests, release/add-on documentation and evidence only. |
| Applicable Rules | Execution, coding, documentation, source-policy guest exception, and existing add-on release-manifest rules. |
| Verification | Rebuild the dedicated patch helper; run focused Win1.01 CMD orchestration and Win3.1 launch-profile tests against release artifacts; review file ownership and no-duplicate payload assertions. |
| Expected Markers | Tool directories contain no duplicated generic utilities/Win31 driver; Win1.01 media PATCH receives released PIF/HASH copies; Win3.1 APPLY leaves recoverable originals and valid standard/386 profiles. |
| Asset Needs | Existing released PIF/HASH/MOUSE101/MOUSE31 assets and build-owned fixtures only. |
| Reporting Requirements | Record exact release identities, backup/recovery behavior, package file lists, focused results, and retained Win3.1 enhanced-mode limitation. |
| Stop Conditions | Unknown guest image identity, a needed guest delta outside the two approved candidates, release-manifest mismatch, or a need to alter Setup/runtime behavior. |
| Exit Criteria | Met: focused positive/negative tests, release identity checks, documentation governance/diff review, committed and pushed delivery. |
| Original Owner Request | “tools/*/里面不要有hash.exe和pif.exe… tools/win31-launch同理。patch386.exe 用来负责给 krnl386.exe和win386.exe打补丁；apply.cmd负责备份这些文件。鼠标驱动也有apply.cmd处理。本目录底下也不要重复保存mouse31.drv，也是去assets/release里面拿。” |
| Similar-Issue Sweep | Check Win1.01 APPLY/UNAPPLY, both generated Win3.1 profiles, existing add-on manifest consumers, and tool documentation. |

See [S8 evidence](../etc/evidence/m0-t437-s8-addon-utility-consolidation.md).

## S7 Delivery Packet

### Win1.01 Setup completion receipt

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T437 S7`, Ordinary Mode; reopening after S6 closure. |
| Candidate Proposal | [Windows 1.01 EGA/InPort runtime](../proposals/proposal-windows-101-ega-inport-runtime-001.md) — bounded Setup-return repair. |
| Admission And Approval | Owner reports original Setup appears to close but `PATCH\SETUP.CMD` remains blocked until Ctrl+C; owner requests an S repair. |
| Objective | Make an ordinary Win1.01 Setup PIF return through the existing run16/NTSRV/NTVDM completion contract after its guest process exits. |
| Non-goals | Do not alter Setup guest media, treat Ctrl+C as success, change unrelated PIF/DOS completion semantics, or modify Win3.1 packaging. |
| Reference Baseline | S5 media-root driver replacement and S6 reversible recovery; existing `SETUP.CMD` calls `run16` synchronously before installed-profile creation. |
| Files And ABI Surface | Initially diagnostic scripts/evidence; any later product caller/worker/service change requires narrowed source ownership and runtime/publication gates. |
| Applicable Rules | Execution, architecture, coding, documentation and source-policy authorities; normal run16 receipt semantics remain authoritative. |
| Verification | Reproduce with an owned prepared fixture or retained field evidence; prove guest exit, receipt delivery, `run16` return, post-Setup profile prompt and no regression in a normal DOS/PIF wait. |
| Expected Markers | One direct request receives a matching terminal receipt; `SETUP.CMD` prints `Original Setup returned` without Ctrl+C and reaches its prompt. |
| Asset Needs | Existing tools, selected media only with owner authorization, and build-scoped logs/fixtures. |
| Reporting Requirements | Record exact reproduction, process/receipt state, root cause, source ownership, focused positive/negative result and any retained manual boundary. |
| Stop Conditions | Pause for guest-media modification, an unproven cross-component lifecycle redesign, inability to reproduce, or a cause outside project-owned code without an admitted repair route. |
| Exit Criteria | Source-backed diagnosis, minimal repair if project-owned, focused tests, required product gate if production code changes, governance/diff review, commit and push. |
| Original Owner Request | “用apply.cmd跑完了安装流程，安装程序结束之后它好像自己卡住了，我按了ctrl+c才显示出来” and “请你准入一个S修复”. |
| Similar-Issue Sweep | Check ordinary DOS/PIF completion and post-exit receipt handling without expanding into the deferred Win3.1 ordinary-Setup transition. |

## T437 S1 Closure Record

See [S1 evidence](../etc/evidence/m0-t437-s1-installed-win31-tools.md).

## S2 Closure Record

See [S2 evidence](../etc/evidence/m0-t437-s2-single-tool-entrypoint.md).

## S3 Closure Record

See [S3 evidence](../etc/evidence/m0-t437-s3-pif-hash-utilities.md).

## S4 Closure Record

See [S4 evidence](../etc/evidence/m0-t437-s4-win101-cmd-orchestration.md).

## S5 Closure Record

See [S5 evidence](../etc/evidence/m0-t437-s5-win101-reversible-media-mouse.md).

## S6 Closure Record

See [S6 evidence](../etc/evidence/m0-t437-s6-win101-media-unapply.md).

## Current Technical Baseline

The current deployed ten-image package is the owner-accepted S1 release
0.0.444 at `fb7650ce0`, protocol 45, including the NTCON palette and NTVDM
blocked-tick repairs. `assets/release/release-manifest.json` records its
identities; S3 r004 verified all ten against the deployed and tested copies.
S2/S3 and this closure changed no product binaries. Guest binaries and default
configuration remain unchanged.
T436's KRNL386/WIN386 candidates and MOUSE31.DRV remain separate approved add-on
assets, not inputs altered by this frontend/test recovery packet.

| Architecture | Images |
| --- | --- |
| AMD64 | `run16.exe`, `ntsrv.exe`, `ntcon.exe`, `ntvwm.exe`, `ntmon.exe`, `nthook64.dll` |
| I386 | `ntvdm.exe`, `WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll` |

## S7 Closure Record

Source review isolates the hold to the original PIF `CloseOnExit=0` inactive
session path, not guest Setup completion. The project-owned PIF creator now
encodes explicit `CloseOnExit` for every tool-generated PIF whose paired
command file waits synchronously, including Win1.01 `WIN101.PIF` and Win3.1
standard/enhanced launch profiles; generic user PIF creation remains
default-clear. Focused utility/package checks pass, and the owner verified the
repaired installed Win1.01 launch path. See the [S7 evidence](../etc/evidence/m0-t437-s7-win101-setup-close-on-exit.md).

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T444 | Owner-accepted indexed palette repair, audit-only capability disposition and measured host VT/API fidelity. [Closure](../history/m0-t444-indexed16-text-closure.md) |
| T441 | Repaired the mixed NTVDM/NTVWM release pair that stalled `COMMAND → ver`; bounded variable worker I/O is retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Retained verified graphics/transport/idle/build improvements; global CCPU pacing withdrawn without a measured UX gain; active root-cause work deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible installed Win3.1 path repair and CMD/PIF/GRP ownership split. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`; T437 is closed after its portable installed-tree
tool delivery. Its ordinary-Setup transition recovery remains an unadmitted
queue-tail candidate.
