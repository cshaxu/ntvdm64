# T423 S29 whole control-plane acceptance

S29 introduced no product mechanism, mirror diff, overlay or guest-media change.
It corrected two test assumptions: the lifecycle probe now distinguishes loss
of the independent NTKVM frontend from loss of its run16 launcher and writes
its evidence to `O:/winnt/Logs2`; the BaseSrv worker-channel fixture now
reports the authenticated same-Console relationship before selecting a
worker. The x86 fixture rebuilt and all six focused modes passed:
worker-channel, frontend-rundown, management-terminate,
launcher-exit-survival, launcher-disconnect-survival, and
completion-rundown-race. The NTCON execution-lifetime fixture passed 448
checks with zero failures; next-command, close and frontend-scope fixtures
also passed.

## Real process gates

Against the unchanged, x86-built S28 protocol-25 package:

| Gate | Result |
| --- | --- |
| Broker death | Launcher and worker failed without request replay; fresh MEM succeeded. |
| NTVDM death | Waiting launcher returned bounded 1067; broker survived; fresh MEM succeeded. |
| NTKVM frontend death | Associated DOS worker closed; bounded root outcome; fresh MEM succeeded. The old probe wrongly killed run16 and is superseded. |
| run16 launcher death | Admitted worker survived; fresh MEM succeeded. |
| NTCON management close, two independent sessions | Selected NTCON/CMD closed; other session still accepted input and returned 23. |
| Ordinary Console matrix | All 17 cases passed with captured text and expected exit codes, including direct/nested COMMAND, MEM, EDIT and native commands. |
| Window matrix | The same 17 cases passed with captured text and expected exit codes. |
| Supplemental handoff | native-interactive-return, nested-mem-typeahead, native-cmd-dos-repeat and frontend-chain-b passed. |
| WOW frontier | WINMINE reached its main window; SOL and WRITE reached their original out-of-memory dialogs. No full SOL/WRITE acceptance is claimed. |

Logs are under `O:/winnt/Logs2` with `m0-t423-s29-*` prefixes, including
`ordinary-full`, `window-full`, `wow-frontier`,
`native-independent-close`, and the four fault-injection prefixes.
Tests used only temporary `Z:` mappings, removed after every run. No product
process or mapping remained. The exact eight published files still hash-match
the S28 candidate listed in the [S28 ledger](m0-t423-s28-management-projection.md);
no redundant re-publication or guest/configuration edit was performed.

## Explicit non-passes and source contract

The supplemental `dos-native-dos` final-screen assertion still fails because
the final 80x25 screen no longer contains the earlier native CMD banner.
The same assertion fails on the unchanged S27 control; intermediate line
snapshots contain the banner and the later DOS task completes. The selected
`opennt_console_resize_grid` binding follows original OpenNT
`ResizeScreenBuffer`'s cursor-containing-row retention on shrink, not an
unbounded history-preserving scrollback. This is a recorded product-experience
limitation, **not** a test pass or a newly admitted shadow-screen change.
The S25 source audit and before/after logs remain in
[S25 evidence](m0-t423-s25-native-nesting-wait.md).

## Diff and ownership accounting

This S contributes **zero production source changes**, **zero mirror/overlay
changes**, and **zero new autonomous runtime lines**; the net code changes are
test-only. Across the restarted T423 baseline `5c78e59a0` through S28,
Git's raw text accounting reports 12 changed mirror-path files with 13,942
added and 13,700 removed lines, and 204 changed non-mirror product-path
files with 22,799 added and 485 removed lines. These raw mirror line counts
include line-ending/format noise and are **not** a count of semantic OpenNT
divergence or a claim that those lines were all new logic. No overlay-path
file was added in that interval. The substantive duplicate removals and
retained worker-local owners are itemized in [S27](m0-t423-s27-worker-control-audit.md)
and [S28](m0-t423-s28-management-projection.md): shared connection/death-watch
and ordered frontend client remain in worker-base; original NTVDM command,
DOS/WOW records and execution stay in place; NTCON's Windows Console process
mechanics remain local. The S26 Job-descendant projection stays rejected,
not silently reinstated.

No other session's NTKVM edit was present in this worktree at the S29 evidence
snapshot. Any later edit must receive its own source review, x86 build,
34-case regression and coherent publication before it can supersede this
published baseline. T423 remains open for owner acceptance.
