# Project Status

## Current Work

**Active: M0 T96 S9 — Full inventory/provider/capability regression.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S9, Ordinary Mode. |
| Admission And Approval | The owner approved holistic BOP recovery by original component and rejected trace-specific handlers. S9 follows closed source planes and S8 no-bypass closure. |
| Objective | Verify every immutable inventory identity receives the expected ingress route, provider disposition, plane classification or explicit non-provider outcome under the first CLI profile. |
| Non-goals | No new provider, host capability, BIOS/Bochs semantic, OpenNT direct link, or trace-driven behavior change. |
| Reference Baseline | history/m0-t96-s8-closure-20260811.md; immutable BOP inventory/map; ingress/registry and plane tests. |
| Files And ABI Surface | Regression harnesses and read-only inventory evidence; no new public ABI or Bochs API. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | A complete automated matrix covers all inventory selectors/services and asserts route/selection/plane/non-provider outcomes; focused compile/tests; governance; diff check. |
| Expected Markers | One reproducible profile-disposition report and no inventory identity that silently receives success or has no classification. |
| Asset Needs | Immutable inventory/map, source plane code/tests and legacy register; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record total/covered identity counts, expected class, any source-table ambiguity and explicit unavailable/deferred outcomes. |
| Stop Conditions | Stop and revise admission if regression exposes a missing identity, ambiguous source owner, unintended success, or needs new semantic code to pass. |
| Exit Criteria | All BOP inventory families and top-level identities have a reproducible profile disposition; S10 real-path trace question is narrowed to admitted providers. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review inventory JSON, BOP catalogue, ingress, registry, all four plane classifiers, no-bypass register and existing provider tests together. |

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
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
