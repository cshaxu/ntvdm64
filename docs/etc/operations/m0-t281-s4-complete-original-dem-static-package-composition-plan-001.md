# M0 T281 S4 — complete original DEM static package composition plan

## Purpose

Combine the two previously verified static composition sets into the complete
original sixteen-file DEM package. This checks package coherence while keeping
all BOP and provider runtime entrypoints disabled.

## Inputs

- S2: `dem.c`, `demdata.c`, `demmsg.c`, and `demdisp.c`;
- S3: `demdasd.c`, `demdir.c`, `demerror.c`, `demfcb.c`, `demfile.c`,
  `demgset.c`, `demhndl.c`, `demioctl.c`, `demlabel.c`, `demlock.c`,
  `demmisc.c`, and `demsrch.c`;
- Existing declaration-only `adapter-vdm-monitor/include/vdm.h` for reached
  `NtVdmControl(VdmQueryDir, ...)` source shape.

## Constraints

- One `.lib` per architecture; no executable, link-to-run rule, selector or
  provider invocation;
- exact original file bodies except already registered mirror divergences;
- no `adapter-vdm-monitor` implementation object, Bochs source, or BOP
  ingress source in the graph.

## Exit criteria

The archive member list is exactly the sixteen named DEM objects on x86/x64;
both generated graphs and their archives pass the selector-disabled verifier;
the source/member ledger and evidence are reproducible; governance checks
pass.
