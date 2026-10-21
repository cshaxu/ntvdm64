# T448 S6 unused interfaces and change-only monitor

## Fixed scope

Owner admits this S after S5 baseline 6a956824c92e8f35f4a1f0d4a69a81fddb8fa939.
Single-person implementer/reviewer; no guest, wire ABI or lifecycle policy change.
Temporary owner: build/t448-s6, 8 GiB, four hours per run; investigate ten minutes
without progress. Hidden jobs retain PID, logs and terminal results.

## Source audit

- CCPU: standalone startup and both fixtures create the halt event before CPU
  execution. Cleanup follows execution/producer retirement. Remove unreachable
  null-event Sleep(0) fallback; retain the blocking event and original caller.
- NTCON: whole source/test/tool reference audit finds no external consumer of
  task create/cancel/join/destroy, event wait/reset, wait-any, sleep/yield.
  Remove declarations, platform implementation and exclusive aliases. Further
  closure tracing proves the sole mailbox notifier selector always supplies
  win32_window_notify: remove the unused NULL-notifier default Event path and
  its event APIs as well. Retain mutexes and the actual window thread/notifier.
- NTMON: sample existing RPC snapshots every 1000 ms. Compare explicit semantic
  fields, not padding/addresses. Identical snapshot/error and elapsed time alone
  do not repaint. UI changes and resize repaint; keyboard remains immediately
  waitable. Opening details/completing management forces a fresh sample.
  Trace drawing uses the sampled nodes rather than querying again.

The nxvm manifest retains original hashes and registers selected-subset changes
with current hashes; verification is not bypassed. No OpenNT mirror or guest diff.

## Verification checkpoints

- Admission r1 rejected missing S5 closure heading; corrected label, r2 passed
  (PID 24720).
- Focused r1 compiled changed fixtures, then CCPU linking exposed missing
  rpcrt4.lib in the existing event-test link rule. Add the required import
  library; no product behavior change.
- Full build r2 correctly rejected unregistered nxvm source changes before
  compilation. Registered the subset adaptation, retaining original hashes.
- Full build r3 passed thirteen images and unchanged second-build intermediates;
  focused monitor loop and CCPU wake/reset exited zero. The final default-Event
  removal also passed full/incremental r4 and focused r4. The final four unused
  Win32 synchronization aliases are removed before full build r5.
- Runtime r4 stopped before channel behavior: its generic observer invocation
  used a desktop with maximum 58x15 characters against the fixture's required
  80x30 canvas (ERROR_INVALID_PARAMETER). Use its existing private-desktop-full
  entry, which configures a private small-font Console; do not relax assertions.
  Correct the text-handoff fixture invocation to supply its required report path.
  No S6 publication claim until the corrected runtime gate passes.
- Full/incremental r5 passed all thirteen images, 1426 unchanged intermediates;
  focused monitor/CCPU passed. Window library and full private channel fixture
  passed; text handoff passed 49 checks. CAF natural cmd/command passed.
- r5 external-close cmd/command failed the active-buffer coordinate assertion:
  standard pre-Enter echo was present at (12,5), while the active cursor had
  been sampled earlier at (0,5). Raw active snapshots contain q at (12,5).
  Source tracing found GetConsoleProcessList between the two cursor samples,
  allowing the outer prompt to finish during the diagnostic query. Move the
  active cursor sample after enumeration, adjacent to injection; do not add
  delay, relax pre-Enter assertions or change product handoff.
  The unchanged S4 baseline external-close three and S6 self-delete three
  passed in the comparison run. This does not erase r5's failed measurement.
  r6 rebuilds only the observer and repeats all eight CAF cases plus Minesweeper
  against the unchanged thirteen-image r5 package: all eight passed. Minesweeper
  flag/counter, reset, open cell, WM_CLOSE and Run16 completion passed.
- Final review additionally compiled the changed live-view fixture and exposed
  its existing missing gdi32.lib dependency. Supply it only on that bitmap test
  target, regenerate the x86 graph and rerun review. No production dependency
  is added; the already tested thirteen-image candidate is unchanged.

## Required acceptance

Verify the real monitor loop: unchanged snapshot/error does not write, changed
row/error does, cadence 1000 ms; retain keyboard/confirmation/identity/trace
tests. Run CCPU wake/reset, frontend presentation/IO and CAF natural/external/
self close regressions on the full thirteen-image candidate. Verify second
incremental build, coherent publication hashes, reviewed commit/push/clean tree.
## Delivery result

All required bounded runtime checks passed: monitor loop and CCPU wake/reset;
native window library; full private channel lifecycle; text handoff 49 checks;
eight CAF natural/external/internal-self-close cases with pre-Enter echo; actual
Minesweeper flag/counter/reset/open/WM_CLOSE and Run16 completion. Full thirteen
images and 1426 unchanged intermediate files passed the second build.

Review compiled the updated live bitmap fixture, verified all selected import
hashes/retired files, absence of removed API references, documentation and
whitespace. Its final whitespace-only EOF correction is registered in the import
manifest; it changes no compiled token or behavior. No original mirror/guest
delta. Product C/H +84/-376 (net -292), tests +66/-4, build rules +3/-2; import
metadata +36/-20. Documentation includes this new ledger separately.

Complete thirteen-image publication succeeded at assets/release and
O:/winnt/system32. publication-r5.json records both hashes and recoverable prior
package at build/t448-s6/publication-r5-backup. Existing identical images remain
hash-identical; this is a complete coherent package, not partial deployment.
Final delivery controller verifies governance, both destinations, candidate,
explicit staged paths, commit/push identity and clean checkout.

Limits: real live-view bitmap/modal RPC capture was compile-checked but not rerun;
deterministic modal/layout/identity tests and actual NTMON management were run.
No claim of exhaustive host scheduling coverage; inherited SOL layout and WRITE
startup-only limits are unchanged. Await owner S6 acceptance, not T closure.
