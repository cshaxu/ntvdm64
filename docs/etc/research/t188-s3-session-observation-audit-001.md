# T188 S3 session-observation audit 001

Date: 2026-08-12  
Packet: M0 T188 S3  
Disposition: no existing general side-effect-free runtime/session observer.

## Audit

The process-local runtime owns its `bx_ntvdm_host_session_v1` as a private static field. Its public runtime header exposes only immutable host-drive copying, installation diagnostic status, dispatch, and `take_pending_*` transfers. Every generic pending accessor either transfers and clears an operation or clears it on a mismatched request. The one read-only session accessor, `pending_guest_read_consumer`, is not reachable through runtime, applies only to the guest-read kind, and cannot classify multi-write queue occupancy.

Although the session structure names `pending_kind`, exposing its private address or reading it directly from Bochs would violate the typed boundary and couple Bochs to adapter layout. Reset, dispatch, queue and take APIs are all side-effecting and are therefore rejected as diagnostic observers.

## Result

No existing observer can establish whether the live `54:0C` pass-through sees a nonempty session, while preserving the route. The next narrow candidate is an adapter-owned fixed-width snapshot API that copies only lifecycle-neutral state such as installed/provider/pending-kind flags, has no pointer or selector/service field, and is called only after the generic runtime dispatch returns. Bochs would merely emit that copied record behind a default-off diagnostic; it would not interpret it. This requires a new S admission, exception-register entry and a fresh one-shot observation contract.
