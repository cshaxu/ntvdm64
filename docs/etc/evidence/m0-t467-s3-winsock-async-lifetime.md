# T467 S3 — WOW32 async Winsock lifetime

## Question

Can the selected original Win16 `WINSOCK.DLL` async database-query path deliver
one correctly identified completion, accept cancellation, and retire an
in-flight context when its WOW task exits without introducing a provider,
scheduler, callback carrier or guest change?

## Source and ownership result

The six original `WSAAsyncGet*` constructors in `wow32/wsdata.c` already own
one global `WINSOCK_ASYNC_CONTEXT_BLOCK` list and the original three post
conversion bodies own completion/removal.  The source has three bounded gaps:

- each constructor tests `context` rather than the second `buffer32`
  allocation;
- `WSACancelAsyncRequest` is an explicit `WSAEINVAL` stub despite the context
  retaining its native request handle; and
- a context has no task serial, so `WWS32TaskCleanup` cannot retire its former
  16-bit reply buffer before the task is destroyed.

`MVDM-HOST-DIV-463` adds only the existing task serial to that context.  An
explicit cancel first finds its same-task native request, delegates to the
existing WSOCK32 provider, then removes/releases the context on provider
success.  Task rundown detaches all same-serial records under the existing
lock, then calls the same provider cancellation and frees them outside the
lock.  A provider completion that wins after removal is stale and returns
success without touching guest memory.  Normal completion still removes,
converts, posts and frees through the selected original body.

Authority remains singular: the original async list owns in-flight context and
native buffer; the original task manager owns task exit; WSOCK32 owns native
request execution; USER owns HWND/message delivery; the guest owns its reply
buffer until its completion, cancellation or task rundown.  No guest file,
provider, host-wide registry or callback thread was added.

## Executed evidence

The independent disposable NE witness
`tests/observation/win16_winsock_async_probe.asm` uses immutable original
`WINSOCK.DLL`, a task-owned hidden USER window and the original ordinal-103
`WSAAsyncGetHostByName("localhost")` path.  It is staged only into a physical
isolated package; no guest image is modified.

| Run | Result |
| --- | --- |
| `async-completion-r4` | PASS: phase 2, zero errors/startup error, task handle 2, exactly one completion, procedure freed. |
| `async-cancel-r1` | PASS: phase 2, zero errors, native cancellation accepted, zero completion, procedure freed. |
| `async-task-loss-r1` | PASS: phase 3 direct task exit after an in-flight request, zero errors and no completion.  The observer exited 0; the verified transient runtime was removed after all project processes had exited. |

The final candidate package is `build/t467-s3/package-r2`, constructed with
the current x86 `WOW32.DLL` and coherent previously verified host images. Its
ten-image manifest matches the exact runtime manifest of the final product
gate: `WOW32.DLL` is
`506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23`; every
other image is unchanged from the S2 package.  The positive observer records
`result=exited`, exit zero, and no graphics handshake.  The test has no
external network peer: `localhost` is local.

The retained async-select peer/window-loss route needs no new context owner:
`WWS32PostAsyncSelect` looks up the existing original socket map at delivery
time and treats a missing mapping as an already-retired stale event.  The
socket owner is removed before native close in `wsraw.c`; task rundown removes
same-task map entries before it closes the native sockets.  Thus an event
after socket or task loss cannot recreate a guest socket identity.  This is a
source boundary review, not a claim that the database-query cancellation
test exercises `WSAAsyncSelect`; loopback socket peer-close/cleanup remains
the retained S2 synchronous-socket contract.

`Invoke-ProductVerification.ps1 -Suite Product` against this exact candidate
completed in `build/t467-s3/product-r7`: retained WOW frontiers passed in
65,346 ms, Console-17 in 43,804 ms, and Window-17 in 42,886 ms (155,759 ms
total).  The three frontiers retain the expected WINMINE, SOL and WRITE
application signatures; the two 17-case matrices retain their output,
completion and cleanup assertions.  Product gate success proves that the
async DLL is part of a coherent host package; it does not turn the immutable
guest DLL into a released normal-package member.

## Scope and remaining verification

These runs prove the newly changed async-query ownership edges, and the
product gate proves the coherent package boundary.  The six constructors all
share the reviewed corrected `buffer32 == NULL` allocation branch; forced
allocation failure and 16-bit request-handle wrap are not independently
injectable through the immutable guest/provider route and remain explicit
coverage limits, not passing claims.  The `WSAAsyncSelect` socket/window-loss
family retains its distinct original socket-map owner described above; it is
not expanded into a second registry by this S.
