# Project Status

## Current Work

**Active: M0 T96 S3 — DOS/DEM host-service plane recovery.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S3, Ordinary Mode. |
| Admission And Approval | The owner approved holistic recovery by original OpenNT component, with original host implementation first, CLI-contained platform adaptation second, source-derived rehost only after a recorded blocker, otherwise source-proven unavailable/deferred. S3 follows S1 mapping and S2 common ingress/selection closure. |
| Objective | Recover the DOS/DEM host-service plane as one provider module: map its original dispatcher and handler contracts to a stable DEM provider entry, classify every service disposition, and admit only the original-compatible or explicitly source-derived portions. |
| Non-goals | No direct link of nt_bop.c or demdisp.c; no DOS kernel/BIOS/CPU/PIC/CCPU/SAS implementation; no unrelated COMMAND/XMS/DPMI/Redirector work; no host mutation, arbitrary path access, or runtime success claim. |
| Reference Baseline | history/m0-t96-s1-p1-20260811.md; history/m0-t96-s2-p1-20260811.md; history/m0-t96-s2-p2-20260811.md; immutable BOP provider map; existing DEM endpoint evidence and CLI capability boundary. |
| Files And ABI Surface | DEM provider-family module behind bx_ntvdm_bop_provider_registry_v1; fixed-width copied inputs/results only; focused tests and source/evidence records. No Bochs public API or OpenNT source ABI expansion. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Every DEM service receives an original-handler/source-blocker/disposition row; provider tests prove no unresolved service succeeds; focused compile/test; documentation governance; git diff --check. |
| Expected Markers | One DEM provider entry rather than new runtime recognizers; original demNotYetImplemented slots preserve unavailable status; all guest access uses checked copied contracts; CLI file capability remains contained and read-only. |
| Asset Needs | Pinned OpenNT DEM source, current immutable BOP map, existing adapter tests and contained CLI namespace inputs; no new third-party, firmware, guest media, or Microsoft asset. |
| Reporting Requirements | Record handler/module, registers or guest structures used, composition blocker, provider precedence, current disposition, evidence confidence, and every deferred/unavailable failure contract. |
| Stop Conditions | Stop and revise admission if a service requires guessing a DOS contract, raw SAS/CCPU access, arbitrary host filesystem mutation, a new Bochs feature, or an unrecorded source-derived semantic replacement. |
| Exit Criteria | A single DEM provider module has an auditable disposition for every DEM service; admitted operations use the common ingress/selection ABI; unresolved operations cannot bypass as success; handoff to COMMAND is explicit. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review demdisp table and all 74 definitions, existing isolated DEM handlers, host session transactions, read-only namespace, startup bridge, generic Bochs exception path, and non-DEM provider boundaries together. |

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

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Admitted in Ordinary Mode; unified DEM plane classifies all 73 callable services and gates the only executable original-no-op slice; source-contract slices remain bounded and runtime migration remains deferred to S8. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
