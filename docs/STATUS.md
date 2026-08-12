# Project Status

## Current Work

**Active: M0 T96 S2 — Shared BOP ingress and provider-result ABI.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T96 S2, Ordinary Mode. |
| Admission And Approval | The owner approved holistic BOP host-service recovery: adapter routes by the original OpenNT structure; original providers are preferred; source-derived rehosting follows only a recorded composition blocker; unavailable/deferred behavior is explicit. S2 follows the completed S1 map. |
| Objective | Define and implement one shared adapter BOP ingress that accepts the bounded Bochs event, resolves a catalogued selector/service to a provider disposition, applies only a typed checked result, and has one explicit unavailable path. |
| Non-goals | No direct link of nt_bop.c or a historical dispatcher; no new DOS, BIOS, CPU, PIC, device, CCPU/SAS, GUI, VDD, Win32, or host-capability semantics; no individual service provider migration; no runtime success claim. |
| Reference Baseline | history/m0-t96-s1-p1-20260811.md; etc/research/t96-s1-opennt-bop-provider-module-map-001.md; immutable BOP inventory and provider/module-map artifacts; existing adapter catalogue/listener evidence. |
| Files And ABI Surface | Adapter-local fixed-width ingress, provider-selection and result structures; focused unit tests; source and evidence documents. No Bochs public API expansion and no OpenNT source linkage. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Positive routing for catalogued test providers; negative selector/service/provider tests; checked-result and pass-through invariants; source map coverage check; documentation governance; git diff --check; focused build/test. |
| Expected Markers | One ingress only; no scattered selector recognizers; every result declares advance/resume or pass-through; unavailable is distinguishable from success; no guest memory pointer or host pointer crosses the ABI. |
| Asset Needs | Existing pinned OpenNT source, immutable inventory/map artifacts, existing minimal Bochs listener and adapter tests; no new third-party, guest, firmware, or Microsoft asset. |
| Reporting Requirements | Record ABI fields, decoded instruction width, route/disposition, result state transition, negative-test coverage, all remaining source/provider blockers, and any boundary exception. |
| Stop Conditions | Stop and revise admission if S2 needs BOP service semantics, a historical dispatcher, guest-memory parsing beyond the bounded copied instruction, a host capability, a Bochs device, or a new Bochs/OpenNT invasive change. |
| Exit Criteria | A single tested ingress and provider-result ABI exists; every catalogue entry reaches one explicit route/disposition; current isolated endpoints have no bypass route; later plane subtasks can install providers without changing Bochs or the ingress ABI. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review BOP listener, catalogue, existing isolated endpoint fixtures, CLI capability boundary, generic Bochs exception path, all mapped families, typed guest-memory/result interfaces, and rejection paths together. |

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
| M0 T96 S2 | Admitted in Ordinary Mode; common ingress and provider-result ABI only. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
