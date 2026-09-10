# M0 T358 S5 — guest COMMAND `/C EXIT` topology audit

## Question

After the first successful `54:01` delivery of `/C EXIT`, does the original
guest/host path require a missing COMMAND BOP leaf, or a missing Base VDM
record-lifecycle capability?

## Original `/C` path

1. `cmdGetNextCmd` copies the delivered application and CR/LF-terminated
   command tail into the original guest `CMDINFO`/`UCOMBUF` contract.  It does
   not interpret `EXIT` itself.
2. Original guest `cmd/command/init.asm` recognizes `/C` and stores the
   remaining command tail in `SingleCom`; the selected `COMMAND.COM` image and
   sources are unchanged.
3. Original guest transient code in `tcode.asm` executes the command.  Its
   `EXIT` path reaches `$EXIT`; a non-permanent single command returns through
   the normal DOS process-return path.  The permanent root-COMMAND-only branch
   invokes `CMDSVC SVC_CMDEXITVDM`, whose original host table entry is
   `cmdExitVDM -> TerminateVDM`.
4. For the normal child return, the original `cmdReturnExitCode` (`54:0B`)
   invokes `GetNextVDMCommand` with `RETURN_ON_NO_COMMAND`.  It expects the
   BaseClient/BaseSrv pair to settle the DOS record lifecycle before deciding
   whether to re-enter or return a completed shell-out result.

## Exact missing state cohort

The original server `opennt-host/base/win32/server/srvvdm.c` owns more than a
single command payload for this route.  Its DOS path maintains
`VDM_TO_TAKE_A_COMMAND`, `VDM_BUSY`, and `VDM_HAS_RETURNED_ERROR_CODE` records,
parent wait handles, and the `RETURN_ON_NO_COMMAND`/`ASKING_FOR_SECOND_TIME`
transition.  BaseClient waits once only when BaseSrv explicitly returns a DOS
wait object, then retries with `ASKING_FOR_SECOND_TIME`.

The current `base_vdm_local` correctly models the first one-shot delivery but
has no DOS parent/child record state.  Once that record is consumed, its
empty-queue branch always publishes the local wait event.  In the child-return
case no producer can signal that event, so `base_vdm_local_wait_for_command`
blocks instead of reaching the original second-request no-command outcome.

This explains the observed second `54:01` wait without claiming that the
guest failed to parse `/C`, that `EXIT` was not delivered, or that a COMMAND
BOP leaf is absent.

## Disposition

The next owner cohort is a bounded, source-derived **single-session Base VDM
DOS record lifecycle** in `adapter-mvdm-host-out/basesrv`.  It must preserve
the original record-state/copy/wake order needed by first-command and child
return; it may not import CSRSS, CSR transport, cross-process records, WOW
notification, raw handles, or a generic scheduler.  The original guest and
COMMAND host source remain unchanged.
