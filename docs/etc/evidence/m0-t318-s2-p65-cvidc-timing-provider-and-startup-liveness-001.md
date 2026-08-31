# M0 T318 S2 P65 — C-VID timing provider and startup liveness

## Problem

After P64, the fixed x86 product reached `0xc00000fd` in C-VID's
`host_get_jump_restart` forwarding shim.  The C-VID metadata identifies the
three associated public CPU-vector slots:

- `GetJumpCalibrateVal`;
- `GetJumpInitialVal`; and
- `SetJumpInitialVal`.

The retained source has forwarding shims in `base/cvidc/accessfn.c`, but no
paired generated provider body.  The previous generated binding erroneously
bound each slot to the same forwarding shim, producing infinite recursion.

## Source-shaped repair

The binding generator now routes exactly those three slots to an overlay-local
timing provider.  The provider preserves the original `qevnt.c` initial
`qevJumpRestart` value of 100, returns it for calibration/restart requests,
and stores the value supplied by the original setter.  No other C-VID/CCPU
slot is redirected or given new behavior.  This is registered as
`MVDM-HOST-DIV-163`: a missing historical generated artifact, not a replacement
CPU, video controller, or guest-memory path.

## Verification

- Formal Ninja x86 product link: passed.
- Formal Ninja x64 same-source product link: passed.
- The unchanged `runtime-r33-config-system-media` container launched the x86
  product with the unchanged `-f -o --ordinary-child` contract and 8000 ms
  timeout.  Its result is now:

```
container=console-owning-nondebug
result=timeout
exit=0x53504354
timeout-ms=8000
```

The report records the immutable media manifest hash and staged product hash.

## Result

The selected original CPU40/CCPU/C-VID startup path now remains alive for the
entire bounded observation instead of escaping through either the first null
video vector or the generated timing-vector recursion.  This is startup
liveness only.  It does not establish that NTIO, NTDOS, EXEC, or ordinary
guest-child completion has been reached.
