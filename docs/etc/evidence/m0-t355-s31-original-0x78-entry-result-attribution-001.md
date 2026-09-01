# M0 T355 S31 — original `0x78` entry-result attribution

## Fixed result facts

The single S30 fixed-container result is `exit=0x00000078`.  The observer
sets `MVDM_MAIN_RETURN_REPORT_PATH` before it creates the child, but its
`m0-t355-s30-composition-close.txt.return.txt` file is absent.  The matching
exception report is absent as well.

Thus the selected original `obj.vdm/ntvdm.c` body did not reach its normal
`mvdm_softpc_record_main_return(ret); return ret;` tail, and it did not take
the observed top-level exception filter.  The result instead passes through
the existing session escape:

```text
source or adapter terminal
  -> session_terminate_current(120)
  -> longjmp(owner->termination_escape)
  -> mvdm_softpc_execution_run_original_entry
  -> app result propagation
  -> successful session disposal
```

The absent S30 disposal report excludes the app-owned `0x48` binding-count
failure.  No runtime behavior was changed or re-run in this audit.

## Original and selected terminal routes

- The ordinary original `host_main` tail calls `host_applClose()` and returns
  `-1`; it cannot explain an observed `0x78` normal entry return.
- Original `host_terminate()` carries `VdmExitCode` through the selected
  session replacement.  The direct source writer in `dos/command/cmdmisc.c`
  is guarded by `!IsFirstCall`, `!IsRepeatCall`, `DosSessionId` and
  `pfdata.CloseOnExit`.
- The fixed observer invokes the product as `-f -o --ordinary-child`, without
  `-i...`; original `host_applInit` therefore leaves `DosSessionId == 0`.
  The app's published local Base VDM record also has `error_code == 0`.
  That COMMAND `VdmExitCode` writer is not a selected explanation for `120`.
- The formal graph intentionally links three controlled-stop adapters which
  use `ERROR_CALL_NOT_IMPLEMENTED` (`120`): CCPU
  `ActivityCheckAfterTimeSlice`, VDD `ClearInstanceDataMarking`, and private
  WOW/debugger hard-error paths.  The first has non-WOW host callers in
  `nt_unix.c` and `nt_eoi.c`; the others require VDD/WOW/debugger conditions
  not established by the fixed observation.

The original source sets no per-terminal provenance value that survives the
current session escape.  The number `120` alone therefore proves the
controlled-stop class, but not which of those linked source-shaped terminal
entries was reached.

## Source availability

The complete available OpenNT, OpenNT 4.5 and OpenNT source-2 baselines
contain declarations and callers of `ActivityCheckAfterTimeSlice`, but no
provider definition.  It is consequently not legitimate to silently turn the
current unavailable adapter into an arbitrary no-op or scheduling policy.

## Bounded successor

S32 is limited to a default-off provenance record at the existing controlled
termination boundary.  It will copy a stable source-entry identifier and
completion code already selected by the caller; it will not alter a BOP,
guest memory, CPU state, timing, session outcome, or error value.  One fixed
observation can then select the actual source-owned recovery package rather
than guessing from `120`.
