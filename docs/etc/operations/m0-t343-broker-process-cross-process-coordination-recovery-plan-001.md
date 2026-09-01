# M0 T343 — Broker process and cross-process coordination recovery

## Purpose

Recover or explicitly transfer the original MVDM process/broker coordination
package as one owner boundary. The task starts from MVDM's Base VDM client,
monitor control and session contracts; it does not turn an individual command
or BOP into an ad-hoc IPC protocol.

## S plan

### S1 — Original broker / process / ABI / failure admission

Map every selected MVDM caller of Base VDM client, CSR/BaseSrv, VDM control,
process discovery, command delivery and cross-process event interfaces. Record
the original record layouts, ownership, public Win32 alternatives, existing
session/broker bindings and hard private boundaries.

### S2 — Selected local broker cohort

If S1 selects a finite original client cohort, retain its original body and
bind only the necessary copied/versioned session records and public Win32
operations. Preserve caller order, wait/disconnect/error behavior and mapping
rules; do not invent process scanning or global session state.

### S3 — Formal graph and contract closure

Verify both formal CPU40 rows and focused copied-record/lifecycle contracts.
Runtime observation may validate a completed broker package but may not choose
individual command or BOP behavior.

## Non-goals

No CSRSS/BaseSrv server recreation, kernel VDM implementation, unrestricted
process discovery/control, cross-user broker, raw guest/native pointer IPC,
new BOP provider, guest media change, alternate executor, Bochs route or
CPU30 route.
