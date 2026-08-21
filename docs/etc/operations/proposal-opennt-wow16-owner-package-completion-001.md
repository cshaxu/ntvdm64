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

## COMMAND Lifecycle Dependency

T236 owns the bounded Direct, non-pipe COMMAND child record used by
`54:08`/`54:0A`/`54:0B`. It deliberately excludes `VDMForWOW`, WOW command
line publication and the historical WOWEXEC/WOW32 broker. This package may
not silently reinterpret the Direct child record as WOW support or add WOW
policy to COMMAND. Its admission must source-map the separate
`GetWowKernelCmdLine`/shortcut and host-hook contracts, then either consume a
future explicitly versioned session capability or produce the original-shaped
unavailable outcome. No host pointer, handle or WOW callback may enter guest
state.

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
