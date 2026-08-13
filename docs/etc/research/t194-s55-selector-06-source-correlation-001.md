# M0 T194 S55: Selector-06 and Late-Fault Source Correlation

## Question

Does the S54 selector-06 re-entry identify a host-service BOP, a Bochs defect,
or an omitted original machine-composition component; and what can it say
about the later EIP/stack fault?

## Source and Trace Anchors

- S54 records 6509 selector-06 observations. The stack pointer falls by six
  bytes through the final repeated entries, consistent with repeated real-mode
  exception-frame delivery.
- OpenNT `softpc.new/base/bios/bios.c` maps BOP 06 to
  `illegal_op_int`, while `softpc.new/base/system/illegalp.c` reads the saved
  fault frame, advances its saved CS:IP by one instruction byte and invokes
  `unexpected_int`.
- OpenNT `dos/v86/doskrnl/bios/spckbd.asm` deliberately installs the INT 06
  vector at an `int06h_vector` pseudo-ROM route. T179 already records the
  intended `BOP 06; iret` lifecycle and a successful separately linked
  original `illegal_op_int` machine island.
- The S49 generated makefile has no `BX_NTVDM_ENABLE_MACHINE_COMPOSITION=1`
  define and no machine object triple. Its final map has no `illegal_op_int`,
  `unexpected_int`, or `bx_ntvdm_machine_bop` symbol. Therefore S51 retained
  deferred POST but did not retain the existing original machine-composition
  island.
- T134 and T150 separately establish that a real-mode prefetch-limit fault
  routes through INT 0Dh/BOP 02/IRET and replays an already-created fault
  frame. They do not identify the earlier invalid control transfer.

## Disposition

Selector 06 is neither a DEM/COMMAND adapter service nor a request to add a
Bochs device/exception workaround. In this composition it is a proven omitted
original SoftPC `illegal_op_int` machine island. The appropriate next action
is a no-runtime source closure that combines the already existing deferred
POST and machine-composition options, preserving the original handler and
three narrowly declared machine mechanics.

The late `EIP=10000h`/stack fault remains distinct. The current S54 listener
does not provide branch history, so it cannot attribute that invalid transfer
to the observed metadata BOPs or to the missing selector-06 island. No
provider, adapter, Bochs or guest semantic change is supported by this audit.
