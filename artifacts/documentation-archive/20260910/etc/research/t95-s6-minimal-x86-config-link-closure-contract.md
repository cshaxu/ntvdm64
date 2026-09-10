# M0 T95 S6: Minimal x86 Configuration And Link-Closure Contract

## Question

What may a future MSVC/x86 composition-fixture configuration and object/link
closure contain after `BX-MACH-026`, without inheriting the historical Bochs
product configuration?

## Inputs

- approved Bochs 2.6 import under `refs/bochs/` and its source-identity record;
- `refs/bochs/bx_ntvdm_minimal_machine.cc` and the registered
  `BX-MACH-023`, `BX-MEM-024`, `BX-IO-025`, and `BX-MACH-026` boundaries;
- `cpu/init.cc`, `cpu/fetchdecode.cc`, `cpu/smm.cc`, `memory/misc_mem.cc`,
  `iodev/devices.cc`, `pc_system.cc`, `logio.cc`, and the original SIM/tree
  implementation; and
- [Bochs 2.6 minimum mechanics closure](bochs-26-minimum-mechanics-closure.md)
  and [the prior MSVC audit](t95-s6-msvc-win32-closure-audit.md).

## Procedure

This record was derived by reading source conditionals and retained object
evidence only.  It did not create `config.h`, an NMake file, a response file,
a fresh build root, an object, or an executable, and it did not invoke a
compiler, linker, project, `bochs.exe`, or `all`.

## Source-Backed Configuration Contract

The following values have a direct incoming edge from the registered minimal
composition.  A future generated configuration must set all of them exactly
and consistently for every Bochs object.

| Macro | Required value | Source reason |
| --- | --- | --- |
| `BX_CPU_LEVEL` | `3` | Below level 4, `BX_CPU_C::initialize` omits the CPUID factory; levels 5+ also introduce SIM parameter reads. |
| `BX_SUPPORT_X86_64` | `0` | The composition is one 32-bit x86 CPU. |
| `BX_SUPPORT_SMP` | `0` | The composition defines original singleton `bx_cpu`, not `bx_cpu_array`. |
| `BX_SUPPORT_APIC` | `0` | CPU3/non-SMP has no admitted APIC owner. |
| `BX_CONFIGURE_MSRS` | `0` | Prevents the MSR-file SIM query in CPU initialization. |
| `BX_WITH_WX` | `0` | Prevents CPU WX state registration. |
| `BX_SUPPORT_FPU` | `0` | M0 supplies no FPU contract and this removes the reset FPU edge. |
| `BX_SUPPORT_VMX`, `BX_SUPPORT_SVM`, `BX_SUPPORT_AVX` | `0` | No virtual-machine or extended-ISA owner is admitted. |
| `BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS` | `0` | A future bounded observation needs the ordinary instruction-accounting boundary. |
| `BX_PHY_ADDRESS_LONG` | `0` | The reset-window memory model is 32-bit physical addressing; `BX_PHY_ADDRESS_WIDTH` consequently derives to `32`. |
| `BX_LARGE_RAMFILE` | `0` | No host-file-backed memory contract is admitted. |
| `BX_NO_LOGGING` | `0` | The composition deliberately uses original logging accessors. |

The following are deny-by-default feature values.  They are not additional
machine capability: a future configuration that defines them must set them to
zero, rather than retain a value from the imported broad `config.h`.

| Category | Macros required to remain `0` |
| --- | --- |
| GUI/configuration | `BX_WITH_WIN32`, `BX_WITH_X11`, `BX_WITH_MACOS`, `BX_WITH_CARBON`, `BX_WITH_TERM`, `BX_WITH_RFB`, `BX_WITH_SDL`, `BX_USE_TEXTCONFIG`, with `BX_WITH_NOGUI=1` |
| Debug/instrumentation | `BX_DEBUGGER`, `BX_GDBSTUB`, `BX_DISASM`, `BX_DEBUGGER_GUI`, `BX_X86_DEBUGGER`, `BX_INSTRUMENTATION`, `BX_TIMER_DEBUG` |
| Product loading/plugins | `BX_PLUGINS`, `BX_HAVE_LTDL`, `BX_HAVE_DLFCN_H` |
| PC devices | `BX_DMA_FLOPPY_IO`, `BX_SUPPORT_PCI`, `BX_SUPPORT_PCIDEV`, `BX_SUPPORT_USB_UHCI`, `BX_SUPPORT_USB_OHCI`, `BX_SUPPORT_USB_XHCI`, `BX_SUPPORT_PCIUSB`, `BX_SUPPORT_CLGD54XX`, `BX_NETWORKING`, `BX_SUPPORT_PCIPNIC`, `BX_SUPPORT_GAMEPORT`, `BX_SUPPORT_SOUNDLOW`, `BX_SUPPORT_BUSMOUSE`, `BX_SUPPORT_IODEBUG` |
| Unadmitted CPU options | `BX_SUPPORT_3DNOW`, `BX_SUPPORT_MISALIGNED_SSE`, `BX_SUPPORT_MONITOR_MWAIT`, `BX_SUPPORT_ALIGNMENT_CHECK`, `BX_SUPPORT_REPEAT_SPEEDUPS` |

The following macro families are **unresolved**, not inherited.  The source
review cannot choose their value without an effective preprocessor result and
the retained-symbol proof below.  A future configuration record must name
each individual value and its MSVC/platform probe; omission is a failure.

| Unresolved family | Why no value is frozen now |
| --- | --- |
| `BX_USE_CPU_SMF`, `BX_USE_MEM_SMF`, `BX_USE_DEV_SMF` | They alter member-function/linkage shape; composition source is compatible only when every candidate object uses one proven common setting. |
| `BX_SUPPORT_A20`, `BX_FAST_FUNC_CALL`, `BX_ASSERT_ENABLE`, `BX_USE_IDLE_HACK` | They change generated mechanics/debug behavior but have no admitted fixture input yet. |
| `BX_EXIT`, `BX_DEFAULT_CONFIG_INTERFACE`, `BX_DEFAULT_DISPLAY_LIBRARY` | Their live/non-live status depends on the final retained fatal/quit sections; they cannot be supplied by the historical product shell. |
| `BX_MAX_ATA_CHANNEL` and every `BX_USE_*_SMF` device submacro not listed above | They must not be copied from a product configuration; a retained section must first prove whether the declaration is needed despite device feature value zero. |
| `BX_HAVE_*`, `SIZEOF_*`, `WORDS_BIGENDIAN`, `CDECL`, `BOCHSAPI`, `BX_CONST64`, `inline` and related platform/CRT defines | These are MSVC/x86 probe facts, not backend semantic choices. They require a recorded compiler/CRT probe and may not be guessed from another toolchain. |

`WIN32`/`_MSC_VER` are compiler-environment facts, not permission to enable
the Bochs Win32 GUI (`BX_WITH_WIN32`).

## Candidate Source And Component Inventory

| Classification | Candidate source/component | Reason and limit |
| --- | --- | --- |
| Required | `bx_ntvdm_minimal_machine.cc`, `bx_ntvdm_minimal_sim.cc` | Registered composition and original minimal SIM ownership. |
| Required candidate | `gui/siminterface.cc`, `gui/paramtree.cc`, `logio.cc` | Original SIM tree and logging accessors; their product-only sections must be eliminated, not merely left uncalled. |
| Required candidate | `memory/memory.cc`, `memory/misc_mem.cc` | Original memory representation, constructor/cleanup and registered no-SIM allocation. |
| Required candidate | `iodev/devices.cc` | Owns global `bx_devices` and registered empty default port space only. |
| Required candidate | `pc_system.cc` | Owns original null PC-system state; no `initialize`, reset, timer start, or device delegation is admitted. |
| Required component | CPU level-3 generic decoder/cache/reset/exception/I/O component, including `init.cc`, `fetchdecode.cc`, `icache.cc`, `smm.cc`, `io.cc`, `exception.cc` and every surviving handler provider. | Retained evidence identifies 74 level-3 CPU objects and 1,012 decoder-handler undefined edges. It cannot be reduced to a fixture opcode or a hand-picked NOP set. |
| Default, not required | `main.cc`, `config.cc`, `plugin.cc`, any fixture executable entry, product VS libraries/resources. | They are product/front-end ownership, not composition mechanics. |
| Rejected | GUI backend/config UI, firmware/ROM loader, `bochs.exe`, `all`, CMOS/timer/device initialization, storage, network, USB, sound, PCI, APIC/SMP, adapter/OpenNT/CLI/BOP/DOS/WOW/DEM. | No admitted source edge reaches them. |

No source object is yet an exact link input.  In particular, source-file
granularity is unsafe: `logio.cc` and `siminterface.cc` contain fatal/quit
paths to historical product ownership, and `devices.cc`, `pc_system.cc`,
`init.cc`, and `misc_mem.cc` contain both retained mechanics and rejected
product paths.

## Required Controlled Proof Procedure

Before any recipe or fresh-root fixture is admitted, a separately approved
closure investigation must perform all of the following with the effective
minimal configuration:

1. Record the exact MSVC x86 compiler identity, `/M*` CRT choice, all source
   flags and the complete generated configuration hash.
2. Compile each candidate Bochs source with `/Gy`; preserve the resulting
   object files only as analysis artifacts with source/hash provenance.
3. Link the declared fixture root with `/OPT:REF`, `/INCREMENTAL:NO`, a map
   file and verbose library/import diagnostics.  No VS2008 solution or
   full-product library may participate.
4. Compare retained COMDAT sections and their undefined symbols against the
   inventory above.  Each surviving external must resolve to a required
   candidate, the selected CRT, or an explicitly admitted MSVC system import.
5. Prove that no retained section references `main.cc`, configuration/UI,
   plugins, ROM loader, device initialization/reset, CMOS, timers, or
   adapter/OpenNT terms.  In particular, prove product-only `bx_atexit`, GUI,
   and configuration paths from `logio.cc`/`siminterface.cc` are absent.
6. Record the final PE import table and map.  `winmm.lib`, `comctl32.lib`, and
   `wsock32.lib` remain rejected unless a new source-reached admission says
   otherwise; historical VS metadata is not evidence for any fixture library.

## Fresh-Root Fixture Admission Criteria

One fresh-root build may be proposed only when the proof record supplies all
of the following: exact generated config bytes/hash; all candidate sources and
retained objects; source flags and MSVC version; CRT model; explicit system
libraries/imports; map/undefined-symbol evidence; artifact destination and
hash plan; and focused negative checks for every rejected category.  It must
also identify a separately admitted fixture root; `bx_ntvdm_minimal_machine_c`
is a composition owner, not an executable front end.

## Explicit Non-Claims

This contract does not create a configuration header, recipe, response file,
fixture entry point, build root, object, executable, or runtime result.  It
does not prove COMDAT removal, linker closure, CRT compatibility, system
library selection, reset-window execution, a CPU loop/stop contract, firmware
boot, a device, or any guest/adapter/OpenNT/DOS/WOW/BOP behavior.

## Interpretation And Follow-Up

The MSVC/x86 toolchain prerequisite remains available, but the fixture route
is blocked at effective configuration plus retained-symbol closure.  The next
allowed action is not implementation: it is admission of the controlled
`/Gy`/`/OPT:REF` closure investigation and its fixture-root scope.  Until that
evidence exists, `config.h`, NMake, response files, build roots, compiler and
linker invocations remain prohibited.

## Confidence

High for the source-backed required/rejected choices and the negative result;
medium for candidate source grouping; no confidence is claimed for any exact
object, library, import, or unresolved macro value until the controlled proof
is completed.
