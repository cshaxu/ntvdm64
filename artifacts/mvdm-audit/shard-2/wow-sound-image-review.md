# Original WOW SOUND dispatch and beep

This shard-2 checkpoint covers pending `wow32/wsman.c` and `wow32/wstbl2.h`.
The [observer](wow-sound-image-observer.py) and [receipt](wow-sound-image-observations.json)
check both original editions, the DOS16 and WOW32 source identities, selected
WOW32 compiler dependencies, and current released image. Both originals retain
100% of their source bytes. No guest or runtime behavior percentage can be
deduced from those bytes or static linking.

The original guest `sound.def` exports DOBEEP at ordinal 17 and `sound.asm`
emits its SOUND thunk; this is an original source chain, not evidence that a
specific installed guest driver was built from it or has been loaded. The
release image's `aw32WOW` SOUND segment starts at index 1482. All 19 actual
table entries are resolved against the selected original table object: 17 is
`WS32DoBeep`, the remaining 18 resolve to `WOW32UnimplementedAPI`. These are
initial link facts, not a decision to remove any supported guest sound feature.
Other SOUND calls and DOS audio remain separately in scope.

The original `WS32DoBeep` body matches the selected object outside its single
COFF relocation; that relocation is resolved to the release image's USER32
`MessageBeep` import. It has no owned callback, guest pointer or sound-handle
resource. The modern host owns actual audio playback. Observable result,
error propagation, task termination during the call and real Win16 guest
behavior remain untested. Source/table identity does not prove that audio is
audible or that a particular guest session reaches this entry.

Recommend both source roles **partial**: `wsman.c` supplies the selected thunk,
and `wstbl2.h` supplies the selected table slice. Source retention is 100%; a
whole behavioral restoration percentage is unmeasurable. No full acceptance
or exception for the other 18 SOUND slots is implied. The prior [beep review](wow-sound-beep-review.md)
keeps those unresolved boundaries.

No guest, product, installation or shared ledger was modified. The coordinator
should replay the observer before any classification merge.
