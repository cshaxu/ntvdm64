# T225 S40 Mantle Watchdog Asynchronous Stop Source/ABI/Failure Map 001

## Question

What is the smallest owner-correct change that makes the already-declared
machine-stage instruction tick budget terminate normal guest execution, so the
S39 selector-blind observer can be validated without an unbounded native run?

## Current route

`ntdos64-native --instruction-tick-budget N` copies `N` through
`src/cli/ntdos64_native_cli.c`, the engine request, and
`src/bx-mantle/bx_ntvdm_engine_run_v1.c` into
`bx_ntvdm_machine_stage_v1_execute`.  The machine stage registers a one-shot
native Bochs tick timer in `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc`.
Its callback sets `bx_pc_system.kill_bochs_request`.

Bochs' normal CPU loop checks that latch through `handleAsyncEvent`, but only
enters that method when the pre-existing `async_event` flag is non-zero.
The timer callback does not set that flag.  A normal repeated exception loop
therefore continues after the timer fires.  The S39 native attempt is the
reproducible witness: both modes repeatedly logged a segment-limit failure
until externally stopped; it did not reach its normal result presentation.

## Source-recovery and ownership disposition

| Rung | Disposition | Evidence and retained contract |
| --- | --- | --- |
| Original source reuse | Not directly applicable | Upstream Bochs owns `kill_bochs_request` and the CPU-loop async-event gate, but has no project finite-run watchdog callback or CLI run contract to reuse. |
| Smallest compositional seam | Selected | The project-owned mantle callback already owns the finite-run lifecycle. It will combine the existing kill latch with the existing native `BX_ASYNC_EVENT_STOP_TRACE` notification, matching the established selector-blind first-fault and generic-stop transfer shape in `src/bx-core/cpu/exception.cc`. |
| External-code intrusion | Rejected | `src/bx-core` remains unchanged; no Bochs exception/decode/device behavior is modified. No registry exception is required. |
| New behavior | Rejected | The correction does not create a CPU, BOP, DOS, device or host-service behavior. It makes the existing mantle one-shot stop request observable by the existing CPU loop. |

## ABI and failure contract

No public ABI changes. `bx_ntvdm_machine_stage_v1_execute` keeps its existing
`EXECUTION_BUDGET` terminal when the one-shot watchdog fires. The cancellation
callback takes the same mechanical stop path and therefore must retain its
existing `HOST_CANCELLATION` terminal. A completed BOP/first-fault controlled
stop retains precedence because those observer outcomes remain tested after
CPU-loop return.

## Implementation and verification boundary

Admitted files are `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc`, one
mantle-focused normal-loop budget fixture, its formal Ninja-manifest entry,
and current Status/evidence/history records. The fixture must execute an
ordinary real-mode non-HLT loop, prove exact budget terminal behavior and
prove no BOP/provider event is involved. A formal native Direct/Readonly run
then proves the original CLI budget returns normally before S39 resumes its
paired observer validation.

No selector-specific route, guest-memory change, provider/profile change,
OpenNT source change, bx-core patch, logging policy alteration or BOP
implementation is admitted.