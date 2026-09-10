# M0 T324 S2 — Original named-pipe completion contract

## Question

Can the original `vrnmpipe.c` asynchronous worker be enabled merely by changing
its cdecl `CreateThread` callback to `WINAPI`?

## Source observations

- `VrReadWriteAsyncNmPipe` at `vrnmpipe.c:1331..1615` reads a fixed DOS
  request, queues a `DOS_ASYNC_NAMED_PIPE_INFO`, and issues overlapped
  `ReadFile`/`WriteFile` only after a persistent worker exists.
- The persistent record keeps guest numeric locations: `Buffer`,
  `pBytesTransferred`, `pErrorCode`, `ANR` and (for type-2 calls) `Semaphore`.
  The original uses those locations after the BOP returns.
- `VrpCompleteAsyncRequest` at `2041..2146` calls `GetOverlappedResult`, writes
  the result to the retained guest locations, and for ANR requests queues
  `VrNmPipeInterrupt` then calls `VrRaiseInterrupt`.
- `VrNmPipeInterrupt` at `1626..1725` runs at the original hardware-completion
  point, translates the retained ANR/semaphore forms into registers and selects
  the named-pipe callback class.
- `VrRaiseInterrupt` is not a generic Win32 notification: `vrinit.c` routes it
  through `call_ica_hw_interrupt(NETWORK_ICA, NETWORK_LINE, 1)`.

## Existing binding and its meaning

`ADAPTER-WIN32-REDIR-001` preserves the original cdecl `CreateThread` call
shape but returns `ERROR_CALL_NOT_IMPLEMENTED`. This is an intentional
source-shaped decline, not a missing calling-convention patch: it prevents the
original code from queuing an I/O that would retain a raw guest-memory pointer
in a worker. The independently existing general `thread_start_compat` bridge
does bind a session correctly, but using it alone would not satisfy the
retained-buffer and SoftPC/ICA completion contract.

## Required same-shaped recovery design

If a later S enables asynchronous named-pipe I/O, it must retain the original
queue/order and add only a private Redirector overlay/binding with all of the
following properties:

1. At submission, decode the source's 16:16 fields under a synchronous
   guest-memory lease; copy writes into host staging storage. Keep only numeric
   guest locations and host-private staging after return.
2. For reads, complete into host staging; at the original completion point,
   copy bytes and result words back under a new checked lease. No worker keeps
   a native guest pointer or a lease past its call epoch.
3. Bind the worker to its owning `session`; retain handles and completion
   records only through the existing host-resource and completion-callback
   mapping instances. Do not add a Redirector mapper.
4. Preserve original cancellation, queue removal, event close and no-ANR
   cleanup ordering. ANR completion must first reach the original
   `VrNmPipeInterrupt`/SoftPC-ICA mechanical route; no direct callback or
   fabricated interrupt is permitted.

## Current disposition

S2 retains the source's precise unavailable result for async named-pipe
requests. Synchronous named-pipe and mailslot calls remain separately eligible
for their original local bindings. This conclusion neither enables a 57:xx
selector nor claims a guest callback/IRQ result.
