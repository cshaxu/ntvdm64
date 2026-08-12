# Project Status

## Current Work

**Active: M0 T96 S7 — SoftPC BIOS selector ownership and native-owner reconciliation.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S7, Ordinary Mode. |
| Admission And Approval | The owner approved minimal Bochs replacement of SoftPC, preserving Bochs machine ownership and using the adapter only for copied BOP composition boundaries. S7 follows closed S1--S6 source planes. |
| Objective | Reconcile every inventoried SoftPC BIOS BOP selector/table form with a Bochs-native owner, an explicit non-owner/deferred disposition, or a separately recorded Bochs intrusion exception. |
| Non-goals | No import of SoftPC BIOS dispatcher/firmware into adapter; no BIOS, PIC, keyboard, mouse, video, disk, CPU or memory reimplementation in adapter; no added Bochs feature without an exception record. |
| Reference Baseline | history/m0-t96-s6-closure-20260811.md; etc/research/opennt-bop-definition-inventory-001.md; immutable BOP inventory; Bochs boundary and intrusion registers. |
| Files And ABI Surface | BIOS selector ownership records only; no provider callback, guest pointer, CPU result, firmware or Bochs public API expansion. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Every inventory BIOS selector/table form obtains an owner/disposition record; no adapter implementation claims a native machine operation; focused record test; documentation governance; git diff --check. |
| Expected Markers | A complete BIOS owner matrix, no generic BIOS dispatcher, and an explicit list of selector collisions with OpenNT host BOP identities. |
| Asset Needs | Pinned SoftPC BIOS table and selector headers, existing Bochs boundary/intrusion evidence, and current inventory; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record selector/form, historical owner symbol, Bochs-native domain, collision status, current disposition, evidence confidence, and every unresolved dependency. |
| Stop Conditions | Stop and revise admission if reconciliation requires guessing a device/firmware contract, importing SoftPC firmware, implementing it in adapter, or an unregistered Bochs change. |
| Exit Criteria | All inventory BIOS forms have an auditable native-owner/non-owner disposition; overlap with host BOP selectors is explicit; S8 legacy endpoint migration scope is unambiguous. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review all 41 BIOS selector definitions and 349 annotated table rows, collision selectors, mouse/keyboard/video/disk/interrupt/CPU/memory domains, Bochs exceptions, and existing adapter BIOS endpoints together. |

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
| M0 T96 S7 | Admitted in Ordinary Mode; SoftPC BIOS selector and Bochs-native ownership reconciliation begins. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
