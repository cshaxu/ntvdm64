# Project Status

## Current Work

**Active: M0 T96 S8 — Existing endpoint migration and no-bypass verification.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S8, Ordinary Mode. |
| Admission And Approval | The owner approved holistic BOP recovery by plane and rejected permanent trace-specific handlers. S8 follows complete source disposition planes S3--S7. |
| Objective | Inventory every existing adapter runtime/end-point BOP recognizer and migrate it behind its matching closed plane, or freeze/reject it with source evidence; prove no new direct legacy bypass is admitted. |
| Non-goals | No new host capability, DOS/BIOS/Bochs semantics, OpenNT direct linking, or change to a legacy service merely to make a trace advance. |
| Reference Baseline | history/m0-t96-s7-closure-20260811.md; histories/m0-t96-s3-closure-20260811.md through m0-t96-s6-closure-20260811.md; existing adapter runtime source/tests. |
| Files And ABI Surface | Existing adapter runtime/end-point code and plane gates only; fixed-width copied ABI; no Bochs public API expansion. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Every legacy recognizer has a service/selector owner, original contract citation, target plane, and migration/freeze disposition; focused no-bypass test; documentation governance; git diff --check. |
| Expected Markers | One complete legacy endpoint register and plane-gated migration changes; no raw selector/service switch survives outside ingress/closed plane modules. |
| Asset Needs | Existing adapter source/tests, closed plane maps and pinned OpenNT source; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record file/symbol, recognized identity, original source owner, current behavior, conflict, target plane and rejection/migration evidence. |
| Stop Conditions | Stop and revise admission if migration needs guessed source semantics, a raw guest pointer, a new Bochs feature, or a one-off handler not owned by a closed plane. |
| Exit Criteria | Every legacy endpoint is plane-gated, explicitly frozen/rejected, or removed; no-bypass verifier covers the resulting adapter source tree; handoff to inventory regression is explicit. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review adapter runtime, all `*_service.c` BOP recognizers, BIOS selector map, host-service catalog and their tests against the four closed source planes together. |

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
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
