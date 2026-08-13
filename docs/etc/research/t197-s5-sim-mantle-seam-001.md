# T197 S5: SIM/Mantle Seam 001

## Result

The first mixed product-shell seam has been reduced without selecting a new
machine profile.  The original Bochs parameter-tree and logging sources are
now physically in `src/bx-mantle/`; the simulator-interface header names
those current locations.  The existing minimal machine still calls the full
`bx_init_siminterface()` and is therefore intentionally **not** a closed
minimal lifecycle yet.

## Native Ownership Map

| Item | Current owner | Evidence | Disposition |
| --- | --- | --- | --- |
| Parameter object implementation | `bx-mantle` | Original `paramtree.{h,cc}` defines the `bx_list_c` and typed parameter objects used by the minimal SIM helper. | Moved by identity. |
| Logging implementation | `bx-mantle` | Original `logio.cc` provides `SAFE_GET_IOFUNC`/`SAFE_GET_GENLOG` backing used by the minimal machine. | Moved by identity; its `BX_CPU(0)->get_eip()` diagnostic remains an explicit core edge. |
| Product SIM/configuration | retained `src/bochs/gui/siminterface.cc` / `config.cc` | `bx_init_siminterface()` creates `bx_real_sim_c`; later product configuration constructs GUI/plugin/device parameters. | Not moved and not a minimal build input. |
| CPU model and CPUID parameter choice | undecided mantle configuration contract | `cpu/init.cc` obtains `cpu.model`; `generic_cpuid.cc` reads a broad configurable CPUID tree, while CPU5 `pentium_mmx.cc` and `amd_k6_2_chomper.cc` are fixed profiles. | Requires an explicit machine-profile decision. |

## Why The CPU Choice Is A Gate

The original configuration default creates `cpu.model` as `bx_generic`.
`generic_cpuid.cc` subsequently consumes CPU-count, vendor, brand and many
CPUID feature parameters.  That preserves the historical configurable
product, but is not a small parameter fragment.

The CPU5 fixed profiles (`pentium_mmx` and `amd_k6_2_chomper`) avoid those
generic CPUID configuration reads, but choosing either would change the
historical default profile.  T197 therefore does not silently substitute a
model merely to make the closure smaller.  The next admitted slice must pick
one of these two contracts:

1. retain `bx_generic` and explicitly own every actually consumed CPUID
   parameter in the mantle; or
2. select and document one fixed CPU5 model as the CLI bare-machine contract.

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
- `bx-ntvdm-minimal-machine-boundary.ps1` and
  `bx-ntvdm-minimal-mechanics-boundary.ps1` pass.
- Documentation governance and `git diff --check` pass.

## Next Decision

The owner must approve the bare-machine CPU profile contract before the full
SIM initializer can be removed from the minimal path.  This is a machine
composition decision, not an adapter or BOP decision.
