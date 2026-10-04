# Project Status

## Current Work

**Active: M0 T427 S5** (Ordinary Mode).

Owner accepts T426 and admits the queued system-root/application-search
isolation package. [T426 closure](../history/m0-t426-console-root-monitor-tree-closure.md)
retains S1-S4 delivery, verification and limits. S1 source/contract audit and
real selected-image reproduction are complete. S2 shared root/internal binding
implementation passes its gates and is published at 7d1bd3452, pushed to main
with a clean worktree. S3 now removes implicit package-first user discovery
and makes the product-generated COMMAND interpreter path explicit. S3 is
delivered at e33e8090c, pushed to main; focused and full product gates,
publication and smoke pass. S4 completes the admitted contract and remaining
caller/API coverage; its reviewed P includes owner-admitted NTMON TASK
presentation cleanup, delivered at d0c2fc1d0 and pushed. Owner now admits S5:
place all six product EXEs and the two host DLLs together in system32, matching
the original NTVDM/COMMAND/provider deployment relationship. S5 implementation,
retained gates and coherent publication are complete; delivery 62a71fd90 is
committed and pushed to main. T427 stays open for owner validation.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T427 S5; Ordinary Mode. |
| Admission And Approval | Owner: “你把我们的另外5个新增组件exe，也都放到 ntvdm.exe同样路径 system32里面，即可。请你一并处理。” S4 delivered; admit original host/guest co-location migration. |
| Candidate Proposal | [System-root and application search isolation](../proposals/proposal-ntvdm-system-root-path-isolation-001.md). |
| Objective | Deploy all six EXEs and WOW32/VDMREDIR in system32; derive Windows/product root from their actual loaded image's parent directory. Preserve original guest system32 paths, root SYSTEM.INI, separate Win16 system semantics, user search and user data. |
| Non-goals | No guest/parser/EXEC patch, new component/helper, RPC validation, broad loader change, lifecycle/scheduler redesign or global host environment/Registry mutation. Preserve existing CLI argument syntax and native shell fallback contract. |
| Reference Baseline | main d0c2fc1d0; published S4/r050/runtime, Product r052 and published smoke r054 passed; APP0.0.427/RPC38/I/O25. |
| Files And ABI Surface | common root/path mechanics, product-generated component/provider locations, staging, selected runtime test/cleanup paths and deployment evidence. No guest binary, original execution algorithm or RPC/I/O wire change. NTVDM.REG must be preserved while maintaining its existing beside-worker ownership. |
| Applicable Rules | AGENTS reading set; execution, architecture, coding, document and source policies; preserve original search/EXEC and immutable guest media. |
| Verification | Map every proposal requirement to exact source and assertion; inspect remaining derivations and native environment/guest API coverage. Reuse S2/S3 sealed evidence by proven unchanged inputs, add actual missing witnesses where needed. Any production change rebuilds affected x86 closure and repeats retained Console17/Window17/WOW, coherent publication/recovery/hash smoke; governance/link/diff and reviewed commit/push. |
| Expected Markers | User search has no implicit package-first authority; internal root derives from actual own EXE; real host paths remain host-owned; guest projection is bounded. Evidence distinguishes current behavior from proposed repair. |
| Asset Needs | Existing source, immutable runtime, x86 fixture tools; subst Z: only if needed with final removal. No source import or new product process. |
| Reporting Requirements | Report exact callers, sources, reproduction results, proposed minimal shared owner, migration stages and unresolved edges; do not claim audit conclusions as implemented behavior. |
| Stop Conditions | Missing source/provenance, required guest change or expanded loader/lifecycle policy requires renewed admission; preserve unrelated work and baseline. |
| Exit Criteria | All root/search requirements have an explicit implementation/test disposition, verified runtime identity and original semantics preserved; no new RPC validation. Any unresolved limit is honestly distinguished from a pass and does not silently weaken the admitted scope. Build/test/publication obligations for changed inputs, review/commit/push complete; T remains open for owner validation. |
| Original Owner Request | Move the five added EXEs to the same system32 location as original NTVDM.EXE; handle related paths coherently. |
| Similar-Issue Sweep | Direct, nested and internal COMMAND launches; all six EXEs; guest config/media/Win16 directories; host fonts/temp, relocation and inherited wrong root. Each process uses its own EXE root; RPC keeps existing acceptance policy. |

The [implementation sequence](../etc/operations/t427-system-root-search-isolation-plan.md)
keeps S1 audit, S2 shared root/resource binding, S3 user search isolation,
S4 integration and S5 host co-location separate. Only S5 is active. S2-S4 deliveries have
indexed evidence and unchanged RPC acceptance.

S4's [guest-root/layout integration record](../etc/evidence/m0-t427-s4-guest-root-layout.md)
tracks the actual WOW boot PDB correction and original TXTSETUP.SIF locations.
Canonical-layout focused DOS/native and Win16 probes pass. Final S4/r052
passes Console17/Window17 and three retained WOW frontiers; r053 publishes
the exact coherent eight-file package recoverably and r054 passes published
Console/Window smoke and all hashes. S4 publication equaled S4/r050/runtime;
S5's system32 host layout supersedes it.
Original media/configuration now use system32 destinations; root SYSTEM.INI,
NTVDM.REG and user applications are preserved. Bare utility names require
ordinary CWD/PATH; explicit system32 paths also work. No RPC policy change.

S1 first-pass [audit and actual selected-image evidence](../etc/evidence/m0-t427-s1-root-search-audit.md)
confirms package shadowing of CWD/PATH in four real native launch cases (r002),
and original directory-first COM/EXE/BAT search from COMMAND source. Shared
root ownership is proposed in common, not worker-base. The follow-up resolves Win16 environment,
selected resource/loader reachability, PIF ordering and package-join audit;
S1 has no production repair claim.

## S1 Closure Record

S1 audit is complete in the [source and actual-image record](../etc/evidence/m0-t427-s1-root-search-audit.md).
Real baseline search cases passed at bd5af1e6e; final source follow-up proves
selected cmosnt uses no external file, distinguishes ANSI guest and Unicode
host environment, and assigns each selected path/module/temp/join boundary.
Runtime isolation remains S2/S3 implementation work, not claimed by this audit.
Governance/link/diff gate this documentation-only closure and S2 admission.

## S2 Closure Record

S2 is delivered at 7d1bd3452, pushed to main after reviewed source/diff,
governance and link gates. Its [evidence](../etc/evidence/m0-t427-s2-own-image-root-bindings.md)
records affected x86 build, focused negatives, full retained product gates and
coherent publication with recovery. Published smoke and all eight hashes pass.
No new RPC validation or wire change; user-search repair remains S3.

## S3 Closure Record

S3 is delivered at e33e8090c, pushed to main with clean worktree. Its
[evidence](../etc/evidence/m0-t427-s3-application-search-isolation.md) records
actual selected-file assertions, internal interpreter/handoff probes, full
retained Product gate, recoverable eight-file O:/winnt publication and smoke.
Governance, links and diff checks pass. No CLI/parser/guest/RPC change.

## S4 Closure Record

S4 is delivered at d0c2fc1d0 and pushed. Its
[guest-root/layout evidence](../etc/evidence/m0-t427-s4-guest-root-layout.md)
records the final affected x86 build, original guest directory/layout probes,
Console17/Window17 and retained WOW gates, recoverable publication and smoke.
Only the owner-approved S5 host co-location follow-up is active; T remains open.

## S5 Closure Record

S5's [host co-location evidence](../etc/evidence/m0-t427-s5-system32-host-colocation.md)
records original-source layout, own-image parent root, internal launch/DLL
binding, actual native/Win16 APIs, selected-image and staging negatives.
Affected x86 closure, final S5/r012 Console17/Window17/WOW gate and all eight
published smoke cases pass. S5/r011 preserves the old layout and publishes
the exact eight-file set to O:/winnt/system32; media/config/user hashes remain.
S5 code delivery 62a71fd90 is committed and pushed; T remains open.

## Current Technical Baseline

S5 uses MSVC14.43/SDK22621/Win32 x86 /MT CCPU40, unchanged APP0.0.427/RPC38/I/O25.
All six EXEs and both host DLLs now reside in O:/winnt/system32. Guest system32
media and root SYSTEM.INI remain. NtvdmSystemRoot is the parent of each actual
EXE directory; ordinary user search and native host APIs are unchanged.
S5/r012, recoverable r011 publication and r013 published smoke pin this set.
T427 awaits owner validation. Other sessions' queue/proposal edits are preserved.

The S4 record below is predecessor evidence, not the current installed layout.

S4 delivery uses MSVC14.43 / SDK22621 / Win32 x86 /MT CCPU40, unchanged
APP0.0.427/RPC38/I/O25. Its [evidence](../etc/evidence/m0-t427-s4-guest-root-layout.md)
records actual guest/native directory separation, original deployment paths,
minimal registered mirror seams, selected-image/internal handoffs and final
Product/publication gates. Reviewed source and publication are complete in
the containing P; only final owner T acceptance remains. Other sessions'
unrelated queue/proposal changes are preserved outside this delivery.

The following S3/S2 records are retained predecessor evidence, not the current
published set.

S3 [search delivery evidence](../etc/evidence/m0-t427-s3-application-search-isolation.md)
records directory-first user discovery and explicit internal interpreter.
Production resolver/CLI tests, 17 actual-image cases, two real internal/DOS
handoffs and retained Console17/Window17/WOW gates pass. Its O:/winnt delivery
equaled S3/r001/runtime, checked against S3/r003 and again after all eight published
smoke cases. S3/r005 retains S2 recovery; S3/r006 retains smoke and hash proof.

S2 [delivery evidence](../etc/evidence/m0-t427-s2-own-image-root-bindings.md)
records common own-image root, internal provider/media paths, bounded ANSI
Win16 environment and restored original host-temp fallback. No RPC root
identity validation is added. The formal x86 build, local root/environment/
layout/resource/loader tests, retained Console17/Window17/WOW frontiers,
22 service and seven real RPC fixture cases pass. Five existing version-
mismatch peers reject launcher/worker with 1306 under approved Z: short paths;
the initial long-path failure remains evidence of the known path limit.

Preceding S2 publication equals build/M0-T427/S2/r002/runtime, all eight hashes
verified against S2/r003/runtime-manifest.json and again after publication.
Files: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe, ntmon.exe,
WOW32.DLL and VDMREDIR.DLL. Recovery/publication manifest: S2/r006; published
smoke: S2/r007. All eight Console/Window COMMAND/MEM/EDIT/native VER smoke
cases pass with final hash equality. MSVC14.43 / SDK22621 / Win32 x86 /MT
CCPU40, APP0.0.427, control/RPC38 and I/O25. Previous T426 package remains
recoverable; guest/configuration inputs remain unchanged. T427 is open:
whole-objective/root-caller integration audit remains S4.

S9/r035-r036 independently passed Console17/Window17 and the three retained
WOW frontiers. S11/r006 adds four selected real continuous-input cases with
zero consumption waits, expected root exit 1, complete MEM reports and
identity-checked cleanup. Z: is removed; no owned product process remains.
S11 changed only tests/docs; the subsequent S2 code-bearing delivery rebuilds
the affected closure and adds its own full retained package gates above.

S8 accepted software VGA FULLSCREEN, natural mouse route/draw/erase and
producer-side update triggers remain. NTCON/worker-base do not filter emitted
display events. Original guest/device/execution logic and imported libraries
remain at their owners. Physical RDP and broader WOW usability retain their
explicit prior boundaries. No speculative CAF repair or new helper/channel/
scheduler is introduced.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T426 | Owner-accepted monitor tree, S4 delivery 700b3d862; published S3 package unchanged. [Closure](../history/m0-t426-console-root-monitor-tree-closure.md). |
| T425 | Owner-directed closure after S11 typeahead proof. [Closure](../history/m0-t425-worker-neutral-frontend-closure.md). |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone. [Closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

Owner accepts T426 on 2026-10-04 and admits T427 S1. Its former queue-head
candidate is removed while all remaining candidates retain relative order.
This closure/admission is documentation-only: no production build, runtime
test or republication is implied. Outstanding debt remains in [TODO](TODO.md).
