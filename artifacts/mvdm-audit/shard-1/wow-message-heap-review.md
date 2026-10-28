# Shard 1: original WOW message heap and callback stack

## Bound implementation

`wow32/walloc16.c` is byte-identical to both frozen original editions. The observer binds all three `malloc16`, `free16`, and `stackalloc16` object functions to released WOW32 code, and resolves direct machine-code calls to `WOWGlobalAllocLock16`, `WOWGlobalUnlockFree16` and the current-TEB provider. The latter is a project support binding, not proof of guest execution. Exact hashes and owner names are in the accompanying JSON.

`malloc16` lazily obtains a shared 8 KiB Win16 global block split into 32 256-byte slots, with static headers and free-index state. Requests use the free slot or contiguous chain and fall back to independent `GlobalAllocLock16` when the pool cannot satisfy them. `free16` selects pool or global release from the pointer position. A slow-path request exactly divisible by 256 computes one additional slot; this is the original allocation policy, not evidence of an overflow or reason by itself to change it. `stackalloc16` instead advances the current task's callback stack bookkeeping: it is not the shared heap and is not released through `free16`.

Original callers include control/message translators in `wcntl32.c` and `wmdisp32.c`, EDIT word-break (`wcall32.c`), GDI enumeration, hooks, and fax. Current `MVDM-HOST-DIV-349` wraps many message-heap allocations in an owner/release path. It does not follow that every allocation site is covered; e.g. `wcntl32.c:472,487` retains an explicit pair. `wcall16.c:678–706,829–867` saves and sets up callback stack state, restores the stack on normal return and has a project `__finally` restoration when abnormal termination can restore control. These source-level relations do not independently prove all nested callbacks, teardown or error handling.

## Existing counterexample and limits

Shard 2 `wordbreak-boundary-review.md` reports an isolated real-object test of `W32EditNextWord`: a 256-character string overwrote the adjacent 256-byte allocation's sentinel because its caller allocated the character count but copied the terminator as well. Its controlled mapping/callback is not a live Win16 guest; preserve that finding's exact conditions and S15 ownership rather than attributing a generic allocator defect or assuming real guest repro. No new test or verification of its repair was performed here.

The pool's headers and free index are global in the original code and no locking is visible in `walloc16.c`. Whether existing WOW scheduling and caller boundaries serialize every access, including nested callbacks and abnormal exits, has not been established by this review. Double free, pointer range/identity, allocation failure and stack limit semantics are likewise untested. They are audit obligations, not proven product races or defects. Do not replace this original allocator, or classify it fully accepted, based solely on object/source identity.

Replay `python artifacts/mvdm-audit/shard-1/wow-message-heap-observer.py`. No guest, product source, shared ledger or build tree was modified, and no product process was run.
