# Project Status

## Current Work

## Active Packet

**Active: M0 T414 S2** — Canonical MVDM source-tree mechanical recomposition
(Ordinary Mode).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T414 S2, Ordinary Mode. |
| Admission And Approval | Owner requested recompose of the original MVDM directory into its OpenNT-relative structure. S1 has closed its blob-checked path map; S2 performs only the admitted mechanical re-root and current-consumer path repair. |
| Candidate Proposal | [Canonical MVDM source-tree recomposition](../proposals/proposal-canonical-mvdm-tree-recomposition-001.md). |
| Objective | Move the 4,615 mapped selected files to `src/mvdm/<OpenNT-relative-path>`, re-root the private overlay to `src/mvdm-overlay`, and update only current formal consumers without content or role changes. |
| Non-goals | No source behavior repair, no content rewrite, no change to host/guest/tool/firmware link roles, no migration of the excluded non-MVDM carriers, and no mixing private overlay code into the original tree. |
| Reference Baseline | T413 S5 published x86 CCPU40 package; selected union is pinned OpenNT/OpenNT-4.5 `base/mvdm`; [proposal](../proposals/proposal-canonical-mvdm-tree-recomposition-001.md). |
| Files And ABI Surface | Mapped MVDM roots, `mvdm-host-overlay`, current formal generator/stager and current source-layout/architecture authorities. No public ABI change. |
| Applicable Rules | docs/README, EXECUTION, DOCUMENT, ARCHITECTURE, CODING, source-policy and CONTRIBUTING. |
| Verification | `Test-T414CanonicalMvdmTree.ps1 -Phase after -BaselineRef 9162b97fe`, Git rename/content review, current-generator/stager path audit and source-layout authority review. |
| Expected Markers | All 4,615 destination blobs equal baseline; no old mapped roots remain; `mvdm-overlay` stays private and absent from original comparison tree; only current consumers use new paths. |
| Asset Needs | Existing repository, pinned OpenNT/OpenNT-4.5 source union and S1 evidence/checker; no new external input. |
| Reporting Requirements | State moved/excluded file totals, renamed roots, updated current consumers, any unchanged historical paths, and content-identity outcome. |
| Stop Conditions | Any blob mismatch, ambiguous/case-colliding target, formal consumer not captured by S1, unexpected build-role change, or a required source-content modification. |
| Exit Criteria | Mechanical moves and current path repairs are complete, 4,615 blob identities pass, architecture records describe the new physical root, and S3 is bounded to build/runtime/governance closure. |
| Original Owner Request | “把原来 mvdm整个目录按照原始 opennt 的mvdm目录结构重新组合起来，不要再散开了。” |
| Similar-Issue Sweep | Check split guest, tool and firmware roots, private overlay re-root, generated formal output, staging inputs, path-case collisions and all current build/stage references; do not rewrite historical records. |

## S1 Closure Record

S1 passed the reproducible canonical-map check at baseline `9162b97fe`: 4,615
selected MVDM blobs map without collision (host 1,975; DOS 877; WOW16 1,658;
Bin86 19; tool 45; firmware 41). The two non-MVDM carries and project READMEs
are explicit exclusions. Evidence: [canonical MVDM tree map](../etc/evidence/m0-t414-s1-canonical-mvdm-tree-map.md).

## Current Technical Baseline

- O:\winnt contains fresh formal x86 /MT run16.exe, basesrv.exe and ntvdm.exe,
  original CCPU40, APP_VERSION 0.0.413 and protocol 3. Hashes match S5 evidence.
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
relative order behind it. This documentation delivery admits no numeric T.

## Recent M0 Closures

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
