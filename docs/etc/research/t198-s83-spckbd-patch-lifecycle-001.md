# M0 T198 S83: SpcKbd `host_int10` Patch Lifecycle

## Question

Does the resident SpcKbd `host_int10` far-pointer change before the bound
adapter can receive a canonical BOP, or does the observed `C000:014A` jump
occur later in guest execution?

## Inputs And Procedure

- The S74 source-built CPU5/MSVC x64 closure and its exact link response.
- The unchanged `t198_s23_native_ntio_boundary` fixture, with a test-only
  observer rebuilt from source together with its C bridge.
- The existing mantle checked ordinary-RAM READ action, applied only to
  physical `0x7ceb..0x7cee`: the four-byte operand after SpcKbd's `EA` far-jump
  opcode. `0x7ce9` is the preceding `POPF` instruction start, not the operand.
- The fixture binding order: composition binds before `ntio_preentry_prepare`
  publishes the exact guest image and before the CPU run begins.
- `tools/Invoke-T198S83SpcKbdLifecycleProbe.ps1`, r6:
  it rebuilds `fixture.obj` and `bridge.obj`, links them against the recorded
  S74 source-built closure, and records the bounded run.

The observer runs at the beginning of the test bridge, before its composition
delegate can consume a canonical event. It copies no guest state other than
the declared four bytes and never changes an event or outcome.

## Observations

The r6 run reaches canonical `C4 C4 50 11`. At that first ingress the checked
read succeeds and reports:

```text
first-canonical observed=1 selector=50 service=11 host-int10-read=1 bytes=00000000
```

The same run preserves the established later sequence:

```text
predecessor=073b:0939
cs-provenance ... code=ea4a0100c0565755ba4600520ebb55
fault=c000:014a
spckbd observed=0
```

The process exit is the retained fixture's expected `2`: its final legacy
assertion still compares the terminal generic event with the older synthetic
`0000:0047` observation. The S83 script records that child status and treats
it as a successful bounded observation, not as a new runtime acceptance claim.

`spckbd.asm` defines the relevant layout and writer: `jmp_native` emits
`EA` followed by `host_int10 dd ?`; the `isk_int9` continuation reads IVT
10h at `0000:0040` and stores both words into `host_int10`. The fixture's
pre-entry IVT input deliberately supplies `4a 01 00 c0` at that vector.

## Interpretation

The far pointer is still its uninitialized `0000:0000` form at the first
observable BOP (`50:11`) after composition binding. It is therefore not a
pre-bind write and not a first-ingress race. By the later `073B:0939` far jump
it is `C000:014A`, exactly matching the supplied IVT-10 value. The write lies
strictly after first canonical ingress and before the C000 transfer.

This strongly reinforces the original `isk_int9` source path as the writer,
but does **not** directly prove that path executed: the existing passive
post-composition observer still reports BOP `5F` absent. That discrepancy is
now a routing/observer-order question, not a ROM, VGA, Bochs-core, or adapter
provider implementation request.

## Result And Follow-up

S83 closes its lifecycle question without a production semantic change.
The next slice may audit why `5F` is not visible to the current passive
observer despite the source-owned `isk_int9` write path. It must first inspect
common ingress/composition disposition and test bridge ordering; it must not
add a BOP provider, write guest memory, select a display profile, import ROM,
or change `bx-core`/`bx-mantle`.

## Confidence

High for the observed before/after bracket and address correction. Medium for
the exact historical writer until the BOP-`5F` visibility discrepancy is
resolved directly.
