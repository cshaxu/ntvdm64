# Shard 3: WOW caret thunk and USER dispatch

Propose **partial** for `wow32/wucaret.c` and `wow32/wutbl2.h`. The
[observer](wow-caret-image-observer.py) and
[observation](wow-caret-image-observations.json) verify both original
editions, both source catalogs and actual selected WOW32 objects/dependencies.
All eight mapped caret thunk bodies match released WOW32 machine code outside
COFF relocation words. Eight initial `aw32WOW` pointers at original USER
ordinals 163–169 and 183 resolve to those actual thunks; the `wutbl2.h`
disposition covers this selected fragment, not its entire USER table.
The [durable verifier](wow-caret-durable-check.py) rechecks current image
identities, bodies, slots and call targets without the prior temporary package.

Five actual x86 call targets from Create/Destroy/SetPos/Hide/Show caret thunks
resolve to the project `wow_native_*` USER lifecycle adapter. The selected
GetCaretPos thunk instead has the host GetCaretPos import. Windows owns the
real caret, the original WOW thunk owns 16/32 argument conversion, and the
adapter owns identity and boundary handling. These static relationships do not
alone demonstrate focus, active window or task lifetime behavior.

Earlier [real guest probes](wow-caret-dispatch-audit.md) exercised selected
positions and bitmap calls against a previous package. This observer confirms
that package's WOW32 executable sections match the current release, but does
not reattribute its full package, state, desktop or results to the current
release. Blink behavior, visible shape, all bitmap/handle failure cases,
cross-task focus and caret teardown remain open. Both source files retain
100% original bytes; complete behavioral recovery percentage is unmeasurable.
No guest or product files changed.
