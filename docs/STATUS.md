# Project Status

## Current Work

**Active: M0 T101 S1 — post-BOP Bochs prefetch-map mechanics audit.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T101 S1, Ordinary Mode. |
| Admission And Approval | Owner authorized priority/ROI-based task selection. T100 proves `50:42` resumes with its original no-op contract and reaches `50:02`; the remaining stop is a Bochs prefetch-map veto. This selects the next first-profile capability checkpoint as a read-only mechanics audit. |
| Objective | Correlate the exact post-BOP trace state and physical prefetch address with Bochs memory-map/firmware/device source and reached OpenNT control flow; classify the missing owner, if any, without changing runtime behavior. |
| Non-goals | No Bochs patch, feature/device enablement, memory mapping, firmware/ROM change, guest-media change, adapter BOP/provider change, host capability, or second guest trace. |
| Reference Baseline | etc/research/t100-s1-original-unavailable-dem-plane-runtime-001.md; t95 PC reset-floor and memory observations; current Bochs 2.6 source and the local OpenNT sources. |
| Files And ABI Surface | Evidence and status/history records only unless an already-present, read-only diagnostic parser is used. No source ABI or build recipe modification. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Reproducible source/trace address calculation, a source-owner map for every relevant mapping layer, and an explicit admitted/deferred/rejected disposition. No executable build or run. |
| Expected Markers | The `0xAF6AF` physical address is classified as a required native mapping, invalid/errant guest control flow, or an unresolved evidence gap; every conclusion distinguishes inference from direct proof. |
| Asset Needs | Existing T100 trace, local Bochs 2.6/OpenNT source and retained evidence only. |
| Reporting Requirements | Record exact source paths/lines, trace state, calculations, confidence and next action; do not represent an audit conclusion as a repair. |
| Stop Conditions | Stop and re-admit before source changes, a Bochs/configuration/device/firmware/guest-media change, adapter change, additional trace, or any new capability decision. |
| Exit Criteria | One reproducible owner/disposition map and a bounded recommended next task are recorded; no repair or runtime claim. |
| Original Owner Request | Holistic BOP recovery with original OpenNT semantics, a minimum Bochs boundary, non-invasive CLI capabilities, and no one-off patches. |
| Similar-Issue Sweep | Check CPU prefetch translation, memory handler/map ownership, ROM/RAM/video ranges, reset-floor device mappings, real/protected mode segmentation, and reached OpenNT continuation evidence. |

## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization. Its incomplete file/search
  capability and full real-path closure remain uncompleted.
- T96 S1 P1 is complete locally: all inventoried selector, service, and BIOS
  forms now have source routing/owner evidence without an enabled provider.
- Bochs remains generic-machine owner. The adapter owns a bounded copied BOP
  ingress and typed result transition, not DOS, BIOS, CCPU, or device behavior.
- Provider precedence is mandatory: original OpenNT host provider; original
  provider with contained CLI backend; source-derived provider only after a
  recorded blocker; otherwise unavailable/deferred.
- M0 T96 S3 is closed: its DEM plane classifies every callable service, and
  all admitted DEM slices are plane-gated. Legacy runtime endpoint migration
  remains expressly deferred to S8.
- M0 T96 S5 is closed: all XMS/DPMI services have a machine-owner disposition;
  only DPMI VCD is explicitly unavailable, and no native-machine operation is
  represented as adapter behavior.
- M0 T96 S6 is closed: all Redirector and debugger services have source-backed
  profile dispositions; selector-level VDD, mouse and device boundaries remain
  explicitly outside adapter execution.
- M0 T96 S7 is closed: the complete SoftPC BIOS inventory has native-owner,
  unavailable or collision disposition, without importing a BIOS dispatcher.
- M0 T96 S8 is closed: legacy runtime identities are registered and gated or
  rejected, with a scan preventing unapproved direct BOP recognizers.
- M0 T96 S9 is closed: complete inventory/profile regression covers all
  selectors, service identities and BIOS forms without claiming runtime.
- M0 T96 is closed: its real-path checkpoint exposes the original-unavailable
  DEM FASTREAD edge, without changing Bochs or fabricating a BOP outcome.
- M0 T97 S1 is complete: no original standalone provider exists; the frozen
  read-only and search candidates are only implementation inventory.
- M0 T97 S2 is complete: partial runtime dispatch and the contradictory
  FASTREAD experiment were withdrawn; the immutable token design is retained.
- M0 T97 S3 is complete: pathname and FCB searches have separate source
  contracts, and the host-directory fixture is expressly excluded.
- M0 T97 S3 P1 is complete: pathname has a source-shaped future-provider
  boundary; FCB remains explicitly deferred pending its distinct ABI.
- M0 T97 S3 P2 records that the existing host-directory search fixture is
  outside profile and not linkable on the current SDK; it will not be patched.

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Complete: all 73 callable DEM services have a component/disposition, and every admitted DEM slice is plane-gated; legacy runtime migration remains S8. |
| M0 T96 S4 | P1--P5 complete: COMMAND map/plane/no-op are closed; CONFIG/AUTOEXEC and target-launch legacy endpoints are reconciled as S8 migration objects. |
| M0 T96 S5 | Complete: 12 XMS and 25 DPMI services are source-mapped and plane-classified without linking historical dispatchers. |
| M0 T96 S6 | Complete: Redirector is source-proven unavailable; debugger and native device selectors remain deferred to their proper owners. |
| M0 T96 S7 | Complete: 41 definitions and 349 historical BIOS table forms have a mechanically verified ownership disposition. |
| M0 T96 S8 | Admitted in Ordinary Mode; existing endpoint migration and no-bypass verification begins. |
| M0 T96 S8 P1 | Complete: legacy endpoint register and no-bypass scanner are active; `host_service_catalog` now consumes common ingress rather than decoding BOP bytes. |
| M0 T96 S8 P2 | Complete: a generic DEM/COMMAND plane gate is verified; the first six runtime endpoints are gate-only migrations with no semantic expansion. |
| M0 T96 S8 P3 | Complete: all DEM/COMMAND legacy endpoint entry stages with a proven service identity now pass the common plane gate; runtime syntax verification passed. |
| M0 T96 S8 P4 | Complete: the conflicting legacy DEM FASTREAD path is removed from runtime, preserving its S3 original-no-op disposition. |
| M0 T96 S8 | Complete: legacy recognizers are plane-gated, frozen or rejected; no-bypass verification is active. |
| M0 T96 S9 | Admitted in Ordinary Mode; full inventory/provider/capability regression begins. |
| M0 T96 S9 | Complete: 192 callable services traverse ingress, registry and plane classification; inventory counts are independently verified. |
| M0 T96 S10 P1 | Complete checkpoint: r49 linked all T96 plane/gate objects and a fresh bounded NTIO/NTDOS run reached the original-unavailable `50:42` boundary; see etc/research/t96-s10-real-path-checkpoint-001.md. |
| M0 T96 | Closed: source ownership, the holistic BOP plane, and the current real-path boundary are recorded in history/m0-t96-closure-20260812.md. |
| M0 T97 S1 | Complete: component/ABI/failure and composition map records the source-derived replacement boundary. |
| M0 T97 S2 | Complete: provider withdrawal and immutable token/resource contract recorded; no FASTREAD helper remains. |
| M0 T97 S3 | Active: pathname/FCB directory-search ABI audit. |
| M0 T97 S3 P1 | Complete: separate DTA and FCB buffer/error/continuation map recorded; no search runtime provider is enabled. |
| M0 T97 S3 P2 | Complete limitation: host-directory fixture is excluded from T97 and its SDK-native query branch is not repaired. |
| M0 T97 S4 P1 | Complete: v4 BYOB profiles admit an exact, immutable four-entry DOS metadata snapshot; pathname routing remains disabled pending the common-plane integration slice. |
| M0 T97 S4 P2 | Complete focused closure: common DEM-plane `50:09/0B` routing consumes only copied guest inputs and the immutable v4 snapshot; FCB and ambient host namespace remain excluded. The legacy runtime fixture needs replacement because it asserts the withdrawn file-service path. |
| M0 T97 S4 P3 | Complete: default runtime regression no longer asserts withdrawn file-token services; its focused v4 mode passes DTA registration, `50:09` success, and `50:0B` no-more. |
| M0 T97 S5 | Complete: v4 acceptance/rejection fixtures pass; the retained engine rejects the v4 execution plan before CPU loop, so zero BOP/transaction is correctly classified as stale engine composition. |
| M0 T97 | Closed: finite profile namespace and pathname provider are source/runtime closed; engine recomposition is transferred to T98. |
| M0 T98 S1 | Complete: retained engine/adapter object and link-closure inventory. |
| M0 T98 S1 P1 | Complete: r5 manifest/makefile/map establishes the exact missing current adapter/profile object set; a narrow derivative relink is the only admitted next build candidate. |
| M0 T98 S1 P2 | Complete: r2 generator/dry-run retains r5 Bochs inputs and exposes only 65 adapter/CLI compilations plus one final link; inherited archive recursion was rejected. |
| M0 T98 S2 | Complete: r3 compiled 66 adapter/CLI objects (including BOP observation) and linked the current composition without a Bochs rebuild. |
| M0 T98 S3 | Complete: bounded v4 trace passed the stale pre-CPU rejection, observed 15 BOPs and three commits, and ended at the benchmark budget. |
| M0 T98 | Closed: current adapter-to-engine composition is source-built and bounded-runtime observed; continuous guest execution remains unproven. |
| M0 T99 S1 | Complete: reached service families map to original components; finite read-only boot namespace is the selected whole provider slice. |
| M0 T99 S2 P1 | Complete: one provider-owned finite namespace state now combines paired config-path production, normal read-only lifecycle and search state; focused producer/consumer test rejects FCB routing. |
| M0 T99 S2 P2 | Complete: v4 runtime now consumes the provider for CONFIG/AUTOEXEC and pathname-search state; narrow engine relink and one bounded `54:0C` trace pass without a Bochs rebuild. |
| M0 T99 S2 P3 | Complete: normal DEM open/seek/read/close moved into the same provider/session family; focused lifecycle passes and the r2 bounded run reaches `54:0C`, `50:12`, `50:00`, then original-unavailable `50:42`. |
| M0 T99 | Closed: finite read-only boot namespace provider is source/test/build closed; one real consumer reaches the next original-unavailable DEM boundary, with no continuous-execution claim. |
| M0 T100 S1 | Active: compose the complete eight-slot original-unavailable DEM provider through the common plane; no individual FASTREAD substitute. |
| M0 T100 | Closed: all eight original-unavailable DEM slots compose through common selection; trace proves `50:42` resume and later close before the separate Bochs prefetch-map stop. |
| M0 T101 S1 | Active: read-only source/trace mechanics audit of the post-BOP `0xAF6AF` prefetch veto. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
