# Project Status

**Active: M0 T445 S3**

M0 T445 S2 closed at c12090805. Owner admits S3's isolated UI experiment.
Product propagation and integration remain S4--S7.
See the [S1 closure design](../etc/evidence/m0-t445-s1-installation-design.md).

## Active Packet

### Isolated Win16-style component and native GUI experiment

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T445 S3, Ordinary Mode; implementation then separate final review. |
| Admission And Approval | Owner requests the independent Win32 experiment and gallery, then authorizes computer-use observation/interaction of that demo to reproduce native caption-button leakage missed by probes. |
| Objective | Build a reusable local frame library and directly launched GUI demonstration, without product integration. |
| Non-goals | No run16 options, Hook/WOW integration, NTCON changes, guest/mirror edits, runtime component or global theme/settings. Menu/MDI retro rendering remains later work. |
| Reference Baseline | c12090805, accepted S2 ten-image package; S1 original-reference research and installation design. |
| Files And ABI Surface | src/win16-style, tests/app and tests/observation; local same-thread C API, x86/x64 /MT static archives. |
| Applicable Rules | Source/architecture/coding/document authorities; background-owned build/tests; build-only outputs; no subst. Computer use is scoped to the owned demo, not other applications or host settings. |
| Verification | Actual HWND gallery: button combinations, caption/system-menu absence, fixed/sizeable/dialog/borderless frames and state changes; separately labelled native menu/MDI/child references. Test system-menu close and disabled close, layout/hits, geometry preservation and lifetime; owned probe captures. |
| Expected Markers | Both-width runnable demo and tests with images; no link to other product components. |
| Asset Needs | Pinned VGA.DRV button resources; original media stays read-only. Owner now selects host System font; embedded VGASYS.FON and private font loading have been removed. |
| Reporting Requirements | Exact binary paths, screenshots, measured test results and visual/physical interaction limitations. |
| Stop Conditions | Need for product/guest integration, global interception/settings or unreviewed resources; no scope expansion. |
| Exit Criteria | Focused tests and reviewed probe images, governance/link/diff checks, commit/push; owner visual stability acceptance before S4. No production release replacement for this isolated experiment. |
| Original Owner Request | Admit S3, implement component and independent Win32 trial; show progress using probe screenshots; no computer use. |
| Similar-Issue Sweep | Missing buttons, title changes, install/remove/destroy, unsupported frames, DPI/high contrast, pressed/canceled actions and application client preservation. |

## S1 Closure Record

T445 S1 closed at 2151cf750. Its [design closure](../etc/evidence/m0-t445-s1-installation-design.md)
and [retained packet](../etc/evidence/m0-t445-s1-closed-packet.md) preserve the
source-backed design and original brief. Stage numbers in that evidence precede
the owner-inserted standalone S2; current numbering is in the working plan.

## S2 Closure Record

Owner-accepted palette repair closes with this commit/push delivery.
See [repair and verification evidence](../etc/evidence/m0-t445-s2-palette-repair.md).
The standalone experiment is admitted as S3 by the next owner request.

## Planned Sequence

S1 is closed. S2 delivers the accepted palette repair. S3 is the isolated UI
experiment; S4 is CLI/policy propagation and title-marker verification; S5
integrates the common frame; S6 covers menus/MDI; S7 is integrated acceptance.
The [working plan](../etc/operations/t445-win31-window-style-plan.md) defines
the contracts. S3 is active; S4 waits for experiment stability.
The shared source owner remains src/win16-style, with no production integration
during S3.

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

Owner accepted r070's native hover/static-residue repairs. NC activation and
cursor painting are custom-owned; minimized presentation remains native.
The 19-entry gallery retains bitmap/font, menu/disabled-close and lifetime
tests. Menu/MDI/child/scrollbar samples remain labelled native references.
Detailed runs and the original residue negative control are retained below.
Overall S3 acceptance and S4 admission wait.
See [S3 experiment evidence](../etc/evidence/m0-t445-s3-style-experiment.md).
No production images or deployed components changed.

Owner's paired normal/selected references resolve the system box as palette-role
inversion, not an arrow-style bevel. r083 implements that state, original 19x18
button geometry and rounded stroke widths; both widths pass 13,885 checks. Private
desktop notification/pixel checks do not certify physical menu tracking.
No whole-window pixel-equivalence claim is made.

Host System font and release-triggered dialog menu were delivered at 879a8584f.
Owner confirms menu persistence. r119 actual input repairs gallery-12 double
click through a scoped menu-loop queue binding; r120 passes 27,220 checks and
18 queued menu cases per width. Blank initial items remain unresolved.
r105/r106 reproduce blank system-menu items in an independent native executable
with no style library. Per-popup TPM_NOANIMATION restores text; system-DPI and
v6-manifest controls do not. r122 now reproduces native File-menu blankness too:
both widths remain blank after 3 seconds; per-popup no-animation draws normally.
r134 both-width native probes localize animation WM_PRINT failure before client
drawing (GreSaveDC abort); underlying DC-state cause remains unproven. S3 is open.

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

### COMMAND interpreter/target/tail contract recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T443 S3`, Ordinary Mode. |
| Admission And Approval | Owner directs “准入 S3，先解释问题和根源”. S2's shared-source Hook correction is complete in the working packet: both Hook bitnesses built and the focused installation suite passed three consecutive times with 104 assertions. S3 is the next coupled H01--H03 repair; Windows 3.1 window appearance remains the queue head. |
| Objective | Preserve the complete original `COMSPEC /c <tail>` command that COMMAND has already assembled, but create it through run16/NTVWM so the host COMSPEC is Hooked. Remove the project-owned `|&<>`, bare `COMMAND.COM /c`, and three-argv quote-rebuild heuristics without taking shell grammar from COMMAND or host CMD. |
| Non-goals | No guest/media mutation, generic Windows command-line parser, filename alias list, new shell/helper/scheduler/protocol, broad H04 fallback change, change to Hook identity behavior, or replacement of a `COMSPEC /c` tail by a new run16 shell fallback. |
| Reference Baseline | [T443 S1 ledger](../etc/evidence/m0-t443-s1-special-case-contract-ledger.md), original `cmdExec` in `src/mvdm/dos/command/cmdexec.c` (its `AH=1` branch builds `COMSPEC /c <tail>`), `command_process_compat.c`, `run16/main.c`, and the focused S2 Hook result. |
| Files And ABI Surface | Expected owner files are `src/ntvdm-exe/win32/command_process_compat.c`, `src/run16-exe/main.c`, their focused tests/evidence, and required governance records. No public protocol or guest ABI change is admitted. |
| Applicable Rules | Execution, architecture, coding, documentation and source-policy authorities; original COMMAND creates the full host `COMSPEC /c` command, while host CMD remains the sole owner of its quoting, pipe, redirection, batch and built-in grammar. |
| Verification | Focused build and component checks now pass; release identity is verified against all ten deployed images. The headless Console observer returns the same pre-input `ERROR_INVALID_PARAMETER` for both candidate and HEAD, so it is recorded as an unsuitable interactive witness rather than a false product regression. Owner acceptance remains `run16 command` with built-in, pipe/redirection, and nested return in a real Windows Terminal Console. See [S3 evidence](../etc/evidence/m0-t443-s3-command-comspec-contract.md). |
| Expected Markers | No route decision is based merely on a metacharacter scan or bare basename; the original full `COMSPEC /c <tail>` reaches run16 unchanged and NTVWM injects Hook before that COMSPEC resumes. |
| Asset Needs | Current ten-image release, source/build-owned command fixtures and existing original/ledger evidence only. |
| Reporting Requirements | Record current call graph, each removed/retained decision, original/provenance basis, positive and negative witnesses, both Console/Window regression scope, and release boundary. |
| Stop Conditions | The only apparent solution needs a general parser, changes original/mirror COMMAND code, requires new IPC/protocol, cannot preserve a nested or composite witness, or reveals that H04's error taxonomy must change; pause for re-admission rather than broaden S3. |
| Exit Criteria | Met: source-backed contract and minimal owner-side repair are complete; focused build/component checks and ten-image release identity passed, and the owner accepted the real-Console COMMAND matrix on 2026-10-10. |
| Original Owner Request | “准入s3 先解释问题和根源”. |
| Similar-Issue Sweep | All project-owned COMMAND target/tail identity checks in the NTVDM adapter and run16, including quoted transport only; exclude H04 fallback and Hook H06 parser work unless the stop condition is met. |

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
