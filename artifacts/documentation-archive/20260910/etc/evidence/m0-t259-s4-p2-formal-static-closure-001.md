# M0 T259 S4 P2 — Formal Static-Closure Reconciliation

Status: completed S4 part
Question: can every formally admitted fixture and the native CLI target link
against the actual source-owned static-module closure, without changing an
OpenNT mirror or Bochs implementation to conceal a missing dependency?

## Inputs

- `tools/build/t225-s7-full-module-manifest.json`.
- `tools/build/New-T225S7FullNinjaGraph.ps1`.
- `bop/shim/opennt_rtl_compat.h` and the COMMAND/DEM source-mirror fixtures.
- Fresh formal build root `build/M0-T259-S4/formal-r11/`.

## Procedure

1. Generate a new x64 `/MT`, CPU5/Pentium-MMX formal Ninja graph from the
   manifest.
2. Link every declared fixture and the native target.  Classify every link
   failure by its actual referenced module rather than applying a universal
   library list.
3. Add only the missing, real module libraries to each affected fixture.
   Fixtures that intentionally supply a mock are not forced to link a
   conflicting module; the obsolete checked-RAM mock in the COMMAND native
   session fixture was instead retired so it can exercise the actual mantle
   failure boundary.
4. Make the canonical OpenNT RTL header valid in both C11 and C++14 while
   preserving its declarations and counted-string layout checks.
5. Regenerate a fresh graph and require all outputs to link, then run focused
   source-mirror and previously underlinked fixtures.

## Observations

| Finding | Correction | Boundary result |
| --- | --- | --- |
| Some `bx-vdm` fixtures referenced `opennt_int06_provider_v1_*` through the startup-machine route without listing `opennt-host`. | Add `opennt-host` only to those fixture library lists. | OpenNT host composition remains a separate owned static module. |
| Some direct DEM/COMMAND fixtures reach Redirector's typed physical-IRQ worker and lacked `bx-mantle`/`bx-core`. | Add those mechanically required libraries to the affected lists. | The provider retains no local IRQ substitute. |
| The runtime-session fixture also reaches its CLI bootstrap provider. | Declare the existing `cli`/`bx-vdm` archive cycle in that fixture's ordered list. | No CLI source moved into `bx-vdm`. |
| `opennt_rtl_compat.h` used C11 `_Static_assert` directly and failed when a C++14 fixture included the same facade. | Use a private C/C++ assertion spelling that expands to `static_assert` in C++ and `_Static_assert` in C. | OpenNT-shaped declarations/layout proof are unchanged. |
| The COMMAND-native-session fixture locally redefined mantle checked-RAM functions. | Remove the obsolete test-local definitions and link the actual machine module. | Its no-machine-stage failure path is now exercised through the real mantle boundary. |

The fresh r11 graph built **453 nodes** and produced **79 executables**: the
78 declared fixtures plus `ntdos64-native`.  A subsequent `ninja -C` reported
`ninja: no work to do.`

Focused r11 executions passed:

- `t230-s8-demerror-lock-direct-import-fixture` — same-named NTDLL facade
  and DEM error/lock contract.
- `t245-s7-printer-openclose-fixture` — formerly underlinked fixture.
- `t231-s10-command-native-session-fixture` — v2 COMMAND session with no v1
  fallback and without local checked-RAM symbol substitution.
- `t236-s2-native-cmdexec32-fixture` — C++ consumer of the canonical RTL
  facade and native mechanical stop path.

## Interpretation

The formal build graph now records actual, per-fixture module ownership.
`bx-vdm` is not treated as a self-contained archive when a fixture reaches an
owned CLI, mantle/core, or OpenNT-host interface; conversely, an intentional
fixture seam is not overridden by a global closure policy.  This is the same
source-first/low-intrusion rule used by the facades: retain the original
interface boundary and make dependencies explicit instead of hiding them in a
replacement implementation.

No OpenNT source mirror, `bx-core`, or `bx-mantle` source file was changed.

## Follow-up

S4 continues with the remaining HOST/MACH facade reconciliation.  Product
shell behavior remains separately deferred; a green static link does not
enable any unadmitted BOP or recursive CCPU behavior.
