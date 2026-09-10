# M0 T95 S6: MSVC Win32 Minimal-Closure Audit

## Question

Can the locally installed Microsoft toolchain build the declared native
observation fixture without inheriting the imported Bochs 2.6 full-product
configuration or guessing a new CPU/device composition?

## Inputs

- approved Bochs input: `O:\repos.external\bochs-2.6-compat\bochs-2.6`,
  represented by the manifest-verified import under `refs/bochs/`;
- imported Visual Studio metadata: `refs/bochs/vs2008/bochs.sln` and the
  component `.vcproj` files;
- imported configuration evidence: `refs/bochs/config.h`;
- minimum-mechanics ledger:
  `docs/etc/research/bochs-26-minimum-mechanics-closure.md`;
- native-fixture/UCRT evidence:
  `docs/etc/research/t95-s5-gather-read-admission.md`; and
- local toolchain discovery on 2026-08-10: Visual Studio 2022 BuildTools
  17.13.35919.96, MSVC 14.43.34808, with
  `VC\Tools\MSVC\14.43.34808\bin\Hostx64\x86\cl.exe` and `nmake.exe`.

## Procedure

1. Inspected the Visual Studio project metadata without invoking the solution,
   `MSBuild`, `nmake`, `bochs.exe`, or `all`.
2. Listed the project source and linker declarations and compared them with
   the M0 initialization, memory, port-space, dispatch, and triple-fault
   ledgers.
3. Inspected the imported `config.h` feature selections and classified them
   against the deny-by-default backend profile.

## Observations

| Item | Observation | Classification |
| --- | --- | --- |
| Toolchain | A supported MSVC/x86 compiler and `nmake.exe` are locally available. | Available prerequisite only |
| `vs2008/bochs.sln` | Historical solution builds the product `bochs.exe`; its Release linker declares `winmm.lib`, `comctl32.lib`, and `wsock32.lib`. | Full product, rejected as fixture invocation |
| `vs2008/bochs.vcproj` | Product sources include `main.cc`, `config.cc`, `plugin.cc`, GUI/configuration support, and Win32 resources. | Default product path, rejected |
| `vs2008/iodev.vcproj` | Contains ACPI, storage, PCI, VGA, serial, floppy, sound-adjacent and other device sources in one historical library. | Not an object-level minimum closure |
| Imported `config.h` | Enables GUI/configuration support and explicit SMF selections for PCI, USB, NICs, sound, VGA, storage, APIC-adjacent and other PC-device facilities. | Deny-by-default violation; unusable as minimal configuration |
| CPU project | Contains the generic decoder/object set, including `exception.cc`; the M0 ledger already establishes that a hand-picked NOP-only subset would replace Bochs decoding rather than use it. | Component candidate; object closure unresolved |
| Triple-fault path | The M0 ledger records a still-unresolved dependency on SIM/CMOS/reset support. | Stop gate |
| Memory and port-space | The M0 ledger records the needed original allocation/handler-table behavior but has not closed a no-SIM initialization object. | Stop gate |

## Interpretation

MSVC/x86 is the correct Bochs toolchain island, but the imported VS2008
solution is only provenance evidence for a historical **full product**. It is
not a minimal-fixture recipe and must not be converted mechanically into one.
Using its `config.h`, its project library lists, or its `bochs.exe` target
would silently admit devices and product behavior that no reached first-profile
caller has requested.

The route is therefore **available as a toolchain prerequisite but blocked as
a build recipe**. The blocker is source-to-object/configuration closure, not
an MSVC installation failure. No build was run, so there is no compile/link
or runtime claim.

## Required, Default, And Rejected Boundary

| Category | Current result |
| --- | --- |
| Required candidates | Original generic CPU decoder component, CPU reset, checked memory/reset-window mechanics, ICache state, and null PC-system state only where the existing M0 ledger proves an incoming edge. |
| Default but not required | Historical product startup, parameter registration, GUI/text configuration, plugin loading, VS resource/product linker setup, and device-library aggregation. |
| Explicitly rejected | `bochs.exe`, `all`, VS2008 solution invocation, the imported broad `config.h`, GUI, debugger, plugins, BIOS product loading, storage, networking, USB, sound, PCI, APIC/SMP, and OpenNT/DOS/BOP/DEM semantics. |

## Follow-up

1. Close the existing M0 SIM/CMOS triple-fault decision and the no-SIM memory
   and unmapped-port initialization symbol ledgers.
2. Derive, from those ledgers, a purpose-specific MSVC/x86 response or NMake
   recipe that names only proven source objects and an independently recorded
   minimal configuration.
3. Before a fresh-root invocation, record the exact command, CRT model,
   artifact path/hash, C ABI direction, and feature-negative checks in the
   active packet.
4. Run one declared fixture invocation only after steps 1--3. It must neither
   alter Bochs source/configuration nor promote S5 adapter tests to native
   runtime proof.

## Confidence

High for the negative conclusion: the observations are direct imported project
and configuration metadata. Medium for the future recipe shape: it is bounded
by the existing M0 ledger but cannot be authored until its unresolved source
edges are closed.

## Coordinated-Session Acceptance

The coordinator independently re-read the executor's cited source on
2026-08-10.  The result is accepted without amendment:

- `cpu/exception.cc:1192-1201` queries
  `SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)`, observes CMOS shutdown
  status, performs `bx_pc_system.Reset(BX_RESET_HARDWARE)` on the reset path,
  and otherwise takes the original panic/shutdown path before the CPU-loop
  `longjmp`;
- `memory/misc_mem.cc:111-152` creates the required vector, ROM backing,
  block and handler tables, then queries the I440FX parameter through `SIM`
  and registers product state;
- `iodev/devices.cc:103-134` installs/allocates the default port-handler
  arrays, while the same original initializer continues into product support;
  its source-defined default handlers at lines 379-390 return `0xffffffff`
  for reads and ignore writes; and
- the imported VS configuration selects CPU level 6/x86-64 and broad
  BIOS/CMOS/DMA/VGA/PCI/USB/network/sound/configuration features, while
  `vs2008/bochs.vcproj` targets `bochs.exe` with product dependencies.

No build, solution invocation, source/configuration patch, or device admission
occurred during this review.  S6 is therefore accepted as **externally/governance
blocked**, not as a toolchain failure.  The only next action is an owner choice
for the triple-fault semantic branch; authoring an MSVC recipe before that
choice would pre-decide a prohibited Bochs boundary change.
