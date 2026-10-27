# S10 shard 2: SoftPC mouse ingress, callback and state cohort

The original/current hashes of `base/keymouse/mouse_io.c` and
`host/src/nt_mouse.c` were independently rediffed against the frozen OpenNT
source. Both remain selected changed sources, and their original INT33 state,
IRQ callback, coordinate, cursor and show/hide algorithms retain ownership.
No product source or original guest was changed by this review.

## Changes and source call chain

- DIV-058 aligns the host range-call pointer declarations to the source's
  `MOUSE_SCALAR` type in both files; it does not introduce a new range policy.
- In `host_os_mouse_pointer`, original ICA locking encloses relative route
  application (DIV-318), original absolute input fallback, current position
  publication and original callback-mask/button assembly. On a Window route,
  the bridge consumes worker-owned relative records and retains original
  counter, event mask and guest callback policies; no active Window route
  means original input selection remains. `mouse_io.c` enables original
  software cursor geometry, shape, show count, redraw/erase and saved-pixel
  paths for the CCPU Window route, and handles route edges at the CPU owner.
  Console host-pointer warp is intentionally not applied to Window route.
- `mouse_io.c::jump_to_user_subroutine` (DIV-222) enters registered INT33
  callback at the live offset for CPU40 as for CPU30, while other profiles
  retain their original BOP fudge. It preserves original pushed return frame
  and register setup; callback exit/restore remains part of the original
  IRQ/INT33 flow, not the bridge.
- `mouse_save_state` (DIV-357) now sums exactly the cookie and context bytes
  written before storing the final checksum byte. The existing restore
  validates cookie and sum; invalid state remains rejected. Older buffers
  with a zero stored checksum and nonzero actual sum remain invalid, as they
  were under the original restore rule.
- `nt_mouse.c` additionally calls
  `mvdm_softpc_wow_page_domain_publish_cursor` after final position and before
  original callback mask assembly, under the ICA lock. The return value is
  ignored in the caller. That is a distinct guest-visible page-domain
  projection and should be reviewed as such; source identity alone cannot
  establish all publication failure/teardown behavior.

## Evidence and limitations

T425 S8 and the S7 ledger describe selected Window route geometry, cursor
repaint/erase, route transitions, guest callbacks and Console interaction;
those are historical product-level witnesses, not all host modes or a fresh
S10 test. T448 S11/S12 records a real independent DOS INT33 save/move/restore
baseline (`SUM=26 SAVED=00`, failure) and candidate (`SUM=26 SAVED=26`,
`MOUSE-STATE-PASS`), including corrupt cookie/checksum negative cases. This
is evidence for those cases, not all binary state layouts, stale persisted
buffers, multiple sessions or callback/route retirement. T404 P12 records the
specific EDIT callback entry finding behind DIV-222, not every registered
callback type. Tests and source evidence must not be generalized to Win3.x
real-driver hardware interactions outside their observed inputs.

A bounded completion matrix should cover paired relative/absolute routes,
ENTER/LEAVE and cursor shape/background across reset/visibility changes,
IRQ/callback nesting and teardown, plus save/restore of multiple states with
invalid cookie/context/checksum and zero-sum control; separately verify the
page-domain cursor projection during owner death or inactive view without
inventing a failure semantic. Tie runtime evidence to the exact image and
applicable guest. Current source/fixtures are not newly executed here.

## Registration distinction

`nt_mouse.c:37` labels cursor projection DIV-274, while README DIV-274 names
an unrelated OEM GetFullPathName boundary; `mouse_io.c` DIV-222 and README
DIV-222 are also used by a separate DPMI mode handoff. They are identifier
collisions, not evidence that either live behavior should be removed. Reconcile
by owner and history before assigning unique metadata references. Both file
rows remain partial, without invented restored percentages.
