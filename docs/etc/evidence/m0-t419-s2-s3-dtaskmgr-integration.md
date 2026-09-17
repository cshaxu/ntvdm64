# M0 T419 S2--S3: DTASKMGR integration

## Scope and source-first disposition

The owner requested a native Console task manager for the standalone
three-program product, including an explicitly confirmed, worker-scoped kill
operation.  OpenNT has no corresponding standalone task-manager program or
management RPC.  The recovery disposition is therefore:

1. **Original source reuse:** the selected, compiled
   `src/opennt-host/base/win32/server/srvvdm.c` owns `DOSHead`, `WOWHead`,
   `DOSRECORD`, `WOWRECORD`, `VDMINFO`, worker sequence numbers and the
   post-exit `BaseSrvCleanupVDMResources` path.  `base_service.c` reads those
   owners under their existing locks; it does not create a second registry.
2. **Smallest source-shaped binding:**
   `src/basesrv/opennt/source/base_service.c` copies a bounded projection from
   the existing authenticated worker watch and original record into
   `OPENNT_BASE_WORKER_INFO`.  The copied data is worker sequence, kind,
   original record state/task/name and registration time.  It has no raw
   pointer, PID or HANDLE.
3. **External intrusion:** none.
4. **New product behavior:** the versioned management RPC and
   `src/dtaskmgr/main.c` Console presentation are new because neither an NT4
   CSRSS management endpoint nor a standalone Console task manager is an
   available composable OpenNT component.

## Boundary

`TaskSnapshot` and `TerminateWorker` take the existing same-user authenticated
RPC process attachment plus `APP_PROTOCOL_VERSION` and `APP_VERSION`.  They
deliberately do not take `VDM_CONNECTION`; polling therefore cannot enter the
original client/worker registry or retain a broker by registration.  The
selection key is `{broker_epoch:uint64, worker_sequence:uint32}`.

BaseSrv creates the epoch once per service start.  It rejects an old epoch
with `ERROR_REVISION_MISMATCH`.  `DTASKMGR` clears a visual selection whenever
the returned epoch changes.  A terminate request contains no PID or native
handle: BaseSrv finds the retained authenticated worker watch, duplicates its
local process handle, invokes `TerminateProcess` outside the service lock, and
lets the already selected worker-exit callback run original record cleanup.
The confirmation text says that all DOS/WOW child tasks hosted by that worker
will end.

## Implementation

- `src/dtaskmgr/main.c`: native Console TUI with unavailable/empty states,
  750 ms refresh, bounded rows, original app-name display, Up/Down selection,
  Q/Escape exit, and K/Delete confirmation.
- `src/basesrv/transport/service.idl`: fixed-width copied management wire
  records and the two authenticated methods.
- `src/basesrv/main.c`: version validation, transport peer verification,
  two-pass bounded snapshot marshalling, and post-call empty-timer
  re-evaluation.
- `src/basesrv/opennt/source/base_service.c`: epoch, worker start time,
  original DOS/WOW record projection, and retained-handle termination.
- `tests/adapter-basesrv/base_service_reservation_test.c`: proves the actual
  `MEM.EXE` original app-name projection, one-entry snapshot, non-zero start
  time, stale-epoch refusal, server-owned worker termination, and original
  worker-exit cleanup inside the established Check/Update/Get/ExitVDM
  lifecycle.

## Verification

Build root: `build/M0-T419/S2/dtaskmgr-x86-001`.

1. Fresh MSVC x86 formal graph: `product-programs` built all four selected
   executables (`run16.exe`, `basesrv.exe`, `ntvdm.exe`, `DTASKMGR.EXE`) and
   `VDMREDIR.dll`; subsequent Ninja reports `no work to do`.
2. `basesrv-service-reservation-test.exe` passed after the new assertions:
   `PASS: original Check/Update/Get/ExitVDM lifecycle completes through
   authenticated worker binding`.  Its isolated
   `--management-terminate` mode also passed: selected epoch/worker
   termination wakes the parent with `ERROR_PROCESS_ABORTED` and runs the
   original worker-exit cleanup.
3. `dtaskmgr-rpc-test.exe` started the sibling formal `basesrv.exe` and passed
   its real authenticated RPC checks: empty row set plus non-zero epoch,
   `ERROR_REVISION_MISMATCH` for a mismatched protocol, and
   `ERROR_NOT_FOUND` for an absent worker termination request.
4. The deployed `O:\winnt` package passed the existing real-ConPTY matrix:
   `direct-mem=0`, `mem-repeat=1`, `nested-mem=1`, `edit=1`.  Raw observation
   remains below `O:\winnt\logs\m0-t419-s3-dtaskmgr-*`.
5. Deployment SHA-256:

   | File | SHA-256 |
   | --- | --- |
   | `run16.exe` | `5021292874E53899E1CF0B67E937401A3B0BFEE1C834E7063476A5BC5CF7EBE1` |
   | `basesrv.exe` | `F88F3F4E1D912FE19AFCFF2B72CA72528FE35CC8532172818693CCCFF0AA4C8A` |
   | `ntvdm.exe` | `ED938A73F2A8BAFEE2CA2A039732E33DEAB28C5EB76DE080768C3D213E633D1D` |
   | `DTASKMGR.EXE` | `EC09C7F15F2CA7E9B45E4EAD3792F9195E868796774DAC4933BD58C539E2E568` |

## Remaining acceptance

The automated checks cover the source/ABI/lifecycle path and existing product
regressions.  The final visual TUI check is intentionally left for the owner
in a real Windows Terminal or conhost session: start `O:\winnt\DTASKMGR.EXE`,
confirm its unavailable/empty state, then observe a worker row while a product
COMMAND session is live.  No Windows-terminal UI automation was used.
