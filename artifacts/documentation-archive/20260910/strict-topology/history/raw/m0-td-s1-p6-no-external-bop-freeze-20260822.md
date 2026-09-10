# M0 Td S1 P6 — No-external-dependency BOP Admission Freeze

## Question

How many tracker rows may enter the first no-external-dependency BOP
candidate, before allocating a numeric T?

## Method

The live tracker was read as the authority. A BOP or dependency entered the
local slice only when its declared minimum closure tier starts at `P0` through
`P6`; every `P7+` extension remains an explicit external transfer. The source
reference `BOP-DEPENDENCY-042` was manually excluded because it is the
uncomposable NT4 CCPU/SAS/DLL/CSR host product composition, not a local
provider merely because it retains a source-defined failure reference.

## Result

| Class | Count | Current local-pass |
| --- | ---: | ---: |
| BOP entries | 75 | 75 |
| OpenNT dependencies | 30 | 30 |
| **Frozen candidate total** | **105** | **105** |

The BOP composition is 55 DEM, 17 COMMAND, 2 XMS and one top-level `59`.
The dependency composition is 15 DEM and 15 COMMAND rows. The detailed ID
ranges, local-contract rule and named exclusion are maintained in the live
tracker, not duplicated here as a second mutable catalogue.

## Decision

This is a governance freeze, not a numeric T admission. If the owner admits
the queue-head candidate, S1 must perform source/route/fixture no-bypass
revalidation of these 105 rows. It must not assume they require new feature
implementation; a discovered local gap remains in scope, while an external
gap transfers to candidate 2 through the tracker.
