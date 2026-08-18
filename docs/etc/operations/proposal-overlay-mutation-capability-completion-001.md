# Proposal: Overlay Mutation-Capability Completion

## Purpose

Complete the deferred Overlay host-mutation profile as the third supported
product mode.  Direct performs permitted real host mutations; Readonly refuses
the complete mutation family; Overlay records every selected mutation in
session-owned state and presents one coherent merged namespace without writing
the host.

Virtual is retired from product selection.  Existing virtual code and evidence
remain retained unless they demonstrably block Direct, Readonly or Overlay;
this proposal adds no virtual behavior.

## Boundary

Overlay is a host-capability/provider concern in `bx-vdm`, preserving the
selected OpenNT owner’s layouts, ordering and failures.  It is not a virtual
disk, BIOS, DOS filesystem or Bochs device.  No raw host object crosses the
typed bridge, and the overlay lifetime is bounded by one CLI session.

## Admission Plan

1. **S1 — audit:** reconcile retained overlay code with every DEM, COMMAND,
   Redirector and related mutation route; map original source/ABI/failure,
   retained workaround and Direct/Readonly/Overlay disposition.
2. **S2 — code complete:** complete one shared overlay store, namespace,
   handle, search, metadata, rename/delete/create and lifecycle provider
   contract; migrate owner packages to it without duplicating policy.
3. **S3 — verification:** run coherent Direct/Readonly/Overlay mutation,
   visibility, rollback-on-session-end and stale-token regressions, followed
   by bounded native integration for each completed owner family.

## Exit Rule

Overlay changes never persist to the host, survive no session boundary, and
cannot silently become virtual-drive behavior.  Virtual remains absent from
CLI selection and from new implementation work.
