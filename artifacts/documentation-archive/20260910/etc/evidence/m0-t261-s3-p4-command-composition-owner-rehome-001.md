# M0 T261 S3 P4 — COMMAND composition owner re-home

## Question

Can the remaining COMMAND process, configuration, environment, and
register/session composition sources be removed from `opennt-host` without
changing the imported COMMAND providers?

## Change

- Moved the five project-owned COMMAND composition inputs from
  `src/opennt-host/command/` to `src/opennt-bop/command/`:
  `opennt_command_composition.{c,h}`, plus binary, configuration, and
  environment composition sources.
- Updated the directly imported COMMAND bodies, BOP ingress, PIF composition,
  tests, and formal manifest to use the new component-qualified header and
  source paths.
- Renamed the include guard to the final BOP composition identity.  No
  function ABI, state layout, provider ordering, or failure path changed.

## Rationale

These sources hold COMMAND dispatch slots, active call state, guest-register
bridging, environment/configuration selection, and child lifecycle state.
They are BOP/session composition, not host capability.  Moving them makes the
remaining `opennt-host` COMMAND input exactly the unchanged historical PIF
triplet.  The NTDLL/Win32-shaped helpers still co-located in the moved files
are explicitly the next extraction slice to `adapter-win32`; this move does
not reclassify them as BOP semantics.

## Verification

- Fresh formal graph: `build/M0-T261-S3/r004`.
- All five moved composition objects compiled.
- `lib/opennt-bop.lib` built successfully from the fresh graph; a second
  Ninja check reported `no work to do`.
- A source/test/formal-manifest search found no `command_misc_shim` path.
- No source remains under `src/opennt-host/command/`; the only retained
  COMMAND-adjacent host source is the byte-identical PIF triplet in its
  original `softpc.new/host/...` layout.

## Scope

The change is a non-behavioral owner correction.  It does not claim that the
remaining compatibility helpers are in their final adapter component, and it
does not enable new BOP services.
