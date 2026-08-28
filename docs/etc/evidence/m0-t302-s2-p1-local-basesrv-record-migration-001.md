# M0 T302 S2 P1 — local BaseSrv record migration

## Question

Can the temporary app/monitor command callback be removed while preserving the
reached one-session DOS `VDMINFO` record contract in a named BaseSrv owner
family?

## Inputs

- exact OpenNT BaseClient `base/win32/client/vdm.c:GetNextVDMCommand`;
- exact OpenNT BaseSrv `base/win32/server/srvvdm.c:BaseSrvGetNextVDMCommand`;
- T302/S1 boundary and seam-disposition ledgers;
- `session` teardown and thread-binding facilities.

## Procedure

1. Moved the former app-owned copied record and monitor callback into the
   named `adapter-mvdm-host-out/basesrv` family.
2. Replaced the callback entry with the original public spelling
   `GetNextVDMCommand(PVDMINFO)` and a session-bound copied record.
3. Preserved the reached DOS copy/capacity, environment-only, reentry,
   no-command and teardown paths. Kept unsupported WOW/PIF and product-shell
   paths as explicit failures.
4. Removed the obsolete monitor command facade and moved its historical Ninja
   recipe out of the active build-tool directory.
5. Generated and executed the T302 formal Ninja graph for both architectures:

   ```text
   powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T302BaseVdmLocalNinja.ps1 -Architecture x64
   ninja -C build/M0-T302/S2/x64 test
   powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T302BaseVdmLocalNinja.ps1 -Architecture x86
   ninja -C build/M0-T302/S2/x86 test
   ```

## Observations

Both x64 and x86 fixtures report `PASS: local VDM command transport`. The
fixture covers publish, environment-only retrieval, capacity feedback/retry,
full copied result, terminal no-command result, reentry increment/decrement,
WOW refusal, unbind and session teardown.

## Interpretation

The app is no longer a parallel Base VDM protocol owner; it may later supply
launch input only. The reached local DOS record is now owned by the correctly
named adapter family and uses the original public entry spelling. This is a
source-shaped local BaseSrv rehost, not direct composition of CSRSS/CSR code.

## Remaining limit

P1 does not yet directly compose the original BaseClient capture transport or
the original BaseSrv wait/wake product shell. CSRSS, cross-process discovery,
first-VDM, WOW/PIF and child-process record branches remain deferred exactly
as T302/S2 specifies.
