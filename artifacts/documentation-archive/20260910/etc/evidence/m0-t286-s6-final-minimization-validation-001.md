# M0 T286 S6 — final minimization and focused validation

## Inputs and method

This closure compares `src/bochs-core` with the pinned Bochs 2.6 tree at
`O:\repos.external\bochs-2.6-compat\bochs-2.6`.  It counts only adopted
`.c`, `.cc` and `.h` paths with an upstream-relative counterpart, and counts
every source/header file in the private `bochs-core-overlay` root.

It also introduces `New-T286BochsBoundaryNinja.ps1`.  The generator creates a
per-architecture Ninja graph and SHA-256 source manifest from exactly these
inputs:

- all selected `bochs-core` source files;
- all private `bochs-core-overlay` implementation bodies, compiled into the
  same `bochs-core.lib`;
- the five current `adapter-bochs` mechanical assembly sources;
- one `t286_memory_boundary_fixture.cc` typed façade fixture.

The graph has no MVDM host, BOP, app, session, `src.old`, prebuilt archive,
GUI/plugin archive or product-device archive input.

## Final reduction

| Measure | S1 baseline | Final | Change |
| --- | ---: | ---: | ---: |
| Non-identical upstream-relative mirror files | 29 | 27 | -2 |
| Mirror insertions | 431 | 393 | -38 |
| Mirror deletions | 353 | 356 | +3 |
| Private overlay source/header files | 15 | 10 | -5 |
| Private overlay physical lines | 1,164 | 1,067 | -97 |

S4 removed six standalone unconsumed observation bodies.  During final
measurement S6 also found an embedded, unconsumed first-fault observer in
`bochs-core-overlay/cpu/exception_hook.cc`; its record shape, constants,
default-off gate and callback branch are now removed.  The former T197
projection manifest likewise no longer injects retired observer or bridge
macros.  The selector-blind #UD copied-record path remains compiled with its
own default-off internal gate; no selector/provider is enabled by this build.

## Final retained boundary

- `bochs-core` contains no production source file absent from the pinned
  upstream tree.
- Only `bochs-core` includes the private overlay header; no adapter, session,
  app, test or MVDM source does so.
- `adapter-bochs` is the only production caller of the core.  It selects
  memory size and lifecycle and exposes typed copies, never a `BX_MEM_C`
  object or a raw RAM pointer.
- Private overlay bodies are limited to opaque #UD copying/result mechanics,
  real/V86 profile mechanics, SIM-free memory initialisation, checked RAM,
  finite port-space/PIC/keyboard lifecycle and reset-window PC lifecycle.
  Their corresponding mirror declarations/calls and README exception index
  remain the authoritative register.

## Verification

The following checks passed:

- `Test-BochsOrdinaryRamBoundary.ps1` checks whole-span A20, handler, monitor
  and host-memory vetoes in the private checked-RAM body.
- `Test-machineMechanicalActionBoundary.ps1` checks that the typed façade
  preflights before copy and has no service/provider semantics.
- The T286 Ninja source manifests and `bochs-core.lib`, `adapter-bochs.lib`
  and `t286-memory-boundary-fixture.exe` built on MSVC `/MT` for both x86 and
  x64.
- The x86 and x64 fixture each initialise the CPU5/Pentium-MMX bare machine,
  perform a typed ordinary-RAM write/read round trip, reject VGA and
  out-of-range spans, and clean up successfully (`exit 0`).
- Documentation inventory/governance and `git diff --check` pass at closure.

Historical Bochs warnings from unchanged upstream source remain warnings; no
warning was converted into a source change.  This is a focused mechanical
closure, not a full MVDM host link or guest-continuity claim.
