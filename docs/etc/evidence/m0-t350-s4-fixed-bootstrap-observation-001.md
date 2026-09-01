# M0 T350 S4 — Fixed CPU40 bootstrap observation

## Immutable container

S4 staged the S3 x86 formal product under
`build/M0-T350/S4/runtime-x86` with the existing
`Stage-OriginalSoftpcRuntime.mjs` media manifest.  The one observation used
the existing `build/tools/console-startup-observer.exe` in non-debug,
console-owning mode, an 8,000 ms timeout, and exactly:

```text
-f -o --ordinary-child
```

The observer report is retained as untracked research evidence at
`artifacts/research/m0-t350-s4-fixed-container.txt` and sidecars.  Its
manifest identity was:

- staged manifest SHA-256:
  `71fd36af2f13a64f8583408daa65fa455bbeb6edef32d385a2d4196bcda2f12a`;
- fixed media SHA-256:
  `15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`;
- selected product SHA-256:
  `c7b0e8023bda0c18cdd53b675c41b884544f1020c3955adfb6f6a9dfb67edce6`.

## Observed result

The observer itself completed successfully.  The product reached its ordinary
bounded timeout result `0x53504354`; it did not report a product exception.
The console contains the existing original ingress sequence:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
MVDM-BOP-DISPATCH 54:05
```

The source mapping is exact: `MS_bop_4` decodes the service byte, emits the
state-neutral observation record, then calls original `CmdDispatch`; index
`0x05` is original `cmdSetInfo`.  Thus the final marker proves only that the
unchanged CPU40 product again reaches the COMMAND ingress before that original
dispatcher call.

## Disposition

No marker after `54:05`, guest-stack record, or child result was observed.
This run consequently does **not** prove that `cmdSetInfo` returned, that
`msinit.asm` copied `SCSINFO`, that NTDOS changed to `dskstack`, or that
COMMAND.COM/EXEC executed.  The result does not select a BOP repair: a second
run would vary no source-defined owner condition and is prohibited by this
packet.

T350 closes its declared source/formal CPU40/SAS/bootstrap cohort and one
classified integration observation.  The remaining presentation/runtime
criterion transfers to the queued SoftPC graphics-workload integration package;
any later post-`54:05` guest-continuity package must begin with a fresh
source-owner admission rather than treating this timeout as a leaf failure.

