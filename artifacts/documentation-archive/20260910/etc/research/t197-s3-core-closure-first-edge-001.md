# T197 S3 First `bx-core` Closure Edge

## Question

What is the first complete native dependency that must be resolved before the
adopted CPU/memory implementation can be moved to `src/bx-core` and compiled
as a minimal machine rather than as the historical product?

## Inputs and Procedure

Read the current `config.h`, `cpu/init.cc`, `cpu/cpu.cc`, `cpu/io.cc`,
`cpu/exception.cc`, `memory/misc_mem.cc`, root `bochs.h`, `cpu/cpu.h`, and
the VS project source lists. Counted current native translation units by
directory and traced the direct SIM, PC-time, RAM and port-space references.

## Observations

- The checked-in configuration is not the requested small fixture profile:
  it defines `BX_CPU_LEVEL 6`, `BX_SUPPORT_FPU 1`,
  `BX_CONFIGURE_MSRS 1`, and APIC support through the level-six setting.
- At level four and above, `BX_CPU_C::initialize()` calls the CPUID factory
  and reads `SIM->cpu.model`; with configurable MSRs and level five-plus it
  also reads `configurable_msrs.path` and `cpu.ignore_bad_msrs`.
- The existing mantle SIM fragment supplies only
  `cpu.reset_on_triple_fault`. It cannot satisfy CPU initialization under the
  checked-in configuration.
- `cpu/cpu.cc` legitimately imports mantle-owned PC time and core-owned RAM.
  `cpu/io.cc` legitimately imports the default port-space ABI. These are
  typed native mechanics, not adapter behavior.
- The CPU directory contains 93 C++ translation units, FPU contains 21,
  disassembly contains 4, and memory contains 2. Selecting isolated CPU
  instruction files would be a forbidden hand-reimplementation; the first
  core vector must preserve the complete CPU instruction family selected by
  one fixed configuration.
- Root `bochs.h` currently imports SIM/GUI, debugger, memory, PC system,
  GUI and instrumentation declarations in one product header. It is a mixed
  interface and cannot be mechanically placed in `bx-core` or `bx-mantle`.

## Interpretation

The first unresolved owner is the **native minimal configuration and SIM
contract**, not an OpenNT, BOP, DOS or adapter requirement. The correct next
step is a deliberate S4 design/extraction of a fixed configuration interface:

```text
bx-core: complete selected CPU/decode/FPU/memory mechanics
  <- fixed native configuration values
  <- bx-mantle SIM parameter fragment and logging/PC-time/port-space ABI
```

The mantle must provide the three currently reached CPU parameters with their
native Bochs types and the already-required triple-fault parameter. It must
not parse Bochs configuration files, load plugins, enable APIC/devices, or
interpret VDM/BOP/DOS names. The exact selected CPU level and CPUID model
remain an S4 source-backed configuration decision; they cannot be guessed from
the current product `config.h` or the old full build recipe.

## Disposition

`cpu/`, `fpu/`, `disasm/` and `memory/` remain positive **core ownership**
candidates, but no `git mv` is admitted until S4 creates the fixed native
configuration/header seam and proves that it replaces the mixed root header
only for the selected core vector. `main.cc`, GUI/SIM, full I/O devices,
plugins, BIOS and product build metadata remain rejected from that vector.

The physical S4 move set is now complete and mechanical: the full
`cpu/` tree (including its `cpudb/` subdirectory), `fpu/`, `memory/`,
`disasm/`, and the root `cpudb.h`. Complete trees move together so Bochs
instruction semantics are neither selected nor reconstructed one file at a
time. The retained root `bochs.h` is a **mixed compatibility header** for the
first transition: it continues to expose native declarations from the core,
mantle and temporarily retained product tree through named include roots. It
will be replaced only after S4's source/object compile map proves a smaller
header contract. This is not a claim that `bochs.h` itself is core-owned.

## Confidence and Follow-up

High confidence in the direct source calls and current macro values. Medium
confidence in the final object count because it depends on the explicitly
chosen CPU level/model. S4 must first make that configuration selection and
then record the exact source/header vector before any core path migration.
