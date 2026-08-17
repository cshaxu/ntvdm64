# T225 S35 paired bootstrap/first-fault native observation result

## Inputs and procedure

- Reused formal native CLI: `build/M0-T225-S30/segment-access-r3/bin/ntdos64-native.exe`, SHA-256 `BF0C2C8FD64D79AE97FCFA4D449FA9907FC4EE0577A3FCB89ADF345A4FC3C095`.
- Immutable S10 input root: `build/M0-T225-S10/primary-bootstrap-inputs-r2/`; profile SHA-256 `C41D200BD3FDAE40D445CF3F8CA6256C0E94DB3988A9DF250C7C3450D52C3D36`; target SHA-256 `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.
- The existing paired config/autoexec source environment was supplied.
- Exactly one Direct and one Readonly invocation used `--instruction-tick-budget 1000000 --observe-bop-sequence --observe-first-fault`. No source, profile, image, provider, or machine change preceded either run.

Raw outputs remain disposable under `build/M0-T225-S35/paired-observation-r1/`.

## Result

Both runs exited `1`, captured forty BOP records without overflow, then captured
the same first non-UD fault and segment-access tuple. Their stdout is byte
identical with SHA-256 `142FF7BECAB65F476AAB37E807084AF7CADA551D0FB1E65C33D732B7DDC73D19`; stderr is byte identical with SHA-256 `CB8526DC115A44AB0B3511EBA6D1C0892FE0534836E1DB9AA61BAB0198021BC6`.

The same-invocation tail is:

| Order | Record | Interpretation boundary |
| --- | --- | --- |
| 33–38 | `0C41` COMMAND `54:02/0D/0F` plus existing DEM file calls | Original host-service packages are reached; this is observation, not new implementation authority. |
| 39 | `074A:0740`, selector `06`, accepted | Existing startup-machine component is reached. The transcript alone does not establish its post-return machine state. |
| first non-UD | `#GP(13)`, `0C41:2357`, EAX `0`, real mode | The failure occurs after the accepted `06h` BOP in this invocation. Causality is unproven. |
| access tuple | DS index `3`, selector `0C41`, word at `FFFF`, direct-limit branch | Unchanged S30 mechanical fact; Direct/Readonly policy is excluded as the cause. |

## Disposition

S35 binds bootstrap, COMMAND, startup-machine and first-fault observations in
the same execution. It does **not** prove that selector `06` caused the fault,
that a BOP/provider must change, or that Bochs needs a segment-wrap repair.
The next owner is the already whole-package startup-machine component: source
and ABI audit its accepted `06h` result/return contract and caller continuation
against the first-fault state, without a singleton service patch.