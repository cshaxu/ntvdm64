# M0 T343 — Broker process and cross-process coordination recovery

## Purpose

Recover or explicitly transfer the original MVDM process/broker coordination
package as one owner boundary. The task starts from MVDM's Base VDM client,
monitor control and session contracts; it does not turn an individual command
or BOP into an ad-hoc IPC protocol.

## S plan

### S1 — Original broker / process / ABI / failure admission (closed)

Map every selected MVDM caller of Base VDM client, CSR/BaseSrv, VDM control,
process discovery, command delivery and cross-process event interfaces. Record
the original record layouts, ownership, public Win32 alternatives, existing
session/broker bindings and hard private boundaries.

### S2 — Original BaseSrv copied-record broker contract audit (closed)

Map the selected BaseSrv DOS, console, WOW, PIF, parent-wait, re-entry,
directory, batch and disconnect records/messages.  Determine the exact
fixed-width copied-record wire content that preserves the original
client/server contract and the operations that remain assigned to COMMAND or
WOW.  The current session-local client remains a valid one-session form; this
audit does not enable a transport.

### S3 — Selected BaseVDM copied-record broker binding (closed)

Extend the existing `broker` component only with the selected DOS
request/wait/retry, identity and disconnect cohort.  Bind original callers
through `adapter-mvdm-host-out/basesrv`; preserve copied request/result,
same-record wake/retry, capacity/error and close-once contracts.  Use public
process/event/pipe facilities only behind the broker implementation.  Do not
admit PIF/separate-WOW, shared-WOW registration, COMMAND parent lifecycle or
any raw handle/pointer transport.

**Result:** the selected DOS record now has a fixed-width core and an
adapter-owned peek/copy/consume binding into the original caller's local
record. It is intentionally transport-neutral and in-process; it does not
claim the omitted BaseSrv product forms.

### S4 — Formal graph and contract closure (active)

Verify both formal CPU40 rows and focused copied-record/lifecycle contracts.
Runtime observation may validate a completed broker package but may not choose
individual command or BOP behavior.

## Non-goals

No CSRSS/BaseSrv server recreation, kernel VDM implementation, unrestricted
process discovery/control, cross-user broker, raw guest/native pointer IPC,
new BOP provider, guest media change, alternate executor, Bochs route or
CPU30 route.
