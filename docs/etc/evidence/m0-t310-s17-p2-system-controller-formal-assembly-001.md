# M0 T310 S17 P2 — system-controller contract and formal assembly

## Source-contract result

The companion ledger reads every selected S17 controller contract together
with its table/initializer, selected callers and identity class.  The result
is intentionally source-first:

- the PIT, PIC, CMOS/RTC, DMA and quick-event algorithms remain original
  source bodies; no controller implementation is rewritten in an adapter;
- `nt_timer.c` retains its original public Win32 heartbeat/event flow and the
  already-selected same-shaped `NtAlertThread` compatibility binding;
- `nt_eoi.c` retains its original ICA lock, EOI hook table and non-MONITOR
  delayed IRQ path.  Its existing `SoftPcEoi(int, int *)` to typed PIC bridge
  is the only registered call-shape binding and does not reinterpret
  interrupt semantics;
- private SoftPC controller state and fixed-width device scalars are not host
  object identities.  They do not require mapping-manager tokens.  A future
  guest-visible external endpoint must use the mapping manager at that actual
  boundary, under its own device owner.

## Assembly evidence

The current selected graph at
`build/M0-T310/S16/original-softpc-ccpu40-s16p5` compiled the complete
original system manifest and the admitted host controller bindings as part of
the formal candidate:

```text
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 original-softpc-candidate
# 369/369 successful
```

The subsequent `/force:unresolved` audit found no new S17-owned missing
controller implementation.  Its unresolved forms are explicitly later
owner edges: device endpoint producers, platform/UI, BaseVDM/BOP, VDD,
Redirector, debugger and optional media paths.  The forced image is only an
ownership audit and is not executable evidence.

## Deferred behavior

Original runtime behavior requires the selected machine lifecycle, physical
endpoint providers and the original reset order to become reachable.  S49
must validate heartbeat → strobe → PIT/RTC/PIC delivery as part of the
integrated SoftPC lifecycle.  This P2 record neither invokes controller code
through a Bochs fixture nor fabricates an interrupt result.
