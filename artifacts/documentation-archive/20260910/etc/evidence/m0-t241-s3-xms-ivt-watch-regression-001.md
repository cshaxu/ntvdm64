# M0 T241 S3 XMS IVT-Watch Regression

## Question

Does the recovered `52:09` route remain formally linked and bounded at the
product boundary without expanding its keyboard/INT15 scope?

## Procedure

1. Reused the fresh T241 S2 formal graph at
   `build/M0-T241-S2/formal-r1`.
2. Rebuilt and ran `t237-s3-xms-a20-source-mirror-fixture.exe`.
3. Ran the XMS-v1 retirement and machine-semantic boundary gates.
4. Linked `bin/ntdos64-native.exe` and ran one Direct, 100,000-tick
   observation against the existing `build/output/dos` and `wow16` bundle,
   with `share.exe`, BOP and generic-UD observers enabled.

## Observations

- The fixture exited zero after a native minimal-machine stage. It exercised
  the sampled IVT `15h` match/update and later mismatch/clear paths, with the
  imported XMS body returning its CX value.
- The XMS-v1 retirement gate and selector-blind machine boundary gate passed.
- The native CLI target linked successfully. Its one bounded observation
  returned exit `3`, `terminal=2 detail=3 lifecycle=3 presentation=3`, with
  BOP sequence `0` and generic-UD sequence `0`.

## Interpretation

The product link includes the recovered route, but this existing bundle
stopped before guest execution reached any BOP. It is therefore neither a
negative result for `52:09` nor authority to add a trace-driven patch. The
local native fixture is the positive source/body regression; guest EXEC and
parent-return are the next dependency-ordered package.
