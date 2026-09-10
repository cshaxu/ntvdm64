# OpenNT Gate 0 Toolchain Rebase

Status: historical-build fidelity correction, 2026-08-07.

## Decision

Gate 0 stages must prefer the recovered OpenNT-source-built `MASM386` over
JWasm or UASM whenever the historical source accepts it. A compatibility
assembler may be used only after a recorded proof that the recovered tool
cannot assemble the specific original input; it may not cause source-text
rewrites merely to make a stage complete.

The following older stages are retained as diagnostics only and are no longer
canonical source-to-image evidence:

| Artifact | Retired route | Reason |
| --- | --- | --- |
| `COMMAND.COM` | staged JWasm replacements plus one MASM386 object | the original 34 required sources all assemble with MASM386; the replacements changed guest source text |
| `HIMEM.SYS` | UASM field-address and symbol-case replacements | all six original sources assemble with MASM386 unchanged |

`EXE2BIN.EXE` already used an unchanged guest source stage and MASM386. Its
fresh v4 rebuild confirms that route.

## Historical-Link Evidence

The historical Microsoft `LINK.EXE` is run inside DOSBox rather than replaced
with WLink. Its input order is the untouched OpenNT response file in each
fresh stage. `opennt-reloc`, `opennt-stripz`, and `opennt-stripdd` are local,
test-validated format helpers used only where the corresponding OpenNT
makefile invokes that named post-link operation.

| Artifact | Original-source assembly | Historical link and packaging state | Current constraint |
| --- | --- | --- | --- |
| `COMMAND.COM` | 34/34 objects | `COMMAND.COM`, 50,384 bytes, SHA-256 `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` | runtime pending |
| `HIMEM.SYS` | 6/6 objects | `HIMEM.SYS`, 4,784 bytes, SHA-256 `995CBF0672E72DBD1041E6025B40BBD382C5D6EB85FFEA9889827266D72D265F` | runtime pending |
| `EXE2BIN.EXE` | 2/2 objects | `EXE2BIN.EXE`, 9,184 bytes, SHA-256 `DD8ED44FC09EEE0F4E9A3BC583F6C24E9456AEC4166775028523AB824E27EF6D` | runtime pending |
| `NTIO.SYS` | 12/12 objects | `NTIO.SYS`, 33,792 bytes, SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` | runtime pending |
| `NTDOS.SYS` | 38/38 objects | `NTDOS.SYS`, 27,858 bytes, SHA-256 `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` | runtime pending |

The repeatable NTIO and NTDOS entry points are
`toolchain-probe/build-opennt-ntio.ps1` and
`toolchain-probe/build-opennt-ntdos.ps1`. Each creates a new pristine source
stage, rejects a pre-existing destination, records the assembler object audit,
and writes a per-artifact manifest after packaging. The verified fresh runs
are retained under `artifacts/toolchain-runs/*-masm386-historical-link-v4/`.

The host-side BUILDMSG correction remains separately recorded in
`OPENNT-BUILDMSG-HOST-ABI-RECOVERY.md`. It changes the host port's fixed-size
status buffer so that a modern 32-bit process does not overwrite its adjacent
state; it never edits a staged guest source. Recovering a runnable historical
16-bit BUILDMSG is desirable evidence, but is not a reason to reintroduce
guest-source overlays.

## Enforcement

`stage-opennt-command.ps1` and `stage-opennt-himem.ps1` copy only manifested
upstream bytes and declare an empty `GuestSourceTransforms` list. The
canonical COMMAND, HIMEM, EXE2BIN, NTIO, and NTDOS build scripts invoke
MASM386 and historical LINK. WLink is no longer a canonical link path; its
old `W1162` results remain comparison evidence only.

This is a build-fidelity decision only. It does not prove a retail NT4 match,
guest execution, NTIO startup, or any host/runtime contract.
