# M0 T364 S1 — COMMAND command-delivery owner ledger

## Question

What exact original owner path produces the later `54:01` reached by the
closed T363 frozen CPU40 workload, and does its observed non-return require a
COMMAND BOP repair, a machine repair, or a whole command-source recovery?

## Inputs

- The frozen T363 S4 observation and its `54:01` continuation markers.
- Mirrored OpenNT COMMAND sources:
  `mvdm-host/dos/command/{cmddisp.c,cmdmisc.c,cmddata.c,cmdexec.c}`.
- The selected BaseClient-shaped transport:
  `adapter-mvdm-host-out/basesrv/source/{base_vdm_client.c,base_vdm_local.c,base_vdm_broker.c}`.
- App composition input:
  `app/launch_declaration.c`.

## Procedure

1. Follow `SVC_CMDGETNEXTCMD` from the original COMMAND service table through
   `cmdGetNextCmd`.
2. Review every first/repeat state transition and the `CMDINFO`/`VDMINFO`
   ownership boundary.
3. Follow the same-shaped `GetNextVDMCommand` client request through the
   one-session Base VDM record and broker.
4. Compare the resulting source transition with the fixed T363 record without
   varying the executable, package root, command, observer, or runtime
   container.

## Source-owned command-delivery chain

1. Guest `54:01` enters original `CmdDispatch` in `cmddisp.c`; table slot
   `SVC_CMDGETNEXTCMD` invokes original `cmdGetNextCmd` in `cmdmisc.c`.
   The T363/T364 observer calls around this table entry are explicitly
   default-off diagnostics only; they neither decode nor route the BOP.
2. `cmdGetNextCmd` snapshots the fixed guest `CMDINFO`, prepares a caller-owned
   `VDMINFO`, then calls `GetNextVDMCommand`.  The source body remains selected.
   Its only relevant product divergences are bounded mapping leases for the
   former `GetVDMAddr` aliases (`MVDM-HOST-DIV-194`) and a host-path receiving
   capacity that matches its original `MAX_PATH + 1` advertisement
   (`MVDM-HOST-DIV-160`).  Neither changes first/repeat command selection.
3. On the first request, original `cmdGetNextCmd` sets
   `ASKING_FOR_FIRST_COMMAND | ASKING_FOR_DOS_BINARY`, preserves its completed
   guest environment, initializes printer flushing, and requests one command.
   The app launch declaration supplies one copied original-shaped record:
   application `<system-root>\\system32\\COMMAND.COM`, command tail
   `/C <declared text>\r\n\0`, `COMSPEC`/`PATH` multi-string, current directory
   and code page.
4. `GetNextVDMCommand` is a source-shaped BaseClient boundary.  It snapshots
   the caller's `VDMINFO`, dispatches the copied request, and only waits/retries
   after `ERROR_IO_PENDING`; no guest/native pointer is retained.  The original
   NT4 CSR capture/transport is unavailable and is not recreated.
5. `base_vdm_local_dispatch` retains the selected BaseSrv record choice:
   DOS requests consume only a DOS record; an empty DOS record records pending
   state and returns `STATUS_PENDING`; an empty WOW request returns the
   source-defined successful empty response.  It neither parses a DOS command
   nor invents a guest result.
6. The local broker owns only copied command-record transport.  The app publishes
   and delivers exactly one DOS record.  Successful normal delivery copies it
   into the Base VDM record, clears `available`, marks the DOS record busy and
   returns to the unchanged original COMMAND body.
7. The original body writes `CMDINFO`, UCOMBUF, environment/current-directory
   and redirection results back through bounded leases, clears `IsFirstCall`,
   and returns `CF=0`.  COMMAND.COM subsequently owns `/C` parsing and built-in
   execution; app and the Base VDM adapter do not parse `EXIT`.
8. A later original `54:01` with `IsFirstCall == 0` and `IsRepeatCall == 0`
   asks again for a DOS command.  Its request has no
   `ASKING_FOR_SECOND_TIME` or `RETURN_ON_NO_COMMAND` flag.  With the one
   bootstrap record already consumed, the source-shaped local Base VDM record
   returns pending; BaseClient waits on its copied wake event and retries only
   when another producer delivers a DOS record.

## Observations

The single T363 S4 fixed observation proved the first request's full return:

```text
MVDM-CMD-CONT svc=01 stage=0 ... first=1 repeat=0 dos-state=00000001
MVDM-CMD-CONT svc=01 stage=1 ... first=0 repeat=0 dos-state=00000002
MVDM-BOP-RETURN 54:01 ... ax=0002 cf=0
```

Later it reached another `54:01` entry with `first=0`, `repeat=0`, and the
same busy record disposition but did not return before the controlled T363
timeout.  The static chain above explains this as an unserved ordinary DOS
command request, not as a printer, SoftPC CPU, BOP-dispatch, guest-pointer or
COMMAND built-in parser failure.

## Disposition and recovery ladder

| Rung | Disposition |
| --- | --- |
| Direct original reuse | `cmddisp.c`, `cmdmisc.c`, `cmddata.c`, `CMDINFO`/`VDMINFO` ordering and COMMAND.COM built-in execution remain selected original source. |
| Same-shaped binding | Existing BaseClient/BaseSrv local record retains copied request, buffer sizing, DOS/WOW selection, pending wait/retry and source failure direction where NT4 CSR/CSRSS transport is unavailable. |
| External intrusion | Not applicable. |
| New behavior | Not admitted.  In particular, app must not become a DOS parser or synthesize a post-`/C` command. |

The required next recovery cohort is therefore finite: source-shaped ordinary
DOS command production, copied queue lifecycle, wake/retry behavior, and
COMMAND handoff for the app-selected command-input source.  It excludes DOS
`EXEC`, `cmdExec32`, child return, WOW, CSRSS recreation, and all machine
changes.

## Confidence and follow-up

High confidence for the first/second request distinction: it is established by
the selected original branch flags, the local adapter's explicit pending
contract, and the one frozen record.  S2 must recover the complete app/session
command-input ownership and its local positive/negative queue behavior before
any additional product observation.  It must not infer interactive console
semantics from the fixed `/C EXIT` workload.
