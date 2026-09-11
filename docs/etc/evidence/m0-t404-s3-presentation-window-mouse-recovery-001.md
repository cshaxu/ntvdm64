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
button-state transitions. It intentionally does not inspect `GetCapture()`:
the fixture invokes the window procedure from a different thread, whereas
Win32 capture is owned and observable on the presentation window's UI thread.

The formal x86 composition linked
`build/M0-T404/S3/r001-mouse-x86/original-softpc-process.exe`. The staged
candidate is `O:\ntvdm64\ntvdm32.exe` with SHA-256
`96196413AD7C0A4B6050A02EA1D7DBEEE60E58C849006E6EC58A5C568082A4ED`.

## Remaining acceptance

The owner must run text-mode `EDIT.COM` in the presentation window and verify
movement, click/release, and a drag outside then back into the client area.
Until that result is recorded, S3 stays active. The proposal left Queue at
admission because Queue contains unadmitted candidates only; this evidence is
not a claim of end-to-end mouse acceptance.
