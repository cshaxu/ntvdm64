# M0 T443 S4 — NTVWM native text sampling contract

## Finding

`f30ee19b2` (T425 S7) correctly kept the last **observed** native hidden
Console state inside NTVWM.  A 30 ms poll with identical cells, viewport,
cursor and packed text state returned without publishing.  That left the
visible host Console's own caret timer alone.

`a47979990` moved NTVWM onto the shared asynchronous publisher.  The move
correctly centralized transport copying and 50 Hz coalescing, but removed the
source-owned observed-state comparison.  The current 20 ms poll consequently
offered a complete transaction every time.  `publish_copy()` faithfully sent
`CURSOR_INFO` and the text frame even where every cell write was skipped;
NTCON faithfully called `SetConsoleCursorInfo` for each request.  Repeatedly
resetting that host state prevents the normal Windows Terminal cursor blink,
which is the reported “cursor anxiety”.

This is a producer classification regression, not a Terminal special case and
not a receiver/transport deduplication problem.

## Repair

`src/ntvwm-exe/presentation.c` now retains the last observed immutable native
snapshot at the NTVWM producer.  It compares the complete Unicode grid and
packed text payload plus viewport, cursor position/shape/visibility, default
attributes, title and frame description.  Equal polls are discarded before
`worker_base_publication_offer()`.  A successful offer transfers ownership of
the observation; a failure leaves it eligible for retry.  A newly attached
frontend clears that observation so it receives a complete initial frame.

`published_cells` remains separate and unchanged: it is the last acknowledged
row image used only to avoid redundant cell writes inside a real transaction.
No filtering was added to NTCON or worker-base.  Direct
`ntvwm_presentation_text()` calls remain unconditional.

## Focused verification

The owned background runner was
`build/M0-T443/S4/r001/run-focused.cmd`; its final run started
2026-10-10T17:27:25Z and exited 0.  It rebuilt the AMD64 NTVWM closure and the
x86 named-pipe observer, then ran:

```text
ntvwm-presentation-test.exe <log> --capture-only
```

Result:

```text
NTVWM-PRESENTATION checks=63 failures=0 named-pipe=yes production-activation=no
```

The focused mode uses a private hidden Console when launched from ConPTY, so
the real-buffer geometry test does not inherit a terminal without an HWND.
It proves the unchanged-capture, cursor-only, Unicode-grid and explicit-frame
contracts without serially running unrelated input/geometry cases.  The
observer's former `FILE *log` name also collided with the MVDM `math.h`
`log()` declaration; it is now `log_file` so this pre-existing test can build.

## Candidate publication

The 2026-10-10 candidate package was staged from the tracked
`assets/winnt.zip` media baseline—not from the mutable `O:\winnt` runtime—then
overlaid with the ten explicitly selected product binaries. The prior runtime
ten-file set is recoverably retained under
`build/M0-T443/S4/r001/prepublish-system32`. After stopping only project
images rooted at `O:\winnt\system32`, the package was copied to both
`assets/release` and `O:\winnt\system32`. The publish runner verified every
manifest SHA-256 at both destinations:

```text
PASS published and verified all ten system32 product components
```

## Owner acceptance

The owner accepted S4 after exercising the published Windows Terminal native
text path on 2026-10-10. The idle caret again blinks naturally; this is the
required owner-visible witness that the producer no longer continually resets
the host cursor. Focused coverage remains responsible for unchanged-capture,
cursor-only, Unicode-grid and explicit-frame distinctions.
