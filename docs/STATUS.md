# Project Status

## Current Work

**Active: M0 T98 S1 — current adapter-to-engine closure inventory.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T98 S1, Ordinary Mode. |
| Admission And Approval | The owner authorized dependency/ROI-driven task selection. T97 S5 proves that the retained engine rejects the current v4 adapter/profile closure before the CPU loop. T98 selects the resulting engine-composition dependency ahead of further provider expansion. |
| Objective | Inventory the exact retained engine artifact, its adapter object/link closure, source-to-binary delta, and minimal recomposition entry point for one selected DOS-engine bundle. |
| Non-goals | No full Bochs configure/build loop, device enablement, new BOP/provider semantics, Bochs feature expansion, guest media, firmware change, or OpenNT/host API import. |
| Reference Baseline | history/m0-t97-closure-20260812.md; etc/research/t97-s5-profile-and-engine-checkpoint-001.md; T95 minimal-engine evidence and the Bochs intrusion register. |
| Files And ABI Surface | Build manifests, existing r5 engine artifacts, adapter source/object lists and fixed-width bridge declarations. No ABI change is admitted by S1. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Hash and inspect the retained binary/input manifest, compare it to the current adapter object set, identify the exact absent/stale object boundary, and verify no full-build or feature patch was invoked. |
| Expected Markers | Pinned artifact identity, adapter/engine source-object matrix, minimal entry/link closure candidate, and an explicit required/default/rejected classification. |
| Asset Needs | Existing local Bochs 2.6 source, r5 build/evidence trees, current adapter sources and local toolchains only. |
| Reporting Requirements | Distinguish source compile/link evidence from runtime evidence; record every excluded default/device and the exact reason for any engine/adapter change. |
| Stop Conditions | Stop and re-admit before compiling a broad engine target, adding a Bochs capability/device, changing Bochs code, or introducing a bridge ABI. |
| Exit Criteria | A reproducible minimal composition plan with object/link and owner disposition sufficient to admit a later narrow build slice, or an evidence-backed external blocker. |
| Original Owner Request | Holistic BOP recovery with original OpenNT semantics, a minimum Bochs boundary, non-invasive CLI capabilities, and no one-off patches. |
| Similar-Issue Sweep | Reconcile BYOB components/placements/boot materialization, readonly namespace, search session/result/request, DTA registration, DEM plane/gate, and runtime source lists together. |

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
| M0 T98 S1 | Active: retained engine/adapter object and link-closure inventory. |
| M0 T98 S1 P1 | Complete: r5 manifest/makefile/map establishes the exact missing current adapter/profile object set; a narrow derivative relink is the only admitted next build candidate. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
