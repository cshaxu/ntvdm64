# Project Status

## Current Work

**Active: M0 T96 S6 — Redirector, debugger, notification, VDD and mouse provider disposition.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S6, Ordinary Mode. |
| Admission And Approval | The owner approved holistic recovery by original OpenNT component, with original host implementation first, CLI-contained platform adaptation second, source-derived rehost only after a recorded blocker, otherwise source-proven unavailable/deferred. S6 follows the closed DEM, COMMAND, and XMS/DPMI planes plus S1/S2 map/ingress closure. |
| Objective | Map Redirector, debugger, notification, VDD, mouse and remaining top-level host-provider dispatchers; classify every service by original component, system binding, composition blocker, and CLI profile disposition. |
| Non-goals | No direct link of nt_bop.c or historical system-bound dispatchers; no VDD/GUI/Win32 handle emulation in adapter; no filesystem, mouse, debugger, or notification success claim before a complete provider component is admitted. |
| Reference Baseline | history/m0-t96-s5-closure-20260811.md; history/m0-t96-s4-closure-20260811.md; history/m0-t96-s2-p1-20260811.md; immutable BOP provider map; Bochs machine boundary evidence. |
| Files And ABI Surface | Remaining BOP provider-family records behind bx_ntvdm_bop_provider_registry_v1; fixed-width copied inputs/results only. No Bochs public API expansion. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Every service in the mapped remaining families receives an original-handler/module/system-binding/disposition row; provider tests prove no unresolved service succeeds; focused compile/test; documentation governance; git diff --check. |
| Expected Markers | One component-level remaining-family disposition record, not new runtime recognizers; Bochs retains native device mechanics; adapter retains only checked copied contracts. |
| Asset Needs | Pinned OpenNT Redirector/debugger/VDD/mouse/notification source, current immutable BOP map, Bochs boundary evidence, and existing adapter tests; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record handler/module, guest/register or system structures used, composition blocker, provider precedence, current disposition, evidence confidence, and every deferred/unavailable failure contract. |
| Stop Conditions | Stop and revise admission if a service requires guessing VDD, GUI, debugger, filesystem, mouse, or device semantics; raw SAS/CCPU access; a new Bochs feature; or an unrecorded source-derived replacement. |
| Exit Criteria | Remaining host-provider families have auditable disposition for every service; admitted operations use common ingress/selection ABI; adapter cannot impersonate native device or system semantics; handoff to SoftPC BIOS ownership is explicit. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review Redirector/debugger/notification/VDD/mouse and remaining top-level dispatch tables with every handler definition, existing legacy endpoints, device ownership, generic exception path, and non-S6 provider boundaries together. |

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

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Complete: all 73 callable DEM services have a component/disposition, and every admitted DEM slice is plane-gated; legacy runtime migration remains S8. |
| M0 T96 S4 | P1--P5 complete: COMMAND map/plane/no-op are closed; CONFIG/AUTOEXEC and target-launch legacy endpoints are reconciled as S8 migration objects. |
| M0 T96 S5 | Complete: 12 XMS and 25 DPMI services are source-mapped and plane-classified without linking historical dispatchers. |
| M0 T96 S6 | Admitted in Ordinary Mode; remaining host-provider family source disposition begins. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
