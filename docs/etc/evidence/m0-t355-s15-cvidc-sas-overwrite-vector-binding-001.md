# M0 T355 S15 — C-VID SAS overwrite-vector binding

## Scope

This record closes only the reached C-VID `Sas.Sas_overwrite_memory` vector
slot.  It does not enable or complete any other SAS vector, alter cache
behavior, change guest memory, alter a BOP/DEM result, or make a runtime
observation.

## Original contract and attribution

* `softpc.new/base/cvidc/accessfn.c::sas_overwrite_memory` invokes
  `Sas.Sas_overwrite_memory(addr, length)`.
* The selected generated initializer in
  `softpc.new/base/ccpu386/sascdef.c` leaves that member null.
* The selected original CPU40 provider is
  `softpc.new/base/ccpu386/ccpusas4.c::c_sas_overwrite_memory`, with the exact
  `PHY_ADDR, PHY_ADDR` contract declared through `sas.h`.
* Its original body is deliberately a no-op because this CCPU path has no
  compiled-code cache to invalidate.  This is an original CPU40 behavior, not
  a substitute cache policy.

## Binding and divergence

The existing C-VID vector-composition overlay assigns exactly that original
provider to the reached member.  The assignment is registered as
`MVDM-HOST-DIV-189` in `src/mvdm-host/README.md` and carries its local
`DIVERGENCE` comment.  No other `Sas` vector member is assigned or changed.

The overlay is used because the selected original CCPU/C-VID source set keeps
the generated carrier and the provider as separate original units, while the
historical product composition that connected this reached slot is not part of
the selected standalone CPU40 graph.

## Verification

The formal CPU40/x86 Ninja product target completed successfully after the
binding:

```text
build\\M0-T355\\S3\\formal-x86\\run-ninja-parallel.cmd original-softpc-process.exe
exit code: 0
ninja: no work to do.
```

This is source/link closure only.  The prior fixed-container runtime
observation stops at this exact owner before reaching `50:42`; no new runtime
claim is made here.
