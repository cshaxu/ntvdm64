# T478 S43 GDI aliases and system-color pseudo-brushes

Status: S43 locally delivered. Focused red/green, coherent build, real guest
class/background/gameplay and Full16/16 pass; eleven images are hash-published
to this worktree assets/release. No O:/winnt write or push.

## Original contract and demonstrated local defect

The inherited T473 WINMINE handoff separates background and window width.
S42 verifies original startup VDD wiring and then reproduces a distinct GDI
alias boundary failure; those are not one provider issue. Its stable-ID audit
reconciliation remains deferred, not completed, while this concrete repair runs.

Original walias.h maps a native GDI handle to its shifted global-table index;
original GETWNDCLASS16 treats values through COLOR_ENDCOLORS as system-color
pseudo-brushes. The selected compatibility declaration maps that bound to
COLOR_INFOBK(24). Modern Windows does not provide the original usable
ENTRYWOW/GdiQueryTable ABI, as the existing DIV-341 registration records.
The current worker-local replacement began an empty table at index1, issuing
alias4 for a real brush. Original class conversion then preserved value4 as a
pseudo-color rather than decoding the brush. This is a local replacement
contract defect, not a missing original drawing algorithm.

Read-only original NT hmgrapi.cxx starts its *global* allocator at index1 too;
we do not invent an original hard-coded reservation. The important difference
is system-global allocation versus a fresh per-worker alias table. The local
carrier must preserve the original consumer's distinction, not copy a global
allocator's initial index without its operating-system context. No original
kernel handle manager is imported or required.

## Bounded change

Only the existing worker alias allocator's first slot changes to
(COLOR_INFOBK >> 2)+1. The table remains0x4000 entries and real aliases remain
index<<2, stable for a live native object, reusable after retirement, with
native GDI retaining type/resource validation. Zero and stale-handle semantics
are unchanged. Six nonzero slots (aliases4..24) are no longer allocated as real
objects; capacity is16377 rather than16383. Existing alias exhaustion checks
cover that finite representation boundary. No new table, drawing owner, type
policy, retry, generation scheme or per-application case is added. Original
WOW mirror code and pseudo-color conversion remain unchanged.

## Baseline and focused evidence

The real native-gray-brush counterexample against the pre-change allocator
returns alias4, decoded pseudo-brush4 and exit1. After the change it returns
alias28, the exact native brush and exit0. Existing stable/reuse/retired-invalid/
full-table tests pass with the reserved consumer range. Inputs/results are in
build/t478-s42 and build/t478-s43/focused.txt.

The S41 baseline game runs through restore, flag/counter change, F2 reset,
left-click change and WM_CLOSE/normal exit. These actions do not prove correct
background or geometry: the [baseline client capture](../../../artifacts/mvdm-audit/t478-final/brush/baseline-client.png)
shows the black right margin in a221x181 client. Current class brush is8; saved
normal outer placement is237x240. A preliminary run without the formal short
PATH reached an illegal-instruction dialog and is retained as failed evidence;
the matching formal environment supplies the baseline used here.

The read-only window observer now records WINDOWPLACEMENT as well as current
rectangles, explicitly distinguishing minimized237x39 from saved normal237x240.
The existing gameplay probe additionally records restored rectangles/class brush;
no guest memory is read or altered. Test source changes are observation only.

## Retained separate obligation

This S repairs class-brush interpretation only. It does not declare the full
T473 WINMINE handoff complete: original requested width versus modern native
minimum-size behavior, normal sizing, difficulty/occlusion/minimize cycles and
related USER evidence remain separately tracked. Do not reduce window width or
crop captured pixels to make this repair appear to solve geometry.

## Current-package result

The repaired package passes the existing real gameplay sequence and normal
launcher exit. The class brush is native00900014 instead of pseudo-value8;
restored outer/client sizes remain237x240/221x181. The [repaired client capture](../../../artifacts/mvdm-audit/t478-final/brush/repaired-client.png)
shows restored gray background without cropping or forced repaint. All11041
pixels in the independently selected right margin x160..220,y0..180 change
from RGB(0,0,0) to RGB(192,192,192). Covered-cell hash3cc8733b and the operation
checks retain original flag/reset/open behavior. This supports the background
repair only; width is intentionally unchanged and remains outstanding.

## Delivery

The [S43 result receipt](../../../artifacts/mvdm-audit/t478-final/brush/s43-results.json)
binds source, original read-only inputs, focused results, before/after pixels,
real guest operations, Full16/16 in372460ms and eleven published image hashes.
Package 22e35db2-13bb-419d-ba68-7a169cc939e1 is published only to this worktree
assets/release; prior assets are backed up under build/t478-s43/release-before.
The Full runner verifies all11 images; existing DOS4GW external-fixture skip
is retained, not a new pass. Documentation governance and diff review accompany
the local commit. No autonomous drawing body or mirror C/H difference is added:
the production change is one allocation-bound constant and loop start plus comment.
S43 closes the brush-collision repair only; normal-size and remaining S42 ledger
obligations remain explicitly open under T478.
