# T423 S35 native text Window carrier

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Result

NTCON no longer turns a valid copied text frame into an indexed graphics frame
because it exceeds 80 columns or 50 rows, uses a font taller than 16 scanlines,
has underlined cells, or has two cursor ranges. The project-local KVM text
carrier now accepts the already-defined cross-EXE protocol limit of 160×96
cells and 32 scanlines. Window renders glyphs and underline from cells and
fonts; its existing timed cursor overlay handles both cursor ranges. Actual
graphics frames remain graphics. No cross-EXE wire, worker, OpenNT image, or
guest media was changed.

The imported nxvm manifest records the hashes of the six changed project-local
library files. The byte-exact manifest verifier accepted all 44 entries.
The maximum text payload copied into the Window library is larger than S34
(up to 15,360 cells, two 256×32 font banks and 15,360 style bytes); it remains
bounded by the unchanged copied protocol. The former raster fallback and its
transient pixel scratch allocation were deleted.

## Verification

The x86 graph was regenerated under `build/M0-T423/S35/candidate2/`. The
ordinary Ninja runner stalled between child compilations in this environment;
the graph's expanded commands were executed in order by the build-local
`manual-build.ps1` without modifying generated command arguments other than
removing `/showIncludes`. `frontend-window-library-test.exe`,
`frontend-window-controller-test.exe` and `ntcon.exe` linked successfully.
The standalone x86 library verifier passed its import-hash check, frame
assertions and keyboard fixture. The controller executable links, but its
direct invocation is intentionally rejected outside the named private desktop;
this was not counted as a passing controller run.

| Check | Result |
| --- | --- |
| `frontend-window-library-test.exe` | Pass: 80×50, 120×30 and 160×96 text; 8–32 scanline fonts; bank/style/underline, two cursor geometries; malformed extents and graphics isolation. Native 120-column frame has `graphics=0`. |
| `verify-frontend-window-library.ps1 -BuildRoot build/M0-T423/S35/library-verify` | Pass: 44 imported file hashes, 15 x86 library units and keyboard tests. |
| Isolated Console product matrix on `Z:` short alias | 17/17; reports `build/M0-T423/S35/candidate2/s35-z-console17-*`. |
| Isolated private-desktop Window product matrix on the same package | 17/17; reports `build/M0-T423/S35/candidate2/s35-z-window17-*`. |
| Published `O:/winnt` observer smoke | `native-zero`, `command-c-mem`, `edit` all pass; reports `s35-published-smoke-*`. |
| Published eight-file coherence | Seven binaries retain S34 hashes; `ntcon.exe` matches staged SHA-256 `28352BACFA99A29BFE085DE07B6168C9927706020EEF4BDA6348C8495A3B1252`. |

The temporary `Z:` alias was removed after the 17+17 runs. The published
package was updated only after those runs; only `ntcon.exe` changed.

## Non-passes and acceptance boundary

The WOW headless-frontier script could not reach WINMINE on either the S35
candidate or the unchanged S34 baseline in the current environment. Attempts
using a long physical package path and a short physical package path both
reported early exit; the `Z:` variant also failed to locate the observer's
named desktop. These are recorded non-passes, not claimed WOW verification.
S35 does not change any WOW executable or source path, and the 17+17 product
matrix shows no DOS/native execution regression. The owner must still check
the actual CMD cursor blink/thickness and window presentation in the live
desktop; automated cell and cursor-rectangle tests do not measure visible
blink phase on that desktop. T423 remains open for owner acceptance.
