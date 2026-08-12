# T95 S6 native-container reconciliation

## Question

What should the project do after the MSVC/x86 object audit proves that a
CPU-only Bochs selection requires four inseparable product-lifecycle roots?

## Evidence reconciled

Two established lines of evidence now agree:

1. `bochs-26-native-bare-machine-container.md` records the architectural
   decision to retain the original Bochs initialization chain
   `bx_init_siminterface -> bx_init_main -> bx_begin_simulation`; and
2. `t95-s6-minimal-mechanics-provider-object-observation-001.md` proves that
   the apparent smaller 103-object MSVC set reaches SIM configuration,
   device/plugin/timer, PC GUI shutdown and log-exit lifecycle roots.

The second result does not invalidate the first. It proves why the first is
the only non-reimplementation route: extracting CPU/RAM/empty-I/O source
objects while retaining their semantics would require a new simulator shell.

## Decision

T95 treats **the native Bochs 2.6 `nogui` lifecycle as the minimal machine
container boundary**. “Minimal” has the following precise meaning:

- minimum *enabled machine/profile surface*, selected through original Bochs
  configuration and existing lifecycle gates;
- not minimum linked source-file count or a project-owned replacement for
  `SIM`, device manager, PC system or logging;
- no adapter construction or ownership of Bochs C++ objects, devices, timers,
  CPU, memory, exceptions, firmware or shutdown; and
- no claim that an intact native container proves an NTDOS boot.

The current default profile retains original `nogui` lifecycle ownership and
uses declarative configuration to keep optional plugins disabled. It does not
describe the remaining stock hard-wired core as a new adapter-owned machine.
Any future profile reduction must use one original lifecycle-level declaration
and a complete initialization/state-registration/reset trace.

## Consequence for S6 micro-closure evidence

The CPU3/MSVC object inventory remains valuable evidence:

- the original 93-unit CPU island is compiler-available;
- the five-member empty-port object is independently compilable; and
- the exact sources that make a CPU-only product impossible are now named.

They do **not** authorize more extraction patches, a hand-written minimal
main, a SIM substitute, a device-manager substitute, a logging substitute, or
a new link attempt. Existing registered source separations remain preserved
as evidence and must not become a second runtime architecture absent a new
owner decision.

## Restored implementation order

1. Derive one MSVC/x86 native-container recipe from the original Bochs build
   metadata, retaining its own lifecycle rather than recreating it.
2. Materialize one declarative `nogui` first-profile input with optional
   plugins denied through original configuration gates; record all retained
   stock-core effects rather than calling them minimal by assumption.
3. Prove only native reset and controlled termination with that same toolchain
   island.
4. Reconnect the already versioned, default-off generic adapter event seam;
   its Bochs half knows no BOP/DOS/OpenNT semantics.
5. Only after those mechanics are live, admit a contained first guest/host
   capability transaction under the existing CLI -> adapter -> guest boundary.

No full-build loop, feature enablement, device-by-device patching or OpenNT
semantic migration is admitted by this reconciliation. The next task is a
recipe design/evidence task, not an immediate `bochs.exe` build.
