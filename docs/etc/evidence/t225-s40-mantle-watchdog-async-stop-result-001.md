# T225 S40 Mantle Watchdog Async-Stop Result 001

## Question

Does the mantle one-shot watchdog make a normal non-HLT CPU loop return its
existing budget terminal, without a BOP/provider or Bochs-core semantic change?

## Inputs and procedure

The formal MSVC x64 `/MT`, CPU5/P-MMX Ninja graph was generated in
`build/M0-T225-S40/watchdog-r1/`. The focused fixture executes a real-mode
`EB FE` loop, uses a 16-tick request, provides only a decline stub for the
unreached generic-UD bridge, and asserts `EXECUTION_BUDGET`. The formal CLI
was then run against the immutable S10 inputs with budget 1 in Direct and
Readonly modes.

## Observations

- The focused fixture built and returned zero.
- Direct and Readonly both exited 4, the existing CLI execution-budget result.
- Each output was 420 bytes and SHA-256
  `98487FF81F86ABDCD106F4E49A330114016B8983BE6C3494EC7DE72E8C0D458A`.
- Both concluded `terminal=4 detail=0 lifecycle=5 presentation=5
  cancellation=0 budget=1`.
- The changed code is only two identical mantle callback additions:
  `bx_cpu.async_event |= BX_ASYNC_EVENT_STOP_TRACE;` immediately after the
  existing kill latch. No bx-core, bx-vdm, OpenNT, profile or CLI source changed.

## Interpretation

The existing machine-stage watchdog now wakes the existing CPU asynchronous
path, whose first action observes the existing kill latch. This restores the
already-declared typed budget terminal; it adds no BOP, guest, provider,
profile, device or CPU behavior. Direct/Readonly parity holds for this
lifecycle check. The S39 observer observation can now be retried exactly once.