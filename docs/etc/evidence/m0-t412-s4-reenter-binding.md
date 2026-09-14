# T412 S4 reentry binding

## Question

Can the standalone three-process route carry the original COMMAND
`cmdCreateProcess` reentry calls without restoring a local BaseSrv policy?

## Inputs

- OpenNT `cmdexec.c`: the detached native-child worker calls
  `GetNextVDMCommand(INCREMENT_REENTER_COUNT)` before `CreateProcess` and
  `GetNextVDMCommand(DECREMENT_REENTER_COUNT)` after completion.
- OpenNT `vdm.c`: those two selectors route to `BasepSetReenterCount`.
- OpenNT `srvvdm.c::BaseSrvSetReenterCount`: owns the counter and its worker
  wake notification.
- S3 product graph under `build/M0-T412/S3/product`.

## Recovery

The retained original COMMAND and BaseSrv bodies are unchanged.  The finite
standalone binding adds one authenticated RPC operation carrying only the
original increment/decrement selector.  It resolves Console identity from the
authenticated connection and invokes original `BaseSrvSetReenterCount` through
the existing finite dispatch table.  No Console handle, guest pointer, command
payload, or replacement reentry counter crosses IPC.

## Verification

The MIDL x86 stubs, `basesrv.exe`, `run16.exe`, and `ntvdm.exe` relinked.
Their maps select `Client_Reenter` in both client products and select
`Server_Reenter` plus `OpenNtBaseServiceReenter` in `basesrv.exe`.

The focused product reservation/worker test used the original BaseClient call
shape.  Its worker performed GetNext, increment, normal exit GetNext, and
decrement; the authenticated BaseSrv trace recorded all four calls with zero
status, then the launcher obtained exit code 7 through the original parent
wait route.  The test also retained the pre-existing negative assertion that
an unreserved worker registration is rejected.

## Limit

This proves the BaseClient/BaseSrv reentry binding only.  It does not yet prove
an actual guest `BOP 54:08` invocation, native child stream attachment, or DOS
parent-return.  Those remain S4 acceptance work.
