# Atomic task retirement candidate

`wow_window_procedure_adopt_native` can select another live task as the owner
while holding the runtime lock. Previously `retire_current_thread` released
that lock, scanned for live owned windows, then `retire_task` reacquired it to
clean up and free the owner. The check and retirement were therefore not one
transaction. This source-level gap is not claimed as the cause of the observed
1400 or as a reproduced production orphan.

The candidate moves the existing `ERROR_BUSY` check into `retire_task` after
it takes the same runtime lock and before cleanup/unlink. The terminal cleanup
flag is also set under that lock. No guest or original mirror source changed.
The existing task-lifecycle fixture passed live-window refusal, failure-state
preservation, task cleanup and exit; current WOW32 cached build passed. A new
coherent ten-image package was staged over complete guest media and its 59
immutable files pass the media check. The fixture uses controlled boundaries;
the exact concurrent adoption/retirement interleaving is not forced. Existing
old-package results do not certify the new package. No release was made.

The execution-frame `ERROR_BUSY` check was then moved into that same critical
section, eliminating the earlier separate lock acquisition and the interval
between execution validation and final retirement. The selected task fixture
again passes active-frame refusal and existing cleanup assertions, and WOW32
rebuilt. `execution-check/` binds this newer source, fixture and newly staged
complete-media package; all earlier package integration results above and
below predate this second change and do not certify the newer package. A
deterministic cross-task interleaving and integration tests remain open.
The test-only `RetirementLockProbe` now inserts a synchronization witness into
a disposable copy of the exact selected lifecycle source. At the beginning of
its actual live-window scan a second native thread tries the runtime lock.
The current implementation excluded the competitor on all three observed
scans and completed the existing lifecycle fixture. The committed prior source
was separately compiled with the same witness at its prior scan location;
the competitor acquired the lock and the fixture rejected it (exit 92).
Final source, instrumented inputs and binary hashes and both outputs are in
`execution-check/lock-contention-r2/`. The witness preserves last error and
is not present in the production object. It demonstrates exclusion at the
scan point, not an end-to-end window-adoption call or every interleaving.
The selected-object `window-borrow` fixture independently passed actual native
first-subclass adoption, publication rollback/retry, retained identity and
terminal retirement with and without an existing subclass. Its callback is a
test double and both adoption cases are same-thread; `execution-check/native-adoption/`
retains output and hashes. It does not combine cross-task adoption with the
lock-contention witness. A subsequent cached NTVDM/WOW32 build reported no
work for either image.
The newer package subsequently passed the real same-worker idle/dialog test
and selected complete Control suite; their separate package-bound results are
under `execution-check/idle-dialog/` and `execution-check/control/`. Neither
forces the adoption/retirement interleaving or constitutes the Full suite.
The same newer candidate also passed Console-17 and Window-17; summaries and
the staged manifest are in `execution-check/product-matrix/`.
The current selected-object native-nested fixture separately passed nested
callback ownership, wait/exception unwind, dead-target error propagation and
receiver retirement with and without a held lock; `execution-check/native-nested/`
contains its output and input hashes. The page-domain parent is a test seam;
these cases are not guest concurrent-publication acceptance.
Three SOL Options cycles on the newer package also completed
(`execution-check/sol-options/`), with open times 4219/2125/2000 ms and
close/recovery 2562/2218/2234 ms. Seconds-long delay remains observable;
this is not a paired benchmark or proof of performance resolution.

The earlier `atomic-retire-package` passed the actual same-worker idle registrar/common-dialog
pair: 17 dialogs, 16 explicit closes, both task retirements and idle dispatch
after peer exit (`idle-dialog/`). The current selected-object process
retirement fixture also passed task/object/class refusal and retry, cleanup,
events and native ownership (`process-retirement/`). Neither forces the
specific adoption-versus-retirement interleaving.

The earlier `atomic-retire-package` (before the execution-frame check was
moved into the retirement critical section) passed three visible SOL Options
open/close/recovery rounds
and launcher completion (`sol-options/`). Observed open times were 3157,
2360 and 2250 ms, with close/recovery 2687, 2328 and 2718 ms. These absolute
times remain seconds long; this run is not a paired performance comparison or
proof that the reported multi-window slowdown is resolved.

The prior source-bound diagnostic attributed substantial cumulative time to
native projection and copied client-graph publication, with per-call timings
inclusive of setup and nesting; it is not a current-package paired benchmark.
The remaining full-slot count in `copy_current_window_view` also cross-checks
published slots against the borrow records used for copying. Replacing that
count with a record-only count would discard that mismatch check and is not
justified by the present evidence. The earlier direct-mapped graph-row cache
also falls back to a linear scan on collisions, so its worst-case complexity
remains quadratic; its SOL run was a non-pass. Neither shortcut has been added.

The earlier `atomic-retire-package` also passed the selected complete Control suite (`control/`):
exports, RPC, GUI routing, version rejection, terminal handoffs, relaunches,
frontend loss and broker I/O. This is not the Full suite, nor a forced
cross-task adoption/retirement interleaving.

That earlier candidate passed the Console-17 and Window-17 command matrices;
`product-matrix/` retains the summaries and staged manifest. This is not the
Full suite and does not exercise the specific concurrency counterexample.

The same candidate passed three Notepad Find open/close cycles and launcher
completion. The immutable guest and WOW32 hashes, timings and limitations are
in [Notepad witness](execution-check/notepad-find/review.md). This does not
prove SOL performance or replace the Full suite.

The same candidate also completed Program Manager New / Program Group Properties open, close and parent recovery in an isolated group-file setup; see [witness](execution-check/progman-new/review.md). Observed times remain over one second and are not a paired benchmark or a performance resolution.

A subsequent identity recheck compared every entry in `execution-check/product-matrix/manifest.json` with the current staged `atomic-execution-retire-package`: all ten binary SHA-256 values match. The current `wow_user_task_lifecycle.c` SHA-256 91E684C4A863677C4C4BFC1402071BE7A05AA781C062B1E4A7C51E0B079ECBD8 also matches `execution-check/inputs-sha256.txt`. This detects later changes to that source and staged binary set; it does not establish a complete source-to-image attestation for every input or replace Full acceptance.

A further isolated original Write witness exercised Find and parent recovery; see [Write evidence](execution-check/write-find/review.md). This does not cover editing, document persistence or Full acceptance.

A fresh Ninja dry run of the selected worker `ntvdm.exe` and WOW32 graphs reported `no work to do` for both. Their cached unsealed hashes match the respective input hashes in `execution-check/manifest.json`: worker `7B4D0AD556AFE550288BE99390E4D832E25EA81C5E9C16F1343C20FE2BDDDE74`, WOW32 `098790BCEEC9DDB0376ABC4086463280D7A936463AAC8FE18636AE45DFD04A5C`. This is an incremental graph and input-identity check, not a fresh full build or a validation of other components.

The regenerated x64 top-level graph did not expose inner service work. Explicit `ninja -C build/.ninja/msvc-x64-release/service -n` showed three tasks; the actual VS-initialized cached service build exited 0, compiling generated/base_vdm_config.c and rebuilding its library and ntsrv.exe. A second service dry run reports no work. The resulting unsealed ntsrv.exe SHA-256 is A45A91653F4CE610E1AB01EABA933804BF1BBA3644E3CD8541E846D7573611CB, different from the tested candidate manifest's input BCA21E3621B15FCFDF27CD27CC9D49D6CB2893BB4BE1C672669FDC59458ACA6F. The exact reason for differing bytes and behavioral impact are not yet established. Therefore the previously tested ten-image package is not a proof of the current ten build inputs. It must be coherently restaged and tested before any publication; no partial replacement was made. [Command and logs](execution-check/cached-service/) are retained.
