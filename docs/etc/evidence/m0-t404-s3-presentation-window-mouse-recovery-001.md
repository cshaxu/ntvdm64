# M0 T404 S3 Presentation-Window Mouse Recovery

## Scope

S3 P1 binds focused Win32 text-window mouse messages to the already existing
single-process console input path. It does not modify an OpenNT or SoftPC
mouse provider, guest media, BOP behavior, graphics input, or broker design.

## Source boundary

The product change is confined to `src/app/presentation_window.[ch]` and its
focused fixture. The window converts `WM_MOUSEMOVE`, left-button and
right-button transitions into bounded `MOUSE_EVENT_RECORD`s on its existing
`CONIN$` handle. Current text dimensions provide the client-pixel-to-cell
conversion; coordinates clamp to the last valid cell; button state is held by
the window; normal Win32 capture remains active only while a button is held.

P4 follows the owner's first `EDIT.COM` presentation run, which produced no
mouse response. The standalone Alt+Enter adapter opens the presentation window
without taking the original NTVDM `host_disable_stream_io` path. That original
path is where OpenNT turns on `ENABLE_MOUSE_INPUT`; its initial
`SetupConsoleMode` intentionally leaves the bit clear while stream-I/O is
active. The presentation window therefore now saves its existing `CONIN$` mode,
temporarily adds only `ENABLE_MOUSE_INPUT`, and restores the exact prior mode
when it closes. This is a standalone window-to-Console handoff correction, not
a change to OpenNT mouse ownership or guest semantics.

The unchanged downstream path is the selected original OpenNT/SoftPC route:
`nt_event.c`, `nt_mouse.c`, then `base/keymouse/mouse_io.c`, ending at INT 33h.
The sibling `softpc` worktree commits `f127560` and `fe67bde` were examined as
behavioral references for handoff and capture only. No sibling source was
copied, and no new source, firmware, guest media, or Microsoft component was
introduced.

## Recovery ladder

1. **Original source reuse:** the original OpenNT/SoftPC mouse providers named
   above already compose in the selected x86 graph and remain the sole
   downstream implementation. There is no original presentation-window
   translation unit in this standalone product to import.
2. **Smallest same-shaped seam:** the existing `CONIN$` input handle is the
   current app's keyboard ingress. Supplying standard Win32
   `MOUSE_EVENT_RECORD`s on that same handle preserves its record ordering and
   failure behavior without an ABI, pointer, or guest-memory bridge.
3. **External-code intrusion:** not applicable; no external code changes or
   imports are needed.
4. **New mechanics:** only client-coordinate clamping, grid conversion and
   locally retained button state are authored at the standalone presentation
   boundary because no historical provider owns Win32 messages for this new
   window. The mechanism is intentionally narrow and remains replaceable by a
   later source-shaped host ingress if one becomes composable.

## Focused verification

On 2026-09-11, the x86 focused fixture was rebuilt and run from
`build/M0-T404/S3/r002-mouse-fixture` with the Visual Studio x86 environment.
It exited zero. The fixture proves mouse-move, left-button down, and
out-of-client left-button up records with grid conversion, edge clamping and
button-state transitions. Its window opens through
`SESSION_VIDEO_EVENT_DISPLAY_TOGGLE`, the text-mode Console Alt+Enter route;
it does not use the separate `graphicsResize` path. It intentionally does not
inspect `GetCapture()`:
the fixture invokes the window procedure from a different thread, whereas
Win32 capture is owned and observable on the presentation window's UI thread.

The P4 focused x86 fixture was rebuilt and run from
`build/M0-T404/S3/r004-mouse-mode-fixture`; it exited zero. In addition to the
existing record checks, it proves that the window enables
`ENABLE_MOUSE_INPUT` while open and restores the exact pre-window Console mode
after close.

The formal x86 composition linked
`build/M0-T404/S3/r001-mouse-x86/original-softpc-process.exe`. The staged
candidate is `O:\ntvdm64\ntvdm32.exe` with SHA-256
`b6a3d1c82e11dc3df061b396b1d0081bf6efee18d94d1e9312cb873ec823e0b8`.

## Automated `EDIT.COM` acceptance

P6 removes the manual-window dependency from this packet. The bounded
`console_startup_observer` now has an observer-only
`--observe-presentation-toggle` option: after the existing default-off
`MVDM-MOUSE stage=3` initialization marker, it writes the public Console
Alt+Enter gesture. The Console adapter consumes that gesture before guest
keyboard delivery and opens the existing text presentation window. A
no-argument GUI launcher exists solely so the standard Windows application
launcher can run this Console-owning test without an operator using a terminal.

On 2026-09-11, the launcher started deployed `O:\ntvdm64\ntvdm32.exe` with
`-f -o --command EDIT.COM`. The observer reported
`scripted-presentation-toggle=delivered` and
`scripted-presentation-toggle-ready=yes`, then kept the selected worker alive
for its bounded 30-second observation. The deployed executable SHA-256 was
`A1B7FBD2E0D02640E49113BA6386292ECBBEA02D9778F96C3B21F8BD1C7F413B`.

Windows UI automation targeted the sole resulting `ntvdm32.exe` presentation
window and sent a left click plus a left-button drag from client grid `(26,11)`
to `(54,21)`. The default-off report records successful `WriteConsoleInputW`
calls (`write=1 count=1 error=00000000`), then original host stages 1 and 2
for left down (`buttons=00000001`), drag
(`buttons=00000001 flags=00000001`), and release
(`buttons=00000000 flags=00000000`). Each observed record was followed by
the unchanged original `mouse_int1` stage 3. The reports are retained at
`O:\ntvdm64\logs\t404-mouse-auto-p6.txt` and
`O:\ntvdm64\logs\t404-mouse-auto-observer.txt`.

This proves the selected text presentation path delivers move, press, drag,
and release through `CONIN$` and the original SoftPC INT 33h entry without
manual input. It does not claim a visual cursor policy: that remains guest
application behavior. The proposal left Queue at admission because Queue
contains unadmitted candidates only.

## Bounded `EDIT.COM` startup observation

P3 staged the formal P1 x86 worker in the disposable
`build/M0-T404/S3/r003-edit-runtime` container and used the existing
Console-owning non-debug observer. The first run identified a container-only
failure: the generated command path was 57 characters and the original
COMMAND buffer rejected it. A temporary four-character junction shortened the
same container, then was removed immediately after observation.

At that short path, `EDIT.COM` returned after reporting `Can not find file
QBASIC.EXE`. This identifies `EDIT.COM` as the original launcher and
`QBASIC.EXE` as its required original companion. The unchanged package copy
of `QBASIC.EXE` was copied into the disposable container, with matching
SHA-256 `ECCA463F1820749A1408FC8EF3799D23FB232C188669CA9D5DB3769CE628746B`.
The repeated 10-second observation then remained alive until its own watchdog
ended only the child it had created; it neither displayed a failure dialog nor
loaded WOW32. The raw reports are retained under `O:\ntvdm64\logs\` as
`T404-S3-P2-edit-startup*.txt`.

This is a startup prerequisite result, not interactive mouse acceptance: the
observer owns a non-visible Console and does not request the text
Alt+Enter presentation window or synthesize mouse input. The deployable
`O:\ntvdm64` package already contains the same original `EDIT.COM` and
`QBASIC.EXE` media for the owner-visible test.

## Guest mouse-use audit

P5 confirms that the selected target actually uses the DOS mouse interface.
The deployed `EDIT.COM` is the 413-byte original QBASIC launcher and contains
the `QBASIC` token. The deployed `QBASIC.EXE` has 25 `CD 33h` instruction
occurrences; its SHA-256 is
`ECCA463F1820749A1408FC8EF3799D23FB232C188669CA9D5DB3769CE628746B`.
The guest source also retains the original NTVDM internal INT 33h driver in
`src/mvdm-guest/dos/v86/doskrnl/bios/spcmse_4.asm`. The source-tree QBASIC
binary differs in package size and hash, so it was used only as a structural
comparison, never deployed or substituted. Therefore a non-responsive EDIT
run cannot be dismissed as an editor that lacks mouse calls; the remaining
question is delivery through the selected host Console-to-INT-33h route.

## P delivery status

P1 (`510d74b76`) delivered the mouse binding and formal x86 staging. P2
(`c6aa2dd5a`) adds the focused text-mode display-transfer proof. P3 records
the bounded `EDIT.COM` startup prerequisite. P4 corrects the observed
stream-I/O Console-mode omission, then formally relinks and stages the x86
product. P5 proves the selected original guest actively calls INT 33h. None
replaces the remaining owner interactive `EDIT.COM` mouse acceptance.
