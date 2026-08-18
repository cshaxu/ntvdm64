# Proposal: OpenNT DEM Owner-Package Completion

## Purpose

Recover the complete OpenNT DEM/DOS host-service family (`50:xx`) as one
owner package for the CLI NTVDM.  Existing handlers and routing evidence are
inputs, not proof of package closure.

## Boundary

`bx-vdm` copies and validates BOP ABI records; original OpenNT DEM source
remains the semantic authority.  Direct uses admitted ordinary Win32 host
capabilities; Readonly preserves the same route and returns the source-derived
write failure.  Overlay keeps its existing ABI seam and is not implemented by
this package.

## Admission Plan

1. **S1 — audit:** map every `50:xx` endpoint to original source, ABI,
   failure rule, owner and Direct/Readonly disposition; classify the existing
   local route as retain, migrate, replace or delete.
2. **S2 — code complete:** recover the complete namespace/search/FCB/file,
   process/session, device/state and drive families, reusing OpenNT source or
   the smallest source-derived host seam.
3. **S3 — verification:** run whole-family local regressions in both modes,
   then one bounded native guest integration observation.

## Exit Rule

Every `50:xx` entry has an evidence-backed provider or an original,
profile-specific failure disposition.  Trace order never selects an individual
service implementation.
