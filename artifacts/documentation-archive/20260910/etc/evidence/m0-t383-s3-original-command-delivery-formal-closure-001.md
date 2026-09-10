# M0 T383 S3 — original COMMAND delivery formal closure

## Question

Does current x86 source still compile and prove the original first-DOS-command
record contract selected by S1/S2?

## Procedure

1. Generated an isolated Ninja graph with the repository's Node 22 generator
   at `build/M0-T383/S3/declared-dos-record-x86`.
2. Recompiled and linked the current x86 sources for `base_vdm_client`,
   `base_vdm_local`, `base_vdm_broker`, `launch_declaration`, session mapping
   support and the existing Base VDM fixture.
3. Ran Ninja target `test`.
4. Ran the formal CPU40/x86 product target
   `original-softpc-process.exe` in
   `build/M0-T371/S2/formal-x86`; Ninja reported no stale inputs.

## Result

The focused executable returned zero and printed:

```
PASS: local Base VDM broker contract
```

It proves the source-shaped cohort as one contract:

- app serialization retains `/C <command>\r\n\0`, selected
  `system32\\COMMAND.COM`, `COMSPEC`/`PATH` double-NUL environment and root
  directory;
- a first COMMAND request with `ASKING_FOR_FIRST_COMMAND` and no host
  environment buffer consumes the copied DOS record exactly once;
- capacity failure reports required sizes without consumption;
- first-environment and DOS/WOW record selection remain distinct;
- no-record wait/retry and one-shot terminal directions remain explicit; and
- no guest pointer, native handle or app-side executor enters `VDMINFO`.

## Interpretation

S1's complete original COMMAND/BaseClient/local-BaseSrv cohort is current
source, x86-compilable and locally executable.  This is a provider/caller
proof, not a guest runtime proof.  S4 owns the one fixed non-debug product
observation using the pure DOS built-in `EXIT`; its only success marker is the
first original `54:01` return.
