# T225 S42 Exception-Loop Watchdog Delivery Map 001

S41 proves that S40's `BX_ASYNC_EVENT_STOP_TRACE` marker is sufficient for an
ordinary instruction loop but insufficient for the repeated exception/longjmp
path. Native CPU loop code clears that trace-control bit when it exits an
inner trace; `handleAsyncEvent` only observes the existing kill latch when a
nonzero async event survives to its outer loop.

The selected minimal composition is a mantle callback setting the existing
ordinary async-event notification (`bx_cpu.async_event = 1`) after setting the
existing kill latch. This is identical in ownership to the S40 watchdog and
cancellation callbacks. It changes no bx-core source, public ABI, BOP route,
provider, guest state, host policy, logging policy or OpenNT source.

Original upstream Bochs has no project finite-run callback to reuse. The
project mantle is the smallest seam; external-code intrusion and new machine
behavior are rejected. Verification must use a focused repeated-exception
fixture and one resumed paired native observation only after that fixture
passes.