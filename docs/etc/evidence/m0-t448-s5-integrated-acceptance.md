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

## Requirement-to-source reconciliation

The proposal's nine inherited rows are the fixed inventory, not nine remaining
runtime bugs. Paths below are repository-relative; S3/S4 ledgers retain the
earlier source-order/mirror review and exact run records.

| Row | Current owner and source | Assertions inspected / evidence | Boundary |
| --- | --- | --- | --- |
| 1 bootstrap | `src/run16-exe/main.c`, connect_broker: one absolute five-second deadline, one absent-service spawn | `base_client_rpc_first_test.c`, rpc_wait_contract bootstrap asserts ERROR_TIMEOUT, 4.5–8s diagnostic envelope and server still alive; S4 rpc-r16 passed | This is pre-admission only; endpoint polling is retained Q01, not session reconnect |
| 2 admission identity | `src/run16-exe/frontend_scope.c`, launch_native closes exported worker/target diagnostics; wait_direct_event waits service/receipt only | `frontend_scope_lifetime_test.c` asserts target diagnostic handle already closed, one submission on ERROR_NOT_READY, loss wins even with signalled receipt; fresh r2 pass | Fixture substitutes transport; real RPC admission and public startup cover integration |
| 3 result and parent return | `src/ntsrv-exe/opennt/source/frontend_registry.c`, WaitFrontendConsoleRestored owns parent-resume submission and root return; native_commands.c owns task result | service parent-resume-origin/reentry/frontend-wait/completed-worker-loss; scope fixture distinguishes result37, final-I/O failure and unfinished failure; r1 natural cmd/command actual return | Native error is not silently reclassified as target success |
| 4 loss | NTSRV client core authenticates service identity; executable cleanup consumes the loss; `src/worker-base/connection.c` delegates, adds no role policy | rpc_wait_contract loss asserts broken call mapping to ERROR_INVALID_HANDLE and latched loss; r2 real suspended-service death plus scope simultaneous receipt/death pass | No peer-restoration ordering guarantee after service death, as approved; no service revival |
| 5 frontend authority | frontend_registry.c return barrier; lifecycle.c reconciliation; NTCON session_service.c executes grants/park/close | frontend-authority fixture checks wrong role/generation/capability, duplicate startup, park without retirement and blocked command cancellation; r2 service40 and frontend-root-parent twice pass | Test cleanup is not evidence of empty-service retirement |
| 6 WOW control | worker_registry.c ConnectionControls; NTVDM console_client.c lifecycle reception separate from character setup | S4 io-r16 lifecycle-wow/pending/loss and natural paths, service40 wow-start-late-query; S3 actual shared/separate startup witnesses | No NTCON for WOW; broader WOW32 features remain queued |
| 7 NTVWM failure and GUI | native_commands.c bind/result and lifecycle.c disposition; NTVWM reports failures and receives exit authorization | r2 frontend-io-fault/worker-console-exit twice, service management-gui rejects target management; fresh dual-width GUI acceptance r3 passed | Native GUI target is observed, not managed; GUI run cannot substitute for CUI handoff |
| 8 waits/mirror diff | Approved three policies and original waits; S4 Q01-Q13 disposition | S4 full13/incremental, r17 presentation521/native142/close CPU; RPC r16 ordinary reply/connect/disconnect waits exceed removed10s then succeed | Active output sampling and original timers retained explicitly; S5 adds no product/mirror/guest diff |
| 9 actual Console return | Same NTSRV close path regardless of requester location | r1 eight conhost cases; r2 service close-order pairs, late close, pending admission, worker exit and requester/root loss | Enumerated interleavings, not exhaustive OS schedule proof; other terminal hosts not newly certified |

## Fresh fault and concurrency run

Hidden controller34028 ran `build/t448-s5/verify-r2.ps1` from20:15:20.488
to20:15:50.776 UTC, passed. Selected Ninja fixture targets reported no work to
do. The scope lifetime fixture passed its native-result, one-submit, loss,
park, I/O-fault, failed-restore and final-join assertions. Service40 completed
in4481ms; observer hash90E87C36333CE5421DC71578BEA9DC78AB494FB4317936A867352AF0E9EF37ED
and fixture hashF0E901460153FBD3DFAE605B20465D7D38AC1DBBC11F87EDC86A6ADE72AB9C5F
match S4 r16. Thus its exact assertion set is reused, not inferred from a new
aggregate name.

Two serial real-package rounds each passed ntvwm-public-startup (concurrent
first callers, exact61/62 results and resident carrier), frontend-root-parent,
worker-console-exit and frontend-io-fault. The latter two use the documented
admitted test worker, not a claim of induced arbitrary guest failure. The final
real RPC loss case terminated the pinned service while a request was pending
and verified the disconnected-handle mapping. Raw outputs and results remain
under build/t448-s5; r2-result.json records success and the owned controller.

## GUI acceptance and final delivery audit

Hidden controller9064 ran gui-r3.ps1 from20:17:10.399 to20:18:03.785 UTC.
Both x86 and x64 actual GUI probes passed startup-only exit0, --wait exit37,
GUI-created text child, nonresident carrier/independent target survival across
the diagnostic11s observation, and text-GUI-text return19. The probe creates
a real unshown HWND on the isolated desktop. Its source SHA256 is
B05D3747B191FE1E24CD55326F72D1436D9BF0CB19684E166A6BAC87BC712667.
Probe hashes and actual process reports are in r3-result.json and gui-r3-*.
Only test binaries were rebuilt, in stable .ninja configuration paths.

The final review covers all six original host C files changed since T447's
transfer baseline4d4ef4d47, including nt_event.c/DIV-355 as well as the five
DIV-356 files checked by S4. It verifies upstream/current hashes, normalized
diffs and exception registration. No guest or S5 production change is admitted.
Full13 build/incremental evidence remains S4 r16 (1420 unchanged intermediates);
no production source/build input has changed since the delivered S4 baseline.
S5's test-only commits do not justify rebuilding or refreshing product binaries.

Whole-T non-Markdown line deltas against4d4ef4d47 are owned source+2052/-1867,
tests+2520/-1147, build tools+42/-7 and six mirror C files+59/-30. Thus owned
source is net+185, tests net+1373 and mirror code net+29: this delivery does
not meet the aspirational total-line reduction. Removed duplicate control,
peer-wait/deadline and polling paths do not justify disguising the added
authorization/cleanup and regression coverage as a net reduction. S5 adds no
production/mirror code; its P1 extends one existing harness, not a second one.

Initial final-review controller37948 reported failure despite the governance
tool itself passing: the wrapper inherited git diff --no-index's expected1
for registered differences. No commit followed. The corrected wrapper resets
that native exit status before invoking the PowerShell gate and retains the
failed record; subsequent review uses a fresh result name.

Final-review-r2 controller18296 passed at20:21:03.451 UTC: all six registered
mirror comparisons and all13 hashes agreed; production/build-input diff since
S4 was empty, governance and whitespace checks passed. No additional runtime
repair was indicated by this fixed acceptance matrix.

## Delivery and retained limits

- Final documentation delivery repeats governance and hash verification before
  commit/push and clean-state verification. No unverified production fix is
  waiting to publish. S5 implementation/acceptance is complete; T acceptance
  remains the owner's decision, not implied by this record.
- Verify the final coherent package identity and review the test/document diff;
  commit/push only after required checks. T closure still requires owner
  acceptance. SOL startup/manual usability is owner-confirmed with layout debt;
  WRITE remains startup-frontier evidence, not editing/exit acceptance.
