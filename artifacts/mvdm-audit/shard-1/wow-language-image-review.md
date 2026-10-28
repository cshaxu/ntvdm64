# Shard 1: original WOW language thunks

Proposed **partial** for `wow32/wulang.c`. The [observer](wow-language-image-observer.py)
and [receipt](wow-language-image-observations.json) verify the current source
against both original editions and both source catalogs, its selected object
and valid build dependency, and the released WOW32 image. Twelve original
thunk bodies match their compiled object outside relocation words. Twelve
initial USER-table pointers resolve to those bodies, and each actual linked
native import resolves to the selected Windows USER32 or KERNEL32 DLL.
Release and cached executable sections match; this does not assert whole-file
equality. The source retains 100% original bytes; full behavioral restoration
percentage is not measurable from these static observations.

The original thunks own guest argument mapping, pointer/character distinction,
16-bit return conversion and copyback. Modern Windows owns the actual ANSI
case, comparison, character classification and next/previous services. The
original `AnsiNext`/`AnsiPrev` thunks reconstruct a guest pointer from the
host-returned offset; `AnsiUpperBuff`/`AnsiLowerBuff` use the `SIZETO64K`
contract for a zero size and flush the mapped buffer after the host call.
These are source-level behavior claims; actual host locale and code-page
compatibility are not determined by image linkage.

Outstanding are actual guest dispatch and copyback, single-character versus
far-pointer forms, zero/maximum counts, invalid or cross-segment pointers,
DBCS lead/trail boundaries, code-page and locale changes, signed comparison
results, allocation/mapping failure, concurrent access and task teardown.
The observer does not claim any guest workload or error path ran. No guest,
product, installed file, build tree or shared ledger was modified; coordinator
review is required before applying this candidate disposition.
