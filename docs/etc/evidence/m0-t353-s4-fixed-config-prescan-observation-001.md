# M0 T353 S4 — fixed CONFIG pre-scan observation

## Frozen inputs

The already-linked product and immutable stage were unchanged from S3:

- product SHA-256: `32e3a533c1ee25b085d7bb80c773bb679bf86bdebc6c4224fda9021f15e865f1`;
- stage manifest SHA-256: `e0b9f35c1c1455805ce48fc66ebbdc11264db5e67820d853e2eb5284211cf907`;
- fixed `mvdm` system-root length: 39 characters.

S3's single launch is explicitly non-evidentiary because its report parent
did not exist. Before this S4 launch, only a new disposable report directory
was created. The observer, product, arguments, staged media and root were
otherwise unchanged.

## One durable observation

The non-debug console-owning observer timed out normally at its existing
eight-second `0x53504354` watchdog. Its main and BOP report SHA-256 values are
respectively:

```text
58ee59f10b6fc6165219c1213eb0e93e921012bfdd794087c17b7ccb19c52b37
289874050e58e1863d241334477dee2ed1d64597f5ad6829e5aa2eeea9f9d121
```

The durable source-ordered tail is:

```text
MVDM-BOP-RETURN 54:0C cs=8E08 ip=0736 ax=0E1A cf=0
MVDM-BOP-DISPATCH 50:12
MVDM-BOP-DISPATCH 50:00
MVDM-BOP-DISPATCH 50:21
MVDM-BOP-DISPATCH 50:42
MVDM-BOP-DISPATCH 50:02
MVDM-BOP-DISPATCH 50:21
MVDM-BOP-DISPATCH 50:12
MVDM-BOP-DISPATCH 50:3D
```

`50:12` is original `SVC_DEMOPEN`; `50:3D` is original
`SVC_DEMEXITVDM -> demExitVDM`, which displays the original bad-`config.nt`
error and terminates the current VDM. The only original guest
`SVC_DEMEXITVDM` call in the selected NTDOS bootstrap source is
`sysinit1.asm::stall`, immediately after `comerr -> badfil`: the permanent
COMMAND bootstrap failure terminal.

## T353 disposition

T353 has proved the source-defined post-`54:0C` bootstrap transition through
the NTDOS configuration pre-scan and has reached its exact original terminal.
It does **not** claim a successfully launched `COMMAND.COM`, workload, `EXEC`,
or graphical run.

The successor is the complete original **permanent COMMAND bootstrap namespace
and load** package: source-shaped command pathname formation, DOS-name to
host/media resolution, open/seek/read/close/handle ownership, `comerr` failure
direction, and only then the command image handoff. It must not repair the
observed `50:12` or `50:3D` services in isolation.
