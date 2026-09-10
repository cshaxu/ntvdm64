# T199 S37 Runtime Composition Wiring 001

## Question

Can the source-built whole-package DEM/COMMAND composition become the
process-local BOP ingress selected by the CLI profile, rather than remain an
unbound fixture object?

## Inputs

- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`
- `src/bx-vdm/bx_ntvdm_boot_namespace_composition_v1.c`
- `src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c`
- `refs/bochs/main.cc` execution-plan call site
- MSVC Build Tools 2022 x64, static `/MT` CRT

## Procedure

1. Trace the generic #UD bridge: it offers a bound whole-package composition
   before the separately bound XMS/DPMI composition.
2. Trace the Bochs pre-CPU-loop execution-plan hook. It first obtains its
   opaque startup plan through the retained plan endpoint, then calls the
   separate composition lifecycle installer.
3. Make the composition lifecycle capture the already-admitted drive snapshot's
   immutable volume metadata, initialize the composition, copy drive/volume
   snapshots and launch plan into it, then bind it.
4. Make runtime reset unbind the composition before it releases the images it
   references.
5. Compile `bx_ntvdm_adapter_runtime.c` with MSVC x64 `/MT` and run the
   existing complete DEM package regression.

## Observations

- The generic bridge in `bx_ntvdm_vdm_generic_ud_bridge_v1.c` first calls
  `bx_ntvdm_boot_namespace_composition_v1_handle`; it does not recognize a
  DEM selector itself.
- The composition lifecycle is reached before CPU execution only when the
  selected Bochs build enables its existing execution-plan hook. It is
  therefore the lifecycle owner of the process-local composition bind.
- The lifecycle is a new narrow adapter unit, not `adapter_runtime.c`:
  symbol inspection showed that the latter would pull legacy host-session and
  per-service provider dependencies into the new whole-package closure.
- The new installation path binds only profiles that already supply the
  immutable command placement, target and search metadata required by that
  composition. Retained pre-contract fixture profiles remain on their legacy
  test route; no partial BOP composition is fabricated for them.
- `artifacts/build/t199-s37-composition-runtime-r1/` contains successful MSVC
  x64 `/MT` compilations of the new lifecycle unit and the registered Bochs
  pre-loop call site. The prior `adapter_runtime.c` option was rejected rather
  than retained as an artificial link dependency.
- `artifacts/build/t199-s37-dem-package-r54/` is a successful MSVC x64 `/MT`
  all-DEM package build and execution after the wiring change.

## Interpretation and confidence

The composition is now owned by a narrow adapter lifecycle in source, and its
snapshots are copied before bind. This prevents a BOP-time ambient volume query
and prevents reset from leaving the generic bridge with a dangling composition
pointer. Confidence is high for source-level ownership and the unchanged DEM
package behavior.

This is deliberately not native runtime evidence: the current minimal Bochs
build recipe does not yet include the expanded composition source/link
closure. A native trace would therefore test a different binary and is not
run.

## Follow-up

Define and build the exact MSVC x64 minimal-Bochs link closure containing the
runtime, generic bridge and whole-package composition. Then use one bounded
native trace only as T199 package integration evidence; it must not create a
new service task.
