# T484 S3 P85 CCPU call and privilege runners

Owner clarified that already migrated tools must also be reviewed for current
use and obsolete/duplicated tools removed. B0118 is retired, not migrated:
its CALLF owner and register/stack assertions are folded into the existing
verify-ccpu-callback.ps1 runner, which already builds and executes the same
high-linear fixture. No extra CMD target or separate compiler wrapper remains.
The previous CALLF-only passing run does not prove the consolidated runner;
that final run is recorded separately before delivery.
B0119 executes a native fixture and checks link owners/results; it has no build
producer. Initial batch PID26852 failed its obsolete diagnostic-file assertion.
Commit7717ce596 removed mvdm_softpc_report_wow_far_call_validation from c_xfer.c
on2026-11-12, but this runner/fixture still set trace variables and required a
far-validate file. Actual invalid-target result was vector13/error8 with
unchanged CR2. This is a stale test expectation, not evidence to restore the
removed product tracing hook.

Remove only that expired tracing contract and three identical invalid-target
runs differentiated solely by the removed trace selector. The two actual
execution paths remain: invalid descriptor faults before reading the absent
page; valid descriptor succeeds; both retain CR2 and seven original symbol
owner checks. The fixture accepts only no argument or valid. Production,
mirror and original guest code are unchanged; invalid CLI inputs are rejected.

B0120/B0121 were already retired with delivery evidence and are not counted
again. B0122 already delegates compilation to CMD ccpu-port-privilege and uses
Python only for assertion/reporting. Its native source, provider map checks,
scalar/string widths, privilege and memory assertions are unchanged.

Corrected batch PID30232 exited0: CALLF behavior, both far-destination paths
and117 port cases pass. Historical /FORCE duplicate fixture seam warnings
remain explicitly outside production link behavior. These are native/selected
CCPU tests, not real guest dispatch acceptance.

[Runtime receipt](../../../artifacts/build-migration/t484/s3-callf/result.json),
first-failure receipt and subsequent cache/publication/governance receipts are
separate. Raw logs are build/t484-s3-callf. The retained verification scripts
replay from a fresh build directory after adjusting their explicit result roots.
All11 raw host inputs and22 publication hashes are checked before delivery;
unchanged images do not require republication.

Following final verification/commit/push, B0118 is retired and B0119/B0122
are retained native test delegates. Fixed355 inventory:167 complete,188
remaining (53 pending producer/staging/verification rows,135 reconciliation).
S3 remains active; final interpreter-free clean acceptance remains S5.

Final consolidation verification: owned runner PID28252 completed both actual
CCPU fixtures with exit0; the high-linear run emitted CCPU_CALLF_RETF_OK.
Final gate PID15840 exited0: all three cached binaries retained hashes and
mtimes, unsupported callback architecture and malformed far arguments were
rejected, all11 raw product inputs and22 published copies matched, and
documentation/diff checks passed. Cached full product build PID30812 exited0
before consolidation; subsequent edits only remove a redundant target alias
and merge runner assertions. No product input or published image changed.

The owner-directed retrospective review is not complete. This batch removes
one redundant standalone tool and its proposed build alias; the existing
callback runner retains its original two cases plus the CALLF witness.
All already migrated producers remain subject to the plan's retention review.