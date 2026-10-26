# T467 S2 — WOW32 synchronous Winsock bridge

## Decision and source recovery

The selected runtime path remains the original chain:

```text
immutable WINSOCK.DLL -> WOW32 wsext.c/wsraw.c -> WSOCK32.DLL -> local IPv4 loopback
```

No guest image, guest source, provider, callback carrier, scheduler, socket
registry, or host network policy was added.  `wsraw.c` remains the owner of
the synchronous socket and conversion bodies; `wsext.c` remains the owner of
per-task startup, the native blocking-hook registration, synchronous
`CallBack16`, cancellation and cleanup.  The host owns actual sockets and
`WSOCK32` service state.  The guest owns its callback code, sockets and
buffers.  The existing WOW task TLS and its socket list are the only mapping
and lifetime authority.

The four-rung recovery ledger is finite:

1. The original `wsext.c::WWS32WSAUnhookBlockingHook` is directly composed.
2. No adapter is needed: its selected native `WSAIsBlocking` predicate already
   has the required no-argument provider ABI.
3. No external source is imported.
4. No new behavior is authored.

The source has one host correction, registered as `MVDM-HOST-DIV-462`:
the original condition tested the address of the non-null `WSAIsBlocking`
entrypoint rather than calling it.  Thus any initialized provider rejected
idle unhook as `WSAEINPROGRESS`.  The correction calls that same original
provider entrypoint and changes no callback, socket, error, mapping or
teardown ownership.

## Immutable inputs and probe shape

The test unpacks `assets/winnt.zip` only into its disposable package and
verifies archived `SYSTEM32/WINSOCK.DLL` SHA-256
`629E86C59C83F66D3623388B9ED6AE9D5A664979E588A9EB45B8A54441126FD9`.
The installed tree's absence of that optional guest DLL is deliberate and is
not changed.  `win16_winsock_probe.asm` is an independently assembled NE
program.  It uses only `127.0.0.1` UDP, starts Winsock 1.1, installs a guest
blocking hook, enters original `select`/`recv`, confirms the callback on its
own task, cancels the receive, checks `WSAEINTR`, unhooks while idle, closes,
frees its procedure instance and cleans up.

The runner validates every appended 28-byte `WSK1` checkpoint and evaluates
the final record.  It does not convert ordinary frame publication, use a
sleep as correctness, or rely on a UI screenshot.  The separately retained
native provider witness also proves host `WSOCK32` ordinal-109 hook
installation/restoration and same-thread cancellation:

```text
WINSOCK_PROVIDER result=0 stage=restore hook calls=31 foreign=0 outside_blocking=0 cancel=0 error=0
```

## Actual differential evidence

Both runs use the identical staged archive and all identical host images;
only a locally built `WOW32.DLL` differs.  The original-source reconstruction
is an evidence-only build, never a release input.

| Package | Final WSK1 phase/errors | cancellation | unhook/close/free/cleanup | Result |
| --- | --- | --- | --- | --- |
| original expression | `1 / 1` | callback 35; one cancel; `WSAEINTR` (10004) | `0 / 0 / 0 / 0` | expected idle-unhook rejection |
| DIV-462 correction | `2 / 0` | callback 35; one cancel; `WSAEINTR` (10004) | `1 / 1 / 1 / 1` | pass |

The first probe draft accidentally used ordinal 109 (SetBlockingHook) when it
claimed to exercise ordinal 110 (UnhookBlockingHook), and expected the former
API's returned previous procedure.  S2 corrects that test to call ordinal 110
with no arguments and assert its documented zero return.  This is why the
final differential is meaningful rather than merely a source inspection.

Affected x86 Ninja link succeeds.  Existing link warnings about historical
undeclared global helpers and duplicate legacy exports predate this change;
they neither introduce nor mask a new warning/error in the changed body.

## Boundary and S3 handoff

This closes only synchronous startup/callback/cancel/idle-unhook teardown.
`WSAAsync*`, `WSACancelAsyncRequest`, completion-message identity, async
context retirement and task-loss cleanup remain S3.  They are not claimed by
this evidence and no asynchronous state was changed here.
