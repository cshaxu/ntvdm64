# Project Status

## Current Work

## Active Packet

**Active: M0 T416 S4** — Retire default-off MVDM diagnostic carriers
(Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T416 S4, Ordinary Mode. |
| Admission And Approval | Sequential execution under the owner's T416 approval; S3 concludes the overlay disposition without invented relocation. |
| Candidate Proposal | [Mirror, overlay and adapter footprint cleanup](../proposals/proposal-mirror-overlay-footprint-cleanup-001.md). |
| Objective | Remove the S1-identified default-off COMMAND diagnostic calls and their recorder-only adapter support, restoring original MVDM control flow without changing MVDM functional carriers. |
| Non-goals | No removal of functional termination/lifecycle support, EOI/keyboard diagnostics, overlay relocation, Console behavior change, broker policy change or WOW/WRITE recovery. |
| Reference Baseline | S1 P1 `2031f14de`, S2 P2 `c59b1008a`, S3 overlay disposition, original `cmddisp.c`/`cmdmisc.c` and adapter termination recorder. |
| Files And ABI Surface | Original MVDM diagnostic hook call sites, recorder declarations/definitions and tests/evidence only; no product ABI addition. |
| Applicable Rules | docs/README, EXECUTION, ARCHITECTURE, CODING, DOCUMENT, source-policy and CONTRIBUTING. |
| Verification | Original hunk comparison, complete symbol/caller/build sweep before and after, focused original lifecycle/CCPU contracts and formal x86 build when changed sources are selected. |
| Expected Markers | Each deleted recorder symbol has no callers/build input; original COMMAND/EOI control structures remain except deleted observation calls. |
| Asset Needs | Pinned OpenNT source, current adapter termination recorder, formal graph and focused fixtures; no external download. |
| Reporting Requirements | Separate mirror-diff, adapter-autonomous and diagnostic-code deletion counts; name all retained functional recorder APIs. |
| Stop Conditions | A recorder call gates behavior, a remaining caller needs retained storage, an original function diverges beyond the diagnosed call, or a focused regression appears. |
| Exit Criteria | Complete default-off diagnostic family removed or each non-removable member proved; no dead declaration/definition/build edge remains and evidence is indexed. |
| Original Owner Request | “以减少 mvdm和opennt-host相对原始opennt代码的diff为目标（包括overlay，甚至adapter的精简），同时保证架构合理、不破坏当前已经实现跑通的部分？” and “准入执行！” |
| Similar-Issue Sweep | COMMAND dispatch, command-environment/stub/return reports, child report-path capture/restore, and every COMMAND-family recorder caller. |

## S1 Closure Record

S1 P1 `2031f14de` freezes the paired OpenNT measurement, overlay inventory,
adapter classification rule and every bounded cohort disposition in the
[footprint baseline](../etc/evidence/m0-t416-s1-footprint-baseline.md). No
runtime source changed in S1; S2 is the first approved restoration group.

## S2 Closure Record

S2 P2 `c59b1008a` removes the unreachable session-based WOW fallback and
restores original missing-`-a` handling; the source proof, original lifecycle
fixture and fresh x86 formal graph are recorded in the
[WOW command-carrier evidence](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md).

## S3 Closure Record

S3 updates the canonical C-VID contract verifier and concludes that no private
overlay body can be safely deleted or relocated; the source/caller/build proof
is in the [overlay composition disposition](../etc/evidence/m0-t416-s3-overlay-composition-disposition.md).

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
