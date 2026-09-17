# Project Status

## Current Work

## Active Packet

**Active: M0 T419 S2** — DTASKMGR read-only BaseSrv snapshot and Terminal TUI.

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T419 S2, Ordinary Mode. |
| Admission And Approval | The owner admitted the former queue-head BDM candidate, approved worker-scoped termination, and directed a `src/dtaskmgr/` component producing 8.3-named `DTASKMGR.EXE`. |
| Candidate Proposal | [BDM Terminal Task Manager](../proposals/proposal-bdm-terminal-task-manager-001.md). |
| Objective | Implement the authenticated, read-only BaseSrv worker snapshot and `DTASKMGR.EXE` Terminal/conhost TUI: connected, empty, unavailable and version-mismatch states; bounded rendering; refresh; Up/Down selection; and worker/hosted-child disclosure. |
| Non-goals | No termination request yet; no second task registry, per-child DOS/WOW termination, Windows process enumeration, direct `TerminateProcess` client path, GUI/guest TUI, arbitrary-process control, or MVDM/OpenNT mirror change. |
| Reference Baseline | S1 [worker-contract audit](../etc/evidence/m0-t419-s1-dtaskmgr-worker-contract-audit.md), `456270147` candidate and formal x86 three-program baseline. |
| Files And ABI Surface | `src/dtaskmgr/`, `src/basesrv/transport/service.idl`, BaseSrv management binding/source, formal build graph and product package selection. New copied fixed-width management records remain BaseSrv-owned. |
| Applicable Rules | Documentation, execution, architecture and coding authorities; authenticated fixed-width broker ABI and executable-owned placement rules. |
| Verification | Focused snapshot/ABI and TUI fixtures; empty/unavailable/version-negative states; rendering/selection/resize checks; fresh formal x86 four-program build; direct and interactive COMMAND/MEM/EDIT regressions, with unavailable automation reported honestly. |
| Expected Markers | Copied epoch/worker rows, no registered DTASKMGR peer, stable selection by worker ID, explicit hosted-child scope and no broker timer retention leak. |
| Asset Needs | Existing local source, formal x86 build scripts and `O:\winnt\logs`; no external source or guest-media acquisition. |
| Reporting Requirements | Record source/boundary, ABI layout, all negative behavior, exact build route, product hashes and owner visual acceptance; report no fabricated WOW16 row. |
| Stop Conditions | A required display field lacks an original worker/record source; snapshot needs a second registry/native value on wire; polling keeps an empty broker alive; formal build cannot be completed by a recorded route; or any established regression fails. Pause for owner decision. |
| Exit Criteria | Read-only management endpoint and DTASKMGR TUI built into the formal x86 package; snapshot/negative/selection tests pass; established regressions pass or have an explicit unavailable result; governance/diff checks and committed/pushed clean worktree. |
| Original Owner Request | Add a Terminal character-mode Task Manager that lists this product's DOS/Win16/WOW16 tasks, shows IDs/times/status, handles no broker/tasks, supports Up/Down selection and kills selected tasks; use `DTASKMGR.EXE`. The owner approved termination at the selected VDM worker scope, with all hosted child tasks disclosed in the confirmation. |
| Similar-Issue Sweep | Inspect all BaseSrv connection/auth/empty-timer paths and Console state adapters, plus version rejection and worker registration/rundown, to avoid a peer-registration leak or stale presentation state. |

## S1 Closure Record

S1 proves the worker—not individual DOS/WOW child—is the only existing
source-shaped selectable/terminable unit. The selected `{broker_epoch,
worker_sequence}` identity, read-only record traversal, server-only handle
use and original post-exit cleanup ordering are recorded in the
[worker-contract audit](../etc/evidence/m0-t419-s1-dtaskmgr-worker-contract-audit.md).
It also records the native Ninja startup lock and deliberately stopped slow
serial fallback as unavailable baseline evidence; neither is claimed as a
formal build pass. S2 owns a recorded full-build route before its closure.

## T419 S3 and T Closure Record

T419 restores the complete audited SoftPC x87 recovery set: little-endian
carrier declarations plus representation-safe binary64 conversion and rounded
`FIST m64int`. `fpu.c` is text-identical to the SoftPC comparison owner;
`cfpu_def.h` retains only SoftPC's inapplicable standalone `fenv` branch.
The formal x86 graph built `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
`VDMREDIR.dll` and all four fixtures. The fixtures pass, and the four product
files were deployed to `O:\winnt`. Full evidence is in the
[T419 x87 audit](../etc/evidence/m0-t419-s1-x87-audit.md).

## S2 Closure Record

S2 restores the exact SoftPC T59 little-endian declaration/member order in
`cfpu_def.h`, as recorded in the [x87 audit evidence](../etc/evidence/m0-t419-s1-x87-audit.md).  The original failure (`FP64.hiword=0`, `mant_lo=4`) becomes the
required layout (`mant_lo=0`, `hiword=4`); FP32, FP80 and FPU_I64 checks also
pass when the fixture uses the same `insignia.h`, `host_def.h`, `cfpu_def.h`
order as the production `fpu.c`.  The only residual header difference is
SoftPC's standalone `fenv` policy, which is intentionally inapplicable to the
NTVDM `_controlfp` host contract.  The `fpu.c` semantic hunks triggered S3,
as required by S2's stop condition.

## S1 Closure Record

S1 froze OpenNT, current and SoftPC identities in the
[x87 audit evidence](../etc/evidence/m0-t419-s1-x87-audit.md).  The SoftPC
source correction is `2c05238335a22d125e0e35f71692c3b2a1b0b15d`.  Its layout probe, compiled
against the current MSVC/x86 header, fails at `FP64.mant_lo` offset: current
reports `hiword=0`, `mant_lo=4`, while the IEEE little-endian carrier requires
the inverse.  The same probe shows incorrect FP32/FP64 fields and an FP80
exponent of 24575 for byte representation `-1.5`.  The failure is a host-C
layout fault, not a guest-endian or formatting issue.  S2 therefore admits
only the conditional declarations; SoftPC's `fenv` branch and all `fpu.c`
algorithm candidates remain separately tested rather than copied.

## S7 Closure Record

T418 S7 deletes the source-proven non-mirror dead-code subset: one selected
264-line classifier, plus unselected duplicate idle/stub providers totalling
28 C lines and their declarations.  The fresh x86 closure moves from 74 /
12,427 to 73 / 12,163 selected non-mirror physical lines.  Original
`nt_unix.c` remains the only mapped `HostIdleNoActivity` provider.  Formal
builds, all focused fixtures, deployed direct COMMAND/MEM regressions and
version rejection pass; the hidden interactive-input observer is accurately
recorded as unavailable.  The full proof and deferred cohort are in the
[S7 retirement evidence](../etc/evidence/m0-t418-s7-proven-dead-code-retirement.md).

## S1 Closure Record

T418 S1 removes the unreachable 387-line local `broker.lib` registry plane and
its two isolated fixtures after a source/export/link sweep and a fresh 506-step
x86 graph build.  The live authenticated broker transport and mirrored BaseSrv
owner remain selected.  The resulting source, graph and VDMREDIR RTL-closure
proof is recorded in the [legacy broker archive removal evidence](../etc/evidence/m0-t418-s1-legacy-broker-archive-removal.md).
S2 is the executable-owned product-ABI/run16 move group.

## S2 Closure Record

T418 S2 moves run16-only entry/probe code and the small version/package surface
to their executable/static owners. The fresh 506-command x86 graph, dedicated
layout fixture and core CCPU/C-VID/memory regressions are recorded in the
[run16/product ownership evidence](../etc/evidence/m0-t418-s2-run16-product-ownership.md).

## T417 S3--S4 Closure Record

S3--S4 remove `mvdm-overlay` and `opennt-host-overlay` under the strict
mirror rule: no new file was added to either mirror.  Existing original owners
now carry the bounded local state/EOI/BaseSrv changes; the generated C-VID
binder and kernel-monitor printer subset are named adapters.  The redundant
SAS facade was deleted after the fresh link proved the original C-VID owner
already exports it.  Fresh x86 graph, affected-owner compilation, three
product artifacts, CCPU fixture, C-VID contract, root-elimination and
documentation-governance checks are recorded in the
[S3--S4 evidence](../etc/evidence/m0-t417-s3-s4-overlay-owner-recovery.md).

## S3 Closure Record

T418 S3 moves the live BaseSrv entry, transport and private bindings to
`src/basesrv`, retaining the mirrored original record owner. Fresh x86 build,
reservation and original lifecycle proof are recorded in the
[BaseSrv ownership evidence](../etc/evidence/m0-t418-s3-basesrv-ownership.md).

S3 updates the canonical C-VID contract verifier and concludes that no private
overlay body can be safely deleted or relocated; the source/caller/build proof
is in the [overlay composition disposition](../etc/evidence/m0-t416-s3-overlay-composition-disposition.md).

## S4 Closure Record

S4 removes all eight COMMAND-only default-off recorders, their continuation
report selector and tool/test acceptance. The retained child-selector restore
seam supports other diagnostics and is not a COMMAND recorder. The source
sweep, original hunk review and focused x86 CCPU40 compilation are recorded in
the [COMMAND observer retirement evidence](../etc/evidence/m0-t416-s4-command-observer-retirement.md).

## S5 And T416 Closure Record

S5's fresh 478-command x86 graph, deployed hashes, COMMAND/MEM/EDIT,
ConPTY mouse/resize, CCPU exception and C-VID verification all pass.  Final
accounting records -74 paired `mvdm` mirror lines and -500 raw
`adapter-mvdm-host-out` lines, with both overlay cohorts unchanged.  See the
[S5 verification](../etc/evidence/m0-t416-s5-integrated-footprint-verification.md)
and [T416 closure](../history/m0-t416-footprint-cleanup-closure.md).

## Current Technical Baseline

- The source baseline selects formal x86 `/MT` run16.exe, basesrv.exe and
  ntvdm.exe with original CCPU40, APP_VERSION 0.0.418 and protocol 3. The
  deployed O:\winnt package is the verified T418 S7 formal product from
  `build/M0-T418/S7/formal-x86-001`.
- Original vglob.c supplies all 38 getter/setter pairs through original typed
  declarations. The registered seven-line header seam retains original full
  vector layout; autonomous field rebinding and 20 conversion wrappers remain
  removed. Original complete USE16 IP arithmetic remains restored.
- Five c_xcptn.c scope corrections keep immediate continuation/flag cleanup
  inside the handled protected-mode hook branch. Unhandled and real-mode faults
  reach original delivery. No new runtime function, adapter or overlay.
- 27 actual-source exceptional-control cases pass; uncorrected source fails
  101 assertions. All 168 IP-expression cases and 12 real DOS instruction cases
  pass, including divide-by-zero guest-handler entry and saved fault-IP check.
- Full original C-VID owner-library fixture, 17 command cases, six ConPTY
  profiles, native resize contracts and five prefilled short-window EDIT runs
  pass. The latter include 20 post-EDIT MEM completions.
- Protected-mode tests prove repaired branch behavior and delivery arguments,
  not complete IDT/TSS or WOW/WRITE runtime compatibility. No such claim is made.
- Final paired MVDM mirror distance is 4,621 versus S1's 4,695; OpenNT-host
  remains 14,410. Overlay cohorts remain 266 and 30 physical lines; the raw
  adapter-mvdm-host-out inventory falls 10,219 to 9,719 lines.
- Build roots: `build/M0-T418/S7/formal-x86-001`; runtime logs only
  `O:\winnt\logs`.
  Owner Terminal/RDP visual acceptance remains separate.

## Delivery References

| Delivery | Evidence |
| --- | --- |
| S2, c59b1008a | [Original WOW command-carrier restoration](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md) |
| S1, 2031f14de | [Footprint baseline and ownership ledger](../etc/evidence/m0-t416-s1-footprint-baseline.md) |
| S1, 23a4fa83c | [Source audit](../etc/evidence/m0-t413-s1-accessor-layout-audit.md) |
| S2, 3cc7851e5 | [Accessor restoration](../etc/evidence/m0-t413-s2-accessor-restoration.md) |
| S3, d9d814b00 and fbb121d72 | [Integrated verification](../etc/evidence/m0-t413-s3-integrated-verification.md) |
| S4, eb0c4fa33 | [Binding cleanup and IP repair](../etc/evidence/m0-t413-s4-binding-cleanup.md) |
| S5, admission 03847f34b; P2 accompanies this record | [Exception scope repair](../etc/evidence/m0-t413-s5-exception-scope.md) |

## Next Work

[Queue](QUEUE.md) remains the sole ordering authority. T418 is closed; its
former proposal is retained as closure provenance, not as a candidate. The
queue head receives a numeric T only on the owner's next admission.

## Recent M0 Closures

| T416 | [Footprint cleanup](../history/m0-t416-footprint-cleanup-closure.md) | S1--S5 reduced paired MVDM mirror distance by 74 lines and raw adapter inventory by 500 lines; final x86 package and runtime matrix passed. |
| T418 | [Three-program component normalization](../history/m0-t418-three-program-component-normalization-closure.md) | S1--S5 normalized the three-process ownership model; S6--S7 then removed one selected duplicate classifier and two unselected dead providers, with fresh x86 deployment and focused runtime proof. |
| T417 | [Private component elimination](../history/m0-t417-private-component-elimination-closure.md) | Retired all three private roots under the no-new-mirror-file rule and produced the separately queued component-boundary plan. |
| T415 | [Final MVDM-root elimination](../history/m0-t415-final-mvdm-root-elimination-closure.md) | 42 exact WinNLS fonts and two exact `v86/util` blobs moved to upstream paths; printer carrier moved to private overlay; formal x86 three-program graph passed. |
| T414 | [Canonical MVDM tree](../history/m0-t414-canonical-mvdm-tree-closure.md) | 4,615 original MVDM blobs physically recomposed at `src/mvdm`; private overlay is adjacent and separate; formal x86/focused checks passed. |

T413 S5 supersedes the previous real-mode divide timeout disposition, without
reopening T412 Broker or claiming full WRITE recovery. T411/T410/T409/T407/T406
retain their recorded scopes and limitations.

## Recent Governance

Owner-requested footprint planning (M0 Td, Ordinary Mode) adds one candidate
proposal at Queue head with five planned S packets. Scope is documentation only;
the current source/runtime baseline is unchanged. Acceptance requires separate
mirror, overlay and adapter-autonomous reductions, generated-code accounting
and an explicit relocation guard. Documentation governance, relative links and
diff checks pass; the delivery commit carries the proposal and ordering change.

S5 P2 delivers the source repair, upstream hunk evidence and separate queued
proposal in one reviewed commit. The current packet is closed without admitting
the next candidate; no document inventory or parallel status is introduced.
