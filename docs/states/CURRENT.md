# Project Status

## Current Work

## Active Packet

**Active: M0 T417 S3** — MVDM overlay owner recovery
(Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T417 S3, Ordinary Mode. |
| Admission And Approval | Owner: “准入执行”; admits Queue candidate 1. |
| Candidate Proposal | [Mirror-private component elimination and project-component reorganization](../proposals/proposal-retire-mvdm-softpc-patch-001.md). |
| Objective | Retire each MVDM overlay body using only a bounded adapter/ABI carrier or a minimal diff in its existing original mirror owner; neither mirror may gain a new file. |
| Non-goals | No OpenNT-host overlay move, adapter reorganization, new behavior, runtime-policy change, WOW/WRITE claim, or changes to the owner-edited Queue/proposal. |
| Reference Baseline | T416 closure `0c0f3232c`; canonical `src/mvdm` topology; the owner-edited Queue/proposal candidate; current x86 CCPU40 package at `O:\winnt`. |
| Files And ABI Surface | MVDM overlay bodies and their original mirror callers, bounded adapter/ABI bindings, formal build manifest and CCPU/C-VID/printer verification. No public product ABI change. |
| Applicable Rules | docs/README, EXECUTION, ARCHITECTURE, CODING, DOCUMENT, CONTRIBUTING and source-policy; original-source recovery ladder is mandatory for every disposition. |
| Verification | Fresh formal Win32/x86 graph; original-owner/caller review; CCPU/C-VID/printer fixtures; COMMAND/MEM/EDIT regression; formal root and diff checks. |
| Expected Markers | Each retired overlay has a direct existing-mirror diff or named adapter/ABI carrier, single ownership, and no new file under either mirror. |
| Asset Needs | Current checkout, T416 formal graph, pinned OpenNT/OpenNT-4.5 comparison trees, and retained NTVDMx64 provenance only where its registered patch exception applies. |
| Reporting Requirements | Record per-overlay deleted/relocated lines, original owner, selected carrier, and focused regression evidence. |
| Stop Conditions | A move adds a mirror file, changes single-state ownership or guest behavior, or has no finite adapter/ABI boundary. |
| Exit Criteria | `mvdm-overlay` is absent, all retained behavior has one valid owner under the strict mirror rule, and focused/formal regressions pass. |
| Original Owner Request | “以减少 mvdm和opennt-host相对原始opennt代码的diff为目标（包括他们的overlay甚至adapter），同时保证架构合理、不破坏当前已经实现跑通的部分.” and “准入执行”. |
| Similar-Issue Sweep | Product and focused generator roots, include ordering, generated formal manifest, audit scripts, MVDM divergence register and every stale live-root reference. |

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

[Queue](QUEUE.md) remains the sole ordering authority. The owner-requested
[footprint cleanup proposal](../proposals/proposal-mirror-overlay-footprint-cleanup-001.md)
is first, with five planned S packets and separate mirror-diff, overlay and
adapter-autonomous-code acceptance metrics. Existing candidates keep their
relative order behind it. T414 closure does not admit the next numeric T.

## Recent M0 Closures

| T416 | [Footprint cleanup](../history/m0-t416-footprint-cleanup-closure.md) | S1--S5 reduced paired MVDM mirror distance by 74 lines and raw adapter inventory by 500 lines; final x86 package and runtime matrix passed. |
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
