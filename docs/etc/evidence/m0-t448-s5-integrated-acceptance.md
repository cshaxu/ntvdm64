# T448 S5 integrated lifecycle acceptance

## Scope and baseline

S5 reconciles the approved lifecycle contract, not new WOW32 functionality.
Baseline is S4 P2 `be2ac8dead2fadc68af39a9b6b32f0bc157327f0`, protocol64.
The [S3 ledger](m0-t448-s3-close-implementation.md) and
[S4 ledger](m0-t448-s4-polling-audit.md) retain exact prior gates and limits.
No production or guest change has been made in this S5 checkpoint.

Temporary owner is the single T448 S5 implementer/reviewer, root
`build/t448-s5`, budget8GiB and4h per run; investigate10min without progress.
Retain the sealed S3/S4 evidence and S4 package until reconciliation finishes.

## Console handoff coverage extension

Source inspection found three existing branches in
`tests/observation/console_startup_observer.c`: in-worker NTMON Delete/Y,
external same-logon management RPC, and natural `exit` in window mode.
The tracked `verify-console-caf-self-delete.ps1` previously asserted only the
first. It now selects these branches with `-Mode SelfDelete`, `ExternalClose`
or `NaturalExit`; the default remains the original three-target self-delete.
The external client uses the same management operation as NTMON, not its UI.
NaturalExit explicitly rejects NTMON: typing `exit` is a shell operation.

| Mode | Targets | Required evidence |
| --- | --- | --- |
| SelfDelete | cmd, command, ntmon | Actual NTMON exact Console confirmation, frontend exit, standard and active-buffer character echo before Enter |
| ExternalClose | cmd, command, ntmon | External management RPC success, frontend exit, standard and active-buffer character echo before Enter |
| NaturalExit | cmd, command | Fresh outer prompt, character echo before Enter, original frontend process still alive |

All modes use the existing hidden conhost Job/harness and real staged package.
Diagnostic containment budgets are test-owned, not new product timeouts.
Stage cleanup is not evidence of normal service retirement.

Run `build/t448-s5/verify-r1.ps1` first checks all13 hashes in the tested
`build/t448-s4/package-r16/system32`, committed release and installed System32,
then runs the eight cases serially and documentation/diff checks.
The first launch PID1088 disappeared before producing any result or stage;
authoritative CIM confirmed it absent. Hidden elevated controller PID18288
started2026-10-21 20:09:51 UTC with separate `r1b.stdout`/`r1b.stderr`.
Controller18288 completed successfully at20:11:42.776 UTC. All eight cases
passed: natural cmd/command at20:10:01/11; external cmd/command/ntmon at
20:10:20/30/39; self-delete cmd/command/ntmon at20:10:54/20:11:09/23.
Each case's results.json records its actual harness PID, start/end, exit0 and
raw observer report. Documentation governance and diff check also passed.
This proves the listed conhost permutations, not all host/presentation modes
or untested concurrent interleavings. No product rebuild or release refresh
is needed for these test/document-only inputs; the verified full13 S4 package
remains the installed baseline.

## Remaining finite reconciliation

- Map the admission/submission failure and startup-only/explicit-wait rows to
  current source and exact prior assertions, not just aggregate green results.
- Reconcile nested I/O return, concurrent management close and confirmed
  service loss to the same source/fixture and integrated evidence matrix.
- Verify the final coherent package identity and review the test/document diff;
  commit/push only after required checks. T closure still requires owner
  acceptance. SOL startup/manual usability is owner-confirmed with layout debt;
  WRITE remains startup-frontier evidence, not editing/exit acceptance.
