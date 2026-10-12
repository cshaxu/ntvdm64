# Project Status

**Active: M0 T444 S1**

## Active Packet

### Native indexed-16 palette projection

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T444 S1`, Ordinary Mode. |
| Candidate Proposal | [Native indexed-16 text contract](../proposals/proposal-native-indexed16-text-contract-001.md). |
| Admission And Approval | Owner directs: “队列第2条已经修复可以剔除。现在准入队列第一条”; subsequent standing goal: “把当前项目所有该通过的测试都通过，包括wow”. The proved EDIT/MEM software-publication regression is included in the required delivery-gate repair, without changing lifecycle ownership. |
| Objective | Prove and, if confirmed, repair NTCON Console projection so a committed text frame's 16-entry RGB palette is applied atomically with its indexed cell attributes, without changing worker identity or the frame protocol. |
| Non-goals | No capability-environment work (S2), VT/RGB emulation (S3), guest change, worker-type branch, polling/helper/lifecycle redesign, or arbitrary per-cell RGB support. |
| Reference Baseline | The queued proposal's static review: NTVWM captures/transmits `CONSOLE_SCREEN_BUFFER_INFOEX` palette and Window decoding retains it, while Console projection reportedly copies attributes without palette application. T442 borrowed-Console lifecycle is delivered at `44c25e138` and is removed from the queue by owner direction. |
| Files And ABI Surface | NTCON Console projection/session code and focused palette fixtures; required-gate repairs include the original-blocked-state guard in nt_graph.c and the r095-proved NTVWM input/output-snapshot coupling. Keep their focused regressions and mirror registration where applicable. Observation harness repairs, evidence and shared application-version synchronization are included. Existing frame and control ABIs remain unchanged. |
| Applicable Rules | Execution, architecture, coding, document and source-provenance authorities; all build/test runs background-owned; `subst` prohibited. |
| Verification | First audit current palette capture, commit, handoff and restoration owners. If production repair is admitted by evidence, build the affected closure and verify non-default indexed colours, palette-only update, failure/rollback, Console/Window switching, DOS/native handoff and borrowed palette restoration. |
| Expected Markers | The visible Console palette equals the committed frame palette whenever that frame's indexed attributes become visible; no mixed cells/palette, stale handoff palette or caller-palette leak remains. |
| Asset Needs | Current ten-image release, build-owned native palette fixture and existing Console/Window harnesses only. |
| Reporting Requirements | Separate static fact, focused fixture and real presentation evidence; record exact palette values, owner/rollback behavior, limitations and any required re-admission. |
| Stop Conditions | A repair needs a frame ABI change, worker-type conditional, new RGB/VT conversion engine, guest alteration, or reveals missing restoration ownership. Pause rather than broaden. |
| Exit Criteria | Automated criteria met by r115/r116, coherent ten-image publication verified by r117, published interactive smoke by r119. Owner acceptance remains pending; S2 is not admitted. |
| Original Owner Request | “队列第2条已经修复可以剔除。现在准入队列第一条”。 |
| Similar-Issue Sweep | Both Console and Window presentation routes; palette-only frames, owner handoff/final frame, worker-neutral callers and borrowed Console restoration. |

## Current Work

The final 0.0.444 ten-image package is published to assets/release and
O:/winnt/system32, with all three copies equal to the tested manifest (r117).
r116 Full passes in 347,227 ms, including Console17, Window17, independent WOW
frontiers and all control gates; real WINMINE gameplay, native fixtures and
service29 also pass. r115 passes 49 palette checks and complete real-pipe
frontend fault/lifecycle coverage. Published COMMAND and CMD both pass
interactive VER/EXIT in r119. Review and documentation governance are recorded
in the linked evidence; owner acceptance remains pending.

Repairs are committed-palette projection and restoration, palette-only
geometry preservation, bounded native intermediate viewport, NTVWM keyboard
delivery independent of output snapshot stability, and the registered original
blocked-state guard before project-added video publication. No guest binaries,
frame/control ABI or lifecycle ownership change. SOL/WRITE's independently
matched historical error frontiers are not functional application passes. The large
inherited-environment failure matches the registered original COMMAND INIT
lifetime defect and is not counted as a pass. See
[S1 evidence](../etc/evidence/m0-t444-s1-native-indexed16-palette-projection.md).
S1 awaits owner acceptance; S2 is not admitted.

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

## S1 Closure Record

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

The current deployed ten-image package contains the S1 NTCON palette repair;
`assets/release/release-manifest.json` records its identities. The additional
NTVDM blocked-tick repair is tested in the isolated r072 candidate package but
is not yet published. Guest binaries and default configuration remain unchanged.
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
| T441 | Repaired the mixed NTVDM/NTVWM release pair that stalled `COMMAND → ver`; bounded variable worker I/O is retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Retained verified graphics/transport/idle/build improvements; global CCPU pacing withdrawn without a measured UX gain; active root-cause work deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible installed Win3.1 path repair and CMD/PIF/GRP ownership split. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`; T437 is closed after its portable installed-tree
tool delivery. Its ordinary-Setup transition recovery remains an unadmitted
queue-tail candidate.
