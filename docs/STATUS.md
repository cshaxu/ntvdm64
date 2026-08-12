# Project Status

## Current Work

**Active: M0 T97 S4 — CLI-admitted DOS metadata and pathname provider.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T97 S4, Ordinary Mode. |
| Admission And Approval | S3 establishes that OpenNT pathname search needs immutable DOS metadata that the current BYOB profile does not carry. The owner authorized project-wide dependency and ROI decisions. |
| Objective | Add a bounded CLI-admitted DOS metadata declaration for the closed BYOB resource set, then integrate one pathname-only `50:09/0B` provider through the common DEM plane or retain an explicit unavailable result. |
| Non-goals | No host filesystem stat/enumeration, timestamps from the current clock, FCB support, mutation, arbitrary files/drives, host handle/pointer, direct recognizer, Bochs change, or FASTREAD/file-token re-enable. |
| Reference Baseline | etc/research/t97-s1-dem-namespace-component-abi-map-001.md; t97-s2-immutable-namespace-token-contract-001.md; t97-s3-dem-search-form-contract-map-001.md; existing BYOB profile and search session/result types. |
| Files And ABI Surface | `src/cli` profile declaration/parser and adapter-local immutable resource metadata. Cross-island bridge ABI is unchanged; metadata is copied within the CLI/adapter island only. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Profile rejects absent/invalid metadata; adapter accepts only declared entries and yields source-shaped pathname DTA success/no-more results; no runtime path references host namespace; FCB and non-profile services pass through. |
| Expected Markers | Fixed profile metadata schema, identity-bound finite snapshot, plane-gated pathname provider, positive/negative fixtures, and a bounded trace result or explicit blocker. |
| Asset Needs | Existing local source and BYOB profiles only; no new guest media, host directory fixture, third-party input, or Bochs feature. |
| Reporting Requirements | Record source layout/error mapping, metadata origin, capability boundary, and exact unsupported paths. Do not describe a static fixture as CLI execution. |
| Stop Conditions | Stop and revise if host metadata/query, time synthesis, PDB/FCB semantics, extra BOP family, new Bochs intrusion, or a new cross-island ABI becomes necessary. |
| Exit Criteria | A verified pathname-only provider or evidence-backed unavailable decision, with FCB still explicitly deferred and a real-path checkpoint correctly classified. |
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
- M0 T97 S3 is active: audit pathname and FCB searches as separate contracts.
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
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
