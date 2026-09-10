# M0 T365 S4 — Frozen `.COM` workload observation

## Fixed invocation

One console-owning, non-debug observation used exactly:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T356/S4/observation/console_startup_observer.exe \
  --product build/M0-T365/S2/x86/original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t365-s4.txt \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t365-s4.txt.bop-return.txt
```

The observer returned zero after its defined eight-second controlled timeout
`0x53504354`; it reported no product exception.  The fixed stage manifest hash
was `d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`,
the immutable-media hash was
`43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`, and
the staged product hash was
`58dd02f970c4faf7bb4799dc5b1eb6f9642927f6f3da5648fb4b1fbb2bbddf74`.

The raw observer sidecars remain at:

- `O:\ntvdm64\observation-t365-s4.txt`;
- `O:\ntvdm64\observation-t365-s4.txt.json`; and
- `O:\ntvdm64\observation-t365-s4.txt.bop-return.txt`.

## Observation

The child reached the existing original COMMAND bootstrap and returned the
first `54:01` command record.  The record then entered the original `54:0E`
and `54:04` continuations and later reached another original `54:01`:

```text
MVDM-BOP-RETURN 54:01 ... ax=0002 cf=0
...
MVDM-BOP-DISPATCH 54:01
MVDM-CMD-CONT svc=01 stage=0 ... first=0 repeat=0 dos-state=00000002
```

No return follows that later `54:01` before the controlled timeout.  There is
no `$Exec` entry, no `.COM` JFN/SFT lifecycle, no `DEMREAD` completion and no
guest parent return in this one run.

## Source-owned disposition

The immutable stage still supplies the existing bootstrap `/C EXIT` declaration
that T364 proved is consumed by original COMMAND.  After it is consumed, the
later `54:01` is the already-proven original BaseClient-shaped no-next-command
wait.  It is therefore an earlier command-input terminal, not evidence that
the T365 file-lifecycle binding failed.

S4 does not authorize a second run, a new command parser, a synthetic BOP
result, guest-media mutation or a trace-derived DEM repair.  A later owner
package must provide a source-shaped, declared DOS program record before the
original guest `$Exec` route can be observed.
