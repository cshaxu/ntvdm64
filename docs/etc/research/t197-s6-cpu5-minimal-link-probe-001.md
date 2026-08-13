# T197 S6 CPU5 Minimal Native Link Probe 001

## Question

Can the first explicit CPU5/Pentium-MMX, non-x86-64 `bx-core`/`bx-mantle`
candidate link a real minimal-machine lifecycle call without importing the
historical Bochs product shell?

## Inputs

- Pinned adopted Bochs source in `src/bx-core`, `src/bx-mantle`, and the
  retained headers under `src/bochs`.
- `tools/t197-s6-cpu5-mantle-config-projection.json`, whose only source-config
  differences select CPU level 5 and disable x86-64.
- `tools/Invoke-T197S6MinimalMachineLinkProbe.ps1`.
- Fresh output root
  `artifacts/build/t197-s6-minimal-machine-link-probe-r1`.

## Procedure

1. Project the pinned Bochs configuration into the fresh output root.
2. Compile exactly nine named sources using MSVC/x86 with `/Gy`: finite SIM,
   minimal machine, empty port-space, parameter tree, logging, PC-time,
   memory, memory helpers, and CPU initialization.
3. Compile one generated link-only probe which calls
   `bx_ntvdm_minimal_machine_c::initialize(0x100000, 0x100000)`.
4. Link only those ten objects with `/OPT:REF`; retain `link.log`, `link.map`
   and a JSON manifest regardless of result.

The manifest rejects `main.cc`, `config.cc`, `gui/siminterface.cc`,
`bochs.exe`, device archives, adapter inputs and OpenNT inputs.

## Observations

- All nine declared native objects compiled under the projected CPU5/x86
  configuration.  This includes the first actual compilation of original
  `paramtree.cc`, `logio.cc`, `pc_system.cc`, `memory.cc`, `misc_mem.cc`, and
  `cpu/init.cc` from their new ownership paths.
- The link intentionally did **not** close: MSVC reported 49 unresolved
  external symbols in the retained `link.log`.
- The unresolved set is not an adapter or OpenNT edge.  It separates into:
  - native CPU5 processor dependencies: CPUID models, local APIC, TLB/flags/
    segment/reset/decode helpers and instruction-cache state;
  - the `bx_devices_c` constructor/destructor, required solely because the
    mantle owns the global empty port-space object;
  - product-shell ownership: text-config virtual parameter methods,
    `bx_atexit`, and `bx_pc_system_c::exit` references to GUI/full-device
    teardown; and
  - original memory instruction-cache support.

## Interpretation And Confidence

High confidence: this is the first reproducible source-built MSVC/x86 link
frontier for the CPU5 minimal lifecycle.  It disproves neither the target
architecture nor native lifecycle ownership.  It proves that a single
`pc_system.cc`/`paramtree.cc` object carries methods from the legacy product
shell even though the minimal initializer does not ask for them; C++ vtables,
global destruction and object-level linkage retain those methods.

The next S6 action is a source-by-source ownership decision.  Native CPU and
memory dependencies remain candidates for the adopted core.  Product-shell
methods must be isolated, disabled by a finite mantle contract, or recorded as
an explicit minimal-mantle exception.  GUI, full-device initialization,
configuration parsing, adapter code and OpenNT code are not admitted to make
this link pass.

## Follow-Up

Audit the two `bx_devices_c` lifetime methods and the finite parameter/PC-time
vtable/destructor reachability before changing a source.  Any source-object
separation or finite no-product replacement must be registered in the
external-intrusion exception register first and rerun through this exact
probe.
