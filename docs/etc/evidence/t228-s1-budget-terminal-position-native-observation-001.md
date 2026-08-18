# T228 S1 budget-terminal position native observation

## Question

Does the first bounded immutable Direct/Readonly multi-program observation
provide a position that can be assigned to a missing BOP provider or a
complete owner domain?

## Reproducible invocation

`Invoke-T228S1GuestExecIntegration.ps1` was extended with the opt-in
`-ObserveBudgetTerminalPosition` flag. It retains its existing lifecycle
ledger and records whether the selector-blind mantle position observation was
selected. The run used the hash-locked T228 input manifest
`c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`,
100,000,000 internal ticks, and a 60-second external watchdog.

## Paired result

Both Direct and Readonly exited normally from the CLI worker with code 4
(the declared instruction-budget terminal), in about 13 seconds. Their stdout
and stderr were byte-identical.

Each output contains exactly one lifecycle-ledger record:

```
phase=1 cs=0041 eip=00005a70 ds=0c41 esi=00000100 disposition=1 resume=5a74
```

Neither run reached `50:3C`, `54:0B`, or a normal child/parent marker. The new
observation copied the identical budget terminal position in both modes:

```
CS:IP=5A5F:9F51
```

The identical Bochs diagnostics preceding it contain five existing
`math_abort: MSDOS compatibility FPU exception` messages and one existing
`LOCK prefix unallowed (op1=0xf1, modrm=0x00)` message.

## Attribution boundary

The observed terminal CS differs from the active COMMAND-segment candidate
`0C41` recorded at `50:36`. A real-mode physical calculation makes
`5A5F:9F51` a runtime address, not a static offset in the loaded COMMAND COM
image. The terminal position therefore cannot honestly be attributed from
this record to a specific OpenNT BOP provider, DEM service, COMMAND service,
or host mutation mode.

The paired identity does exclude a Direct/Readonly capability distinction at
this boundary. The remaining question is a whole guest/control-flow or
minimal-machine CPU/x87/interrupt continuity problem. It is deliberately
unclassified until a source/ABI/failure map can select one complete owner
domain. No BOP handler, guest-image change, or Bochs input-address patch is
admitted by this result.