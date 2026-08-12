# Project Status

## Current Work

**Active: M0 T96 S5 — XMS/DPMI provider and machine-owner disposition.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S5, Ordinary Mode. |
| Admission And Approval | The owner approved holistic recovery by original OpenNT component, with original host implementation first, CLI-contained platform adaptation second, source-derived rehost only after a recorded blocker, otherwise source-proven unavailable/deferred. S5 follows the closed DEM and COMMAND planes plus S1/S2 map/ingress closure. |
| Objective | Map XMS and DPMI dispatchers and classify every service by original host/Bochs machine ownership, composition blocker, and profile disposition. |
| Non-goals | No direct link of nt_bop.c, xmsdisp.c, or dpmi32.c; no A20/CPU/descriptor implementation in adapter; no unrelated COMMAND/Redirector work or runtime success claim. |
| Reference Baseline | history/m0-t96-s4-closure-20260811.md; history/m0-t96-s2-p1-20260811.md; history/m0-t96-s2-p2-20260811.md; immutable BOP provider map; Bochs machine boundary and existing XMS/DPMI evidence. |
| Files And ABI Surface | XMS/DPMI provider-family records behind bx_ntvdm_bop_provider_registry_v1; fixed-width copied inputs/results only. No Bochs public API expansion. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Every XMS/DPMI service receives an original-handler/machine-owner/source-blocker/disposition row; provider tests prove no unresolved service succeeds; focused compile/test; documentation governance; git diff --check. |
| Expected Markers | One XMS/DPMI provider-plane record rather than new runtime recognizers; A20, CPU mode, descriptors, and extended memory remain native machine owner facts; adapter has only checked copied contracts. |
| Asset Needs | Pinned OpenNT XMS/DPMI source, current immutable BOP map, Bochs boundary evidence, and existing adapter tests; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record handler/module, registers or machine structures used, composition blocker, provider precedence, current disposition, evidence confidence, and every deferred/unavailable failure contract. |
| Stop Conditions | Stop and revise admission if a service requires guessing CPU/A20/descriptor semantics, raw SAS/CCPU access, a new Bochs feature, or an unrecorded source-derived semantic replacement. |
| Exit Criteria | XMS and DPMI provider planes have auditable disposition for every service; admitted operations use common ingress/selection ABI; adapter cannot impersonate native machine semantics; handoff to Redirector/debugger is explicit. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review XMS/DPMI dispatch tables and every function definition, existing legacy XMS endpoints, A20/extended memory/mode-switch Bochs ownership, generic exception path, and non-XMS/DPMI provider boundaries together. |

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

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Complete: all 73 callable DEM services have a component/disposition, and every admitted DEM slice is plane-gated; legacy runtime migration remains S8. |
| M0 T96 S4 | P1--P5 complete: COMMAND map/plane/no-op are closed; CONFIG/AUTOEXEC and target-launch legacy endpoints are reconciled as S8 migration objects. |
| M0 T96 S5 | Admitted in Ordinary Mode; XMS/DPMI source dispatcher and native-machine-owner disposition begins. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
