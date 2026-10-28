# Original debugger event provider: selected code and historical guest witness

`src/mvdm/dbg/dbg.c` is byte-identical to both compared OpenNT editions,
SHA-256 `5A3A207EFF1570908E270C737D12FB40FE66097895063136D17FE6B971494B90`.
The current cached worker graph compiles that exact source as
`obj/mvdm-debugger/dbg.obj`; its map attributes `DBGDispatch`,
`GetNormalContext`, `ModuleLoad`, and `SendVDMEvent` to the original object.
The [observer](debugger-provider-observe.py) and
[receipt](debugger-provider-evidence.json) bind source, object, graph, map,
DEM caller and the historical evidence ledger by SHA-256. Re-run the observer
from the repository root; rebuilt inputs can legitimately change its output.

The source path `demmisc.c::SignalSegmentNotice → dbg.c::ModuleLoad →
SendVDMEvent` is reported as executed by a real worker in the historical
[T420 S43 debugger ledger](../../../docs/etc/evidence/m0-t420-s43-debugger-capability-ledger.md):
a debugger received and continued a `DBG_MODLOAD` event for a temporary MEM
guest. The later OEM witness used a CP437 `£` in the path and recorded the
expected module notice and zero exit. Earlier trouble with `é` was separately
traced in that ledger to guest filename normalization; it is not evidence
that all non-ASCII event paths fail. Those witnesses bind to their historical
package and specified input, not automatically to the present installed image.

The standalone debugger-event and module-event fixtures described in the
same ledger prove public Windows transport and payload behavior; they do not
themselves call the original `dbg.c` body. The historical real-worker test is
the relevant positive execution observation, with its stated scope.

Recommend **partial** for the selected original file: source retention is
100%, but the historical limited module-load event is not proof of all
debugger services. Current package identity, WOW event routes, other OEM
codepages, malformed guest frames, debugger disconnect, nested exceptions,
register ownership and cleanup remain open. No behavioral percentage is
inferred, and no guest, product, build or shared ledger changed in this review.
