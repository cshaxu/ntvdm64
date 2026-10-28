# WOW Winsock ABI and thunk table review — shard 3

The two shard 3 source items are `inc/wowwsock.h` and `wow32/wwstbl2.h`. `winsock-abi-observer.py` and captured JSON compare SHA-256 against both original trees and preserve excerpts from the existing WOW32 linker map. No compilation, target execution, guest modification or network test was performed.

## Original calling chain

- `wow32/precomp.h:131` includes `winsockp.h`, which at `:23` includes `wowwsock.h`. The latter supplies 16-bit socket handles, packed 16-bit address and argument structures (`inc/wowwsock.h:17-35,172-205`).
- `wow32/wowtbl.c:60` includes `wwstbl2.h` in the master thunk table; `:110` establishes the module offset. Entries include BIND and CONNECT (`wwstbl2.h:19,21`) and SOCKET (`:44`), alongside explicit unimplemented entries. Thunk entry sizes refer to the corresponding packed `*16` argument structures.
- `wow32/wsraw.c:250-302` shows the BIND example: obtain the 16-bit arguments, translate the socket handle and address, call the native Winsock binding, and release the argument mapping after a call that can reenter 16-bit code. CONNECT and socket paths appear at `:361` and `:1559` respectively. This is a source-level chain, not proof of live dispatch or reentrant safety.
- Existing shard evidence (`file-review.csv` rows for the two paths) reports 77 compile dependency edges for the ABI header and one for the table header. The observed existing WOW32 map includes `InitThunkTableOffsets` from wowtbl.obj and WWS32bind/connect/socket plus WSThunkAddrBuf from wsraw.obj; its SHA-256 and exact map lines are captured in JSON. Map-to-current-released-DLL identity and actual table bytes were not established here.

## Status and divergence

| path | proposed status | precise limit |
|---|---|---|
| `inc/wowwsock.h` | pending | Exact identity to both original trees and historical consumers/recorded dependency are verified. Actual compiled layouts, per-API guest ABI, failure handling, lifetime and live behavior remain unverified. |
| `wow32/wwstbl2.h` | pending | Exact identity, static table entries and recorded table source dependency verified; map confirms associated implementation symbols in the observed build. No direct evidence verifies table bytes, product identity, all ordinal routes or behavior. |

Do not infer that the entire chain is original: `wow32/winsockp.h` differs from both references. Current file adds `ThreadSerialNumber` to the async context and declarations for task-scoped lookup/removal and cancellation. These are independent of the two identical shard 3 files and require the shard 0 implementation and lifecycle audit; no correctness or defect conclusion follows solely from this diff. `wow32/wsraw.c` and `wowtbl.c` hashes match both references, but runtime wrappers, task lifetime, async completion/cancellation, memory movement and socket errors remain open. No percentage of restored runtime behavior is inferred.

The asynchronous context change is already registered as MVDM-HOST-DIV-463; see docs/etc/evidence/m0-t467-s3-winsock-async-lifetime.md for its rationale and its stated allocation-failure, handle-wrap and async-select test boundaries.

Additional follow-up boundary (not an established defect): `WWS32CancelAsyncRequestsForTask` ignores the provider cancellation return before releasing `Buffer32`; verify the provider contract and buffer lifetime when cancellation fails or races completion, including whether a write can still occur. Also verify concurrency and wraparound of `WWS32ThreadSerialNumberCounter++`. The existing exception and evidence do not by themselves prove those cases safe.
