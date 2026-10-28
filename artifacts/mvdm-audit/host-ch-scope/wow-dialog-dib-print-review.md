# Selected WOW dialog, DIB and printer declarations and providers

Five pending source paths have byte-identical current and two compared
OpenNT identities: `wcommdlg.h`, `wdib.c`, `wdib.h`, `wgprnset.c` and
`wgprnset.h`. The [observer](wow-dialog-dib-print-observe.py) and
[receipt](wow-dialog-dib-print-evidence.json) record the hashes, x86 WOW32
dependency groups, calling-source hashes and cached map symbols. Reproduce
from the repository root; the recorded result is bound to that build snapshot.

* `wcommdlg.h` declares original common-dialog thunks. `wcmdgtbl.h` registers
  `WCD32GetOpenFileName`, `WCD32ChooseColor` and `WCD32PrintDlg`; the map
  attributes these to `wcommdlg.obj`. Actual callback and dialog lifecycle
  behavior is not proved by these static facts.
* `wdib.c` and `wdib.h` implement and declare original DIB allocation and
  retirement. `wgdi.c` calls `W32HandleDibDrv` and
  `W32CheckAndFreeDibInfo`; the cached map attributes them to `wdib.obj`.
  Descriptor mapping, callback failures, selector reuse and cleanup remain
  unresolved; see [the existing failure handoff](../cross-369/selected-host-reproduced-failures-handoff.md).
* `wgprnset.c` and `wgprnset.h` implement and declare original printer setup
  thunks. `wgtbl2.h` registers `WG32DeviceMode`, `WG32ExtDeviceMode` and
  `WG32DeviceCapabilities`, and the map attributes all three to
  `wgprnset.obj`. The [resolver failure review](../cross-369/spool-resolver-shared-failure-review.md)
  records untested failure and lifecycle concerns; no printer execution is
  claimed here.

Recommend **partial** for these five physical roles based on selected original
source/declarations and static call or dispatch chains. This is not a claim
that all entries are reachable in a real guest, that current deployed images
match the cached maps, or that callbacks, errors, cleanup, device interactions
and concurrency pass. Each original source-text identity is 100%; behavior
cannot be assigned a numerical completion percentage from these observations.
No guest, product, build or shared ledger was changed for this review.
