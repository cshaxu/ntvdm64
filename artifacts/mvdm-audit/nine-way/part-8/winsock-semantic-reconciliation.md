# Part 8: Win16 WINSOCK entry and callback boundary

The two owned guest source paths are independently hash-bound against both original editions in `guest-family-inventory-observations.json`. `wow16/winsock/winsock.def` defines Winsock 1.x ordinals including startup, cleanup, synchronous sockets and asynchronous/blocking hooks. `winsock.asm` emits original 16-bit `WinsockThunk` entries and WEP; source presence does not prove that a matching binary is part of the current package or that every ordinal works.

Original WOW32 `wsext.c:308-568` loads host WSOCK32, establishes thread-local buffers and installs the blocking callback; `WWS32CallBackHandler` routes default pumping or original guest callback. `WWS32WSACleanup` owns the corresponding cleanup. Host Winsock owns native sockets and I/O, guest/WOW owns its 16-bit data and task/thread-bound callback state. A callback must preserve actual task identity and execution return, rather than being treated as a host window procedure or a new network dispatcher.

| Owned path | Bounded decision |
| --- | --- |
| `wow16/winsock/winsock.asm` | Preserve original guest thunk interfaces, callback behavior and alternate source; selected packaged binary must be independently identified. |
| `wow16/winsock/winsock.def` | Preserve ordinal/ABI contract and resident WEP; export presence does not establish provider, callbacks or network success. |

The T447 S2 record at `m0-t447-s2-scope-freeze.md:2695-2719` says its earlier frozen package lacked WINSOCK/WSOCK, yet an exact-hash original WINSOCK.DLL existed in source and was initially deferred. Later `:2845-2859` records an isolated controlled `frozen-winsock-guest-r2` with that original guest and worker SHA `373CF39CEA1264A38DDE72621412DE09072CA1F7205E2573DE7B470F9F2A2411`: 35 callbacks, one cancellation, error 10004 and cleanup passed. This is a *later local callback witness* on a specific candidate, not general network acceptance or proof of the current release's guest composition. Historical `m0-t422-s1-dynamic-host-surface-audit.md` identifies 49 ordinal lookups; export existence alone does not prove transactional startup and teardown.

S14 should identify current guest/provider package explicitly, exercise startup failure and balanced repeated startup/cleanup, real guest loopback send/receive, blocking-hook nesting/cancel and task loss with scoped peer; distinguish no provider from network errors. S11 owns reentrant callback execution and identity; S19 ties results to released hashes. Preserve original thunk and host socket owner, avoid a second registry or a made-up success result, and do not modify/rebuild guest. No tests ran during this review.

Replay `rg -n "WinsockThunk|WEP" src/mvdm/wow16/winsock/winsock.asm`, inspect winsock.def, `rg -n "WWS32WSAStartup|WWS32WSACleanup|WWS32CallBackHandler" src/mvdm/wow32/wsext.c`, read the exact cited T447 sections and part-8 inventory observer. No product or shared ledger change.
