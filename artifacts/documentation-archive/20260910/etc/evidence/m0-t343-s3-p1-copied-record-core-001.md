# M0 T343 S3 P1 — BaseVDM copied-record core

## Result

`broker` now owns a transport-neutral, fixed-width DOS record core in
`base_vdm_record.c` and `base_vdm_record.h`.  It preserves the selected
source-level BaseVDM mechanics without serializing the NT4 CSR product shell:

- a record carries copied `VDMINFO`-shaped DOS request/result values, not a
  `VDMINFO *`;
- `VDMState` retains the original DOS request bits and rejects WOW, PIF and
  separate-WOW forms rather than treating them as empty DOS work;
- registration keys a bounded record slot by the broker/session pair;
- first empty DOS retrieval enters pending state, while the original
  `RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME` form returns no-command;
- publishing delivers exactly one copied result to that same record, and
  disconnect removes it once.

The record deliberately contains no native pointer, `HANDLE`, guest address,
mapping-manager identifier, capture buffer or process address.  A mapping
manager remains a session-local resource and cannot traverse this broker
boundary.

## Original source basis

- `opennt-host/base/win32/client/vdm.c::GetNextVDMCommand` supplies the
  caller-copy, wait and second-call ordering.
- `opennt-host/base/win32/server/srvvdm.h` supplies the record roles and
  `VDMINFO.VDMState` terminology.
- `opennt-host/base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand` supplies
  the DOS wait/no-command distinction and source-record consumption ordering.

## Verification

The reproducible `broker-base-vdm-record-test.exe` Ninja target compiles and
passes for both `x86` and `x64`.  It covers registration, pending, the second
empty-call result, copied publish/consume, rejected mixed DOS/WOW state,
cross-record rejection and close-once disconnect.  The regular
`original-softpc-candidate` target then reported `no work to do` on both
formal CPU40 graphs after compiling `broker.lib`.

## Explicit non-claims

This P1 does **not** bind an original BaseVDM caller, create a named pipe or
event transport, implement BaseSrv/CSRSS, expose a raw handle/pointer, or
implement PIF, shared-WOW, parent lifecycle, batch, cross-user or process
discovery behavior.  Those forms retain the S2 ledger's named owner transfers.
