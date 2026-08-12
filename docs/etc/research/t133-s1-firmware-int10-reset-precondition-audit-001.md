# T133 S1 firmware/INT10 reset-precondition audit 001

## Question

Does T130's later `EIP=10000 > CS.limit=FFFF` / INT 0Dh route prove a missing
Bochs reset, POST, IVT, or INT10 lifecycle edge?

## Evidence

- The T130 corrected deferred trace records the option ROM at `CA800` and,
  before the deferred handoff, many original real-mode INT10 transfers to
  `C000:014A`, including callers in the VGA ROM (`C000`) and system BIOS
  (`F000`).
- OpenNT `InstSpcKbd` saves IVT INT10 (`0000:0040`) into `host_int10` before
  it overwrites its pseudo-ROM vectors. Its eventual `jmp_native` is a far
  jump through that saved value.
- T101--T103's zero-vector diagnosis applied to the former direct-entry
  composition, which intentionally transferred to NTIO before POST. T104
  closes that mode gap through the native POST plus deferred `CA80:0003` UD2
  handoff; T130 uses the same deferred consumer shape.
- T130 reaches the later `0BC1:FFFF` prefetch/INT0Dh path only after native
  POST, the handoff, four existing transactions, and normal guest BOP flow.

## Disposition

The current composition has a **satisfied native firmware/reset
precondition**. Its INT10 vector was live during POST, so the late prefetch
failure is not evidence to synthesize IVT state, re-enable a device, alter
Bochs reset, add a BOP handler, or expand the adapter.

The remaining issue is a separate, post-handoff guest control-transfer/fault
state question. Its next evidence task must begin at the exact `0BC1:FFFF`
predecessor and use original caller/frame state; it must not reopen the
already-closed direct-entry/POST decision.
