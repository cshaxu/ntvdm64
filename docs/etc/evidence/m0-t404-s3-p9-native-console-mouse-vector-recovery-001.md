# M0 T404 S3 P9 Native Console Mouse-Vector Hypothesis Withdrawal

## Superseded temporary result

The vector derivation documented below was an uncommitted experiment. The
owner subsequently identified EDIT.COM's separate startup welcome modal and
reported that this form makes the apparent text insertion point follow mouse
movement. The experiment is withdrawn: its host-coordinate change, altered
callback witness, and `MVDM-HOST-DIV-265` are removed before commit. The raw
build and physical-release observations remain retained as historical evidence
only; they are not acceptance evidence for a product change.

## Rollback build and deployment

On 2026-09-11 the two restored original-source objects were compiled with the
selected x86 MSVC environment. The generated `original-softpc-keymouse.lib`
and `original-softpc-host-roots.lib` were rebuilt directly from the existing
formal Ninja graph's object lists, followed by its unchanged process link
argument vector. Link completed with only the historical non-fatal `LNK4070`
output-name warning.

The resulting P8-only image was deployed to `O:\ntvdm64\ntvdm32.exe`:

```
SHA-256 6859FF956792B9C19DAF0AC02DF55061A5DDEA0E3643E548819AD2602573A30B
```

It supersedes the withdrawn P9 experiment (`E732BD...BEC3`). Physical
EDIT.COM acceptance, after Escape dismisses the welcome modal, remains
pending.

## Finding

P8 correctly enabled real Console `MOUSE_EVENT` delivery, but the original
host-visible-pointer text branch passed a zero `MOUSE_VECTOR` into
`mouse_int1`. The original callback transfer therefore delivered a changed
position with zero motion to `EDIT.COM`. The owner observed that a physical
mouse move then opened EDIT.COM's Help surface.

## Original experimental disposition (withdrawn)

The direct original owners remain `softpc.new/host/src/nt_mouse.c` for Console
coordinate conversion and `base/keymouse/mouse_io.c` for the mouse interrupt
and INT 33h callback. No composable external facade can construct the vector
at the required point because only `nt_mouse.c` owns the converted current and
previous virtual coordinates. `MVDM-HOST-DIV-265` is the minimal host-boundary
intrusion: it derives the per-event signed vector before the unchanged
`old_x/old_y` update. The original mouse layer then accumulates it and passes
that original accumulator in callback `SI/DI`. No guest, BOP, event dispatch,
mouse provider, graphics, or broker source changed.

## Build and bounded Console result

`nt_mouse.c` compiled with the formal x86 host command. The changed
`mouse_io.c` was rebuilt into `original-softpc-keymouse.lib`, `nt_mouse.c` was
rebuilt into `original-softpc-host-roots.lib`, and the formal process link
completed without errors. The normal historical `LNK4070` output-name warning
remains non-fatal.

The staged x86 product is:

```
O:\ntvdm64\ntvdm32.exe
SHA-256 e732bd30955d871cc00176ae70b65a27ad070b9e430e5fbe997a050921debec3
```

The non-debug real-Console observer launched that exact image with
`-f -o --command EDIT.COM`, observed `CONIN$` mode `0x000001b8`, and delivered
its ordinary move/press/drag/release sequence. The default-off report shows:

```
Console (26,11): stage 4 CX/DX=208/88, per-event vector=208/88
                 stage 6 AX=1, CX/DX=208/88, SI/DI=208/88
Console (54,21): stage 4 CX/DX=432/168, per-event vector=224/80
                 stage 6 AX=1, CX/DX=432/168, SI/DI=432/168
```

The second `SI/DI` pair is the original accumulated motion, not a replacement
per-event protocol. Press and release retain their original `AX=2` and
`AX=4` callback masks. Raw observer reports remain under
`O:\ntvdm64\logs\m0-t404-s3-p9-vector-observer-2.*`.

## Limitation and next acceptance

Synthetic Console records prove the repaired host-to-callback shape but cannot
substitute for physical pointer acceptance. The owner must now verify that
moving the physical mouse in ordinary `cmd/conhost` `EDIT.COM` no longer opens
the program's Help surface, and that click/drag behaves normally. P9 remains
open until that result is reported.

## Physical release attribution

On 2026-09-11 the owner ran the staged product with the default-off report
enabled and reported a possible stuck button after press/release. The physical
record is unambiguous:

```
host press:   stage 1 button=1; stage 4/5 mask=2; stage 6 AX=2; stage 7 return
host release: stage 1 button=0; stage 4/5 mask=4; stage 6 AX=4; stage 7 return
```

Between them Console also supplied a `DOUBLE_CLICK` event with button state 1.
The original event owner does not synthesize a button transition for that
record. The final release is nevertheless delivered to the original callback,
and `host_os_mouse_pointer` has already assigned zero to its callback `BX`
button-state source before that callback runs. Therefore the host and INT 33h
button state is not stuck. Any remaining appearance of a held selection is an
EDIT.COM UI behavior or a separate interaction semantic, not a lost release.
The raw record remains
`O:\ntvdm64\logs\edit-mouse-physical.txt`.
