# T465 S3 — classic DDE loss and release ledger

## Decision

S3 closes the selected classic-DDE loss/release cohort without a product
repair.  The source and real two-task witness establish that the selected
original transport has **no conversation or task-lifetime reaper**.  It owns
only conversion records and paired global-memory aliases; application-owned
DDE data remains live until the application explicitly releases it or the
WOW worker itself terminates.

Adding a sweep from `W32FreeTask` would invent a second ownership authority,
change the original global-memory contract, and can free data still needed by
the surviving peer.  It is therefore deliberately not added.  DDEML remains
outside the admitted set.

## Selected-owner ledger

| Event | Original authority | Release / outcome | S3 evidence |
| --- | --- | --- | --- |
| Rejected posted conversion | `wumsg.c::W32CancelDDEPost` and its `__finally` path | Frees only the undelivered packed-message and conversion-owned pair. | Existing real S2 rejected-DATA-post witness: one `FreeDDElParam`, one paired `GlobalFree`. |
| Normal INITIATE | `wdde.c::DDEInitiateList`; `wmdisp32.c::WM32DDEInitiate` and matching unthunk | Per-invocation initiator node is retired after direct-atom ACK conversion. | Existing S2 native INITIATE/ACK witness. |
| Normal data/ACK | `wdde.c::phDDEFirst`, `W32DdeFreeHandle16`, `W32DDEFreeGlobalMem32` | The endpoint that calls `GlobalFree` removes the pair before the opposite free callback. | Existing S2 bidirectional EXECUTE/ACK and DATA/POKE/ADVISE witness. |
| Sending Win16 task exits while another task survives | `wkman.c::W32DestroyTask` → `W32FreeTask`; global-memory release remains KERNEL/`W32DdeFreeHandle16`-driven | No task-wide DDE alias sweep occurs.  The sender-owned alias remains application-owned; final worker process teardown reclaims process memory. | S3 real task-loss witness below. |
| Native peer/module disappears | Classic DDE messages carry application HWND/atom/handle endpoints; selected `wdde.c` has no peer-module or conversation-manager table. | There is no automatic peer/module reaper to reconstruct.  Delivery failure uses the selected cancellation path; a live application must follow classic ACK/TERMINATE/explicit-free protocol. | Source census; S2 failed-post rollback is the admitted delivery-failure witness. |

## Real task-loss witness

The noninteractive probe runs two actual Win16 tasks in one WOW worker.  The
sender creates a `WM_DDE_DATA` alias, arms a native `GlobalFree` observer,
notifies the receiver, and exits without an explicit `GlobalFree`.  The
receiver waits for the sender HWND to disappear, then closes the native peer.

The fresh run `build/t465-s3-taskloss-r7` passed:

- `MSGSEND` reached phase `0,1,2,3` and armed the loss alias;
- `MSGRECV` reached `0,5,6`, observed the sender window disappear, and
  survived through normal shutdown;
- the native report recorded all prior normal DDE ownership events but
  `ddeTaskLossFree=0`; and
- guest inputs were immutable.

This is not a leak claim: the test deliberately models an application that
retains its own DDE allocation.  It proves the precise negative requirement:
task cleanup does not silently become the allocation owner.

## Scope and S4 handoff

The ledger is finite for the selected classic transport.  It does not claim
DDEML, item-atom conversation management, arbitrary malformed-peer recovery,
cross-worker DDE scheduling, or automatic ownership cleanup on peer/module
death.  S4 owns only aggregate acceptance of S1--S3; it must not turn these
explicit original stopping boundaries into a new provider or scheduler.

No guest, product source, asset, installed component or release image changed.
