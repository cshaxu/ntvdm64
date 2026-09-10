# M0 T378 S5 — Frozen original DOSX observation

## Fixed invocation

One and only one console-owning, non-debug observation ran with no
`--child-environment` diagnostic option:

```text
console-startup-observer.exe
  O:\ntvdm64\original-softpc-process.exe
  O:\ntvdm64
  O:\ntvdm64\observation-t378-s5-dosx-frozen.txt
  -f -o --command DOSX.EXE
```

The product SHA-256 was
`09d666c501b5ef859dd36dcac1c537db6c9f2d695ac2ca1c444d1762977085a8`.
The fixed media-manifest SHA-256 was
`59c6a2df219d022739312d44079354baf8cddd50f38b119fde502e50a3339004`.

## Result

The fixed observer returned:

```text
container=console-owning-nondebug
result=exited
exit=0x00000000
timeout-ms=8000
fixed-system-root=O:\ntvdm64
```

The observer's standard source-owned sidecars (not an optional child
diagnostic) show successful original DEM opens of:

```text
O:\NTVDM64\SYSTEM32\HIMEM.SYS
O:\NTVDM64\SYSTEM32\DOSX.EXE
```

Both have phase-0 and phase-1 success (`status=00000000`, `AX=0000`,
`CF=0`).  This confirms that the frozen original configuration/device route
reaches the immutable HIMEM carrier and that the declared original DOSX file
route is entered.

No `MS_bop_3`, `53:xx`, protected-mode, or DPMI table marker was emitted in
this single observation.  A normal process result consequently proves neither
general protected-mode application compatibility nor an arbitrary DOS-extender
matrix.  It is also not a failure that authorizes an isolated DPMI/BOP patch.

## Disposition

T378's selected workload is closed at its declared evidence level: immutable
source/media identity, original XMS and DPMI owner cohorts, formal CPU40 link,
and one fixed observation that reached HIMEM/DOSX file loading and returned
normally.  Any later protected-mode client matrix must be a separately
admitted workload package.
