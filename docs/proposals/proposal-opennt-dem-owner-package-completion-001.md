# Proposal: OpenNT DEM Owner-Package Completion

## Purpose

Recover the complete OpenNT DEM/DOS host-service family (`50:xx`) as one
owner package for the CLI NTVDM.  Existing handlers and routing evidence are
inputs, not proof of package closure.

## Boundary

`bx-vdm` copies and validates BOP ABI records; original OpenNT DEM source
remains the semantic authority. Supported public Win32 capabilities may serve
that selected original owner; obsolete/private API differences follow the
tracker’s compatibility decision rule. This plan does not create alternate
mutation-profile semantics.

## Admission Plan

1. **S1 — audit:** map every `50:xx` endpoint to original source, ABI,
   failure rule, owner and compatibility disposition; classify the existing
   local route as retain, migrate, replace or delete.
2. **S2 — code complete:** recover the complete namespace/search/FCB/file,
   process/session, device/state and drive families, reusing OpenNT source or
   the smallest source-derived host seam.
3. **S3 — verification:** run whole-family declared-capability regressions,
   then one bounded native guest integration observation.

## Exit Rule

Every `50:xx` entry has an evidence-backed provider or an original,
profile-specific failure disposition.  Trace order never selects an individual
service implementation.

## Queue relationship

DEM source/ABI recovery is recorded in the tracker rather than reserved as an
independent queue position. Remaining DEM branches enter candidate 1 or 2
strictly by their `BOP-DEPENDENCY-…` prerequisites.
