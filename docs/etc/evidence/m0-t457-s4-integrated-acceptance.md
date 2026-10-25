# M0 T457 S4 — integrated acceptance

## Result

S4 closes the complete T457 W06 boundary. No additional production seam was
observed after S2's modal scope repair and S3's shared-idle correction.

## Coherent package witnesses

All witnesses use the coherent ten-image package staged at
`build/t457-s3/package-r1`; its SHA-256 values match the release manifest and
the installed System32 target as recorded in `publication-r1.json`.

| Boundary | Integrated result |
| --- | --- |
| Win16 dialog/control/input/caret | `WOW_DIALOG_GUEST_OK phases=8 init=6 modal=2 rollback=1 keyboard=2 retirement=1` in `build/t457-s4/dialog-r1/result.txt` |
| Timer callback | `WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1 exception=1 denied=2` |
| Hook creation/reentry | `WOW_HOOK_CREATION_GUARD errors=0 enrollment=6 accepted=1 rollback=4`; its normal, denied, nested and exception scope assertions pass |
| Separate task/worker retirement | S3 actual guest witness confirms the exiting worker has no later idle callback and its peer continues normally |
| Shared task/worker retirement | S3 actual guest witness confirms A's registered vector executes on B, then both tasks finish normally without callback after retirement |

The native input-desktop idle-chain subcase remains unavailable before WOW code
because this execution environment cannot create/switch its input desktop. It
is recorded as a non-pass, not substituted by fixtures. The real guest shared
and separate worker witnesses cover the required lifecycle behavior in the
available isolated environment.

## Closure

The selected original providers remain singular: `wudlg.c`/`wumsg.c`/`wucaret.c`
for dialog/input/caret and `wutmr.c`/`wuhook.c`/`wowhooks.c` for timers/hooks.
The two repaired adapters are bounded call-context/registration seams; no
second USER manager, router, scheduler, timer engine or hook registry exists.
Guest media, guest binaries and guest configuration remain unchanged.
