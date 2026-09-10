# M0 T343 S3 P2 — BaseVDM caller-to-broker binding

## Result

The selected app launch declaration now uses the only admitted bridge:

`app launch declaration → adapter-mvdm-host-out/basesrv → broker copied record → base_vdm_local → original GetNextVDMCommand caller`.

`base_vdm_broker` preserves the original MVDM-facing shape.  It does not
replace `GetNextVDMCommand`, does not retain a caller `VDMINFO *`, and does not
make `base_vdm_local` expose broker values to guest state.  The bridge accepts
only DOS records; PIF, shared WOW and separate WOW remain explicit rejects.

Delivery is atomic at the selected record boundary: it peeks the available
fixed-width record, performs the existing local copied publication, and marks
the broker record consumed only if that publication succeeds.  An unavailable
destination therefore cannot silently discard a source command.

The session owns bridge teardown.  Its local broker/session IDs are scalar
identity values; no host pointer, `HANDLE`, guest address or mapping-manager
ID enters the copied record.

## Source basis

The original BaseClient preserves a caller-owned capture and retries after a
DOS wait.  The original BaseSrv validates/copied a DOS command result before
removing its source record.  The bridge adopts precisely those two observable
ordering requirements while replacing the private CSR record with the bounded
P1 copied record.

## Verification

`basesrv-base-vdm-broker-test.exe` passed on both x86 and x64.  It proves that
an already-occupied local destination leaves the broker record available, then
proves a subsequent same-record delivery copies the original command bytes and
closes cleanly.  Both formal CPU40 `original-softpc-candidate` Ninja graphs
compiled the adapter and app composition incrementally afterwards.

## Explicit non-claims

This is an in-process, one-session binding.  It does not implement a public
pipe/event transport, process authentication, cross-process BaseSrv server,
CSRSS, shared WOW registration, PIF/separate WOW, batch, parent lifecycle or
raw resource transport.
