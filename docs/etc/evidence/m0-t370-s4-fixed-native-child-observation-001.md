# M0 T370 S4 — fixed native-child observation

## Fixed invocation

Exactly one non-debug, console-owning observation ran:

```text
node.exe tools/observation/ObserveSoftpcStartup.mjs
  --launcher build/M0-T369/S2/observer/console-startup-observer.exe
  --product build/M0-T370/S3/x86/original-softpc-process.exe
  --stage O:\ntvdm64
  --report O:\ntvdm64\observation-t370-s4.txt
  --product-command LOADFIX.COM
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t370-s4.txt.bop-return.txt
```

The immutable stage-manifest hash is
`5af5a7c9d9eecaeceadc325da899fd2d276fbea98557c20f5df193df23bf1077`;
its immutable media-manifest hash is
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.
The current formal product copied into the stage has SHA-256
`a4a3488bee18c04830ba25862a2ca33947858371cd6fa2c77fc5805bee760e9f`.

## Result

The observer completed normally and classified the child as its prescribed
eight-second timeout (`0x53504354`).  The staged root remained `O:\ntvdm64`
(10 characters).  The child-only report reaches the original bootstrap and
COMMAND path through `54:01`, `54:0E` and `54:04`, then later reaches a second
original `54:01` record continuation.

It does not reach `54:08`, `54:0A`, `54:0B`, a host-native child, normal DOS
`$Exec`, child entry, `$Exit`, or PSP/parent restoration before that terminal.
The first terminal relevant to this package is therefore an earlier original
COMMAND/Base VDM continuation, not a failure of the recovered native-child
cohort.

## Disposition

T370 proves the local source-shaped native-child cohort at source, focused
fixture and formal-link levels.  It does not claim native reachability from
this current workload.  No trace-selected repair is admitted; the retained
earlier COMMAND/Base VDM continuation belongs to the later cross-family
execution/control-broker package.
