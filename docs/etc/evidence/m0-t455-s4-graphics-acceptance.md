# M0 T455 S4 — GDI graphics acceptance

## Scope and immutable inputs

S4 exercised the selected original `WG32*` GDI/DC/DIB/palette route that S1--S3
had already traced.  It introduced no product or guest change.  The only source
change in this delivery is a test-probe type correction: the Win32 `LRESULT`
from `SendMessageTimeoutW` is explicitly converted to the probe's `BOOL`
observation value.  This removes an x64 `/WX` narrowing diagnostic; it does
not alter its timeout, message, target or pass/fail contract.

The tested package was `build/t454-s3/package-r3`.  Its `run16.exe`,
`ntsrv.exe`, `ntcon.exe`, `ntvdm.exe`, `ntvwm.exe`, `WOW32.DLL` and
`VDMREDIR.DLL` SHA-256 values each matched `assets/release/`, including
`WOW32.DLL` `203D17B5F3F1C35C1243688D11A47A2B363A77D87209285F8C80889C2B051D01`.
The runners used the configured modern `pwsh.exe`, a physical short stage, and
an owned unswitched private desktop; no computer-use or guest modification was
used.

## Results

| Consumer / route | Entry point and result | Disposition |
| --- | --- | --- |
| Native/guest DIB storage | S3's `original-external-memory-test.exe` remains the direct positive/negative selection, bidirectional write and release witness. | Pass; no new owner discovered. |
| WINMINE GDI content | `verify-wow-mine-gameplay.ps1` compiled its probe and ran `WINMINE.EXE` from an owned short physical stage. It recorded flag/counter change, F2 restoration, opened-cell change, `WM_CLOSE`, and `run16 --wait` exit 0. | Pass. |
| SOL GDI content | `verify-wow-sol-gameplay.ps1` compiled with the repaired probe and ran from an owned short physical stage. SOL created its window and completed `WM_CLOSE`/launcher exit 0. Its client remained 324x384 because the private desktop monitor was 320x607 and host maximum tracking restricted the requested 585x384 geometry. | Startup/cleanup pass; full Deal/stock interaction is **not verified**, rather than a graphics pass. This is the existing environment geometry boundary, not a proved W07 source defect. |
| WRITE text/graphics window frontier | `observe-wow-frontiers.ps1` plus `verify-wow-write-frontier.ps1` observed one stable `ntvdm.exe` from an owned short physical stage at four samples (4/8/12/16 s). Each had visible `MSWRITE_MENU`, `MSWRITE_DOC`, and `MSWRITE_PAGEINFO`. | Pass for startup/content frontier. Editing, save, and ordinary exit remain outside this S. |

All runners completed and removed their owned physical stage.  The initially
combined diagnostic runner was stopped before it started an observer and is
not used as evidence.

## Owner and lifecycle review

- The selected original DIB/palette owners remain `wdib.c` and `wgpal.c`; the
  bounded alias carrier remains the single host identity bridge from S1.
- No consumer exposed a second bitmap/alias/palette authority, stale-alias
  transition, selection leak, or release-order defect.  Therefore S4 makes no
  speculative product repair and does not republish unchanged runtime images.
- The private-desktop SOL maximum-tracking condition cannot be solved by a GDI
  provider change without changing the test environment or application geometry
  contract; it remains a non-pass for later broader usability acceptance.

## Closure

W07's reachable GDI/DC/DIB/palette consumers have either passed their claimed
content/lifecycle frontier or have the finite, observed SOL geometry boundary
above.  No unowned GDI work is transferred.  Font/text/metafile conversion is
explicitly the next W08 owner and is not implied by these window observations.
