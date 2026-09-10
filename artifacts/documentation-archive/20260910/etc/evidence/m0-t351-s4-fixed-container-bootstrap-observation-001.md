# M0 T351 S4 — fixed-container bootstrap observation

## Immutable input

One CPU40/x86 product formally linked in T351 S2 was staged with
`tools/build/Stage-OriginalSoftpcRuntime.mjs` into
`build/M0-T351/S4/fixed-container`.  The staged product SHA-256 was:

```text
059b8a99cf52596c429f5045d2f1fa4a8e5bb5accfc44328d3aacc2fffd0eda8
```

The stage manifest SHA-256 was
`9ad0373af7334307012f858196bf16baae7df001ed13aa8150789acf0262140c`.
It records the unchanged original DOS/COMMAND and SoftPC firmware asset
hashes.  No source, media, command line, console mode, timeout, or observer
was changed after S3 froze the input.

## One observation

The existing non-debug, console-owning observer launched exactly:

```text
build/tools/console-startup-observer.exe
  build/M0-T351/S4/fixed-container/original-softpc-process.exe
  build/M0-T351/S4/fixed-container
  build/M0-T351/S4/fixed-container/startup-observation.txt
```

The observer supplied owned `CONIN$`/`CONOUT$`, launched one child with
`-f -o --ordinary-child`, and waited its fixed 8,000 ms.  Its report SHA-256
was `e57632dc9f74a93ebe3091af4eb5b7a4e93317cb1134ab0fbe2ddf98ced272a`;
the companion console snapshot SHA-256 was
`08d02176a75149096c905ce20b00042429eac4d9aaf95943d0ee80733c54bb90`.

The child timed out and the fixed watchdog ended that child only with
`0x53504354`.  The console snapshot contains the original ingress markers:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
... 54:05
```

## Classification and limit

This confirms the same pre-existing fact under the T351 formal product:
original COMMAND `54:05` ingress is reached before the fixed timeout.  It
does **not** prove that `cmdSetInfo` returned, that CPU40 executed the resumed
post-BOP stream, or that NTDOS `msinit.asm` established its stack.  It also
does not identify a fault or authorize a trace-selected BOP repair.

T351 therefore closes its documented source/binding/formal/one-observation
scope only.  The next runtime work must first select one complete source owner
package for the still-unattributed post-`54:05` CPU40/guest continuation; the
queued graphics workload remains unreachable until such a package establishes
that predecessor.
