# System Architecture

## Architectural Goal

NTDOS64 is a layered composition of an adopted guest machine, an OpenNT guest
environment, and a modern contained host. Its design objective is to preserve
each layer's native responsibility rather than recreate one layer inside
another.

## Components And Ownership

| Component | Owns | Does not own |
| --- | --- | --- |
| CLI (`src/cli`) | Invocation contract, immutable profile admission, contained host policy, diagnostics, and engine launch | Guest execution, guest-memory access, guest-service semantics, or machine mechanics |
| Host capability seam | Modern contained host facilities required by the composition | Guest protocol meaning, CPU/device behavior, or Bochs internals |
| Adapter composition (`src/bx-ntvdm-adapter`) | Typed boundary contracts; guest-service routing and provider selection; compatibility adaptation; machine-composition selection | CPU execution, firmware, PIC, memory model, device model, or DOS kernel algorithms |
| Bochs 2.6 backend (`src/bochs`) | x86 execution, memory, firmware, interrupts, PC-device mechanics, and native machine lifecycle | Guest-service interpretation, OpenNT/DOS/WOW semantics, host policy, or compatibility-provider selection |
| OpenNT guest layer | DOS, NTDOS, COMMAND, WOW16 payloads, and guest-visible behavior | CPU interpretation, firmware, PC devices, or modern-host composition |
| Historical machine-handler islands | Individually admitted original machine-facing behavior under adapter selection | A replacement SoftPC/CCPU backend or general host-service plane |
| Research fixtures | Reproducible evidence for a bounded question | Product behavior or implicit runtime dependencies |

## Composition

```text
CLI and contained host policy
  -> adapter composition
      -> adopted Bochs machine
          -> OpenNT guest environment
      -> selected original or source-derived host capability
```

The adapter is the composition boundary between machine events and host-side
meaning. It receives and returns versioned, fixed-width values and checked
guest-memory ranges. It does not pass C++ objects, host pointers, CRT-owned
memory, implicit handle ownership, or cross-architecture callbacks across that
boundary.

## Boundary Invariants

- Machine mechanics stay in Bochs. The adapter may request bounded mechanical
  operations through typed contracts but does not reproduce CPU, memory,
  firmware, interrupt, or device algorithms.
- Guest and host-service meaning stays outside Bochs. The adapter owns the
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
CLI -> adapter -> Bochs mechanics
CLI -> adapter -> host capability seam
adapter -> OpenNT-derived provider or historical machine-handler island
OpenNT guest -> adapter contract -> selected provider
```

No component may reverse these directions by importing another component's
private execution state. In particular, Bochs remains reusable as a guest
machine, and the adapter remains reusable as the explicit NTVDM composition
boundary for future hosts.
