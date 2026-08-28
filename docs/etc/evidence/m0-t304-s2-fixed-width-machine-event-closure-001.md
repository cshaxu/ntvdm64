# M0 T304 S2 — fixed-width machine-event ingress closure

## Result

The CPU-to-MVDM ingress now has one public, fixed-width, selector-blind
machine-event contract. It is a mechanical boundary only; it does not form a
BOP event or interpret any selector, service, guest object, or OpenNT provider
state.

## Public contract and private mirror boundary

- `src/adapter-mvdm-host-in/generic_ud_bridge.h` is the sole public definition
  of `runtime_generic_ud_event`, `runtime_generic_ud_outcome`, and
  `runtime_generic_ud_context_outcome`. It asserts event size `112`, outcome
  size `64`, context-outcome size `88`, and the critical event/result offsets
  on both C and C++ builds.
- The Bochs overlay retains its own private record declarations. That is
  deliberate: `bochs-core-overlay` cannot include an upper MVDM adapter header
  without reversing the component dependency direction. They are no longer a
  second *public* ABI.
- `src/adapter-bochs/machine_facade.cc` keeps source-compatible opaque callback
  registration, but interposes a selector-blind wrapper. It copies event bytes
  into a bounded local record and, for an accepted nonempty outcome, copies the
  bounded result back. The consumer never receives the Bochs CPU-stack record;
  the temporary copied records have callback-only lifetime.
  Capacity values are mechanical (`128` bytes each) and carry no record
  interpretation.
- Unbound, null, zero-length, or oversized records decline. Existing
  observation callbacks that use an empty outcome remain valid.

## Focused formal evidence

`tools/build/New-T304MachineEventNinja.ps1` generates a six-edge Ninja graph:
one compile-only `adapter-bochs` facade object, the two ingress transport
objects, and a small executable fixture. The source manifest forbids
`src.old`, a SoftPC/CCPU executor, a BOP provider, and a prebuilt Bochs archive.
It does not configure or rebuild the full Bochs mirror.

The generated fixture verifies malformed record rejection, unbound decline,
and accepted `RESUME`, `PENDING`, and `STOP` outcomes. The facade object and
the executable fixture were built for both supported targets; each fixture
returned zero:

```text
x86-exit=0
x64-exit=0
```

## Deliberate limits and next owner

This does not prove a native guest BOP, `VDMEVENTINFO`, EIP policy, or guest
execution continuity. The next DEM/COMMAND vertical package may consume this
mechanical contract to form a source-shaped event, without reopening the
Bochs/CPU boundary.
