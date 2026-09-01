# M0 T355 S21 — fixed-container observation after original XMS backing selection

## Scope and immutable inputs

S21 made exactly one non-debug, console-owning observation.  It used the S20
CPU40/x86 product and the existing immutable `O:\ntvdm` media package.  No
source, guest, firmware, observer, timeout, or container setting changed.

The durable records are:

```text
O:\ntvdm\m0-t355-s21-xms.txt
O:\ntvdm\m0-t355-s21-xms.txt.json
O:\ntvdm\m0-t355-s21-xms.txt.bop-return.txt
O:\ntvdm\m0-t355-s21-xms.txt.dem-open.txt
O:\ntvdm\m0-t355-s21-xms.txt.console.txt
```

The staged product is the S20 formal output with SHA-256
`555b4ff35c291bd24e802be11f676134dc38e19d8ab6355d7335590980c913bc`.
The observation JSON records the fixed-media manifest SHA-256
`15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`
and the stage manifest SHA-256
`cd007a94bd72b0abcc145dd6d27930b3d3c38d1d7d3a144a8a17545e3067758f`.

## Result

The observer reports `result=exited`, `exit=0x00000048`, and the admitted
8-second timeout.  There is no S21 exception report and no direct-RAM report.
In particular, the former first-XMS terminal at requested physical address
`0x00110000` is absent.

The retained BOP trace reaches materially beyond that predecessor.  It
includes original return records for `54:05`, `54:0C`, `54:04`, `54:02`,
`54:0D`, and `54:0F`, together with repeated `50:42` and `50:02` calls.  The
original DEM open records successfully open `HIMEM.SYS`, `COUNTRY.SYS`,
`COMMAND.COM`, `REDIR.EXE`, and `DOSX.EXE` under the fixed MVDM system root.
Those facts prove only post-XMS progression and ordinary host-file activity;
they do not prove COMMAND completion, guest-program completion, or a
continuous DOS session.

## Terminal attribution

`0x48` is decimal 72.  `app/entry.c` defines 72 as
`APP_STARTUP_DISPOSE_FAILURE` and returns it only when `session_dispose()`
rejects the final session cleanup.  The exact static rejection predicates are
an invalid session, a nonzero thread-binding count, or an armed termination
escape (`session/session.c`).  The one permitted observation does not expose
which predicate remained true, so it must not be attributed to a particular
SoftPC, DEM, COMMAND, XMS, BaseSrv, or BOP implementation.

The original path may legitimately call `ExitVDM`, which is currently bound
through the source-shaped Base VDM client/local replacement.  That path calls
`session_complete`; it is distinct from the generic disposal invariant.  This
run does not prove that the original exit call happened, nor that it failed.

## Disposition

S21 closes the XMS backing question: the original non-`i386` allocation/commit
selection removed the prior direct-RAM terminal without changing CCPU fallback
or the `50:42` contract.  The sole successor is an app/session lifecycle audit
of original return, completion, binding and termination-escape ownership.
It is not a trace-authorized repair and does not authorize a second execution.
