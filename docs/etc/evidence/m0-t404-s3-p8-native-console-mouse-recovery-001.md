# M0 T404 S3 P8 Native Console Mouse Recovery

## Decision

The supported surface is the ordinary `cmd/conhost` text Console. The earlier
self-painted presentation window is not an acceptance surface for this packet.
The original `SetupConsoleMode` owner now enables the public Console mouse
record mode while standalone `EDIT.COM` remains in `STREAM_IO`, sets extended
input flags, and clears QuickEdit. Its original mode-query, calculation and
restoration shape remains intact.

## Original-owner audit

`softpc.new/host/src/nt_hosts.c::SetupConsoleMode` is the direct original
owner of the Console input handle. Its non-stream branch already enabled
`ENABLE_MOUSE_INPUT`, but the standalone text path never entered that branch.
`nt_event.c` already dispatches `MOUSE_EVENT` to the original SoftPC mouse
route; no guest media, BOP, mapper, broker, graphics, or mouse-provider change
was necessary. This is therefore a same-owner, minimal mode recovery rather
than a new input implementation.

## Bounded regression result

On 2026-09-11 the selected x86 image was linked from
`build/M0-T404/S3/r008-p7-mouse-guest-attribution/original-softpc-process.exe`
and staged as `O:\ntvdm64\ntvdm32.exe`:

```
SHA-256 14b083fdc9a4a62c1fc6118744342a125b4a87dcbc08fe71933245b782b05db0
```

The bounded Console-owning observer launched that image with
`-f -o --command EDIT.COM`. It observed its actual `CONIN$` mode as
`0x000001b8`: `ENABLE_MOUSE_INPUT` and `ENABLE_EXTENDED_FLAGS` were set and
`ENABLE_QUICK_EDIT_MODE` was clear.

After the original callback registration, the observer deliberately waited
500ms. This is longer than the original `DelayMouseEvents(2)` startup interval
(330ms); an immediate sequence is correctly discarded by that source-owned
mode/startup guard. It then wrote ordinary public Console move, left-down,
drag and left-up records to that same `CONIN$` queue. The observer reported:

```
console-mouse-mode-observed=yes
console-mouse-input-ready=yes
console-mouse-input=delivered
```

The default-off source witnesses recorded each record through unchanged
`nt_process_mouse` (stage 1), IRQ completion (stage 2), `mouse_int1` (stage
3), converted coordinates (stage 4), callback selection (stage 5), original
callback transfer (stage 6), and `mouse_int2` return (stage 7). The move from
Console cell `(26,11)` reached the original guest path as `(208,88)`; the
press, drag and release completed the same route.

The transient PIC/suppression attribution markers used to distinguish the
330ms guard were removed before this record; they are not product behavior.

## Scope and remaining acceptance

This evidence proves the real Console queue and the original SoftPC route,
not a Win32 presentation window. The staged product is ready for a final
ordinary `cmd/conhost` owner-visible `EDIT.COM` click/drag/release check; no
manual re-test was used to substitute for the bounded engineering proof.
