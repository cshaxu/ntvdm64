# S15 P45 N022 control drawing — delivered

Cached full build, runtime50 dual publication,40control cases, real CCPU
window-borrow lifecycle and SOL Options passed.
[Receipt](n022-draw-result.json), [census](n022-draw-census.json).
N02216/39 delivered; S15 9/14 and overall57/159 directories. No S/T closure.

## Inputs and decisions

Pinned XP revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/client/draw.c, blob
ddc66236d2ddf62fed88d3a429198389c5313396. Registered OpenNT blob
c9311e180849aa02c77ae9e84c544e7a628e75c9. Original XP has85lines.
Raw snapshots and comparisons: build/t485-s15/n022-draw.

Restore the exact XP Git-object bytes, removing the two redundant local #line
directives. XP PaintRect obtains the desktop identity once; a desktop brush
uses origin(0,0), while another brush window uses the difference between the
two client origins. Both paths restore the caller's brush origin after filling.
XP also uses HandleToUlong for a CTLCOLOR pseudo-brush. Existing validation,
native GDI and original GetControlColor bindings already support the complete
body. No source exclusions, new product adapters or guest changes are needed.

## Validation and statistics

Extend the existing control-brush fixture by linking the actual original
draw.c object. Use real memory DC/GDI drawing and a finite color-service double
that samples the brush origin. Cover implicit desktop origin, ordinary window
offset, same-window no-change, pixel filling, restoration and invalid HWNDs.
This reuses one fixture, not a second PaintRect implementation or test framework.

Raw byte equality with the pinned XP object is established; the logical-line
cross-check covers72nonblank original lines without mismatch. Nonerror
2points/2lines against OpenNT becomes0/0 against XP. There are no residual
adaptations or exclusions in this file.

Final full census:1329files,783byte-identical,794nonerror-identical,
535nonerror-different,7783points/116981lines. Before1329/782/793/536/7785/116983;
delta0/+1/+1/-1/-2/-2. Baselines1282XP/47OpenNT. Final source/package hashes were rechecked and the census updated. P42 runtime47 Full24/24 is the last
checkpoint; this is the third mirror since that checkpoint.

## Retained verification attempts

Initial fixture link failed because PaintRect needs the existing color_client
object in addition to brush_client. The test link now reuses both original
objects. The subsequent cohort passed39/40; its sole pixel assertion used
the memory DC's default bitmap. Selecting an explicit writable bitmap and
initializing it white made the original FillRect black-pixel assertion pass.
The bitmap, previous selection and DC are released in the fixture.
No production correction was needed. The retry retains the39 unchanged
fixture executable hashes and reruns only the brush case; the two unexecuted
real-package checks then run once. Original failed receipts remain recorded.
