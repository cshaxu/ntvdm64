# System Architecture

## Architectural Goal

NTDOS64 is a layered composition of an adopted guest machine, an OpenNT guest
environment, and a modern contained host. Its design objective is to preserve
each layer's native responsibility rather than recreate one layer inside
another.

## Components And Ownership

| Component | Owns | Does not own |
| --- | --- | --- |
| OpenNT layer (`src/opennt`) | Original NTVDM host-service source, DOS/WOW guest source, and their source-derived contracts | CPU execution, firmware, PC-device emulation, or Bochs lifecycle |
| VDM adapter (`src/bx-vdm`) | Typed bx↔VDM contracts, BOP ingress/egress, compatibility adaptation, and explicit composition of OpenNT host/guest contracts | CPU execution, firmware, PIC, memory model, device model, DOS/WOW kernel algorithms, or ambient-host policy |
| Bochs mantle (`src/bx-mantle`) | Minimal native Bochs lifecycle composition: SIM/logging/no-device time state and assembly of admitted core mechanics | VDM, BOP, OpenNT, DOS, host policy, GUI, plugins, or unadmitted PC devices |
| Bochs core (`src/bx-core`) | Adopted Bochs CPU/decode, memory, exceptions, and admitted no-device mechanics | VDM/guest-service interpretation, OpenNT/DOS/WOW semantics, host policy, or compatibility-provider selection |
| Contained host-capability seam | Modern host facilities selected by the VDM adapter | Guest protocol meaning, CPU/device behavior, or Bochs internals |
| OpenNT source layer (`src/opennt`) | Normative historical guest payloads and host-provider source/contract evidence | CPU interpretation, firmware, PC devices, or modern-host composition |
| Historical machine-handler islands | Individually admitted original machine-facing behavior under adapter selection | A replacement SoftPC/CCPU backend or general host-service plane |
| Research fixtures | Reproducible evidence for a bounded question | Product behavior or implicit runtime dependencies |

## Composition

```text
OpenNT host and guest layer
  -> bx-vdm adapter
      -> bx-mantle -> bx-core
      -> selected contained host capability
```

The VDM adapter is the composition boundary between machine events and host-side
meaning. It receives and returns versioned, fixed-width values and checked
guest-memory ranges. It does not pass C++ objects, host pointers, CRT-owned
memory, implicit handle ownership, or cross-architecture callbacks across that
boundary.

The first runtime process is MSVC x64 throughout: CLI, VDM adapter, mantle and
adopted Bochs core share one static CRT. The guest CPU architecture is an
emulation property, not a host-process property; CPU5/Pentium-MMX guest code
therefore remains valid inside the x64 process. No MinGW object or a separate
CRT may enter this in-process composition.

## Boundary Invariants

- Machine mechanics stay in the Bochs core. The VDM adapter may request bounded mechanical
  operations through typed contracts but does not reproduce CPU, memory,
  firmware, interrupt, or device algorithms.
- The mantle is Bochs-internal assembly only. It reuses native Bochs code and
  data structures, extracting only product-shell paths that prevent independent
  minimal operation. It has no VDM or guest meaning.
- Guest and host-service meaning stays outside Bochs. The VDM adapter owns the
  interpretation and routing needed to compose OpenNT-derived host behavior.
- The guest owns DOS and WOW behavior. Neither the CLI nor the adapter becomes
  a replacement DOS kernel or filesystem implementation.
- Modern host capability is explicit and contained. It is introduced through
  the host capability seam, never by exposing ambient host state to the guest.
- Historical code is normative source and ownership evidence. It is reused
  only through a bounded, independently auditable composition boundary; it is
  not a dependency on the unavailable historical product shell.

## Dependency Direction

Dependencies point inward through declared contracts:

```text
src/opennt -> bx-vdm -> bx-mantle -> bx-core
bx-vdm -> contained host-capability seam
bx-vdm -> src/opennt host/guest contract or historical machine-handler island
```

No component may reverse these directions by importing another component's
private execution state. In particular, the Bochs core and mantle remain
reusable as a guest machine, and the VDM adapter remains reusable as the
explicit NTVDM composition boundary for future hosts.
