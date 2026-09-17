# M0 T419 S4: DTASKMGR disconnected empty-state

## Scope and boundary

The owner required `DTASKMGR.EXE` to remain a useful, ordinary empty task
screen when `basesrv.exe` is absent: it must label that condition and retry,
rather than exit or replace the table with an error-only screen.  It must not
start BaseSrv.  This is a Console presentation change in
`src/dtaskmgr/main.c`; it changes neither the BaseSrv management IDL nor the
original DOS/WOW record owner.

OpenNT has no standalone Console task manager.  Consequently the applicable
source-policy level is newly authored product presentation only.  The existing
BaseSrv binding and `TaskSnapshot` failure result remain the source of truth.

## Implementation and proof

`render` now always writes `No product tasks.` for a zero-row snapshot.  If
the refresh status is nonzero it additionally writes `BaseSrv not connected
(error <n>); refresh will retry.` and the header says `BaseSrv not connected`.
The main loop still calls `refresh`, renders, and waits 750 ms; it has no
`CreateProcess`, `ShellExecute` or broker-start path.  Thus a missing broker
does not end DTASKMGR and cannot cause one to be launched.

The former error-only strings (`BaseSrv unavailable` and `Broker unavailable`)
are absent from the selected source.  There is no selected worker on a
zero-row result, so the existing termination command naturally returns its
no-selection result without making a management termination request.

## Verification

Build root: `build/M0-T419/S4/dtaskmgr-disconnected-x86-001`.

1. A fresh formal MSVC x86 graph built all selected product files:
   `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `DTASKMGR.EXE`, and `VDMREDIR.dll`,
   plus `basesrv-service-reservation-test.exe` and `dtaskmgr-rpc-test.exe`.
2. BaseSrv lifecycle fixtures passed:
   - `PASS: original Check/Update/Get/ExitVDM lifecycle completes through authenticated worker binding`
   - `PASS: management epoch/worker termination performs original worker-exit cleanup`
   - `PASS: authenticated DTASKMGR RPC sees empty broker, rejects version and cannot terminate absent worker`
3. The deployed `O:\winnt` package passed the established observer matrix:
   `direct-mem=0`, `mem-repeat=1`, `nested-mem=1`, `edit=1`.
   Raw observations are retained under
   `O:\winnt\logs\m0-t419-s4-dtaskmgr-*`.
4. With no `basesrv.exe` process present, the deployed `DTASKMGR.EXE` remained
   alive for two seconds (`PASS: DTASKMGR stayed alive for 2 seconds without
   BaseSrv`).  The single hidden test instance was then ended; no product
   worker was involved.
5. Deployment SHA-256:

   | File | SHA-256 |
   | --- | --- |
   | `run16.exe` | `9AF19BC25920A2431072287D434078939CFE06C401F5CF6A40414EAA06DB4A24` |
   | `basesrv.exe` | `E4834C0ED7AA5F2C2BA3D43F92AD5108738367EA54152A8C85FB0F0846B5153B` |
   | `ntvdm.exe` | `3B69BFB4ABE69EB62800F4A1F1861FF31163E5D3C8BF3605F837299AEDB13A4F` |
   | `DTASKMGR.EXE` | `3B45743591E2966875E3CE2945AEDEF207DDCF899CB5526542D7B8F0BB59C84A` |

## Acceptance limitation

The native Console visual check intentionally remains owner-operated: run
`O:\winnt\DTASKMGR.EXE` with no `basesrv.exe` and confirm the normal empty
table plus the separate disconnected status; then start a product COMMAND
session and confirm a live worker row appears.  No terminal UI automation was
used.  S4 implementation may close after its delivery, while T419 remains
open pending that owner acceptance.
