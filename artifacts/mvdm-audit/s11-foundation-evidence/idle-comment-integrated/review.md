# Current idle-comment candidate

The full current ten-image candidate was staged after a comment-only WOW32
rebuild. Guest media verification passed for 59 immutable files. The shared
WOW idle/dialog witness passed: two tasks in one worker, 17 dialogs, 16
explicit closes, a later idle notification and both retirements. Its isolated
stage uses a fresh package identity; all ten input hashes match this candidate.
The previous candidate's other receipts are not attributed to this manifest;
Full and remaining package verification are open. No publication occurred.

The isolated Console-17 and Window-17 exit-status matrices passed against
this candidate; per-matrix summaries and manifest are retained here.

All 12 Control gates passed, including RPC, GUI, version rejection, DOS/native
return, frontend loss and nested window I/O. The ten actual Control runtime
hashes match this candidate's sealed images. Timings and runtime manifest are
retained here. This does not replace the Full product gate.

Notepad Find, Write Find and Program Manager New Group each passed dialog
close, parent recovery and launcher completion. Each recorded manifest matches
all ten sealed images in this candidate. Results are in their corresponding
subdirectories. These bounded probes neither prove all WOW applications nor
resolve the separately observed native-return 1400.

The unmodified Win16 Calculator also passed About, parent recovery and
launcher exit, with all ten sealed image hashes matching this candidate.
This does not establish actual WIN87EM module identity, FPU arithmetic or
the pre-3.0 loader branch.

The real-worker VDD/MEM witness passed query, allocate, reallocate, free and
exhaustion rollback; MEM and launcher completed. This fresh-identity stage's
ten input hashes match the candidate. Results are retained in `export-live/`.

The selected current idle-hook object also passed a dedicated real USER chain
fixture: worker-shared registration, nested and exceptional callbacks,
foreign/retired task guards, and native next-hook forwarding after registration
and withdrawal. The fixture reported 13 next-hook calls and restored its
owned desktop; result and selected-object hashes are in `idle-chain/`. The
fixture uses test-bound task callbacks, not arbitrary third-party hooks or a
complete guest teardown proof.

Selected-object process-retirement and initialization-rollback fixtures
passed on the current provider: task/object/class refusal and retry, cleanup,
execution scopes and native ownership, as well as two injected allocation
failures and three idle-observer installation failures. These fixtures include
test injection seams; they are not live guest failure injection or proof of
all concurrent teardown. Logs and hashes are in `process-retirement/` and
`init-rollback/`.

The current candidate was independently checked for the SOL and Write portions
of the WOW startup frontier. With the product gate's short PATH and short
history environment, both had their expected visible main window and a live
worker in all four samples; all ten staged image hashes matched the candidate.
`frontiers/` retains both observation reports. The observers intentionally
terminate after their observation timeout, so this is startup/liveness evidence,
not natural exit or gameplay, and does not complete the three-app frontier.

An earlier attempt omitted those two gate environment settings and produced
illegal-instruction dialogs for both programs. It is retained as a non-pass
in `frontiers-uncontrolled-nonpass/`; the controlled rerun changed both PATH
and short-history settings, so it does not establish which difference caused
the failures or prove they are unrelated to product behavior. The earlier
observation runner returned zero because it reports observations rather than
asserting the full gate's expected window. Do not count that run as a pass.

Follow-up SOL controls changed one setting at a time: controlled PATH alone
produced the expected visible Solitaire window in all four samples, whereas
short history alone retained an illegal-instruction dialog and the launcher
reported exit 6. Both staged manifests match all ten candidate image hashes.
These observations associate the failure with the inherited PATH/environment
rather than short-history alone, but do not prove the exact cause: PATH changes
both lookup and inherited environment contents and the guest failure path is
not yet traced. The observer's zero process status is not a product pass; both
window and launcher reports are retained in `frontiers-path-only/` and
`frontiers-history-only/`.

A third SOL control used the same minimal leading PATH as the passing case,
plus 75 copies of a test-owned nonexistent path segment (no user PATH data).
It reproduced an illegal-instruction dialog; all ten image hashes matched.
This establishes that the original machine-specific PATH entries are not
necessary for this controlled failure, but not a threshold, full root cause,
or equivalence to the accepted COMMAND.COM EnvSiz defect. The historical
COMMAND defect is documented separately in TODO and its workaround is not
authorized. Original command environment construction and the initial WOW
projection are reachable during setup; the failing guest instruction address
and corrupted owner have not yet been traced. Do not relabel this new WOW
observation as that guest defect without the direct evidence used there.

The later direct guest-state witness ties the controlled long-environment WOW
startup to the same COMMAND.COM `EnvSiz` overwrite documented in TODO; see
[witness](wow-env-attribution/review.md). This supersedes the earlier
unattributed hypothesis only for this reproduced scenario.

The Alt+Enter input-batch fixture was freshly compiled from the current
console adapter and passed mixed-order, key-down/up, left/right Alt, peek,
queue pressure and mouse controls. The cached x86 worker input hash matches
this candidate; the fixture uses controlled queue stubs rather than a physical
keyboard. Output and hashes are in `alt-enter-batch/`.

The cached current frontend private-desktop controller test also exited 0.
It checks synthetic Alt-down, Enter-down/up and Alt-up ordering, continued
Window visibility, and Ctrl+Alt+F return to Console. Current NTCON input hash
matches this candidate. Output and hashes are in `alt-enter-window/`; this is
not a physical-keyboard or host-terminal compatibility test.

The current selected-object window-reclaim fixture passed, including deferred
window and menu reclamation, failed-first/successful-later collection, handle
slot and address reuse, nested borrows and final empty class storage. Its
retained output and hashes are in `window-reclaim/`. This uses controlled
fixture failures and does not prove every real guest or concurrent teardown.

The selected current-provider native-nested fixture passed the execution
owner/last-error recovery, nested callbacks, native wait unwind, exception,
window destruction and receiver retirement checks. Results and input hashes
are in `native-nested/`; synthetic error 1400 there does not identify the
separately observed intermittent SOL failure.

The selected current-provider menu-resource fixture passed original menu
resource/parser cases, owner-draw and thunks, tracking/reentry, rollback,
native return and retirement. Logs and hashes are retained in `menu-resource/`.
This is controlled CCPU/provider coverage, not a real guest failure trace or
proof of the separate intermittent 1400.

The selected current WOW32 bridge passed its direct BOOLEAN-return fixture and
8 native Windows loader attach/unload cycles with a controlled provider.
Its rebuilt input hash matches this candidate; see `dll-bridge/`. These tests
are not actual WOW32 heap-failure injection or guest DLL lifecycle coverage.

The current sealed candidate also passed three SOL Options open/close/parent
recovery interactions and normal launcher completion; all ten actual image
hashes match. See `sol-correctness/`. This limited functional regression does
not establish a fix for the intermittent native-return 1400 or measure
performance acceptance; performance optimization remains queued separately.

After these checks, all ten sealed candidate inputs were compared to the
current cached x86/x64 binaries and matched. Paths and hashes are in
`current-cache-hashes.json`. This is an identity/freshness check, not Full
product acceptance or proof of worktree-wide build completeness.

The owner explicitly authorized restarting Winmine and changing its profile.
The Full suite subsequently passed all 16 recorded gates, including the
three WOW startup frontiers, Console-17, Window-17 and Control cases, with
WindowOwnedConsole enabled. All ten runtime hashes match this sealed candidate;
the gate timings, manifest and startup snapshots are in `full-user-authorized/`.
This is the suite's startup/liveness witness for Winmine, SOL and Write, not
gameplay or proof of the separate intermittent SOL 1400 root cause.

An independent Winmine gameplay probe subsequently passed flag/counter,
reset, opening a cell, window close and launcher completion on an isolated
stage of this candidate. Its [receipt](winmine-gameplay/result.txt),
[observations](winmine-gameplay/probe.txt) and input hashes are retained.
The runner records guest, NTVDM, WOW32 and Run16 hashes, not all ten runtime
hashes, so do not present that separate record as a full ten-image manifest.

After publication, a separate three-round WINMINE Custom-dialog probe passed
each dialog close and parent recovery, followed by normal launcher completion.
Its ten-image manifest matched `assets/release/release-manifest.json` exactly;
the [result](winmine-dialog/result.txt), [probe](winmine-dialog/probe.txt),
and [manifest](winmine-dialog/manifest.json) are retained. The probe recorded
the main window minimized, so this verifies its message-driven dialog path,
not on-screen placement or a forced concurrent retirement. Three passes do
not identify or disprove the intermittent 1400 failure.

A same-worker Winmine / prebuilt CDLPROBE run first failed its probe's 10-second
wait: the sibling was still running after 11 recorded successful dialog closes.
That is a non-pass, not a captured 1400 or proof of deadlock. A second run
changed only the test probe's sibling wait to 30 seconds and its observer
watchdog to 60 seconds; no product or guest was modified. It completed all 16
explicit dialog closes and sibling retirement, then Winmine flag, reset, open,
close and launcher exit. The guest binary SHA-256 was
`406D84F9E4EA04CD269045B354E95C47A342C986E0400A2C4F6A58DC818D3A02`.
The [initial non-pass](winmine-sibling/initial-timeout-probe.txt),
[successful result](winmine-sibling/sibling-result.txt),
[probe](winmine-sibling/probe.txt), and guest checkpoint are retained there.
This is a timed, instrumented run with four product input hashes recorded;
it does not prove the original 10-second contract, forced cross-task race,
the independent SOL 1400 attribution, or performance acceptance.

The tested ten-component package was subsequently published to both
`assets/release` and `O:/winnt/system32`, with all 20 destination hashes
verified against the Full receipt. The previous two sets were saved under
`build/t473-s11/s11-current-publication`; publication receipt is
`build/t473-s11/s11-current-publication/result.json`. A post-publication
attempt to run the same gameplay runner with `O:/winnt` as PackageRoot was
rejected by its build-owned staging precondition before launching a guest.
It is not a product failure or a passing post-publication gameplay test.

The preceding launch with a stripped environment did not pass: all three WOW
frontiers displayed a `config.nt` error, and the gate rejected Winmine. The
successful run restored the inherited environment; several variables changed,
so this comparison does not identify an individual cause. An earlier runner
attempt failed before staging because its stripped PATH hid Python. Neither
non-pass is part of the successful Full receipt.
