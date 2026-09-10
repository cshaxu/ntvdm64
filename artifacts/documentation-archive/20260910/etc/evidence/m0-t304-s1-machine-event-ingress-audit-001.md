# M0 T304 S1 — machine-event ingress audit

## Original event boundary

The source-shaped monitor event record is `VDMEVENTINFO` in
`opennt-abi/source/public/internal/base/inc/vdm.h`.  Its fixed-width fields
are `Size`, `VDMEVENTCLASS Event`, `InstructionSize`, and one union member:
`VDMIOINFO`, `VDMSTRINGIOINFO`, `VDMBOPINFO`, `VDMFAULTINFO`,
`VDMERRORINFO`, or `VDMINTACKINFO`.  The source monitor receives it through
the kernel VDM product path in `v86/monitor/i386/monitor.c`; after an
`NtVdmControl(VdmStartExecution, ...)` return it advances EIP by
`InstructionSize` and dispatches one of seven original event owners:
IO, string IO, memory access, interrupt acknowledge, BOP, error, or IRQ13.

`VDMEVENTINFO` is therefore a **kernel-VDM-to-monitor contract**, not a
generic CPU exception record.  Modern user-mode NTVDM64 must not claim to
recreate the kernel producer or place BOP/provider semantics in Bochs.

## Existing replacement seam

`bochs-core-overlay/cpu/exception_hook.cc` emits a copied, fixed-field #UD
machine fact only: vector/error, execution mode, fault RIP, copied GPR/segment
state and at most 15 fetched instruction bytes.  The core retains only a
selector-blind callback forwarding point.  `adapter-bochs/machine_facade.*`
is its only public mechanical owner.

`adapter-mvdm-host-in/generic_ud_bridge.h` mirrors that byte layout in
`runtime_generic_ud_event` and defines copied `RESUME`, `STOP` and `PENDING`
outcomes. `bop_ingress.c` currently validates the record, invokes a
same-process route only when bound, and otherwise declines. It has no guest
pointer, Bochs object or direct provider implementation.

## Required separation

- The machine event record stays selector- and service-blind. It must carry no
  BOP number, DOS service, host handle or raw guest pointer.
- A later MVDM monitor binding may decode the copied instruction window and
  form a `VDMEVENTINFO`-shaped **BOP** event only after it validates the BOP
  opcode and instruction length. That binding owns the EIP advance/result
  contract, not `adapter-bochs`.
- IO, string-IO, int-ack, error, IRQ13 and memory-access source paths are
  distinct original event owners. They are explicitly not admitted by this
  packet.
- `STOP`/`PENDING` are machine lifecycle outcomes. They do not mean a BaseSrv,
  COMMAND, DEM, XMS or DPMI result.

## S2 implementation decision

S2 shall consolidate the duplicated private/public #UD layout into one
versioned fixed-width `adapter-mvdm-host-in` machine-event ABI with compile-time
size/offset assertions on x86 and x64.  The Bochs overlay copies into it only
through the existing opaque callback. The initial implementation provides
validate/copy/decline behavior and no BOP decoder or route. A focused x86/x64
fixture must prove malformed-record decline, exact copied bytes, unbound
decline, and that all outcome fields remain mechanical.
