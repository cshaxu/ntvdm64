# Project Status

## Current Work

**Active: M0 T96 S1 — OpenNT BOP provider and module map.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | `M0 T96 S1`, Ordinary Mode. |
| Admission And Approval | The owner selected the first Queue candidate and approved the provider-first BOP recovery model: original OpenNT host code first, contained CLI capability substitution second, source-derived rehost only after a recorded composition failure, otherwise evidenced unavailable/deferred behavior. |
| Objective | Produce a complete, static map from every inventoried BOP selector/service form to its original OpenNT dispatcher, module/owner, provider-precedence evidence, and one current disposition. Migrate no endpoint and implement no new provider in S1. |
| Non-goals | No BOP behavior change; no handler migration; no new host capability; no CCPU/SAS or historical `BIOS[]` execution; no Bochs/OpenNT source change; no DOS/WOW/BIOS/device implementation; no runtime trace retry. |
| Reference Baseline | `etc/research/opennt-bop-definition-inventory-001.md`, `etc/research/t95-s7-opennt-bop-catalog-completeness-001.md`, `etc/operations/opennt-bop-host-service-plane-recovery-candidate.md`, and the closed T95 record in `history/m0-t95-closure-20260811.md`. |
| Files And ABI Surface | One source/evidence map and focused map-verification material under `docs/etc/`; no C/C++ ABI or runtime source surface in S1. |
| Applicable Rules | `rules/EXECUTION.md`, `rules/ARCHITECTURE.md`, `rules/CODING.md`, `rules/DOCUMENT.md`, `design/ARCHITECTURE.md`, and `etc/operations/policy/source-policy.md`. |
| Verification | Verify every inventory family and selector range has exactly one owner/disposition record; cross-check source paths and existing endpoints; run documentation governance and `git diff --check`. |
| Expected Markers | No selector/service falls into an implicit handler; each endpoint is marked original provider, original-with-contained-backend candidate, source-derived candidate, native owner, unavailable, deferred, or provenance unresolved. |
| Asset Needs | Pinned OpenNT source tree, immutable BOP inventory, existing catalogue/listener evidence, and no new third-party or guest asset. |
| Reporting Requirements | Record source paths, original dispatcher/module symbols, composition blockers, provider precedence, confidence, current disposition, and explicitly unresolved provenance. |
| Stop Conditions | Stop and revise admission if mapping requires executing a historical dispatcher, guessing an owner, introducing a runtime provider, choosing a host capability, or changing Bochs/OpenNT/adapter behavior. |
| Exit Criteria | A complete auditable provider/module map exists; every existing isolated endpoint is located in its target family; all unknowns and blocked compositions are explicit; and S2 can define ingress/provider-selection ABI without a per-BOP exception. |
| Original Owner Request | Implement BOP holistically, preserve OpenNT semantics, and make adapter route to original OpenNT host code before using minimal source-derived rehosting. |
| Similar-Issue Sweep | Review DOS/DEM, COMMAND, XMS, DPMI, redirector, debugger/top-level selectors, SoftPC BIOS provenance, existing adapter recognizers, native Bochs owners, and the CLI capability boundary together. |

## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization; its completed backend,
  catalogue, controlled-stop, and CLI EOF evidence is retained without a claim
  that its unfinished file/search capability or full runtime closure completed.
- Bochs remains the generic machine owner. The adapter observes BOP identity
  and will become the provider router; it does not become a DOS/BIOS/CCPU
  implementation.
- Provider precedence is mandatory: original OpenNT host provider; original
  provider with contained CLI backend; minimal source-derived provider only
  after a recorded blocker; otherwise unavailable/deferred.

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in `history/m0-t95-closure-20260811.md`. |
| M0 T96 S1 | Admitted in Ordinary Mode; static provider and module mapping only. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
