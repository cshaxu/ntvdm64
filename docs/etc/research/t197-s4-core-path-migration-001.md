# T197 S4 Core Path Migration

## Question

Can the complete adopted CPU, FPU, memory and decode families move to the
physical `bx-core` layer without introducing a semantic fork or placing a
product-shell owner in the core?

## Procedure

1. Used `git mv` for the full `cpu/` tree, including `cpu/cpudb/`, then the
   complete `fpu/`, `memory/`, `disasm/` trees and `cpudb.h`.
2. Repaired live test and derivative-tool paths only.
3. Kept the mixed root `bochs.h`, GUI/SIM, full I/O device tree, product
   configuration, firmware, plugins and executable build metadata under the
   retained historical root.
4. Ran the minimal-machine, minimal-mechanics, BOP-listener, #UD default-off,
   #UD seam, ordinary-RAM, machine-composition and execution-plan boundaries.

## Result

`src/bx-core/` now contains the complete adopted native execution families:

```text
bx-core/cpu/       complete CPU implementation and CPUID database
bx-core/fpu/       complete native x87 implementation
bx-core/memory/    native RAM/ROM mechanics
bx-core/disasm/    native decode support
bx-core/cpudb.h    shared CPUID profile declarations
```

All eight focused boundary checks pass. The move does not edit CPU, FPU,
memory, decode or exception bodies; Git preserves source identity as renames.

## Retained Compatibility Edge

The original root `src/bochs/bochs.h` remains temporarily mixed. It imports
declarations from the core, mantle and retained product interfaces. A future
minimal compile vector must provide `src/bx-core`, `src/bx-mantle`, the
retained root and only the specifically admitted I/O header path as include
roots. This is a transitional header boundary, not a new runtime dependency on
GUI, plugins or full devices.

The legacy `Makefile*` and `vs2008` projects still describe the rejected
`bochs.exe` product and therefore retain their old layout references. They are
not rewritten to disguise the product target as the minimum machine build.

## Exception-Register Location Crosswalk

Entries in the intrusion register use historical `src/bochs/...` paths for
the facts at the time each exception was made. Current locations are:

| Historical register prefix | Current source prefix |
| --- | --- |
| `src/bochs/cpu/` | `src/bx-core/cpu/` |
| `src/bochs/fpu/` | `src/bx-core/fpu/` |
| `src/bochs/memory/` | `src/bx-core/memory/` |

No exception gains new behavior from this relocation. The next modification to
one of these files must register its current `bx-core` location before the
patch.

## Follow-up

The next package must define the exact selected configuration and include/object
vector, then compile the native core/mantle closure. It must not repair the
historical full product build or add VDM/BOP meaning to either layer.
