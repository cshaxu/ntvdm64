# T197 S5: SIM/Mantle Seam 001

## Result

The first mixed product-shell seam is now reduced under the owner-confirmed
CPU5/Pentium MMX bare-machine contract.  The original Bochs parameter-tree
and logging sources are physically in `src/bx-mantle/`; the simulator-interface
header names those current locations.  The minimum path no longer calls
`bx_init_siminterface()`.

## Native Ownership Map

| Item | Current owner | Evidence | Disposition |
| --- | --- | --- | --- |
| Parameter object implementation | `bx-mantle` | Original `paramtree.{h,cc}` defines the `bx_list_c` and typed parameter objects used by the minimal SIM helper. | Moved by identity. |
| Logging implementation | `bx-mantle` | Original `logio.cc` provides `SAFE_GET_IOFUNC`/`SAFE_GET_GENLOG` backing used by the minimal machine. | Moved by identity; its `BX_CPU(0)->get_eip()` diagnostic remains an explicit core edge. |
| Product SIM/configuration | retained `src/bochs/gui/siminterface.cc` / `config.cc` | `bx_init_siminterface()` creates `bx_real_sim_c`; later product configuration constructs GUI/plugin/device parameters. | Not moved and not a minimal build input. |
| CPU model and CPUID parameter choice | finite mantle configuration contract | `cpu/init.cc` obtains `cpu.model`; `generic_cpuid.cc` reads a broad configurable CPUID tree, while CPU5 `pentium_mmx.cc` is a fixed profile. | Owner-confirmed CPU5/Pentium MMX. |

## Why The CPU Choice Is A Gate

The original configuration default creates `cpu.model` as `bx_generic`.
`generic_cpuid.cc` subsequently consumes CPU-count, vendor, brand and many
CPUID feature parameters.  That preserves the historical configurable
product, but is not a small parameter fragment.

The CPU5 fixed profiles (`pentium_mmx` and `amd_k6_2_chomper`) avoid those
generic CPUID configuration reads.  On 2026-08-12 the owner selected
`pentium_mmx`.  The mantle therefore requires `BX_CPU_LEVEL == 5` and
`BX_SUPPORT_X86_64 == 0`, creates the original typed `cpu.model` enum with
`bx_cpudb_pentium_mmx`, and does not create the generic CPUID subtree.

In both cases the independent CPU initialization reads remain
`cpu.reset_on_triple_fault`, `cpu.ignore_bad_msrs` and, when configured,
`cpu.msrs`.  The existing `reset_on_triple_fault=false` is retained only as
the original non-reset triple-fault branch; it is not a synthetic reset
policy.

## Negative Scope

No BOP selector, DOS, OpenNT, firmware, BIOS, CMOS, PIC, device, GUI, plugin,
host capability or full `bochs.exe` behavior was introduced.  No moved source
body changed except the include path required to find the moved CPU header.

## Verification

- `git diff --summary` reports three identity moves.
- `Test-BochsFmtLlLexicalBoundary.ps1` passes with the current mantle path.
- `bx-ntvdm-minimal-sim-boundary.ps1` proves no full SIM initializer remains
  and requires the typed CPU5/Pentium MMX parameters.
- `bx-ntvdm-minimal-machine-boundary.ps1` and
  `bx-ntvdm-minimal-mechanics-boundary.ps1` pass.
- Documentation governance and `git diff --check` pass.

## Compile Probe

The retained `r21` CPU5 generated configuration has `BX_CPU_LEVEL=5` and
`BX_SUPPORT_X86_64=0`, so it exercises the intended source branch.  A direct
MSVC object probe cannot use it unchanged: its MinGW-shaped configuration does
not define `WIN32`, causing the common root header to include `unistd.h`.
That is evidence for S6's configuration projection/build shim; it is not a
reason to reintroduce product SIM or change the finite SIM source.

## Next Closure Edge

Compile the finite SIM path against a CPU5/non-x86-64 generated configuration,
then classify its remaining original logging and root-header linker imports.
The prior MSVC probe with a retained Unix-shaped generated configuration stops
at `unistd.h`; that is a build-configuration seam, not a SIM semantic failure.
