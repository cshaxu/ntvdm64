# Proposal — shared implementation ownership review

## Objective and admission

Review project-owned implementation shared across production components and
place each mechanism at an appropriate single source owner. This is an unnumbered
queued candidate, not an active task; the current packet continues unchanged.
Allocate a T and authorize implementation only through normal admission.

## Scope and decision criteria

Inventory actual production consumers, build selections, interfaces, source
provenance and dependency directions for shared implementation. Distinguish
production use from test-only linking, original-source ownership from local
mechanisms, and shared mechanics from component policy. Check all supported
architectures and selected build graphs. Evaluate the existing shared component
before creating another component or interface.

Initial review candidates, not predetermined defects or moves:

- Native hook installation, context and shared classification, including
  sources presently under `nthook32-dll` and `run16-exe`.
- Run16 native-request and bootstrap clients also selected by frontend builds.
- Base RPC client slices presently under NTSRV. Their current ownership is
  explicitly specified in `docs/design/CODING.md`; change it only if a full
  dependency and source-ownership review justifies an authority update.
- Other production cross-component source and static-library reuse found by
  the inventory, including appropriate existing common mechanisms.

For each candidate record consumers, actual policy and state owners, lifetime,
failure contracts, compatibility, proposed disposition and evidence. Retain a
component-owned implementation when its ownership is justified. Consolidate
only where it reduces coupling or duplication without moving policy into a
generic framework, altering behavior, or increasing maintenance complexity.
Do not infer reuse merely because a test links a production object.

## Delivery and verification

For admitted changes, preserve existing observable behavior and component
responsibilities. Keep original-source provenance and registered mirror diffs;
do not modify or rebuild guest images. Verify real production consumers and
supported build profiles, positive and failure paths, lifecycle and relevant
cross-component scenarios. Apply the existing coherent build, test, publication,
review and delivery rules. Report retained and moved ownership, dependency and
code-size changes, test evidence and remaining limits.

The existing project-wide implementation-minimization candidate overlaps this
review; at admission reconcile its inventory and findings rather than repeating
or dropping work. This proposal neither changes its queue position nor assumes
the outcome of its decisions. Cross-project library extraction is not implied.
