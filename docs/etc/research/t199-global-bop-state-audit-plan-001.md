# M0 T199 Global BOP State Audit and Package Plan

T199 audits every catalogued OpenNT BOP identity against the current source,
composition and evidence. It produces one global matrix and an implementation
plan by complete owner package, not by trace order.

The matrix must cover the inventoried 16 top-level selectors, 74 DOS/DEM
definitions, 18 COMMAND definitions, 12 XMS definitions, 25 DPMI definitions,
49 Redirector definitions, 16 Debugger definitions, 41 BIOS constants and the
349 conditional BIOS table rows. Every callable item receives exactly one
current status: original reusable, capability-adapted original, source-derived
rehost, explicit unavailable, deferred, machine-owned, unimplemented, or
not-callable table row.

The implementation plan must organize these statuses into complete packages:
DEM namespace/search/session/device/state; COMMAND bootstrap/launch/console/
execution/environment/lifecycle; XMS A20/allocator/UMB/INT15; DPMI PM/LDT/
exception/memory; Redirector unavailable; Debugger deferred; and bx-core/
bx-mantle machine/BIOS. For each package it records prerequisites, ABI seams,
failure contract, acceptance regression and the one subsequent trace role.

Trace is prohibited during this audit. After a package is source-mapped,
dispositioned and regression-closed, one trace may validate behavior change.
Only after every BOP package has a closure disposition may trace-led runtime
repair begin.
