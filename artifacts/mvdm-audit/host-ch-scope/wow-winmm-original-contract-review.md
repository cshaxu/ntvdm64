# Original WOW handle resolver and WINMM provider contract

The original `wow32/isvwow.h` describes `WOW32ResolveHandle` as an exported
extension interface for multimedia thunk DLLs. Its six parameters identify a
handle type, mapping direction, 16-bit input/output and 32-bit input/output;
its declared result is `BOOL`. The current header has the separately
registered `MVDM-HOST-DIV-342` ABI correction, so it must not be described as
byte-identical to the original header. Current and both original versions of
`wow32/wmmedia.c` are identical. The cached WOW32 PE exports both
`WOW32ResolveHandle` and `WOW32DriverCallback`.

For source research only, original OpenNT 4.5
`windows/media/winmm/mmwow32.c::WOW32ResolveMultiMediaHandle` has the same
six-parameter `BOOL APIENTRY` signature and supports wave-in/out and
MIDI-in/out types. In 16→32 direction it searches the original native handle
list for the given 16-bit handle, writes the resulting native handle to the
32-bit output for a nonzero input, and returns TRUE only when a mapping is
found. A zero 16-bit input leaves that output untouched and returns FALSE.
In 32→16 direction it obtains `GetWOWHandle`, writes the 16-bit output and
returns TRUE only for a nonzero result. Its exception handler returns FALSE.
The original provider's own invalid-type branch can leave its local value
uninitialized; these observations do not establish a general invalid-input
contract or justify copying that implementation into this project.

The selected WOW wrapper calls this provider in multimedia arms but then
stores a separate local value which those arms do not initialize. It also
returns `MMSYSERR_NODRIVER` (6) from two `BOOL` functions if the dynamic
provider is missing. The [controlled selected-object fixture](wow-winmm-resolve-object-review.md)
observed both effects with explicit fake loader/provider seams; the original
WINMM source supplies positive evidence that the fake's valid-type
success/output and BOOL convention are source-shaped. It does not prove the
modern host implements the same private contract, that real callers can reach
the conditions, or what output a real provider leaves after failure. S16 must
establish those points before changing behavior, and register any retained
original-host exception.

The bounded case-insensitive source search covers C/H/C++/ASM/DEF files in
both compared MVDM trees and OpenNT 4.5 `private/windows`. It finds only the
MVDM declarations, implementation and exports for `WOW32ResolveHandle`, not
an internal original caller in those scanned roots. The interface is intended
for external extensions, so a zero source-search caller count cannot rule out
dynamic lookup, external binaries or guest-driven use. No real guest caller
was demonstrated. The [observer](wow-winmm-original-contract-observe.py) and
[receipt](wow-winmm-original-contract-evidence.json) bind the searched roots,
exact hits, source hashes and cached PE export evidence. The external WINMM
source is read-only research material, not a newly imported provider.

Separately, original WINMM's `WOW32DriverCallback` writes a queued argument
at the old send index before publishing the next index. The corresponding
original guest receiver assembly reads the old receive index before advancing
it. This supports those two original source algorithms being aligned; the
nearby guest pseudocode comment alone does not describe that indexing. It does
not prove the current immutable guest or modern WINMM ABI, callback delivery
or task-rundown safety. No product, guest, build or shared ledger changed.
