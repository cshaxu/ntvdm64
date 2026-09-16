# Project Status

## Current Work

## Active Packet

**Active: M0 T416 S2** — Restore the original WOW command-line carrier
(Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T416 S2, Ordinary Mode. |
| Admission And Approval | Sequential execution under the owner's T416 approval; S1 P1 `2031f14de` froze the removal ledger. |
| Candidate Proposal | [Mirror, overlay and adapter footprint cleanup](../proposals/proposal-mirror-overlay-footprint-cleanup-001.md). |
| Objective | Prove the run16-produced original WOW `-a` command line reaches the worker, then remove the unreachable session fallback and its complete autonomous carrier without changing DOS, nested COMMAND or original WOW parser behavior. |
| Non-goals | No full WOW/WRITE recovery, program reclassification, broker policy change, guest-memory redesign, x87 or NTVDMx64 correction. |
| Reference Baseline | S1 P1 `2031f14de`; original `BaseGetVdmConfigInfo` command construction; existing three-program formal x86 graph. |
| Files And ABI Surface | `cmdmisc.c`, firmware/session fallback carrier and focused fixtures/evidence only; original WOW `-a` parser remains the sole worker interface. |
| Applicable Rules | docs/README, EXECUTION, ARCHITECTURE, CODING, DOCUMENT, source-policy and CONTRIBUTING. |
| Verification | Static complete-caller sweep, original BaseGetVdmConfigInfo DOS/WOW fixture, source-shape assertion that `cmdmisc.c` retains only original ` -a ` parser, formal x86 build and DOS regressions. |
| Expected Markers | No setter/caller for fallback storage before removal; original `-w -a` command construction; no fallback symbol, storage field or declaration remains after removal. |
| Asset Needs | Pinned OpenNT, existing original server lifecycle fixture, formal generator and staged DOS regressions; no external download. |
| Reporting Requirements | State exact mirror and adapter-autonomous reduction separately; distinguish direct WOW proof from unavailable full WRITE acceptance. |
| Stop Conditions | Any selected worker launch path lacks ` -a `, a retained fallback caller is discovered, or a DOS/nested COMMAND regression appears. |
| Exit Criteria | Complete fallback chain removed, source-shaped WOW parser retained, formal x86/DOS checks pass, metrics recomputed and evidence indexed. |
| Original Owner Request | “以减少 mvdm和opennt-host相对原始opennt代码的diff为目标（包括overlay，甚至adapter的精简），同时保证架构合理、不破坏当前已经实现跑通的部分？” and “准入执行！” |
| Similar-Issue Sweep | All `mvdm_wow_bootstrap_kernel` definitions/callers, Base config binding, direct worker entry and shared/separate WOW construction. |

## S1 Closure Record

S1 P1 `2031f14de` freezes the paired OpenNT measurement, overlay inventory,
adapter classification rule and every bounded cohort disposition in the
[footprint baseline](../etc/evidence/m0-t416-s1-footprint-baseline.md). No
runtime source changed in S1; S2 is the first approved restoration group.

## Current Technical Baseline

- The source baseline selects formal x86 `/MT` run16.exe, basesrv.exe and
  ntvdm.exe with original CCPU40, APP_VERSION 0.0.416 and protocol 3. The
  deployed O:\winnt package remains the last tested T413 product (0.0.413);
  T414 does not publish a layout-only rebuild.
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
- Mirror diff distance is 437 versus pre-T413 506; overlay cohort stays 125
  lines. S5 adds only ten braces and five attribution comments across existing
  condition lines (+15/-5); no executable statement is added.
- Build roots: build/M0-T413/S5/formal and integration. Prior products remain
  recoverable under integration/prior-products; runtime logs only O:\winnt\logs.
  Owner Terminal/RDP visual acceptance remains separate.

## Delivery References

| Delivery | Evidence |
| --- | --- |
| S2, pending P2 | [Original WOW command-carrier restoration](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md) |
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
