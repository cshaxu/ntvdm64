# WOW resource conversion and record lifetime

Scope: shard 2 `wow32/wres16.c`; frozen reconciliation SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. Original OpenNT/OpenNT45 and current file are byte-identical SHA-256 24324D4B95B7C5AC102E9751872AF1DB779EFC0CFF223B32AA066034CB874AE9; direct object and map evidence is in `wow32-selected-link-inventory.json`. No guest or product changes or new runtime test. Exact file identity is not proof of entire feature restoration: keep pending.

## Calls and owner

`WU32DialogBoxParam` (`wudlg.c`) first computes `ConvertDialog16` output size when input size is absent, otherwise allocates a conservative double-size buffer; it converts and dispatches modal/modeless creation, then frees temporary heap buffer. `wcommdlg.c:2141` calls the same converter for a dialog template. `WU32LoadMenu` and `WU32LoadMenuIndirect` invoke `ConvertMenu16` before a host menu creation boundary.

`wres16.c` owns its linked `RES` list: `AddRes16` allocates/links a record, `LockResource16` obtains guest resource and caches converted output, `UnlockResource16` frees output, `FreeRes16` unlinks the record, and `DestroyRes16` scans and destroys records for an hmod argument. `wkman.c:1528` calls `DestroyRes16(ptd->htask16)` during task retirement. Common dialog callers use resource lock/unlock (e.g. `wcommdlg.c:941,983,1661,1715`). Task/module identity and reentrant teardown require a runtime witness rather than assuming all resources belong to exactly one task.

## Evidence and boundaries

`m0-t420-s40-wow-user-profile.md` records six passing fixture cases of extracted original `ConvertMenu16` plus original `WU32LoadMenu`/`Indirect`, synthetic guest owner-draw input, counted allocation/failure injection and real native HMENU boundary. The same entry explicitly says actual guest execution and callback paths remained open at that historical point. That fixture is not a full original `wres16` linked-resource lifecycle test. `m0-t447-s3-control-source-audit.md` traces the actual dialog conversion-to-modal/modeless chain but describes historical creator gaps; it cannot alone prove current production semantics.

One source-level failure candidate in the *caller* `wudlg.c::WU32DialogBoxParam`: local `ul` has no visible initialization and is assigned only under `if (cb && pDlg)`, then returned unconditionally. Zero conversion size or heap allocation failure could therefore return an indeterminate value if reachable. This is inherited source shape, not a reproduced product failure; verify valid guest reachability, original API failure contract, allocation-failure behavior, and any existing tests before deciding on an exception. No other shard or product source is changed.

Complete evidence should cover actual Win16 menu and dialog templates (including named/ordinal and nested menu forms), ownership through callback/reentry and task/module teardown, zero/truncated template and resource/malloc failure, while distinguishing guest invalid input and converted-buffer lifetime. No test result here proves all formats or malformed-input safety.
