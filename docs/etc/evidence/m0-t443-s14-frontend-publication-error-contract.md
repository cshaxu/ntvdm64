# M0 T443 S14 — Frontend Publication Error Contract

## Decision

NTVDM and NTVWM no longer convert a rejected frontend publication into a
successful paint.  `ERROR_NOT_READY` and `ERROR_BUSY` remain the actual result
of text/configuration frames, title frames, and the obsolete NTVWM ownership
probe.  The one retained NTVDM route-selection query is not a publication:
before NTSRV has bound the initial frontend route, `ERROR_NOT_READY` means
"defer this selection to the next software tick" and returns a false
text-demand result.

This is a project-owned repair only.  It changes no OpenNT/MVDM mirror source,
guest binary, worker-to-frontend protocol, broker state, or final-frame
barrier.

## Source audit

| Site | Old behavior | S14 disposition |
| --- | --- | --- |
| `ntvdm-exe/win32/console_text.c` text demand | Deferred an unbound initial route as no text demand. | Retained only for `ERROR_NOT_READY`: it does not publish or claim ownership, and its next tick repeats the query after the broker binding. Other errors propagate. |
| `ntvdm-exe/win32/console_text.c` text/configuration publication | Turned `ERROR_NOT_READY` into `TRUE`. | Propagate the failure. |
| `ntvwm-exe/presentation.c` title publication | Turned `ERROR_NOT_READY` or `ERROR_BUSY` into success and continued to the frame. | Propagate the failure. |
| `ntvwm-exe/main.c` presentation take | Sent a pre-begin barrier to an inactive route and normalized its rejection. | Removed: opening transport does not claim ownership; service-authorized begin does. |
| `ntvdm-exe/win32/console_graphics.c` palette-not-installed capture | Returned success while retaining source dirty state. | Retained: this is local source readiness, not a rejected frontend send. |
| Common client/NTCON ownership gate | Produce `ERROR_NOT_READY` for absent pipe/foreign owner. | Retained as the canonical error source. |

The existing ordering is the reason no replacement state machine is needed:
`worker_base_publication_active(FALSE)` drains a queued final frame before
`worker_base_io_close`; the NTVDM and NTVWM release paths perform that drain and
barrier before physical I/O close; NTSRV retires the route only after the worker
and frontend release acknowledgements.  A send rejected after that ordering is
therefore an ordering/lifecycle failure, not an invisible successful redraw.

## Focused evidence

All builds and tests used task-local `build/M0-T443/S14` roots and independent
background processes.

| Check | Result |
| --- | --- |
| x86 `ntvdm.exe` and `console-text-producer-test.exe` build | pass |
| `console-text-producer-test.exe` | pass: rejected text/configuration frames return `ERROR_NOT_READY`; an initial unready route query defers with no text demand |
| x86 `ntvwm-presentation-test.exe` isolated `--title-not-ready` mode | pass: 49 checks, 0 failures; rejected title reaches `ntvwm_presentation_end` as `ERROR_NOT_READY` |
| AMD64 `ntvwm.exe` native closure build | pass |
| AMD64 `common-worker-control-test.exe` | pass: 220 checks, 0 failures, handle delta 0 |

The historical all-modes NTVWM presentation fixture still has an unrelated
mode-0 call-count mismatch (observed 4 where its pre-existing assertion expects
5).  S14 does not weaken that assertion; the new negative condition has its own
single-mode entry point so it remains a discriminating witness.

The AMD64 `ntvwm-execution-lifetime-test.exe` native graph also retains an
unrelated pre-existing compile blocker: its file-scope `log` identifier collides
with the CRT math declaration under the AMD64 closure.  The production NTVWM
image itself links successfully; this test blocker is not counted as S14
evidence or repaired by this packet.

## Package and boundary

A coherent ten-image package was assembled from the immutable staged baseline,
the S14 x86 build, and the final AMD64 worker build.  Each staged image was
checked for its intended PE machine and SHA-256, then copied individually to
the release directory and the configured product `System32`, with a second
hash comparison at both destinations.  The release manifest records the final
identities.

The task-local product observer remains unsuitable as a runtime acceptance
witness.  Both the S14 candidate package and a control package made from the
previous release images exited each optional WOW observer with `0x0000042B`
before any window/startup snapshot was produced.  The identical result shows a
staging/harness condition rather than an S14 regression; it does not replace a
real Console/Window acceptance check.  S14's direct producer and control
fixtures above are its release evidence.
