# Project Status

## Current Work

## Active Packet

**Active: M0 T415 S1** — Final retired-MVDM-root elimination (Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T415 S1, Ordinary Mode. |
| Admission And Approval | Owner directed completion of the remaining old MVDM roots after moving fonts and `v86/util`; explicitly authorized moving the remaining `mvdm-host` printer carrier. |
| Candidate Proposal | [Final MVDM-root tail elimination](../proposals/proposal-final-mvdm-root-tail-elimination-001.md). |
| Objective | Verify and retain the user’s font/tool moves, move the standalone printer carrier to `mvdm-overlay`, remove all three retired roots, and repair only direct path consumers. |
| Non-goals | No functional source recovery, no monitor algorithm change, no new adapter, no runtime publication, and no content rewrite of original blobs. |
| Reference Baseline | T414 canonical MVDM closure `622b71cda`; original OpenNT `base/mvdm` and `base/win32/winnls/fontsup/system`. |
| Files And ABI Surface | User-moved font and `v86/util` files, monitor printer carrier, formal generator, source-layout authorities and APP_VERSION metadata. No public ABI change. |
| Applicable Rules | docs/README, EXECUTION, ARCHITECTURE, CODING, DOCUMENT, source-policy and CONTRIBUTING. |
| Verification | Exact blob comparison to OpenNT, fresh x86 worker link, final tree/path audit, documentation governance and diff review. Identity/tree/graph checks pass; the actual x86 link is temporarily blocked before `cl.exe` creation by local Defender/process launch. |
| Expected Markers | No `src/mvdm-guest`, `src/mvdm-tools`, or `src/mvdm-host` tracked content; printer carrier selected only from `src/mvdm-overlay`; all original inputs remain byte-identical. |
| Asset Needs | Existing OpenNT baseline, user’s working-tree moves and formal build generator; no external download. |
| Reporting Requirements | State font/tool upstream paths, carrier classification, updated consumer count, verification and any retained root. |
| Stop Conditions | Any blob mismatch, a non-mechanical source-content change, a link-role change, or an unexpected remaining root. |
| Exit Criteria | Identity/build/governance pass and a clean pushed T415 closure. The actual x86 link remains the only unmet condition. |
| Original Owner Request | “字体目录、mvdm-tool都已经改好。这个你帮我也移动了吧。” |
| Similar-Issue Sweep | Verify all three retired roots, user move nesting, original path identity, formal-generator selections and source-layout documentation. |

## Current Technical Baseline

- The source baseline selects formal x86 `/MT` run16.exe, basesrv.exe and
  ntvdm.exe with original CCPU40, APP_VERSION 0.0.415 and protocol 3. The
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
| T415 S1 P1, pending actual-link closure | [Final root elimination](../etc/evidence/m0-t415-s1-final-root-elimination.md) |
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
