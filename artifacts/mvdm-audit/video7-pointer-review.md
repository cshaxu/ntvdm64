# T473 S10 P: Video7 pointer candidate

## Pending mouse-range correction

An independent Windows 3.1 investigation observed 640×480 mode `0x67`,
BIOS mode byte `0x1b`, stale mouse height 350 and an effective bottom near
row 272. Its candidate calls the existing mouse mode owner after an extended
mode change and decodes the biased BIOS value in the project-owned relative
mouse bridge only when the video owner agrees. The isolated candidate reported
height 480 and mouse Y 479; this is frontend-injected input, not physical Raw
Input or complete Setup acceptance.

The new mirror exception is `VIDEO7-MOUSE-MODE`, registered in
`src/mvdm/README.md`. Relative to primary OpenNT, `v7_video.c` now has
12 added and 3 removed lines across both registered Video7 differences;
the additional current candidate contributes 8 added lines. A raw line-match
heuristic retains 521/524 original lines (99.43%); this is not behavioral
coverage. The project-owned bridge and test each have separate additions.
The separate investigation subsequently replaced the installed and release
`ntvdm.exe` with this candidate and updated the ten-image manifest. On review,
both directories match that manifest, but only NTVDM was replaced and the
full product suite was not rerun before that replacement. The complete
ten-component build and regression are still required; the earlier full-suite
result must not be attributed to this later executable. The owner manually
accepted the fix on 2026-10-28 and authorized committing before the full
regression. No guest was modified.

The original Video7 register code selects the cursor mask and position, but
the linked host callbacks were the stub `dummy`. The user's VGA/Video7
comparison and the original NTVDM link map identify the missing host
compositor; this is not evidence that the guest mouse driver is absent.

The candidate adapts `src/app-softpc/compat/v7_pointer.c` from the sibling
SoftPC checkout (SHA-256
`1C0F8893DAC407575D00D40191A50CEBF2E267E1C75539FFB59CCF8E78B1F5B2`).
The source file was clean at SoftPC commit
`844c7e17964a93d731b7be4c2829ed4464f25efa` when rechecked.
The worker owns the indexed surface, backing pixels and the bitmap lock.
Original V7 register and guest behavior remain in place. The source mirror
changes are marked `VIDEO7-POINTER-ENABLE` (operator precedence) and
`VIDEO7-POINTER-UPDATE` (host painting boundary); no guest image was edited.
Their current mirror diff is respectively +3/-1 and +16/-1 lines against the
task baseline. The original VGA controller and its enabled/disabled register
state remain authoritative; binding the existing callbacks avoids replacing
the controller or patching the Windows mouse driver. Host surface teardown
invalidates the saved background and a new full body gates pointer drawing.
The [current source-retention overlay](video7-pointer/source-retention.csv)
supersedes the older uncommitted-candidate measurements: it estimates 514/516
(99.61%) and 1,414/1,424 (99.30%) original code lines respectively. Both roles
remain partial; source-line retention is not functional coverage.
The candidate computes pointer damage, while transport still copies complete
graphics frames. The existing KVM Window renderer compares decoded pixels and
derives dirty rectangles at the receiver; this is not SoftPC's sender-side
dirty-rectangle transport.

Focused tests pass for the AND/XOR mask, clipping, background restoration,
invalid state, bitmap locking, initial-surface readiness and batched source
publication. The repaired console-client test builds; its broken-pipe fixture
passes in a private console. These tests do not prove Setup or raw input.

The previous pointer interaction probe accepted a stale result file, so its
screenshots are not acceptance evidence. A fresh, separately captured standard
mode probe shows the pointer moving and opening the File menu. An independent
visible-desktop Setup probe reached the original graphical name page and
shows the pointer moving after frontend-sink injection; neither probe exercises
physical Raw Input. A fresh enhanced-mode witness on the final ten-component
candidate reached the guest, recorded mouse input and opened File via the same
test-owned sink (`build/t473-s10/v7-full-pointer-r60`). The four Standard/Enhanced
and DOSONLY combinations passed on the visible test desktop, including clean
exit. These probes stamp a fresh package identity on the candidate copies for
isolation, so their executable hashes differ from the release candidate. The
private-desktop standard probe previously timed out on exit; the unmodified
baseline failed identically in that environment. The same candidate's Setup
probe, also using a fresh package identity, reached the graphical name page
and showed a pointer moved by the sink ([before](video7-pointer/setup-before.png),
[after](video7-pointer/setup-after.png)). The enhanced-mode
[menu frame](video7-pointer/enhanced-menu.png) is preserved alongside it.
The original media and user installation remain untouched by these probes.

The candidate passed the [14-gate Full product suite](video7-pointer/full-timings.json)
and the [Video7 BIOS probe](video7-pointer/video7-bios.json) (32 modes). All ten components were
published together to `assets/release` and `O:/winnt/system32`, with both
locations checked against manifest SHA-256
`2883CB67868D8F220B8B4087ADABAE9A78F18DF454004165DD29C63EA37761C6`;
the [receipt](video7-pointer/publication.json) is retained here and prior
files are under `build/t473-s10/v7-publish-r64`.
A targeted synthetic
system-input attempt was refused: the input desktop was `Screen-saver`, while
the test window belonged to `Default`. Thus physical Raw Input remains
unverified; sink injection is not represented as proof of it. The Setup probe
still awaits completion, and its screenshot reads are explicitly non-atomic.

## Mouse-range repair full regression

After commit `16c9ca687`, the current published ten-image package was copied
to an isolated test package and all ten images were stamped with a fresh test
identity. The [Full suite receipt](video7-pointer/mouse-range-full-timings.json)
records all 14 gates passed, including WOW frontiers, Console-17, Window-17,
RPC, GUI, version rejection, return and nested-window handoff. This is a
post-repair test, distinct from the earlier candidate receipt above; it is not
a physical Raw Input or complete Video7 mode coverage claim. The user accepted
the mouse-range repair separately. No guest image was modified or rebuilt.
