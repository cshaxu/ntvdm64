# M0 T281 S1 — original BOP dispatcher build evidence

## Inputs

- Original dispatcher baseline:
  `O:\repos.external\opennt\base\mvdm\softpc.new\host\src\nt_bop.c`,
  SHA-256 `378a34807645277090bf4b38556ea210502f79a898855b649b39d6c4ccc47f66`.
- Tested mirror:
  `src/opennt-mvdm-host/softpc.new/host/src/nt_bop.c`, SHA-256
  `f93960cfc3c31efffbbe08598ed5bee2eaf50c5416fa8bbf6034510ac5fd2821`;
  it differs only at the three `MVDM-HOST-DIV-008` explicit casts.
- Byte-exact external ABI carrier: `source/opennt/public/internal/base/inc/vdm.h`,
  SHA-256 `c614c03a004f974c1eef37f1b29d3a8e1d7cfc33d04e7e7304befcbd6faafd18`.
- Existing original MVDM declaration roots: `opennt-mvdm-support/inc`,
  `softpc.new/host/inc`, `softpc.new/base/inc`, and `softpc.new/base/cvidc`.
- Generator: `tools/build/New-T281OriginalBopDispatchNinja.ps1`.

## Procedure

For each `x86` and `x64` graph, run the generator below `build/M0-T281/S1/`,
then run Ninja under the matching MSVC `/MT` developer environment:

```text
New-T281OriginalBopDispatchNinja.ps1 -Architecture <x86|x64>
ninja -v
Test-T281OriginalBopDispatchSurface.ps1 -Architecture <x86|x64>
```

The graph has one source input, creates `obj/nt_bop.obj`, and archives only
`original-bop-dispatch.lib`. It has neither a link/run rule nor an
`adapter-bop` source input.

## Observed result

Both x86 and x64 completed `cl` and `lib` successfully and produced their
separate static libraries. The compile retains historical warnings, including
old file-scope-function syntax, `strcpy`, and x64 pointer-truncation warnings;
they are source observations, not a claim that the corresponding dynamic
provider branches are safe to enable.

The selector-disabled surface verifier passed for both graphs. No provider,
guest route, adapter-bop ingress, executable, or dynamic dispatch path was
created or run.

## Interpretation

This proves a bounded source/build closure for the original dispatcher only.
It does not resolve the library's deferred external symbols and does not
authorize execution of any `MS_bop_*` selector. Dynamic WOW, installable BOP,
Redirector, VDD, debugger, monitor, and host-machine providers remain owned by
their later package plans.
