# M0 T194 S37: Total Observation Record

Packet: M0 T194 S37  
Disposition: controller repair verified; no observation was launched.

## Question

Can the bounded controller preserve its final JSON record when a child process
still holds `stdout.log` or `stderr.log` at cleanup time?

## Change

`tools/probe/Invoke-T194S10V6BoundedObservation.ps1` now uses one `Try-Hash` helper
for both terminal log hashes. It returns null for a missing file or a
`Get-FileHash` failure, allowing the pre-existing `finally` record write to
continue. It does not alter the invocation, watchdog, exact `taskkill /T /F`
scope, bounded five-second post-cleanup wait, retry policy, or any runtime
input.

This directly closes the S36 controller-only failure mode. A null hash means
the terminal record could not safely obtain that hash at that instant; it is
not evidence that the log was absent, complete, or semantically valid.

## Verification

`tests/bx-ntvdm-adapter/Test-T194S34ObservationControllerBoundary.ps1` passed.
The focused static test requires the common helper, its non-throwing catch,
both hash call sites, the bounded cleanup terms and the final JSON write. It
continues to reject unbounded waits, retry loops and dynamic execution.

No process was launched, killed or inspected by this S. In particular, S36 is
not retried. Documentation governance and `git diff --check` remain required
for this delivery.

## Follow-up

The source-backed next product decision remains the coherent `demIOCTL`
drive-metadata provider group: `AL=08` and `AL=09` together, selected through
the common ingress/registry/DEM plane and supplied only by the immutable CLI
drive snapshot. Raw-media `AL=0D`/`AL=11` remains deferred. That future work
requires a fresh S admission and, if runtime evidence is needed, a separately
authorized one-shot invocation.
