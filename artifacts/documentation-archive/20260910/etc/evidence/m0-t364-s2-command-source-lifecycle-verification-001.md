# M0 T364 S2 — COMMAND source lifecycle verification

## Question

Does the selected local Base VDM route preserve the original COMMAND
command-record lifecycle sufficiently for first delivery and later ordinary
reacquisition, without app parsing a command or adding a BOP result?

## Original source comparison

The imported OpenNT source at
`opennt-host/base/win32/{client/vdm.c,server/srvvdm.c}` establishes the
contract:

- `GetNextVDMCommand` uses a captured `VDMINFO`, waits on the server's returned
  DOS wait object, adds `ASKING_FOR_SECOND_TIME`, then retries.
- `BaseSrvGetNextVDMCommand` differentiates DOS and WOW requests, returns an
  empty successful WOW result rather than blocking WOWEXEC, delivers a ready
  DOS record with its copied `VDMINFO`, and otherwise creates/returns a DOS
  wait object.
- A DOS record changes from `VDM_TO_TAKE_A_COMMAND` to `VDM_BUSY` on delivery.
  With no available next DOS record, an ordinary non-`RETURN_ON_NO_COMMAND`
  caller waits; only the explicit return-on-no-command retry receives the
  original terminal no-command result.
- `BaseSrvCopyCommand` owns copied command, application, environment and
  directory payloads before dispatch.  The caller does not lend a pointer to
  a queued VDM.

The selected local profile cannot compose CSR capture buffers, CSRSS console
records, duplicated parent handles or the external process broker.  It retains
the reached local one-session slice in `base_vdm_client.c` and
`base_vdm_local.c`: copied `VDMINFO`, copied command record, DOS/WOW selection,
pending event, client retry and source-shaped terminal no-command disposition.
The app declaration remains a producer of one copied original-shaped bootstrap
record; it does not parse `EXIT` or any other guest command.

## Procedure

Ran the existing formally generated x86 fixture from its recorded Ninja graph:

```text
ninja -C build/M0-T302/S2/x86 test
```

It rebuilt the affected fixture/client/local/broker/declaration units and
reported:

```text
PASS: local Base VDM broker contract
```

The fixture asserts all of the following in one session:

1. original-style first-record delivery with no environment capture buffer;
2. environment retrieval and required-size failure reporting;
3. copied `/C EXIT\r\n\0` declaration/application/environment/current-directory
   shape;
4. a consumed DOS record followed by a separate publisher that observes
   pending, publishes, signals, and causes BaseClient retry to return a new
   copied command;
5. the explicit `RETURN_ON_NO_COMMAND` terminal result after a completed
   one-session record;
6. reentry count balance, DOS/WOW queue separation, unsupported separate-WOW
   failure, current-directory copied ownership and session teardown.

## Result and disposition

The whole currently selected command-record transport is already source-shaped
and locally verified.  No production replacement, command parser, guest-memory
mapping, handle pass-through or BOP change is required in S2.

The later fixed `54:01` wait remains the intended ordinary DOS no-next-record
outcome.  It is not an `EXIT` parsing failure.  A future public command-input
or cross-process producer must enter through the existing copied
`base_vdm_broker_publish`/`base_vdm_broker_deliver` boundary; it may not call
or modify original COMMAND directly.  That future producer is outside this
single-record `/C` owner cohort and does not block formal closure here.

## Confidence and follow-up

High confidence for the selected one-session transport, based on direct
client/server source comparison plus an executed x86 lifecycle fixture.  The
next S must prove that this exact transport and original COMMAND package are
linked in the CPU40/x86 product; only then may the task perform its one frozen
command-delivery observation.
