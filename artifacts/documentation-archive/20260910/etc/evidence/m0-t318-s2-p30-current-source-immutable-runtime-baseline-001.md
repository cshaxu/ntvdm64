# M0 T318 S2 P30 — Current-source immutable runtime baseline

## Correction

P25 named the mutable `cpu40-v7vga-r4` build directory as its product input.
Its manifest recorded SHA-256
`4d7ab0e86d463a4d80857aa0301e2a9758a9e42e921c7100f14a56e5ebc4d1ee`,
but that PE no longer exists in the workspace and the later file at the same
path has another hash. P25's timeout remains historical evidence only; it is
not a reproducible current-product baseline.

## Immutable current product

Fresh CPU40 formal Ninja products were built from the current source in
separate x86 and x64 roots:

- `build/M0-T318/S2/cpu40-v7vga-r7-fixed-x86`;
- `build/M0-T318/S2/cpu40-v7vga-r7-fixed-x64`.

Both linked `original-softpc-process.exe`. The x86 product was then staged,
without overwrite, by `Stage-OriginalSoftpcRuntime.mjs` into
`build/M0-T318/S2/runtime-r30`. Its immutable manifest identifies the product
as SHA-256
`94a70b3997c7cb3e04bdc4b586881ee981a3263dd0e951978d5b1c954b24994f`
and records all DOS and firmware media identities.

## Fixed observation

The unchanged non-debug, real-console observer ran the staged x86 product
with the admitted `-f -o --ordinary-child` contract and eight-second limit.
It exited before the timeout:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

The observer's JSON records the stage-manifest hash
`9c94ed36c9bdc4bc785d88bf39d86a9cfc047f46fdbc90fbd6bd0e80152eb950`.
From this point, `runtime-r30` is the sole formal runtime container: media,
firmware, observer, console ownership, arguments and timeout remain fixed;
only a newly formal, hash-identified product may later replace its EXE under a
separate evidence record.

## Disposition

The stable current observation now ends before a claimed NTIO/BOP/NTDOS/EXEC
marker. P29's startup owner clusters remain the required recovery order. This
record does not attribute the access violation to one CCPU, video, console or
Base VDM operation, and it adds no BOP or guest behavior.
