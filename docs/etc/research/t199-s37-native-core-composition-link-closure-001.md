# T199 S37 Native-Core Composition Link Closure 001

## Question

Can the current source-built whole-package composition link against the
project's x64 minimal Bochs CPU5/Pentium-MMX machine closure without retaining
an unresolved adapter, BOP, OpenNT, RAM, A20, or product-executable object
dependency?

## Inputs

- `tools/Invoke-T197S6MinimalMachineLinkProbe.ps1`, invoked with
  `-WholeCpu5Core -HostArchitecture x64`.
- `tools/t198-s50-bx-vdm-composition-manifest.json`, currently naming 94
  composition-provider sources.
- `src/bx-mantle/bx_ntvdm_mechanical_action_v1.cc`.
- The T199 S37 runtime-composition lifecycle record:
  [runtime composition wiring](t199-s37-runtime-composition-wiring-001.md).

## Procedure

1. Rebuilt the complete admitted minimal machine from source with MSVC Build
   Tools 2022 x64 and static `/MT` CRT.  The resulting CPU5/Pentium-MMX
   closure contains bx-core CPU/FPU/memory and bx-mantle SIM, parameter,
   logging, PC-time, default-port-space and machine lifecycle units.
2. Compiled all 94 current composition-provider sources with the same x64
   `/MT` contract, including the native XMS/DPMI package router and the narrow
   composition runtime.
3. Recompiled the mantle mechanical-action unit against the fresh projected
   CPU5 configuration.
4. Linked the composition objects, mechanical-action object, and every
   minimal-machine object except its generated `main` probe into a no-entry
   audit DLL.  The static CRT link set is `libcmt`, `libvcruntime`, `libucrt`,
   `legacy_stdio_definitions`, `kernel32`, and `bcrypt`.

## Observations

- `artifacts/build/t199-s37-native-core-x64-r1/` source-builds and links the
  minimal x64 CPU5/Pentium-MMX machine probe successfully.
- The initial composition-only audit exposed six real machine symbols: the
  mantle A20 and extended-memory capabilities plus the four core ordinary-RAM
  methods used by the mechanical-action ABI.  They are native mantle/core
  owners, not adapter or BOP work.
- Adding the fresh minimal-machine object closure removed every project symbol
  from the composition link.  The only subsequent errors were static-CRT
  library selection errors caused by the intentionally no-entry DLL form.
- With the static UCRT libraries above, the final link returned zero and
  produced
  `artifacts/build/t199-s37-composition-closure-r1/composition-native-core-audit.dll`.

## Interpretation

The prior claim that SIM, logging, PC-time, memory and default-port-space were
an unclosed product-shell blocker was too broad.  They are already represented
by a bounded project-owned bx-mantle lifecycle and an admitted bx-core source
closure.  The real omission was that the composition audit had not included
that closure.

This proves static source/link closure only.  The audit DLL has no entry point,
does not install a CLI profile, does not execute a guest instruction, and does
not run a native trace.  It must not be cited as runtime, BOP-family, DEM, or
COMMAND closure evidence.

## Follow-up

Use this exact object membership to define the CLI executable's source-built
link recipe.  Before any native trace, add a process-level lifecycle fixture
that installs and resets the composition through the actual minimal-machine
startup contract.  Keep the remaining DEM S37 work organized by its component
packages and final provider dispositions; this link evidence does not admit a
trace-driven service task.
