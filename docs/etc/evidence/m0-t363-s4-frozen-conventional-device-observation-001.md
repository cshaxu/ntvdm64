# M0 T363 S4 — Frozen conventional-device workload observation

## Fixed invocation

One non-debug, console-owning invocation used the existing fixed stage
`O:\\ntvdm64`, the unchanged CPU40/x86 product
`build/M0-T362/S2/x86/original-softpc-process.exe`, the existing x86 observer,
and the original eight-second timeout. `ObserveSoftpcStartup.mjs` verified all
immutable media hashes before copying only the unchanged product into the
stage. The staged product SHA-256 was
`e2a8d705159738b5900ea197b8d7aed64bbac178102e98d2954eabdcc01730f2`.

The observer completed normally and reported a controlled timeout
`0x53504354`; no retry is permitted.

## Result

The source-order child record proves the earlier original conventional-device
path did not prevent progress. It records a complete first `54:01` command
retrieval return:

```text
MVDM-CMD-CONT svc=01 stage=0 ... first=1 repeat=0 dos-state=00000001
MVDM-CMD-CONT svc=01 stage=1 ... first=0 repeat=0 dos-state=00000002
MVDM-BOP-RETURN 54:01 ... ax=0002 cf=0
```

It then reaches and returns from `54:0E` and `54:04`, continues through
original DEM activity, and reaches a later `54:01` continuation:

```text
MVDM-CMD-CONT svc=01 stage=0 ... first=0 repeat=0 dos-state=00000002
```

The eight-second observer ends before that later call returns. This is not a
printer failure, a claim that `/C EXIT` completed, or evidence that DOS child
execution works. It is a source-owned transfer to the original COMMAND
command-delivery/continuation cohort.

## T363 conclusion

The original CPU40 conventional-device cohort has source ownership, selected
formal linkage, and a workload observation that progresses beyond it. T363
therefore closes without a printer shim. The next package is COMMAND command
delivery and built-in dispatch, before the downstream DOS EXEC package.
