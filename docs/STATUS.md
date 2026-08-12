# Project Status

## Current Work

**Active: M0 T97 S2 — Immutable CLI namespace resource and file-token design.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T97 S2, Ordinary Mode. |
| Admission And Approval | S1 is closed by source audit. The owner already authorized dependency- and ROI-based sequencing; this is its bounded, source-derived replacement branch. |
| Objective | Define one immutable CLI-admitted namespace resource and token lifecycle that can replace the non-composable original `50:12/00/16/02` host-handle contract without exposing host paths or handles. |
| Non-goals | No runtime BOP enablement, FASTREAD response, search/FCB behavior, arbitrary host-drive projection, mutation, ambient CWD, Bochs change, or new direct recognizer. |
| Reference Baseline | etc/research/t97-s1-dem-namespace-component-abi-map-001.md; existing readonly namespace/file candidate; T96 plane and legacy register. |
| Files And ABI Surface | Adapter-local resource/token design, evidence and focused tests only. Any bridge result remains the existing versioned copied transaction ABI. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Compare source inputs/outputs and failures for the four selected DEM services against the proposed token contract; prove no host path/handle leaves the adapter and no direct recognizer is added. |
| Expected Markers | A provider replacement-or-withdrawal decision, exact immutable resource identity/lifecycle, fixed token properties, and focused positive/negative boundary tests. |
| Asset Needs | Existing BYOB profile/resource inputs and source evidence only; no external filesystem scan, new guest media, or Bochs asset. |
| Reporting Requirements | State all unsupported modes and error dispositions; distinguish a design/test fixture from runtime enablement. |
| Stop Conditions | Stop and revise if an original behavior needs a host handle/path, a new machine feature, an unbounded guest memory read, or a separate endpoint-specific hook. |
| Exit Criteria | One verified immutable token/resource contract that either safely replaces the frozen candidate in the common plane or records why it must remain unavailable; S3 handoff is explicit. |
| Original Owner Request | Holistic BOP implementation with an adapter boundary, minimum Bochs, original OpenNT semantics, and no accumulation of one-off patches. |
| Similar-Issue Sweep | Reconcile DEM open/seek/read/close, COMMAND boot/current-directory consumers, BYOB profile identity, legacy register, and all existing readonly namespace tests together. |

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
- M0 T97 S2 is active: define one immutable resource/token replacement for
  the paired DEM open/seek/read/close contract.

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
| M0 T97 S2 | Active: immutable CLI namespace resource and file-token design. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
