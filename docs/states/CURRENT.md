# Project Status

## Current Work

**No active M/T/S packet.** The focused T440/S8 checkpoint below is closed;
the next timing-policy packet requires separate admission.

## Latest Closure

### M0 T440 S8 — Win3.1 desktop CPU attribution after idle-HLT repair

S8 removes the project-added disabled SAS-store environment lookup and replaces
only the full-overwrite XMS forward-copy bridge with the original numeric CCPU
forward move. It retains S7's event-driven idle HLT repair (`MVDM-HOST-DIV-330`).
Fresh x86 `r028-close` built `ntvdm.exe` (`B5217F3D…2229B4`) and passed the CCPU
HLT/reset, CCPU throughput, original external-memory and mapped-XMS fixtures.
The identical NTVDM was published with a recoverable predecessor under
`O:\winnt\builds`; all ten release-manifest identities were checked. The
normal full product matrix was not run because its only runner creates `Z:` via
`subst`, which the owner has prohibited. A pipe-hosted `COMMAND.COM /c ver`
smoke returned 87 and is recorded as an unsuitable noninteractive witness, not
as a product pass. S8's evidence identifies the next independent problem: the
CCPU quick-event clock assumes one instruction per microsecond. See
[S8 evidence](../etc/evidence/m0-t440-s8-win31-xms-attribution.md).

## S6 Closure Record

## S7 Bounded Conclusion

S7 retains a minimal `MVDM-HOST-DIV-330` event wait at CCPU's original HLT
boundary. The focused old/new carrier comparison reduces a 500 ms steady idle
window from 0.515625 CPU seconds to zero while preserving original reset-vector
delivery. A real Win3.1 desktop diagnostic remained active-guest CPU work, so
S7 does not claim a desktop-throughput improvement and defers its product P
until the active-cost attribution is complete. See
[S7 evidence](../etc/evidence/m0-t440-s7-ccpu-hlt-event-wait.md).

S6 found that S5's variable 1 MiB-bounded worker records removed the earlier
graphics-transfer bottleneck: current 640x480 payload/dispatch p50 is 0.405
ms. A retained current Win3.1 Standard worker instead used 17.45 CPU seconds
over an 18-second wall interval, while ordinary COMMAND idle used no measurable
CPU in its separate sample. Source review isolates the reached Win3.1 cost to
CCPU's ring-0 `HLT` polling loop. The physical desktop witness remains owner
facing; this attribution did not claim a new graphical acceptance result. See
[S6 evidence](../etc/evidence/m0-t440-s6-post-s5-responsiveness-attribution.md).

## S4 Closure Record

S4 records a quantified attribution without modifying product behavior.  The
fresh current-source 640x480 renderer p50 is 0.382 ms, while the accepted
real Window EDIT trace measures 86.845 ms p50 through the synchronous
worker-to-NTCON transfer boundary for a roughly 20 KiB text frame.  Current
source requires three request/reply exchanges for that text frame and twenty
for a 307,200 B 640x480 DIB.  Renderer/decode/assembly are excluded as the
primary cause; mouse-IRQ consumption has a retained 97.950 ms tail.  The
current private-desktop observer could not form a current native-frontend
Window witness, so historical end-to-end percentiles remain explicitly
labelled rather than projected as a current graphics measurement.  See
[S4 evidence](../etc/evidence/m0-t440-s4-window-performance-attribution.md).

## S5 Closure Record

S5 replaces the fixed 16 KiB direct worker/NTCON payload array with bounded
variable-length records (1 MiB maximum actual payload). A normal 640x480 VGA
frame now crosses as `VIDEO_BEGIN` plus one payload record; larger frames
remain ordered bounded parts with one final reply. During release validation,
palette-indexed graphics exposed a latent publication race: capture before the
guest palette install produced `ERROR_NOT_READY` and poisoned the publisher.
That state now remains dirty and is republished when the existing palette
install signals readiness. Focused framing, staging, palette-late graphics,
build and attached-console graphics smoke checks pass. A subsequent field
failure of DOS graphical launch was traced to stale `ntcon.exe` and `ntvdm.exe`
release artifacts, not WOW routing or guest graphics semantics: a clean
current-source relink of that pair restored both `MYSmb16` and `WINSTD` while
the recorded release pair returned immediately. The pair was rebuilt, release
manifest hashes refreshed, and published together to `O:/winnt/system32` and
`assets/release`; interactive Win3.1/Window acceptance remains owner-facing.
See [S5 evidence](../etc/evidence/m0-t440-s5-variable-worker-io.md).

## S1 Closure Record

S1 established a current, reproducible project-owned candidate without
changing production code.  The shared publisher's ordinary waitable timer
delivered roughly 31 ms p50 rather than its nominal 20 ms cadence; a test-only
high-resolution timer factory delivered roughly 21 ms under the same policy.
The worker mouse queue measured 9–14 ns per immediate pair and is excluded as
a visible-stall cause.  Private Win3.1 and non-console startup probes did not
produce a valid witness and remain explicitly unavailable.  The test harness,
evidence are continued sequentially in S2; no timer implementation delivery
is authorized.
See [S1 evidence](../etc/evidence/m0-t440-s1-publication-timing-baseline.md).

## S2 Closure Record

S2's source audit found no missing dirty-rectangle support in the shared
Window renderer: NTVDM64 and the SoftPC comparison project use the same
pixel-diff, damaged-rectangle invalidation path. The material difference is
upstream. NTVDM64 constructs and copies a complete DIB/frame before
worker-base can replace it with a later frame, whereas SoftPC rejects an
unchanged VGA generation before capture. The pure 640x480 Window renderer
measured roughly 0.55 ms median per frame; this is not an end-to-end Win3.1
witness. S3 owns only the resulting project-adapter repair.
See [S2 evidence](../etc/evidence/m0-t440-s2-graphics-chain-audit.md).

## S3 Closure Record

S3 moves project-owned NTVDM DIB construction from every original graphics
invalidation to worker-base's existing accepted publication boundary.  The
original SoftPC painter and guest media remain unchanged; a private
source-capture seam preserves existing 50 Hz policy, sender ownership and
final-drain ordering.  The focused publication and real graphics-source
fixtures pass, the x86 product relinks, and the verified ten-image package is
published to `O:/winnt/system32`.  The existing observer-only `87` startup
limitation occurs before graphics handoff and is retained rather than counted
as a pass.  See [S3 evidence](../etc/evidence/m0-t440-s3-source-side-graphics-publication.md).

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
| T438 | Reversible installed Win3.1 path repair and CMD/PIF/GRP ownership split. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`; T437 is closed after its portable installed-tree
tool delivery. Its ordinary-Setup transition recovery remains an unadmitted
queue-tail candidate.
