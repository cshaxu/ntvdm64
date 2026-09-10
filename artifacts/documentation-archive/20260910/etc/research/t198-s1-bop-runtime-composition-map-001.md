# T198 S1 BOP Runtime Composition Map 001

## Question

Can the existing bounded BOP runtime be directly recomposed with the closed
CPU5 `bx-core` / `bx-mantle` machine, while preserving selector-blind Bochs
and the adapter-owned holistic service plane?

## Inputs

- T197 r12 native CPU5 core/mantle manifest and lifecycle witness.
- The retained T95/T98 runtime root
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r5` and its
  `t95-s7-runtime-trace-build-root.json`.
- `tools/build/New-T98S1CurrentAdapterEngineDerivative.ps1`.
- Current `src/bx-vdm`, `src/cli/ntdos64_bochs_engine.c`, and retained
  `refs/bochs/main.cc`.

## Procedure

1. Read the retained runtime-root manifest and derivative generator.
2. Trace every retained Bochs object/archive and the current `main.cc`
   execution-plan consumer.
3. Compare that input graph against r12's 129 core/mantle sources, finite SIM
   lifecycle and forbidden product input list.
4. Confirm the CLI shim's role separately from the in-process BOP boundary.

## Observations

| Surface | Existing bounded runtime | T197 native closure | Disposition |
| --- | --- | --- | --- |
| CPU/decode/memory | Retained full Bochs archives | Whole CPU5 adopted source set | Same native owner, but not a reusable object ABI |
| Machine lifecycle | Historical `main.cc`, product SIM/configuration and startup | Finite SIM, RAM, empty port-space, CPU reset/cleanup | Incompatible; no shared runtime entrypoint |
| BOP ingress | Generic `#UD` event compiled into a retained `cpu/exception.o` | No instruction loop or generic event runner yet | Must be recomposed after a native execution contract exists |
| Service plane | Source-built `bx-vdm` ingress/catalog/registry/DEM/COMMAND providers | Unchanged adapter owner | Reusable by source, not by retained object identity |
| CLI engine shim | Validates an immutable bundle then writes `bochsrc` and launches `ntdos64-bochs.exe` | No product config or executable target | Historical product-wrapper path; excluded |
| Product inputs | `main.o`, `gui/libgui.a`, `iodev/libiodev.a`, `memory/libmemory.a`, CPU archives and firmware configuration | Explicitly excludes `main.cc`, GUI, device archives and product target | Rejected from T198 composition |

The T98 derivative generator makes the product dependency concrete: its final
link command retains `$(BX_OBJS)`, `$(SIMX86_OBJS)`, `iodev`/`memory`/`gui`
archives and, in applicable modes, rebuilt `main.o`/`cpu\exception.o`.  It
was correct evidence for the former runtime, but cannot be relinked against
the finite mantle without reintroducing the product shell.

The old `main.cc` owns both the opaque execution-plan application and the
CPU loop.  Its includes and startup path also own text configuration, GUI,
plugin/device initialization, ROM configuration and product shutdown.  The
finite mantle deliberately owns none of those.  Conversely, r12 proves only
initialization and cleanup; it exposes no declared reset-image/entry, finite
instruction budget or controlled-stop entrypoint.

## Interpretation And Confidence

High confidence: direct object reuse or an incremental T98 derivative is not
an admissible way to run BOP on the new architecture.  It would silently
restore exactly the `main.cc`/GUI/device product shell that T197 excluded.
This is an execution-composition blocker, not a missing BOP provider and not
a reason to hand-write individual selector handlers.

The valid next closure slice is a **native execution contract** under
`bx-mantle`: fixed reset/entry bytes supplied through a typed, finite
machine-owned request; a finite instruction budget; and an explicit
machine-owned controlled stop/report.  It must be selector-blind and contain
no BOP/OpenNT/DOS terms.  Only after that contract source-builds can
`bx-vdm`'s already global ingress/registry/provider plane be composed with it.

## Follow-Up

Admit T198 S2 only for the native execution-contract map and its minimal
source/build closure.  Do not modify BOP provider behavior, enable devices or
firmware, or use the CLI `bochsrc` engine shim as a runtime dependency.
