# Shard 1: original raw spool and shared handle carrier

`wow32/wspool.c` is byte-identical to both frozen OpenNT editions. All 11 map-visible functions from its object bind to the current release WOW32 code; the observer records hashes, build membership, selected resolved internal call edges and the close routine's `LocalFree16` relocation. Optimization inlines carrier helpers in some consumers, so absence of a direct helper call is not absence of behavior. The six raw spool entry declarations occur in the original GDI table; this review does not verify its live dispatch or actual provider interaction.

## Ownership and scope

`WG32OpenJob` obtains the legacy `[devices]` mapping, opens a native printer and starts a document. `GetPrn16` places the native handle in a USER-instance `LocalAlloc16` block. `Prn32` obtains it through `LocalLock16`; `WG32CloseJob` ends the document, closes the printer and releases the carrier only when final close returns success. `WG32DeleteJob` uses the original DeletePrinter route without freeing the carrier. Historical T468 S3 explains ordinary and abrupt-task-loss ownership; S4's real guest raw OpenJob witness returned zero with no legacy mapping, and does not prove successful raw printing or task-loss behavior.

The carrier helper is also shared outside printing: `walias.h:249–251` maps HDWP conversions onto `GetPrn16`, `Prn32` and `FreePrn`; `wuwind.c:240,935,1095–1096` uses it for deferred window positions. Therefore a proposed spool-only helper change can alter deferred window positioning too. Those references are source-level evidence, not a fresh deferred-window runtime test.

## Unverified original failure candidates

- `WG32OpenJob` allocates `char szDriver[40]`; `GetDriverName` copies a matching profile key into that array using `lstrcpy` without a length parameter (`wspool.c:63,286–322`). A legacy mapping whose name exceeds the buffer could overflow if it reaches that branch; no suitable host mapping or runtime trigger was tested. Verify the actual Windows profile behavior and controlled provider route before classifying impact.
- `Prn32` declares `h32` without initialization and returns it if `LocalLock16` fails or mapped memory is unavailable (`wspool.c:254–273`). Source-level failure paths include both print and HDWP consumers; no evidence here establishes those failures or their reachable effects in a live guest. `GetPrn16` also has allocation/lock failure paths whose native-handle lifecycle is not shown to complete; OpenPrinter/StartDoc failure and `dwWritten` partial-write semantics require separate owner/error review. These behaviors occur in the original source, so do not call them project-introduced regressions or imply an approved semantic fix.

Replay: `python artifacts/mvdm-audit/shard-1/wow-spool-carrier-observer.py`. Companion JSON identifies image, source and object by hash. No product execution, provider changes, guest changes, shared ledger edits or build modifications were performed. This is partial static evidence; provider success/failure, exhaustion, task loss and deferred-window lifetimes remain unverified.
