# M0 T368 S1 — Original COMMAND post-record topology ledger

## Scope and method

This is a static, source-first ledger for the exact original route after a
Base VDM record is returned through COMMAND BOP `54:01`.  It does not infer
execution from a repeated BOP in T367's single observation, and it does not
change a provider, guest image, machine, or runtime configuration.

The relevant owner boundary is the imported original COMMAND host package
under `src/mvdm-host/dos/command/`, its existing same-shaped Base VDM binding
under `src/adapter-mvdm-host-out/basesrv/`, and the immutable DOS COMMAND
guest mirror under `src/mvdm-guest/dos/v86/cmd/command/`.

## Ordered original route

1. The guest transient COMMAND loop prepares the `CMDINFO` buffer and issues
   `CMDSVC SVC_CMDGETNEXTCMD` in
   `src/mvdm-guest/dos/v86/cmd/command/tcode.asm` (lines 537--558).  Carry
   clear enters `run_cmd`; carry set is exclusively the documented
   environment-buffer-resize path, which returns to `do_again`.
2. `CmdDispatch` in `src/mvdm-host/dos/command/cmddisp.c` maps that selector
   to original `cmdGetNextCmd` in `cmdmisc.c`.  The product-side dispatch
   continuation record is diagnostic only; it does not parse or execute the
   guest command.
3. Original `cmdGetNextCmd` snapshots the guest `CMDINFO`, constructs a
   `VDMINFO`, and calls `GetNextVDMCommand`.  The only adapted portions are
   the bounded session guest-memory leases and opaque redirection identity
   seams already registered as `MVDM-HOST-DIV-194`, `-160`, `-111`, and the
   source-shaped redirection seam.  The original ordering remains: return
   record, transform/copy command information, write `CMDINFO`, clear carry.
4. The current Base VDM binding's `get_next_command` in
   `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c` preserves the
   reached BaseSrv distinction: an eligible DOS record copies command,
   application, environment and current-directory fields into `VDMINFO`,
   consumes the record, marks it busy, and returns success.  An empty DOS
   queue returns `STATUS_PENDING`, unless the original
   `RETURN_ON_NO_COMMAND` terminal conditions apply.  A WOW request has the
   distinct successful-empty result.  Thus an empty second DOS request is not
   evidence of a missing command parser or a valid reason to synthesize a
   command.
5. On a successful return, `cmdGetNextCmd` writes the file portion of
   `VDMINFO.AppName` plus the supplied command tail into guest `UCOMBUF`,
   preserves the full application path in `EXECPATH`, records its extension
   in `EXECEXT_TYPE`, updates the guest environment/drive/code-page and
   redirection fields, then clears carry.  This is in
   `src/mvdm-host/dos/command/cmdmisc.c` (the `Prepare ccom's UCOMBUF` block
   through the final `setCF(0)`).
6. Guest `run_cmd` continues in `tcode.asm`, applies original standard-handle
   and code-page work, reaches `GotCom`, copies `UCOMBUF` to `COMBUF`, and
   enters the original `DOCOM -> PRESCAN -> parseline` path.  No host-side
   parser is part of this route.
7. For an external `.COM`/`.EXE`, original `tmisc1.asm:EXTERNAL` uses the
   host-populated `EXECPATH_SIZE` exactly once.  When nonzero it bypasses the
   regular guest PATH search, uses `EXECEXT_TYPE`, and enters `EXECUTE`.
   `EXECUTE` performs original redirection setup, prepares the resident EXEC
   block and jumps through the resident `EXEC_ADDR` transaction.  The
   resident `Exec_Trap` transfers into original `Ext_Exec`; its return path
   obtains the DOS return code through `WAITPROCESS` and continues COMMAND
   recovery (`command1.asm:Exec_Ret` / `Exec_Wait`).

## Important separations

- `54:08`, `54:0A`, and `54:0B` are native-child/COMSPEC lifecycle selectors
  reached by distinct guest paths.  They are not the handoff for the selected
  DOS `.COM` route and cannot be used to replace the guest INT 21h EXEC path.
- The final INT 21h EXEC and `$Exec` implementation belong to the immutable
  DOS guest/NTDOS owner.  T368 neither recompiles nor alters it.
- The source route has no point at which an app-side DOS loader or a new BOP
  leaf handler is legitimate.  The app's sole role is publication of the
  one source-shaped Base VDM record.

## T367 observation interpretation

T367 observed an initial original `54:01` delivery and later an empty
continuation/wait.  This ledger establishes that a later empty DOS
`GetNextVDMCommand` is compatible with the current source-shaped
`STATUS_PENDING` contract.  The observation did not contain enough
instruction-level evidence to prove whether the first successful record
reached `GotCom`, `EXTERNAL`, `EXECUTE`, or the resident INT 21h EXEC
transaction.  It therefore cannot identify a missing COMMAND implementation.

## S2 admission decision

No missing host COMMAND cohort is proved by the static route.  The earliest
complete owner cohort is already present: original `cmdGetNextCmd` record
copy-out plus the original guest transient `run_cmd -> GotCom -> EXTERNAL ->
EXECUTE` transfer.  S2 is consequently limited to a source/ABI closure check
of that existing cohort, including its final-link inputs and record field
contract; it may not add a synthetic parser, no-next result, host loader, or
new BOP service.  If that check finds no gap, S2 records a no-code closure and
T368 proceeds directly to formal link and one fixed observation.

## Source locations consulted

- `src/mvdm-host/dos/command/cmddisp.c`
- `src/mvdm-host/dos/command/cmdmisc.c`
- `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c`
- `src/mvdm-guest/dos/v86/cmd/command/tcode.asm`
- `src/mvdm-guest/dos/v86/cmd/command/tmisc1.asm`
- `src/mvdm-guest/dos/v86/cmd/command/command1.asm`
- `src/mvdm-guest/dos/v86/cmd/command/stub.asm`
