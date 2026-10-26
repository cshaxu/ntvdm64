# M0 T467 closure — selected WOW32 Winsock

T467 closes the selected original Win16 Winsock bridge without guest change or
rebuild.  It retains original socket, blocking-hook, async-query and async
select owners, using WSOCK32 only as the existing native provider.

## Delivered sequence

- S1 fixed the finite original owner/lifetime and archive-input inventory.
- S2 repaired the source-proven idle `WSAUnhookBlockingHook` predicate and
  proved the real Win16 synchronous loopback/callback/cancellation route.
- S3 repaired async query allocation, cancellation and task-rundown ownership
  without adding a new provider or callback carrier.
- S4 adds the missing real Win16 `WSAAsyncSelect` loopback notification,
  disable and close witness, and reconciles it with the preceding lifecycle
  evidence and published product package.

The detailed records are [S1](../etc/evidence/m0-t467-s1-winsock-owner-matrix.md),
[S2](../etc/evidence/m0-t467-s2-synchronous-winsock-bridge.md),
[S3](../etc/evidence/m0-t467-s3-winsock-async-lifetime.md) and
[S4](../etc/evidence/m0-t467-s4-winsock-acceptance.md).

## Boundary and successor

The capability covers the selected Winsock 1.1 conversion, loopback data,
blocking-hook cancellation, async query completion/cancellation/task loss and
async-select notification/disable/close.  It does not claim a forced provider
allocation failure, request-handle wrap, arbitrary external network policy or
a deterministically injected post-close provider race.  These limits add no
new runtime work to the next candidate.

The current published ten-image package remains `build/t467-s3/package-r2`;
the S4 test/documentation delivery changes no production image.  Guest media
is unchanged.  The next ordered candidate, WOW32 printing and spool, is
automatically admitted as **M0 T468 S1** for its source-first contract audit.
