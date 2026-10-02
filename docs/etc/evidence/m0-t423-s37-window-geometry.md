# T423 S37: stable Window geometry across DOS/native handoff

The S36 published eight-file package was the baseline. In an isolated
80x30 Windows Console, `run16 command`, Ctrl+Alt+F, then `ver` changed the
Window text frame from 80x28 to 80x30 and back. The measured client width
remained 640 pixels; the visible jump in this reproducer was height (392 to
420 pixels), not a measured change in columns. The baseline assertion exited
1 with `unexpected30=1` in
`build/M0-T423/S37/p1/baseline-assert.raw`.

## Cause and change

NTKVM's DOS page has a logical 80x28 viewport while its canonical visible
Console can retain the caller's physical 80x30 viewport. During an active
native-channel switch, `console_channel.c` cleared `logical_window`. NTCON's
initial screen-info request consequently seeded its hidden Console from the
physical 30-row viewport and published an unintended 30-row text frame. The
subsequent DOS frame restored 28 rows.

S37 keeps the already-bound logical viewport through native seeding. NTCON's
existing `CONSOLE_IO_WINDOW_RECT` publication still updates that viewport
when a native program genuinely changes it. No guest, original OpenNT/MVDM
mirror, worker-base protocol, shared KVM library, font rule or timed redraw
was changed. The only production edit is in
`src/ntkvm-exe/console_channel.c`.

## Reproduction and acceptance

The x86 `console_channel.c` object was compiled and `ntkvm.exe` linked using
the S36 generated product graph and its cached, unchanged objects. The
seven other staged product binaries remained hash-identical to S36. A fresh
S37 x86 graph was also generated; a cold compile was interrupted because the
normal Ninja runner yielded no progress and the serial runner would rebuild
all 588 commands. This was not counted as a successful cold full build.

The x86 private-desktop regression observer
(`--s37-window-geometry`) records the real Window client rectangle, text
frame columns/rows/font height and fails if an unrequested 80x30 frame occurs.
On the candidate:

- `ver` plus DOS -> CMD -> DOS: 769 sampled published-run frames,
  `unexpected30=0`, exit 0; the only reported geometry was
  `client=640x392 text=80x28 font=14`.
- Explicit native `MODE CON COLS=100 LINES=40`: the test observed
  `100x40` and then DOS `80x43`; `unexpected30=0`, exit 0. Thus the
  correction does not freeze genuine geometry requests.
- The unchanged product verifier passed 17/17 Console and 17/17 Window cases,
  including COMMAND, MEM, EDIT, nested MEM, native streams and guest exit code.
- The existing private Console frontend and Window controller fixtures both
  exited 0.

Reports are under `build/M0-T423/S37/p1/`:
`candidate-assert-roundtrip.raw`, `candidate-assert-resize.raw`,
`s37-console17-summary.json`, `s37-window17-summary.json` and
`published-assert-roundtrip.raw`. The published `O:/winnt/ntkvm.exe` hash is
`A93E5AE94D13AB8402ACEBB1926E385DD7AB2726DAB98FCD27A6EC88E41A83FC`,
matching the candidate. The other seven published product files match their
S36 staged hashes. The temporary `Z:` test alias was removed.

The private-desktop probe proves this sampled route and explicit resize; it
does not prove that every terminal emulator presents the same outer window
decoration or that every native application has identical resize timing.
T423 remains open for owner acceptance. The prior task-trace S37 is S38 and
was not admitted as part of this repair.
