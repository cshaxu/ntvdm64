# M0 T383 S1 — original COMMAND startup command-delivery owner ledger

## Question

Before changing any provider, does the current source-shaped COMMAND/Base VDM
composition already implement the complete first-command contract between
`SVC_GETINITENVIRONMENT (54:0F)` and `SVC_CMDGETNEXTCMD (54:01)`?

## Inputs

- Selected original COMMAND sources:
  `O:\repos.external\OpenNT\base\mvdm\dos\command\cmdenv.c`
  (`AFC71D306491FE6A7F8CD7356B8682176A33A05F3652BAE735BE079B3CD0A32B`) and
  `cmdmisc.c`
  (`1CA036932368A0D0BF1848B078CC80CD66E67A4CC46AD26A0C1B236FE40F4281`).
- Current mirror/binding sources:
  `src/mvdm-host/dos/command/{cmdenv.c,cmdmisc.c,cmddisp.c}`,
  `src/adapter-mvdm-host-out/basesrv/source/{base_vdm_client.c,base_vdm_local.c,base_vdm_broker.c}`, and
  `src/app/launch_declaration.c`.
- Fixed-stage identity check: both `O:\ntvdm64\COMMAND.COM` and
  `O:\ntvdm64\system32\COMMAND.COM` exist and are 50,384-byte selected media.
- T381 current-formal observation and T382 LPT classification.

## Owner and ABI chain

1. Original `cmdGetInitEnvironment` owns DOS's initial OEM environment.  It
   deliberately uses `BX` as a paragraph-capacity/result field: insufficient
   capacity returns the required count, then COMMAND calls it again.  Its only
   active modern boundary is the registered synchronous `ES:0` guest-memory
   lease (`MVDM-HOST-DIV-193`); construction, filtering, ordering and the
   capacity result remain original.
2. Original `cmdGetNextCmd` owns the first `54:01` request.  With
   `IsFirstCall`, it sets `ASKING_FOR_FIRST_COMMAND`, deliberately supplies no
   host environment buffer because `54:0F` already populated guest COMMAND,
   and passes caller-owned command/application/current-directory buffers to
   `GetNextVDMCommand`.  `MVDM-HOST-DIV-194` replaces only the historical
   durable guest aliases with bounded snapshots and commits.
3. `GetNextVDMCommand` retains the BaseClient name and request/capture/retry
   shape.  Its local record implementation accepts a first DOS request with
   `Enviornment == NULL` and `EnviornmentSize == 0`, requires the command
   buffer, reports capacity failures without consuming the record, copies the
   record once, then marks the DOS record busy.
4. `app_launch_declaration_publish` is not a command executor.  Before CPU
   startup it publishes one copied DOS record through the Base VDM broker:
   `application = <root>\\system32\\COMMAND.COM`, command
   `/C <declared-command>\r\n\0`, a double-NUL `COMSPEC`/`PATH`
   environment, and the selected root current directory.  The app's sole
   one-shot policy applies only when COMMAND later asks for a further record.

## Observation interpretation

The two observed `54:0F` returns are consistent with the original
initial-environment sizing handshake.  They do not prove failure of
`cmdGetInitEnvironment`, loss of the published record, or an LPT blocker.
The absent `54:01` in the eight-second CPU snapshot is therefore an
insufficient runtime discriminator; it cannot authorize a trace-selected
COMMAND/DEM/SoftPC patch.

## Disposition

No missing source-owned first-command provider or ABI branch was found.  The
complete immediate cohort is already present with its registered mapping
divergences.  S2 is consequently a no-new-provider recovery closure: retain
this source-shaped cohort and prove it locally as a whole; S3 must rebuild the
current CPU40/x86 product and run the focused original-caller proof before the
one allowed runtime observation.

## Confidence and follow-up

High confidence for source ownership and caller-visible record semantics;
medium confidence for the exact runtime delay because no intermediate CPU
progress observation has yet been admitted.  T383 S4, not S1, owns the single
fixed runtime observation.  `cmdExec32` remains explicitly out of scope until
T383 proves the first `54:01` return.
