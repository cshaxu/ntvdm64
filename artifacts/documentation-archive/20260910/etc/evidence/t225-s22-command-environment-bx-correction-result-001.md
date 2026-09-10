# T225 S22 P2 COMMAND Environment BX Contract Correction

## Finding

OpenNT `rdata.asm` invokes `54:0F` twice. After the second call it requires
`BX == EnvSiz`; `cmdenv.c` likewise returns the required paragraph count both
when capacity is insufficient and when it writes the environment. The prior
provider returned BX=0 on successful publication, contradicting that contract.

## Change and Verification

`bx_ntvdm_cmd_comspec_bootstrap_service.c` now returns
`ceil(environment_bytes / 16)` on successful publication and validates that
same value before consuming the state. The post-consumption repeat remains
BX=0, matching the original non-initial path. Focused package assertions were
updated accordingly.

A fresh formal Ninja rebuild under
`build/M0-T225-S22/startup-continuity-r1` linked the COMMAND family fixture
and native CLI. `t225-s20-command-package-family-fixture.exe` passed in
Direct and Readonly coverage.

## Limitation

The subsequent one-million-tick Direct/Readonly native observation remains
identical at the later `074A:0740` source-defined `C4 C4 06` machine boundary.
Therefore this correction is source-proven and regression-tested but not sufficient
to close startup continuity. S22 P3 assigns that boundary to the original SoftPC
INT 06 machine path; it does not authorize an adapter provider.