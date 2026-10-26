# T467 S1 — WOW32 Winsock owner and lifetime inventory

## Scope and conclusion

This is the source-first admission record for the selected Win16 Winsock
surface.  It adds no provider, scheduler, callback carrier, socket registry,
guest binary, runtime image or release asset.  It establishes that the
original `WINSOCK.DLL` guest thunk, the selected original WOW32 bodies and
the native `WSOCK32.DLL` provider form a finite binding.  The immutable
`assets/winnt.zip` guest archive contains the original DLL; the currently
installed `O:\WINNT` tree does not.  Consequently this is not yet an
installed-package capability claim.

The active WOW32 build selects `wsraw.c`, `wsdata.c` and `wsext.c`, and the
formal x86 graph selects all 77 active original WOW32 sources.  `wwstbl2.h`
contains the dispatch table for the guest module, while the guest module
itself is an immutable, separately supplied original input.  No guest file is
to be put in the host release merely to make this testable.  S2 will unpack
the archive into an isolated test package and record the original DLL hash.

## Authority and data ownership

| Family | Original owner and call shape | Sole persistent authority | Normal / failure / retirement rule | S disposition |
| --- | --- | --- | --- | --- |
| Startup and per-task data | `wsext.c::WWS32WSAStartup` loads `WSOCK32.DLL`, starts native Winsock 1.1, registers `WWS32CallBackHandler` and `WWS32DispatchPostMessage`; its matching cleanup is `WWS32WSACleanup`. | `WINSOCK_THREAD_DATA` in the WOW task TLS slot; native provider owns its process service state. | Version/startup count is task-local.  Four 16-bit data buffers and the TLS record are released by `WWS32TaskCleanup`. | S2 proves 1.0/1.1 setup, nested startup/cleanup and failed setup without changing the owner. |
| Synchronous sockets | `wsraw.c` owns all mapped `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`, `select`, options and address conversion operations. | `WINSOCK_SOCKET_INFO` global list maps one native `SOCKET` to one 16-bit socket and records its creator task serial. | Caller-owned guest buffers are copied/reacquired across native calls; successful socket mapping is removed before native close.  Task rundown unlinks then abortively closes only matching serials outside the lock. | S2 proves loopback stream/UDP payload, address/option conversion, bad handle and peer-close behavior. |
| Blocking hook | `wsext.c::WWS32CallBackHandler` is the original registered native WSOCK32 hook; default dispatch peeks/translates/dispatches existing USER messages; a custom hook uses the existing synchronous `CallBack16(RET_WINSOCKBLOCKHOOK)` route. | `vBlockingHook` in the current task TLS record. | No callback object is retained after the native call.  The callback's low byte is its result.  `WSACancelBlockingCall` remains a native provider operation. | S2 proves install, cancel, default/custom dispatch and unhook through the original guest DLL. |
| Async select | `wsext.c::WWS32WSAAsyncSelect` / `WWS32PostAsyncSelect`; the native post routine encodes the 16-bit message in the high word. | Native provider owns readiness; socket map owns h16/h32 identity. | Completion maps native socket back to h16 and posts the original Win16 message.  A missing mapping is silently stale and must not create a replacement identity. | S3 owns accepted notification, disable/reconfigure, socket close and task/window loss. |
| Async name/proto/service lookup | `wsdata.c` allocates a `WINSOCK_ASYNC_CONTEXT_BLOCK`, native request buffer and 16-bit reply handle; `WWS32PostAsyncGet{Host,Proto,Serv}` removes it, converts into the caller buffer, posts and frees it. | The global async-context list owns each in-flight request record and native buffer. | Completion removes context under the critical section before conversion/post/free.  Caller owns the guest reply buffer until completion/cancellation/rundown. | S3 owns cancellation and task-loss retirement.  Current code is not sufficient to claim either. |
| Task exit | `wkman.c` calls `WWS32TaskCleanup` for initialized tasks; `wsext.c` owns its buffer/socket teardown. | Original task manager remains the task-lifetime authority. | Task cleanup clears TLS, unlinks its sockets under lock, closes them outside lock, then frees task data. | S3 must add only source-shaped async-context cancellation/rundown; no second task manager. |

Modern Windows owns real sockets, message queues, HWNDs and provider threads.
WOW32 only maps 16-bit inputs/results and preserves the original conversion,
dispatch and cleanup owners.  It must not treat a native socket, callback or
post-routine value as a guest-stable handle.

## Source-established defects and boundaries

These are facts of the selected original source, not claims that a repaired
runtime has passed:

1. `WWS32WSAUnhookBlockingHook` tests the non-null
   `WOW_WSAISBLOCKING` function pointer instead of calling it.  Thus an
   initialized provider always reports `WSAEINPROGRESS`, even when it is not
   blocking.  The identical expression is present in the pinned OpenNT
   source at `base/mvdm/wow32/wsext.c`.  S2 will use the smallest source-shaped
   correction—call the existing provider predicate—and register the mirror
   difference with positive idle and negative actively-blocking cases.
2. `WWS32WSACancelAsyncRequest` is an explicit `WSAEINVAL` stub.  It cannot
   cancel any request it records.
3. Async records have no creator task serial and `WWS32TaskCleanup` only
   retires sockets/data.  A task can therefore leave an async record whose
   completion refers to its former guest reply buffer.  S3 must give the
   existing context record the task identity it already needs, cancel/remove
   it exactly once, and reject late posts.  It may not add a global callback
   carrier or a new task registry.
4. The six async request constructors test `context == NULL` after allocating
   `buffer32`, rather than testing `buffer32 == NULL`.  A buffer-allocation
   failure can proceed with a null native buffer.  This is bounded with the
   async family and is an S3 correction candidate, not a reason to change
   synchronous socket handling.

`WWS32GetAsyncTaskHandle16` returns the incremented counter rather than its
local pre-increment variable.  The returned value and recorded value remain
the same on that call, so this audit records it as a source anomaly, not an
independent ownership bug.  S3 must exercise wrap and cancellation before
deciding whether a semantic correction is required.

## Executed local provider witness

`tests/observation/verify-wow-winsock-provider.ps1` was run from the known
desktop-capable 32-bit compiler-driver path, with an owned background probe in
`build/t467-s1-native-provider-r6`.  It compiles
`wow_winsock_provider_probe.c` against `ws2_32.lib` and confines all traffic
to an IPv4 loopback listener/client pair.  The process record is PID 19160,
2026-10-26T16:59:45Z; its result was:

```text
WINSOCK_PROVIDER result=0 stage=restore hook calls=31 foreign=0 outside_blocking=0 cancel=0 error=0
```

This proves only the modern `WSOCK32` 1.1 provider behavior that the original
bridge relies on: ordinal-109 blocking-hook installation/restoration, rejection
of a null hook, same-thread/TLS callback execution, delayed loopback receive
and cancellation returning `WSAEINTR`.  It does not prove the WOW thunk,
guest callback, 16-bit socket conversion, async request, cancellation or task
rundown path.  The probe now names every failing phase in its result and the
runner asserts its terminal `restore hook` marker; this turns a prior
ambiguous provider failure into a localizable assertion without changing
product code.

## Finite S2 handoff

S2 owns only the synchronous bridge: isolated original-guest `WINSOCK.DLL`
test staging; `WSAStartup`/`WSACleanup`; synchronous loopback socket,
address, buffer and error conversion; custom/default blocking hook;
`WSACancelBlockingCall`; and the source-proven unhook correction.  It must
not implement `WSACancelAsyncRequest`, async-context task ownership, a new
provider or any host-wide network policy.  The exact S3 owner is the existing
async context list and post routine: async select/query completion,
cancellation, stale completion and task/window/socket loss.

S2 acceptance requires the immutable original guest probe to demonstrate
loopback data plus custom-hook cancellation and successful unhook, with a
native local peer; it must record the test-only guest DLL hash and never add
that DLL to the normal package.  A WINSOCK-less normal Windows 3.1 package
remains a supported no-Winsock configuration, not a test failure.
