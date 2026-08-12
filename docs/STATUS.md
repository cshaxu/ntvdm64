# Project Status

## Current Work

**Active: M0 T99 S2 — profile-backed read-only boot namespace composition.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T99 S2, Ordinary Mode. |
| Admission And Approval | T99 S1 selected one coherent producer/consumer family: original COMMAND config materialization with DEM normal read-only file and pathname-search consumption. Historical direct composition is source-proven blocked. |
| Objective | Replace or withdraw that family's legacy endpoint path as one finite profile-backed, read-only boot namespace provider; preserve original paired service and bounded buffer/error contracts. |
| Non-goals | No individual trace-hit handler, no BOP ingress rewrite, no Bochs/device/firmware change, no DOS/BIOS semantics in the adapter, no host-invasive API, host directory, host handle, FCB emulation, write/create/rename, raw IOCTL or process execution. |
| Reference Baseline | etc/research/t99-s1-opennt-dem-command-composition-map-001.md; T97 pathname/profile contracts; T96 DEM/COMMAND maps and endpoint register. |
| Files And ABI Surface | Existing fixed-width ingress/plane/result ABI; profile namespace, search transaction, read-only namespace and legacy gate source. Any added composition state must be copied/opaque and documented. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Focused producer/consumer success and unavailable tests; scan proving no direct legacy bypass; source build/relink through the T98 composition recipe; one bounded trace only after source closure. |
| Expected Markers | One provider owner/state boundary, paired `54:0C/0D` plus read-handle/search disposition, no-host-namespace proof, and explicit FCB/write/IOCTL refusal. |
| Asset Needs | Existing local OpenNT source, finite BYOB profile, current adapter/CLI sources and r5-derived build island only. |
| Reporting Requirements | Distinguish source compile/link evidence from runtime evidence; record every excluded default/device and the exact reason for any engine/adapter change. |
| Stop Conditions | Stop and re-admit before any Bochs/device/firmware/guest-media change, host API capability, FCB/write/IOCTL expansion, new bridge ABI, or a second runtime retry. |
| Exit Criteria | The coherent provider is source/test/build closed or an evidence-backed composition blocker is recorded; no continuous-DOS claim. |
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
| M0 T98 S1 | Complete: retained engine/adapter object and link-closure inventory. |
| M0 T98 S1 P1 | Complete: r5 manifest/makefile/map establishes the exact missing current adapter/profile object set; a narrow derivative relink is the only admitted next build candidate. |
| M0 T98 S1 P2 | Complete: r2 generator/dry-run retains r5 Bochs inputs and exposes only 65 adapter/CLI compilations plus one final link; inherited archive recursion was rejected. |
| M0 T98 S2 | Complete: r3 compiled 66 adapter/CLI objects (including BOP observation) and linked the current composition without a Bochs rebuild. |
| M0 T98 S3 | Complete: bounded v4 trace passed the stale pre-CPU rejection, observed 15 BOPs and three commits, and ended at the benchmark budget. |
| M0 T98 | Closed: current adapter-to-engine composition is source-built and bounded-runtime observed; continuous guest execution remains unproven. |
| M0 T99 S1 | Complete: reached service families map to original components; finite read-only boot namespace is the selected whole provider slice. |
| M0 T99 S2 P1 | Complete: one provider-owned finite namespace state now combines paired config-path production, normal read-only lifecycle and search state; focused producer/consumer test rejects FCB routing. |
| M0 T99 S2 | Active: migrate the provider as one runtime/session family; do not re-enable legacy endpoints separately. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
