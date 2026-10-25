# M0 T456 S2 — glyph-outline rotation seam

## Scope and source decision

The selected production path remains the original `wgfont.c` / `wgtext.c`
family.  The one reached private dependency which cannot compose on a modern
host is `dcquery.c::GetGlyphOutlineWow`: its call to
`GetGlyphOutlineInternalA` passes `bIgnoreRotation=TRUE` for the Win3.1
caller.  The public `GetGlyphOutlineA` API has no equivalent flag.

The retained S2 seam is therefore deliberately finite.  When the current DC
font has nonzero `lfEscapement` or `lfOrientation`,
`GetGlyphOutlineWow` makes a one-shot public `LOGFONTA` copy with precisely
those two fields zeroed, selects it for the one glyph query, restores the
caller's font, and destroys the temporary font.  For a nonrotated font, an
unqueryable font, or a failed temporary-font selection it retains the direct
public call.  It adds no font cache, handle table, renderer, private GDI
import, guest change, or second lifetime authority.  Modern GDI remains the
sole owner of the DC, selected object, output buffer, metrics and error
contract.

This is the first usable recovery rung: original behavior is retained through
the smallest public host seam.  Direct reuse is blocked only by the private
GDI entry point and its unavailable flag; importing that engine or inventing
a renderer would expand the dependency closure without preserving a narrower
observable contract.

## Focused build and fixture evidence

`tests/adapter-mvdm-host-out/wow/wow_glyph_outline_rotation_fixture.c` links
the actual facade source as x86 `/MT`.  With a 90-degree Arial font it proves:

1. raw public rotated glyph output differs from the facade result;
2. the facade result equals public output from the equivalent zero-rotation
   font; and
3. the caller's rotated `HFONT` remains selected after the call.

`tests/observation/verify-wow-glyph-outline-rotation.ps1` is the checked-in
reproducible x86 build/run entry point.  Its recorded run
`build/t456-s2/glyph-fixture-r3` emitted
`WOW_GLYPH_OUTLINE_IGNORE_ROTATION_OK`; the input source SHA-256 was
`96502AE1183BE27BA5DF8CDBDD314B07832EFAD372470B783729B50C888C6101`.
The post-build executable run in `build/t456-s2/glyph-runtime-r1` emitted the
same marker and exited zero.

After correcting the checked-in PowerShell entry point's parameter-default
ordering, the final reproducible run at `build/t456-s2/glyph-fixture-final`
again emitted the marker and exited zero.

The formal x86 WOW32 graph then reported `ninja: no work to do` for the
already rebuilt `wow32.dll` in
`build/.ninja/msvc-x86-release/wow32`.  Its SHA-256 is
`B787071516DA18BA2A31CAA96F31CA0354ED2A7404681F59339A9F5171087BBA`.

## Candidate and publication identity

The first two staging attempts contained newer unapproved service/hook
outputs, so they were normalized before any acceptance use.  The resulting
candidate is exactly the existing released nine-image set plus the rebuilt
WOW32 provider.  Its authoritative list is
`build/t456-s2/package-r2/product-system32-manifest.json`; the before/after
and three-way checks are `three-way-prepublish.json` and
`publish-final-verify.json` in the same disposable root.

All ten candidate hashes now equal both `assets/release/` and
`O:\winnt\system32`.  WOW32 is the sole changed image; immutable guest media
and configuration were neither changed nor rebuilt.

## Published guest startup witness

The checked-in `console-startup-observer.exe` was launched from the required
desktop-capable modern PowerShell token against the installed, hash-verified
package with `run16 --wait WRITE.EXE` on its own private desktop.  The first
restricted-token attempt wrote `result=not-started` and is correctly retained
as runner-configuration evidence only.  The valid run is
`build/t456-s2/write-private-r2/observer.txt`:

- the observer entered its own Console/private-desktop container;
- its direct child was the installed `O:\winnt\system32\ntsrv.exe`;
- it observed the installed `O:\winnt\system32\ntvdm.exe` descendant live
  throughout the bounded interval; and
- it reached the expected observer timeout result, not an illegal instruction,
  startup failure, or process crash.

This is real immutable-guest startup/liveness evidence for the published
provider.  It deliberately is not a normal WRITE task-retirement result: the
existing observer's ten-second liveness contract terminates its owned
container at expiry.

## Runtime boundary and closure decision

The test runner now has a desktop-capable WMI-owned background process, so the
retained private-desktop integration entries were retried rather than skipped.
Both fail before the selected glyph/text behavior can be observed:

- `verify-wow-mine-gameplay.ps1` reached an owned physical stage and observer,
  but `WINMINE.EXE` exited `0xffffffff` before a window was enumerated; its
  probe therefore returned 4.
- `observe-wow-frontiers.ps1` for `WRITE.EXE` recorded `exit=0x0000042b`
  before its first four-second window sample.

The candidate's `WINMINE.EXE`, `WRITE.EXE` and `WOW32.DLL` hashes equal the
deployed files.  These are the same known private-desktop frontiers recorded
for unchanged packages in T423 S30, not evidence that the selected outline
seam fails.  They are retained as non-passes and are not converted into a
runtime success claim.

S2 closes because its finite production change is a public-GDI adaptation of
an original private call, exercised exactly with rotated and zero-rotation
fonts plus caller-state restoration.  The published `WRITE.EXE` private-
desktop startup/liveness witness additionally proves that the changed provider
loads with immutable guest text/font content.  Whole-application retirement
and metafile playback remain S3/S4 acceptance work; they are not an implicit
precondition for a two-field glyph-query adaptation.
