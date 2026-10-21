# base

This frontend-selected `base` subset owns blocking mutexes.
It depends only on header-only `types`; no Console, KVM, storage or application
dependency is permitted. The selected Win32 headers remain component-local.

Use `sync_interface.h`.
Mutexes require a live object,
same-thread unlock and no recursion; destroy requires no owner or waiters.
M0 T448 S6 removes the uncalled task, event, wait/cancellation, reset and
sleep/yield APIs and unused default mailbox Event from this frontend subset; the
window component retains its own actively used native thread/message loop.
These are the existing primitives,
not forwarding aliases for another component. Caller-owned locks retain their
existing lock order and callback non-reentry requirements.
