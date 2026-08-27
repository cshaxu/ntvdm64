# M0 T297 S1 — MVDM guest original-root recovery plan

## Objective

Restore the `mvdm-guest` mirror to explicit original guest roots while keeping
its DOS and Win16 content byte-identical:

- `dos/*` becomes `dos/v86/*`.
- `win16/bin86/*` becomes `bin86/*`.
- `win16/wow16/*` becomes `wow16/*`.
- `win16/{drivers,inc,system,timer,wfwnet}/*` becomes the corresponding
  `wow16/*` subtree.
- `win16/fonts/*` becomes `font16/*`.

## Boundaries

This is a path-only, hash-verified guest mirror move. The resulting `dos/v86`,
`bin86`, `wow16` and `font16` trees remain immutable guest inputs and never
enter a host compile/link graph. No font is moved to guest-media or removed.
No `src.old` file may be read or used.

## Verification

For every pre-move guest file, record old/new relative paths and SHA-256.
When a supplementary old path is a byte-identical duplicate of its canonical
WOW16 target, record it in a separate duplicate-elision ledger rather than
carry it twice. Reject every differing destination or changed hash. Update the
guest carry manifests and component README, confirm no old root remains, then
run documentation governance and `git diff --check`. Diagnostics that arise
solely from byte-identical imported original guest files are recorded, not
normalized; all T297-authored governance files must pass the same check.
