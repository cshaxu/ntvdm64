# Project Status

## Current Work

**Active: M0 T460 S3**

Automatic sequential S closure/advancement is approved. Completed work follows
the read-only `ntvdm64-wow32-audit`: recover selected original behavior with
the smallest source-shaped binding and no guest modification or rebuild.

Owner direction on 2026-10-25 additionally authorizes the standalone Video7
preserve-mode source correction, matching SoftPC T70 S10. It is limited to
normalizing the BIOS mode before validation in `v7_video.c`, with no guest
change or Win95-specific branch. The source register is `src/mvdm/README.md`;
`tests/observation/verify-video7-mode-flags.ps1` owns the focused BIOS check.
This source integration does not close S3 or claim coherent publication.
Owner-requested test publication on 2026-10-25 subsequently rebuilt/staged
the ten-image package, passed the 32-setting Video7 BIOS regression and
verified release/installed manifest identities. Publication/rollback records
are in `O:/WINNT/logs/video7-owner-publication`; S3 acceptance remains open.
Owner subsequently reported doubled-width/black Win95 output. Isolated host
DIB capture reproduces 1280x480; default 16-colour configuration was restored.
Owner explicitly authorizes SoftPC packed-surface fixes in `nt_graph.c` and
`nt_vga.c`; `src/mvdm/README.md` records their source and actual-output gate.
The two-file candidate passes actual painter/baseline-control and real Win95
640x480 host-DIB checks; test publication is in `O:/WINNT/logs/video7-packed-surface`.
Temporary size exception retains concurrent S3 evidence until closure compaction.
Owner separately authorizes `src/addon/win95-compat-sys` on 2026-10-25: retain the
installed compatibility driver, add opt-in guest boot-logo display/handoff,
and provide source/build instructions. No core guest/host change or S3 closure.
Disposable output: `build/t460-s3/win95comp-logo-owner` (45 minutes, 2 GiB,
10-minute no-progress budget); supplied IO.SYS and original image stay read-only.
The add-on is now `compat.asm` -> `compat95.sys`; both mouse sources are
`mouse.asm`, retaining MOUSE101.DRV/MOUSE31.DRV outputs. Four isolated logo
cases pass (enabled/missing/invalid/disabled), including exact image DIB and
Explorer desktop handoff. Released/installed identities and recoverable
configuration backup are under `O:/WINNT/logs/win95-compat-logo` and
`O:/WINNT/builds/pre-compat95-logo-20261025`; this does not close S3.
Owner then requests original boot-bar animation in the same add-on. Reuse the
same disposable root/budgets for source-bound IO.SYS contract research and
four-case DAC-phase/desktop-handoff verification; no core/runtime change.
Animation now passes all four cases, including three real DAC phases and
51 frozen post-handoff samples. The 4856-byte `compat95.sys` is published and
installed; results and static-version rollback are under
`O:/WINNT/logs/win95-compat-animation` and
`O:/WINNT/builds/pre-compat95-animation-20261025`. No S3 closure is claimed.
Owner separately requests a reusable LNK.EXE add-on and repair of the installed
Win95 menu/shortcut relocation. Admit `src/addon/lnk`, format tests, shared
add-on publication and recoverable installed-menu preparation; no host/guest
core change or original-image write. Disposable root
`build/t460-s3/win95-menu-owner`: 60 minutes, 2 GiB, 10-minute no-progress budget.
LNK.EXE is published/installed; 31 menu entries and 26 exact application copies
have rollback at `O:/WINNT/builds/pre-win95-startmenu-20261025`. Original Win95
SHELL32 reads all 30 installed LNK files with zero missing targets; three canary
targets and a CP936 filename also pass. This is a menu path-resolution pass
only: separate program launch returned error 8 and remains unresolved.
Evidence is retained under `O:/WINNT/logs/win95-startmenu`; the supplied image,
original extracted trees and core binaries stay unchanged.
Owner additionally requests audit and joint submission of all new add-on and
launch-tool sources. [The add-on audit](../etc/evidence/m0-t460-s3-addon-tool-audit.md)
records provenance, release identities and installation/recovery checks.
Clipboard callback source tracing identified twelve stray WORD arguments in
the independently authored probe's zero-argument Close/EmptyClipboard calls.
Removing them passes owner-destroy callback return and all eighteen shared
Clipboard transfer/deferred/replacement phases; the guarded version also
passes. [The S3 evidence](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md)
supersedes the unproved continuation-failure hypothesis. Final coherent-package
acceptance, review and joint submission remain open.
The independent add-on/tool audit forms a bounded progress delivery;
installer journal-failure rollback passes. Core candidate r2 passes callback,
PIC/keyboard, DOS and message checks; complete shared Clipboard reruns hit
external OpenClipboard denial. Core publication/S3 closure remain open.
## Active Packet

### M0 T460 S3 — delayed rendering/loss, PIC-reset and keyboard-EOI repair

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T460 S3, Ordinary Mode; single-agent implementation and independent final review. |
| Admission And Approval | S2 transfer delivery is closed; automatic continuation under Queue W10 Clipboard proposal. Owner approved independent PIC-reset/EOI repair and keyboard early-EOI slot repair in this S on 2026-10-25. |
| Objective | Prove selected original Clipboard deferred callbacks, replacement and task-loss cleanup; restore source-proven device EOI reset cancellation and keyboard-byte retention when IRQ1 is acknowledged before port 60 is read. |
| Non-goals | No guest change/rebuild; no Clipboard manager/cache/listener/DDE substitute; no Win95 experiment/image, timer-reset, IRQ-injection, fabricated keyboard EOI/reinjection, driver change or command-specific change. |
| Reference Baseline | [S1 owner matrix](../etc/evidence/m0-t460-s1-clipboard-owner-matrix.md), [Clipboard proposal](../proposals/proposal-wow32-clipboard-001.md), W01/W02/W03/W07 records, [PIC-reset plan](../etc/evidence/m0-t460-s3-pic-eoi-reset-plan.md), read-only keyboard experiment evidence, and WOW32 minimum-diff governance. |
| Files And ABI Surface | Original `wuclip.c` callbacks and public USER/VDM/GDI boundary; original `ica.c` ICW1 reset, `nt_eoi.c` EOI hooks and device callback contracts; original `keyba.c` output slot and port 60/64 paths; immutable guest/native fixtures. |
| Applicable Rules | Applicable execution, architecture, coding, documentation, source-policy and WOW32 minimum-diff rules. |
| Verification | Clipboard matrix plus PIC baseline/fix/control, IRR/ISR/empty/master-slave/repeat-reset cases, early/normal IRQ1 ordering, port-64 non-consumption, direct controller replies, delayed IRQ and reset/resume cases; focused device callbacks, DOS timing/input/protected-mode and noninteractive WOW/Win3.x lifecycle routes; coherent package regression if production changes. |
| Expected Markers | Clipboard callbacks retire once; PIC reset retires only source-proven outstanding device ownership, with no stale EOI block, duplicate callback or cascade leak; keyboard output advances exactly once only after both data consumption and its normal PIC EOI. |
| Asset Needs | Existing immutable guest media, mirrors, fixtures and read-only PIC research evidence only; no Win95 image use or new guest artifact. |
| Reporting Requirements | Exact Clipboard, PIC and keyboard-slot source/ownership matrices, IRR-versus-ISR conclusion, device/cascade/result and pre-read-EOI result, failure disposition, production diff and package hashes or explicit no-change result. |
| Stop Conditions | Guest change; Clipboard substitute; Win95-specific change; source-less PIC cancellation; unproved ISR semantics; callback requeue/lock contract not established; keyboard change that treats status read as data consumption, duplicates an IRQ or loses direct controller reply semantics. |
| Exit Criteria | Clipboard paths meet their existing boundary; PIC repair has source-proven IRR/ISR semantics; and keyboard slot repair proves both normal and early-EOI orders with required noninteractive evidence, or each bounded path is retained as a documented candidate/non-pass. |
| Original Owner Request | 单人双角色模式复通 WOW32；新增 PIC 重新初始化 EOI 通用兼容修复；源码追溯优先、最少 diff、禁止 guest/Win95 实验混入。 |
| Similar-Issue Sweep | Clipboard formats/ownership/callbacks; PIC pending/in-service state, timer/keyboard/RTC/mouse hooks, callback requeue, master/slave, ICW ordering, AEOI, locks and repeated reset. |
| Candidate Proposal | [WOW32 clipboard](../proposals/proposal-wow32-clipboard-001.md). |

S3 retains original handle projection and scopes the only public-USER
adaptation to the three live Clipboard operands and four returned HWND
results; its private-desktop control reaches the owner call but is denied
`ERROR_ACCESS_DENIED`, not accepted. [S3
progress](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md) records
the deferred/replacement/dead-owner obligations.

The PIC addendum is independent of Clipboard/Win95; its research candidate
only proves IRQ0/IRR. The source matrix and decision are in [the PIC-reset
plan](../etc/evidence/m0-t460-s3-pic-eoi-reset-plan.md).

## S1 Closure Record

T460 S1 closed the original Clipboard source/owner inventory and callback ABI
audit. The [S1 owner matrix](../etc/evidence/m0-t460-s1-clipboard-owner-matrix.md)
records the selected original conversion/cache owners, no autonomous
Clipboard substitute, and finite S2 transfer/S3 delayed-render handoffs.
This is source/composition evidence, not a runtime transfer claim.

## S2 Closure Record

S2 repaired the bounded host-generation invalidation and rejected-handoff
ownership seam while retaining the selected original `wuclip.c` conversion
owners.  Two real guest/native transfer rounds, the original callback ABI and
formal x86 graph, cached component build, all retained WOW frontiers and the
Console-17/Window-17 product gates pass against one published ten-image
package.  [The S2 transfer acceptance](../etc/evidence/m0-t460-s2-clipboard-transfer-acceptance.md)
records the hashes, modern-host exceptions and finite S3 handoff.  Guest media
is unchanged.

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

T454 S3 `5ede6c0e4` is the current coherent ten-image delivery: it publishes
the bounded WOW32 free/rollback correction with rebuilt dependent product
images. Its release identity and formal evidence are indexed in
`docs/etc/README.md`; guest media remains unchanged. T452 S2 `0f8487f04`
remains the prior W03 allocation/alias foundation delivery.

## Successor Boundary

W05 and W07 are closed. W08 font/text/metafiles is active at S1.
Resource-specific menu/dialog conversion remains a later owner.

## Temporary Evidence

T458 S1 may use only `build/t458-s1` for disposable source-audit and focused
resource-fixture evidence, with a 30-minute phase, 2-GiB size and 10-minute
no-progress budget. Stable `.ninja` and `.ccache` remain reusable caches and
are never task evidence roots.

T458 S2 may use only `build/t458-s2` for retained conversion and guest
resource evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress
budget. Stable `.ninja` and `.ccache` remain reusable caches and are never task
evidence roots.

T458 S4 may use only `build/t458-s4` for disposable consumer acceptance, with
a 45-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T459 S1--S4 used `build/t459-s1` through `build/t459-s4` only for disposable
evidence. Their retained results are indexed from the closure; stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T460 S3 may use only `build/t460-s3` for disposable delayed-render/loss
evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T457 S1 may use only `build/t457-s1` for disposable source-audit and focused
fixture evidence. Stable `.ninja` and `.ccache` remain reusable caches and
are never task evidence roots.

T457 S2 may use only `build/t457-s2` for disposable guest-dialog/input/caret
evidence. Stable `.ninja` and `.ccache` remain reusable caches and are never
task evidence roots.

T457 S3 may use only `build/t457-s3` for disposable hook/timer evidence.
Stable `.ninja` and `.ccache` remain reusable caches and are never task
evidence roots.

T457 S4 may use only `build/t457-s4` for disposable integrated acceptance
evidence. Stable `.ninja` and `.ccache` remain reusable caches and are never
task evidence roots.

T454 may use only declared `build/t454-s1`, `build/t454-s2`, `build/t454-s3`
and `build/t454-s4` roots for disposable evidence; stable `.ninja` and
`.ccache` remain reusable caches. Retain durable results before exact-target
cleanup; never use a task root as a build cache.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

T455 S2 may use only `build/t455-s2` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T455 S3 may use only `build/t455-s3` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T455 S4 may use only `build/t455-s4` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T456 S1 may use only `build/t456-s1` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T456 S2 may use only `build/t456-s2` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

## Recent M0 Closures

- [T454](../history/m0-t454-closure.md): WOW32 resource discovery and loading.
- [T453](../history/m0-t453-closure.md): files, directories, environment and OEM.
- T452: modules, memory and aliases.
- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T454 S4 added only acceptance evidence and closure records. Its S3 product
delivery remains the current verified release; S4 changed no product source,
guest media or release asset.
