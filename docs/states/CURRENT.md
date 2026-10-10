# Project Status

## Current Work

**Active: M0 T442 S1**

## Active Packet

### Native CUI Console-window virtualization design

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T442 S1`, Ordinary Mode. |
| Admission And Approval | Owner admits the queue-head Native CUI Console-window virtualization package and directs completion of its design before implementation. |
| Objective | Establish the smallest authenticated control path which maps a native CUI target's proven request to show/foreground its inherited carrier Console onto the already-bound NTCON Console projection, without exposing the NTVWM carrier. |
| Non-goals | No application-name exception, CUI-to-GUI reclassification, target stream/Console-identity emulation, new persistent helper/registry/polling loop, task/process observation, guest/media change, or unrelated lifecycle redesign. |
| Reference Baseline | [Candidate proposal](../proposals/proposal-native-cui-console-window-virtualization-001.md), the matching T441 worker release, current `nthook32-dll` CreateProcess-only interception, NTSRV-owned frontend routes, and NTCON's existing Console/Window selector. |
| Files And ABI Surface | Design/evidence under `docs/etc/operations` and `docs/etc/evidence`; expected later ownership is `src/nthook32-dll`, `src/common` control declarations, NTSRV service/RPC bindings, NTVWM, NTCON's existing presentation operation, affected tests and release manifest. |
| Applicable Rules | Execution, architecture, coding, documentation and source-policy authorities; NTSRV remains lifecycle/relationship authority; NTCON remains worker-neutral; hidden Console carrier remains private. |
| Verification | Source-map the existing carrier, target hook bootstrap, service route, worker event and frontend selector; capture the actual target API sequence before selecting a hooked API; document positive, unrelated-HWND, explicit new-Console, failure and direct-CMD controls. |
| Expected Markers | The design names one authenticated intent record/event, one service validation point, one worker consumption point and the existing NTCON display selection; it distinguishes visibility from `FreeConsole`/`AllocConsole`/`AttachConsole`. |
| Asset Needs | Current released package, owner-supplied `mysmb64.exe` reproduction and build-owned trace/test material only. |
| Reporting Requirements | Record API trace evidence, exact validated identity predicates, state/event ordering, failure passthrough, width coverage, test matrix and any unsupported identity-changing transition. |
| Stop Conditions | The target allocates, frees or attaches a distinct Console; a required hook cannot identify the current Console HWND; a design needs direct hook-to-NTCON control, broad HWND interception, new process tracking, or an unproved lifecycle change. |
| Exit Criteria | A source-backed, implementable S1 design with the exact trace gate and no unresolved control-owner ambiguity; governance/diff review and a committed design delivery. |
| Original Owner Request | “准入t任务并完成设计”。 |
| Similar-Issue Sweep | Existing direct CMD behavior, native GUI route, x86/x64 hook bootstrap, nested native CUI children, NTSRV frontend route identity, NTCON Console/Window selection, teardown and target completion. |

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

The current ten-image release remains unchanged. T436 supplied approved,
identity-bound `KRNL386` and `WIN386` candidates plus released `MOUSE31.DRV`.
This task consumes those assets only for an existing, recoverable installed
Windows 3.1 tree; it does not publish a host release merely for tooling work.

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
