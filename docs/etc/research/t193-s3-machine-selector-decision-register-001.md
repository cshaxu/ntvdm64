# T193 S3 machine-selector decision register 001

Date: 2026-08-12  
Packet: M0 T193 S3  
Disposition: closure decision; no source, build, or runtime change.

## Decision

T193 admits **no new machine-selector implementation**.  The complete reached
set has one exceptional, already-linked original machine island (`06`), two
prior-island identities in the frozen image (`02` and `06`), and no other
selector with an independently proven original handler, mechanics closure and
reached-path need.

| Group | Reached selectors | Binding disposition |
| --- | --- | --- |
| Original machine island | `06` | Retain the existing original `illegal_op_int` island and its narrow Bochs mechanics seam.  It remains outside adapter and is not a generic BIOS dispatch facility. |
| Native/firmware candidate | `12`, `15`, `17` | Keep Bochs/guest-firmware ownership.  Do not import the SoftPC table or create standalone handlers without a newly proven reached feature requirement. |
| Historical overlap | `5E`, `5F` | Preserve the ambiguity between the NTVDM top-level row and alternative SoftPC rows.  A machine selector cannot resolve it, and adapter must not guess a service from bare BOP bytes. |
| Historical unavailable / unadmitted device | `66`, `C8` | Keep unavailable or deferred.  EMS and host-mouse activation would be new machine/device scope. |
| Engine control | `FE` | Retain the existing controlled-stop contract as engine control, not a host service or BIOS-table choice. |

The frozen T190 image already has `BX_NTVDM_ENABLE_MACHINE_COMPOSITION=1` and
links both permitted original islands.  Adding a selector branch, a BIOS table
or another adapter provider would therefore be expansion without evidence,
not closure work.

## Handoff

The first coherent next package is not a selector implementation.  It is a
whole execution-continuation package: establish the source and engine contract
for the reached `FE` controlled stop, determine whether its finite plan can
hand off to the live guest without losing the proven BOP/machine boundaries,
and only then decide whether a new engine composition is warranted.  It must
not convert process exit into DOS status, enable devices, or reclassify BOP
ownership.  Terminal-result transport remains queued behind an actual normal
guest return.
