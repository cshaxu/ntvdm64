# T225 S20 COMMAND Live-Route Inventory 001

## Question

Does the formal MSVC x64 `/MT` CLI/native graph reach COMMAND only through the
one S20 package session, or can a legacy selector-specific runtime path bypass
that owner?

## Inputs

- `tools/build/t225-s7-full-module-manifest.json`;
- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`;
- `src/bx-vdm/bx_ntvdm_boot_namespace_composition_v1.c`;
- `src/bx-vdm/bx_ntvdm_command_package_session_v1.c`;
- retained `src/bx-vdm/bx_ntvdm_adapter_runtime.c`; and
- original ordered table `src/opennt/base/mvdm/dos/command/cmddisp.c`.

## Observations

1. The formal manifest contains the composition runtime, boot-namespace
   composition and COMMAND package session, but omits
   `bx_ntvdm_adapter_runtime.c`.
2. Composition installation binds the mutation profile, admitted CLI streams,
   host/session context and immutable launch plan to one
   `bx_ntvdm_boot_namespace_composition_v1` instance.
3. The live common BOP path invokes the bound COMMAND package session.  That
   session classifies all callable `54:00..10` services through the facade and
   owns the bootstrap, launch/execution, console/keyboard and lifecycle
   subproviders.  No CLI or mantle source contains a direct COMMAND selector
   branch.
4. `bx_ntvdm_adapter_runtime.c` still has historical exact service gates for
   COMMAND bootstrap and current-directory helpers.  It is not an input to the
   formal library, fixture or native CLI target, so it cannot be used as an
   S20 behavior source.
5. The live `54:08/0A` Direct path is the already-recorded bounded terminal
   child transaction.  It does not create a host-child-to-DOS re-entry queue;
   the source-based rejection of such a queue is retained in
   `t202-s7-command-reentry-admission-decision-001.md`.  Readonly reaches the
   package-owned unavailable result instead.

## Disposition

| Surface | Current status | S20 decision |
| --- | --- | --- |
| Composition runtime -> boot namespace -> COMMAND session | Formal live route | Retain as the only COMMAND owner route. |
| COMMAND package-session subproviders | Formal live route | Keep all service semantics inside this package. |
| `bx_ntvdm_adapter_runtime.c` exact branches | Historical derivative only | Do not migrate or test new behavior there; retain as cleanup evidence. |
| Host-child re-entry queue | Not admitted | Do not synthesize a queue from the immutable DOS launch plan. |

## Interpretation

The P3 full-family regression tests the same COMMAND owner that the formal
CLI graph installs.  It does not validate the legacy adapter-runtime branches
and does not turn the native trace into a selector-discovery exercise.

## Follow-up

The route-inventory gate is complete.  S20 may now perform its one bounded
Direct/Readonly native package validation using the existing formal native
composition.  Any newly observed selector is evidence only and cannot create
a new implementation task.