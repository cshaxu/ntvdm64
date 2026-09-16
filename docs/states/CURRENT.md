# Project Status

## Current Work

## Active Packet

**Active: M0 T418 S1** — legacy broker archive removal proof
(Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T418 S1, Ordinary Mode. |
| Admission And Approval | Owner: “批准收口当前的T任务，准入下一个任务并开始执行。” Admits former Queue candidate 1 after T417 closure. |
| Candidate Proposal | [Component-boundary normalization and legacy broker retirement](../proposals/proposal-component-boundary-normalization-001.md). |
| Objective | Freeze every `broker.lib` source/export/link edge, prove whether its six-file legacy local state plane has any retained production or fixture consumer, and remove it only if a fresh x86 graph and map prove it unnecessary. |
| Non-goals | No BaseSrv policy rewrite; no move of modern RPC/Console/ABI mechanics into either mirror; no session-extension extraction or adapter-family reorganization in S1; no WOW completion claim. |
| Reference Baseline | T417 closure `3bb6d9e79`; S5 relationship audit; formal x86 graph `build/M0-T417/S4/formal-x86-003`; current x86 package at `O:\winnt`. |
| Files And ABI Surface | `src/broker/{broker,wire,base_vdm_record}.{c,h}`, generator archive/link lists and focused fixtures. No public wire or product ABI change. |
| Applicable Rules | docs/README, EXECUTION, ARCHITECTURE, CODING, DOCUMENT, CONTRIBUTING and source-policy; strict no-new-mirror-file and original-owner recovery ladder. |
| Verification | Source/export/reference sweep; fresh Win32/x86 graph with and without `broker.lib`; map inspection; focused CCPU/C-VID and BaseClient/BaseSrv lifecycle tests; COMMAND/MEM/EDIT/nested-COMMAND/broker-death regressions if removal is reached. |
| Expected Markers | Either no retained reference and a successful archive-free build/map, or the exact retained caller and a revised disposition; no speculation from archive linkage alone. |
| Asset Needs | Current selected OpenNT/MVDM sources, formal generator, broker lifecycle fixtures and local `O:\winnt` runtime package. |
| Reporting Requirements | Record each source, archive, link and symbol edge; state net deleted versus retained lines and every test result/limitation. |
| Stop Conditions | Any retained caller, export, ABI/lifetime dependency, build failure, or runtime regression outside the proposed 387-line plane pauses deletion and requires an evidence-backed scope decision. |
| Exit Criteria | A separately reviewable S1 evidence record establishes deletion or exact retention; no source deletion without fresh x86 link/map proof and required regressions. |
| Original Owner Request | “批准收口当前的T任务，准入下一个任务并开始执行。” The standing objective is to clean adapters while preserving the completed runnable paths. |
| Similar-Issue Sweep | All product, fixture and generator link lists; archive symbols; direct and indirect source includes; broker transport versus local-plane callers; stale documentation references. |

## S1 Closure Record

S1 freezes all selected source/include/object/archive/link/caller edges and
records an evidence-backed source-first disposition for every private body in
the [private-component ownership freeze](../etc/evidence/m0-t417-s1-private-component-ownership-freeze.md).
No runtime source changed.  S2 is the patch-root exit group.

## S2 Closure Record

S2 P2 `c59b1008a` removes the unreachable session-based WOW fallback and
restores original missing-`-a` handling; the source proof, original lifecycle
fixture and fresh x86 formal graph are recorded in the
[WOW command-carrier evidence](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md).

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
  ntvdm.exe with original CCPU40, APP_VERSION 0.0.416 and protocol 3. The
  deployed O:\winnt package is the verified T416 formal product.
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
- Build roots: build/M0-T416/S5/formal-x86-002 and integration; runtime logs
  only O:\winnt\logs.
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

[Queue](QUEUE.md) remains the sole ordering authority.  T417's owner-reviewed
follow-up is the first candidate: [component-boundary normalization and legacy
broker retirement](../proposals/proposal-component-boundary-normalization-001.md).
It does not admit a new numeric T until the owner chooses it.

## Recent M0 Closures

| T416 | [Footprint cleanup](../history/m0-t416-footprint-cleanup-closure.md) | S1--S5 reduced paired MVDM mirror distance by 74 lines and raw adapter inventory by 500 lines; final x86 package and runtime matrix passed. |
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
