# M0 T377 S5 — Frozen DOS device workload observation

## One immutable container

S5 made the three predeclared observations in `O:\\ntvdm64`, using the same
console-owning observer, formal product and media manifest.  No invocation
used `--child-environment` or any other optional diagnostic.  All three JSON
sidecars record the identical identities:

- staged-manifest SHA-256:
  `499dd7d55de944173359973e1eb01ed6468a036e8c49a130b5c0b5cb873420cc`;
- fixed-media-manifest SHA-256:
  `59c6a2df219d022739312d44079354baf8cddd50f38b119fde502e50a3339004`;
- product SHA-256:
  `09d666c501b5ef859dd36dcac1c537db6c9f2d695ac2ca1c444d1762977085a8`.

The root was `O:\\ntvdm64` in every report (10 characters), within the
original NTDOS generated-shell-path limit.

## Results

- `MEM.EXE` exited `0x00000000` before the eight-second bound.
- `KB16.COM US` exited `0x00000000` before the same bound.
- `GRAPHICS.COM` exited `0x00000048` before the same bound.

The first two are bounded successful returns of the selected original DOS
workload artifacts.  They do not prove interactive keyboard input, a keyboard
layout installation transaction, arbitrary DEM IOCTL behavior, or a general
console device contract.

`0x48` is not an original `GRAPHICS.COM` error exit.  The mirrored original
`grinst.asm` takes its ordinary error exit through DOS `INT 21h/AH=4Ch` with
`AL=1`; the app's distinct `APP_STARTUP_DISPOSE_FAILURE` is `72` (`0x48`) and
is returned only when `session_dispose_with_reason` rejects final cleanup after
the original machine shell has returned.  With diagnostics deliberately
disabled in this frozen S, no source-owned event identifies whether graphics
had installed an interrupt vector, reached presentation, or why final session
cleanup rejected.  It is therefore an exact **app/session cleanup boundary**,
not evidence of a VGA, timer, IRQ, C-video, font or `GRAPHICS.PRO` defect.

## Disposition

The complete immediate original owner cohorts are formally linked; selected
`MEM` and `KB16` return normally under one immutable container.  The graphics
workload reaches the explicit post-shell app/session boundary above.  T377
does not create a trace-selected graphics repair.  Any attempt to recover the
cleanup result must be a whole session lifecycle package, and only after its
original-owner and binding contract is admitted.
