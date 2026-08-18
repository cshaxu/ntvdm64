# Proposal: OpenNT WOW16 Owner-Package Completion

## Purpose

Recover a complete, declared WOW16 profile: source-built guest NE artifacts,
WOW/DPMI transition, host hooks, lifecycle and a bounded non-GUI execution
contract.  WOW16 is a target capability, not a permanent negative fixture.

## Boundary

Guest NE loader/runtime and DPMI guest code remain OpenNT guest owners.
Historical WOWEXEC/WOW32 host composition is source evidence and must be
reused where independently composable or recovered through minimal CLI host
capability seams.  Neither Bochs nor `bx-vdm` implements Win16 APIs.

## Admission Plan

1. **S1 — audit:** inventory each required guest artifact and map every
   WOW32 hook, callback, thunk, loader, host dependency, layout and failure
   route for one non-GUI profile.
2. **S2 — code complete:** compose that whole profile using the recovery
   ladder; separately classify GUI, USER/GDI, printer and input capabilities.
3. **S3 — verification:** run a bounded NE/DPMI load-entry-return regression
   and one native guest integration observation.

## Exit Rule

The package cannot claim generic Win16 or GUI support; it closes only the
declared profile and its full original host/guest composition.
