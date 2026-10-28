# Shard 2: original WOW USER menu and COMM dispatch

This independently extends the existing [menu](wow-menu-owner-lifecycle-review.md)
and [COMM](wow-comm-port-lifecycle-review.md) reviews. The
[observer](parallel-review-fourth-observer.py) verifies current source bytes
against both original editions, selected x86 graph and dependencies, and the
currently released WOW32 code image. Its [receipt](parallel-review-fourth-evidence.json)
records hashes, each resolved target and the bounded native call edges.

The non-profile `W32` entry is a single pointer. The original kernel table has
310 entries and one separator, so USER starts at `aw32WOW[311]`. Every one of
the 37 selected USER ordinals below is matched from original `wutbl2.h` to the
corresponding object relocation, release image pointer and relocation-masked
body in its original selected object. This establishes initial linkage, not
actual guest use, dynamic task routing or behavior of all branches.

| File | Candidate | USER ordinals | Remaining scope |
| --- | --- | --- | --- |
| `wow32/wumenu.c` | partial | 150–154, 156, 158, 160–162, 220, 410–418 | Original menu load, conversion and public USER bridge; guest-owned data, native HMENU identity, owner-draw callbacks, nested tracking, remove versus delete, errors, reentry and destruction remain bounded obligations. |
| `wow32/wucomm.c` | partial | 200–215, 245 | Original Win16 USER COMM and its PortTab. Actual peer, queue/overlapped IO, notifications, multi-port task cleanup, cancellation, timeout/worker state and error/handle ownership remain open. |

Both source files retain 100% bytes from both original editions. This static
evidence does not justify a whole-behavior restored percentage, and neither
file qualifies as `full`.

For menus, the image's `WU32LoadMenu` has one and `WU32LoadMenuIndirect` two
relocation-verified calls to original `ConvertMenu16` in `wres16.obj`. This
establishes those linked conversion edges, not validity of every template or
the owner-draw and teardown contracts. Earlier S8/S9 witnesses are bounded as
described in the menu review; they are not all-ordinal proof.

For COMM, the image's `WowOriginalFreeTask` has a relocation-verified call to
original `FreeCommSupportResources` in `wucomm.obj`; the latter's emitted
body also matches its selected object outside relocation words. The compiler
inlines port deletion within that body, so this audit does not invent a
separate direct call relocation. The original source scans PortTab, deletes
the first entry for the task thread and breaks; multi-port cleanup remains an
original behavior limitation to review, not an unverified claim of a new
regression. Source inspection shows the 1.5-second and 5-second waits and
subsequent resource cleanup; an actual still-running waiter or use-after-free
has not been demonstrated. The DOS `nt_com.c` fixture is not a WOW USER COMM
test. Do not weaken error handling or remove inherited waits without proving
the contract.

No real serial endpoint or guest test was run; no shared ledger, product,
guest or installation changed. Main review must replay the observer and
review each remaining boundary before merging these candidates.
