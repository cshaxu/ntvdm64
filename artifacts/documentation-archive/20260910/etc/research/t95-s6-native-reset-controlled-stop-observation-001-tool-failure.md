# T95 S6 Native Reset-to-Controlled-Stop Observation 001 Tool Failure

## Question

Did the first admitted r4 native lifecycle observation produce a complete,
classifiable result?

## Inputs and Procedure

The admitted observer created fresh evidence root
`artifacts/analysis/t95-s6-native-reset-stop-observation-001-20260811-001`,
copied the r4 profile and both ROMs, and attempted the one approved process
launch.  It did not alter the r4 build root.

## Observation

The observer raised PowerShell's null-valued-expression error while collecting
redirected process streams.  The evidence root contains only the copied
profile/ROM inputs: no stdout, stderr, Bochs log, terminal process record, or
exit-code record was written.  The process-launch attempt therefore has no
complete command-result chain and cannot establish either reset success or a
Bochs failure.

## Interpretation

This is an observer-transport failure, not valid Bochs runtime evidence.  It
does not license an in-place rerun and makes no claim that the contained
process reached reset, configuration, benchmark activation, or controlled
stop.  The partial evidence root is retained immutable.

## Corrective Readiness Work

The observer has been changed without a further execution: a root-local CMD
wrapper now clears all adapter environment opt-ins, redirects process streams
to declared files, writes an atomic-equivalent exit-code file, and is itself
recordable.  This change is unexecuted.  Any new runtime observation requires
a separate `STATUS.md` admission and a distinct evidence root.

## Follow-Up

S6 retains native build closure.  Its runtime controlled-stop predicate is
unproven; the next coordinator decision may admit one new transport-corrected
observation or proceed with a different evidence-only task.  It must not call
the first attempt a successful or failed reset test.
