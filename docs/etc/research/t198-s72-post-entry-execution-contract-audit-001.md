# M0 T198 S72: Post-App-Entry Finite Execution Contract Audit

## Current terminal meaning

The finite runner defines status zero as `COMPLETED_BUDGET`; its timer ends
the native CPU loop when the fixture's copied instruction-tick budget expires.
The current NTIO fixture supplies `8192` ticks. Thus its post-`50:36`
terminal at `0032:5A82` is a bounded-observation limit, not a guest exit, HLT,
or CLI-success result.

## Source-owned terminal forms

OpenNT names `C4 C4 FE` `BOP_UNSIMULATE`, the source-defined end of VDM code.
The existing controlled-stop provider maps only that exact copied real-mode
event to generic typed STOP. Retained T161 evidence reaches such a terminal
after continued COMMAND work; it does not authorize changing the stop
contract.

The repository also retains an explicit one-target COMMAND normal-return
contract: after a target returns, `54:11` records the guest's low DX status
and resumes with no next command. That is distinct from `FE` and is not a
CLI exit-result transport.

## Minimal next observation

The present fixture intentionally uses its one-byte HLT `TARGET.COM` and does
not claim the historical `QUIT.COM` terminal sequence. Replacing guest media
or profile mode would change the question. The narrow next step is therefore
to keep inputs, composition and mechanics identical while raising only the
fixture's finite tick budget from `8192` to `1000000`, a bound already used as
the IPS value. It will record the first post-entry BOP or the bounded
terminal. No provider, device, or Bochs change is justified.
