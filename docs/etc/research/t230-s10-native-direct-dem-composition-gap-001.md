# T230 S10 — Native Direct DEM Composition Gap

## Question

Can the current native CLI reach the directly imported OpenNT DEM dispatcher
without executing a retained `bop-v1` provider or composition runtime?

## Inputs

- `src/bx-mantle/bx_ntvdm_engine_run_v1.c`
- `src/bx-vdm/bop-v1/bx_ntvdm_composition_runtime_v1.h`
- `src/bx-vdm/bop-v1/bx_ntvdm_vdm_generic_ud_bridge_v1.c`
- `src/bx-mantle/bx_ntvdm_generic_ud_bridge.h`
- `src/bx-vdm/bop/shim/dem_ingress_shim.{h,c}`
- fresh `build/M0-T230-S10/concurrent-migration-r7` Direct fixture matrix

## Procedure

Read the native engine installation path and the retained external generic-UD
bridge, then compare their available event data with the checked call required
by the direct OpenNT DEM shim.  Rebuild the ten formal Direct DEM fixtures
against the current migration worktree.

## Observations

1. `bx_ntvdm_engine_run_v1.c` currently includes and installs
   `bop-v1/bx_ntvdm_composition_runtime_v1`.  It therefore still makes the
   v1 composition runtime a native-engine dependency.
2. The retained `bx_ntvdm_vdm_generic_ud_bridge_v1.c` routes a copied generic
   #UD event through the v1 ingress and provider registry before invoking its
   v1 composition handlers.
3. The new `dem_ingress_shim` has no v1 dependency and reaches the original
   OpenNT `DemDispatch` table, but deliberately requires a complete
   `bx_ntvdm_demhndl_call`: a Direct host context, copied CPU/boundary state,
   typed result and checked guest read/write callbacks.
4. `bx_ntvdm_generic_ud_event_v1` carries only copied exception/register and
   instruction-window facts.  It has no session identity, Direct host context
   or checked guest-memory transaction channel.  Consequently it cannot
   truthfully construct a `bx_ntvdm_demhndl_call` for any DEM body that reads
   or writes guest memory.
5. All ten local Direct fixtures, including the original 73-slot dispatcher
   and the new bounded ingress fixture, source-build and return zero from the
   current migration tree.  That proves local direct closure, not native
   machine composition.

## Interpretation

The remaining native gap is a composition boundary, not a missing DEM
algorithm and not a reason to revive v1.  A valid replacement must be a
`bx-vdm` session-owned Direct DEM composition that binds:

```text
mantle copied #UD event
  -> bx-vdm per-session boundary/cpu records
  -> checked guest-memory transport supplied by the existing machine contract
  -> Direct host context + original DemDispatch
  -> typed generic-UD outcome
```

Bochs remains selector-blind and owns machine mechanics.  The adapter may
recognize only the bounded `50:xx` encoding already admitted by
`dem_ingress_shim`; it must not expose raw Bochs memory, recreate DOS service
semantics, or reuse the v1 provider registry.  The source-owned DEM bodies
and `DemDispatch` remain unchanged.

## First bridge closure

`bop/shim/dem_native_session_shim.{h,c}` now implements the adapter half of
that boundary.  Its explicit per-session bind owns a Direct context and
checked guest read/write callbacks; it converts a copied generic-UD event to
the existing fixed boundary/CPU/window records, calls the v1-free
`dem_ingress_shim`, and converts the typed DEM result back to the fixed
generic-UD outcome.  It does not install the mantle's global bridge and does
not use a Bochs pointer.

The formal `t230-s10-dem-native-session-direct-fixture` verifies an admitted
`C4 C4 50 1F` copied event reaches the original ordinary-return dispatcher
slot and publishes a typed resume.  It separately proves rejection of a
COMMAND selector and dispatch after unbind.  This is adapter-composition
closure for the bounded Direct path; it is not yet a native CPU execution
claim.

## Follow-up

Before a bounded native observation can be claimed, bind this session to the
existing machine-contract checked-memory transport, replace the engine's v1
runtime install, and prove a native `50:xx` transaction through the original
dispatcher.  The next implementation decision is the existing
machine-contract location for those callbacks; it must be reused rather than
duplicated.
