# T467 S4 — WOW32 Winsock acceptance

## Decision

T467 closes the selected original Win16 Winsock capability.  The package uses
the original guest `WINSOCK.DLL` only as a hash-recorded, disposable test input;
it is not installed into the normal package.  The production path remains:

```text
Win16 WINSOCK.DLL -> original WOW32 wsraw/wsdata/wsext -> WSOCK32 -> loopback
```

Original WOW32 owns conversions, task TLS, the socket map, async context list
and cleanup.  WSOCK32 owns native sockets/requests; USER owns HWND/message
delivery.  No guest change, provider, scheduler, callback carrier, secondary
socket registry or network policy was added.

## Executable guest evidence

All cases used the published `build/t467-s3/package-r2` package, immutable
archive-derived `WINSOCK.DLL`, `console-startup-observer.exe`, a marker-owned
physical stage, and the disposable NE witness
`tests/observation/win16_winsock_async_probe.asm`.  Each observer reported
`result=exited`; each final guest record had `startup=0` and `errors=0`.

| Case | Guest assertion | Result |
| --- | --- | --- |
| `async-completion-r2` | Original ordinal-103 query posts exactly one completion for task handle 2 and frees its procedure. | phase 2, completion 1, freed 1. |
| `async-cancel-r2` | The original request is accepted by source-shaped cancellation and does not later post. | phase 2, cancel 1, completion 0, freed 1. |
| `async-task-loss-r2` | Direct Win16 task exit retires its in-flight request without a late completion. | phase 3, completion 0, freed 0. |
| `asyncselect-r13` | A task-owned UDP socket binds loopback, registers ordinal-101 `WSAAsyncSelect`, sends to itself, receives the original mapped h16/`FD_READ` window message, then disables notification and closes. | phase 2, completion 1, select/disabled/closed all 1. |
| `asyncselect-task-loss-r1` | A registered socket sends a loopback datagram then its Win16 task exits before it consumes a readiness message. | phase 3, select 1, completion 0, disabled/closed 0; observer exits cleanly. |

The new select case is intentionally self-contained: it requires no external
peer, sleep, polling protocol or added host test service.  The probe initially
reported `WSAENOTSOCK` because its test-only 16-bit call stack was reversed;
the successful final layout follows the original thunk's reverse argument
record.  A second probe issue treated the two `LPARAM` words in the opposite
order; the final witness asserts low word `FD_READ` and high word zero error.
Neither was a product defect.

Source review completes the retirement boundary: `WWS32PostAsyncSelect` maps
the native socket only at delivery; a missing map returns stale success.
`wsraw.c` removes a socket's mapping before native close, and task cleanup
unlinks same-task mappings before closing them.  Thus close/task loss cannot
create a replacement guest identity.  The guest witnesses prove notification,
disable, close and task exit with a pending select registration.  A deliberately
raced provider post after close cannot be deterministically injected through
this immutable provider and remains a source-reviewed stale-event boundary,
not a passing race claim.

## Package and limits

S3's complete cached component build and Product gate remain the tested
production package evidence: `build/t467-s3/package-r2` is three-way
hash-equal with `assets/release` and `O:\winnt\system32`; its `WOW32.DLL`
SHA-256 is `506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23`.
S4 changes only a disposable probe, its runner and records, so it does not
replace or redeploy that coherent ten-image package.

Forced native allocation failure, request-handle wrap and arbitrary external
network/device policy are outside the finite selected capability.  They are
not represented as successful coverage.  Guest media remains unchanged.
