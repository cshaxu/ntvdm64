# T225 S7 P2 Ninja build-graph result

## Decision

Ninja is the sole intended formal incremental-build executor. T225 S7 P1's
custom object cache remains historical evidence only and must be deleted once
the admitted full module graph replaces it.

## Implemented graph

`New-T225S7NinjaGraph.ps1` writes, below a disposable local build root:

- `module-manifest.json`: MSVC identity hash, Ninja version, x64 `/MT`,
  CPU5/P-MMX configuration, ordered include roots, fixed source list, module
  dispositions and forbidden inputs;
- `build.ninja`: `deps = msvc` plus `/showIncludes`, a static-library rule and
  a separate fixture-link rule;
- `msvc-x64-mt.cmd`: the fixed MSVC environment entry point.

The admitted focused graph is:

```text
src/bx-vdm/bx_ntvdm_cpu_state_abi.c → bx-vdm.lib
fixture object + bx-vdm.lib          → bx-vdm-cpu-state-abi-fixture.exe
```

The manifest explicitly records that `bx-core`, `bx-mantle`, `opennt-host` and
`cli` await extraction from their existing audited source closures. They are
not silently represented by globbing or empty libraries.

## Verification

Ninja 1.13.2 and MSVC 19.43.34809 were exercised under x64 `/MT` in
`build/t225-s7/ninja-r3/`.

1. Cold `ninja -v` compiled two objects, archived `bx-vdm.lib`, and linked the
   fixture. The fixture exited 0.
2. `ninja -t deps obj/bx_ntvdm_cpu_state_abi.obj` recorded the actual source
   header `src/bx-vdm/bx_ntvdm_cpu_state_abi.h` through `deps = msvc`.
3. A timestamp-only, reversible header change rebuilt both dependent objects,
   then `bx-vdm.lib`, then the fixture executable. Source metadata was
   restored afterward.
4. An unchanged subsequent Ninja invocation reported `no work to do`.

This proves actual MSVC header dependency propagation, not a hand-written
object-cache approximation. It does not claim the full CPU5/P or T225 fixture
closure has yet migrated to Ninja.