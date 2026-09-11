# M0 T404 S3 P11 Native Console OpenNT Mouse-Contract Recovery

## Decision

P10's standalone movement filter is removed.  `SetupConsoleMode` is restored
to its original stream-I/O shape.  The only retained modern-Console adapter
change is at the original `host_disable_stream_io` transition: it enables
extended Console input and disables QuickEdit as it enables the pre-existing
mouse/window record route.

## Source basis

For an NTVDM INT 33h service, original
`softpc.new/base/keymouse/mouse_io.c::mouse_io_interrupt` first calls
`disable_stream_io`.  Original `video.c::disable_stream_io` reaches
`softpc.new/host/src/nt_fulsc.c::host_disable_stream_io`, which selects the
Console mode and enables mouse/window records.  The original `nt_event.c`
then queues movements, presses and releases without a standalone policy
filter.  Modern QuickEdit is a host interception absent from that historical
Console contract, so clearing it only at this original boundary is the
minimal public-Console adaptation.

P8 incorrectly forced that transition at startup and treated callback
registration (`stage=8`) as input readiness.  This P11 record supersedes that
claim; stage 8 proves only that the guest installed a callback.

## Build and bounded observation

The selected x86 image was formally linked from
`build/M0-T404/S3/r009-p12-console-motion-vector/original-softpc-process.exe`
and staged as `O:\ntvdm64\ntvdm32.exe`:

```
SHA-256 07311a6edd8e45e70e7e26af4bef8d587953d1d16d9346b871c70ad94e2a7ab7
```

The rebuilt bounded Console-owning observer launched `EDIT.COM` with the
published image.  It observed the actual `CONIN$` mode:

```
console-mouse-mode-observed=yes
console-input-mode=0x000001b8
console-mouse-input-ready=yes
console-mouse-input=delivered
```

`0x1b8` has mouse/window records and extended input enabled, with QuickEdit
clear.  The first synthetic movement was read by the original event worker
but correctly discarded during its source-owned short mouse/render transition.
The observer then retried only its external record; it made no product-state
change.  The accepted movement, left-down, drag and left-up each recorded the
ordered original path `stage=1→2→3→4→5→6→7`: Console record consumed, IRQ
queued, mouse interrupt entered, coordinate/call-mask conversion, callback
selection, guest callback transfer and `mouse_int2` return.

The source-owned callback masks distinguish the records as movement `1`,
left-button press `2`, drag movement `1`, and left-button release `4`.
Consequently raw Console movement is not converted into a synthetic left press
anywhere in the selected OpenNT event path.

The retained logs are `O:\ntvdm64\logs\t404-p11c-console-observer.txt` and
`O:\ntvdm64\logs\t404-p11c-mouse-chain.txt`; they are operational observation
logs, not repository inputs.

## P12 absolute-coordinate and button-contract audit

The owner specifically required that a host Console cell be delivered as the
guest's absolute text position, not replaced with a relative motion scheme.
The original visible-system-pointer branch already does that: it maps Console
cell `(column,row)` to callback `CX/DX`; `SI/DI` remain the separate original
relative-motion fields. A proposed visible-branch vector derivation was
withdrawn: it was not source-required and would not improve the authoritative
absolute hit test.

For one bounded observation build only, a default-off scalar witness recorded
the registers immediately after the original `jump_to_user_subroutine` stores.
The witness was removed before the final image above was rebuilt and staged.
Its public-Console sequence established the following exact callback contract:

| Console record | `AX` reason | `BX` buttons | `CX/DX` absolute position |
| --- | ---: | ---: | --- |
| move `(26,11)` | `1` | `0` | `(208,88)` |
| left-down `(26,11)` | `2` | `1` | `(208,88)` |
| drag `(54,21)` | `1` | `1` | `(432,168)` |
| left-up `(54,21)` | `4` | `0` | `(432,168)` |

Thus movement reaches the guest as movement, press as press, and release as
release; no Console row/column or button state is invented. The retained
operational audit logs are `O:\ntvdm64\logs\t404-p12-abi-console-observer.txt`
and `O:\ntvdm64\logs\t404-p12-abi-mouse-chain.txt`. The final minimum-diff
image then repeated the isolated `EDIT.COM` sequence successfully in
`O:\ntvdm64\logs\t404-p12-final-console-observer.txt` and
`O:\ntvdm64\logs\t404-p12-final-mouse-chain.txt`.

## Remaining acceptance

The published executable is ready for one owner-visible ordinary
`cmd/conhost` `EDIT.COM` check after Escape dismisses the welcome modal.
Expected behavior is normal hover without a synthetic click, and click/drag/
release only when physical buttons are used.  No guest mouse code, BOP path,
presentation window behavior, broker behavior or media changed.
