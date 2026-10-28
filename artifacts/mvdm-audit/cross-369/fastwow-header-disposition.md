# S10 part 9: fast WOW profile interface and selected callback path

Owned keys `wow32/fastwow.h` and `wow32/i386/fastwow.asm` are assessed here. `fastwow-header-observer.py` compares their original/current hashes and the cached selected callback object's normalized VALID dependency; reproduce with `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/cross-369/fastwow-header-observer.py`. No product, guest, graph or shared ledger was modified.

Both files match both original editions. The header is a VALID dependency of selected `wcall16.obj` and participates in choosing fast versus non-fast callback behavior. Original `i386/sources` explicitly lists `fastwow.asm`; its NT4 fast implementation uses private monitor/TIB and native segmented context. The selected CPU40 graph omits that assembler. `shard-3/fastwow-comparative-audit.md` strengthens the branch conclusion beyond text selection: the selected callback object imports `host_simulate`, not the fast callback or monitor symbol, while original non-fast frame construction/restoration and the narrow CPU40 BOP endpoint remain. This is a real alternative implementation profile, not an empty original source.

Prior T447 guest callback observations and the bounded CPU/SAS endpoint fixture demonstrate selected branches only; the latter uses doubles, not a real guest. Neither proves all interrupt ordering, exception/reentry timing or original fast-path performance equivalence.

| Owned key | Recommended role | Open boundary |
| --- | --- | --- |
| `wow32/fastwow.h` | Partial: selected original configuration interface with bound current-object evidence. | S11/S14 validate profile, callback/return/frame ownership and exact selected binary. |
| `wow32/i386/fastwow.asm` | Pending original alternate implementation, not automatically excluded as nonfunctional. | Determine whether any observable fast-path ordering/behavior must be preserved; keep the approved CPU40 architecture and do not import NT4 monitor state without a separately authorized closure. |

Required tests cover guest dispatch/return registers, nested callbacks, interrupt ordering, cancellation/exception and reset against the actual selected images, with a separately identified original reference where feasible. Any new semantic relaxation must be explicit. No new test or runtime change is claimed.
