# T197 S1 Bochs Ownership and Physical-Placement Map

## Question

Which current Bochs 2.6 source areas are pure adopted mechanics, pure native
lifecycle composition, or inseparable product-shell mixtures for the first
physical `bx-core` / `bx-mantle` move?

## Inputs

- Current local adopted tree: `refs/bochs/` (664 tracked files).
- Pinned comparison tree: `O:\repos.external\bochs-2.6-compat\bochs-2.6`.
- T196 effective-closure audit and remaining diff/register crosswalk.
- Current minimal entry points: `bx_ntvdm_minimal_machine.cc`,
  `bx_ntvdm_minimal_sim.cc`, `memory/misc_mem.cc`,
  `iodev/minimal_port_space.cc`, `cpu/init.cc`, and `main.cc`.

## Procedure

1. Enumerated each top-level Bochs directory and the root build-bearing files.
2. Read the current minimal entry points and their direct lifecycle calls.
3. Checked the upstream `Makefile.in` product target: it links main/config,
   GUI/SIM, devices, plugins, debugger, FPU, and optional product libraries.
4. Classified a location as *pure* only if its current role has one owner;
   a component which merely contains a usable function but also assembles an
   excluded product path is marked *mixed*.

## Current Map

| Current area | Disposition | Evidence / immediate action |
| --- | --- | --- |
| `cpu/` and `cpu/cpudb/` | Pure core candidate | CPU initialization and instruction decode are Bochs mechanics. Move as a unit in S2 after include/build metadata inventory. |
| `memory/` | Core candidate with one mixed file | RAM/ROM mechanics are core. `memory/misc_mem.cc` retains full product initialization alongside `init_memory_without_sim`; do not split or move that file until S3 selects its native lifecycle seam. |
| `fpu/` | Pure core candidate | Native x87 implementation; no VDM or product composition meaning. Include only if the S3 object map proves the selected CPU level requires it. |
| `disasm/` | Pure core candidate | Native decode/disassembly support, not a host-service plane. Keep out of the first closure unless its CPU object set proves it is required. |
| `pc_system.{cc,h}` | Pure mantle candidate | Native PC-time/reset/timer lifecycle owner. S3 must enumerate the exact no-device subset before move/build admission. |
| `iodev/minimal_port_space.cc` | Pure mantle candidate | T196-established empty default port-space lifecycle; it has no selector or device meaning. Move in S2 with its required declarations. |
| `iodev/iodev.h` and remaining `iodev/` | Mixed | The header exposes port mechanics, while `devices.cc` owns full device/plugin/CMOS/VGA assembly. Keep in place; S3 decides a narrow mantle interface rather than moving the whole directory. |
| `bx_ntvdm_minimal_machine.{cc,h}` | Pure mantle candidate | Project-added native object assembly for SIM, RAM, port space and CPU reset. Move in S2 from the adopted tree to `bx-mantle/`. |
| `bx_ntvdm_minimal_sim.{cc,h}` | Pure mantle candidate with extraction debt | It defines the deliberately tiny triple-fault parameter fragment but calls full `bx_init_siminterface`. Move as mantle code in S2; S3 must replace that full-SIM reach with a native minimal lifecycle or record the first impossible edge. |
| `gui/siminterface.cc` and `gui/` | Mixed | `bx_init_siminterface` lives here, but the directory is the historical GUI/configuration product surface. It is not a core move and is not a whole-mantle move. |
| `main.cc` | Mixed | Defines global machine objects but also owns the historical executable, config parsing, plugins and run loop. S3 extracts only named native lifecycle ownership; `main.cc` stays put. |
| `config.cc`, `plugin.cc`, `load32bitOShack.cc`, `osdep.*` | Mixed/product shell | Configuration, dynamic loading and product-host behavior. No S2 move. |
| `logio.*` | Mantle candidate with shared dependency | Logging is native lifecycle support, but globals are currently also initialized by `main.cc`. S3 names a single mantle initialization/ownership point before any move. |
| `host/` | Mixed platform support | Platform-specific helpers can serve mechanics and historical product shell. No directory-wide move. |
| `instrument/`, `bx_debug/`, `gdbstub.*` | Product/debug mixture | Not required by the stated minimal closure; retain and reject from first mantle closure. |
| `bios/`, `build/`, `vs2008/`, `patches/` | Product/build inputs | Firmware, configuration or full-product build material. Retain outside first core/mantle closure. |
| `doc/`, `docs-html/`, README/license/changelog files | Provenance/documentation | Retain with adopted material; no runtime owner move. |

## S2 Exact Candidate Set

S2 may use `git mv` for these project-owned or unambiguously native files,
subject only to the build-metadata path repair stated in the packet:

```text
refs/bochs/bx_ntvdm_minimal_machine.cc
refs/bochs/bx_ntvdm_minimal_machine.h
refs/bochs/bx_ntvdm_minimal_sim.cc
refs/bochs/bx_ntvdm_minimal_sim.h
refs/bochs/iodev/minimal_port_space.cc
refs/bochs/pc_system.cc
refs/bochs/pc_system.h
```

The source-tree-wide physical `src/bx-core/` move is **not** yet an S2 action:
the core candidate directories use relative includes and generated build
metadata whose closure must first be enumerated. That is a dependency map,
not reluctance to rename. The S2 move will place only the listed pure files
under `src/bx-mantle/`; S3 establishes the reproducible adopted-core root and
the exact core object vector before any bulk `refs/bochs → src/bx-core` move.

## Observations

`bx_ntvdm_minimal_machine_c::initialize` currently calls the minimal SIM
initializer, RAM initialization without SIM, empty port-space initialization,
then CPU initialization/reset. The SIM initializer invokes the full GUI/SIM
initializer. The upstream `bochs.exe` target is consequently unsuitable as a
closure proxy: its link target explicitly includes GUI/SIM, I/O devices,
plugins, debugger, FPU and optional product libraries.

The direct adapter rename moved 168 tracked files from
`src/bx-ntvdm-adapter/` to `src/bx-vdm/` and 105 test files to
`tests/bx-vdm/`. A tracked non-documentation search finds no old component or
source/test path. The five focused PowerShell boundary checks for build
isolation, #UD interception, empty port space, machine composition and
adapter-owned BOP observation pass.

A fresh MinGW/Ninja CMake configure at `artifacts/build/t197-s1-bx-vdm`
compiled the renamed sources. It exposed three pre-existing omissions in the
two explicit test link vectors: `bx_ntvdm_cpu_result_v2.c` in the
machine-composition target, and `bx_ntvdm_exception_observer_v1.c` plus
`bx_ntvdm_bop_catalog_v1.c` in the runtime target. The vectors are now
complete. `bx-ntvdm-machine-bop-v1-test.exe` links and exits zero. The broader
`bx-vdm-runtime-test.exe` also links but exits one without a diagnostic
marker; its source is unchanged in behavior and the failure is recorded as a
pre-existing/stale semantic-fixture limitation, not repaired by this path-only
S packet.

## Interpretation and Confidence

High confidence: CPU/decode, RAM/ROM mechanics and the three listed
project/minimal lifecycle files have the stated owner dispositions. High
confidence: `main.cc`, GUI/SIM, full I/O devices and plugin/configuration code
are mixed or excluded. Medium confidence: the exact smallest FPU/disassembly
object set depends on the S3 compile map; they are core-owned but not yet
admitted into the first build.

## Follow-up

S2 performs the listed `git mv` operations and path-only metadata repair.
S3 then resolves the two blocking mixed edges: full `bx_init_siminterface`
and `memory/misc_mem.cc`'s coexistence of full product and no-SIM lifecycles.
