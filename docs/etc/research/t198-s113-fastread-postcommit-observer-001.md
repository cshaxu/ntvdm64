# M0 T198 S113: DEMFASTREAD Post-Commit Observer

## Change

The native fixture's test-only bridge now records one `50:42` event only after
`bx_ntvdm_boot_namespace_composition_v1_handle` has returned true with:

- canonical `C4 C4 50 42` bytes;
- typed `RESUME` disposition; and
- `resume_rip == fault_rip + 4`.

It copies only fixed-width result metadata: resume RIP, GPR16 write mask, AX,
EFLAGS write mask and EFLAGS value. It reads no guest memory or provider
state, mutates no event/outcome and is absent from production composition.
The existing fallback marker remains after a false composition return, so the
two states cannot be conflated.

## Verification

- `artifacts/build/t198-s113-postcommit-observer-compile-r2` compiles the
  updated test bridge and fixture against the locked CPU5/x64 configuration;
  it does not link or run guest code.
- `tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` passes at
  `artifacts/build/t198-s113-postcommit-composition-r1`. Its existing focused
  composition test verifies `50:42` returns a checked three-byte write with
  AX=`3`, while `50:43` remains pass-through.

## Boundary

The next task may make one native observation. The observer neither changes
the `50:42` provider nor authorizes FASTWRITE, ROM/VGA, Bochs changes or a
second run.
