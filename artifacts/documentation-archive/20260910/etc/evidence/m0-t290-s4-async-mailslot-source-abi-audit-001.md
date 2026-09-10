# M0 T290 S4 — mailslot and async source/ABI audit

## Scope

This record audits the admitted original Redirector group before any route is
enabled: `vrmslot.c` (`57:09..0e`), `vrnmpipe.c` asynchronous read/write
(`57:23/24`, plus cancellation) and `vrinit.c` completion delivery.  It is a
source/ABI decision record, not evidence that a Redirector selector is live.

## Mailslots

- `VrDeleteMailslot`, `VrGetMailslotInfo`, `VrMakeMailslot`,
  `VrReadMailslot` and `VrWriteMailslot` retain original public Win32 calls
  (`CreateMailslot`, `GetMailslotInfo`, `SetMailslotInfo`, `ReadFile`,
  `WriteFile`) and original DOS register/error order.  They are eligible for
  source-body composition with bounded synchronous guest leases.
- `VrPeekMailslot` is already an original explicit `ERROR_NOT_SUPPORTED`
  result.  It needs regression proof, not an NT-level replacement.
- `VR_MAILSLOT_INFO::Handle32` is host-private.  It may retain a native-width
  `HANDLE` because it never crosses into guest memory.  The guest-visible
  `Handle16` must not become a truncated native handle.
- The original private `VrpAllocateHandle16`/`VrpFreeHandle16` bitmap is a
  historical handle allocator.  It cannot become a fourth mapping-manager
  implementation.  The composition binding must use the current session's
  existing `host_resource_mappings` instance to publish and release the
  private record, accepting only nonzero identifiers representable as a
  16-bit DOS handle.  Exhaustion retains the source failure direction.  This
  is a same-shaped adapter binding, not a new generic mapper.
- Every source `POINTER_FROM_WORDS`, `LPSTR_FROM_WORDS` or `READ_FAR_POINTER`
  use must be entered only after declaring its exact synchronous span in the
  existing Redirector pointer scope.  No temporary host pointer persists
  after the original function returns.

## Asynchronous named pipes

- `VrReadWriteAsyncNmPipe` preserves the original worker/queue/error order:
  source creates a wake event and `CreateThread`, records an `OVERLAPPED`
  request, starts `ReadFile`/`WriteFile`, then returns carry clear only for
  pending or immediately started I/O.
- Its original `DOS_ASYNC_NAMED_PIPE_INFO` persists `Buffer`, error/count
  destinations, semaphore and ANR values after the BOP call returns.
  `VrpCompleteAsyncRequest` later writes those destinations and queues
  `VrNmPipeInterrupt`; `VrNmPipeInterrupt` turns the persisted ANR into guest
  registers.  On NT4 those values were usable VDM aliases.  They are neither
  native pointers nor transferable worker inputs on x64.
- Consequently a session worker may own only opaque native handles and copied
  completion records.  It may not inherit a guest lease, raw guest pointer,
  raw guest callback, or a direct Bochs reference.  This is a hard safety
  boundary, not an implementation preference.
- `VrQueueCompletionHandler` plus `VrRaiseInterrupt` eventually requires a
  physical IRQ.  T290/S4 explicitly excludes physical IRQ delivery.  Until a
  separately admitted selector-blind mechanical completion request exists,
  ANR-bearing completion remains source-defined unavailable; it must not
  report an invented callback success.
- A zero-ANR request is different: its source completion disposes the event
  and request itself.  It is a candidate for a bounded session-owned worker
  proof only after its buffer and output destinations are represented as
  copied completion data and cancellation/disconnect teardown is proven.
- The existing `mvdm_redirector_create_thread` facade intentionally returns
  `ERROR_CALL_NOT_IMPLEMENTED`.  S4 may replace that facade only with a
  session-bound cdecl-to-WINAPI thunk that has no guest memory or BOP
  knowledge.  It may not create a second broker or make the static original
  worker a process-global execution owner.

## S4 sequencing decision

1. Compose and test the original synchronous mailslot family, including stale
   16-bit identity, invalid guest spans and terminate cleanup, through the
   existing session mappings and pointer scope.
2. Compile the original asynchronous source with its current same-shaped
   thread facade and prove its source failure direction where no safe
   completion delivery exists.
3. Only then decide whether the zero-ANR completion record can be bound to a
   session worker without altering source ownership.  ANR/IRQ delivery stays
   unavailable unless its mechanical owner is separately admitted.

## Explicit exclusions

No selector ingress, raw host handle/pointer in guest memory, additional
mapping-manager implementation, direct Bochs call, synthetic callback, or
NetAPI/NetBIOS/DLC work is authorized by this audit.
