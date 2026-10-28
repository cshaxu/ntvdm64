# Selected debugger and WOW fax shared declarations

Both `inc/dbginfo.h` and `inc/wowfax.h` are byte-identical to both compared
OpenNT editions. The [observer](wow-debug-fax-declarations-observe.py) and
[receipt](wow-debug-fax-declarations-evidence.json) bind the source hashes,
current Ninja dependency groups, source call-chain files and cached map lines.
They are static build/ownership evidence, not a runtime test or proof of the
currently installed binary identity.

`dbginfo.h` defines `VDMINTERNALINFO` and `COM_HEADER`. The x86 worker's
`dbg.obj` records an exact dependency on it. Original `dbg/dbg.c` uses the
former in `GetNormalContext`, populates it with register and remote-debug
data, and calls that function from debug event paths such as `SegmentLoad`.
`nt_bop.c` dispatches debug BOPs to `DBGDispatch`; both `DBGDispatch` and
`GetNormalContext` are present in the cached worker map under `dbg.obj`.
This supports **partial** classification of the retained declaration and
selected provider chain, not full debug functionality. Debugger attachment,
event delivery, structure/ABI compatibility across processes, malformed or
stale addresses, exit and nested task lifetime remain unverified here.

`wowfax.h` is an exact recorded dependency of selected WOW32 `wkman.obj` and
`wow32fax.obj`. `WK32RegisterShellWindowHandle` calls `WOW32FaxHandler`; the
handler's source uses `WOWFAXINFO`, shared mapping and 16-bit conversions.
Both named functions appear in the cached WOW32 map under those respective
objects. This supports **partial** classification of the shared declaration
and selected static chain, not full fax support. The separate original fax
driver and UI, spooler setup, shared-memory lifetime, failure paths, actual
device behavior, current deployed identity and 16-bit caller remain unproved.
The separate `_WOWFAX16_` conditional includes guest print declarations and
must not be conflated with the selected host branch.

Reproduce the JSON by running
`python artifacts/mvdm-audit/host-ch-scope/wow-debug-fax-declarations-observe.py`
from the repository root. A change to inputs or a rebuilt graph legitimately
changes the receipt. No guest, source, product, or shared ledger was changed.
