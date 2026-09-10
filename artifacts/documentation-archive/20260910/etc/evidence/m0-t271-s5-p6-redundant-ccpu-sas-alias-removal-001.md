# M0 T271 S5 P6 — redundant CCPU/SAS alias removal

## Question

Does the top-level `opennt-bop/opennt_ccpu_sas_facade.h` provide a necessary
source-shaped boundary, or does it only rename the existing DEM call-scoped
facade?

## Inputs

- The former forwarding header.
- `dem/opennt_dem_ccpu_sas_facade.{c,h}`.
- VDMREDIR mailslot compatibility source and its focused fixture.

## Procedure

1. Searched every consumer of the forwarding aliases.
2. Replaced the sole VDMREDIR compatibility consumer and app Redirector
composition with direct same-shaped `runtime_demhndl_*` calls.
3. Removed the forwarding header and its tautological test assertion.
4. Regenerated/rebuilt the formal Redirector fixture and ran it.

## Observations

- The removed header contained no state, original source fragment, or
  independent ABI: every symbol was a typedef or macro alias of the DEM
  facade.
- No live source retains its aliases after removal.
- Formal Ninja rebuilt the affected VDMREDIR/app objects, linked
  `t251-s3-redir-ingress-fixture.exe`, and the fixture exited zero.

## Interpretation and confidence

The header was redundant project-authored indirection in a mirror root.  Its
removal reduces the BOP mirror's non-original surface without changing the
source-facing CCPU/SAS contract or introducing a second mapper/frame.

## Follow-up

Continue the final ledger audit of the remaining family-local compatibility
headers and private overlays; do not infer a broader Redirector capability
claim from this structural cleanup.
