# M0 T358 S9 — `/C EXIT` completion-versus-reacquisition audit

## Question

What does the later observed `54:01` mean after a declared `/C EXIT` record,
and what source-owned marker is needed to prove T358's bounded built-in
completion rather than merely another COMMAND BOP ingress?

## Original continuation

1. `cmdmisc.c::cmdGetNextCmd` requests the first DOS record with
   `ASKING_FOR_FIRST_COMMAND | ASKING_FOR_DOS_BINARY`; Base VDM copies the
   declared application and CR/LF command tail into the original `CMDINFO` /
   `UCOMBUF` contract.
2. The selected guest `COMMAND.COM` parses `/C` in `init.asm::SetSSwitch`.
   It stores the tail in `SingleCom` and explicitly clears `PermCom`.
3. `tcode.asm` executes a single command and converts completed
   `SingleCom` to `-1`; `TCOMMAND` transfers to `$EXITPREP`.
4. `tcmd2b.asm::$EXIT` branches on `PermCom`.  The permanent COMMAND branch
   calls `SVC_CMDEXITVDM` (`54:00`) and never returns.  The `/C` branch has
   `PermCom=0`, instead frees the child COMMAND state and uses original DOS
   termination (`INT 21h AH=4Ch`) to return to its parent.
5. A resident parent COMMAND can then call original `54:01` again to acquire
   a new DOS command.  Its request is no longer
   `ASKING_FOR_FIRST_COMMAND`; BaseSrv's original DOS-record logic considers
   its `VDM_BUSY` record, marks a completed child as returned where
   appropriate, and either serves another record or exposes its wait path.

`cmdReturnExitCode` / `54:0B` is separately used by the guest's DOS-binary
shell-out paths.  It is not the direct `COMMAND.COM /C EXIT` terminal above,
so the absence of `54:0B` cannot disprove the selected built-in route.

## Current binding and frozen facts

The valid root-layout observation reaches a first `54:01`, then `54:0E` and
`54:04`, and later a second `54:01`.  It records neither `54:00` nor `54:0B`.
That is consistent with the source-defined non-permanent `/C` exit returning
to a parent COMMAND rather than killing the session.

The current one-session Base VDM binding preserves the first record's
`TO_TAKE_A_COMMAND -> BUSY` transition.  It also keeps `RETURN_ON_NO_COMMAND`
as the explicit child-return form.  A normal later `cmdGetNextCmd` request
still uses the original ordinary DOS command-acquisition route.  No evidence
currently proves the exact `CMDINFO` role, `IsFirstCall`/`IsRepeatCall` state,
or guest call site of either observed `54:01`, so the service number alone is
not a completion proof.

## Disposition

No missing COMMAND BOP, guest rewrite, command producer, or BaseSrv/CSRSS
product shell is selected by this audit.  The immediate bounded next step is
a default-off, state-neutral original COMMAND-call observer which records, at
the existing dispatcher boundary only:

- service number and entry/return stage;
- guest `CS:IP`, `AX`, `BX`, `CF`; and
- `IsFirstCall`, `IsRepeatCall`, and the local DOS-record state as copied
  scalar diagnostics.

The observer must not retain a guest pointer, decode command text, mutate a
record, change a return value, add a command producer, or alter the guest.
One later frozen workload can then show whether the second `54:01` is the
source-defined parent reacquisition that completes `/C EXIT`, or name a
different earliest source-owned boundary.

