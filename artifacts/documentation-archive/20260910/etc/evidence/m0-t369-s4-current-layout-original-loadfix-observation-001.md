# M0 T369 S4 — Current-layout original LOADFIX observation

## Fixed inputs

Exactly one non-debug, console-owning observation ran through the rebuilt
T369 observer:

```text
node.exe tools\observation\ObserveSoftpcStartup.mjs
  --launcher build\M0-T369\S2\observer\console-startup-observer.exe
  --product build\M0-T365\S2\x86\original-softpc-process.exe
  --stage O:\ntvdm64
  --report O:\ntvdm64\observation-t369-s4.txt
  --product-command LOADFIX.COM
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t369-s4.txt.bop-return.txt
```

The observer and product hashes are respectively
`dabaf70d12a17d64ac319211a40d3cdb482d2c236aed32eb50a3ae3aad20826d` and
`eb7214703f7ec9562a84dcba37d9ed5b17c9b194cc9eeac3be7098cb34a5d43d`.
The stage manifest hash is
`5af5a7c9d9eecaeceadc325da899fd2d276fbea98557c20f5df193df23bf1077`;
the immutable media manifest hash is
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.

## Result

The observer reports the current package contract exactly:

```text
fixed-system-root=O:\ntvdm64
fixed-system-root-chars=10
image-path=O:\ntvdm64\original-softpc-process.exe
result=timeout
exit=0x53504354
```

The source-shaped return log reaches the original first `54:01` record route,
then the already recovered `54:0E` and `54:04` COMMAND services.  It also
records ordinary original DEM opens/closes and fast-read (`50:42`) requests.
It later reaches another original `54:01` call whose stage-zero continuation
is recorded but whose return is absent before the prescribed timeout.

The observation does **not** record an attributable normal `50:16` result,
guest `$Exec`, `SVC_DEMENTRYDOSAPP`, LOADFIX child entry, `$Exit`, parent
restoration or COMMAND result consumption.  Raw reports remain at the named
`O:\ntvdm64` paths; no second run is admitted.

## Disposition

This is a valid current-layout replacement for the stale T367 observer
result, but it still does not prove DOS `.COM` or MZ execution.  The exact
runtime terminal is the second source-owned original COMMAND/Base VDM
`54:01` pending continuation after the complete current layout/startup path.
It does not authorize a trace-selected BOP or fast-read repair.  The next
package resumes the queue's owner-package order; cross-family execution
reconciliation retains the unresolved DOS/COMMAND continuation evidence.
